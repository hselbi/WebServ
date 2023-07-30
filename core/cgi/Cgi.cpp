#include "../../includes/core/Cgi.hpp"
#include "../../includes/core/Client.hpp"

Cgi::Cgi() : _cgi_bin(""), _cgi_script(""), _body(""), _cgi_output_file(NULL), _envp(NULL), _argv(NULL), _extension("")
{
	_ready_to_read_from_cgi = 1;
	_cgi_status = 0;
	_start_time = 0;
	_pid = 0;
}

Cgi::~Cgi()
{
}

void Cgi::setClient(Client &client)
{
	this->_client = &client;
}

int Cgi::start_cgi(std::string script_path)
{
	// std::cout << "start_cgi" << std::endl;
	set_cgi_bin("/usr/bin/php");
	set_cgi_script(script_path);
	init_env_vars();
	// std::map<std::string, std::string>::iterator it = _env_vars.begin();
	// for (; it != _env_vars.end(); ++it)
	// {
	// 	std::cout << it->first << ": " << it->second << std::endl;
	// }
	return exec_cgi();
}

int Cgi::exec_cgi() // !! upload handiinng
{
	pid_t _pid;
	int write_to_cgi[2];

	_cgi_output_file = tmpfile();
	if (!_cgi_output_file)
	{
		std::cerr << "tmpfile failed" << std::endl; return -1;
	}

	if(pipe(write_to_cgi) == -1)
	{
		std::cerr << "pipe failed" << std::endl; return -1;
	}

// !!!!

	set_body(_client->get_request().getBody());
	std::cout << _client->get_request().getBody() << std::endl;
	std::cout << "-----------------------\n";
	std::cout << _client->get_request().getBody().length() << std::endl;

	write(write_to_cgi[1], _body.c_str(), _body.length()); // write to pipe
	sleep(2);
	_start_time = time(NULL);


// !!!


	if ((_pid = fork()) == -1)
	{
		std::cerr << "fork failed" << std::endl; return -1;
	}


	if (_pid == 0)
	{
		close(write_to_cgi[1]);

		dup2(write_to_cgi[0], 0); // read from pipe
		dup2(fileno(_cgi_output_file), 1);

		close(write_to_cgi[0]);
		char *newargv[] = {(char*)"/usr/bin/php", (char*)_cgi_script.c_str(), NULL};
		execve("/usr/bin/php", newargv, _envp);
		std::cout << "execve failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		close(write_to_cgi[0]);
		close(write_to_cgi[1]);

		// !! check if child process terminated∆
		// sleep(3);
        _ready_to_read_from_cgi = waitpid(_pid, 0, WNOHANG);
		// std::cout << "_ready_to_read_from_cgi " << _ready_to_read_from_cgi << std::endl;
		// !!! if result == 0, child process still running, if result == -1, error, else child process terminated

		lseek(fileno(_cgi_output_file), 0, SEEK_SET);
	}

	return fileno(_cgi_output_file);
}

void Cgi::set_body(std::string payload)
{
	_body = payload;
}

void Cgi::set_cgi_bin(std::string cgi_bin)
{
	_cgi_bin = cgi_bin;
}

void Cgi::set_cgi_script(std::string cgi_script)
{
	_cgi_script = cgi_script;
}

// /board/time/index.php, find last occurence of / and get what is after it
std::string Cgi::get_cgi_script_name(std::string path)
{
	std::string script_name;
	size_t pos = path.find_last_of('/');
	if (pos != std::string::npos)
		script_name = path.substr(pos + 1);
	return script_name;
}

int Cgi::get_pid()
{
	return _pid;
}

void Cgi::reset()
{
	// !!! clean_env_vars();
	_cgi_bin.clear();
	_cgi_script.clear();
	_body.clear();
	_cgi_output_file = NULL;
	_envp = NULL;
	_argv = NULL;
	_ready_to_read_from_cgi = 1;
	_cgi_status = 0;
	_start_time = 0;
	_pid = 0;

}

int Cgi::get_ready_to_read_from_cgi()
{
	return _ready_to_read_from_cgi;
}

void Cgi::set_ready_to_read_from_cgi(int ready_to_read_from_cgi)
{
	_ready_to_read_from_cgi = ready_to_read_from_cgi;
}

int Cgi::get_cgi_status()
{
	return _cgi_status;
}

void Cgi::set_cgi_status(int cgi_status_code)
{
	_cgi_status = cgi_status_code;
}
std::string Cgi::get_path_info(std::string url)
{
	std::string path_info;
	long len = _env_vars["SCRIPT_NAME"].length();
	size_t pos = url.find(_env_vars["SCRIPT_NAME"]);
	if (pos != std::string::npos)
		path_info = url.substr(pos + len);
	return path_info;
}

void Cgi::init_env_vars()
{
	std::string root = _client->get_response().getRoot();

	_env_vars["SERVER_SOFTWARE"] = "MortalKOMBAT/1.0";
	_env_vars["SERVER_NAME"] = _client->get_server_block().getServerName();
	_env_vars["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env_vars["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env_vars["SERVER_PORT"] = std::to_string(_client->get_server_block().getPort());
	_env_vars["REQUEST_METHOD"] = _client->get_request().getMethod();
	_env_vars["REQUEST_URI"] = _client->get_request().getPath();
	_env_vars["DOCUMENT_ROOT"] = root;
	_env_vars["SCRIPT_NAME"] = _cgi_script.c_str();
	_env_vars["SCRIPT_FILENAME"] = _env_vars["DOCUMENT_ROOT"] + "/" + _env_vars["SCRIPT_NAME"];
	_env_vars["QUERY_STRING"] = _client->get_request().getQuery();
	_env_vars["PATH_INFO"] = get_path_info(_client->get_request().getPath());

	_envp = new char *[_env_vars.size() + 1];
	int i = 0;
	for (env_vars_t::iterator it = _env_vars.begin(); it != _env_vars.end(); ++it, ++i)
	{
		std::string env_var = it->first + "=" + it->second;
		_envp[i] = new char[env_var.length() + 1];
		strcpy(_envp[i], env_var.c_str());
	}
	_envp[i] = NULL;

	_argv = new char *[3];
	_argv[0] = new char[_cgi_bin.length() + 1];
	_argv[1] = new char[_cgi_script.length() + 1];
	// strcpy(_argv[0], _cgi_bin.c_str());
	strcpy(_argv[0], _cgi_bin.c_str());
	strcpy(_argv[1], _cgi_script.c_str());
	_argv[2] = NULL;
}

void Cgi::clean_env_vars()
{
	for (int i = 0; _envp[i]; ++i)
		delete[] _envp[i];
	delete[] _envp;

	for (int i = 0; _argv[i]; ++i)
		delete[] _argv[i];
	delete[] _argv;

	_env_vars.clear();
}


int Cgi::get_start_time()
{
	return _start_time;
}


/*

cgi status cdoe excve
*/
