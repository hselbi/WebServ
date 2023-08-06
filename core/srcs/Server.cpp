#include "../../includes/core/Server.hpp"

Server::Server() : _biggest_socket(0), _server_count(1), prev_socket(0), body_ending(false), request_index(0), before_hex(false), after_hex(false), lineFeed(false), carriageReturn(false), getChunkSize(false) {}

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
	get_client(client_socket)->set_res_status(NOT_STARTED);
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
	if (get_client(client_socket))
		delete get_client(client_socket);
	_clients.erase(client_socket);
	std::cout << "Closed client socket " << client_socket << "\n";
}

void Server::feed_request(std::string request, long client_socket) // feed request to the Request class
{
	// std::cout << request << std::endl;
	get_client(client_socket)->get_request().parseReq(request);
	// std::cout << YELLOW << request << RESET<< std::endl;


}


bool Server::isReqFinished(int client_socket)
{
	// !  check methods that dont have body
	// !  check if the request is "Transfer-Encoding: chunked", and other stuff

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
	// std::cout << "Sending response to client socket " << client_socket << "\n";
	// std::cout << "Response: " << get_client(client_socket)->get_response_data() << "\n";
	long bytes_sent;
	if ((bytes_sent = send(client_socket, get_client(client_socket)->get_response_data().c_str(), get_client(client_socket)->get_response_data().length(), 0)) == -1)
	{
		std::cerr << "Error: send() failed on client socket " << client_socket << "\n";
		drop_client(client_socket); // Connection: close
		return;
	}
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


void Server::disconnect_connection(int client_socket)
{
	if (get_client(client_socket) == NULL) // !!!!!!!!! after send failed
		return;
	if (get_client(client_socket)->get_res_status() == DONE )
	{
		drop_client(client_socket); // Connection: close
	}
}


void Server::handle_outgoing_response(long client_socket) // ! send response to client
{
	build_response(get_client(client_socket)->get_request(), client_socket);

	send_response(client_socket);

	disconnect_connection(client_socket);
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

	if (get_client(client_socket)->get_request().getCodeRet() == 400)
		return true;
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
& **************************************** HAFID ***********************************************
*/

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
			// std::cout << RED << "hafid!!!!====> false" << RESET << std::endl;
			return (0);
		}
		else
		{

			// std::cout << GREEN << "hafid!!!!====> true" << RESET<< std::endl;
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

std::string FileExtension(std::string path)
{
    // std::string path = RequestHeaders["Content-Type"];
    if (path.find("text/css") != std::string::npos)
        return ".css";
    if (path.find("text/csv") != std::string::npos)
        return ".csv";
    if (path.find("image/gif") != std::string::npos)
        return "gif";
    if (path.find("text/htm") != std::string::npos)
        return ".html";
    if (path.find("text/html") != std::string::npos)
        return ".html";
    if (path.find("video/mp4") != std::string::npos)
        return ".mp4";
    if (path.find("image/x-icon") != std::string::npos)
        return ".ico";
    if (path.find("image/jpeg") != std::string::npos)
        return ".jpeg";
    if (path.find("image/jpg") != std::string::npos)
        return ".jpeg";
    if (path.find("application/javascript") != std::string::npos)
        return ".js";
    if (path.find("application/json") != std::string::npos)
        return ".json";
    if (path.find("image/png") != std::string::npos)
        return ".png";
    if (path.find("application/pdf") != std::string::npos)
        return ".pdf";
    if (path.find("image/svg+xml") != std::string::npos)
        return ".svg";
    if (path.find("text/plain") != std::string::npos)
        return ".txt";
    return "";
}

/**
 * @brief 
 * 
 * @param received_data 
 * @return size_t 
 */

size_t containsMultipleCarriageReturns(const char received_data[]) {
    int count = 0; // Initialize a counter for '\r' occurrences

    // Iterate through the array until the buffer size or the null terminator is reached
    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        if (received_data[i] == '\r') {
            count++; // Increment the counter for each occurrence of '\r'
        }
    }
    return count; // Return true if more than one occurrence of '\r' was found
}

size_t containsCarriage(const char received_data[]) {
    // int count = 0; // Initialize a counter for '\r' occurrences

    // Iterate through the array until the buffer size or the null terminator is reached
    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        if (received_data[i] == '\r') {
            return i; // Increment the counter for each occurrence of '\r'
        }
    }
    return 0; // Return true if more than one occurrence of '\r' was found
}

