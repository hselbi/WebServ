#ifndef UTILS_HPP
# define UTILS_HPP
#include <string>
#include <iostream>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <map>
#include <vector>
#include <dirent.h>


typedef struct ResponseHeader {
    int statusCode;
    std::string statusMessage;
    std::map<std::string, std::string> headers;
} t_responseHeader;


class Utils
{
	public:
		static std::string toString(int value);
		static std::string getWebservRootPath();
		static std::string getStatusMessage(int statusCode);
		static std::string ResponseHeaderToString(const t_responseHeader &responseHeader);
		static bool isValidURI(const std::string& uri);
		static bool isDirectory(const std::string& path);
		static bool fileExists(const std::string& path);
};

#endif