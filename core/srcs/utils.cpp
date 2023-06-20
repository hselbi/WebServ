#include "../../includes/core/utils.hpp"

std::string int_to_string(long number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}
