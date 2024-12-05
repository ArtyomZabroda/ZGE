#include <utils.h>
#include <fstream>
#include <filesystem>

bool zge::GetBytesFromFile(const char* file_path, std::vector<std::byte>& bytes) {
  std::filesystem::path inputFilePath{file_path};
  auto length = std::filesystem::file_size(inputFilePath);
  if (length == 0) {
    return {};  // empty vector
  }
  bytes.resize(length);
  std::ifstream inputFile(file_path, std::ios_base::binary);
  if (!inputFile.is_open()) {
    return false;
  }
  inputFile.read(reinterpret_cast<char*>(bytes.data()), length);
  if (inputFile.bad() || inputFile.fail()) {
    inputFile.close();
    return false;
  }
  inputFile.close();
  return true;
}