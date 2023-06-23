#pragma once

#include <iostream>
#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <utility>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <cstring>
#include <sstream>

#include <map>
#include <algorithm>
#include <vector>
#include "Client.hpp"
#include "utils.hpp"

#include "../config/Config.hpp"
#include "../config/ConfServer.hpp"

#define QUEUE_LIMIT 1000
#define REQUEST_END "\r\n\r\n"
#define REQUEST_DELIMETER "\r\n"

#define DEFAULT_CONFIG_FILE "../assets/server.conf"
#define SERVER_BLOCK_COUNT 1
// 4096 kb = 4 mb = 4096 * 1024 bytes = 4194304 bytes
#define BUFFER_SIZE 4194304 // 4 mb

struct Client;

struct Server
{
	typedef std::map<long, Client *> Clients;

	Server();
	~Server();

	void load_config_file(char *config_file);
	void start(); // configure server and setup the sockets for each server block // Starts all servers in the cluster.
	void stop();
	void setup_server();
	void start_server();
	void restart_server(std::string message);
	void cleanup_by_closing_all_sockets();

	void create_server_socket();
	void bind_socket(long server_socket, std::string host, int port);
	void listen_on_socket(long server_socket);
	// void init_socket_pool(long server_socket);
	void zero_socket_pool();
	long monitor_clients(); //  wait until either a client has data available or a new client is attempting to connect.
	void accept_new_connection(long server_socket);
	void handle_incoming_request(long client_socket);
	void handle_outgoing_response(long client_socket);
	void feed_request(std::string request, long client_socket); // feed request to the Request class
	void build_response(Request &request, long client_socket);	// generate a response
	void send_response(long client_socket);
	std::string get_http_method(std::string &request);
	bool is_request_completed(std::string &request, long client_socket);
	void close_all_sockets();
	Client *create_client();
	void drop_client(long client_id);
	bool is_connection_close(std::string &request);
	void throw_error(std::string error_message);

	// getters and setters
	Client *get_client(long client_id);
	void match_client_request_to_server_block(long client_socket);

	std::vector<long> &get_server_sockets();

private:
	Config _config;
	std::vector<ConfServer> *_server_blocks;
	long _server_count;
	std::vector<std::string> _hosts;
	std::vector<int> _ports;
	Clients _clients;
	std::vector<long> _server_sockets;
	long _biggest_socket;
	// std::string _host;
	// std::string _port;
	// std::string _server_name;
	fd_set _socket_pool;
	fd_set _write_set_pool;
	fd_set _read_set; // sets of file descriptors to monitor for read
	fd_set _write_set;
	struct sockaddr_in _server_addr, _client_addr;

	std::ofstream _outputFile;

	// Request request;
	// Response response;
};

//  http header = method + path + version + headers + body
// http request = header + body
// http response = header + body
// http headers = metada about the request or response (key-value pairs) + body (optional) + status code (optional)
// http hedaeer = information about the request or response (key-value pairs)
