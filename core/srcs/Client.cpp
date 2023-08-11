#include "../../includes/core/Client.hpp"
#include "../../includes/request/Request.hpp"

Client::Client() :  _res_status(NOT_STARTED) {}

Client::~Client() {}

std::string &Client::get_request_data() { return _request_buffer; }

void Client::append_request_data(char *request_buffer, long read_bytes) { _request_buffer.append(request_buffer, read_bytes); }

std::string &Client::get_response_data() { return _response_buffer; }

void Client::append_response_data(std::string &response_buffer) { _response_buffer.append(response_buffer); }

void Client::reset_request_data() { _request_buffer.clear(); }

void Client::reset_response_data() { _response_buffer.clear(); }

int Client::get_res_status() { return _res_status; }

void Client::set_res_status(int status) { _res_status = status; }

void Client::set_server_block(ConfServer &config) { _config = config; }

ConfServer &Client::get_server_block() { return _config; }

long Client::get_server_socket() { return _server_socket; }

void Client::set_server_socket(long server_socket) { _server_socket = server_socket; }

Request &Client::get_request() { return _request; }

Response &Client::get_response() { return _response; }

Cgi &Client::get_cgi() { return _cgi; }
