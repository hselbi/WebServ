#include "../includes/request/Request.hpp"

bool isWhitespace(const std::string& str) {
    for (std::size_t i = 0; i < str.length(); i++) {
        if (!std::isspace(str[i])) {
            return false;
        }
    }
    return true;
}


void    Request::check_headers(std::string key, std::string value)
{

    if (key == "Host")
    {
        size_t i;

        i = value.find_first_of(':');
        if (i == std::string::npos)
        {
            m_host = value;
            m_port = 80;
        }
        else
        {
            std::string tmp(value, i + 1);
            m_host = value.substr(0, i);
            m_port = atoi(tmp.c_str());
        }
    }
}


// check if request finished or not
bool Request::isWhitespace(const std::string &str)
{
    for (std::size_t i = 0; i < str.length(); i++)
    {
        if (!std::isspace(str[i]))
            return false;
    }
    return true;
}

// bool Request::isFinished(std::string &str, size_t &i)
// {
//     if (m_code_ret == 400)
//         return true;
//     if (!m_headers["Content-Length"].empty())
//     {
//         if (m_headers["Content-Length"] == "0")
//             return true;
//         if (std::stoi(m_headers["Content-Length"]) == m_body.size())
//             return true;
//     }
//     if (!m_headers["Transfer-Encoding"].empty())
//     {
//         if (m_headers["Transfer-Encoding"] == "chunked")
//         {
//             if (str.size() == 0)
//                 return false;
//             if (str[str.size() - 1] == '\n' && str[str.size() - 2] == '\r')
//                 return true;
//         }
//     }
//     return false;
// }

bool Request::isFinished()
{
    if (m_code_ret == 400)
        return true;
    if (!m_headers["Content-Length"].empty())
    {
        if (m_headers["Content-Length"] == "0")
            return true;
        if (std::stoi(m_headers["Content-Length"]) == m_body.size())
            return true;
    }
    if (!m_headers["Transfer-Encoding"].empty())
    {
        if (m_headers["Transfer-Encoding"] == "chunked")
        {
            if (m_body.size() == 0)
                return false;
            if (m_body[m_body.size() - 1] == '\n' && m_body[m_body.size() - 2] == '\r')
                return true;
        }
    }
    return false;
}

// bool Request::isFinished(const std::string &str)
// {
//     if (m_code_ret == 400)
//         return true;
//     if (!m_headers["Content-Length"].empty())
//     {
//         if (m_headers["Content-Length"] == "0")
//             return true;
//         if (std::stoi(m_headers["Content-Length"]) == m_body.size())
//             return true;
//     }
//     if (!m_headers["Transfer-Encoding"].empty())
//     {
//         if (m_headers["Transfer-Encoding"] == "chunked")
//         {
//             if (str.size() == 0)
//                 return false;
//             if (str[str.size() - 1] == '\n' && str[str.size() - 2] == '\r')
//                 return true;
//         }
//     }
//     return false;
// }

int								checkEnd(const std::string& str, const std::string& end)
{
	size_t	i = str.size();
	size_t	j = end.size();

	while (j > 0)
	{
		i--;
		j--;
		if (i < 0 || str[i] != end[j])
			return (1);
	}
	return (0);
}

bool checkReq(const std::string &str)
{
    size_t	i = str.find("\r\n\r\n");
	if (i != std::string::npos)
	{
		if (str.find("Content-Length: ") == std::string::npos)
		{
			if (str.find("Transfer-Encoding: chunked") != std::string::npos)
			{
				if (checkEnd(str, "0\r\n\r\n") == 0)
					return (0);
				else
					return (1);
			}
			else
				return (0);
		}

		size_t	len = std::atoi(str.substr(str.find("Content-Length: ") + 16, 10).c_str());
		if (str.size() >= len + i + 4)
			return (0);
		else
			return (1);
	}

	return (1);
}


