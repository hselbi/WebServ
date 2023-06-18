#include "../../includes/core/Server.hpp"

Server::Server() : _biggest_socket(0), _server_count(1), _config_file(DEFAULT_CONFIG_FILE) { }

Server::~Server()
{
	cleanup_by_closing_all_sockets();
}

void Server::load_config_file(char *config_file)
{
	if (config_file)
		_config_file = config_file;

	// _config.parsing_config_file(get_config_file()); // throw execption on error

	// set_server_count(_config.get_server_count()); // !! set server count
	// set_host(_config.get_host()); // !! set host and port for each server
	// set_port(_config.get_port());
	// set_error_page(_config.get_error_page()); // !! set error page for each server
	// set_cgi_extension(_config.get_cgi_extension()); // !! set cgi extension for each server
	// set_cgi_path(_config.get_cgi_path()); // !! set cgi path for each server
}

void Server::cleanup_by_closing_all_sockets()
{
	// close all client sockets
	for (Clients::iterator client = _clients.begin(); client != _clients.end(); ++client)
	{
		drop_client(client->first);
		delete client->second;
	}
	_clients.clear();
	// close all server sockets
	for (std::vector<long>::iterator server_socket = _server_sockets.begin(); server_socket != _server_sockets.end(); ++server_socket)
	{
		close(*server_socket);
	}
}



std::vector<long> &Server::get_server_sockets() { return _server_sockets; }

// long Server::get_server_socket() { return _server_socket; }

void Server::throw_error(std::string error_message)
{
	// throw std::runtime_error(error_message + " in server : " + std::to_string(get_server_id()) + ", on port : " + get_port() + ", on host : " + get_host() + "\n");
	throw std::runtime_error(error_message + " in server : " + "\n");
}

bool Server::is_connection_close(std::string &request)
{
	size_t connection_header_pos = request.find("Connection: ");
	if (connection_header_pos != std::string::npos)
	{
		std::string connection_header = request.substr(connection_header_pos + 12, request.find(REQUEST_DELIMETER, connection_header_pos) - connection_header_pos - 12);
		if (connection_header == "close")
			return true;
	}
	return false;
}

Client *Server::get_client(long client_socket)
{
	return _clients[client_socket];
}

Client *Server::create_client()
{
	Client *client = new Client();
	client->get_response().setClient(*client);
	return (client);
}

void Server::drop_client(long client_socket)
{
	FD_CLR(client_socket, &_socket_pool);
	FD_CLR(client_socket, &_read_set);
	FD_CLR(client_socket, &_write_set);
	FD_CLR(client_socket, &_write_set_pool);
	get_client(client_socket)->reset_request_data();  // clear the request buffer for  next request
	get_client(client_socket)->reset_response_data(); // clear the response buffer for  next response
	get_client(client_socket)->reset_total_bytes_sent();
	get_client(client_socket)->get_request_body().clear();
	get_client(client_socket)->get_request_body_length() = 0;
	close(client_socket);
	_clients.erase(client_socket);
}

void Server::feed_request(std::string request, long client_socket) // feed request to the Request class
{
	get_client(client_socket)->get_request().getRequest(request);
}

std::string generate_response()
{
	return std::string("HTTP/1.1 200 OK\r\nContent-Type: text/json\r\nContent-Length: 13\r\n\r\n{\"status\": 6}");
}
void Server::build_response(Request &request, long client_socket) // generate a response
{
	get_client(client_socket)->get_response().processing();
}

