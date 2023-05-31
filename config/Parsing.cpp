#include "Parsing.hpp"

void Parsing::checkBrackets(std::vector<std::string> content)
{
    auto it = content.begin();
    int count = 0;
    while (it != content.end())
    {
        for (char c: *it)
        {
            if (c == '{')
                count++;
            else if (c == '}')
                count--;
        }
        ++it;
    }
    std::cout << "count: " << count << std::endl;
    if (count != 0)
        throw std::runtime_error("Error: Brackets are not balanced");
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