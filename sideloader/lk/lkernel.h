#ifndef __LKERNEL_H__
#define __LKERNEL_H__

#define PAGE_4K             (0x1000)

#define CONFIG_PARAM_NUM    4
#define PARM_CPU			0
#define PARM_MEM 			1
#define PARM_IMAGE  	2
#define PARM_KERN32 	3 

#define MAX_PATH  256	

typedef struct unikernel_param {
			unsigned short param[CONFIG_PARAM_NUM] ;
			char full_filename[MAX_PATH] ;
} unikernel_param_t ;
	 
#define SECTOR				512

#define AZ_PARAM            10
#define AZ_LOADING          11
#define AZ_GET_MEM_ADDR     12
#define AZ_PRINT_MSG        13

#define CPU_ON				100
#define CPU_OFF				110
#define CPU_ALL_ON		120
#define	CPU_ALL_OFF		130

#define IO_REMAP			300

#define TOTAL_COUNT_OFFSET	0x83

#define REMOTE_PAGE_MEMORY_SIZE     64

void init_page_table(unsigned long base_va);

#endif  /* __LKERNEL_H__ */
