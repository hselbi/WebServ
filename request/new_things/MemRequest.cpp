#include "Request.hpp"



int Request::parseReq(const std::string &str)
{
    std::string key;
    std::string value;
    std::string line;
    size_t i(0);

    reqLine(lineNext(str, i));
    while ((line = lineNext(str, i)) != "\r" && line != "" && this->m_code_ret != 400)
	{
		key = keyReader(line);
		value = valueReader(line);
        if (m_headers.count(key))
            m_headers[key] = value;
		// if (key.find("Secret") != std::string::npos)
		// 	this->_env_for_cgi[formatHeaderForCGI(key)] = value;
	}
    setLanguage();
    setBody(str.substr(i, std::string::npos));
    setQuery();
    

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
    header = m_headers["Content-Language"];

    if (header != "")
    {
        vec = split(header, ',');
        for (std::vector<std::string>::iterator it = vec.begin(); it < vec.end(); i++)
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

	// to_lower(str);
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
	while (j && str[j - 1] == ' ')
		j--;
	str.resize(j);
	for (j = 0; str[j] == ' '; j++);
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
    size_t j;

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
    line = str.substr(i, j - i);
    if (line[line.size() - 1] == '\r')
    {
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