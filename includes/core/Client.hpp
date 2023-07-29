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

class Request;
class Response;
class Cgi;

struct Client
{
	Client(); // init the client object
	~Client();

	std::string &get_request_data();
	std::string &get_response_data();
	std::string &get_request_body();
	long &get_request_body_length();
	long &get_total_bytes_received();
	long &get_total_bytes_sent();

	void append_request_data(char *request_buffer, long read_bytes);
	void append_response_data(std::string &response_buffer);
	// void append_total_bytes_received(long bytes_read);
	void append_total_bytes_sent(long bytes_sent);

	void reset_request_data();
	void reset_response_data();
	// void reset_total_bytes_received();
	void reset_total_bytes_sent();

	void set_request_body_length(long request_body_size);
	void set_request_body(std::string request_body);

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
	long _client_socket;		 // client socket
	std::string _request_buffer; // clear the request buffer for the next request
	std::string _request_body;
	long _request_body_size;
	// long _total_payload_received; //   _total_payload_received number of bytes stored
	long _total_payload_sent;	  //   _total_payload_received number of bytes stored
};
