#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <queue>

using namespace std;

vector<string> getCommand(int&, queue<string>&);
vector<string> getCommandsFromFile(const string&);
vector<vector<string>> tokenize(const vector<string>&);
void parse(const vector<vector<string>>&);
int findToken(const vector<string>&, const string&);
void fill(char**, const vector<string>&);
void executeUserCommand(char** cStringArr);

bool contains(const string&, const string&);
bool contains(const char*, const char*);

string lowercase(const string&);

void print(const vector<vector<string>>&);
void print(const vector<string>&);
void print(char**, int);
