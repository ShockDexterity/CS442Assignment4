/*
g++ *.cpp -o output.exe && ./output.exe
*/

#include "funcs.h"

int main()
{
	while (true)
	{
		// printf("Hello %s\n", "World!");
		// printf("Amogus\n");

		// string command;
		// getline(cin, command);

		// if (command == "exit")
		// {
		// 	break;
		// }

		int size { 0 };
		vector<string> commandTokens { getCommand(size) };
		char* commandString[size + 1];
		//char* commandString[size + 1];
		commandString[size] = NULL;
		fill(commandString, commandTokens);

		int org_STDIN { dup(0) };
		int org_STDOUT { dup(1) };

		int fdChildToParent[2] {}; // [0] is reading
		int fdParentToChild[2] {}; // [1] is writing

		if (pipe(fdChildToParent) < 0)
		{
			cout << "Failed to make fdChildToParent into a pipe" << '\n';
		}
		if (pipe(fdParentToChild) < 0)
		{
			cout << "Failed to make fdParentToChild into a pipe" << '\n';
		}

		pid_t pid { fork() };

		if (pid < 0)
		{
			cout << "Failed to make a child." << '\n';
			exit(1);
		}
		else if (pid == 0)
		{
			cout << "THIS IS ";
			print(commandString, size);

			if (contains(commandString[0], "pwd"))
			{
				char path[256];

				if (getcwd(path, sizeof(path)) != NULL)
				{
					printf("%s\n", path);
				}
			}

			exit(0);
		}
		else
		{
			wait(0);
		}
	}

	return 0;
}