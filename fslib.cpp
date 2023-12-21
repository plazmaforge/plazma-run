#include <cstring>
#include "fslib.h"

static const char* filter_pattern;

////

#ifdef _WIN32
#else


static int match_file_nix(const char* pattern, const char* name, int mode);

static int match_file_nix(const char* pattern, const char* name);

static int filter_file_nix(const struct dirent *entry);

static int default_filter_file_nix(const struct dirent *entry);

#endif

static std::vector<std::string> _getFiles(const char* dirName, const char* pattern);

////

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
    filter_pattern = pattern;
    //char* dirname = "../";

    struct dirent **files;
	int n = scandir(dirName, &files, pattern ? filter_file_nix : NULL, alphasort);
    for (int i = 0; i < n; i++) {
        result.push_back(files[i]->d_name);
        //printf("%s\n", files[i]->d_name);
    }
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

static int filter_file_nix(const struct dirent *entry) {
    // TODO: stub for file only
    if (entry->d_type == DT_DIR) {
        return 0;
    }
	return match_file_nix(filter_pattern, entry->d_name);
}

static int default_filter_file_nix(const struct dirent *entry) {
    //if (entry->d_name ==  "." || entry->d_name = "..") {
    //    return 0;
    //}
	return 1;
}

#endif
