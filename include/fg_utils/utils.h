#pragma once
#include "fg_utils/log.h"
#include "fg_utils/time.h"

namespace fg_utils {
#ifdef _WIN32
#include "fg_macros.h"
FG_API void CreateDebugConsole();
#endif
}  // namespace fg_utils
