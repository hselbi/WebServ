#include "config.hpp"

config::config(char *char_path)
{
    // std::cout << "config constructor" << std::endl;
    this->_path = char_path;
    this->content = readFiles::readingFile(char_path);
    this->contentSize = content.size();
}



config::~config()
{
    std::cout << "config destructor" << std::endl;
}

void config::print()
{
    // std::cout << "config print**" << std::endl;
    for(size_t i = 0; i < content.size(); i++)
        std::cout << content[i] << "**" << std::endl;
}

int config::parse(int i, std::vector<std::string> content)
{
    std::vector<std::string> tmp;
    std::string directive;

    for(; i < content.size() && content[i] != "}"; i++)
    {
        if (content[i] == "listen")
        {
            i++;
            if (content[i] != "80")
            {
                std::cerr << "Error: listen directive must be 80" << std::endl;
                return (-1);
            }
            tmp.push_back(content[i]);
        }
        else if (content[i] == "server_name")
        {
            i++;
            tmp.push_back(content[i]);
        }
        else if (content[i] == "error_page")
        {
            i++;
            tmp.push_back(content[i]);
        }
        else if (content[i] == "root")
        {
            i++;
            tmp.push_back(content[i]);
        }
        else if (content[i] == "location")
        {
            i++;
            if (content[i] != "/")
            {
                std::cerr << "Error: location directive must be /" << std::endl;
                return (-1);
            }
            tmp.push_back(content[i]);
        }
        else if (content[i] == "autoindex")
        {
            i++;
            if (content[i] != "on" && content[i] != "off")
            {
                std::cerr << "Error: autoindex directive must be on or off" << std::endl;
                return (-1);
            }
            tmp.push_back(content[i]);
        }
        else if (content[i] == "index")
        {
            i++;
            tmp.push_back(content[i]);
        }
        else if (content[i] == "client_max_body_size")
        {
            i++;
            tmp.push_back(content[i]);
        }
        else if (content[i] == "allow_methods")
        {
            i++;
            tmp.push_back(content[i]);
        }
        else if (content[i] == "cgi_extension")
        {
            i++;
            tmp.push_back(content[i]);
        }
        else if (content[i] == "cgi_path")
        {
            i++;
            tmp.push_back(content[i]);
        }
        else if (content[i] == "upload_path")
        {
            i++;
            tmp.push_back(content[i]);
        }
        else if (content[i] == "upload_store")
        {
            i++;
            tmp.push_back(content[i]);
        }
        else if (content[i] == "upload_pass")
        {
            i++;
            tmp.push_back(content[i]);
        }
        else if (content[i] == "upload_store")
        {
            i
    }
}

int config::setup()
{
    std::cout << "config setup" << std::endl;
    unsigned int file_size = content.size();
    for (unsigned int i = 0; i < file_size; i++)
    {
        if (content[i] == "server")
        {
            i++;
            if (content[i] != "{")
            {
                std::cerr << "Error: server block must start with '{'" << std::endl;
                return (-1);
            }
            i++;
            if (parse(i, content) == -1)
            {
                std::cerr << "Error: error in config file ["<< _path << "]" << std::endl;
                return (-1);
            }
            this->_servers.push_back(ser);
        }
        else
        {
            std::cerr << "Error: Unknown context [" << content[i] << "]" << std::endl;
            return (-1);
        }
    }
    
    return (0);
}

config &config::operator=(config const &src)
{
    std::cout << "config operator=" << std::endl;
    return (*this);
}

std::vector<std::string> config::spliter(std::string line, std::string delim)
{
    std::cout << "config spliter" << std::endl;
    return (std::vector<std::string>());
}


const char	*config::FileNotFoundException::what() const throw(){
    return ("File not found");
}