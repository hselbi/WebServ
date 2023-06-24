#include "../../includes/core/Client.hpp"

Client::Client() : _total_payload_received(0), _request_body_size(0), _status(0) {}

// Client::Client(long client_socket) { }
Client::~Client() {}

long &Client::get_total_bytes_received() { return _total_payload_received; }

void Client::append_total_bytes_received(long bytes_read)
{
	_total_payload_received += bytes_read;
}

std::string &Client::get_request_data() { return _request_buffer; }

void Client::append_request_data(char *request_buffer, long read_bytes)
{
	_request_buffer.append(request_buffer, read_bytes);
}

std::string &Client::get_response_data() { return _response_buffer; }

void Client::append_response_data(std::string &response_buffer) { _response_buffer.append(response_buffer); }

long &Client::get_total_bytes_sent() { return _total_payload_sent; }

void Client::append_total_bytes_sent(long bytes_sent) { _total_payload_sent += bytes_sent; }

void Client::reset_total_bytes_sent() { _total_payload_sent = 0; }

void Client::reset_total_bytes_received() { _total_payload_received = 0; }

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

int Client::get_status() { return _status; }

void Client::set_status(int status) { _status = status; }

void Client::set_server_block(ConfServer &config) { _config = config; }

ConfServer &Client::get_server_block() { return _config; }
