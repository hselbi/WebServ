#include "../includes/config/Tools.hpp"

int replace(std::string &original, std::string word1, std::string word2)
{
	unsigned long pos = original.find(word1);
	int result = 0;
	while (pos != std::string::npos)
	{
		result = 1;
		original = original.replace(pos, word1.length(), word2);
		pos = original.find(word1);
	}
	return result;
}

std::vector<std::string> split(std::string input, char delimiter)
{
    std::vector<std::string> answer;
    std::stringstream ss(input);
    std::string temp;
 
    while (getline(ss, temp, delimiter))
        answer.push_back(temp);
 
    return answer;
}

size_t StringToHexNumber(std::string input)
{
	std::stringstream convert;
	size_t ret = 0;
	convert << std::hex << input;
	convert >> ret;
	return ret;
}

bool isAllDigits(const std::string& str) {
    for (int i = 0; i < str.length(); i++) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}
