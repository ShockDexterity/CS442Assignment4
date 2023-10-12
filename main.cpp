/*
g++ *.cpp -o output.exe && ./output.exe
*/

#include "funcs.h"

int main()
{
	while (true)
	{
		int numTokens { 0 };
		vector<string> commandTokens { getCommand(numTokens) };

		if (lowercase(commandTokens.at(0)) == "exit")
		{
			// TODO: print out history to file :)

			cout << "Exiting shell" << '\n';
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

			executeUserCommand(commandString);



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