#include "../../includes/core/Server.hpp"

Server::Server() : _biggest_socket(0), _server_count(1), prev_socket(0) {}

Server::~Server()
{
	cleanup_by_closing_all_sockets();
}

void Server::load_config_file(const char *config_file)
{
	_server_blocks = _config.parser(config_file);
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

void Server::drop_client(long client_socket)
{
	FD_CLR(client_socket, &_read_set);
	FD_CLR(client_socket, &_read_set_pool);
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
	// std::cout << request << std::endl;

	get_client(client_socket)->get_request().parseReq(request);
	std::cout << get_client(client_socket)->get_request() << std::endl;


}


bool Server::isReqFinished(int client_socket)
{
	// !  check methods that dont have body 
	// !  check if the request is "Transfer-Encoding: chunked", and other stuff
	
    if (get_client(client_socket)->get_request().getCodeRet() == 400)
		return true;
    if (!get_client(client_socket)->get_request().getHeaders()["Content-Length"].empty())
    {
        if (get_client(client_socket)->get_request().getHeaders()["Content-Length"] == "0")
		{
			// 204 (No Content) or 304 (Not Modified)
            return true;
		}
        if (std::stoi(get_client(client_socket)->get_request().getHeaders()["Content-Length"]) == get_client(client_socket)->get_request().getBody().size())
            return true;
    }
    if (!get_client(client_socket)->get_request().getHeaders()["Transfer-Encoding"].empty())
    {
        if (get_client(client_socket)->get_request().getHeaders()["Transfer-Encoding"] == "chunked")
        {
            if (get_client(client_socket)->get_request().getBody().size() == 0)
                return false;
            if (get_client(client_socket)->get_request().getBody()[get_client(client_socket)->get_request().getBody().size() - 1] == '\n' && get_client(client_socket)->get_request().getBody()[get_client(client_socket)->get_request().getBody().size() - 2] == '\r')
                return true;
        }
    }
    return false;
}

std::string generate_response()
{
	return std::string("HTTP/1.1 200 OK\r\nContent-Type: text/json\r\nContent-Length: 13\r\n\r\n{\"status\": 6}");
}

void Server::send_response(long client_socket)
{
	long bytes_sent;
	if ((bytes_sent = send(client_socket, get_client(client_socket)->get_response_data().c_str(), get_client(client_socket)->get_response_data().length(), 0)) == -1)
	{
		std::cerr << "Error: send() failed on client socket " << client_socket << "\n";
		drop_client(client_socket);
		return;
	}
	// else if (bytes_sent == 0)
	// {
	// 	std::cout << "send bytes send 0 " <<  "\n";
	// 	return;
	// }
	else if (bytes_sent < get_client(client_socket)->get_response_data().length()) // if send returns less than the number of bytes requested, we should use select() to determine when the socket is ready to accept new data, and then call send() with the remaining data.
	{
		std::cout << "send bytes send < response length "
				  << "\n";
		get_client(client_socket)->get_response_data().erase(0, bytes_sent);
	}
	else
	{
		get_client(client_socket)->get_response_data().clear();
	}
}

void Server::build_response(Request &request, long client_socket) // generate a response
{
	get_client(client_socket)->get_response().processing();
}

void Server::handle_outgoing_response(long client_socket) // ! send response to client
{

	build_response(get_client(client_socket)->get_request(), client_socket);
	send_response(client_socket);
	if (get_client(client_socket) == NULL)
		return;
	if (get_client(client_socket)->get_res_status() == DONE)
	{
		if (is_connection_close(get_client(client_socket)->get_request_data()))
		{
			// std::cout << "Connection: close" << std::endl;
			drop_client(client_socket); // Connection: close
		}
		else // Connection: keep-alive
		{
			// std::cout << "Connection: keep-alive" << std::endl;
			FD_CLR(client_socket, &_write_set_pool);
			FD_SET(client_socket, &_read_set_pool);

			get_client(client_socket)->reset_request_data();
			get_client(client_socket)->reset_response_data();
			get_client(client_socket)->set_res_status(NOT_STARTED);

			get_client(client_socket)->get_request().resetReq();
			// TODO: reset response
		}
	}
}

// !! GET /index.html HTTP/1.1 | POST /form HTTP/1.1
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
				size_t start_of_body_pos = end_of_headers_pos + 4;																			// 4 length of \r\n\r\n
				_clients[client_socket]->get_request_data().erase(start_of_body_pos, _clients[client_socket]->get_request_data().length()); // erase the request body from the request data
				return true;																												// request done - no payload
			}
		}
		else
		{
			return false; // not completed request
		}
	}
	else
	{
		if ((request.find(REQUEST_END) != std::string::npos))
			return true;
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


/*
!	======================= From Hafid ===================================
*/
int     checkEnd(const std::string& str, const std::string& end)
{
	size_t	i = str.size();
	size_t	j = end.size();

	while (j > 0)
	{
		i--;
		j--;
		if (i < 0 || str[i] != end[j])
			return (1);
	}
	return (0);
}

void		Server::processChunk(long socket)
{
	std::cout << BLUE << _requests[socket].size() << RESET<< std::endl;
	std::string	head = _requests[socket].substr(0, _requests[socket].find("\r\n\r\n"));
	std::cout << "Head ==> " << BLUE << head.size() << RESET<< std::endl;
	
	std::string	chunks = _requests[socket].substr(_requests[socket].find("\r\n"), _requests[socket].size() - 1);
	std::cout << "Chunks ==> " << BLUE << chunks.size() << RESET<< std::endl;
	std::string	subchunk = chunks.substr(0, 100);
	std::string	body = "";
	int			chunksize = strtol(subchunk.c_str(), NULL, 16);
	size_t		i = 0;
	std::cout << "==> $$$" << chunksize << "<===" <<std::endl; 

	while (chunksize)
	{
		i = chunks.find("\r\n", i) + 2;
		body += chunks.substr(i, chunksize);
		i += chunksize + 2;
		subchunk = chunks.substr(i, 100);
		chunksize = strtol(subchunk.c_str(), NULL, 16);
	}

	_requests[socket] = head + "\r\n\r\n" + body + "\r\n\r\n";
}

bool Server::checkReq(const std::string &str)
{
    size_t	i = str.find("\r\n\r\n");
    // std::cout << "===> " << RED << str << RESET << std::endl;
	if (i != std::string::npos)
	{
		if (str.find("Content-Length: ") == std::string::npos)
		{
			std::cout << "------->" << std::endl;
			if (str.find("Transfer-Encoding: chunked") != std::string::npos)
			{
				if (checkEnd(str, "0\r\n\r\n") == 0)
				{
					// std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5" << std::endl;
					return (0);
				}
				else
					return (1);
			}
			else
				return (0);
		}

		body_length = std::atoi(str.substr(str.find("Content-Length: ") + 16, 10).c_str());
		std::cout << body_length << std::endl;
		if (str.size() >= body_length + i + 4)
		{
			std::cout << RED << "hafid!!!!====> false" << RESET << std::endl;
			return (0);
		}
		else
		{

			std::cout << GREEN << "hafid!!!!====> true" << RESET<< std::endl;
			return (1);
		}
	}

	return (1);
}

bool isComplete(const std::string &str)
{
	if (str.find("Transfer-Encoding: chunked") != std::string::npos)
	{
		if (checkEnd(str, "0\r\n\r\n") == 0)
		{
			std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5" << std::endl;
			return (0);
		}
		else
			return (1);
	}
	else
		return (0);
}
unsigned int hextodec( const std::string &hex ) throw() {
 
    unsigned int dec;
    std::stringstream ss;

    ss << std::hex << hex;
    ss >> dec;

    return dec;
 
}
std::string checkingRecv(const std::string &str)
{
	std::string head;
	// head until \r\n\r\n
	size_t end = str.find("0\r\n\r\n");
	size_t crlf = str.find("\r\n\r\n");
	size_t cr = str.find("\r\n");
	if (end != std::string::npos)
		std::cout << "==>>" << end << "<<===" <<std::endl;
	else if(crlf != std::string::npos)
	{
		std::cout << "==>>" << crlf << std::endl;
		head = str.substr(0, crlf);
	}
	else if (cr != std::string::npos)
	{
		// size_t hex = str.find("\r\n");
		std::string numb = str.substr(0, cr);
		std::cout << "rest!!!==> " << hextodec(numb) << std::endl;
	}
	else{
		std::cout << str << std::endl;
	}
	

	// from 
	return str;
}


/*
!	======================= From Hafid ===================================
*/



void Server::handle_incoming_request(long client_socket)
{
	char received_data[BUFFER_SIZE];
	long bytes_read;
	std::cout << client_socket << std::endl;
	if ((bytes_read = recv(client_socket, received_data, BUFFER_SIZE, 0)) == -1)
	{
		std::cerr << "Error: recv() failed on client socket " << client_socket << " on server port " << _server_port[get_client(client_socket)->get_server_socket()] << "\n";
		drop_client(client_socket);
		return;
	}
	else if (bytes_read == 0)
	{
		std::cout << "Connection closed by the client on socket " << client_socket << " on server port " << _server_port[get_client(client_socket)->get_server_socket()] << "\n";
		drop_client(client_socket);
	}
	else
	{
		if (client_socket != prev_socket)
		{
			std::cout << "this is first time!" << std::endl;
			prev_socket = client_socket;
			feed_request(std::string(received_data), client_socket);
			
		}
		else
		{
			std::cout << "this is not first time!" << std::endl;
			std::string str = std::string(received_data);
			// std::cout << YELLOW << "1===> " << str.size() << RESET << std::endl;
			size_t cr = str.find("\r\n");
			std::cout << RED << cr<< RESET << std::endl;
			std::string numb = str.substr(0, cr);
			std::cout << numb << std::endl;

			std::cout << PURPLE << "2===> " << (str.substr(cr)).size() << "/" << str.size() << RESET << std::endl;



			std::cout << BLUE << hextodec(numb) << RESET << std::endl;
			size_t len = hextodec(numb);
			size_t i = 0;
			std::string _body;
			while (len)
			{
				
				_body += str[i];
				len--;
				i++;
			}

			std::cout << GREEN << "3===> " << _body.size() << "/" << str.size() << RESET << std::endl;
			

		}
		

		// std::cout << "===$$$> " <<bytes_read << std::endl;
		// get_client(client_socket)->append_request_data(received_data, bytes_read);
		// std::string afterReq = checkingRecv(std::string(received_data));
		// _requests[client_socket] += afterReq;
		

		// if (checkReq(_requests[client_socket]))
		// 	std::cout << GREEN << "==> chunked" << RESET << std::endl;
		// else
		// 	std::cout << RED << "==> Not chunked" << RESET << std::endl;
		// std::cout << "===>" <<_requests[client_socket].size()<<"/"<<len_request << std::endl;
		// if (_requests[client_socket].find("Transfer-Encoding: chunked") != std::string::npos && _requests[client_socket].find("Transfer-Encoding: chunked") < _requests[client_socket].find("\r\n\r\n"))
		// 	processChunk(client_socket);
		// std::cout << "final results ==> " << _requests[client_socket].size() << std::endl;
		
		
		
		/*======================> this hafid <====================*/
		// !! remove this, only for testing
		if (!isComplete(_requests[client_socket]))
		{
			if (is_request_completed(get_client(client_socket)->get_request_data(), client_socket))
			{
				std::cout << "hafid" << std::endl;
				match_client_request_to_server_block(client_socket);
				FD_CLR(client_socket, &_read_set_pool);
				FD_SET(client_socket, &_write_set_pool);
			}
		}
	}
}


void Server::accept_new_connection(long server_socket)
{
	socklen_t client_len = sizeof(struct sockaddr_in);
	long client_socket;
	if ((client_socket = accept(server_socket, (struct sockaddr *)&_client_addr, &client_len)) == -1)
		throw std::runtime_error("accept failed");

	std::cout << PURPLE << "accepted new client " << client_socket << " on port " << _server_port[server_socket] << RESET << "\n";

	_clients.insert(std::make_pair(client_socket, create_client()));

	get_client(client_socket)->set_server_socket(server_socket);

	FD_SET(client_socket, &_read_set_pool);
	if (client_socket > _biggest_socket) // keep tracking of max socket descriptor
		_biggest_socket = client_socket;
}

void Server::zero_socket_pool()
{
	FD_ZERO(&_server_socket_pool); // clears all  descriptors in the set, initializing it to an empty set.
	FD_ZERO(&_read_set);
	FD_ZERO(&_write_set);
	FD_ZERO(&_read_set_pool);
	FD_ZERO(&_write_set_pool);
}

void Server::listen_on_socket(long server_socket)
{
	if (listen(_server_sockets[server_socket], QUEUE_LIMIT) == -1) // SOMAXCONN // 1000 - queue limit for incoming connections before the OS starts rejecting and refusing connections
		throw_error("server socket listening failed");
}

void Server::bind_socket(long server_socket_id, std::string host, int port)
{
	std::vector<ConfServer>::iterator server_block = _server_blocks.begin();
	memset(&_server_addr, 0, sizeof(struct sockaddr_in));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(port);
	if (host == "localhost")
		host = "127.0.0.1";
	if (inet_aton(host.c_str(), (struct in_addr *)&_server_addr.sin_addr.s_addr) == 0) // !! host.c_str() should be valid ip address,
		throw_error("inet_aton failed, invalid ip address format");

	// _server_addr.sin_addr.s_addr = INADDR_ANY;
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
	FD_SET(server_socket, &_server_socket_pool);
	FD_SET(server_socket, &_read_set_pool);
	if (server_socket > _biggest_socket)
		_biggest_socket = server_socket;
}

long Server::monitor_clients()
{
	struct timeval timeout;
	long ready_count = 0;
	timeout.tv_sec = 1;
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
		long ready_count = monitor_clients(); // monitor socket descriptors for activity
		for (long socket = 0; (socket <= _biggest_socket) && ready_count > 0; ++socket)
		{
			if (FD_ISSET(socket, &_read_set))
			{
				if (FD_ISSET(socket, &_server_socket_pool)) // ready to read
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
				handle_outgoing_response(socket); // !! send response to client
			}
		}
	}
}

void Server::setup_server()
{
	zero_socket_pool();
	for (long i = 0; i < _server_blocks.size(); i++)
	{
		create_server_socket();
		bind_socket(i, _server_blocks[i].getHost(), _server_blocks[i].getPort());
		listen_on_socket(i);
		_server_port.insert(std::make_pair(_server_sockets[i], _server_blocks[i].getPort()));
		std::cout << "Server " << (i + 1) << " created, Host: " << _server_blocks[i].getHost() << ", listening on Port: " << _server_blocks[i].getPort() << std::endl;
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
