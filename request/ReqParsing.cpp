#include "ReqParsing.hpp"

void ReqParsing::printReq(void)
{
    std::map<std::string, std::string>::iterator it = req.begin();
    while (it != req.end())
    {
        std::cout << "[" << it->first << "]: " << it->second << std::endl;
        ++it;
    }
}

void ReqParsing::startLine(std::string line)
{
    std::istringstream iss(line);
    struct stat fileInfo;
    std::string words;
    while (iss >> words)
    {
        if (words == "POST" || words == "GET" || words == "DELETE")
            this->req["METHODS"] = words;
        else if (words == "HTTP/1.1")
        {
            this->req["PROTOCOLS"] = words;    
        }
        else
            this->req["TARGET"] = words;
    }
}

void ReqParsing::headers(std::string line)
{
    std::istringstream iss(line);
    std::string words;
    while (iss >> words)
    {
        if (words == "Host:")
        {
            iss >> words;
            this->req["HOST"] = words;
        }
        else if (words == "Content-Type:")
        {
            iss >> words;
            this->req["CONTENT_TYPE"] = words;
        }
        else if (words == "Content-Length:")
        {
            iss >> words;
            this->req["CONTENT_LENGTH"] = words;
        }
    }
}

ReqParsing::ReqParsing(const char *f)
{
    std::string line;
    std::ifstream req_file;
    req_file.open(f, std::ios::in);
	if (!req_file) {
		std::cout << "No such file" << std::endl;
        std::exit(0);
	}

    while(std::getline(req_file, line))
    {
        if (!line.empty())
        {
            startLine(line);
            break;
        }
    }

    while (std::getline(req_file, line))
    {
        if (!line.empty())
            headers(line);
    }
    req_file.close();
}

int main(int ac, char *av[])
{
    if (ac != 2)
        std::cout << "really man!! without a file" << std::endl;
    ReqParsing r(av[1]);
    r.printReq();

    return 0;
}