void Server::send_response(long client_socket)
{
	std::cout << "\33[32m";
		std::cout << "------> before RESPONSE : <------\n";
		std::cout << get_client(client_socket)->get_response_data() << "\n";

	/* 	// compare the return value from send() with the number of bytes that we tried to send. If the number of bytes actually sent is less than requested, we should use select() to determine when the socket is ready to accept new data, and then call send() with the remaining data. */
	// std::cout << "sending response\n";
	long bytes_sent;
	if ((bytes_sent = send(client_socket, get_client(client_socket)->get_response_data().c_str(), get_client(client_socket)->get_response_data().length(), 0)) == -1)
	{
		std::cerr << "Error: send() failed on client socket " << client_socket <<  "\n";
		drop_client(client_socket);
		return;
	}
	else if (bytes_sent == 0)
	{
		std::cout << "Client socket " << client_socket << " closed the connection\n";
		drop_client(client_socket);
		return;
	}
	else if (bytes_sent < get_client(client_socket)->get_response_data().length()) // if send returns less than the number of bytes requested, we should use select() to determine when the socket is ready to accept new data, and then call send() with the remaining data.
	{
		std::cout << "bytes sent : " << bytes_sent << "\n";
		std::cout << "response not completed  : " << get_client(client_socket)->get_response_data() << "\n";

		get_client(client_socket)->get_response_data().erase(0, bytes_sent);
	}
	else
	{
		std::cout << "\33[32m";
		std::cout << "------> RESPONSE : <------\n";

		// std::cout << "bytes sent : " << bytes_sent << "\n";
		// std::cout << "completed response : " << get_client(client_socket)->get_response_data() << "\n";
		// std::cout << get_client(client_socket)->get_response_data() << "\n";

		// handle case where Connection header is set to close or keep-alive
		if (is_connection_close(get_client(client_socket)->get_request_data()))
		{
			std::cout << "Connection header is set to close\n";
			drop_client(client_socket);
		}
		else
		{
			std::cout << "Connection did not closed, header is set to keep-alive\n";
			FD_CLR(client_socket, &_write_set);
			FD_CLR(client_socket, &_write_set_pool);
			// FD_SET(client_socket, &_socket_pool);
			get_client(client_socket)->reset_response_data(); // clear the request buffer for  next request

			get_client(client_socket)->reset_total_bytes_received();
			get_client(client_socket)->reset_request_data();
		}
	}
}

void Server::handle_outgoing_response(long client_socket)
{
	// build_response(get_client(client_socket)->get_request(), client_socket);
	send_response(client_socket);
}
// ! http request line example:
// !! GET /index.html HTTP/1.1
// !! POST /form HTTP/1.1
std::string Server::get_http_method(std::string &request)
{
	std::string http_method;
	std::size_t found = request.find(" ");
	if (found != std::string::npos)
		http_method = request.substr(0, found);
	return http_method;
}

bool Server::is_request_completed(std::string &request, long client_socket)
{

	std::string http_method = get_http_method(request);

	if (http_method == "POST") // !! not finished - need to check if the request is "Transfer-Encoding: chunked", and other stuff
	{

		size_t end_of_headers_pos = request.find(REQUEST_END);
		if (end_of_headers_pos != std::string::npos)
		{
			size_t content_length_header_pos;
			content_length_header_pos = request.find("Content-Length: ");
			if (content_length_header_pos != std::string::npos)
			{
				size_t end_of_content_length_header_pos = request.find(REQUEST_DELIMETER, content_length_header_pos);
				if (_clients[client_socket]->get_request_body_length() == 0)
				{
					std::string s = request.substr(content_length_header_pos + 16, end_of_content_length_header_pos - content_length_header_pos - 16);
					long i = 0;
					std::istringstream(s) >> i;
					_clients[client_socket]->set_request_body_length(i);
					// _clients[client_socket]->set_request_body_length(std::stoi(request.substr(content_length_header_pos + 16, end_of_content_length_header_pos - content_length_header_pos - 16)));
				}

				size_t start_of_body_pos = end_of_headers_pos + 4; // 4 is the length of the \r\n\r\n

				if (_clients[client_socket]->get_request_body_length() == (_clients[client_socket]->get_request_data().length() - start_of_body_pos))
				{
					_clients[client_socket]->get_request_body_length() = 0;
					return true; // request done - all payload received
				}
			}
			else // erase the request body from the request data
			{
				// request doesnt have Content-Length, request_body_length will be 0
				size_t start_of_body_pos = end_of_headers_pos + 4; // 4 length of \r\n\r\n
				_clients[client_socket]->get_request_data().erase(start_of_body_pos, _clients[client_socket]->get_request_data().length()); // erase the request body from the request data
				return true; // request done - no payload
			}
		}
		else
		{
			return false; // not completed request
		}
	}
	else //((http_method == "GET" || http_method == "DELETE")) // !! usualy http get request is not more than 1k bytes long -  headers + body
	{
		if ((request.find(REQUEST_END) != std::string::npos) )
			return true;
	}

	return false;
}

