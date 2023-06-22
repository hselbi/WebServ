#ifndef INCLUDES_HPP
#define INCLUDES_HPP


#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <list>
#include <sstream>
# include <utility>
#include <ctime>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <algorithm>
#include <cctype>
#include <iomanip>
 
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

#include <fstream>

#include <unistd.h>



enum MethodType
{
	GET,
	POST,
	DELETE,
	INVALID,
};

#endif