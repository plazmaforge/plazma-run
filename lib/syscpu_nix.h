#ifndef PLAZMA_LIB_SYSCPU_NIX_H
#define PLAZMA_LIB_SYSCPU_NIX_H

#if defined __unix__ || defined __linux__ || (defined __APPLE__ && defined __MACH__)

#include <stdlib.h>
#include <string.h>

#include <sys/utsname.h>    /* CPU utsname: machine     */

#if defined __APPLE__ && defined __MACH__
//#include <sys/param.h>    /* CPU (?)                  */
#include <sys/sysctl.h>     /* CPU - sysctl: cpu.count  */
#else
#include <unistd.h>         /* CPU - sysconf cpu.count  */
#endif

#include "syscpu.h"

arch_t get_cpu_arch_type() {
    /*struct*/ utsname name;
    uname(&name);
    return get_cpu_arch_type_by_machine(name.machine);
}

const char* get_cpu_issalist() {
   #ifdef SI_ISALIST
   /* supported instruction sets */
   char list[258];
   sysinfo(SI_ISALIST, list, sizeof(list));
   return strdup(list); // [alocate]
   #else
   /*struct*/ utsname name;
   uname(&name);
   return get_cpu_issalist_by_machine(name.machine);
   #endif    
}

int _get_cpu_count() {

    #if defined __APPLE__ && defined __MACH__
    int nm[2];
    uint32_t count;
    size_t len = sizeof(count);
 
    nm[0] = CTL_HW; 
    nm[1] = HW_NCPU; //HW_AVAILCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);

    //if(count < 1) {
    //    nm[1] = HW_NCPU;
    //    sysctl(nm, 2, &count, &len, NULL, 0);
    //}
    
    #else
    count = sysconf(_SC_NPROCESSORS_ONLN);
    #endif
    return count > 0 ? count : 1;
}

#endif

#endif // PLAZMA_LIB_SYSCPU_NIX_H

