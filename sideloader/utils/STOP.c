#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../lk/lkernel.h"
#include "arch.h"

/**
 * @brief Load the disk image to the unikernel with parameters
 * @param [0]-index, 
 * @param [1]-cpu_start, [2]-cpu_end, 
 * @prarm [3]-memory_start, [4]-memory_end
 */
int main(int argc, char **argv )
{
  int fd_lk = -1;
  int id =0 ;
	int ret = 0 ;

  if ( argc < 2 )
	{
			printf("Usage: STOP <unikernel id>\n") ;
			return -1; 
	}

	id = atoi(argv[1]) ;

  // Open the lk module
  fd_lk = open("/dev/lk", O_RDWR);
  if (fd_lk < 0) {
    printf("START: /dev/lk open error\n");
    return -1;
  }

  // AZ_PARAM - send parameters to the kernel
  ret = ioctl(fd_lk, CPU_ALL_OFF, &id);
  if (ret < 0) {
		printf("STOP: fail to turn off cores (unikernel id : %d)\n", id) ; 
    close(fd_lk);
    return -1;
  }

  close(fd_lk);

  return 0;
}
