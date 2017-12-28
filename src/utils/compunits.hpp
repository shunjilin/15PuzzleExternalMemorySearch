// Copyright 2017 Shunji Lin. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#include <cstddef>

using std::size_t;

namespace compunits
{
    constexpr size_t operator "" _B(unsigned long long const size)
    {
        return size;
    }
    constexpr size_t operator "" _KiB(unsigned long long const size)
    {
        return size * 1024;
    }
    constexpr size_t operator "" _MiB(unsigned long long const size)
    {
        return size * 1024 * 1024;
    }
    constexpr size_t operator "" _GiB(unsigned long long const size)
    {
        return size * 1024 * 1024 * 1024;
    }
}
