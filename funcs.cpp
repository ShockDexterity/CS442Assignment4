#include "funcs.h"

vector<string> getCommand(int& numTokens, queue<string>& histQ)
{
    string input { "" };
    cout << "Enter a command: ";
    getline(cin, input);

    histQ.emplace(input);

    stringstream stream { input };
    string token { "" };
    vector<string> tokens {};
    while (getline(stream, token, ' '))
    {
        tokens.emplace_back(token);
        ++numTokens;
    }
    return tokens;
}

bool findToken(const vector<string>& tokens, const string& key)
{
    for (const string& token : tokens)
    {
        if (token == key)
        {
            return true;
        }
    }
    return false;
}

void fill(char** container, const vector<string>& stuff)
{
    for (int i { 0 }; i < stuff.size(); ++i)
    {
        container[i] = (char*)stuff.at(i).c_str();
    }
}

void executeUserCommand(char** cStringArr)
{
    if (execvp(cStringArr[0], cStringArr) < 0)
    {
        cout << "Couldn't change execution image" << endl;
    }
}

void executePipedCommand(const vector<string>& tokenizedCommand, const string& output_file, bool& hasInput)
{
    int fd { open(output_file.c_str(), O_TRUNC | O_WRONLY) };
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
        exit(1);
    }
    else if (pid == 0) // grandchild
    {
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
            hasInput = true;
            postPipe.emplace_back("<");
            postPipe.emplace_back(output_file);
            executeCommandWithInput(postPipe);
        }
        else
        {
            int size { static_cast<int>(postPipe.size()) };
            char* comString[size + 1] {};
            comString[size] = NULL;
            fill(comString, postPipe);
            executeUserCommand(comString);
        }
    }
}

void executeCommandWithInput(const vector<string>& tokenizedCommand)
{
    vector<string> command {};
    string file { "" };
    bool foundInput { false };
    for (const string& token : tokenizedCommand)
    {
        if (!foundInput && token == "<") // found the <
        {
            foundInput = true;
        }
        else if (!foundInput) // command before the <
        {
            command.emplace_back(token);
        }
        else // file after the <
        {
            file = token;
        }
    }

    int fd { open(file.c_str(), O_RDONLY) };
    if (fd < 0)
    {
        cout << "Failed to open file for input" << '\n';
        exit(1);
    }

    int orgSTDIN { dup(0) };
    dup2(fd, 0);

    int size { static_cast<int>(command.size()) };
    char* comString[size + 1] {};
    comString[size] = NULL;
    fill(comString, command);
    executeUserCommand(comString);
}

void executeCommandWithOutput(const vector<string>& tokenizedCommand)
{
    vector<string> command {};
    string file { "" };
    bool foundOutput { false };
    for (const string& token : tokenizedCommand)
    {
        if (!foundOutput && token == ">") // found the >
        {
            foundOutput = true;
        }
        else if (!foundOutput) // command before the >
        {
            command.emplace_back(token);
        }
        else // file after the >
        {
            file = token;
        }
    }

    int fd { open(file.c_str(), O_CREAT | O_TRUNC | O_WRONLY) };
    if (fd < 0)
    {
        cout << "Failed to open file for input" << '\n';
        exit(1);
    }

    int orgSTDOUT { dup(1) };
    dup2(fd, 1);

    int size { static_cast<int>(command.size()) };
    char* comString[size + 1] {};
    comString[size] = NULL;
    fill(comString, command);
    executeUserCommand(comString);
}

bool contains(const string& haystack, const string& needle)
{
    if (haystack.find(needle) != string::npos)
    {
        return true;
    }
    return false;
}

bool contains(const char* haystack, const char* needle)
{
    if (strstr(haystack, needle) != NULL)
    {
        return true;
    }
    return false;
}

string lowercase(const string& ref)
{
    string res { "" };
    for (const char& c : ref)
    {
        res += static_cast<char>(tolower(c));
    }
    return res;
}

bool tripleCompare(bool a, bool b, bool c)
{
    return ((a && b) || (a && c) || (b && c));
}

void print(const vector<vector<string>>& vec)
{
    cout << "{\n";
    bool b { false };
    for (const vector<string>& subv : vec)
    {
        if (b)
        {
            cout << ",\n";
        }
        print(subv);
        b = true;
    }
    cout << "\n}\n";
}

void print(const vector<string>& vec)
{
    cout << "{ ";
    bool b { false };
    for (const string& str : vec)
    {
        if (b)
        {
            cout << ", ";
        }
        cout << str;
        b = true;
    }
    cout << " }\n";
}

void print(char** arr, int size)
{
    for (int i { 0 }; i < size; i++)
    {
        printf("%s\n", arr[i]);
    }
}
