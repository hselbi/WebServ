#ifndef PARSING_HPP
#define PARSING_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <stdexcept>

class Parsing
{
    private:
        std::string _path;
        std::vector<std::string> content;
    public:
        void ReadFile(char *path);
        void checkBrackets(std::vector<std::string> content);
};

#endif // PARSING_HPP
