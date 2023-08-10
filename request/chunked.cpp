#include "../includes/request/Request.hpp"

/*
bool isChunkLine(const std::string &s)
{
    if (s.empty())
        return false;
    if (s.back() != '\r')
    {
        // std::cout << "1" << std::endl;
        return false;
    }
    for (size_t i = 0; i < s.size(); ++i)
    {
        // std::cout << "2" << std::endl;
        if (!std::isxdigit(s[i]) && (s[i] != '\r' && s[i] != '\n'))
        {
            // std::cout << "3" << std::endl;
            return false;
        }
    }
    return true;
}
*/


/*
void Request::makeChunkedRequest(const std::string &str)
{
    // open file and write from 

    // std::cout << "helllow !!!!" << std::endl;
    // std::fstream requestBody;
    // _tmp_file.open("bod.txt", std::ios_base::out | std::ios_base::app | std::ios_base::binary);
    // requestBody.write();
    if (this->m_headers["Transfer-Encoding"] != "chunked")
        return;

    std::istringstream f(str);
    std::string new_body;
    std::string line;
    // std::cout << "body: " << this->bodyContent << std::endl;

    while (std::getline(f, line))
    {
        if (line == "0")
        {
            _bodyFlag = REQUEST_BODY_COMPLETED;

            break;
        }

        if (isChunkLine(line))
        {
            std::cout << BOLDGREEN << "|" << line << RESET <<"|" <<std::endl;
            // try another way
        }
        else
        {
            // std::cout << "HAAFIIID" << std::endl;
            // new_body += line;
            _tmp_file.write(line.c_str(), line.size());
            f.ignore(2);
        }
    }

    // this->bodyContent = new_body;
    // this->m_headers["content-length"] = std::to_string(new_body.size());
}

*/

/*void    Server::recursiveFindHex(const char received_data[], long client_socket, size_t pos)
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
}*/


bool containsCRLF(const std::string& input) {
    // Search for "\r\n" in the input string
    size_t found = input.find("\r\n");
    // Check if "\r\n" was found
    return (found != std::string::npos);
}


size_t crflFinder(const std::string& input, size_t pos) {
    // Search for "\r\n" in the input string
    size_t found = input.find("\r\n", pos);
    // Check if "\r\n" was found
    return found;
}

bool containsCR(const std::string& input) {
    // Search for "\r" in the input string
    size_t found = input.find("\r");
    // Check if "\r" was found
    return (found != std::string::npos);
}

size_t posContainsCR(const std::string& input, size_t pos) {
    // Search for "\r" in the input string
    size_t found = input.find("\r", pos);
    // Check if "\r" was found
    return (found != std::string::npos);
}


size_t containsLF(const std::string& input) {
    // Search for "\n" in the input string
    size_t found = input.find("\n");
    // Check if "\n" was found
    return found;
}

// find '\n' and '\r' in the string and return the position of '\r'
size_t containsCarriage(const std::string& input) {
    // Search for "\r" in the input string
    size_t found = input.find("\r");
    // Check if "\r" was found
    return found;
}


size_t hextodec(const std::string& hex) {
    size_t result = 0;
    for (size_t i = 0; i < hex.length(); ++i) {
        if (hex[i] >= '0' && hex[i] <= '9') {
            result = result * 16 + (hex[i] - '0');
        }
        else if (hex[i] >= 'a' && hex[i] <= 'f') {
            result = result * 16 + (hex[i] - 'a' + 10);
        }
        else if (hex[i] >= 'A' && hex[i] <= 'F') {
            result = result * 16 + (hex[i] - 'A' + 10);
        }
    }
    return result;
}

long extractHexaCRFL(const std::string& input, size_t pos) {
    size_t found = input.find("\r\n", pos);
    std::string hexa = input.substr(pos, found - pos);
    return hextodec(hexa);
}

long extractHexaCR(const std::string& input, size_t pos) {
    size_t found = input.find("\r", pos);
    std::string hexa = input.substr(pos, found - pos);
    return hextodec(hexa);
}

