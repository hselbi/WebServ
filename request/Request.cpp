#include "../includes/request/Request.hpp"
#include "../includes/includes.hpp"



std::vector<std::string>		Request::initMethods()
{
	std::vector<std::string>	methods;

	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");

	return methods;
}

std::vector<std::string>	Request::methods = Request::initMethods();


Request::Request(const std::string &str): m_method(""), m_body(""), m_code_ret(200), m_version(""), m_path(""), m_port(80), m_raw(""), m_query("")
{
	std::cout << "Request constructor" << std::endl;
	defaultReq();
	m_env_cgi.clear();

	std::string line;
    std::ifstream req_file;
    req_file.open(str, std::ios::in);
	if (!req_file) {
		std::cout << "No such file" << std::endl;
        std::exit(0);
	}
	while(std::getline(req_file, line))
    {
        if (!line.empty())
        {
			std::cout << "********" << std::endl;
            std::cout << line << std::endl;
			parseReq(line);
			std::cout << "********" << std::endl;
        }
	}

    req_file.close();
	if (m_code_ret != 200)
		std::cerr << "Parsing Error: " << m_code_ret << std::endl;

}

Request::~Request()
{

}

void Request::defaultReq()
{

	// Host: developer.mozilla.org
	// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko/20100101 Firefox/50.0
	// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
	// Accept-Language: en-US,en;q=0.5
	// Accept-Encoding: gzip, deflate, br
	// Referer: https://developer.mozilla.org/testpage.html
	// Connection: keep-alive
	// Upgrade-Insecure-Requests: 1
	// If-Modified-Since: Mon, 18 Jul 2016 02:36:04 GMT
	// If-None-Match: "c561c68d0ba92bbeb8b0fff2a9199f722e3a621a"
	// Cache-Control: max-age=0


	/*
	* search more about the empty values
	*/

	m_headers["Allow"] = "";
	m_headers["Content-Language"] = "";
	m_headers["Content-Location"] = "";
	m_headers["Date"] = "";
	m_headers["Last-Modified"] = "";
	m_headers["Location"] = "";
	m_headers["Retry-After"] = "";
	m_headers["Server"] = "";
	m_headers["Accept"] = "";
	m_headers["Accept-Language"] = "";
	m_headers["Accept-Encoding"] = "";
	m_headers["Referer"] = "";
	m_headers["Connection"] = "keep-alive";
	m_headers["Host"] = "";
	m_headers["User-Agent"] = "";
	m_headers["Content-Type"] = "";
	m_headers["Content-Length"] = "";
	m_headers["Authorization"] = "";
	m_headers["Transfer-Encoding"] = "";
	
}


std::string	Request::getMethod() const {
	return m_method;
}

std::string	Request::getBody() const {
	return m_body;
}

int	Request::getCodeRet() const {
	return m_code_ret;
}

std::string	Request::getVersion() const {
	return m_version;
}

std::string	Request::getPath() const {
	return m_path;
}

std::string	Request::getQuery() const {
	return m_query;
}

std::string	Request::getRaw() const {
	return m_raw;
}

std::map<std::string, std::string>	Request::getHeaders() const {
	return m_headers;
}

std::map<std::string, std::string>	Request::getEnvCgi() const {
	return m_env_cgi;
}

std::list<std::pair<std::string, float> >	Request::getLanguage() const {
	return m_language;
}

int		Request::getPort() const
{
	return m_port;
}

void	Request::setMethod(const std::string &method) {
	m_method = method;
}

void	Request::setCodeRet(int code) {
	m_code_ret = code;
}

std::string	Request::getHost() const {
	return m_host;
}


void	Request::setBody(const std::string& str)
{
	if (str.size() == 0)
		return ;
	char	strip[] = {'\n', '\r'};

	this->m_body.assign(str);
	for (int i = 0; i < 4; i++)
		if (this->m_body.size() > 0 && this->m_body[this->m_body.size() - 1] == strip[i % 2])
		{
			if (m_body.size())
				m_body.resize(m_body.size() - 1);
		}
		else
			break ;
}

void Request::setQuery()
{
	size_t i;

	i = m_path.find_first_of('?');
	if (i != std::string::npos)
	{
		m_query.assign(m_path, i + 1, m_path.size() - i);
		m_path = m_path.substr(0, i);
	}
}


int Request::setPort()
{
	size_t i;

	i = m_headers["Host"].find_first_of(':');
	if (i == std::string::npos)
		m_port = 80;
	else
	{
		std::string tmp(m_headers["Host"], i + 1);
		m_port = atoi(tmp.c_str());
	}
	return m_port;
}

#include <map>

std::ostream&		operator<<(std::ostream& os, const Request& re)
{
	std::map<std::string, std::string>::const_iterator	it;

	os << YELLOW << ">Method : " << GREEN << re.getMethod() << "\n" << RESET;
	os << YELLOW << ">HTTP version : " << GREEN << re.getVersion() << '\n' << RESET;
	os << YELLOW << ">Port : " << GREEN << re.getPort() << '\n' << RESET;
	os << YELLOW << ">Path : " << GREEN << re.getPath() << '\n' << RESET;
	os << YELLOW << ">Query : " << GREEN << re.getQuery() << '\n' << RESET;
	
	
	std::map<std::string, std::string> tmp = re.getHeaders();
	for (it = tmp.begin(); it != tmp.end(); it++)
	{
		if (it->second != "")
			os << YELLOW << it->first << RED << ": " << it->second << RESET << '\n';
	}
	os << '\n' << "Request body :\n" << GREEN << re.getBody() << '\n' << RESET;

	return os;
}

Request &Request::operator=(const Request &other)
{
	if (this != &other)
	{
		m_method = other.m_method;
		m_path = other.m_path;
		m_version = other.m_version;
		m_body = other.m_body;
		m_query = other.m_query;
		m_raw = other.m_raw;
		m_headers = other.m_headers;
		m_env_cgi = other.m_env_cgi;
		m_language = other.m_language;
		m_code_ret = other.m_code_ret;
		m_port = other.m_port;
		m_host = other.m_host;
	}
	return *this;
}

Request::Request(const Request &other)
{
	if (this != &other)
	{
		m_method = other.m_method;
		m_path = other.m_path;
		m_version = other.m_version;
		m_body = other.m_body;
		m_query = other.m_query;
		m_raw = other.m_raw;
		m_headers = other.m_headers;
		m_env_cgi = other.m_env_cgi;
		m_language = other.m_language;
		m_code_ret = other.m_code_ret;
		m_port = other.m_port;
		m_host = other.m_host;
	}
	*this = other;
}