size_t containsCarriageNextLvl(const char received_data[], size_t pos) {
    // int count = 0; // Initialize a counter for '\r' occurrences

    // Iterate through the array until the buffer size or the null terminator is reached
    for (size_t i = pos; i < BUFFER_SIZE; ++i) {
        if (received_data[i] == '\r') {
            return i; // Increment the counter for each occurrence of '\r'
        }
    }
    return 0; // Return true if more than one occurrence of '\r' was found
}
/**
 * @brief 
 * 
 * @param received_data 
 * @return size_t 
 */

size_t checkForNewlineOrNull(const char received_data[]) {
    // Iterate through the array until the buffer size or the null terminator is reached
    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        if (received_data[i] == '\n') {
            return 1; // Found '\n', return 1
        }
		else if (received_data[i] == '\0')
            return 2; // Found '\0', return 2
    }

    return 0; // No '\n' and No '\0' found, return 0
}

/**
 * @brief 
 * 
 * @param received_data 
 * @return size_t 
 */

int containsCRLF(const char received_data[], int i) {
    // Iterate through the array until the buffer size minus 1
    // (to avoid accessing beyond the array's bounds) or the null terminator is reached
    for (; i < BUFFER_SIZE; ++i) {
        if (received_data[i] == '\r' && received_data[i + 1] == '\n') {
            return i; // Found "\r\n", return true
        }
    }

    return -1; // "\r\n" not found in the array
}


bool hasLineFeedAtPosition(const char* received_data, size_t position) {
	if (received_data[position] == '\n')
		return true;
	else
		return false;
}


int positionNULL(const char received_data[]) {
    // Iterate through the array until the buffer size minus 1
    // (to avoid accessing beyond the array's bounds) or the null terminator is reached
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        if (received_data[i] == '\0') {
            return i; // Found "\r\n", return true
        }
    }
    return -1; // "\r\n" not found in the array
}

bool checkCRLF(const char received_data[], size_t i) {
    // Iterate through the array until the buffer size minus 1
    // (to avoid accessing beyond the array's bounds) or the null terminator is reached
    for (; i < BUFFER_SIZE - 1; ++i) {
        if (received_data[i] == '\r' && received_data[i + 1] == '\n') {
            return true; // Found "\r\n", return true
        }
    }
    return false; // "\r\n" not found in the array
}

bool hasHexBetweenPositions(const char received_data[], size_t start_pos, size_t end_pos) {
    size_t array_size = strlen(received_data);

    // Check if start_pos and end_pos are within the array bounds
    if (start_pos >= array_size || end_pos >= array_size || start_pos > end_pos) {
        return false;
    }

    // Check if there's a valid hexadecimal representation between start_pos and end_pos
    for (size_t i = start_pos; i <= end_pos; ++i) {
        char c = received_data[i];
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
            return true;
        }
    }

    return false; // No valid hexadecimal representation found
}

// Custom substr function for const char*
const char* substr(const char* inputString, size_t startIndex, size_t length) {
    size_t inputStringLength = strlen(inputString);

    // Check if the start index is within the string length
    if (startIndex >= inputStringLength) {
        return ""; // Return an empty string if the start index is beyond the string length
    }

    // Calculate the actual length of the substring
    size_t actualLength = std::min(length, inputStringLength - startIndex);

    // Create a new char array to hold the substring
    char* substring = new char[actualLength + 1]; // +1 for the null terminator

    // Copy characters from the inputString to the substring
    for (size_t i = 0; i < actualLength; ++i) {
        substring[i] = inputString[startIndex + i];
    }

    // Null-terminate the substring
    substring[actualLength] = '\0';

    return substring;
}

