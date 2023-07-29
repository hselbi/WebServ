#pragma once

#include "Utils.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include "Utils.hpp"
#include "../config/ConfLoca.hpp"

# define RES_BUFFER_SIZE 64000

struct Client;

class Response {
	public:
		Response();
		~Response();
		Response(const Response &copy);

		void				processing();
		void				setClient(Client &client);
		std::string				getRoot();
		
	private:
		Client				*_client;
		ConfLoca 			*_location;	
		std::ifstream		_file;
		int					_cgi_file;
		std::string			_header_buffer;
		char	 			_buffer[RES_BUFFER_SIZE];
		bool				_have_cgi;

		std::string				getContentType(const std::string& filePath);
		std::string				getErrorPagePath(int statusCode);
		std::string				isDirHasIndexFiles();
		void					readFile();
		void					readCgiFile();
		void					readFileByPath(std::string filePath);
		void					autoIndex();
		void					errorPages(int statusCode);
		void 					checkWhichRequestedMethod();
		void					Method_GET();
		void					Method_DELETE();
		void					setRediration(std::string location);
		bool					checkRequestIsFormed();
		bool					isLocationHaveRedirection();
		bool					getMatchedLocation();
		bool					isMethodAllowedInLocation();
		bool 					getAutoIndex();
		void					deleteFile();
		void 					deleteAllFolderFiles();
		void					processingCgi();
		
};
