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
		// std::cout << GREEN << request_index<< RESET << std::endl;
		// std::cout << std::string(received_data).size() << std::endl;
		std::ofstream file;
		file.open("bodyRequest1mb.txt", std::ios::out | std::ios::app | std::ios_base::binary);
		if (client_socket != prev_socket)
		{
			std::cout << YELLOW << request_index<< RESET << std::endl;
			body_ending = false;
			prev_socket = client_socket;
			feed_request(std::string(received_data), client_socket);
			std::string _b = get_client(client_socket)->get_request().getBody();
			file << _b.substr(0, _b.size() - 6);
		}
		else
		{
			// std::cout << BLUE << request_index<< RESET << std::endl;
			std::string next_req = std::string(received_data);
			std::string searchStr = "\r\n";
			size_t cr = next_req.find(searchStr);
			std::string _body;
			size_t closest = 0;
			size_t chunked_size = 0;
			std::string numb;
			if (cr != std::string::npos)
			{
				// std::cout << YELLOW << request_index<< RESET << std::endl;
				size_t prev = 0;
				size_t size_req = next_req.size() - 2;
				size_t pos = next_req.find(searchStr);
				size_t rest =  get_client(client_socket)->get_request().getRestChunk();
				while (pos != std::string::npos) {
					if (body_ending)
						break;
					prev = pos;
					pos = next_req.find(searchStr, pos + searchStr.length());
					if ((pos - prev) < 10)
					{
						numb = next_req.substr(prev + 2, pos - (prev + 2));
						chunked_size = hextodec(numb);
						get_client(client_socket)->get_request().set_rest_chunk(chunked_size);
					}
				}
				if (!body_ending)
				{
					// std::cout << GREEN << "im here in" << body_ending << RESET << std::endl;
					if (rest < size_req)
					{
						size_t end = next_req.find("0\r\n");
						
						if (end)
						{
							size_t mid1 = end - (numb.size() + 1);
							file << next_req.substr(0, mid1);
							size_t mid2 = end + numb.size() - 2;
							size_t cr2 = next_req.find("\r\n", mid2);
							if (cr2 != std::string::npos)
							{
								file << next_req.substr(mid2, cr2) << next_req.substr(cr2 + 4, size_req - cr2 - 6 );
							}
							else{
								size_t cr3 = next_req.find("\r\n", mid2);
								file << next_req.substr(mid2, size_req - mid2 - 4);
							}
						}
						else{
							file << next_req.substr(0, rest - (numb.size() + 2));
							file << next_req.substr(rest, size_req - numb.size());
						}
					}else{
						// std::cout << YELLOW << request_index<< RESET << std::endl;
						if (chunked_size)
						{
							size_t t = next_req.find("\r\n");
							size_t t2 = next_req.find("0\r\n", t + 4);
							std::string start =  next_req.substr(0, t);
							std::string end;
							// std::cout << t << " / " << t2 << std::endl;
							if (t2 != std::string::npos)
							{

								// std::cout << PURPLE << request_index<< RESET << std::endl;
								end =  next_req.substr(t + numb.size() + 4, size_req - t2 - 7);
							}
							else
							{

								// std::cout << GREEN << request_index<< RESET << std::endl;
								// std::cout << BLUE <<t + numb.size() + 4 << " / "<< size_req - (t + numb.size() + 5) << " = "<< size_req << RESET << std::endl;
								end =  next_req.substr(t + numb.size() + 4, size_req - (t + numb.size() + 5));
							}

							file << start;
							file << end;
						}
					}
				}
				// else
				// {
				// std::cout << YELLOW << "chunked ===> " << chunked_size << std::endl;

				// }
				if (!chunked_size)
				{


					body_ending = true;
				}
			}
			else
			{
				// std::cout << RED << request_index<< RESET << std::endl;
				std::cout << GREEN << "==>im here in " << body_ending << RESET << std::endl;
				size_t size_req = next_req.size();
				size_t new_rest;
				size_t rest_chunk = get_client(client_socket)->get_request().getRestChunk();
				if (rest_chunk > size_req)
				{
					std::cout<< RED << "hey ==> " << rest_chunk << RESET << std::endl;
					size_t cr2 = next_req.find("\r\n");
					new_rest = rest_chunk - size_req;
					get_client(client_socket)->get_request().set_rest_chunk(new_rest);
				}
				else{
					std::cout << "hey ==> " << rest_chunk << " / " << size_req << std::endl;
					size_t new_rest = rest_chunk;
					get_client(client_socket)->get_request().set_rest_chunk(new_rest);
				}
				if (!body_ending)
				{
					// std::cout << PURPLE << "##### im here " << RESET << std::endl;
					std::string searchStr = "\r\n";
					size_t cr = next_req.find(searchStr);
					std::cout << "1 ===> cr ===> " << cr << " | "<<size_req <<std::endl;
					file << next_req.substr(0, size_req - 6);

					// file << "111111 " << request_index << "777777";
					// file << "------------------------------------";
				}
				else
				{
					std::string searchStr = "\r\n";
					size_t cr = next_req.find(searchStr);
					std::cout << "2 ===> cr ===> " << cr << " | "<< size_req <<std::endl;
					file << next_req.substr(0, size_req - 6);
					// std::cout << "hafid!!!!" << body_ending << " this size: "<< size_req << " this is buffer: "<< bytes_read << std::endl;
					// file << "++++++++++++++++++++++++++++++++++++";
					// file << "++++++++++++++++++++++++++++++++++++";
					// std::exit(1);
				}
			}
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