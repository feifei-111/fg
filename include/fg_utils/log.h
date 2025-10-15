#pragma once
#ifdef USE_GLOG

#include <glog/logging.h>

#else
struct DummyLog {
    template <typename T>
    DummyLog& operator<<(const T&) {
    return *this;
    }
};
 
#define VLOG(verbose_level) DummyLog()
#define CHECK_EQ(a, b) DummyLog()

#endif