#ifndef CONFIG_HPP
#define CONFIG_HPP


#include "includes.hpp"
#include "ConfServer.hpp"
#include "ConfLoca.hpp"
#include "Tools.hpp"

class Config
{
private:
    std::string content;


public:
    Config(const char* filename);
    ~Config();

    std::vector<ConfServer> *parser();
    ConfServer parse_server(size_t *cur);
    ConfLoca	parse_location(size_t *cur);
    int check_line_syntax(std::string line);
    int setLocaValue(ConfLoca *loca, const std::string key, const std::string value);
    int setServValue(ConfServer *serv, const std::string key, const std::string value);

};




#endif