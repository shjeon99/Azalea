#ifndef __RESOURCE_H_
#define __RESOURCE_H_   

#include "arch.h"

typedef struct a_cpumask {
        unsigned long bits[MASK_SIZE] ;
} a_cpumask_t ;

extern void init_meminfo(char *meminfo) ;
extern void init_cpulist (char *cpulist) ;
extern void init_unikernels_info(void) ;

extern void print_unikernel_info(void) ;
extern int get_io_bitmask(int ukid, a_cpumask_t *bitmap) ;

extern int free_unikernel(int id) ;
extern int alloc_unikernel(int core, int mem) ;

extern void print_unikernel_info(void) ;
extern int print_available_cpus(void) ;
extern void print_free_memory_region(void) ;

extern int get_io_bitmask(int ukid, a_cpumask_t *bitmap) ;

extern int getUnikernelMemInfo(int ukid, int *memroy_start, int *memory_end);
extern int getUnikernelCPUInfo(int ukid, int *core_start) ; 
extern int getUnikernelCPUList(int ukid, int *corelist);

extern int getAzaleaMemInfo(int *memory_start, int *memory_end) ;

extern int sysfs_azalea_init(void);
extern void sysfs_azalea_exit(void);

#endif

