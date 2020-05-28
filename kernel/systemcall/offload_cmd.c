#include <sys/lock.h>
#include <sys/uio.h>
#include <sys/stat.h>

#include "console.h"
#include "az_types.h"
#include "offload_channel.h"
#include "offload_page.h"
#include "offload_mmap.h"
#include "offload_message.h"
#include "offload_fio.h"
#include "page.h"
#include "memory.h"
#include "systemcalllist.h"
#include "utility.h"
#include "thread.h"

//#define DEBUG

extern QWORD g_memory_start;
extern int g_ukid ;

/**
 * @brief system system call :  execute a shell command
 * @param command
 * @return success (the return status of the  command), fail (-1)
 */
int cmd_off_debug(char *msg)
{
  int iret = -1;

  channel_t *ch = NULL;
  struct circular_queue *ocq = NULL;

  TCB *current = NULL;
  int mytid = -1;

  ch = get_offload_channel(-1);
  if(ch == NULL)
    return (-1);

  ocq = ch->out;

  current = get_current();
  mytid = current->id;

  send_offload_message(ocq, mytid, CMD_DEBUG, g_ukid, get_pa((QWORD) msg), 0, 0, 0, 0);
/*
  iret = (int) receive_offload_message(icq, mytid, CMD_off_debug);
*/
  return iret;
}

