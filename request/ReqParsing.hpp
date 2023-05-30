#ifndef REQPARSING_HPP
#define REQPARSING_HPP


#include <iostream>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <cstdio>
#include <vector>

class ReqParsing
{
private:
    std::map<std::string, std::string> req;
    std::string header;
    std::string body;
public:
    ReqParsing(const char *r);
    // void headerBodySpliter();
    void printReq();
    void startLine(std::string line);
    void headers(std::string line);
    void tokenize(std::string &str, char *delim, std::vector<std::string> &out);
    // ~ReqParsing();
};



#endif