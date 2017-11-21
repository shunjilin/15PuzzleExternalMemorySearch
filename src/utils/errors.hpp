// Copyright 2017 Shunji Lin. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
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
