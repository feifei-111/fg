#pragma once
#include <glm/glm.hpp>
#include <string>

#include <fg/macros.h>

namespace fg::utils {

template <typename T = float>
T FG_API GetTime();

const std::string FG_API ToString(const glm::vec3& vec);
const std::string FG_API ToString(const glm::mat4& mat);

// executable 和资源放在一起，用快捷方式触发
// 由于相对路径是基于当前命令行路径的，所以不得不去找到绝对路径执行
// 也就是说，需要一个 runtime 获取绝对路径的接口
const std::string FG_API AbsPath(const std::string& relative_path);

}  // namespace fg::utils
