#include "../includes/request/Request.hpp"


unsigned int                    hextodeci( const std::string &hex ) throw() {
 
    unsigned int dec;
    std::stringstream ss;

    ss << std::hex << hex;
    ss >> dec;

    return dec;
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

std::string                     FileExtension(std::string path)
{
    if (path.find("text/css") != std::string::npos)
        return ".css";
    if (path.find("text/csv") != std::string::npos)
        return ".csv";
    if (path.find("image/gif") != std::string::npos)
        return "gif";
    if (path.find("text/htm") != std::string::npos)
        return ".html";
    if (path.find("text/html") != std::string::npos)
        return ".html";
    if (path.find("video/mp4") != std::string::npos)
        return ".mp4";
    if (path.find("image/x-icon") != std::string::npos)
        return ".ico";
    if (path.find("image/jpeg") != std::string::npos)
        return ".jpeg";
    if (path.find("image/jpg") != std::string::npos)
        return ".jpeg";
    if (path.find("application/javascript") != std::string::npos)
        return ".js";
    if (path.find("application/json") != std::string::npos)
        return ".json";
    if (path.find("image/png") != std::string::npos)
        return ".png";
    if (path.find("application/pdf") != std::string::npos)
        return ".pdf";
    if (path.find("image/svg+xml") != std::string::npos)
        return ".svg";
    if (path.find("text/plain") != std::string::npos)
        return ".txt";
    return "";
}

bool                            langsComparition(const std::pair<std::string, float> first, const std::pair<std::string, float> second)
{
  return ( first.second < second.second );
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

/*      memeber functions       */

void        Request::setLanguage()
{
    std::string header;
    std::vector<std::string> vec;
    size_t i;
    std::string lang;
    header = m_headers["Accept-Language"];

    if (header != "")
    {
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

void        Request::check_headers(std::string key, std::string value)
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

bool        Request::isWhitespace(const std::string &str)
{
    for (std::size_t i = 0; i < str.length(); i++)
    {
        if (!std::isspace(str[i]))
            return false;
    }
    return true;
}

int         Request::parseReq(const std::string &str)
{
    std::string key;
    std::string value;
    std::string line;
    size_t i(0);
    if (_bodyFlag == REQUEST_BODY_NOT_STARTED)
    {
        resetReq();
        reqLine(lineNext(str, i));
        while (!isWhitespace(line = lineNext(str, i)) && line != "" && this->m_code_ret == 200)
        {
            key = keyReader(line);
            value = valueReader(line);
            if (m_headers.count(key))
                m_headers[key] = value;
            check_headers(key, value);
        }
        setLanguage();
        setQuery();
        if (getMethod() == "POST" &&  m_headers["Content-Length"] != "")
        {   
            if (m_headers["Content-Length"] == "0")
            {
                m_code_ret = 411;
                return m_code_ret;
            } 
            if (i != std::string::npos)
            {
                _tmp_file_name = "/tmp/cgi_body_output_" + Utils::generateFileName() + ".txt";
                _tmp_file.open(_tmp_file_name, std::ios::out);
                setBody(str.substr(i));
            }
            int pos_boundary = m_headers["Content-Type"].find("boundary=");
            if (pos_boundary != std::string::npos)
            {
                _boundary = m_headers["Content-Type"].substr(pos_boundary + 9) + "--";
                if (_boundary != "" && str.substr(i).find(_boundary) != std::string::npos)
                {
                    _bodyFlag = REQUEST_BODY_COMPLETED;
                    _tmp_file.close();
                }
                else
                    _bodyFlag = REQUEST_BODY_STARTED;
            }
            else if (_bodyFlag != REQUEST_BODY_COMPLETED)
                _bodyFlag = REQUEST_BODY_STARTED;
                
        }
        else if (getMethod() == "POST" && m_headers["Transfer-Encoding"] == "chunked")
        {
            if (i != std::string::npos)
            {
                _tmp_file_name = "./www/html/uploads/" + Utils::generateFileName() + FileExtension(m_headers["Content-Type"]);
                std::string bd = str.substr(i);
                size_t cr = bd.find("\r\n");
                std::string numb = bd.substr(0, cr);
                chunked_size = hextodeci(numb);
                std::string _body = bd.substr(cr + 2, bd.size() - cr - 2);
                body_size = _body.size();
                rest_chunk = chunked_size - body_size;
                set_rest_chunk(rest_chunk);
                _tmp_file.open(_tmp_file_name, std::ios_base::out | std::ios_base::app | std::ios_base::binary);
                _tmp_file.write(_body.c_str(), _body.size());
            }
            _bodyFlag = REQUEST_BODY_STARTED;
        }
    }
    else if (_bodyFlag == REQUEST_BODY_STARTED && m_headers["Content-Length"] != "")
        setBody(str);
    else if (_bodyFlag == REQUEST_BODY_STARTED && m_headers["Transfer-Encoding"] == "chunked")
        setBody(str);
    return m_code_ret;
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

int         Request::reqLine(const std::string &line)
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

int         Request::readRequestLine(std::string str, size_t &i)
{
    if (readPath(str, i) == 400)
        return 400;
    if (readVersion(str, i) == 400)
        return 400;
    if (methodChecker() == 400)
        return 400;
    return 200;
}

int         Request::readPath(std::string str, size_t &i)
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

int         Request::readVersion(std::string str, size_t &i)
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

int         Request::methodChecker()
{
    for (size_t i = 0; i < methods.size(); i++)
    {
        if (methods[i] == m_method)
            return m_code_ret;
    }
    m_code_ret = 406;
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