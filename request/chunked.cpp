#include "../includes/request/Request.hpp"

/*  all about CRLF  */
bool containsCRLF(const std::string& input) {
    size_t found = input.find("\r\n");
    return (found != std::string::npos);
}

size_t crflFinder(const std::string& input, size_t pos) {
    size_t found = input.find("\r\n", pos);
    return found;
}

/*  all about CR    */
bool containsCR(const std::string& input) {
    size_t found = input.find("\r");
    return (found != std::string::npos);
}

size_t posContainsCR(const std::string& input, size_t pos) {
    size_t found = input.find("\r", pos);
    return (found != std::string::npos);
}

size_t containsCarriage(const std::string& input) {
    size_t found = input.find("\r");
    return found;
}

/*  all about LF    */

size_t containsLF(const std::string& input) {
    size_t found = input.find("\n");
    return found;
}

/*  all about hexadecimal  */

size_t hextodec(const std::string& hex) {
    size_t result = 0;
    for (size_t i = 0; i < hex.length(); ++i) {
        if (hex[i] >= '0' && hex[i] <= '9') {
            result = result * 16 + (hex[i] - '0');
        }
        else if (hex[i] >= 'a' && hex[i] <= 'f') {
            result = result * 16 + (hex[i] - 'a' + 10);
        }
        else if (hex[i] >= 'A' && hex[i] <= 'F') {
            result = result * 16 + (hex[i] - 'A' + 10);
        }
    }
    return result;
}

long extractHexaCRFL(const std::string& input, size_t pos) {
    size_t found = input.find("\r\n", pos);
    std::string hexa = input.substr(pos, found - pos);
    return hextodec(hexa);
}

long extractHexaCR(const std::string& input, size_t pos) {
    size_t found = input.find("\r", pos);
    std::string hexa = input.substr(pos, found - pos);
    return hextodec(hexa);
}

/*  chunked function */

void Request::makeChunkedRequest(const std::string &str)
{
    size_t buffer_size = getChunkedSize();
	long bytes_size = str.size();
    if (_beforeHex)
    {
        if (_rest == 1)
        {
            std::string tmp = str.substr(1);
            _tmp_file.write(tmp.c_str(), tmp.size());
            set_rest_chunk(buffer_size - tmp.size());
            _beforeHex = false;
            _lineFeed = false;
            _carriageReturn = false;
            _rest = -1;
        }
        if (_rest == 2)
        {
            _beforeHex = false;
            _lineFeed = false;
            _carriageReturn = false;
            _rest = -1;
            goto normal_state;
        }
        if (_rest == 0)
        {
            if (!_hex.empty())
            {
                size_t crlf = crflFinder(str, 0);
                std::string tmp = str.substr(0, crlf);
                _hex.append(tmp);
                long hexa_value = hextodec(_hex);
                size_t size = str.size() - crlf - 2;
                std::string tmp2 = str.substr(crlf + 2, size);
                set_rest_chunk(hexa_value - size);
                _tmp_file.write(tmp2.c_str(), tmp2.size());
                _hex = "";
                _beforeHex = false;
                _lineFeed = false;
                _carriageReturn = false;
                _rest = -1;
            }
            else
            {
                size_t next_crlf = crflFinder(str, 0);
                long hexa = extractHexaCRFL(str, 0);
                std::string tmp = str.substr(next_crlf + 2);
                _tmp_file.write(tmp.c_str(), tmp.size());
                set_rest_chunk(hexa - tmp.size());
                _beforeHex = false;
                _lineFeed = false;
                _carriageReturn = false;
                _rest = -1;
            }
        }
        return;
    }

    if (buffer_size > REQUEST_BUFFER_SIZE)
    {
        normal_state:
        size_t new_chunk = buffer_size - bytes_size;
        set_rest_chunk(new_chunk);
        _tmp_file.write(str.c_str(), bytes_size);
    }
    else
    {
        std::string hexaDecimal;
        size_t crlf = str.find("\r\n", buffer_size - 1);
        if (crlf != std::string::npos)
        {
            _tmp_file.write(str.c_str(), buffer_size);
            size_t next_crlf = crflFinder(str, crlf + 2);
            if (next_crlf != std::string::npos)
            {
                long hexa_val = extractHexaCRFL(str, crlf + 1);
                if (hexa_val == 0)
                {
                    _bodyFlag = REQUEST_BODY_COMPLETED;
                    _tmp_file.close();
                    set_rest_chunk(0);
                    return;
                }
                else
                {
                    std::string tmp = str.substr(next_crlf + 2);
                    _tmp_file.write(tmp.c_str(), tmp.size());
                    set_rest_chunk(hexa_val - tmp.size());
                }
            }
            else 
            {
                size_t cr = str.find('\r', crlf + 1);
                size_t lf = str.find('\n', crlf + 2);
                if (cr != std::string::npos)
                {
                    if (lf != std::string::npos)
                    {
                        _rest = 2;
                        _beforeHex = false;
                        _carriageReturn = false;
                        _lineFeed = false;
                    }
                    else
                    {
                        std::cerr << BOLDGREEN  << str << RESET << std::endl;
                        _rest = 1;
                        _beforeHex = true;
                        _carriageReturn = false;
                    }
                    std::string hex = str.substr(crlf + 2, cr - crlf - 2);
                    long hexa_val = hextodec(hex);
                    set_rest_chunk(hexa_val);
                }
                else
                {
                    _beforeHex = true;
                    _carriageReturn = false;
                    _lineFeed = false;
                    _rest = 0;
                    _hex = "";
                    if (str.size() > crlf + 2)
                        _hex = str.substr(crlf + 2);
                }
            }
        }
    }
}
