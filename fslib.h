#include <string>
#include <vector>

bool isWildcardChar(char ch);

int isWildcard(const char* pattern);

int getWildcardIndex(const char* pattern);

// ../a/name*.text - > 4 -> '/'
int getWildcardPathIndex(const char* pattern, const char* fileName);

int getWildcardPathIndex(int wildcardIndex, const char* fileName);

std::vector<std::string> getFiles(const char* dirName);

std::vector<std::string> getFiles(const char* dirName, const char* pattern);

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files);

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files, int level);