#ifndef PLAZMA_LIB_SYSUSER_NIX_H
#define PLAZMA_LIB_SYSUSER_NIX_H

#if defined __unix__ || defined __linux__ || (defined __APPLE__ && defined __MACH__)

// STD
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

// NIX
#include <unistd.h>       // getuid (user_name, user_home), getcwd (current_dir), macos:confstr (tmp_dir)
#include <sys/utsname.h>  // os_name, os_version
#include <pwd.h>          // getpwuid (user_name, user_home)
#include <limits.h>       // PATH_MAX
//#include <dlfcn.h>
//#include <langinfo.h>   // nl_langinfo

// BSD
#ifdef _ALLBSD_SOURCE
#ifndef P_tmpdir
#include <paths.h>
#define P_tmpdir _PATH_VARTMP
#endif
#endif

#ifndef P_tmpdir
#define P_tmpdir "/var/tmp"
#endif

char* getUserName() {
  /* User */
  struct passwd *pwent = getpwuid(getuid());
  if (pwent) {
    return strdup(pwent->pw_name);
  } 
  return NULL;
}

char* getUserHome() {
  /* User */
  struct passwd *pwent = getpwuid(getuid());
  if (pwent) {
    return strdup(pwent->pw_dir);
  } 
  return NULL;
}

char* getUserDir() {
  /* Current directory */
  char buf[PATH_MAX];
  //errno = 0;
  if (getcwd(buf, sizeof(buf)) == NULL) {
       fprintf(stderr, "Can't get current working directory.\n");
  } else {
       return strdup(buf);
  }
  return NULL;
}

char* getTmpDir() {
   #if defined __APPLE__ && defined __MACH__
    /* Darwin has a per-user temp dir */
    static char tmp_path[PATH_MAX]; // static - important
    int pathSize = confstr(_CS_DARWIN_USER_TEMP_DIR, tmp_path, PATH_MAX);
    if (pathSize > 0 && pathSize <= PATH_MAX) {
        return tmp_path;
    }
    return (char*) P_tmpdir;
   #else 
     return (char*) P_tmpdir;
   #endif
}

#endif

#endif // PLAZMA_LIB_SYSUSER_NIX_H