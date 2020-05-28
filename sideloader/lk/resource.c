#include <linux/string.h>
#include <linux/list.h> 
#include <linux/vmalloc.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/sysfs.h>
#include <linux/slab.h>


#include "arch.h"

#if 0
char cpu[]="0,4,32,36,64,68,96,100,128,132,160,164,224,228,256,260,288,292,8,12,40,44,72,76,104,108,136,140,168,172,200,204,232,236,264,268,48,52,80,84,112,116,144,148,176,180,192,196,208,212,240,244,272,276,24,28,56,60,88,92,120,124,152,156,184,188,216,220,248,252,280,284," ;
char mem[]="0x100000000,0x343fffffff" ;
#endif

typedef enum {FALSE = 0, TRUE} boolean;

//#define MASK_SIZE    ( (MAX_CORE/sizeof(unsigned long))+1)

typedef struct a_cpumask {
	unsigned long bits[MASK_SIZE] ;
} a_cpumask_t ;

struct a_cpulist {
	short apicid[MAX_CORE] ;
} ;

struct global_available_cpulist {
	unsigned short nr_cpus ;
	boolean cpus [MAX_CORE] ;
} ; 

typedef struct mem_region {
        unsigned short size ;
        unsigned short from ;
        unsigned short to ;
        struct list_head list;
} mem_region_t ;

struct unikernel_info {
	short  id ;
	boolean avail ;
	mem_region_t *meminfo ;
	unsigned short nr_cpus ;
	struct a_cpulist cpulist ;
  struct kobject *unikernel_kobj ; 
} ;

struct {
	unsigned short mem_start ;
	unsigned short mem_end ;
	unsigned short nr_cpus ;
	struct a_cpulist cpulist ;
} total_resource_info ;

struct {

	unsigned short nr_unikernels ;
	struct unikernel_info unikernels[MAX_UNIKERNEL] ;

	struct global_available_cpulist avail_cpulist ;

	char *avail_mem ;
//	LIST_HEAD(mem_region_list);
	unsigned int sz_avail_mem ;
	struct list_head freemem_region_list ; //= LIST_HEAD_INIT(freemem_region_list) ;

	struct kobject *azalea_kobj ; 
} az_info ;


LIST_HEAD(mem_region_list) ;

static ssize_t meminfo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
				return sprintf(buf, "%d %d\n", total_resource_info.mem_start, total_resource_info.mem_end) ;
}

static ssize_t cpuinfo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
				int i, ret=0 ;
				ret = sprintf(buf, "No. total cores : %d\n", total_resource_info.nr_cpus ) ;
				ret += sprintf(buf+ret, "CPU list (apicid) : ") ;
				for ( i = 0 ; i < total_resource_info.nr_cpus ; i ++ )
					{
							ret += sprintf(buf+ret, " %d ", total_resource_info.cpulist.apicid[i]) ;
					}

				return ret ;
}

static ssize_t avail_cpu_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int i, ret = 0 ; 

	for ( i = 0 ; i < total_resource_info.nr_cpus ; i++ )
		{
				if  ( az_info.avail_cpulist.cpus[i] == TRUE ) 
				{
						ret += sprintf(buf+ret, " %d", total_resource_info.cpulist.apicid[i]) ;
				 }
		}
		
	return ret ;
}

static ssize_t avail_mem_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
			mem_region_t *ptr ;
			int ret = 0 ;

			list_for_each_entry(ptr, &mem_region_list, list){
							ret += sprintf(buf+ret, " (%d, %d)", ptr->from, ptr->to) ;
			}
			return ret ;
}

/*
static ssize_t unikernel_ids_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int ret = 0 ;
  int i ;

  for ( i = 0 ; i < MAX_UNIKERNEL; i ++ )
   {
     if ( az_info.unikernels[i].avail == FALSE )
      {
					ret += sprintf(buf+ret, " %3d ", az_info.unikernels[i].id ) ;
			}
   } 

	return ret ;
}
*/