void Request::chunkedProcess(const std::string &str)
{

    std::cout << str << std::endl;
	std::string	head = str.substr(0, str.find("\r\n\r\n"));
    std::cout << RED << head << RESET <<std::endl;
	std::string	chunks = str.substr(str.find("\r\n\r\n") + 4, str.size() - 1);
    std::cout << GREEN << chunks << RESET << std::endl;
	// std::string	subchunk = chunks.substr(0, 100);
    // std::cout << YELLOW << subchunk << RESET << std::endl;
	// std::string	body = "";
	// int			chunksize = strtol(subchunk.c_str(), NULL, 16);
    // std::cout << BLUE << chunksize << RESET << std::endl;
	// size_t		i = 0;

	// while (chunksize)
	// {
	// 	i = chunks.find("\r\n", i) + 2;
	// 	body += chunks.substr(i, chunksize);
	// 	i += chunksize + 2;
	// 	subchunk = chunks.substr(i, 100);
	// 	chunksize = strtol(subchunk.c_str(), NULL, 16);
	// }

	// std::string req = head + "\r\n\r\n" + body + "\r\n\r\n";
    // std::cout << PURPLE << req << std::endl;
}


int Request::parseReq(const std::string &str)
{

    // * check if chunked or not
    if (checkReq(str))
        std::cout << "True" << std::endl;
    else
        std::cout << "false" << std::endl;
        
    // std::cout << "here" << std::endl;

    std::string key;
    std::string value;
    std::string line;
    size_t i(0);
    defaultReq();
    reqLine(lineNext(str, i));
    /*
    *   check if line is not equal to "\r\n" or "" or 400
    */
    // while ((line = lineNext(tmp, i)) != "\r" && line != "" && this->m_code_ret != 400)
    while (!isWhitespace(line = lineNext(str, i)) && line != "" && this->m_code_ret != 400)
	{
		key = keyReader(line);
		value = valueReader(line);
        if (m_headers.count(key))
            m_headers[key] = value;
        check_headers(key, value);
		// if (key.find("Secret") != std::string::npos)
		// 	this->_env_for_cgi[formatHeaderForCGI(key)] = value;
	}
    setLanguage();
    if (i != std::string::npos)
    {
        setBody(str.substr(i, std::string::npos));
    }
    setQuery();

    // file.close();

    return m_code_ret;
}

std::vector<std::string>		split(const std::string& str, char c)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(str);

	while (std::getline(tokenStream, token, c))
		tokens.push_back(token);
	return tokens;
}

// comparison, 
bool langsComparition (const std::pair<std::string, float> first, const std::pair<std::string, float> second)
{
  return ( first.second < second.second );
}

void    Request::setLanguage()
{
    std::string header;
    std::vector<std::string> vec;
    size_t i;
    std::string lang;
    header = m_headers["Accept-Language"];

    if (header != "")
    {
        // std::cout << "header = " << header << std::endl;
        vec = split(header, ',');
        for (std::vector<std::string>::iterator it = vec.begin(); it < vec.end(); it++)
        {
            float w = 0.0;
            std::string lang;
            lang = (*it).substr(0, (*it).find_first_of('-'));
            plunder(lang, ' ');
            i = lang.find_first_of(';');
            if (i != std::string::npos)
                w = atof((*it).substr(i + 4).c_str());
            if (i > 2)
                lang.resize(2);
            else
                lang.resize(i);
            m_language.push_back(std::pair<std::string, float>(lang, w));
        }
        m_language.sort(langsComparition);
    }
}

std::string&					capitalize(std::string& str)
{
	size_t	i = 0;

    std::transform(str.begin(), str.end(),str.begin(), ::tolower);
	str[i] = std::toupper(str[i]);
	while((i = str.find_first_of('-', i + 1)) != std::string::npos)
	{
		if (i + 1 < str.size())
		str[i + 1] = std::toupper(str[i + 1]);
	}
	return str;
}

