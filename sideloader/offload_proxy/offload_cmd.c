#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dirent.h>

#include "offload_channel.h"
#include "offload_fio.h"
#include "offload_message.h"
#include "offload_mmap.h"
#include "offload_thread_pool.h"

void cmd_off_stop(job_args_t *job_args)
{
	char command[255]  ;
	char *msg ;

  //struct circular_queue *out_cq = NULL;
  io_packet_t *in_pkt = NULL;

  int iret = -1;

  //char *command = NULL;;

  //int tid = 0;
  //int  offload_function_type = 0;

  //struct channel_struct *ch;
//  ch = job_args->ch;
//  out_cq = job_args->ch->out_cq;
  in_pkt = (io_packet_t *) &job_args->pkt;

//  tid = (int) in_pkt->tid;
//  offload_function_type = (int) in_pkt->io_function_type;

	sprintf(command, "STOP %ld", in_pkt->param1);
	printf("================ %s ================", command ) ;
  msg = (char *) get_va(in_pkt->param2);
	printf("invoked from %s\n", msg) ;

  // execute unlink
  iret = system(command);

  // check error
  if(iret == -1)
    fprintf(stderr, "FIO SYSTEM: %s, %s\n", strerror(errno), command);

#if 0  // do not return
  // retrun ret
  pthread_mutex_lock(&ch->mutex);
  send_offload_message(out_cq, tid, offload_function_type, (unsigned long) iret);
  pthread_mutex_unlock(&ch->mutex);
#endif
}
