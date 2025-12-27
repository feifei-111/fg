#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>

#include <whereami.h>

#include <fg/macros.h>
#include <fg/utils/utils.h>
#include "fg/utils/log.h"

namespace fg::utils {

/* ====================================================================== */
/* =============================== time ================================= */
/* ====================================================================== */
using SysTime = std::chrono::steady_clock::time_point;

// ratio<1> 表示用秒作为单位
template <typename T>
using TimeDura = std::chrono::duration<T, std::ratio<1>>;

namespace {
SysTime GetBaseTime() {
    static SysTime base_time = std::chrono::steady_clock::now();
    return base_time;
}
}  // namespace

template <typename T>
T GetTime() {
    // 先构造这个，免得出现 time<0 的时间
    SysTime base_time = GetBaseTime();
    TimeDura<T> duration = std::chrono::steady_clock::now() - base_time;
    return duration.count();
}

template FG_API float GetTime<float>();

template FG_API double GetTime<double>();

/* ====================================================================== */
/* ============================= glm str ================================ */
/* ====================================================================== */

const std::string ToString(const glm::vec3& vec) {
    std::stringstream ss;
    ss << "glm::vec3 [" << vec.x << ", " << vec.y << ", " << vec.z << "]";
    return ss.str();
}

const std::string ToString(const glm::mat4& mat) {
    std::stringstream ss;
    ss << "glm::mat4 (4x4) (column major)\n";
    ss << "[";

    ss << "[" << std::fixed << std::setprecision(4) << mat[0][0] << ", "
       << std::fixed << std::setprecision(4) << mat[1][0] << ", " << std::fixed
       << std::setprecision(4) << mat[2][0] << ", " << std::fixed
       << std::setprecision(4) << mat[3][0] << "],\n";

    ss << " [" << std::fixed << std::setprecision(4) << mat[0][1] << ", "
       << std::fixed << std::setprecision(4) << mat[1][1] << ", " << std::fixed
       << std::setprecision(4) << mat[2][1] << ", " << std::fixed
       << std::setprecision(4) << mat[3][1] << "],\n";

    ss << " [" << std::fixed << std::setprecision(4) << mat[0][2] << ", "
       << std::fixed << std::setprecision(4) << mat[1][2] << ", " << std::fixed
       << std::setprecision(4) << mat[2][2] << ", " << std::fixed
       << std::setprecision(4) << mat[3][2] << "],\n";

    ss << " [" << std::fixed << std::setprecision(4) << mat[0][3] << ", "
       << std::fixed << std::setprecision(4) << mat[1][3] << ", " << std::fixed
       << std::setprecision(4) << mat[2][3] << ", " << std::fixed
       << std::setprecision(4) << mat[3][3] << "]";

    ss << "]";
    return ss.str();
}

/* ====================================================================== */
/* ============================== path ================================== */
/* ====================================================================== */

const std::string GetRuntimeAbsPath(const std::string& relative_path) {
    char buffer[512] = {0};
    int success =
      wai_getExecutablePath(buffer, 512, NULL);  // 一个 c 库，所以用 NULL

    std::filesystem::path exe_path = std::filesystem::canonical(buffer);
    std::filesystem::path exe_dir = exe_path.parent_path();
    std::filesystem::path abs_path = exe_dir / relative_path;

    return std::filesystem::canonical(abs_path).string();
}
}  // namespace fg::utils
