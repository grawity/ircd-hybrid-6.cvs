/************************************************************************
 *   IRC - Internet Relay Chat, src/bsd.c
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Computing Center
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   $Id: bsd.c,v 1.5 1999/07/16 02:40:36 db Exp $
 */
#include "struct.h"
#include "common.h"
#include "sys.h"
#include "h.h"
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>

void dummy(int sig)
{
#if !defined(POSIX_SIGNALS)
  signal(SIGALRM, dummy);
  signal(SIGPIPE, dummy);
# ifdef SIGWINCH
  signal(SIGWINCH, dummy);
# endif
#endif

#if 0 /* POSIX signals reinstall handlers themselves */
  struct  sigaction       act;

  act.sa_handler = dummy;
  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);
  sigaddset(&act.sa_mask, SIGALRM);
  sigaddset(&act.sa_mask, SIGPIPE);
#  ifdef SIGWINCH
  sigaddset(&act.sa_mask, SIGWINCH);
  sigaction(SIGWINCH, &act, NULL);
#  endif
  sigaction(SIGALRM, &act, NULL);
  sigaction(SIGPIPE, &act, NULL);
#endif /* 0 */
}


/*
 * deliver_it
 *	Attempt to send a sequence of bytes to the connection.
 *	Returns
 *
 *	< 0	Some fatal error occurred, (but not EWOULDBLOCK).
 *		This return is a request to close the socket and
 *		clean up the link.
 *	
 *	>= 0	No real error occurred, returns the number of
 *		bytes actually transferred. EWOULDBLOCK and other
 *		possibly similar conditions should be mapped to
 *		zero return. Upper level routine will have to
 *		decide what to do with those unwritten bytes...
 *
 *	*NOTE*	alarm calls have been preserved, so this should
 *		work equally well whether blocking or non-blocking
 *		mode is used...
 */
int deliver_it(aClient *cptr, char *str, int len)
{
  int	retval;

  retval = send(cptr->fd, str, len, 0);
  /*
  ** Convert WOULDBLOCK to a return of "0 bytes moved". This
  ** should occur only if socket was non-blocking. Note, that
  ** all is Ok, if the 'write' just returns '0' instead of an
  ** error and errno=EWOULDBLOCK.
  **
  */
  if (retval < 0 && (errno == EWOULDBLOCK || errno == EAGAIN ||
		     errno == ENOBUFS))
    {
      retval = 0;
      cptr->flags |= FLAGS_BLOCKED;
      return(retval);  /* Just get out now... */
    }
  else if (retval > 0)
    {
      cptr->flags &= ~FLAGS_BLOCKED;
    }

  if (retval > 0)
    {
      cptr->sendB += retval;
      me.sendB += retval;
      if (cptr->sendB > 1023)
	{
	  cptr->sendK += (cptr->sendB >> 10);
	  cptr->sendB &= 0x03ff;	/* 2^10 = 1024, 3ff = 1023 */
	}
      else if (me.sendB > 1023)
	{
	  me.sendK += (me.sendB >> 10);
	  me.sendB &= 0x03ff;
	}
    }
  return(retval);
}


