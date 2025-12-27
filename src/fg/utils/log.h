#pragma once

// 不把 log 暴露出去，否则会依赖 glog
#include <glog/logging.h>

#ifndef USE_VLOG

struct DummyLog {
    template <typename T>
    DummyLog& operator<<(const T&) {
        return *this;
    }
};

#define VLOG(verbose_level) DummyLog()
#define VLOG_IS_ON(x) false

#endif
