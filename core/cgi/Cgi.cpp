#include "Cgi.hpp"

Cgi::Cgi()
{
}

Cgi::~Cgi()
{
}

void Cgi::init_env_vars(Config &config)
{
	_cgi_env["REQUEST_METHOD"] = "GET";
	_cgi_env["CONTENT_TYPE"] = "text/html";
	_cgi_env["CONTENT_LENGTH"] = "0";
	_cgi_env["REQUEST_URI"] = "/index.php?name=anas&lastname=douib";
	_cgi_env["DOCUMENT_ROOT"] = "";
	// config.get_root();
	_cgi_env["SCRIPT_NAME"] = 	"index.php";
	_cgi_env["SCRIPT_FILENAME"] = "/usr/bin/php"; // document root + the script file name // config.get_root() + _cgi_env["SCRIPT_NAME"]; // `/var/www/html/myscript.php`
	_cgi_env["QUERY_STRING"] = "name=anas&lastname=douib";
	_cgi_env["PATH_INFO"] = "/usr/bin/php";
	// ...
}

void Cgi::exec()
{
	// fork and run the cgi script using execve and wait for it to finish and return the output to the client
	int pid;
	if ((pid = fork()) == -1)
	{
		std::cerr << "fork failed" << std::endl;
		exit(1);
	}
	if (pid == 0)
	{
		// child process
		// execve
		// return output to
		excve("/usr/bin/php", NULL, NULL);
	}
	else if (pid > 0)
	{

	}

}
