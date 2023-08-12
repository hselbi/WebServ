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

Request::Request(): m_method(""), m_code_ret(200), m_version(""), m_path(""), m_port(80), m_raw(""), m_query(""),
	  _req_status(REQUEST_NOT_COMPLETED), _bodyFlag(REQUEST_BODY_NOT_STARTED), _tmp_file_name(""), _boundary(""), bodyContent(""), _carriageReturn(false), _lineFeed(false), _beforeHex(false), _hex(""), _rest(-1), m_host("127.0.0.1")
{}

void Request::resetReq()
{
	body_size = 0;
	chunked_size = 0;
	rest_chunk = 0;
	m_method = "";
	m_code_ret = 200;
	m_version = "";
	m_path = "";
	m_port = 80;
	m_raw = "";
	m_query = "";
	m_headers.clear();
	m_env_cgi.clear();
	m_language.clear();
	defaultReq();
	_req_status = REQUEST_NOT_COMPLETED;
	_bodyFlag = REQUEST_BODY_NOT_STARTED;
	_tmp_file_name = "";
	_boundary = "";
	_tmp_file.close();
	bodyContent = "";
	m_host = "127.0.0.1";
}

Request::Request(const std::string &str): m_method(""), m_code_ret(200), m_version(""), m_path(""), m_port(80), m_raw(""), m_query("")
{
	resetReq();
	m_env_cgi.clear();
	std::string line;
    std::ifstream req_file;
    req_file.open(str, std::ios::in);
	if (!req_file) {
		std::cout << "No such file" << std::endl;
	}
	while(std::getline(req_file, line))
    {
        if (!line.empty())
			parseReq(line);
	}
    req_file.close();
	if (m_code_ret != 200)
		std::cerr << "Parsing Error: " << m_code_ret << std::endl;
}

Request::Request(const Request &other)
{
	if (this != &other)
	{
		m_method = other.m_method;
		m_path = other.m_path;
		m_version = other.m_version;
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

Request::~Request() {}

void Request::defaultReq()
{
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
	m_headers["Content-Disposition"] = "";
	m_headers["Cookie"] = "";
}

/*		setters		*/

void	Request::setBodyFlag(int flag) {	_bodyFlag = flag;	}

void	Request::setMethod(const std::string &method) {	m_method = method;	}

void	Request::setPath(const std::string &path) {	m_path = path;	}

void	Request::setCodeRet(int code) {	m_code_ret = code;	}

void	Request::set_size_body(size_t size) {	this->body_size = size;	}

void	Request::set_rest_chunk(size_t stops) {	chunked_size = stops;	}

void	Request::set_req_status(int status) { _req_status = status; }

void	Request::setBody(const std::string& str)
{
	
    if (getMethod() == "POST" &&  m_headers["Content-Length"] != "")
	{
		std::string tmp = str;
		if (tmp.size() == 0)
			return ;
		else if (tmp.size() > REQUEST_BUFFER_SIZE)
			tmp = tmp.substr(0, REQUEST_BUFFER_SIZE);
		if (_tmp_file.is_open())
			_tmp_file << tmp;
		
		if (_boundary != "" && tmp.find(_boundary) != std::string::npos)
		{
			_bodyFlag = REQUEST_BODY_COMPLETED;
			_tmp_file.close();
		}
		else if (_boundary == "" && m_headers["Content-Type"].find("boundary=") == std::string::npos)
		{
			_tmp_file.seekg(0, std::ios::end);
			std::streampos fileSize = _tmp_file.tellg();
			_tmp_file.seekg(0, std::ios::beg);
			
			if ( fileSize == atol(m_headers["Content-Length"].c_str()))
			{
				_bodyFlag = REQUEST_BODY_COMPLETED;
				_tmp_file.close();
			}
		}
	}
    else if (getMethod() == "POST" && m_headers["Transfer-Encoding"] == "chunked")
		makeChunkedRequest(str);

}

void	Request::setQuery()
{
	size_t i;
	
	i = m_path.find_first_of('?');
	if (i != std::string::npos)
	{
		m_query.assign(m_path, i + 1, m_path.size() - i);
		m_path = m_path.substr(0, i);
	}
}

int		Request::setPort()
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

/*		getters		*/

std::map<std::string, std::string>			Request::getHeaders() const {	return m_headers;	}

std::map<std::string, std::string>			Request::getEnvCgi() const {	return m_env_cgi;	}

std::list<std::pair<std::string, float> >	Request::getLanguage() const {	return m_language;	}

std::string									Request::getMethod() const {	return m_method;	}

std::string									Request::getVersion() const {	return m_version;	}

std::string									Request::getPath() const {	return m_path;	}

std::string									Request::getQuery() const {	return m_query;	}

std::string									Request::getRaw() const {	return m_raw;	}

std::string									Request::getHost() const {	return m_host;	}

std::string									Request::getBodyFileName() {	return _tmp_file_name;	}

size_t										Request::getChunkedSize() const {	return chunked_size;	}

int											Request::getBodyFlag() {	return _bodyFlag;	}

int											Request::getCodeRet() const {	return m_code_ret;	}

int											Request::getPort() const {	return m_port;	}

int											Request::get_req_status() { return _req_status; }

/*		operator assignements		*/

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
	return os;
}

Request			&Request::operator=(const Request &other)
{
	if (this != &other)
	{
		m_method = other.m_method;
		m_path = other.m_path;
		m_version = other.m_version;
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