static ssize_t unikernel_cpu_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) 
{
		int ret =  0;
	  int id = -1 ;
		int i ;

		ret = kstrtou32(kobj->name, 10, &id) ;
    if ( ret != 0 )
				return 0 ;

		for ( i = 0 ; i < az_info.unikernels[id].nr_cpus ; i ++ )
			{
				ret += sprintf(buf+ret, "%3d ", az_info.unikernels[id].cpulist.apicid[i]) ;
			}

		return ret ;
}

static ssize_t unikernel_mem_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
		int ret = 0;
		int id = -1 ;
		int i ;

		ret = kstrtou32(kobj->name, 10, &id ) ;
	
  	if ( ret != 0 ) return 0 ;
		
		ret += sprintf(buf+ret, "%3d ", (az_info.unikernels[id].meminfo)->from ) ;
		ret += sprintf(buf+ret, "%3d ", (az_info.unikernels[id].meminfo)->to ) ;

		return ret ;
} 

static struct kobj_attribute azalea_meminfo =__ATTR(meminfo, 0444, meminfo_show, NULL );
static struct kobj_attribute azalea_cpuinfo =__ATTR(cpuinfo, 0444, cpuinfo_show, NULL );
static struct kobj_attribute azalea_available_cpu =__ATTR(available_cpu, 0444, avail_cpu_show, NULL );
static struct kobj_attribute azalea_available_mem =__ATTR(available_mem, 0444, avail_mem_show, NULL );

static struct kobj_attribute unikernel_meminfo =__ATTR(cpuinfo, 0444, unikernel_cpu_show, NULL );
static struct kobj_attribute unikernel_cpuinfo =__ATTR(meminfo, 0444, unikernel_mem_show, NULL );

static struct attribute *azalea_attrs[] = {
			&azalea_meminfo,
			&azalea_cpuinfo,
			&azalea_available_cpu,
			&azalea_available_mem,
			NULL,
} ;

static struct attribute_group azalea_attr_group = {
			.attrs = azalea_attrs,
};

static struct attribute *unikernel_attrs[] = {
			&unikernel_meminfo,
			&unikernel_cpuinfo,
			NULL,
};

static struct attribute_group unikernel_attr_group = {
			.attrs = unikernel_attrs,
} ;

int sysfs_add_unikernel(int id)
{
		int ret = 0 ;
		char buf[255] ;
	

		if (! az_info.azalea_kobj )
			return -1 ;
	
		sprintf(buf,"%d", id) ;	
    az_info.unikernels[id].unikernel_kobj = kobject_create_and_add( buf , az_info.azalea_kobj ) ;
		if ( !az_info.unikernels[id].unikernel_kobj )
			return -ENOMEM ;

		ret = sysfs_create_group(az_info.unikernels[id].unikernel_kobj, &unikernel_attr_group) ;
		if (ret)
				kobject_put(az_info.unikernels[id].unikernel_kobj) ;

		return ret ;
}

int sysfs_del_unikernel(int id)
{

		if (! az_info.azalea_kobj )
			return -1 ;

		if (! az_info.unikernels[id].unikernel_kobj )
			return -1 ;
		
		kobject_put(az_info.unikernels[id].unikernel_kobj) ;

		return 0 ;
}


int sysfs_azalea_init(void) 
{
  int ret = 0 ; 
 
  az_info.azalea_kobj  = kobject_create_and_add("azalea", NULL) ;
	if ( ! az_info.azalea_kobj )
			return -ENOMEM ;

  ret = sysfs_create_group(az_info.azalea_kobj, &azalea_attr_group) ;

	if (ret)
		kobject_put(az_info.azalea_kobj) ;
	
  return ret ;
}

void sysfs_azalea_exit(void)
{
  if (az_info.azalea_kobj){
        kobject_put(az_info.azalea_kobj) ;
  }
}