void Request::makeChunkedRequest(const std::string &str)
{

    /*
        check cases u need to handle '\n' or '\r' or both of them :
        1- \r\n
        2- \r
        4- split hexadecimal into two parts
    */
    size_t buffer_size = getChunkedSize();
    std::cout << "========================================" << std::endl;
    std::cout << BOLDYELLOW << str << RESET << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << BOLDBLUE << buffer_size << RESET << std::endl;
    std::cout << "========================================" << std::endl;

    // std::cout << "========================================" << std::endl;
	long bytes_size = str.size();

    // ! check if there's '\r\n' in previous chunked 
    if (_beforeHex)
    {
        if (_carriageReturn)
        {
            size_t crfl = crflFinder(str, 0);
            std::cout << "=============================" << std::endl;
            std::cout << crfl << std::endl;
            std::string hex = str.substr(0, crfl);
            long hexa = hextodec(hex);
            std::string tmp = str.substr(crfl + 2);
            _tmp_file.write(tmp.c_str(), tmp.size());
            set_rest_chunk(hexa - tmp.size() - 2);
        }
        if (!_hex.empty())
        {
            size_t crlf = extractHexaCRFL(str, 0);
            std::string tmp = str.substr(0, crlf);
            _hex.append(tmp);
            long hexa_value = hextodec(_hex);
            size_t size = str.size() - crlf - 2;
            set_rest_chunk(hexa_value - size);
            _tmp_file.write(str.c_str(), size);
            // _tmp_file.flush();
        }
        else{
            size_t next_crlf = crflFinder(str, 0);
            std::cout << BOLDYELLOW << "next_crlf: " << next_crlf << RESET << std::endl;
            long hexa = extractHexaCRFL(str, 0);
            std::string tmp = str.substr(next_crlf + 2);
            _tmp_file.write(tmp.c_str(), tmp.size());
            set_rest_chunk(hexa - tmp.size() - 2);
            // exit(0);
            // _tmp_file.flush();
        }
        return;
    }
    else if (_carriageReturn)
    {
        // * check if there's '\r\n' in the string
        size_t lf = containsLF(str);
        if (lf != std::string::npos)
        {
            // * hna kayn 3ndna '\n'
            size_t crlf = crflFinder(str, lf);
            std::string tmp = str.substr(0, crlf);
            long hexa = extractHexaCRFL(str, crlf);
            size_t size = str.size() - crlf - 2;
            set_rest_chunk(hexa - size);
            _tmp_file.write(str.c_str(), size);
            // _tmp_file.flush();
        }
        return;
    }
    if (buffer_size > REQUEST_BUFFER_SIZE)
    {
        // ~ this is for doesn't have chunked size
        size_t new_chunk = buffer_size - bytes_size;
        set_rest_chunk(new_chunk);
        _tmp_file.write(str.c_str(), bytes_size);
        // _tmp_file.flush();
    }
    else
    {
        std::cout << "========================================" << std::endl;
        std::string hexaDecimal;
        size_t crlf = crflFinder(str, 0);
        if (crlf != std::string::npos)
        {
            std::cout << buffer_size << std::endl << str << std::endl;
            std::cout << crlf << std::endl;
            // * hna kayn 3ndna '\r\n'
            _tmp_file.write(str.c_str(), buffer_size);
            size_t next_crlf = crflFinder(str, buffer_size + 2);
            std::cout << next_crlf << std::endl;
            if (next_crlf != std::string::npos)
            {
                std::cout << "2" << std::endl;
                // * kayn 3endna '\r\n' f had chunk
                long hexa_val = extractHexaCRFL(str, crlf + 1);
                std::cout << hexa_val << std::endl;
                if (hexa_val == 0)
                {
                    std::cout << "end" << std::endl;
                    // ~ this is the last chunk
                    _bodyFlag = REQUEST_BODY_COMPLETED;
                    std::string tmp = str.substr(crlf);
                    _tmp_file.write(tmp.c_str(), tmp.size());
                    // _tmp_file.flush();
                    _tmp_file.close();
                    set_rest_chunk(0);
                    return;
                }
                else
                {
                    std::cout << "not yet" << std::endl;
                    std::string tmp = str.substr(next_crlf + 2);
                    _tmp_file.write(tmp.c_str(), tmp.size());
                    // _tmp_file.flush();
                    set_rest_chunk(hexa_val - tmp.size());
                    // exit(0);
                }
            }
            else 
            {
                // * makaynch '\r\n' f had chunk
                size_t cr = posContainsCR(str, crlf + 2);
                std::cout << crlf << " | " <<cr <<" 1.1 => " << str.substr(crlf) << std::endl;
                if (cr != std::string::npos)
                {
                    // ~ need to 
                    std::cout << "2.2" << std::endl;
                    // * kayn '\r' tanya f had chunk
                    std::string hex = str.substr(crlf + 2);
                    std::cout << BOLDRED << str.substr(crlf)<< RESET << std::endl;

                    long hexa_val = hextodec(hex);
                    std::cout << hexa_val << std::endl;
                    set_rest_chunk(hexa_val - 2); // -1: '\r' lil9ina f lkher
                    _carriageReturn = true; // 3ndna '\r' tanya f had chunk
                    _beforeHex = true; // 3ndna '\r\n' f had chunk
                }
                else
                {
                    std::cout << "3" << std::endl;
                    // * makaynch '\r' f had chunk
                    _beforeHex = true;
                    _carriageReturn = false;
                    _lineFeed = false;
                    if (str.size() > crlf + 2)
                        _hex = str.substr(crlf + 2);
                }
            }
        }
        else {
            std::cout << "hadi makhashach tdkhel" << std::endl;
            size_t cr = extractHexaCR(str, 0);
            if (cr != std::string::npos)
            {
                // * hna kayn 3ndna '\r'
                _carriageReturn = true;
                _beforeHex = false;
                _lineFeed = false;
            }
        }
    }
    // std::cout << BOLDPURPLE << "this is new chunked ===> " << get_client(client_socket)->get_request().getRestChunk() << RESET << std::endl;
}
