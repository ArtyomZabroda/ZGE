#ifndef CORE_UTILS_H_
#define CORE_UTILS_H_

#include <vector>

namespace zge {

bool GetBytesFromFile(const char* file_path, std::vector<std::byte>& bytes);

}

#endif