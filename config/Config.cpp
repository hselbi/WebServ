#include "Config.hpp"


Config::Config(const char* filename)
{
    // std::cout << filename << std::endl;
    std::string read;
	std::ifstream fs;

	content.clear();
	fs.open(filename);
	if (fs.is_open())
	{
		while (!fs.eof())
		{
			getline(fs, read);
			content.append(read + '\n');
		}
		fs.close();
	}
	else
	{
		std::cout << "\e[0;31m[ERROR] file open failed.\n\e[0m";
		exit(1);
	}
    // std::cout << "====================================================" << std::endl;
    // std::cout << "## ** " << content << std::endl;
    // std::cout << "====================================================" << std::endl;
}

// void 

Config::~Config()
{
}

std::vector<ConfServer> *Config::parser()
{

    std::vector<ConfServer> *result = new std::vector<ConfServer>();

    std::cout << "> config file parsing start\n";
	size_t pre = 0;
	size_t cur = content.find_first_not_of(" \t\n", pre);
    std::cout << "==> "<< cur << std::endl;
	if (cur == std::string::npos)
    {
        std::cout << "[ERROR] config parsing failed." << std::endl;
		exit(1);
    }
	while (cur != std::string::npos)
	{
		pre = content.find_first_not_of(" \t\n", cur);;
		cur = content.find_first_of(" \t\n", pre);
		std::string key = content.substr(pre, cur - pre);
		if (key != "server")
        {
            std::cout << "[ERROR] config parsing failed." << std::endl;
			exit(1);
        }
		ConfServer server = parse_server(&cur);
		result->push_back(server);
	}

	std::cout << "> config file parsing finish\n";
	return result;
}

ConfServer Config::parse_server(size_t *t)
{
    ConfServer result;
	size_t key_start;
	size_t value_end;

	size_t pre = content.find_first_not_of(" \t\n", *t);
	if (pre == std::string::npos || content[pre] != '{')
    {

        std::cout << "[ERROR] config parsing failed." << std::endl;
		exit(1);
    }
		
	pre++;
	size_t cur = content.find_first_not_of(" \t\n", pre);
	while (cur != std::string::npos)
	{
		if ((pre = content.find_first_not_of(" \t\n", cur)) == std::string::npos)
        {
            std::cout << "[ERROR] config parsing failed." << std::endl;
            exit(1);
        }
		key_start = pre;
		if ((cur = content.find_first_of(" \t\n", pre)) == std::string::npos)
        {
            std::cout << "[ERROR] config parsing failed." << std::endl;
            exit(1);
        }
		std::string key = content.substr(pre, cur - pre);
		if (key == "}")
		{
			*t = content.find_first_not_of(" \n\t", cur + 1);
			break;
		}

		if (key == "location")
		{
			result.locations.push_back(parse_location(&cur));
		}
		else
		{
			if ((pre = content.find_first_not_of(" \t\n", cur)) == std::string::npos)
			{
                std::cout << "[ERROR] config parsing failed." << std::endl;
                exit(1);
            }
			if ((cur = content.find_first_of("\n", pre)) == std::string::npos)
            {
                std::cout << "[ERROR] config parsing failed." << std::endl;
                exit(1);
            }
			if ((value_end = check_line_syntax(content.substr(key_start, cur - key_start))) == -1)
            {
                std::cout << "[ERROR] config parsing failed." << std::endl;
                exit(1);
            }
			if ((int)value_end == -2)
				continue;
			std::string value = content.substr(pre, value_end - pre + key_start + 1);
			if (setServValue(&result, key, value) == -1)
            {
                std::cout << "[ERROR] config parsing failed." << std::endl;
				exit(1);
            }
		}
	}
	return result;
}

int Config::setServValue(ConfServer *serv, const std::string key, const std::string value)
{

	if (key == "server_name")
	{
		serv->server_name = value;
	}
	else if (key == "listen")
	{
		if (value.find_first_of(':') == std::string::npos)
		{
			serv->host = "0.0.0.0";
			serv->port = value;
		}
		else
		{
			std::vector<std::string> tmp = split(value, ':');
			if (serv->host != "" && serv->host != tmp[0])
				return -1;
			serv->host = tmp[0];
			serv->port = tmp[1];
		}
	}
	else if (key == "root")
	{
		serv->root = value;
	}
	else if (key == "index")
	{
		std::vector<std::string> tmp = split(value, ' ');
		for (unsigned long i = 0; i != tmp.size(); i++)
			serv->index.push_back(tmp[i]);
	}
	else if (key == "allow_methods")
	{
		std::vector<std::string> tmp = split(value, ' ');
		for (unsigned long i = 0; i != tmp.size(); i++)
			serv->allow_methods.push_back(ConfServer::strtoMethod(tmp[i]));
	}
	else if (key == "autoindex")
	{
		serv->autoindex = value == "on" ? true : false;
	}
	else if (key == "client_body_limit")
	{
		serv->client_body_limit = atoi(value.c_str());
	}
	else if (key == "recv_timeout")
	{
		serv->recv_timeout.tv_sec = atoi(value.c_str());
	}
	else if (key == "send_timeout")
	{
		serv->send_timeout.tv_sec = atoi(value.c_str());
	}
	else if (key == "return")
	{
		std::vector<std::string> tmp = split(value, ' ');
		serv->redirect_status = atoi(tmp[0].c_str());
		serv->redirect_url = tmp[1];
	}
	else if (key == "error_page")
	{
		std::vector<std::string> tmp = split(value, ' ');
		std::string path = tmp[tmp.size() - 1];
		for (unsigned long i = 0; i != tmp.size() - 1; i++)
		{
			int status_code = atoi(tmp[i].c_str());
			if (serv->error_pages.find(status_code) != serv->error_pages.end())
				continue;
			serv->error_pages[status_code] = path;
		}
	}
    else
    {
        return -1;
    }
    return 1;
}

