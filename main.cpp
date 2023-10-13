/*
g++ *.cpp -o output.exe && ./output.exe
*/

/******************************************
 AUTHORS: William Canter and Rowan Verdouw
******************************************/

#include "funcs.h"

const string OUTPUT_FILE { "command_output.txt" };

void executePipedCommand(const vector<string>&);
void executePipedCommand(const vector<string>& tokenizedCommand)
{
	cout << "In function executePipedCommand" << '\n';

	int fd { open(OUTPUT_FILE.c_str(), O_TRUNC | O_WRONLY) };
	if (fd < 0)
	{
		cout << "Failed to open file for piping" << '\n';
		exit(1);
	}

	vector<string> prePipe {};
	vector<string> postPipe {};
	bool foundPipe { false };
	for (const string& token : tokenizedCommand)
	{
		if (!foundPipe && token == "|") // found the pipe
		{
			foundPipe = true;
		}
		else if (!foundPipe) // command before the pipe
		{
			prePipe.emplace_back(token);
		}
		else // command after the pipe
		{
			postPipe.emplace_back(token);
		}
	}

	int orgSTDOUT { dup(1) };
	dup2(fd, 1);

	pid_t pid { fork() };

	if (pid < 0) // no grandchild
	{
		// change output back to stdout
		fflush(stdout);
		dup2(orgSTDOUT, 1);
		close(fd);
		fflush(stdout);
		cout << "Failed to creat a child for piping" << '\n';
		exit(1);
	}
	else if (pid == 0) // grandchild
	{
		// todo: run the first half of the command
		int size { static_cast<int>(prePipe.size()) };
		char* comString[size + 1] {};
		comString[size] = NULL;
		fill(comString, prePipe);
		executeUserCommand(comString);
	}
	else // must be in main child
	{
		wait(0);

		// change output back to stdout
		fflush(stdout);
		dup2(orgSTDOUT, 1);
		close(fd);
		fflush(stdout);

		if (postPipe.size() == 1 && postPipe.at(0) == "wc")
		{
			postPipe.emplace_back(OUTPUT_FILE);
		}

		int size { static_cast<int>(postPipe.size()) };
		char* comString[size + 1] {};
		comString[size] = NULL;
		fill(comString, postPipe);
		executeUserCommand(comString);

	}
}

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
			histFile.clear();
			histFile.seekg(0, ios::beg);
			while (!historyQueue.empty())
			{
				if (!isdigit(historyQueue.front()[0]))
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

		int org_STDIN { dup(0) };
		int org_STDOUT { dup(1) };

		int fd[2] {}; // [0] for reading, [1] for writing
		if (pipe(fd) == -1)
		{
			cout << "Failed to make a pipe :(" << '\n';
			exit(1);
		}

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
				string copy { "" };
				for (int i = 0; i < historyQueue.size(); i++)
				{
					cout << numCommands << ' ' << historyQueue.front() << endl;
					copy = historyQueue.front();
					historyQueue.pop();
					historyQueue.emplace(copy);
					copy.clear();
				}
			}
			else if (hasPipe)
			{
				executePipedCommand(commandTokens);
			}
			else if (hasOutput)
			{
				// todo: executeCommandWithOutput();
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
			// delete[] commandString; // free the memory (it got mad at me)
		}
	}

	return 0;
}