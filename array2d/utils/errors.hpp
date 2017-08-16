#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <stdexcept>
#include <string>

class IOException : public std::runtime_error
{
public:
    explicit IOException(const std::string& message) :
        std::runtime_error("IO Exception Raised: " + message) {}
};


struct OpenListEmpty {};

#endif
