/*
 * restart.c
 *
 * $Id$
 */
#include "restart.h"
#include "common.h"
#include "ircd.h"
#include "send.h"
#include "struct.h"
#include "s_debug.h"
#include "s_log.h"

#include <unistd.h>
#include <stdlib.h>

/* external var */
extern char** myargv;

void restart(char *mesg)
{
  static int was_here = NO; /* redundant due to restarting flag below */

  if (was_here)
    abort();
  was_here = YES;

  ilog(L_NOTICE, "Restarting Server because: %s, memory data limit: %ld",
         mesg, get_maxrss());

  server_reboot();
}

void server_reboot(void)
{
  int i;
  
  sendto_ops("Aieeeee!!!  Restarting server... memory: %d", get_maxrss());

  ilog(L_NOTICE, "Restarting server...");
  flush_connections(0);

  for (i = 0; i < MAXCONNECTIONS; ++i)
    close(i);
  execv(SPATH, myargv);

  exit(-1);
}