void printWhitespaceSubstring(const char* input, size_t startPos, size_t length) {
    size_t inputLength = strlen(input);

    for (size_t i = startPos; i < startPos + length && i < inputLength; ++i) {
        char currentChar = input[i];
        if (std::isspace(currentChar))
            std::cout << '_'; // Print underscore for whitespace
        else
            std::cout << currentChar;
    }
    std::cout << std::endl;
}

void    Server::recursiveFindHex(const char received_data[], long client_socket, size_t pos)
{
    if (getChunkSize)
        return ;
    if (before_hex)
    {
        size_t next_carr = containsCRLF(received_data, pos);
        if (next_carr)
        {
			_hexa = substr(received_data, pos, next_carr - pos);
			std::cout << "here's hexa!!!! ==>" << _hexa << std::endl;
            // if (hasLineFeedAtPosition(received_data, pos))
            // {
				
            //     // size_t next_crlf = containsCRLF(received_data, pos);
            //     // if (next_carr == next_crlf)
            //     // {

            //     //     // ! we found hexa
            //     //     // ! check if the string has something
            //     //         // ^ if yes, adding it to the string
            //     //         // ^ if no, create it to the string
            //     // }
            //     // else
            //     // {
            //     //     // ! we didn't find hexa
            //     //     before_hex = false;
            //     //     after_hex = false;
            //     //     lineFeed = false;
            //     //     carriageReturn = false;
            //     // }
            // }
            // else
            // {
            //     // ! we didn't find '\n' line feed
            //     before_hex = false;
            //     after_hex = false;
            //     lineFeed = false;
            //     carriageReturn = false;
            // }
        }
        else
        {
			std::cout << "Hafidddd ======>" << std::endl;
            // ! we didn't find '\r' carriage return
            before_hex = false;
            after_hex = false;
            lineFeed = false;
            carriageReturn = false;
        }
    }
    else
    {
        size_t next_carr = containsCarriageNextLvl(received_data, pos + 1);
        if (hasLineFeedAtPosition(received_data, next_carr + 1))
        {

            std::cout << BOLDCYAN << "second conditions!!!!"  << RESET << std::endl;
            carriageReturn = true;
            if (size_t next_crlf = (containsCRLF(received_data, next_carr + 2)))
            {
                if ((next_crlf - next_carr) < 10 && (next_crlf - next_carr) >= 3 && hasHexBetweenPositions(received_data, next_carr, next_crlf))
                {
                    const char* extractedSubstring = substr(received_data, next_carr + 2, next_crlf - next_carr - 2);
                    long  hex_val = hextodec(std::string(extractedSubstring));
                    get_client(client_socket)->get_request().set_rest_chunk(hex_val);
                    size_t rest_chunk = get_client(client_socket)->get_request().getRestChunk();
                    carriageReturn = false;
                    getChunkSize = true;
                }
				else {
					
					if (next_crlf == std::string::npos)
					{
						std::cout << "Hafid!!!! 1" << std::endl;
						// std::cout 
						// ~ we found the first '\r\n' 
						// ~ now we need to check if there's a second one in the next request
						lineFeed = false;
						carriageReturn = false;
						before_hex = true;
						after_hex = false;

					}
					else {

						std::cout << "Hafid!!!! 2" << std::endl;
						// ! there's two 
						size_t crlf_tt = (containsCRLF(received_data, next_crlf + 2));
						if (crlf_tt < 6)
						{
							// ~ we found hexa
							// ! save it in ~ a string ~ until we check the next request
							_hexa = substr(received_data, next_crlf + 2, crlf_tt - next_crlf - 2);
							std::cout << "here's hexa!!!! ==>" << _hexa << std::endl;
							// _hexa = std::string(received_data).substr(next_crlf + 2, crlf_tt - n÷\ext_crlf - 2);
						}
						else{
							// ! we didn't find hexa
							lineFeed = false;
							carriageReturn = false;
							before_hex = false;
							after_hex = false;
						}
					}
				}
            }
        }
        else
        {
			std::cout << BOLDYELLOW << "third conditions!!!!"  << RESET << std::endl;
            recursiveFindHex(received_data, client_socket, next_carr + 1);
        }
    }
}

