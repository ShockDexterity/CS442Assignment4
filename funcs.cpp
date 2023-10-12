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

vector<string> getCommandsFromFile(const string& filename)
{
    vector<string> commands {};

    ifstream infile {};
    infile.open(filename);
    if (infile.fail())
    {
        throw std::runtime_error("Failed to open " + filename + '\n');
    }

    string command { "" };
    while (getline(infile, command))
    {
        commands.emplace_back(command);
    }

    infile.close();
    return commands;
}

vector<vector<string>> tokenize(const vector<string>& tokenStrings)
{
    vector<vector<string>> tokenizedStrings {};

    for (const string& str : tokenStrings)
    {
        vector<string> v_tokens {};
        stringstream stream { str };
        string token { "" };
        while (getline(stream, token, ' '))
        {
            v_tokens.emplace_back(token);
        }
        tokenizedStrings.emplace_back(v_tokens);
    }

    return tokenizedStrings;
}

void parse(const vector<vector<string>>& tokenizedStrings)
{
    vector<string> parseResults {};
    for (const vector<string>& tokenizedCommand : tokenizedStrings)
    {
        string res { "" };
        int pipeIndex { findToken(tokenizedCommand, "|") };
        int leftAngleIndex { findToken(tokenizedCommand, "<") };
        int rightAngleIndex { findToken(tokenizedCommand, ">") };

        if (pipeIndex >= 0)
        {
            res += "| found at position " + std::to_string(pipeIndex);
        }
        else
        {
            res += "No |";
        }

        res += ", ";

        if (leftAngleIndex >= 0)
        {
            res += "< found at position " + std::to_string(leftAngleIndex);
        }
        else
        {
            res += "No <";
        }

        res += ", ";

        if (rightAngleIndex >= 0)
        {
            res += "> found at position " + std::to_string(rightAngleIndex);
        }
        else
        {
            res += "No >";
        }

        parseResults.emplace_back(res);
    }
}

int findToken(const vector<string>& haystack, const string& token)
{
    for (unsigned int i { 0 }; i < haystack.size(); ++i)
    {
        if (haystack.at(i) == token)
        {
            return i;
        }
    }
    return -1;
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
    //string filePath = "output.txt";
//     int outFd = open(filePath.c_str(), O_WRONLY | O_CREAT, 0666);

    //Changing the output image to the file
    // dup2(outFd, 1);
    if (execvp(cStringArr[0], cStringArr) < 0)
    {
        cout << "Couldn't change execution image" << endl;
    }
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
