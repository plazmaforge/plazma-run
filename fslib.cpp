#include <string.h>

#ifdef _WIN32

#include <wchar.h>
#include <windows.h>

//#include <shlwapi.h>

//#include <winioctl.h>
//#include <direct.h>
//#include <shlobj.h>
//#include <shlwapi.h>

#else

#include <fnmatch.h>
#include <dirent.h>
#include <errno.h>

#endif

#include "fslib.h"

////

#ifdef _WIN32

static int match_file_win(const char* pattern, const char* name, int mode);

static int match_file_win(const char* pattern, const char* name);

#else

static int match_file_nix(const char* pattern, const char* name, int mode);

static int match_file_nix(const char* pattern, const char* name);

#endif

static std::vector<std::string> _getFiles(const char* dirName, const char* pattern);

////

bool isPatchChar(char ch) {
    return ch  == '\\' || ch == '/';
}

int countPathLevel(const char* path) {
    if (path == NULL || path[0] == '\0') {
        return 0;
    }
    int start = 0;
    if (isPatchChar(path[start])) {
        start++;
    }
    if (path[start] == '\0') {
        return 0;
    }
    char ch;
    int i = start;
    int level = 1;
    int end = -1;

    // find start level
    while ((ch = path[i]) != '\0') {
        if (isPatchChar(ch)) {
            level++;
        }
        i++;
    }
    return level;
}

