/*
g++ *.cpp -o output.exe && ./output.exe
*/

#include "funcs.h"

int main()
{
	// at beginning of session, put the data from the file back into the queue
	int numCommands {1};
	queue<string> historyQueue;
	string tempString;
	fstream histFile("history.txt", ios::in | ios::out | ios::app);
	// histFile.open("history.txt");
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
			
			// TODO: print out history to file :)
			histFile.clear();
			histFile.seekg(0, ios::beg);
			while(!historyQueue.empty())
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
			cout << "THIS IS ";
			print(commandString, numTokens);

			if (contains(commandString[0], "pwd"))
			{
				char path[256];

				if (getcwd(path, sizeof(path)) != NULL)
				{
					printf("%s\n", path);
				}
			}
			else if (contains(commandString[0], "history"))
			{
				string copy;
				for (int i = 0; i < historyQueue.size(); i++)
				{
					cout << numCommands << ' ' << historyQueue.front() << endl;
					copy = historyQueue.front();
					historyQueue.pop();
					historyQueue.emplace(copy);
					copy = "";
				}
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