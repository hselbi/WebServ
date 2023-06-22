#pragma once

#include "../includes.hpp"

class Request
{
private:
        std::string m_method;
        std::string m_path;
        std::string m_version;
        std::string m_body;
        std::string m_query;
        std::string m_raw;
        std::map<std::string, std::string> m_body_query;
        std::map<std::string, std::string> m_headers;
        std::map<std::string, std::string> m_env_cgi;
        std::list<std::pair<std::string, float> > m_language;
        static	std::vector<std::string>	methods;
        int m_code_ret;
        int m_port;
public:
    Request();
    Request(const std::string &str);
    ~Request();
    Request &operator=(const Request &other);
    Request(const Request &other);
    
    /* setters */
    void setMethod(const std::string &method);
    void setBody(const std::string &body);
    void setCodeRet(int code);
    void setLanguage();
    void setQuery();
    int setPort();

    static std::vector<std::string>    initMethods();

    /* getters  */
    std::string		getMethod() const;
    std::string		getBody() const;
    int				getCodeRet() const;
    int             getPort() const;
    std::string		getVersion() const;
    std::string		getPath() const;
    std::string		getQuery() const;
    std::string		getRaw() const;
    std::map<std::string, std::string>	getHeaders() const;
    std::map<std::string, std::string>	getEnvCgi() const;
    std::list<std::pair<std::string, float> >	getLanguage() const;


    /* member functions */
    void defaultReq();

    int parseReq(const std::string &str);

    int reqLine(const std::string &line);
    std::string lineNext(const std::string &str, size_t &i);

    int readRequestLine(std::string str, size_t &i);
    int readPath(std::string str, size_t &i);
    int readVersion(std::string str, size_t &i);
    int methodChecker();

    std::string keyReader(std::string &line);
    std::string valueReader(std::string &line);

    std::string myGetLine(std::string line, size_t &i);

    void getRequest(const std::string &str);

    void check_headers(std::string key, std::string value);
};

std::string plunder(std::string &str, char c);

std::ostream&	operator<<(std::ostream& os, const Request& re);
