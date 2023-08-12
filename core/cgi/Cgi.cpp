#include "../../includes/core/Cgi.hpp"
#include "../../includes/core/Client.hpp"

Cgi::Cgi() : _cgi_bin(""), _cgi_script(""), _body(""), _envp(NULL), _argv(NULL), _ready_to_read_from_cgi(1), _cgi_status(0), _pid(0), _status(0) {}

size_t Cgi::_counter = 0;

Cgi::~Cgi() {}

void Cgi::reset()
{
	clean_env_vars();
	_cgi_bin.clear();
	_cgi_script.clear();
	_body.clear();
	_envp = NULL;
	_argv = NULL;
	_ready_to_read_from_cgi = 1;
	_cgi_status = 0;
	_pid = 0;
	_status = 0;
}

std::string Cgi::generate_cgi_output_file_name()
{
	std::string tmp_filename = "/tmp/cgi_output_" + std::to_string(time(NULL) + _counter++);
	return tmp_filename;
}

void Cgi::setClient(Client &client) { _client = &client; }

void Cgi::set_body(std::string payload) { _body = payload; }

void Cgi::set_cgi_bin(std::string cgi_bin) { _cgi_bin = cgi_bin; }

void Cgi::set_cgi_script(std::string cgi_script) { _cgi_script = cgi_script; }

int Cgi::get_pid() { return _pid; }

int Cgi::get_ready_to_read_from_cgi() { return _ready_to_read_from_cgi; }

void Cgi::set_ready_to_read_from_cgi(int ready_to_read_from_cgi) { _ready_to_read_from_cgi = ready_to_read_from_cgi; }

int Cgi::get_cgi_status() { return _cgi_status; }

void Cgi::set_cgi_status(int cgi_status_code) { _cgi_status = cgi_status_code; }

int Cgi::get_status() { return _status; }

void Cgi::set_status(int status) { _status = status; }

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

void Cgi::init_env_vars()
{
	std::string root = _client->get_response().getRoot();
	std::string path = _cgi_script;
	std::map<std::string, std::string> req_headers = _client->get_request().getHeaders();

	_env_vars["REMOTE_ADDR"] = req_headers["Host"];
	_env_vars["REMOTE_HOST"] = _client->get_server_block().getServerName();
	_env_vars["SERVER_SOFTWARE"] = "MortalKOMBAT/1.0";
	_env_vars["SERVER_NAME"] = _client->get_server_block().getServerName();
	_env_vars["SERVER_PORT"] = std::to_string(_client->get_server_block().getPort());
	_env_vars["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env_vars["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env_vars["REQUEST_METHOD"] = _client->get_request().getMethod();
	_env_vars["REQUEST_URI"] = _client->get_request().getPath();
	_env_vars["DOCUMENT_ROOT"] = root;
	_env_vars["SCRIPT_NAME"] = _client->get_request().getPath();
	_env_vars["SCRIPT_FILENAME"] = path;
	_env_vars["QUERY_STRING"] = _client->get_request().getQuery();
	_env_vars["PATH_TRANSLATED"] = path;
	_env_vars["REDIRECT_STATUS"] = "200";
	_env_vars["HTTP_HOST"] = req_headers["Host"];
	_env_vars["HTTP_REFERER"] = req_headers["Referer"];
	_env_vars["HTTP_USER_AGENT"] = req_headers["User-Agent"];
	_env_vars["HTTP_ACCEPT"] = req_headers["Accept"];
	_env_vars["HTTP_ACCEPT_LANGUAGE"] = req_headers["Accept-Language"];
	_env_vars["HTTP_ACCEPT_ENCODING"] = req_headers["Accept-Encoding"];
	_env_vars["HTTP_ACCEPT_CHARSET"] = req_headers["Accept-Charset"];
	_env_vars["HTTP_KEEP_ALIVE"] = req_headers["Keep-Alive"];
	_env_vars["HTTP_CONNECTION"] = req_headers["Connection"];
	_env_vars["HTTP_CACHE_CONTROL"] = req_headers["Cache-Control"];

	if (req_headers["Cookie"] != "")
		_env_vars["HTTP_COOKIE"] = req_headers["Cookie"];
	if (_client->get_request().getMethod() == "POST")
	{
		_env_vars["CONTENT_TYPE"] = req_headers["Content-Type"];
		_env_vars["CONTENT_LENGTH"] = req_headers["Content-Length"];
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

std::string Cgi::exec_cgi()
{
	std::string tmp_filename = generate_cgi_output_file_name();
	FILE *cgi_output_file = fopen(tmp_filename.c_str(), "w+");
	if (!cgi_output_file)
		return "-1";

	FILE *read_body_from_file = NULL;
	if (_client->get_request().getMethod() == "POST")
	{
		read_body_from_file = fopen(_client->get_request().getBodyFileName().c_str(), "r");
		if (!read_body_from_file)
			return "-1";
	}

	if ((_pid = fork()) == -1)
		return "-1";

	if (_pid == 0)
	{
		if (_client->get_request().getMethod() == "POST")
			dup2(fileno(read_body_from_file), 0);
		dup2(fileno(cgi_output_file), 1);

		char *newargv[] = {(char *)_cgi_bin.c_str(), (char *)_cgi_script.c_str(), NULL};
		execve(_cgi_bin.c_str(), newargv, _envp);
		exit(EXIT_FAILURE);
	}

	_ready_to_read_from_cgi = waitpid(_pid, &_status, WNOHANG); // if == 0, child process still running, if >-1 child process terminated
	fclose(cgi_output_file);
	if (read_body_from_file)
		fclose(read_body_from_file);
	return tmp_filename;
}

std::string Cgi::start_cgi(std::string binary, std::string script_path)
{
	set_cgi_bin(binary);
	set_cgi_script(script_path);
	init_env_vars();
	return exec_cgi();
}
