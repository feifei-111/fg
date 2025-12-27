#pragma once
#include <string>
#include <glm/glm.hpp>

namespace fg::utils{

template <typename T = float>
T GetTime();

const std::string ToString(const glm::vec3& vec);
const std::string ToString(const glm::mat4& mat);

// executable 和资源放在一起，用快捷方式触发
// 由于相对路径是基于当前命令行路径的，所以不得不去找到绝对路径执行
// 也就是说，需要一个 runtime 获取绝对路径的接口
const std::string GetRuntimeAbsPath(const std::string& relative_path);

// 这个感觉还是放在外面比较好，因为这个逻辑和 window 无关
#ifdef _WIN32
namespace win32 {
void CreateConsole();
}
#endif
}