void Server::handle_incoming_request(long client_socket) // ready to read socket descriptor
{
	char received_data[BUFFER_SIZE];
	long bytes_read;
	if ((bytes_read = recv(client_socket, received_data, BUFFER_SIZE, 0)) == -1) // !! receiving data from a client may not arrive all at once, it can be delivered in chaunks or packets
	{
		std::cerr << "Error: recv() failed on client socket " << client_socket << "\n";
		drop_client(client_socket);
		return;
	}
	else if (bytes_read == 0)
	{
		// std::cout << "Connection closed by the client on socket " << client_socket << ", on server " << get_server_id() << ", port " << get_port() << ", host " + get_host() << "\n";
		drop_client(client_socket);
	}
	else
	{
		get_client(client_socket)->append_total_bytes_received(bytes_read);
		get_client(client_socket)->append_request_data(received_data, bytes_read);

		std::cout << "\033[0;35m ------> REQUEST : <------\n";
		std::cout << " bytes read : " << bytes_read << "\n";
		// std::cout << strlen(received_data) << "\n";
		// std::cout << received_data << "\n";

		// std::string http_method = get_http_method(get_client(client_socket)->get_request_data());

		// if ((bytes_read < BUFFER_SIZE) && http_method != "POST")
		// if (std::string(received_data).length())

		if (is_request_completed(get_client(client_socket)->get_request_data(), client_socket)) // Check if the entire request has been received
		{
			std::cout << "Request completed\n";
			// ! parsing done? build response, move fd from read_set to write_set
			// LOG_INFO("parsing done for socket", client_socket);
			feed_request(get_client(client_socket)->get_request_data(), client_socket); // feed request to the Request class
			build_response(get_client(client_socket)->get_request(), client_socket);

			FD_SET(client_socket, &_write_set_pool);

			// get_client(client_socket)->reset_total_bytes_received();
			// get_client(client_socket)->reset_request_data();
		}

		// else
		// {

		// }
	}
}

void Server::accept_new_connection(long server_socket)
{
	socklen_t client_len = sizeof(struct sockaddr_in);
	long client_socket;
	if ((client_socket = accept(server_socket, (struct sockaddr *)&_client_addr, &client_len)) == -1)
		throw std::runtime_error("accept failed");

	// std::cout << "server " << get_server_id() << " accepted new connection on port " << get_port() << ", host " + get_host() << "\n";
	// std::cout << "accepted new connection " << client_socket  <<  "\n";

	_clients.insert(std::make_pair(client_socket, create_client()));

	FD_SET(client_socket, &_socket_pool);
	if (client_socket > _biggest_socket) // keep tracking of max socket descriptor
		_biggest_socket = client_socket;
}

void Server::zero_socket_pool()
{
	FD_ZERO(&_socket_pool); // clears all  descriptors in the set, initializing it to an empty set.
	FD_ZERO(&_read_set);
	FD_ZERO(&_write_set);
	FD_ZERO(&_write_set_pool);
}

void Server::listen_on_socket(long server_socket)
{
	if (listen(_server_sockets[server_socket], QUEUE_LIMIT) == -1) // SOMAXCONN // 1000 - queue limit for incoming connections before the OS starts rejecting and refusing connections
		throw_error("server socket listening failed");

	// std::cout << "server " << get_server_id() << " listening on port " << get_port() << " on host " + get_host() << "\n";
}

void Server::bind_socket(long server_socket_id, std::string host, int port)
{
	memset(&_server_addr, 0, sizeof(struct sockaddr_in));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(port);	// _server_addr.sin_addr.s_addr = INADDR_ANY;
	// _server_addr.sin_addr.s_addr = inet_addr(host);
	_server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(get_server_sockets()[server_socket_id], (struct sockaddr *)&_server_addr, sizeof(struct sockaddr_in)) == -1)
		throw_error("server socket binding failed");
}

