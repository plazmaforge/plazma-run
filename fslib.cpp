#include <cstring>
#include "fslib.h"

#ifdef _WIN32
#else

#include <fnmatch.h>
#include <dirent.h>
#include <errno.h>

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
    HANDLE dir = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW ent;
    
    char *path = dirName;
    len = strlen(dirName);
    wchar_t *wpath = NULL;

    int wlen = MultiByteToWideChar(CP_UTF8, 0, path, len, NULL, 0);
    wpath = malloc(siseof(wchar_t) * wlen);
    //std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, len, len, wpath, wlen);

    dir = FindFirstFileW(wpath, &ent);

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
    errno = 0;
    while ((file = readdir(dir)) != NULL) {
        //printf("%s\n", file->d_name);
        if ((pattern == NULL || match_file_nix(pattern, file->d_name)) && file->d_type != DT_DIR) {
            result.push_back(file->d_name);
        }        
    }
    if (errno != 0) {
        // TODO: stderr: error
    }
    closedir(dir);
    #endif

    return result;
}

#ifdef _WIN32
#else

bool isPatchChar(char c) {
    return c  == '\\' || c == '/';
}

int count_pattern_level(const char* pattern) {
    if (pattern == NULL || pattern[0] == '\0') {
        return 0;
    }
    int start = 0;
    if (isPatchChar(pattern[start])) {
        start++;
    }
    if (pattern[start] == '\0') {
        return 0;
    }
    char c;
    int i = start;
    int level = 1;
    int end = -1;

    // find start level
    while ((c = pattern[i]) != '\0') {
        if (pattern[i] == '\\' || pattern[i] == '/') {
            level++;
        }
        i++;
    }
    return level;
}

char* select_pattern(const char* pattern, int level) {
    if (pattern == NULL || pattern[0] == '\0' || level < 0) {
        return NULL;
    }
    int start = 0;
    if (isPatchChar(pattern[start])) {
        start++;
    }
    if (pattern[start] == '\0') {
        return NULL;
    }
    char c;
    int i = start;
    int curr_level = -1;
    int end = -1;

    // find start level
    while ((c = pattern[i]) != '\0') {
        if (pattern[i] == '\\' || pattern[i] == '/') {
            curr_level++;
            if (level - curr_level == 1) {
                start = i + 1;
            }
            if (curr_level == level) {
                end = i;
                break;
            }            
        }
        i++;
    }

    //printf("curr level: %d\n", curr_level);
    //printf("start path: %d\n", start);

    if (curr_level < 0) {
        // No level in pattern
        end = i;
    } else if (curr_level < level) {
        if (level - curr_level == 1) {
            end = i;
        } else {
            // Incorrect deep level in pattern
            return NULL;
        }
    }

    //printf("end path  : %d\n", end);

    if (start == end) {
        end++;
    }    

    char* p = (char*) malloc(end - start + 1);
    strncpy(p, pattern + start, end - start);
    return p;

}

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files, int level) {

    if (dirName == NULL) {
        return;
    }

    DIR* dir = NULL;
    struct dirent *file;
    dir = opendir(dirName);
    if (dir == NULL) {
        return;
    }

    int dir_len = strlen(dirName);
    int total_level = count_pattern_level(pattern);
    char* level_pattern = select_pattern(pattern, level);

    //printf("scandir : %s\n", dirName);
    //printf("total   : %d\n", total_level);
    //printf("level   : %d\n", level);
    //printf("pattern : %s\n", pattern);
    //printf("select  : %s\n", level_pattern);
    
    errno = 0;
    while ((file = readdir(dir)) != NULL) {
        //printf("%s\n", file->d_name);
        //printf("try [%d] %s, %s, :: %s\n", level, dirName, file->d_name, level_pattern);
        if (pattern == NULL || match_file_nix(level_pattern, file->d_name)) {

            int sep_len = (dirName[dir_len - 1] == '\\' || dirName[dir_len - 1] == '/') ? 0 : 1;

            // match        
            char* fullName = (char*) malloc(dir_len + sep_len + file->d_namlen + 1);
            strcpy(fullName, dirName);
            if (sep_len) {
                strcat(fullName, "/");
            }
            strcat(fullName, file->d_name);

            if (file->d_type != DT_DIR && (level == 0 || level == total_level - 1)) {
                // We add file from last pattern level only
                //printf("match: [%s] %s, %s, %s\n", (file->d_type == DT_DIR ? "D" : " "), fullName, dirName, file->d_name);
                files.push_back(fullName);
            }
 
            if (file->d_type == DT_DIR) {
                scandir(fullName, pattern, files, level + 1);
            }
        }        
    }
    if (errno != 0) {
        // TODO: stderr: error
    }
    closedir(dir);
    free(level_pattern);
}

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files) {
    scandir(dirName, pattern, files, 0);
}

static int match_file_nix(const char* pattern, const char* name, int mode) {
    // PathMatchSpecA
    int val = fnmatch(pattern, name, FNM_PERIOD);
    int res = val == 0;
    //printf(" %s -> %s, %d, %d\n", pattern, name, val, res);
    //return fnmatch(pattern, name, FNM_PERIOD) == 0; // true id '0'
    return res;
}

static int match_file_nix(const char* pattern, const char* name) {
    return match_file_nix(pattern, name, FNM_PERIOD);
}

#endif
