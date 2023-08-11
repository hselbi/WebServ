#ifndef TOOLS_HPP
#define TOOLS_HPP

#include "../includes.hpp"

int		replace(std::string &original, std::string word1, std::string word2);
std::vector<std::string> split(std::string input, char delimiter);
size_t	StringToHexNumber(std::string input);
bool isAllDigits(const std::string& str);

template<typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &op)
{
	if (op.size() == 0)
	{
		out << "(empty)";
		return out;
	}
	for (unsigned long i = 0; i < op.size(); i++)
		out << op[i] << "(" << i << ") ";
	return out;
}

template<typename T1, typename T2>
std::ostream &operator<<(std::ostream &out, const std::map<T1, T2> &op)
{
	if (op.size() == 0)
	{
		out << "(empty)";
		return out;
	}

	for (typename std::map<T1, T2>::const_iterator i = op.begin(); i != op.end(); i++)
		out << "[ " << (*i).first << " ] = " << (*i).second << "\n";
	return out;
}

#endif