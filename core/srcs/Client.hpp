#pragma once

#include <iostream>
#include <vector>


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
	void append_response_data(const char *response_buffer);
	void append_total_bytes_received(long bytes_read);
	void append_total_bytes_sent(long bytes_sent);

	void reset_request_data();
	void reset_response_data();
	void reset_total_bytes_received();
	void reset_total_bytes_sent();


	void set_request_body_length(long request_body_size);
	void set_request_body(std::string request_body);
	private:
		long _client_socket; // client socket
		std::string _request; // clear the request buffer for the next request
		std::string _request_body;
		long _request_body_size;
		std::string _response; // clear the response buffer for the next response
		long _total_payload_received; //   _total_payload_received number of bytes stored
		long _total_payload_sent; //   _total_payload_received number of bytes stored
};

