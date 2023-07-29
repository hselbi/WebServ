#include "Post.hpp"

Post::Post(Request &req)
{
    std::cout << "Post" << std::endl;
    std::cout << req.getm_body() << std::endl;
    std::cout << "Post" << std::endl;



    /*********************************/

    std::cout << YELLOW << "POST method\n" << std::endl;;

	if (req.m_headers["Transfer-Encoding"] != "chunked" 
	&& req.m_headers["Content-Length"].empty())
	{
        // ERROR PAGE 411
		return;
	}
	
    /*
    *   check if there's content-type header
    ~   find the beginin of "boundary" until the end of it
    ! loop for 
        * find the begining of *name=* + 6(characters)
        * find the ending (hint is ")
        * if all of the are in npos break the loop 
        * substrct
    */
    if (!req.m_headers["Content-Length"].empty())
    {
        size_t begin = req.m_headers["Content-Type"].find("boundary=");
        if (begin != std::string::npos)
        {
            std::string boundary = req.m_headers["Content-Type"].substr(begin + 9);
            begin = 0;
            size_t end = 0;
            std::string name;
            while (true)
            {
                begin = req.m_body.find("name=", begin) + 6;
                end = req.m_body.find_first_of("\"", begin);
                if (begin == std::string::npos || end == std::string::npos)
                    break;
                name = req.m_body.substr(begin, end - begin);
                begin = req.m_body.find("\r\n\r\n", end) + 4;
                end = req.m_body.find(boundary, begin);
                if (begin == std::string::npos || end == std::string::npos)
                    break;
                if (write_file_in_path(client, req.m_body.substr(begin, end - begin - 4), full_path + "/" + name) < 0)
                    break;
                if (req.m_body[end + boundary.size()] == '-')
                    break;
            }
        }
        else
        {
            // ERROR PAGE 400
            return;
        }
    }
    else
    {
            // ERROR PAGE 400
        return;
    }

	int code = 201;
	if (req.m_headers["Content-Length"] == "0")
		code = 204;

	// Response response(status_info[code]);
	// std::string header = response.make_header();
	int send_ret = send(client.get_socket(), header.c_str(), header.size(), 0);
	if (send_ret < 0)
        // ERROR PAGE 500 (something went wrong) send_ret == -1
	else if (send_ret == 0)
        // ERROR PAGE 400 (client closed the connection)
	else
		std::cout << "> " << full_path << " posted(" << code << ")\n";
}

Post::~Post()
{
}