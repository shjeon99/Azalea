#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h>

#include "offload_channel.h"
#include "offload_fio.h"
#include "offload_message.h"
#include "offload_mmap.h"
#include "offload_thread_pool.h"

#include "../lk/lkernel.h" 

void cmd_off_stop(job_args_t *job_args)
{
	char command[255]  ;
	int fd = -1 ;
  //struct circular_queue *out_cq = NULL;
  io_packet_t *in_pkt = NULL;
  int iret = -1;

  in_pkt = (io_packet_t *) &job_args->pkt;

  fd = open("/dev/lk", O_RDWR);
  if (fd < 0) {
    return ;
  }

	iret = ioctl(fd, CPU_ALL_OFF, in_pkt->param1) ;

#if 0
	sprintf(command, "STOP %ld", in_pkt->param1);
	printf("================ %s ================", command ) ;
  msg = (char *) get_va(in_pkt->param2);
	printf("invoked from %s\n", msg) ;

  // execute unlink
  iret = system(command);
#endif

  // check error
  if(iret == -1)
    fprintf(stderr, "FIO SYSTEM: %s, %s\n", strerror(errno), command);

	close(fd) ;

#if 0  // do not return
  // retrun ret
  pthread_mutex_lock(&ch->mutex);
  send_offload_message(out_cq, tid, offload_function_type, (unsigned long) iret);
  pthread_mutex_unlock(&ch->mutex);
#endif
}
