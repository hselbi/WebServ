#ifndef REQUEST_HPP
#define REQUEST_HPP


#include "includes.hpp"

class Request
{
    private:
        std::string _method; // GET, POST, DELETE
        std::string _path; // /index.html
        std::string _version;   // HTTP/1.1
        std::map<std::string, std::string> _headers; // key: value pairs of headers (Host: localhost:8080) (User-Agent: curl/7.68.0) (Accept: */*)
        std::map<std::string, std::string> cgi_env; // cgi environment variables
        std::string _body; // body of the request
        
        int _return_code; // return code of the request
        int _port; // port of the request
        std::list<std::pair<std::string, float>> _lang; // list of languages
        const std::string& _raw;
        std::string _body;
        std::string _query;

        static std::vector<std::string> _methods;
    public:

        Request();
        Request(const std::string& str);
        ~Request();

        /*** GETTERS ***/

        const std::string& getMethod() const;
        const std::string& getPath() const;
        const std::string& getVersion() const;
        const std::map<std::string, std::string>& getHeaders() const;
        const std::map<std::string, std::string>& getCgiEnv() const;
        const std::string& getBody() const;
        int getReturnCode() const;
        int getPort() const;
        const std::list<std::pair<std::string, float>>& getLang() const;
        const std::string& getRaw() const;
        const std::string& getQuery() const;

        /*** SETTERS ***/

        void setMethod(const std::string& method);
        void setBody(const std::string& body);
        void setReturnCode(int return_code);

        /*** UTILITIES ***/

        int parseRequest(const std::string& str);
        void defaultHeaders();
        void displayRequest() const;

        static std::vector<std::string> initMethods();

        /*** OPERATOR ASSIGNEMENTS ***/

        Request& operator=(const Request& other);
};


std::ostream& operator<<(std::ostream& os, const Request& req);
#endif