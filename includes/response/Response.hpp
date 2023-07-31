#pragma once

#include "Utils.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include "Utils.hpp"
#include "../config/ConfLoca.hpp"
#include <sys/types.h>
#include <sys/wait.h>

# define RES_BUFFER_SIZE 64000
# define TIMEOUT_CGI 6

struct Client;

class Response {
	public:
		Response();
		~Response();
		Response(const Response &copy);

		void				processing();
		void				setClient(Client &client);
		std::string				getRoot();
		bool get_cgi_status();
		void set_cgi_status(bool status);

		int 				get_cgi_file();
		void 				set_cgi_file(int fd);

	private:
		Client				*_client;
		ConfLoca 			*_location;
		std::ifstream		_file;
		std::map<std::string, std::ifstream>		_files;
		int					_cgi_file;

		std::string			_header_buffer;
		char	 			_buffer[RES_BUFFER_SIZE];
		bool				_have_cgi;

		std::string				getContentType(const std::string& filePath);
		std::string				getErrorPagePath(int statusCode);
		std::string				isDirHasIndexFiles();
		std::string				getRequestPath();
		std::string				tmp_getRequestPath();
		void					readFile();
		void					readCgiFile();
		void					readFileByPath(std::string filePath);
		void					autoIndex();
		void					errorPages(int statusCode);
		void 					checkWhichRequestedMethod();
		void					Method_GET();
		void					Method_POST();
		void					Method_DELETE();
		void					setRediration(std::string location);
		bool					checkRequestIsFormed();
		bool					isServerHaveRedirection();
		bool					getMatchedLocation();
		bool					isMethodAllowedInLocation();
		bool 					getAutoIndex();
		void					deleteFile();
		void 					deleteAllFolderFiles();
		void					processingCgi();
		void					setResStatus(int status);
};
