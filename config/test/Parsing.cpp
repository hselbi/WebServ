#include "Parsing.hpp"

// make lines into one line
void Parsing::lineJoin(std::string &line)
{
    int pos = line.find('#');
    if (pos != std::string::npos)
        line.erase(pos);
    for (std::string::iterator it = line.begin(); it != line.end(); ++it)
    {
        if (*it == '\t' || *it == '\n' || *it == '\r')
            line.replace(it, it + 1, " ");
        
    }
    pos = line.find(';');
    if (pos != std::string::npos)
        line.insert(pos, 1, ' ');
}

// split line into words
void Parsing::splitLine(std::vector<std::string> &content, std::string &line, char delim)
{
    size_t size = line.size();
    size_t start = 0;
    (void)delim;
    size_t end;
    while (start < size)
    {
        while (start < size && isspace(line[start]))
            ++start;
        if (start == size)
            break;
        end = start + 1;
        while (end < size && !isspace(line[end]))
            ++end;
        content.push_back(line.substr(start, end - start));
        start = end;
    }
}

void Parsing::ReadFile(char *path)
{
    std::string line;
    std::ifstream req_file;
    req_file.open(path, std::ios::in);
	if (!req_file) {
		std::cout << "No such file" << std::endl;
        std::exit(0);
	}
    while(std::getline(req_file, line))
    {
        if (!line.empty())
            std::cout << "start line " << line << std::endl;
        content.push_back(line);

        
    }

    try
    {
        checkBrackets(content);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }   
    req_file.close();
}


int main(int argc, char **argv)
{
    Parsing p;

    p.ReadFile(argv[1]);
    return 0;
}