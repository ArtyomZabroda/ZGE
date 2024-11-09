#include <Windows.h>
#include <string>
#include <vector>
#include <cstddef>

namespace zge {
  extern int Main(int argc, char** argv);

  static std::string ConvertFromWString(const WCHAR* wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, (int)wcslen(wstr), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr, (int)wcslen(wstr), &str[0], size_needed, NULL, NULL);
    return str;
  }
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  int argc;
  WCHAR** wargv = CommandLineToArgvW(pCmdLine, &argc); // wide argv, we need to convert it to multibyte argv. This function allocates memory.
  std::vector<std::string> argv_string_vec;
  std::vector<char*> argv_vec;
  for (std::size_t i{}; i < argc; ++i) {
    argv_string_vec.emplace_back(zge::ConvertFromWString(wargv[i]));
  }
  for (auto& string : argv_string_vec)
    argv_vec.push_back(&string[0]);
  LocalFree(wargv); // now we free an allocated memory

  return zge::Main(argc, argv_vec.data());
}