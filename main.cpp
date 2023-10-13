/*
g++ *.cpp -o output.exe && ./output.exe
*/

/******************************************
 AUTHORS: William Canter and Rowan Verdouw
******************************************/

#include "funcs.h"

const string command_output { "command_output.txt" };

int main()
{
	// at beginning of session, put the data from the file back into the queue
	int numCommands { 1 };
	queue<string> historyQueue {};
	string tempString { "" };

	fstream histFile("history.txt", ios::in | ios::out | ios::app);

	if (!histFile)
	{
		cout << "Error with file" << endl;
	}

	while (getline(histFile, tempString))
	{
		historyQueue.emplace(tempString);
		numCommands++;
	}

	while (true)
	{
		int numTokens { 0 };
		vector<string> commandTokens { getCommand(numTokens, historyQueue) };

		if (lowercase(commandTokens.at(0)) == "exit")
		{
			//saving data from the queue into the history file
			histFile.clear();
			histFile.seekg(0, ios::beg); // moves the file pointer to the front of the file
			while (!historyQueue.empty())
			{
				if (!isdigit(historyQueue.front()[0])) // checking if the row already has a line number
				{
					histFile << numCommands << ' ' << historyQueue.front() << endl;
					numCommands++;
				}
				historyQueue.pop();
			}
			cout << "Exiting shell" << '\n';
			histFile.close();
			exit(0);
		}

		// Does the command contain a | ?
		bool hasPipe { findToken(commandTokens, "|") };
		// Does the command contain a < ?
		bool hasInput { findToken(commandTokens, "<") };
		// Does the command contain a > ?
		bool hasOutput { findToken(commandTokens, ">") };

		if (tripleCompare(hasPipe, hasInput, hasOutput))
		{
			cout << "Please enter only a |, a <, a >, or nothing, this shell cannot handle multiple options." << '\n';
			continue;
		}

		char* commandString[numTokens + 1];
		commandString[numTokens] = NULL;
		fill(commandString, commandTokens);

		int fd[2] {}; // [0] for reading, [1] for writing
		if (pipe(fd) == -1)
		{
			cout << "Failed to make a pipe :(" << '\n';
			exit(1);
		}

		int orgSTDIN { dup(0) };
		int orgSTDOUT { dup(1) };

		pid_t pid { fork() };

		if (pid < 0)
		{
			cout << "Failed to make a child." << '\n';
			exit(1);
		}
		else if (pid == 0) // must be in child process
		{
			if (contains(commandString[0], "pwd"))
			{
				char path[256] {};

				if (getcwd(path, sizeof(path)) != NULL)
				{
					printf("%s\n", path);
				}
			}
			else if (contains(commandString[0], "history"))
			{
				// possible send output to wc just in case the command is history | wc
				bool wc { false };
				if (hasPipe && commandTokens.at(2) == "wc")
				{
					wc = true;

					int fd { open(command_output.c_str(), O_TRUNC | O_WRONLY) };
					if (fd < 0)
					{
						cout << "Failed to open file for piping" << '\n';
						exit(1);
					}

					int orgSTDOUT { dup(1) };
					dup2(fd, 1);
				}

				// printing data from the queue to the screen for the user to view
				string copy { "" };
				for (int i = 0; i < historyQueue.size(); i++)
				{
					cout << numCommands << ' ' << historyQueue.front() << endl;
					copy = historyQueue.front();
					historyQueue.pop();
					historyQueue.emplace(copy);
					copy.clear();
				}

				if (wc)
				{
					// change output back to stdout
					fflush(stdout);
					dup2(orgSTDOUT, 1);
					fflush(stdout);

					executeCommandWithInput(vector<string>{
						"wc", "<", "command_output.txt"
					});
				}
			}
			else if (hasPipe)
			{
				executePipedCommand(commandTokens, command_output, hasInput);
			}
			else if (hasInput)
			{
				executeCommandWithInput(commandTokens);
			}
			else if (hasOutput)
			{
				executeCommandWithOutput(commandTokens);
			}
			else
			{
				executeUserCommand(commandString);
			}

			exit(0);
		}
		else // must be in parent process
		{
			wait(0);

			if (hasInput)
			{
				// change input back to stdin
				fflush(stdin);
				dup2(orgSTDIN, 0);
				fflush(stdin);
			}
			else if (hasOutput)
			{
				// change output back to stdout
				fflush(stdout);
				dup2(orgSTDOUT, 1);
				fflush(stdout);
			}
		}
	}

	return 0;
}