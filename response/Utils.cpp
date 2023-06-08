#include "includes/Utils.hpp"

std::string Utils::toString(int value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}