std::string plunder(std::string &str, char c)
{
	if (!str.size())
		return str;
	size_t j = str.size();
	while (j && str[j - 1] == c)
		j--;
	str.resize(j);
	for (j = 0; str[j] == c; j++);
	str = str.substr(j, std::string::npos);
	return str;
}

std::string Request::keyReader(std::string &line)
{
    size_t i;
    std::string key;

    i = line.find_first_of(':');
    key.append(line, 0, i);
    capitalize(key);
    return (plunder(key, ' '));
}

std::string Request::valueReader(std::string &line)
{
    size_t i;
    std::string value;

    i = line.find_first_of(':');
    i = line.find_first_not_of(' ', i + 1);
    if(i != std::string::npos)
        value.append(line, i, std::string::npos);
    return (plunder(value, ' '));
}

int Request::reqLine(const std::string &line)
{
    size_t i;
    std::string str;

    i = line.find_first_of('\n');
    str = line.substr(0, i);
    i = line.find_first_of(' ');
    
    if (i == std::string::npos)
    {
        m_code_ret = 400;
        std::cout << "Error: no method" << std::endl;
        return 400;
    }
    m_method.assign(line, 0, i);
    return (readRequestLine(str, i));
}

int Request::readRequestLine(std::string str, size_t &i)
{
    if (readPath(str, i) == 400)
        return 400;
    if (readVersion(str, i) == 400)
        return 400;
    if (methodChecker() == 400)
        return 400;
    return 200;
}

int Request::readPath(std::string str, size_t &i)
{
    size_t j;

    j = str.find_first_not_of(' ', i);
    if (j == std::string::npos)
    {
        m_code_ret = 400;
        std::cerr << "Error: no path" << std::endl;
        return m_code_ret;
    }
    i = str.find_first_of(' ', j);
    if (i == std::string::npos)
    {
        m_code_ret = 400;
        std::cerr << "Error: no HTTP" << std::endl;
        return m_code_ret;
    }
    m_path.assign(str, j, i - j);
    return 200;
}


int Request::readVersion(std::string str, size_t &i)
{

    i = str.find_first_not_of(' ', i);
    if (i == std::string::npos)
    {
        m_code_ret = 400;
        std::cerr << "Error: no HTTP version" << std::endl;
        return m_code_ret;
    }
    if (str[i] == 'H' && str[i + 1] == 'T' && str[i + 2] == 'T' && str[i + 3] == 'P' && str[i + 4] == '/')
        this->m_version.assign(str, i + 5, 3);
    if (this->m_version != "1.0" && this->m_version != "1.1")
    {
        m_code_ret = 400;
        std::cerr << "Error: Bad HTTP version [" << this->m_version << "]" << std::endl;
        return m_code_ret;
    }
    return 200;
}

int Request::methodChecker()
{
    for (size_t i = 0; i < methods.size(); i++)
    {
        if (methods[i] == m_method)
            return m_code_ret;
    }
    m_code_ret = 400;
    std::cerr << "Error: Invalid method [" << m_method << "]" << std::endl;
    return m_code_ret;
}

std::string Request::lineNext(const std::string &str, size_t &i)
{
    std::string line;
    
    size_t  j;
    if (i == std::string::npos)
        return "";
    j = str.find_first_of('\n', i);
    // std::cout << "i = " << i << " j = " << j << std::endl;
    line = str.substr(i, j - i);
    if (line[line.size() - 1] == '\r')
    {
        // std::cout << "line = " << line << std::endl;
        if (line.size())
		    line.resize(line.size() - 1);
    }
    if (j == std::string::npos)
        i = j;
    else
        i = j + 1;
    
    return line;
}

std::string Request::myGetLine(std::string line, size_t &i)
{
    std::string str;

    size_t  j;
    if (i == std::string::npos)
        return "";
    j = line.find_first_of('\n', i);
    str = line.substr(i, j - i);
    if (str[str.size() - 1] == '\r')
    {
        if (str.size())
            str.resize(str.size() - 1);
    }
    if (j == std::string::npos)
        i = j;
    else
        i = j + 1;
    return str;
}