char* getLevelPath(const char* path, int level) {
    if (path == NULL || path[0] == '\0' || level < 0) {
        return NULL;
    }
    int start = 0;
    if (isPatchChar(path[start])) {
        start++;
    }
    if (path[start] == '\0') {
        return NULL;
    }
    char ch;
    int i = start;
    int curr_level = -1;
    int end = -1;

    // find start level
    while ((ch = path[i]) != '\0') {
        if (isPatchChar(ch)) {
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

    int len = end - start;
    char* p = (char*) malloc(len + 1);
    strncpy(p, path + start, len);
    p[len] = '\0';
    return p;

}

char* getFilePath(const char* dirName, const char* fileName) {
    if (dirName == NULL && fileName == NULL) {
        return NULL;
    }
    if (dirName == NULL) {
        return strdup(fileName);
    }
    if (fileName == NULL) {
        return strdup(dirName);
    }

    int len1 = strlen(dirName);
    if (len1 == 0) {
        return strdup(fileName);
    }

    int len2 = strlen(fileName);
    if (len2 == 0) {
        return strdup(dirName);
    }

    int sep_len = 0;
    if (isPatchChar(dirName[len1 - 1])) {
        sep_len++;
    }
    if (isPatchChar(fileName[0])) {
        sep_len++;
    }
    if (sep_len == 2) {
        sep_len = -1; // erase 1 position
    } else if (sep_len == 1) {
        sep_len = 0;  // nothing
    } else {
        sep_len = 1;  // add 1 position between dir and file
    }

    int len = len1 + sep_len + len2;

    char* path = (char*) malloc(len + 1);
    strcpy(path, dirName);
    if (sep_len == 1) {
        path[len1] = '/';
        path[len1 + 1] = '\0'; // ???: Maybe for next strcat
        //strcat(path, "/");
    }
    // shift fileName if erase 1 position
    strcat(path, sep_len == -1 ? fileName + 1 : fileName);
    path[len] = '\0';

    return path;
}

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
    for (int i = 0; i < len; i++) {
        if (isWildcardChar(pattern[i])) {
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
        if (isPatchChar(fileName[i])) {
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

    /*
    // TODO: Stub
    HANDLE dir = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW ent;
    
    char *path = dirName;
    len = strlen(dirName);
    wchar_t *wpath = NULL;

    int wlen = MultiByteToWideChar(CP_UTF8, 0, path, len, NULL, 0);
    wpath = malloc(siseof(wchar_t) * wlen);
    MultiByteToWideChar(CP_UTF8, 0, len, len, wpath, wlen);
    dir = FindFirstFileW(wpath, &ent);
    */

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

wchar_t* char2wchar(const char* str, int len) {    
    int wlen = MultiByteToWideChar(CP_UTF8, 0, str, len, NULL, 0);
    wchar_t* wstr = (wchar_t*) malloc(sizeof(wchar_t) * wlen + 1);
    MultiByteToWideChar(CP_UTF8, 0, str, len, wstr, wlen);
    wstr[wlen] = '\0';
    return wstr;
}

wchar_t* char2wchar(const char* str) {    
    return char2wchar(str, strlen(str));

}

char* wchar2char(const wchar_t* wstr, int wlen) {    
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, NULL, 0, NULL, NULL);
    char* str = (char*) malloc(sizeof(char) * len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    str[len] = '\0';
    return str;
}

char* wchar2char(const wchar_t* wstr) {
    return wchar2char(wstr, wcslen(wstr)); 
}

bool isDir(WIN32_FIND_DATAW file) {
    return file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

std::string getCurrentFindPath() {
    return "./*"; // Why not '.\*'?
}

bool isCurrentFindPath(const char* path) {
    if (path == NULL) {
        return false;
    }
    return strcmp(path, "./*") == 0; // Why not '.\*'?

}

// Convert directory name to WIN32 find path: add '\*'
char* getFindPath(const char* dirName) {
    if (dirName == NULL) {
        return NULL;
    }

    int len = strlen(dirName);
    int add = 0;

    if (len == 0) {
        add = 3;
    } else {
        if (dirName[len - 1] == '\\') {
            add = 1;
        }
    }

    len += add;
    char* path = (char*) malloc(len + 1);

    if (add == 3) {
        strcpy(path, "./*"); // Why not '.\*'?
    } else {
        strcpy(path, dirName);
        if (add == 1) {
           strcat(path, "*");
        }
    }
    
    path[len + 1] = '\0';
    return path;
}

// https://github.com/Quintus/pathie-cpp/blob/master/src/path.cpp

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files, int level) {

    if (dirName == NULL) {
        return;
    }
       
    char* path = getFindPath(dirName); // convert 'dirName' to WIN32 find path: add '\*'
    wchar_t* wpath = char2wchar(path);

    //printf("path    : '%s'\n", path);

    HANDLE dir = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW file;

    dir = FindFirstFileW(wpath, &file);
    if (dir == INVALID_HANDLE_VALUE && GetLastError() != ERROR_FILE_NOT_FOUND) {
        fprintf(stderr, "Directory not found: %s", dirName);
        return;
    }

    int total_level = countPathLevel(pattern); // count path level in pattern
    char* level_pattern = getLevelPath(pattern, level);

    //printf("scandir : '%s'\n", dirName);
    //printf("total   : %d\n", total_level);
    //printf("level   : %d\n", level);
    //printf("pattern : %s\n", pattern);
    //printf("select  : %s\n", level_pattern);
                                                                                         
    while (FindNextFileW(dir, &file) != 0) {

        wchar_t* wfileName = file.cFileName;
        char* fileName = wchar2char(wfileName);

        //printf("try [%d] %s, %s, :: %s\n", level, dirName, fileName, level_pattern);
        if (pattern == NULL || match_file_win(level_pattern, fileName)) {

            char* fullName = NULL;
            if (isCurrentFindPath(dirName)) {
               fullName = strdup(fileName);
            } else {
               fullName = getFilePath(dirName, fileName);
            }            
            //printf("match:fullName: %s\n", fullName);

            if (!isDir(file) && (level == 0 || level == total_level - 1)) {
                // We add file from last pattern level only
                //printf("match: [%s] %s, %s, %s\n", (isDir(file) ? "D" : " "), fullName, dirName, fileName);
                files.push_back(fullName);
            }
 
            if (isDir(file)) {
                scandir(fullName, pattern, files, level + 1);
            }
        }

        free(fileName);
    }

    free(path);
    free(wpath);
    free(level_pattern);
    
}

static int match_file_win(const char* pattern, const char* name, int mode) {

    // PathMatchSpecA
    //printf(" %s -> %s, %d, %d\n", pattern, name, val, res);

    wchar_t* wpattern = char2wchar(pattern, strlen(pattern));
    wchar_t* wname = char2wchar(name, strlen(name));

    //return PathMatchSpecW(wname, wpattern);
    //return PathMatchSpecA(name, pattern);

    // TODO: Stub
    return strcmp(name, "find.cpp") == 0;

    //TODO
    //return 1;
}

static int match_file_win(const char* pattern, const char* name) {
    return match_file_win(pattern, name, 0);
}

#else

bool isDir(struct dirent* file) {
    if (file == NULL) {
        return false;
    }
    return file->d_type == DT_DIR;
}

std::string getCurrentFindPath() {
    return ".";
}

bool isCurrentFindPath(const char* path) {
    if (path == NULL) {
        return false;
    }
    return path[0] == '.';
}

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files, int level) {

    if (dirName == NULL) {
        return;
    }

    DIR* dir = NULL;
    struct dirent* file;
    dir = opendir(dirName);
    if (dir == NULL) {
        fprintf(stderr, "Directory not found: %s", dirName);
        return;
    }

    int total_level = countPathLevel(pattern); // count path level in pattern
    char* level_pattern = getLevelPath(pattern, level);

    //printf("scandir : %s\n", dirName);
    //printf("total   : %d\n", total_level);
    //printf("level   : %d\n", level);
    //printf("pattern : %s\n", pattern);
    //printf("select  : %s\n", level_pattern);
    
    errno = 0;
    while ((file = readdir(dir)) != NULL) {

        char* fileName = file->d_name;

        //printf("try [%d] %s, %s, :: %s\n", level, dirName, fileName, level_pattern);
        if (pattern == NULL || match_file_nix(level_pattern, fileName)) {

            char* fullName = getFilePath(dirName, fileName);
            //printf("match:fullName: %s\n", fullName);

            if (!isDir(file) && (level == 0 || level == total_level - 1)) {
                // We add file from last pattern level only
                //printf("match: [%s] %s, %s, %s\n", (isDir(file) ? "D" : " "), fullName, dirName, fileName);
                files.push_back(fullName);
            }
 
            if (isDir(file)) {
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

static int match_file_nix(const char* pattern, const char* name, int mode) {
    int val = fnmatch(pattern, name, mode);
    int res = val == 0;
    //printf(" %s -> %s, %d, %d\n", pattern, name, val, res);
    //return fnmatch(pattern, name, FNM_PERIOD) == 0; // true id '0'
    return res;
}

static int match_file_nix(const char* pattern, const char* name) {
    return match_file_nix(pattern, name, FNM_PERIOD);
}

#endif

void scandir(const char* dirName, const char* pattern, std::vector<std::string>& files) {
    scandir(dirName, pattern, files, 0);
}

