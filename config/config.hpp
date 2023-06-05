#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "webServ.hpp"
#include "readFiles.hpp"

class config
{
private:
    std::vector<std::string> content;
    unsigned int contentSize;
public:
    config(char *char_path);
    ~config();
    void print();
    config &operator=(config const &src);
    int readFile(const char *filename);
    std::vector<std::string> spliter(std::string line, std::string delim);
    
    class	FileNotFoundException: public std::exception{
        virtual const char	*what() const throw();
    };
};

#endif