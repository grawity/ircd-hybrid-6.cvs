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
 *   $Id: s_err.c,v 1.31 1999/07/23 13:24:26 db Exp $
 */
#include "numeric.h"
#include "irc_string.h"
#include "common.h"     /* NULL cripes */

#ifdef CUSTOM_ERR            /* ZZZZ ick */
#include "messages_cust.tab"
#else
#include "messages.tab"
#endif


/*
 * The observant will note that err_str and rpl_str
 * could be replaced by one function now. 
 * -Dianora
 * ok. ;-)
 */

static char numbuff[512];  /* ZZZ There is no reason this has to
                                * be so large
                                */

const char* form_str(int numeric)
{
  char *nptr;

  if ((numeric < 0) || (numeric > ERR_LAST_ERR_MSG))
    {
      ircsprintf(numbuff, ":%%s %d %%s :INTERNAL ERROR: BAD NUMERIC! %d",
                 numeric, numeric);
      return numbuff;
    }

  if (!(nptr = replies[numeric]))
    {
      ircsprintf(numbuff, ":%%s %d %%s :NO ERROR FOR NUMERIC ERROR %d",
                 numeric, numeric);
      return numbuff;
    }
  else
    return nptr;
}


