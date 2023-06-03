#ifndef READFILES_HPP
#define READFILES_HPP

#include "webServ.hpp"

class readFiles
{
private:
    /* data */
public:

    readFiles(void);
    ~readFiles();
    readFiles &operator=(readFiles const &src);
    std::vector<std::string> readingFile(const char *filename);
    std::vector<std::string> spliter(std::string line, std::string delim);


    class	FileNotFoundException: public std::exception{
			virtual const char	*what() const throw();
		};
    
};



#endif