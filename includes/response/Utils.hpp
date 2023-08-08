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
#include <sys/types.h>
#include <sys/stat.h>

typedef struct Header {
	std::string key;
	std::string value;
} t_header;

typedef struct ResponseHeader {
    int statusCode;
    std::string statusMessage;
    std::map<std::string, std::string> m_headers;
    std::vector<Header> v_headers;

} t_responseHeader;


class Utils
{
	private:
		Utils();
	public:
		static size_t counter;
		static std::string toString(int value);
		static std::string getWebservRootPath();
		static std::string getStatusMessage(int statusCode);
		static std::string ResponseHeaderToString(const t_responseHeader &responseHeader);
		static std::string getExtensionFile(const std::string &path);
		static std::string uriDecode(const std::string &uri);
		static std::string generateFileName();
		static bool isDirectory(const std::string& path);
		static bool fileExists(const std::string& path);
		static bool isExecutable(const std::string& path);
		static bool	isValidURI(const std::string& uri);
};

#endif