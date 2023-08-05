#pragma once

#include <iostream>
#include <vector>
#include "../request/Request.hpp"
#include "../response/Response.hpp"
#include "../core/Cgi.hpp"
#include "../config/ConfServer.hpp"

enum E_RESPONSE_STATUS
{
	NOT_STARTED,
	ON_PROCESS,
	DONE
};

enum E_CGI_RESPONSE_STATUS
{
	CGI_NOT_STARTED,
	CGI_ON_PROCESS,
	CGI_DONE
};

class Request;
class Response;
class Cgi;

struct Client
{
	Client();
	~Client();

	std::string &get_request_data();
	std::string &get_response_data();
	long &get_total_bytes_received();

	void append_request_data(char *request_buffer, long read_bytes);
	void append_response_data(std::string &response_buffer);

	void reset_request_data();
	void reset_response_data();

	Request &get_request();
	Response &get_response();
	Cgi &get_cgi();

	int get_res_status();
	void set_res_status(int status);

	void set_server_block(ConfServer &config);
	ConfServer &get_server_block();

	long get_server_socket();
	void set_server_socket(long server_socket);

private:
	long _server_socket;
	int _res_status;
	ConfServer _config;
	Request _request;
	Response _response;
	Cgi _cgi;
	std::string _response_buffer;
	std::string _request_buffer;
};
