#include "../../includes/core/Server.hpp"

Server::Server() : _biggest_socket(0){}

Server::~Server()
{
	cleanup_by_closing_all_sockets();
}

void Server::cleanup_by_closing_all_sockets()
{
	// close all client sockets
	for (Clients::iterator client = _clients.begin(); client != _clients.end(); ++client)
	{
		drop_client(client->first);
		delete client->second;
	}
	_clients.clear(); // clear the clients map
	for (std::vector<long>::iterator server_socket = _server_sockets.begin(); server_socket != _server_sockets.end(); ++server_socket) // close all server sockets
		close(*server_socket);
}

void Server::load_config_file(const char *config_file)
{
	_server_blocks = _config.parser(config_file);

}

std::vector<long> &Server::get_server_sockets() { return _server_sockets; }

void Server::throw_error(std::string error_message)
{
	throw std::runtime_error(error_message + "\n");
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

bool Server::is_request_completed(std::string &request, long client_socket)
{
    std::string http_method = get_client(client_socket)->get_request().getMethod();

    if (get_client(client_socket)->get_request().getCodeRet() != 200)
        return true;

    if (http_method == "GET" || http_method == "DELETE")
    {
		// usually GET and DELETE requests size is less than 1000 bytes
        if ((request.find(REQUEST_END) != std::string::npos))
            return true;
    }
    else if (http_method == "POST")
    {
        std::string content_length = get_client(client_socket)->get_request().getHeaders()["Content-Length"];
        std::string transfer_encoding = get_client(client_socket)->get_request().getHeaders()["Transfer-Encoding"];

        if (!content_length.empty())
        {
            if (content_length == "0" || get_client(client_socket)->get_request().getBodyFlag() == REQUEST_BODY_COMPLETED)
				return true;
        }
        else if (!transfer_encoding.empty())
        {
            if (transfer_encoding == "chunked")
            {
				if (get_client(client_socket)->get_request().getBodyFlag() == REQUEST_BODY_COMPLETED)
                {
					std::cout << BOLDGREEN << "Uploading done. \n" << RESET; 
					return true;
				}
            }
        }
        else
        {
            size_t end_of_headers_pos = request.find(REQUEST_END);
            if (end_of_headers_pos != std::string::npos)
                return true;
        }
    }
    return false;
}

void Server::match_client_request_to_server_block(long client_socket)
{
	for (std::vector<ConfServer>::iterator server_block = _server_blocks.begin(); server_block != _server_blocks.end(); ++server_block)
	{
		if (server_block->getHost() == get_client(client_socket)->get_request().getHost() && server_block->getPort() == get_client(client_socket)->get_request().getPort())
		{
			get_client(client_socket)->set_server_block(*server_block);
			return;
		}
	}
}

void Server::zero_socket_pool()
{
	FD_ZERO(&_server_socket_pool); // clears all  descriptors in the set, initializing it to an empty set.
	FD_ZERO(&_read_set);
	FD_ZERO(&_write_set);
	FD_ZERO(&_read_set_pool);
	FD_ZERO(&_write_set_pool);
}

void Server::drop_client(long client_socket)
{
	get_client(client_socket)->set_res_status(NOT_STARTED);
	FD_CLR(client_socket, &_read_set);
	FD_CLR(client_socket, &_read_set_pool);
	FD_CLR(client_socket, &_write_set);
	FD_CLR(client_socket, &_write_set_pool);
	get_client(client_socket)->reset_request_data();  // clear the request buffer for  next request
	get_client(client_socket)->reset_response_data(); // clear the response buffer for  next response
	if (get_client(client_socket))
		delete get_client(client_socket);
	_clients.erase(client_socket);
	close(client_socket);
}

Client *Server::get_client(long client_socket)
{
	if (_clients.find(client_socket) == _clients.end())
		return NULL;
	return _clients[client_socket];
}

Client *Server::create_client()
{
	Client *client = new Client();
	client->get_response().setClient(*client);
	client->get_cgi().setClient(*client);
	return (client);
}

void Server::listen_on_socket(long server_socket)
{
	if (listen(_server_sockets[server_socket], QUEUE_LIMIT) == -1) // - queue limit for incoming connections before the OS starts rejecting and refusing connections -  maximum number of incoming connections that can be waiting in the queue for acceptance, while your server is currently handling other connections.
		throw_error("server socket listening failed");
}

void Server::bind_socket(long server_socket_id, std::string host, int port)
{
	std::vector<ConfServer>::iterator server_block = _server_blocks.begin();
	memset(&_server_addr, 0, sizeof(struct sockaddr_in));
	_server_addr.sin_family = AF_INET; // IPv4
	_server_addr.sin_port = htons(port);
	// _server_addr.sin_addr.s_addr = INADDR_ANY;
	if (host == "localhost")
		host = "127.0.0.1";
	if (inet_aton(host.c_str(), (struct in_addr *)&_server_addr.sin_addr.s_addr) == 0) // !! host.c_str() should be valid ip address
		throw_error("inet_aton failed, invalid ip address format");
	if (bind(get_server_sockets()[server_socket_id], (struct sockaddr *)&_server_addr, sizeof(struct sockaddr_in)) == -1)
		throw_error("Host or Port is invalid.");
}

void Server::create_server_socket()
{
	long server_socket;
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw_error("socket creation failed");

	int reuse = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) // socket, level, level options
		throw_error("setsockopt SO_REUSEADDR failed");

	int nosigpipe = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_NOSIGPIPE, &nosigpipe, sizeof(int)) == -1) // socket, level, level options
		throw_error("setsockopt SO_NOSIGPIPE failed");

	int flags;
	if ((flags = fcntl(server_socket, F_GETFL, 0)) == -1) // get file status flags,
		throw_error("fcntl F_GETFL failed");
	if (fcntl(server_socket, F_SETFL, flags | O_NONBLOCK) == -1) // set file status flags, O_NONBLOCK - non-blocking mode
		throw_error("fcntl F_SETFL failed");

	_server_sockets.push_back(server_socket);
	FD_SET(server_socket, &_server_socket_pool);
	FD_SET(server_socket, &_read_set_pool);
	if (server_socket > _biggest_socket)
		_biggest_socket = server_socket;
}

