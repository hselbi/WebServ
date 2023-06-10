#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "webServ.hpp"
#include "readFiles.hpp"

class config
{
private:
    std::string _path;
    std::vector<std::string> content;
    std::vector<std::string> ser;
    std::vector<std::string> _servers;
    unsigned int contentSize;
public:
    config(char *char_path);
    ~config();
    void print();
    config &operator=(config const &src);
    // std::vector<std::string> readingFile(const char *filename);
    std::vector<std::string> spliter(std::string line, std::string delim);
    
    class	FileNotFoundException: public std::exception{
        virtual const char	*what() const throw();
    };
};

#endif