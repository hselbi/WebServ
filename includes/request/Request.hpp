#pragma once

#include "../includes.hpp"
#include <map>
#include <fstream>
#define REQUEST_BUFFER_SIZE 2000
// enum
enum requestFlag {
    REQUEST_NOT_COMPLETED,
    REQUEST_COMPLETED,
};
enum requestBodyFlag {
    REQUEST_BODY_NOT_STARTED,
    REQUEST_BODY_STARTED,
    REQUEST_BODY_COMPLETED,
} ;


class Request
{
private:
        std::string bodyContent;
        std::string m_method;
        std::string m_path;
        std::string m_version;
        std::string m_query;
        std::string m_raw;
        std::string m_host;
		std::string m_cookie;
        std::map<std::string, std::string> m_body_query;
        std::map<std::string, std::string> m_headers;
        std::map<std::string, std::string> m_env_cgi;
        std::list<std::pair<std::string, float> > m_language;
        static	std::vector<std::string>	methods;

	    int _req_status;
        int m_code_ret;
        int m_port;
        size_t chunked_size;
        size_t rest_chunk;
        size_t body_size;
        int _rest;
        int _bodyFlag;
        bool _carriageReturn;
        bool _lineFeed;
        bool _beforeHex;
        std::string _hex;
        std::fstream _tmp_file;
        std::string _tmp_file_name;
        std::string _boundary;
        std::fstream requestBody;

public:
    Request();
    Request(const std::string &str);
    ~Request();
    Request &operator=(const Request &other);
    Request(const Request &other);

    static std::vector<std::string>    initMethods();
    
    /*      setters         */

    void    setMethod(const std::string &method);
    void    setBody(const std::string &body);
    void    setPath(const std::string &path);
	void    set_req_status(int status);
    void    set_size_body(size_t size);
    void    set_rest_chunk(size_t stops);
    void    setCodeRet(int code);
    void    setLanguage();
    void    setQuery();
    int     setPort();
    void    setBodyFlag(int flag);

    /*      getters     */
    
    std::map<std::string, std::string>          getHeaders() const;
    std::map<std::string, std::string>          getEnvCgi() const;
    std::list<std::pair<std::string, float> >   getLanguage() const;
    std::string		                            getMethod() const;
    std::string		                            getBody() const;
    std::string		                            getVersion() const;
    std::string		                            getPath() const;
    std::string		                            getQuery() const;
    std::string		                            getRaw() const;
    std::string		                            getHost() const;
    std::string                                 getBodyFileName();
    size_t                                      getBodySize() const;
    size_t                                      getChunkedSize() const;
    size_t                                      getRestChunk() const;
    int                                         get_req_status();
    int			                                getCodeRet() const;
    int                                         getPort() const;
    int                                         getBodyFlag();

    /*      member functions        */

    void        resetReq();
    void        defaultReq();
    int         parseReq(const std::string &str);
    int         reqLine(const std::string &line);
    std::string lineNext(const std::string &str, size_t &i);

    int         readRequestLine(std::string str, size_t &i);
    int         readPath(std::string str, size_t &i);
    int         readVersion(std::string str, size_t &i);
    int         methodChecker();

    std::string keyReader(std::string &line);
    std::string valueReader(std::string &line);
    std::string myGetLine(std::string line, size_t &i);
    void        getRequest(const std::string &str) const ;
    void        check_headers(std::string key, std::string value);

    bool        isWhitespace(const std::string &str);
    void        makeChunkedRequest(const std::string &str);
    
};

std::string plunder(std::string &str, char c);

std::ostream&	operator<<(std::ostream& os, const Request& re);
