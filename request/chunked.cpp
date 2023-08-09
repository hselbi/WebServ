#include "../includes/request/Request.hpp"


bool isChunkLine(const std::string &s)
{
    if (s.empty())
        return false;
    if (s.back() != '\r')
    {
        return false;
    }
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (!std::isxdigit(s[i]) && (s[i] != '\r' && s[i] != '\n'))
        {
            return false;
        }
    }
    return true;
}

void Request::makeChunkedRequest(void)
{
    if (this->m_headers["transfer-encoding"] != "Chunked")
        return;

    std::istringstream f(this->bodyContent);
    std::string new_body;
    std::string line;

    while (std::getline(f, line))
    {
        if (line == "0")
            break;

        if (isChunkLine(line))
        {
            size_t chunk_size;
            std::istringstream iss(line);
            iss >> std::hex >> chunk_size;
            std::string chunk_data(chunk_size, '\0');
            f.read(&chunk_data[0], chunk_size);
            new_body += chunk_data;
            f.ignore(2);
        }
        else
        {
            new_body += line;
            f.ignore(2);
        }
    }

    this->bodyContent = new_body;
    this->m_headers["content-length"] = std::to_string(new_body.size());
}