ConfLoca Config::parse_location(size_t *i)
{

	ConfLoca result;
	size_t key_start;
	size_t value_end;

	size_t pre = content.find_first_not_of(" \t\n", *i);
	size_t cur = content.find_first_of(" \t\n", pre);
	result.path = content.substr(pre, cur - pre);

	pre = content.find_first_not_of(" \t\n", cur);
	if (pre == std::string::npos || content[pre] != '{')
	{
        std::cout << "[ERROR] config parsing failed." << std::endl;
        exit(1);
    }
		
	pre++;
	cur = content.find_first_not_of(" \t\n", pre);
	while (cur != std::string::npos)
	{
		if ((pre = content.find_first_not_of(" \t\n", cur)) == std::string::npos)
        {
            std::cout << "[ERROR] config parsing failed." << std::endl;
            exit(1);
        }
		key_start = pre;
		if ((cur = content.find_first_of(" \t\n", pre)) == std::string::npos)
        {
            std::cout << "[ERROR] config parsing failed." << std::endl;
            exit(1);
        }
		std::string key = content.substr(pre, cur - pre);
		if (key == "}")
		{
			*i = cur;
			break;
		}
		else
		{
			if ((pre = content.find_first_not_of(" \t\n", cur)) == std::string::npos)
            {
                std::cout << "[ERROR] config parsing failed." << std::endl;
                exit(1);
            }
			if ((cur = content.find_first_of("\n", pre)) == std::string::npos)
            {
                std::cout << "[ERROR] config parsing failed." << std::endl;
                exit(1);
            }
			if ((value_end = check_line_syntax(content.substr(key_start, cur - key_start))) == -1)
			{
                std::cout << "[ERROR] config parsing failed." << std::endl;
                exit(1);
            }
			if ((int)value_end == -2)
				continue;
			std::string value = content.substr(pre, value_end - pre + key_start + 1);
			if (setLocaValue(&result, key, value) == -1)
			{
                std::cout << "[ERROR] config parsing failed." << std::endl;
                exit(1);
            }
		}
	}
	return result;
}

int Config::setLocaValue(ConfLoca *loca, const std::string key, const std::string value)
{
    if (key == "root")
    {
        loca->root = value;
    }
    else if (key == "index")
    {

        /*
            std::vector<std::string> tmp = split(value, ' ');
		    for (unsigned long i = 0; i != tmp.size(); i++)
			    loca->index.push_back(tmp[i]);
        */
        std::string tmp;
        size_t pre = 0;
        size_t cur = value.find_first_of(" \t\n", pre);
        while (cur != std::string::npos)
        {
            tmp = value.substr(pre, cur - pre);
            loca->index.push_back(tmp);
            pre = value.find_first_not_of(" \t\n", cur);
            cur = value.find_first_of(" \t\n", pre);
        }
        tmp = value.substr(pre, cur - pre);
        loca->index.push_back(tmp);
    }
    else if (key == "allow_methods")
    {
        std::vector<std::string> tmp = split(value, ' ');
        for (unsigned long i = 0; i < tmp.size(); i++)
        {
            loca->allow_methods.push_back(ConfLoca::strtoMethod(tmp[i]));
        }
    }
    else if (key == "client_body_limit")
	{
		loca->client_body_limit = atoi(value.c_str());
	}
	else
	{
		return -1;
	}
    return 1;
}

int Config::check_line_syntax(std::string line)
{
    // remove the comments
    size_t sharp;
	sharp = line.find_first_of("#");
	if (sharp != std::string::npos)
	{
		line.erase(sharp);
		if (line.find_first_not_of(" \t\n") != std::string::npos)
			return -2;
	}

    // line must be end with semicolon
	size_t semicol;
	size_t find;

	semicol = line.find_first_of(";");
	if (semicol == std::string::npos)
		return -1;
	find = line.find_first_not_of(" \t\n", semicol + 1, line.length() - semicol - 1);
	if (find != std::string::npos)
		return -1;
	find = line.find_last_not_of(" \t", semicol - 1);
	return find;

}