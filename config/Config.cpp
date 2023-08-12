#include "../includes/config/Config.hpp"

Config::Config()
{}

int Config::openfile(const char* filename)
{
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
	return 0;
}


Config::~Config()
{}

std::vector<ConfServer> Config::parser(const char* filename)
{
	if (openfile(filename) == 1)
		return std::vector<ConfServer>();
    std::vector<ConfServer> result;

	size_t pre = 0;
	size_t cur = content.find_first_not_of(" \t\n", pre);
	if (cur == std::string::npos)
    {
        std::cout << RED <<"[ERROR] config parsing failed." << RESET << std::endl;
		exit(1);
    }
	size_t id = 0;
	while (cur != std::string::npos)
	{
		id += 1;
		pre = content.find_first_not_of(" \t\n", cur);;
		cur = content.find_first_of(" \t\n", pre);
		std::string key = content.substr(pre, cur - pre);
		if (key != "server")
        {
            std::cout << RED << "[ERROR] config parsing failed." << RESET << std::endl;
			exit(1);
        }
		ConfServer server = parse_server(&cur, id);
		result.push_back(server);
	}
	return result;
}

int Config::check_server_syntax(std::vector<ConfServer>& servers) {
    std::vector<ConfServer>::iterator it = servers.begin();
    std::vector<ConfServer>::iterator itn;
    for (; it != servers.end(); ++it) {
        itn = it;
        ++itn;
        for (; itn != servers.end(); ++itn) {
            if ((*itn).getPort() == (*it).getPort()) {
                std::cout << RED << "[ERROR] Config parsing failed. Port is not unique." << RESET << std::endl;
                exit(1);
            }
        }
    }
    return 1;
}


ConfServer Config::parse_server(size_t *t, size_t id)
{
    ConfServer result;
	size_t key_start;
	size_t value_end;
	size_t index = 0;

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
			if (!index) {
				std::cout << RED << "[ERROR] config parsing failed."<< RESET << std::endl;
				exit(1);
			}
			*t = content.find_first_not_of(" \n\t", cur + 1);
			break;
		}

		if (key == "location")
			result.locations.push_back(parse_location(&cur));
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
			if (setServValue(&result, key, value, id) == -1)
            {
                std::cout << "[ERROR] config parsing failed. **" << std::endl;
				exit(1);
            }
		}
		index++;
	}
	return result;
}

int	checkHost(std::string host)
{
	std::vector<std::string> tmp = split(host, '.');
	if (tmp.size() != 4  && tmp.size() != 1)
		return -1;
	if (tmp.size() == 4)
	{
		for (unsigned long i = 0; i != tmp.size(); i++)
		{
			if (tmp[i].length() > 3)
				return -1;
			for (unsigned long j = 0; j != tmp[i].length(); j++)
			{
				if (tmp[i][j] < '0' || tmp[i][j] > '9')
					return -1;
			}
		}
	}
	else
	{
		if (tmp[0] != "localhost")
			return -1;
	}
	return 1;
}

