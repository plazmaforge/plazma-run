#ifndef PLAZMA_LIB_FSLIB_H
#define PLAZMA_LIB_FSLIB_H

#include <string>
#include <vector>

/*
 Returns full file path 
 */
char* getFilePath(const char* dirName, const char* fileName);

std::string getCurrentFindPath();

std::vector<std::string> getFiles(const char* dirName);

std::vector<std::string> getFiles(const char* dirName, const char* pattern);

char* getRealPath(const char* path);

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files);

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files, int level);

#endif // PLAZMA_LIB_FSLIB_H