#include "config.hpp"

config::config(char *char_path)
{
    // std::cout << "config constructor" << std::endl;
    this->content = readFiles::readingFile(char_path);
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