int Config::setServValue(ConfServer *serv, const std::string key, const std::string value, size_t id)
{

	serv->server_id = id;
	if (key == "server_name")
		serv->server_name = value;
	else if (key == "listen")
	{
		if (value.find_first_of(':') == std::string::npos)
		{
			if (checkHost(value) == -1)
			{
				std::cout << RED << "FAILED CONFIG: host(solo)" << RESET << std::endl;
				return -1;
			}
			serv->host = value;
			serv->port = "8080";
		}
		else
		{
			std::vector<std::string> tmp = split(value, ':');
			if (tmp[0] == "")
			{
				std::cout << RED << "FAILED CONFIG: Host required" << RESET << std::endl;
				return -1;
			}
			serv->host = tmp[0];
			if (checkHost(serv->host) == -1)
			{
				std::cout << RED << "FAILED CONFIG: Host(1st)" << RESET << std::endl;
				return -1;
			}
			if (isAllDigits(tmp[1]) || tmp[1] == "0")
				serv->port = tmp[1];
			else
			{
				std::cout << RED << "FAILED CONFIG: port(2nd)" << RESET << std::endl;
				return -1;
			}
		}
	}
	else if (key == "root")
		serv->root = value;
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
		serv->autoindex = value == "on" ? true : false;
	else if (key == "client_body_limit")
	{
		if (isAllDigits(value))
			serv->client_body_limit = atoi(value.c_str());
		else
		{
			std::cout << RED << "FAILED CONFIG: client_body_limit" << RESET << std::endl;
			return -1;
		}
	}
	else if (key == "recv_timeout")
		serv->recv_timeout.tv_sec = atoi(value.c_str());
	else if (key == "send_timeout")
		serv->send_timeout.tv_sec = atoi(value.c_str());
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
        return -1;
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
            std::cout << "[ERROR] config parsing failed. 1" << std::endl;
            exit(1);
        }
		key_start = pre;
		if ((cur = content.find_first_of(" \t\n", pre)) == std::string::npos)
        {
            std::cout << "[ERROR] config parsing failed. 2" << std::endl;
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
                std::cout << "[ERROR] config parsing failed. 3" << std::endl;
                exit(1);
            }
			if ((cur = content.find_first_of("\n", pre)) == std::string::npos)
            {
                std::cout << "[ERROR] config parsing failed. 4" << std::endl;
                exit(1);
            }
			if ((value_end = check_line_syntax(content.substr(key_start, cur - key_start))) == -1)
			{
                std::cout << "[ERROR] config parsing failed. 5" << std::endl;
                exit(1);
            }
			if ((int)value_end == -2)
				continue;
			std::string value = content.substr(pre, value_end - pre + key_start + 1);
			if (setLocaValue(&result, key, value) == -1)
			{
                std::cout << "[ERROR] config parsing failed. 6" << std::endl;
                exit(1);
            }
		}
	}

	return result;
}

int Config::setLocaValue(ConfLoca *loca, const std::string key, const std::string value)
{
    if (key == "root")
        loca->root = value;
	else if (key == "autoindex")
		loca->autoindex = (value == "on" ? ON : OFF);
	else if (key == "cgi_status")
		loca->cgi_status = (value == "on" ? true : false);
	else if (key == "upload_status")
		loca->upload_status = (value == "on" ? true : false);
    else if (key == "index")
    {
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
	else if (key == "error_page"){
		std::vector<std::string> tmp = split(value, ' ');
		std::string path = tmp[tmp.size() - 1];
		for (unsigned long i = 0; i != tmp.size() -1; i++){
			int status_code = atoi(tmp[i].c_str());
			if (loca->error_pages.find(status_code) != loca->error_pages.end())
				continue;
			loca->error_pages[status_code] = path;
		}
	}
    else if (key == "allow_methods")
    {
        std::vector<std::string> tmp = split(value, ' ');
        for (unsigned long i = 0; i < tmp.size(); i++)
            loca->allow_methods.push_back(ConfLoca::strtoMethod(tmp[i]));
    }
    else if (key == "client_body_limit")
		loca->client_body_limit = atoi(value.c_str());
	else if (key == "cgi_info")
	{
		int i;
		int pos = 0;
		std::vector<std::string> tmp = split(value, ' ');
		if (tmp.size() != 2)
			return -1;
		loca->cgi_infos[tmp[0]] = tmp[1];
	}
	else if (key == "return")
	{
		std::vector<std::string> tmp = split(value, ' ');
		loca->redirect_status = atoi(tmp[0].c_str());
		if (tmp.size() == 2)
			loca->redirect_url = tmp[1];
		else
			return -1;
	}
	else
		return -1;
    return 1;
}

int Config::check_line_syntax(std::string line)
{
    size_t sharp;
	sharp = line.find_first_of("#");
	if (sharp != std::string::npos)
	{
		line.erase(sharp);
		if (line.find_first_not_of(" \t\n") != std::string::npos)
			return -2;
	}
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