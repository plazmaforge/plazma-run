#include <cstring>
#include "fslib.h"

#ifdef _WIN32
#else

#include <fnmatch.h>
#include <dirent.h>

#endif

////

#ifdef _WIN32
#else

static int match_file_nix(const char* pattern, const char* name, int mode);

static int match_file_nix(const char* pattern, const char* name);

#endif

static std::vector<std::string> _getFiles(const char* dirName, const char* pattern);

////

bool isWildcardChar(char ch) {
    return (ch == '*' || ch == '?' || ch == '[' || ch == ']');
}

int isWildcard(const char* pattern) {
    return getWildcardIndex(pattern) != -1;
}

int getWildcardIndex(const char* pattern) {
    if (pattern == NULL) {
        return -1;
    }
    int len = strlen(pattern);
    if (len == 0) {
        return -1;
    }
    char ch;
    for (int i = 0; i < len; i++) {
        ch = pattern[i];
        if (isWildcardChar(ch)) {
            return i;
        }
    }
    return -1;
}

// ../a/name*.text - > 4 -> '/'
int getWildcardPathIndex(const char* pattern, const char* fileName) {
    if (pattern == NULL || fileName == NULL) {
        return -1;
    }
    int wildcardIndex = getWildcardIndex(pattern);
    if (wildcardIndex < 0) {
        return -1;
    }
    return getWildcardPathIndex(wildcardIndex, fileName);
}

int getWildcardPathIndex(int wildcardIndex, const char* fileName) {
    if (fileName == NULL) {
        return -1;
    }
    int i = wildcardIndex;
    // go from end to start
    bool found = false;
    while (i >= 0) {
        if (fileName[i] == '\\' || fileName[i] == '/') {
            found = true;
            break;
        }
        i--;
    }
    return found ? i : -1;
}

std::vector<std::string> getFiles(const char* dirName) {
    return getFiles(dirName, NULL);
}

std::vector<std::string> getFiles(const char* dirName, const char* pattern) {
    return _getFiles(dirName, pattern);
}

////

static std::vector<std::string> _getFiles(const char* dirName, const char* pattern) {
    std::vector<std::string> result;
    if (dirName == NULL) {
        return result;
    }

    #ifdef _WIN32
    // TODO: Stub
    return result;
    #else

    /*
    filter_pattern = pattern;
    //char* dirname = "../";
    struct dirent **files;
	int n = scandir(dirName, &files, pattern ? filter_file_nix : NULL, alphasort);
    for (int i = 0; i < n; i++) {
        //printf("%s\n", files[i]->d_name);
        result.push_back(files[i]->d_name);
    }
    */

    DIR* dir = NULL;
    struct dirent *file;
    dir = opendir(dirName);
    if (dir == NULL) {
        return result;
    }
    while ((file = readdir(dir)) != NULL) {
        //printf("%s\n", file->d_name);
        if ((pattern == NULL || match_file_nix(pattern, file->d_name)) && file->d_type != DT_DIR) {
            result.push_back(file->d_name);
        }        
    }
    closedir(dir);
    #endif

    return result;
}

#ifdef _WIN32
#else

static int match_file_nix(const char* pattern, const char* name, int mode) {
    // PathMatchSpecA
    //printf(" %s %s\n", pattern, name);
    return fnmatch(pattern, name, FNM_PERIOD) == 0; // true id '0'
}

static int match_file_nix(const char* pattern, const char* name) {
    return match_file_nix(pattern, name, FNM_PERIOD);
}

#endif
