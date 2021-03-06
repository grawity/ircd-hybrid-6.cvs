/************************************************************************
 *   IRC - Internet Relay Chat, src/numeric.c
 *   Copyright (C) 1992 Darren Reed
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
 *
 *      I kind of modernized this code a bit. -Dianora
 *
 *   $Id: numeric.c,v 1.3 1999/07/28 22:27:46 tomh Exp $
 */
#include "numeric.h"
#include "irc_string.h"
#include "common.h"     /* NULL cripes */

#include <assert.h>

#ifdef CUSTOM_ERR            /* XXX ick */
#include "messages_cust.tab"
#else
#include "messages.tab"
#endif

char numbuff[512];

const char* form_str(int numeric)
{

  assert(-1 < numeric);
  assert(numeric < ERR_LAST_ERR_MSG);
  assert(replies[numeric] != NULL);
  
  /* XXX eek these were meant to be dummy entries in message.tab for now... */

  if ((numeric < 0) || (numeric > ERR_LAST_ERR_MSG))
  {
      ircsprintf(numbuff, ":%%s %d %%s :INTERNAL ERROR: BAD NUMERIC! %d",
                 numeric, numeric);
      return numbuff;
  }

  if (replies[numeric] == NULL)
  {
    ircsprintf(numbuff, ":%%s %d %%s :NO ERROR FOR NUMERIC ERROR %d",
	       numeric, numeric);
    return numbuff;
  }

  return replies[numeric];
}


