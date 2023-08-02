#include "../../includes/core/Cgi.hpp"
#include "../../includes/core/Client.hpp"
Cgi::Cgi() : _cgi_bin(""), _cgi_script(""), _body(""), _cgi_output_file(NULL), _envp(NULL), _argv(NULL), _extension(""), _ready_to_read_from_cgi(1), _cgi_status(0), _start_time(0), _pid(0), _status(0) {}

size_t Cgi::_counter = 0;

Cgi::~Cgi() {}

std::string Cgi::start_cgi(std::string binary, std::string script_path)
{
	set_cgi_bin(binary);
	set_cgi_script(script_path);
	init_env_vars();
	return exec_cgi();
}


std::string Cgi::exec_cgi() // !! upload handiinng
{
	int write_to_cgi[2];

	std::string tmp_filename = "/tmp/cgi_output_" + std::to_string(time(NULL) + _counter++);
	FILE *_cgi_output_file = fopen(tmp_filename.c_str(), "w+");

	if (!_cgi_output_file)
	{
		std::cout << "Failed to open the file! 2222" << std::endl;
		return "-1";
	}

	if (pipe(write_to_cgi) == -1)
	{
		return "-1";
	}
	set_body(_client->get_request().getBody());
	_start_time = time(NULL);

	if ((_pid = fork()) == -1)
		return "-1";

	if (_pid == 0)
	{
		close(write_to_cgi[1]);
		dup2(write_to_cgi[0], 0);
		dup2(fileno(_cgi_output_file), 1);
		close(write_to_cgi[0]);

		char *newargv[] = {(char *)_cgi_bin.c_str(), (char *)_cgi_script.c_str(), NULL};
		execve(_cgi_bin.c_str(), newargv, _envp);
		exit(EXIT_FAILURE);
	}
	else
	{
		close(write_to_cgi[0]);
		write(write_to_cgi[1], _body.c_str(), _body.length());
		close(write_to_cgi[1]);
		_ready_to_read_from_cgi = waitpid(_pid, &_status, WNOHANG); // if result == 0, child process still running, if result == -1, error, else child process terminated
		 // 0 means parent  process not ready to read from cgi
		 // not 0 means parent process ready to read from cgi
	}

	fclose(_cgi_output_file);
	return tmp_filename;
}


void Cgi::init_env_vars()
{
	std::string root = _client->get_response().getRoot();
	std::string path = _cgi_script;
	_env_vars["REMOTE_ADDR"] =  _client->get_request().getHeaders()["Host"];
	_env_vars["REMOTE_HOST"] = _client->get_server_block().getServerName();
	_env_vars["SERVER_SOFTWARE"] = "MortalKOMBAT/1.0";
	_env_vars["SERVER_NAME"] = _client->get_server_block().getServerName();
	_env_vars["SERVER_PORT"] = std::to_string(_client->get_server_block().getPort());
	_env_vars["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env_vars["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env_vars["REQUEST_METHOD"] = _client->get_request().getMethod();
	_env_vars["REQUEST_URI"] = path;
	_env_vars["DOCUMENT_ROOT"] = root;
	_env_vars["SCRIPT_NAME"] = path;
	_env_vars["SCRIPT_FILENAME"] = _env_vars["SCRIPT_NAME"];
	_env_vars["QUERY_STRING"] = _client->get_request().getQuery();
	_env_vars["PATH_INFO"] = path;
	_env_vars["PATH_TRANSLATED"] = path;
	_env_vars["REDIRECT_STATUS"] = "200";
	if (_client->get_request().getHeaders()["Cookie"] != "")
		_env_vars["HTTP_COOKIE"] = _client->get_request().getHeaders()["Cookie"];
	if (_client->get_request().getMethod() == "POST")
	{
		_env_vars["CONTENT_TYPE"] = _client->get_request().getHeaders()["Content-Type"];
		_env_vars["CONTENT_LENGTH"] = _client->get_request().getHeaders()["Content-Length"];
	}

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
	strcpy(_argv[0], _cgi_bin.c_str());
	strcpy(_argv[1], _cgi_script.c_str());
	_argv[2] = NULL;
}

void Cgi::setClient(Client &client) { _client = &client; }

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

int Cgi::get_pid()
{
	return _pid;
}

void Cgi::reset()
{
	// clean_env_vars();
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
	_status = 0;
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

int Cgi::get_status()
{
	return _status;
}

void Cgi::set_status(int status)
{
	_status = status;
}

/*
cookies

*/

// int Cgi::exec_cgi() // !! upload handling
// {
// }

/**/
