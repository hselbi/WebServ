#include "includes/Utils.hpp"

std::string Utils::toString(int value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

std::string Utils::ResponseHeaderToString(const t_responseHeader &responseHeader)
{
	std::stringstream ss;

	ss << "HTTP/1.1 " << responseHeader.statusCode << " " << responseHeader.statusMessage << "\r\n";

	std::map<std::string, std::string>::const_iterator it;
	for (it = responseHeader.headers.begin(); it != responseHeader.headers.end(); ++it)
	{
		ss << it->first << ": " << it->second << "\r\n";
	}
	ss << "\r\n";

	return ss.str();
}
