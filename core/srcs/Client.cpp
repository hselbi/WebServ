#include "../../includes/core/Client.hpp"
#include "../../includes/request/Request.hpp"

Client::Client() : _request_body_size(0), _res_status(NOT_STARTED) {}

// Client::Client(long client_socket) { }
Client::~Client() {}



std::string &Client::get_request_data() { return _request_buffer; }

void Client::append_request_data(char *request_buffer, long read_bytes)
{
	_request_buffer.append(request_buffer, read_bytes);
}

std::string &Client::get_response_data() { return _response_buffer; }

void Client::append_response_data(std::string &response_buffer) {
		_response_buffer.append(response_buffer); 
	}

long &Client::get_total_bytes_sent() { return _total_payload_sent; }

void Client::append_total_bytes_sent(long bytes_sent) { _total_payload_sent += bytes_sent; }

void Client::reset_total_bytes_sent() { _total_payload_sent = 0; }

void Client::reset_request_data()
{
	_request_buffer.clear();
}
void Client::reset_response_data()
{
	_response_buffer.clear();
}

std::string &Client::get_request_body() { return _request_body; }

void Client::set_request_body(std::string request_body) { _request_body = request_body; }

long &Client::get_request_body_length() { return _request_body_size; }

void Client::set_request_body_length(long request_body_size) { _request_body_size = request_body_size; }

Request &Client::get_request() { return _request; }

Response &Client::get_response() { return _response; }

Cgi &Client::get_cgi() { return _cgi; }

int Client::get_res_status() { return _res_status; }

void Client::set_res_status(int status) { _res_status = status; }

void Client::set_server_block(ConfServer &config) { _config = config; }

ConfServer &Client::get_server_block() { return _config; }

long Client::get_server_socket() { return _server_socket; }

void Client::set_server_socket(long server_socket) { _server_socket = server_socket; }
