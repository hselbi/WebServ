#include "Post.hpp"

Post::Post(Request &req)
{
    std::cout << "Post" << std::endl;
    std::cout << req.getBody() << std::endl;
    std::cout << "Post" << std::endl;



    /*********************************/

    std::cout << YELLOW << "POST method\n" << std::endl;;

	if (req.headers["Transfer-Encoding"] != "chunked" 
	&& req.headers.find("Content-Length") == req.headers.end())
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
    if (req.headers.find("Content-Type") != req.headers.end())
    {
        size_t begin = req.headers["Content-Type"].find("boundary=");
        if (begin != std::string::npos)
        {
            std::string boundary = req.headers["Content-Type"].substr(begin + 9);
            begin = 0;
            size_t end = 0;
            std::string name;
            while (true)
            {
                begin = req.body.find("name=", begin) + 6;
                end = req.body.find_first_of("\"", begin);
                if (begin == std::string::npos || end == std::string::npos)
                    break;
                name = req.body.substr(begin, end - begin);
                begin = req.body.find("\r\n\r\n", end) + 4;
                end = req.body.find(boundary, begin);
                if (begin == std::string::npos || end == std::string::npos)
                    break;
                if (write_file_in_path(client, req.body.substr(begin, end - begin - 4), full_path + "/" + name) < 0)
                    break;
                if (req.body[end + boundary.size()] == '-')
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
	if (req.headers["Content-Length"] == "0")
		code = 204;

	Response response(status_info[code]);
	std::string header = response.make_header();
	int send_ret = send(client.get_socket(), header.c_str(), header.size(), 0);
	if (send_ret < 0)
		send_error_page(500, client, NULL);
	else if (send_ret == 0)
		send_error_page(400, client, NULL);
	else
		std::cout << "> " << full_path << " posted(" << code << ")\n";
}

Post::~Post()
{
}