/*void 	Server::chunkedPost(const char received_data[], long client_socket, size_t request_index)
{
	
	std::ofstream f;
	f.open("test.txt", std::ios::out | std::ios::app | std::ios::binary);

	if (before_hex)
	{
		if (hasLineFeedAtPosition(received_data, 0))
		{
			// std::cout << YELLOW << substr(received_data, 0, 5)<< RESET << std::endl;
			before_hex = false;
			after_hex = true;
		}
		else
		{
			// this is the case where we have a line feed in the middle of the request
		}
	}
	else if (after_hex)
	{
		if (hasLineFeedAtPosition(received_data, 0))
		{
			after_hex = false;
			lineFeed = true;
		}
		else
		{
			// this is the case where we have a line feed in the middle of the request
		}
	}
	else if (lineFeed)
	{
		if (hasLineFeedAtPosition(received_data, 0))
		{
			lineFeed = false;
			carriageReturn = true;
		}
		else
		{
			// std::cout << "hafid@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl

		}
	}

	if (containsMultipleCarriageReturns(received_data))
	{
		// std::cout << BOLDWHITE << request_index << RESET << std::endl;
		size_t carr = containsCarriage(received_data);
		if (carr)
		{
			// ghadi n7tajo f hna ... 
			if (hasLineFeedAtPosition(received_data, carr + 1))
			{
				// std::cout << BOLDRED << request_index << RESET << std::endl;
				carriageReturn = true;
				if (size_t crlf = (containsCRLF(received_data, carr + 2)))
				{
					if ((crlf - carr) < 10 && (crlf - carr) > 4 && hasHexBetweenPositions(received_data, carr, crlf))
					{
						// std::cout << carr << " / " << crlf << std::endl;
						const char* extractedSubstring = substr(received_data, carr + 2, crlf - carr - 2);
						std::string hex = std::string(received_data).substr(carr + 2, crlf - carr - 2);
						// std::cout << "|"<< RED << extractedSubstring << RESET<< "|"<< std::endl;
						long  hex_val = hextodec(std::string(extractedSubstring));
						// std::cout << "|"<< GREEN << hex_val << RESET<< "|"<< std::endl;
						get_client(client_socket)->get_request().set_rest_chunk(hex_val);
						size_t restChunk = get_client(client_socket)->get_request().getRestChunk();

						carriageReturn = false;
						getChunkSize = true;
						// std::cout << restChunk << std::endl;
					}
					else if (crlf == std::string::npos)
					{
						lineFeed = true;
						before_hex = true;


						// ! here we need to check if there's 
						// if (containsCarriage(received_data))

						// std::cout << "hafid@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;

					}
					else {

						size_t crlf_t = (containsCRLF(received_data, crlf + 2));
						// std::cout << "Hafid <<<===============> " << crlf << " / " << crlf_t << std::endl;
						if ((crlf_t - crlf) < 10 && (crlf_t - crlf) > 4 && hasHexBetweenPositions(received_data, crlf, crlf_t))
						{
							// std::cout << RED << crlf << " / " << crlf_t << RESET  << std::endl;
							const char* extractedSubstring = substr(received_data, crlf + 2, crlf_t - crlf - 2);
							std::string hex = std::string(received_data).substr(crlf + 2, crlf_t - crlf - 2);
							// std::cout << "|"<< RED << extractedSubstring << RESET<< "|"<< std::endl;
							long  hex_val = hextodec(std::string(extractedSubstring));
							// std::cout << "|"<< GREEN << hex_val << RESET<< "|"<< std::endl;
							get_client(client_socket)->get_request().set_rest_chunk(hex_val);
							size_t rest_chunk = get_client(client_socket)->get_request().getRestChunk();
							carriageReturn = false;
							getChunkSize = true;
							// std::cout << rest_chunk << std::endl;
						}
						else if (crlf_t == std::string::npos)
						{
							lineFeed = true;
							// ! here we need to check if there's 
							// if (containsCarriage(received_data))

							// std::cout << YELLOW <<"hafid@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << RESET << std::endl;

						}
						else {

							size_t crlf_t = (containsCRLF(received_data, crlf + 2));
							// std::cout << BLUE << "Hafid <<<===============> " << crlf << " / " << crlf_t << RESET << std::endl;
							
						}
						
					}
				}
			}
			else {
				// std::cout << BOLDGREEN << request_index << RESET << std::endl;
				size_t next_carr = containsCarriageNextLvl(received_data, carr + 1);
				if (hasLineFeedAtPosition(received_data, next_carr + 1))
				{
					std::cout << BOLDCYAN << "second conditions!!!!"  << RESET << std::endl;
					carriageReturn = true;
					if (size_t next_crlf = (containsCRLF(received_data, next_carr + 2)))
					{
						if ((next_crlf - next_carr) < 10 && (next_crlf - next_carr) > 4 && hasHexBetweenPositions(received_data, next_carr, next_crlf))
						{
							// std::cout << next_carr << " / " << next_crlf << std::endl;
							const char* extractedSubstring = substr(received_data, next_carr + 2, next_crlf - next_carr - 2);
							// std::string hex = std::string(received_data).substr(carr + 2, crlf - carr - 2);
							// std::cout << "|"<< RED << extractedSubstring << RESET<< "|"<< std::endl;
							long  hex_val = hextodec(std::string(extractedSubstring));
							// std::cout << "|"<< GREEN << hex_val << RESET<< "|"<< std::endl;
							get_client(client_socket)->get_request().set_rest_chunk(hex_val);
							size_t rest_chunk = get_client(client_socket)->get_request().getRestChunk();
							carriageReturn = false;
							getChunkSize = true;
							// std::cout << rest_chunk << std::endl;
						}
						else if (next_crlf == std::string::npos)
						{
							lineFeed = true;
							// ! here we need to check if there's 
							// if (containsCarriage(received_data))

							// std::cout << "hafid@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;

						}
						else {
							size_t crlf_tt = (containsCRLF(received_data, next_crlf + 2));
							// std::cout << "Hafid <<<===============" << crlf_tt << " / " << next_crlf <<  std::endl;
						}
					}
				}
				else {
					std::cout << BOLDRED << "first conditions!!!!"  << RESET << std::endl;

					size_t n_carr = containsCarriageNextLvl(received_data, next_carr + 1);

					std::cout << PURPLE << "$$$$$$$$$$$$$$$$$$$$$$  " << std::endl;
					printWhitespaceSubstring(received_data, next_carr, 500000);
					std::cout << "/" << std::endl;
					printWhitespaceSubstring(received_data, n_carr, 200000);
					std::cout << "  $$$$$$$$$$$$$$$$$$$$$$$$$" << RESET <<std::endl;
					if (hasLineFeedAtPosition(received_data, n_carr + 1))
					{
						std::cout << "AGAAAAAAAAAAAIIIIIIINNNNNNNN" << std::endl;
					}
					else{
						// ! always here after all
						std::cout << "HAFIIIIIID U ARE STUPID " << std::endl;
						size_t next_carr = containsCarriageNextLvl(received_data, carr + 1);
						if (hasLineFeedAtPosition(received_data, next_carr + 1))
						{
							std::cout << BOLDCYAN << "second conditions!!!!"  << RESET << std::endl;
							carriageReturn = true;
							if (size_t next_crlf = (containsCRLF(received_data, next_carr + 2)))
							{
								if ((next_crlf - next_carr) < 10 && (next_crlf - next_carr) > 4 && hasHexBetweenPositions(received_data, next_carr, next_crlf))
								{
									// std::cout << next_carr << " / " << next_crlf << std::endl;
									const char* extractedSubstring = substr(received_data, next_carr + 2, next_crlf - next_carr - 2);
									// std::string hex = std::string(received_data).substr(carr + 2, crlf - carr - 2);
									// std::cout << "|"<< RED << extractedSubstring << RESET<< "|"<< std::endl;
									long  hex_val = hextodec(std::string(extractedSubstring));
									// std::cout << "|"<< GREEN << hex_val << RESET<< "|"<< std::endl;
									get_client(client_socket)->get_request().set_rest_chunk(hex_val);
									size_t rest_chunk = get_client(client_socket)->get_request().getRestChunk();
									carriageReturn = false;
									getChunkSize = true;
									// std::cout << rest_chunk << std::endl;
								}
								else if (next_crlf == std::string::npos)
								{
									lineFeed = true;
									// ! here we need to check if there's 
									// if (containsCarriage(received_data))

									std::cout << "hafid@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;

								}
								else {
									size_t crlf_tt = (containsCRLF(received_data, next_crlf + 2));
									std::cout << "Hafid <<<===============" << crlf_tt << " / " << next_crlf <<  std::endl;
								}
							}
						}
						else
						{
							std::cout << "HAFIIIIIID U ARE STUPID " << std::endl;
						}
						
					}

				}
				// std::cout << YELLOW << carr << " / "<< GREEN << containsCarriageNextLvl(received_data, carr + 1) << RESET<< std::endl;
			}
		}
		else {
			std::cout << "==========> there's nothing more important than this!!!!!!!!!!!!"<< std::endl;
		}
		
	}
	else {
		// std::cout << BOLDYELLOW << request_index << RESET << std::endl;
	}
	// f << received_data;

}
*/
void Server::handle_incoming_request(long client_socket)
{
	char received_data[BUFFER_SIZE];
	long bytes_read;

	memset(received_data, 0, BUFFER_SIZE);
	if ((bytes_read = recv(client_socket, received_data, BUFFER_SIZE, MSG_DONTWAIT)) == -1)
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

		request_index += 1;
		// std::cout << GREEN << "Request number ==> " << request_index<< RESET << std::endl;
		// std::cout << std::string(received_data).size() << std::endl;
		std::ofstream file;
		file.open("bodyRequest.txt", std::ios::out | std::ios::app | std::ios_base::binary);
		if (client_socket != prev_socket)
		{
			// std::cout << YELLOW << request_index<< RESET << std::endl;
			body_ending = false;
			prev_socket = client_socket;
			feed_request(std::string(received_data), client_socket);
			std::string _b = get_client(client_socket)->get_request().getBody();
			file << _b.substr(0, _b.size() - 6);

		}
		else
		{
			std::cout << RED << request_index<< RESET << std::endl;
			std::cout << YELLOW << BUFFER_SIZE << RESET << std::endl;
			size_t old_chunk = get_client(client_socket)->get_request().getRestChunk();
			std::cout << "this is old chunked ===> " << BOLDYELLOW << old_chunk << RESET << std::endl;
			if (old_chunk <= BUFFER_SIZE)
			{
				size_t rest = BUFFER_SIZE - old_chunk;
				std::cout << BOLDGREEN << rest << std::endl;
				if (rest == 1)
				{
					carriageReturn = true;
				}
				else if (rest == 2)
				{
					before_hex = true;
				}
				else if (rest == 0)
				{
					before_hex = false;
					after_hex = false;
					lineFeed = false;
					carriageReturn = false;
					get_client(client_socket)->get_request().set_rest_chunk(0);
				}
				else {
					if (old_chunk)
						get_client(client_socket)->get_request().set_rest_chunk(0);
					// ! need to put the old_chunk in file
					// ! after that get new chunk size
					recursiveFindHex(received_data, client_socket, 0);
					if (getChunkSize)
					{
						size_t new_chunk = get_client(client_socket)->get_request().getRestChunk() - rest;
						get_client(client_socket)->get_request().set_rest_chunk(new_chunk);
					}
					else{
						if (before_hex)
						{
							//  ! receive the rest of other the chunk
							// ! find the '\r\n' and get the hexa
							size_t t = containsCRLF(received_data, 0);
							std::string hex = std::string(received_data).substr(0, t);
							std::cout << "this is hexa ===> " << BOLDYELLOW << hex << RESET << std::endl;
							long  hex_val = hextodec(hex);
							get_client(client_socket)->get_request().set_rest_chunk(hex_val);
							size_t rest_chunk = get_client(client_socket)->get_request().getRestChunk();
							carriageReturn = false;
							getChunkSize = true;
						}
						else{
							if (carriageReturn)
							{
								// ! receive the rest of other the chunk
								// ! find the '\r\n' and get the hexa
								size_t t = containsCRLF(received_data, 0);
								std::string hex = std::string(received_data).substr(1, t);
								std::cout << "this is hexa ===> " << BOLDYELLOW << hex << RESET << std::endl;
								long  hex_val = hextodec(hex);
								get_client(client_socket)->get_request().set_rest_chunk(hex_val);
								size_t rest_chunk = get_client(client_socket)->get_request().getRestChunk();
								carriageReturn = false;
								getChunkSize = true;
							}
							else{
								// ! receive the rest of other the chunk
								// ! find the '\r\n' and get the hexa
								size_t t = containsCRLF(received_data, 0);
								size_t t2 = containsCRLF(received_data, t + 2);
								std::string hex = std::string(received_data).substr(t, t2);
								std::cout << "this is hexa ===> " << BOLDYELLOW << hex << RESET << std::endl;
								long  hex_val = hextodec(hex);
								get_client(client_socket)->get_request().set_rest_chunk(hex_val);
								size_t rest_chunk = get_client(client_socket)->get_request().getRestChunk();
								carriageReturn = false;
								getChunkSize = true;
							}
							// std::cout << BOLDRED << "FAAAAIIIIIIILEEEED TO GET NEEEEWWW CHUNKEEED!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << RESET << std::endl;
							// std::cout << "Before Hex : " << before_hex << std::endl;
							// std::cout << "After Hex : " << after_hex << std::endl;
							// std::cout << "Line Feed : " << lineFeed << std::endl;
							// std::cout << "Carriage Return : " << carriageReturn << std::endl;
							// std::exit(1);
						}
					}

				}

			}
			else
			{
				size_t new_chunk = old_chunk - BUFFER_SIZE;
				get_client(client_socket)->get_request().set_rest_chunk(new_chunk);
			}
			std::cout << BOLDPURPLE << "this is new chunked ===> " << get_client(client_socket)->get_request().getRestChunk() << RESET << std::endl;
		}
		/*======================> this hafid <====================*/
		// !! remove this, only for testing
		if (!isComplete(_requests[client_socket]))
		{
			if (is_request_completed(get_client(client_socket)->get_request_data(), client_socket))
			{
				// std::cout << "hafid" << std::endl;
				match_client_request_to_server_block(client_socket);
				FD_CLR(client_socket, &_read_set_pool);
				FD_SET(client_socket, &_write_set_pool);
			}
		}
		// std::cout << "================" << std::endl;
	}
}



/*
& **************************************** HAFID ***********************************************
*/


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
	// _server_addr.sin_addr.s_addr = INADDR_ANY;
	if (host == "localhost")
		host = "127.0.0.1";
	if (inet_aton(host.c_str(), (struct in_addr *)&_server_addr.sin_addr.s_addr) == 0) // !! host.c_str() should be valid ip address,
		throw_error("inet_aton failed, invalid ip address format");
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

	// !! TODO: check if this is needed because not working on linux
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
		long ready_count = monitor_clients(); // monitor socket descriptors for activity
		for (long socket = 0; (socket <= _biggest_socket) && ready_count > 0; ++socket)
		{
			if (FD_ISSET(socket, &_read_set))
			{
				if (FD_ISSET(socket, &_server_socket_pool)) // ready to read
				{
					accept_new_connection(socket);
				}
				else
				{
					handle_incoming_request(socket);
				}
			}
			else if (FD_ISSET(socket, &_write_set)) // ready to write
			{
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