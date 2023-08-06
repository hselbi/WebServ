#include "../../includes/core/Server.hpp"


void    recursiveFindHex(const car received_data[], long client_socket, size_t pos)
{
    if (getChunkSize)
        return ;
    if (before_hexa)
    {
        size_t next_carr = containsCarriageNextLvl(received_data, 0);
        if (next_carr < 6)
        {
            if (hasLineFeedAtPosition(received_data, 0))
            {
                size_t next_crlf = containsCRLF(received_data, 0);
                if (next_carr == next_crlf)
                {
                    // ! we found hexa
                    // ! check if the string has something
                        // ^ if yes, adding it to the string
                        // ^ if no, create it to the string
                }
                else
                {
                    // ! we didn't find hexa
                    before_hexa = false;
                    after_hexa = false;
                    lineFeed = false;
                    carriageReturn = false;
                }
            }
            else
            {
                // ! we didn't find '\n' line feed
                before_hexa = false;
                after_hexa = false;
                lineFeed = false;
                carriageReturn = false;
            }
        }
        else
        {
            // ! we didn't find '\r' carriage return
            before_hexa = false;
            after_hexa = false;
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
                if ((next_crlf - next_carr) < 10 && (next_crlf - next_carr) > 4 && hasHexBetweenPositions(received_data, next_carr, next_crlf))
                {
                    const char* extractedSubstring = substr(received_data, next_carr + 2, next_crlf - next_carr - 2);
                    long  hex_val = hextodec(std::string(extractedSubstring));
                    get_client(client_socket)->get_request().set_rest_chunk(hex_val);
                    size_t rest_chunk = get_client(client_socket)->get_request().getRestChunk();
                    carriageReturn = false;
                    getChunkSize = true;
                }
                else if (next_crlf == std::string::npos)
                {
                    // ~ we found the first '\r\n' 
                    // ~ now we need to check if there's a second one in the next request
                    lineFeed = false;
                    carriageReturn = false;
                    before_hex = true;
                    after_hex = false;

                }
                else {

                    // ! there's two 
                    size_t crlf_tt = (containsCRLF(received_data, next_crlf + 2));
                    if (crlf_tt < 6)
                    {
                        // ~ we found hexa
                        // ! save it in ~ a string ~ until we check the next request
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
        else
        {
            recursiveFindHex(received_data, client_socket, next_carr + 1)
        }
    }
}

void    Server::postChunked(const char received_data[], long client_socket, size_t request_index)
{
    std::ofstream f;
    f.open("test.txt", std::ios::out | std::ios::app | std::ios::binary);
}


void 	Server::chunkedPost(const char received_data[], long client_socket, size_t request_index)
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
		size_t carr = containsCarriage(received_data);
		if (carr)
		{
			if (hasLineFeedAtPosition(received_data, carr + 1))
			{
				carriageReturn = true;
				if (size_t crlf = (containsCRLF(received_data, carr + 2)))
				{
					if ((crlf - carr) < 10 && (crlf - carr) > 4 && hasHexBetweenPositions(received_data, carr, crlf))
					{
						const char* extractedSubstring = substr(received_data, carr + 2, crlf - carr - 2);
						std::string hex = std::string(received_data).substr(carr + 2, crlf - carr - 2);
						long  hex_val = hextodec(std::string(extractedSubstring));
						get_client(client_socket)->get_request().set_rest_chunk(hex_val);
						size_t restChunk = get_client(client_socket)->get_request().getRestChunk();
						carriageReturn = false;
						getChunkSize = true;
					}
					else if (crlf == std::string::npos)
					{
						lineFeed = true;
						before_hex = true;
						// ! here we need to check if there's 


					}
					else {

						size_t crlf_t = (containsCRLF(received_data, crlf + 2));
						if ((crlf_t - crlf) < 10 && (crlf_t - crlf) > 4 && hasHexBetweenPositions(received_data, crlf, crlf_t))
						{
							const char* extractedSubstring = substr(received_data, crlf + 2, crlf_t - crlf - 2);
							std::string hex = std::string(received_data).substr(crlf + 2, crlf_t - crlf - 2);
							long  hex_val = hextodec(std::string(extractedSubstring));
							get_client(client_socket)->get_request().set_rest_chunk(hex_val);
							size_t rest_chunk = get_client(client_socket)->get_request().getRestChunk();
							carriageReturn = false;
							getChunkSize = true;
						}
						else if (crlf_t == std::string::npos)
						{
							lineFeed = true;
							// ! here we need to check if there's 

						}
						else {
							size_t crlf_t = (containsCRLF(received_data, crlf + 2));
						}
						
					}
				}
			}
			else {
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
							long  hex_val = hextodec(std::string(extractedSubstring));
							get_client(client_socket)->get_request().set_rest_chunk(hex_val);
							size_t rest_chunk = get_client(client_socket)->get_request().getRestChunk();
							carriageReturn = false;
							getChunkSize = true;
						}
						else if (next_crlf == std::string::npos)
						{
							lineFeed = true;
							// ! here we need to check if there's 

						}
						else {
							size_t crlf_tt = (containsCRLF(received_data, next_crlf + 2));
						}
					}
				}
				else {
					std::cout << BOLDRED << "first conditions!!!!"  << RESET << std::endl;

					size_t n_carr = containsCarriageNextLvl(received_data, next_carr + 1);
					if (hasLineFeedAtPosition(received_data, n_carr + 1))
					{
						std::cout << "AGAAAAAAAAAAAIIIIIIINNNNNNNN" << std::endl;
					}
					else{
						// ! always here after all
						size_t next_carr = containsCarriageNextLvl(received_data, carr + 1);
						if (hasLineFeedAtPosition(received_data, next_carr + 1))
						{
							carriageReturn = true;
							if (size_t next_crlf = (containsCRLF(received_data, next_carr + 2)))
							{
								if ((next_crlf - next_carr) < 10 && (next_crlf - next_carr) > 4 && hasHexBetweenPositions(received_data, next_carr, next_crlf))
								{
									const char* extractedSubstring = substr(received_data, next_carr + 2, next_crlf - next_carr - 2);
									long  hex_val = hextodec(std::string(extractedSubstring));
									get_client(client_socket)->get_request().set_rest_chunk(hex_val);
									size_t rest_chunk = get_client(client_socket)->get_request().getRestChunk();
									carriageReturn = false;
									getChunkSize = true;
								}
								else if (next_crlf == std::string::npos)
								{
									lineFeed = true;
									// ! here we need to check if there's 

								}
								else {
									size_t crlf_tt = (containsCRLF(received_data, next_crlf + 2));
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