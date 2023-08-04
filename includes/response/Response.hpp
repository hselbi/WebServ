#pragma once

#include "Utils.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include "Utils.hpp"
#include "../config/ConfLoca.hpp"
#include <sys/types.h>
#include <sys/wait.h>

# define RES_BUFFER_SIZE 10000

struct Client;

class Response {
	public:
		Response();
		~Response();
		Response(const Response &copy);

		void				processing();
		void				setClient(Client &client);
		std::string			getRoot();
		ConfLoca			*getLocation();
		bool get_cgi_status();
		void set_cgi_status(bool status);



	private:
		Client				*_client;
		ConfLoca 			*_location;
		std::ifstream		_file;
		std::map<std::string, std::ifstream>		_files;

		std::string			_header_buffer;
		std::string			_cgi_file_path;
		char	 			_buffer[RES_BUFFER_SIZE];
		bool				_have_cgi;

		t_header				setHeaderElement(std::string key, std::string value);
		std::string				getContentType(const std::string& filePath);
		std::string				startCgi(std::string script_path);
		std::string				getErrorPagePath(int statusCode);
		std::string				isDirHasIndexFiles();
		std::string				getRequestPathFile();
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
		bool					parseCgiHeader(std::string header, int contentLength, int delimiterLength);
		bool					checkRequestIsFormed();
		bool					isServerHaveRedirection();
		bool					getMatchedLocation();
		bool					isMethodAllowedInLocation();
		bool 					getAutoIndex();
		void					deleteFile();
		void 					deleteAllFolderFiles();
		void					setResStatus(int status);

};
