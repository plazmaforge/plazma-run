#include <string>
#include <vector>

#ifdef _WIN32
#else

#include <fnmatch.h>
#include <dirent.h>

#endif

std::vector<std::string> getFiles(const char* dirName);

std::vector<std::string> getFiles(const char* dirName, const char* pattern);
