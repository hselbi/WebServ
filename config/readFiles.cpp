#include "readFiles.hpp"

// void Parsing::splitLine(std::vector<std::string> &content, std::string &line, char delim)
// {
//     size_t size = line.size();
//     size_t start = 0;
//     (void)delim;
//     size_t end;
//     while (start < size)
//     {
//         while (start < size && isspace(line[start]))
//             ++start;
//         if (start == size)
//             break;
//         end = start + 1;
//         while (end < size && !isspace(line[end]))
//             ++end;
//         content.push_back(line.substr(start, end - start));
//         start = end;
//     }
// }

std::vector<std::string> readFiles::spliter(std::string line, std::string delim)
{
    std::vector<std::string> content;

    line += delim[0];
    std::string::size_type start = line.find_first_not_of(delim, 0);
    std::string::size_type end = 0;
    while (true)
    {
        end = line.find_first_of(delim, start);
        if (end == std::string::npos)
            break;
        std::string str = line.substr(start, end - start);
        content.push_back(str);
        if ((start = line.find_first_not_of(delim, end)) == std::string::npos)
            break;
    }
    return content;
}

std::vector<std::string> readFiles::readingFile(const char *filename)
{
    int ret = 1024;
    char buf[1025];
    std::string line="";
    int fd;
    std::vector<std::string> file;
    // std::vector<std::string> token;

    for (size_t i = 0; i < 1025; i++)
        buf[i] = '/0';
    
    if ((fd = open(filename, O_RDONLY)) <= 0)
        throw readFiles::FileNotFoundException();
    
    for ( ret = 1024 ; ret > 0; ret = (ret = read(fd, buf, 1024)))
    {
        buf[ret] = '\0';
        line += buf;
    }

    // get the last line which is ignored
    if (ret == -1) {
		std::cerr << "\033[31m" << "Error while reading config file." << "\033[0m" << std::endl;
		return file;
	}

    file = spliter(line, std::string(" \n\t"));
    return file;
}

const char *readFiles::FileNotFoundException::what() const throw() {
    return "Exception thrown: could not open configuration file";
}