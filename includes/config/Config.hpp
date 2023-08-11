#ifndef CONFIG_HPP
#define CONFIG_HPP


#include "../includes.hpp"
#include "ConfServer.hpp"
#include "ConfLoca.hpp"
#include "Tools.hpp"

class Config
{
private:
    std::string content;


public:
    Config();
    ~Config();

    std::vector<ConfServer> parser(const char* filename);
    ConfServer parse_server(size_t *cur, size_t id);
    ConfLoca	parse_location(size_t *cur);
    int check_line_syntax(std::string line);
    int check_server_syntax( std::vector<ConfServer> &result);
    int setLocaValue(ConfLoca *loca, const std::string key, const std::string value);
    int setServValue(ConfServer *serv, const std::string key, const std::string value, size_t id);

    int openfile(const char* filename);
};




#endif