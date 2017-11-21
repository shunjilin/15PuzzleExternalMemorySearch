// Copyright 2017 Shunji Lin. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#ifndef WALL_TIMER_HPP
#define WALL_TIMER_HPP

// measures wall clock timings

#include <chrono>
#include <ostream>
using namespace std::chrono;

namespace utils {

    class WallTimer {
        bool stopped = false;
        steady_clock::time_point start_time;
        steady_clock::time_point end_time;
    public :
        // construction implictly starts timer
        WallTimer();

        void reset();

        bool is_stopped() const;

        void start();
  
        void stop();

        // get elapse time in seconds
        // if stopped, return stop - start
        // if not stopped but started, return now - start
        // otherwise returns 0
        duration<float>::rep get_seconds() const;
    };

    std::ostream &operator<<(std::ostream &os, const WallTimer &wall_timer);

    extern WallTimer overall_wall_timer;
}
#endif