static mem_region_t *alloc_mem(int size)
{
        mem_region_t *target=NULL, *victim, *ptr ;
        unsigned int diff = 1024 ;

        if ( size <=0 ) return NULL ;

        list_for_each_entry(victim, &mem_region_list, list)
        {
                if ( victim->size > size ) {
                       if ( diff > (victim->size-size))
                        {
                                diff = victim->size - size ;
                                target = victim ;
                        }
                } else if ( victim->size == size ) {
                        list_del(&victim->list) ;
                        return victim ;
                }
        }

        if ( target == NULL ) return NULL ;

    //    ptr = malloc(sizeof(*ptr)) ;
	ptr = vmalloc(sizeof(*ptr)) ;

        INIT_LIST_HEAD(&ptr->list) ;
        ptr->from = target->from ;
        ptr->to= target->from + size ;
        target->from = ptr->to ;

	ptr->size = ptr->to - ptr->from ;

        return ptr ;
}


static void merge_mem_region(void)
{
  mem_region_t *ptr ;
  mem_region_t *next_ptr ;
  struct list_head *temp, *temp1 ; 

  for ( temp = (&mem_region_list)->next; temp != (&mem_region_list) ; ) 
  {   
       temp1 =  temp->next ;
       if ( temp1 != (&mem_region_list)) 
        {   
          ptr = list_entry(temp, mem_region_t, list ) ; 
          next_ptr = list_entry(temp1, mem_region_t, list ) ; 

         if ( ptr->to == next_ptr->from )
         {   
            ptr->to = next_ptr->to ;
            ptr->size = ptr->to - ptr->from ;
            list_del(&next_ptr->list) ;
						vfree(next_ptr) ;
            continue ; 
         }   
        temp = temp->next ;
        }   
      else
          break ;
  }
}

static void free_mem(mem_region_t *item)
{

	mem_region_t *ptr = NULL ;
	// mem_region_t *prev, *next ;

	int found = 0 ;

	list_for_each_entry(ptr, &mem_region_list, list)
	{
		if ( ptr -> from > item->from )
			{
				found = 1 ;
				break ;
      }
	}

	if (!found)
		list_add_tail(&item->list, &mem_region_list) ;
	else
    list_add_tail(&item->list, &ptr->list) ;
		
	merge_mem_region() ;

	return ;
}


static void init_unikernel_info( struct unikernel_info *ui )
{
	int i = 0 ;
	ui->id = -1 ;
	ui->avail = TRUE ;
	ui->meminfo = NULL ; 
	for ( i = 0 ; i < MAX_CORE ; i ++ )
		ui->cpulist.apicid[i] = -1 ;
	ui->nr_cpus = 0 ;
	
}

void init_unikernels_info(void)
{
	int i = 0 ;
	for ( i = 0 ; i < MAX_UNIKERNEL ; i ++ )
		init_unikernel_info(&az_info.unikernels[i]) ;
}

void init_cpulist (char *cpulist)
{
	int core_cnt = 0 ;
	char *pch, *ptext ;
  int ret ;
  unsigned int value ;

	pch = cpulist ;


	while ((ptext= strsep(&pch, ",")) !=  NULL)
	{
		ret = kstrtou32(ptext, 10, &value) ;
		if ( ret != 0 )  
			break ;
    total_resource_info.cpulist.apicid[core_cnt] = value ;
    az_info.avail_cpulist.cpus[core_cnt++] = TRUE ;
	}	
	
	az_info.avail_cpulist.nr_cpus = core_cnt ;
	total_resource_info.nr_cpus = core_cnt ;	

  printk(KERN_INFO "TOTAL available CPUs : %d\n" , core_cnt ) ;
}

void init_meminfo(char *meminfo) 
{
	unsigned long long addr ;
	char *pch = meminfo ;
	char *pText ;
  int ret ;

	mem_region_t *ptr ;

	pText = strsep(&pch, ",") ;
	if ( pText != NULL )
	{
		ret = kstrtoull ( pText, 16, &addr ) ;
     
		if ( ret != 0 )  
     {
         printk(KERN_INFO "mem parameter parsing failed\n") ;
         return ;
     }
		total_resource_info.mem_start = (addr >> 30) + SHARED_MEMORY_SIZE ;
	}

	pText = strsep(&pch, ",") ;

	if ( pText != NULL )
	{
	  ret = kstrtoull ( pText, 16, &addr ) ;
    if ( ret != 0 )  
		{
			printk(KERN_INFO "mem parameter parsing failed\n") ;
		  return ;
		}
		total_resource_info.mem_end = (addr+1) >> 30;
	}

	ptr  = vmalloc(sizeof(*ptr)) ;
  if ( ptr == NULL ) {
		printk(KERN_INFO "memory region entry allocation failed\n" ) ;
		return ;
	}

	INIT_LIST_HEAD(&ptr->list) ;
	
	ptr-> from = total_resource_info.mem_start ;
	ptr-> to = total_resource_info.mem_end ;
	ptr->size = ptr->to - ptr->from ;

	list_add_tail(&ptr->list, &mem_region_list);

  printk(KERN_INFO "TOTAL available memory : %d (%dG ~ %dG)\n" , ptr->size, ptr->from, ptr->to ) ;
	return ;
}

