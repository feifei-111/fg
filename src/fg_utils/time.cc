#include <chrono>
#include "fg_macros.h"

namespace fg_utils{

using SysTime = std::chrono::system_clock::time_point;
template<typename T>
using TimeDura = std::chrono::duration<T>;

static SysTime GetBaseTime(){
    static SysTime base_time = std::chrono::system_clock::now();
    return base_time;
}

template<typename T>
T GetTime(){
    // 先构造这个，免得出现 time<0 的时间
    SysTime base_time = GetBaseTime();
    TimeDura<T> duration = std::chrono::system_clock::now() - base_time;
    return duration.count();
}

template
FG_API float GetTime<float>();

template
FG_API double GetTime<double>();


}
