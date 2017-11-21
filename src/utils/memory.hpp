// Copyright 2017 Shunji Lin. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <memory>

namespace memory {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}

#endif