//return number of allocated cpus
static int alloc_cpus(struct a_cpulist *cpus, int size)
{
    	int i, j=0 ;

    	if ( size <= 0 ) return 0 ;
    	if ( size >= MAX_CORE || size > az_info.avail_cpulist.nr_cpus ) return 0 ;

 	
	for ( i = 0 ; i < total_resource_info.nr_cpus ; i ++ )
	{
	    if ( az_info.avail_cpulist.cpus[i] == TRUE )
	    {
		az_info.avail_cpulist.cpus[i] = FALSE ;
		cpus->apicid[j++] = total_resource_info.cpulist.apicid[i] ;
		if ( j >= size )
		{
			az_info.avail_cpulist.nr_cpus -= size ;
			return size ;
		}
	    }
	}


	return 0 ;
}

static void free_cpus(struct a_cpulist *cpus, int size)
{
	int i, j  ;

	for ( j = 0 ; j < size ; j++ )
 	{
		for ( i = 0 ; i < total_resource_info.nr_cpus ; i ++ )
	    	{
			if ( cpus->apicid[j] == total_resource_info.cpulist.apicid[i] ) 
			{
				az_info.avail_cpulist.cpus[i] = TRUE ;
				az_info.avail_cpulist.nr_cpus ++ ;
				break ; 			
			}	
		}
	}
}

void print_free_memory_region(void)
{
	mem_region_t *ptr ;

	printk(KERN_INFO "Free mem :\n ") ; 
	list_for_each_entry(ptr, &mem_region_list, list){
                printk(KERN_INFO "(%d, %d)", ptr->from, ptr->to ) ;
        }
	printk(KERN_INFO "\n") ;
}

int print_available_cpus(void)
{
	int i ;
	
	printk(KERN_INFO "available cpu : %d \n", az_info.avail_cpulist.nr_cpus) ;
	for ( i = 0 ; i < total_resource_info.nr_cpus ; i ++ )
	  if  (az_info.avail_cpulist.cpus[i] == TRUE) 
		printk(KERN_INFO " %3d", total_resource_info.cpulist.apicid[i]) ;

	printk(KERN_INFO "\n") ;

	return 0 ;
}

int alloc_unikernel(int core, int mem)
{
	int i ;
	int nr_cpu = core, sz_mem = mem ;
	for ( i = 0 ; i < MAX_UNIKERNEL;i++)
		if ( az_info.unikernels[i].avail == TRUE )
 			{
			  //alloc cpu
			az_info.unikernels[i].nr_cpus = alloc_cpus(&az_info.unikernels[i].cpulist, nr_cpu) ;
			if ( az_info.unikernels[i].nr_cpus != nr_cpu ) 
				return -1 ;
			az_info.unikernels[i].meminfo = alloc_mem(sz_mem) ;
			if ( az_info.unikernels[i].meminfo == NULL )
				return -1 ;
			
			az_info.unikernels[i].avail = FALSE ;
			az_info.unikernels[i].id = i ;

			sysfs_add_unikernel(i) ;
			return i ;
			}
	return -1 ;
}

int free_unikernel(int id) 
{
	if (az_info.unikernels[id].avail == TRUE )
		return 0;

	 free_cpus(&az_info.unikernels[id].cpulist, az_info.unikernels[id].nr_cpus) ;
	 free_mem(az_info.unikernels[id].meminfo) ;

	 az_info.unikernels[id].avail = TRUE ;
	 az_info.unikernels[id].id = -1 ;

	 sysfs_del_unikernel(id) ;
	 return 0 ;
}