void Server::disconnect_connection(int client_socket)
{
	if (get_client(client_socket) == NULL)
		return;
	if (get_client(client_socket)->get_res_status() == DONE )
		drop_client(client_socket);
}

void Server::build_response(Request &request, long client_socket) // generate a response
{
	get_client(client_socket)->get_response().processing();
	
}

void Server::send_response(long client_socket)
{
	long bytes_sent;
	if ((bytes_sent = send(client_socket, get_client(client_socket)->get_response_data().c_str(), get_client(client_socket)->get_response_data().length(), MSG_DONTWAIT)) == -1)
	{
		drop_client(client_socket);
		return;
	}
	else if (bytes_sent < get_client(client_socket)->get_response_data().length())
	{
		get_client(client_socket)->get_response_data().erase(0, bytes_sent);
	}
	get_client(client_socket)->get_response_data().clear();
}

void Server::handle_outgoing_response(long client_socket)
{

	build_response(get_client(client_socket)->get_request(), client_socket);

	send_response(client_socket);

	disconnect_connection(client_socket);
}

void Server::feed_request(std::string request, long client_socket)
{
	get_client(client_socket)->get_request().parseReq(request);
}

void Server::handle_incoming_request(long client_socket)
{
	char received_data[BUFFER_SIZE];
	long bytes_read;

	memset(received_data, 0, BUFFER_SIZE);
	if ((bytes_read = recv(client_socket, received_data, BUFFER_SIZE, MSG_DONTWAIT)) == -1)
	{
		drop_client(client_socket);
		return;
	}
	else if (bytes_read == 0)
		drop_client(client_socket);
	else
	{
		get_client(client_socket)->append_request_data(received_data, bytes_read);
		feed_request(std::string(received_data, bytes_read), client_socket);
		if (is_request_completed(get_client(client_socket)->get_request_data(), client_socket))
		{
			match_client_request_to_server_block(client_socket);
			FD_CLR(client_socket, &_read_set_pool);
			FD_SET(client_socket, &_write_set_pool);
		}
	}
}


void Server::accept_new_connection(long server_socket)
{
	socklen_t client_len = sizeof(struct sockaddr_in);
	long client_socket;
	if ((client_socket = accept(server_socket, 0, &client_len)) == -1)
		throw std::runtime_error("accept failed");

	_clients.insert(std::make_pair(client_socket, create_client()));

	get_client(client_socket)->set_server_socket(server_socket);

	FD_SET(client_socket, &_read_set_pool);
	if (client_socket > _biggest_socket) // keep tracking of max socket descriptor
		_biggest_socket = client_socket;
}

long Server::monitor_clients()
{
	struct timeval timeout;
	long ready_count = 0;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	FD_ZERO(&_read_set);
	FD_ZERO(&_write_set);

	_read_set = _read_set_pool;
	_write_set = _write_set_pool;

	if ((ready_count = select(_biggest_socket + 1, &_read_set, &_write_set, 0, &timeout)) == -1)
		restart_server("select failed");
	return ready_count;
}

void Server::start_server()
{
	for (;;)
	{
		long ready_count = monitor_clients(); // monitor socket descriptors for activity, return number of ready clients
		for (long socket = 0; (socket <= _biggest_socket) && ready_count > 0; ++socket)
		{
			if (FD_ISSET(socket, &_read_set))
			{
				if (FD_ISSET(socket, &_server_socket_pool)) // ready to read
				{
					accept_new_connection(socket); // accept new client
				}
				else
				{
					handle_incoming_request(socket); // read request from client
				}
			}
			else if (FD_ISSET(socket, &_write_set)) // ready to write
			{
				handle_outgoing_response(socket); // construct response then send it to client
			}

		}
	}
}

void Server::setup_server()
{
	zero_socket_pool();
	for (long i = 0; i < _server_blocks.size(); i++)
	{
		create_server_socket(); // create server socket
		bind_socket(i, _server_blocks[i].getHost(), _server_blocks[i].getPort()); // bind server socket to host and port from config
		listen_on_socket(i); // listen on server socket, ready to accept incoming connections
		_server_port.insert(std::make_pair(_server_sockets[i], _server_blocks[i].getPort())); //
		std::cout << PURPLE << "Server " << (i + 1) << " created, Host: " << _server_blocks[i].getHost() << ", listening on Port: " << _server_blocks[i].getPort() << RESET << std::endl;
	}
}

void Server::start()
{
	setup_server();
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
