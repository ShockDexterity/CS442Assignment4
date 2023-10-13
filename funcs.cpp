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