void print_unikernel_info(void)
{
	int i, j  ;

	for ( i = 0 ; i < MAX_UNIKERNEL; i ++ ) 
	{
		if ( az_info.unikernels[i].avail == FALSE )
		  {
			printk(KERN_INFO "ID: %d\n", az_info.unikernels[i].id) ;
			printk(KERN_INFO "*CPULIST: %d\n", az_info.unikernels[i].nr_cpus ) ;
 			for ( j = 0 ; j < az_info.unikernels[i].nr_cpus ; j++ )
			{
				printk(KERN_INFO " %d", az_info.unikernels[i].cpulist.apicid[j]) ;
			}
			printk(KERN_INFO "*MEM: size: %d, from: %d to: %d\n", 
					az_info.unikernels[i].meminfo->size,
					az_info.unikernels[i].meminfo->from,
					az_info.unikernels[i].meminfo->to ) ;
		  }				  
	}
}

int get_io_bitmask(int ukid, a_cpumask_t *bitmap)
{
	int i, j ;
	boolean found ;
	int index, bits ;

	a_cpumask_t mask ;

	if ( bitmap == NULL ) return -1 ;

	for ( i = 0 ; i < MASK_SIZE ; i ++ )
	{
		mask.bits[i] = 0 ;
	}

	found = FALSE ;

	if ( az_info.unikernels[ukid].avail == TRUE ) return -1 ;

	for ( j = 0 ; j < az_info.unikernels[ukid].nr_cpus ; j++ )
	{
		for ( i = 0 ; i < total_resource_info.nr_cpus ; i++ )
		{
			if ( total_resource_info.cpulist.apicid[i] 
				== az_info.unikernels[ukid].cpulist.apicid[j] )
			{
				found = TRUE ;
				break ;
			}
		}

		if ( found == TRUE ) 
		{
			index = (i) / ((sizeof(unsigned long))*8) ;
			bits =  (i) % ((sizeof(unsigned long))*8) ;

			mask.bits[index] |= ((unsigned long)1 << (bits)) ; 
		}

		found = FALSE ;
	}
	for ( i = 0 ; i < MASK_SIZE ; i++ )
	{
		bitmap->bits[i] = mask.bits[i] ;		
	}

	return 0 ;
}
int getUnikernelMemInfo(int ukid, int *memory_start, int *memory_end) 
{
  if ( az_info.unikernels[ukid].avail == TRUE ) return -1 ; 

	*memory_start = az_info.unikernels[ukid].meminfo->from ;
  *memory_end = az_info.unikernels[ukid].meminfo->to ;

	return 0 ;
}
int getUnikernelCPUInfo(int ukid, int *core_start) 
{
	if ( az_info.unikernels[ukid].avail == TRUE ) return -1 ;

	(*core_start) =az_info.unikernels[ukid].nr_cpus ;

	return 0 ;
}

int getUnikernelCPUList(int ukid, int *corelist) 
{
	int i = 0 ;
	if (az_info.unikernels[ukid].avail == TRUE ) return -1 ;

	for ( i = 0 ; i < az_info.unikernels[ukid].nr_cpus ; i++ )
			corelist[i] = az_info.unikernels[ukid].cpulist.apicid[i] ;

	return 0 ;
}
int getAzaleaMemInfo(int *memory_start, int *memory_end)
{

	*memory_start  = total_resource_info.mem_start ;
	*memory_end = total_resource_info.mem_end ;
		
  return 0 ;
}

#if 0  
void main()
{
	int i = 0, id[5] ;
	init_unikernels_info() ;
	init_cpulist(cpu);
	init_meminfo(mem);

//	print_available_cpus() ;
//	print_free_memory_region() ;

	id[1] =  alloc_ukid(72,10);
	get_io_bitmask(id[1], NULL) ;
/*
	id[2] = alloc_ukid(10,5) ;
	id[3] = alloc_ukid(3,15) ;
	id[4] = alloc_ukid(1,7) ;
	id[0] = alloc_ukid(4,8) ;
*/

	print_unikernel_info() ;

}
#endif