void Server::create_server_socket()
{
	long server_socket;
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw_error("socket creation failed");

	int reuse = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) // socket, level, level options
		throw_error("setsockopt SO_REUSEADDR failed");

	// int nosigpipe = 1;
	// if (setsockopt(server_socket, SOL_SOCKET, SO_NOSIGPIPE, &nosigpipe, sizeof(int)) == -1) // socket, level, level options
	// 	throw_error("setsockopt SO_NOSIGPIPE failed");

	struct timeval timeout;
	timeout.tv_sec = 3; // Timeout value in seconds
	timeout.tv_usec = 0;
	if (setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) // socket, level, level options
		throw_error("setsockopt SO_RCVTIMEO failed");

	if (setsockopt(server_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1) // socket, level, level options
		throw_error("setsockopt SO_SNDTIMEO failed");

	int flags;
	if ((flags = fcntl(server_socket, F_GETFL, 0)) == -1)
		throw_error("fcntl F_GETFL failed");
	if (fcntl(server_socket, F_SETFL, flags | O_NONBLOCK) == -1)
		throw_error("fcntl F_SETFL failed");

	_server_sockets.push_back(server_socket);
	FD_SET(server_socket, &_socket_pool);
	// FD_SET(server_socket, &_write_set_pool);
	_biggest_socket = server_socket;

	// return server_socket;
}

long Server::monitor_clients()
{
	struct timeval timeout;
	long ready_count = 0;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	_read_set = _socket_pool;
	_write_set = _write_set_pool;
	// if (FD_ISSET(b, &_write_set_pool))
	// {
	// 	std::cerr << "b is set2\n";
	// }
	if ((ready_count = select(_biggest_socket + 1, &_read_set, &_write_set, 0, &timeout)) == -1)
	{
		FD_ZERO(&ready_count);
		restart_server("select failed");
	}
	// else if (ready_count == 0)
	// 	std::cout << "select timed-out";
	// 	restart_server("select timed-out after " + int_to_string(timeout.tv_sec) + " seconds");
		// restart_server("select timed-out after " + std::to_string(timeout.tv_sec) + " seconds");

	return ready_count;
}

void Server::start_server()
{
	for (;;)
	{
		long ready_count = monitor_clients(); // monitor socket descriptors for activity
		// for (Clients::iterator client = _clients.begin(); client != _clients.end(); ++client)
		for (long socket = 0; (socket <= _biggest_socket) && ready_count > 0; ++socket)
		{
			if (FD_ISSET(socket, &_read_set)) // ready to read
			{
				if (std::find(_server_sockets.begin(), _server_sockets.end(), socket) != _server_sockets.end()) // socket == server socket
				{
					// ! new connection
					accept_new_connection(socket);
				}
				else
				{
					// ! incoming request
					handle_incoming_request(socket);
				}
			}
			else if (FD_ISSET(socket, &_write_set)) // ready to write
			{
				// ! outgoing response
				handle_outgoing_response(socket);
			}
		}
	}
}

void Server::setup_server()
{
	_ports.push_back(5001);
	// _ports.push_back(5002);
	zero_socket_pool();
	long server_socket;
	for (long i  = 0; i < SERVER_BLOCK_COUNT; i++)
	{
		// std::cout << "server " << i <<  std::endl;
		create_server_socket();
		bind_socket(i, "127.0.0.1", _ports[i]);
		listen_on_socket(i);
	}
}

void Server::start()
{
	setup_server(); // !! add host and port, _config.get_host(index), _config.get_port(index)
	start_server();
}

void Server::restart_server(std::string message)
{
	std::cout << message << "\n";
	std::cout << "server restarted \n";
	cleanup_by_closing_all_sockets();
	setup_server();
	start_server();
}

std::string generate_request(const std::string &host, const std::string &path)
{
	return std::string("GET / HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n\r\n");
}

