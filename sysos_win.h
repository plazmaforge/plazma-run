#ifndef PLAZMA_LIB_SYSOS_WIN_H
#define PLAZMA_LIB_SYSOS_WIN_H

#if defined _WIN32
#include <io.h>
#include <windows.h>
//#include <shlobj.h>
//#include <objidl.h>

#include <stdlib.h>
#include "sysos.h"

os_info_t* get_os_info() {
    // TODO: STUB  
    return NULL;
}
#endif

#endif // PLAZMA_LIB_SYSOS_WIN_H