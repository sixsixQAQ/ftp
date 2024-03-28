#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
constexpr int COMMAND_LINE_MAX_SIZE = 8192;

vector<string> split(const string &str)
{
    vector<string> tokens;
    stringstream stream(str);

    string token;
    while (stream >> token)
    {
        tokens.emplace_back(token);
    }
    return tokens;
}

int main()
{
    string command;

    vector<string> args;

    std::cout << "ftp>";
    for (;;)
    {
        getline(std::cin, command);
        vector<string> tokens = split(command);
    }

}