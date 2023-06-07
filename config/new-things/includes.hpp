#ifndef INCLUDES_HPP
#define INCLUDES_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <sstream>
#include <ctime>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
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