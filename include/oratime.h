/* 
 * Internet Relay Chat, include/oratime.h
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
 *
 * $Id: struct.h,v 1.83 1999/07/31 04:13:52 tomh Exp $
 */
#ifndef INCLUDED_oratime_h
#define INCLUDED_oratime_h
#ifndef INCLUDED_config_h
#include "config.h"
#endif

#ifdef ORATIMING
/* 
 * Timing stuff (for performance measurements): compile with -DORATIMING
 * and put a TMRESET where you want the counter of time spent set to 0,
 * a TMPRINT where you want the accumulated results, and TMYES/TMNO pairs
 * around the parts you want timed -orabidoo
 */
extern void orat_reset(void);
extern void orat_yes(void);
extern void orat_no(void);
extern void orat_report(void);

#define TMRESET orat_reset();
#define TMYES   orat_yes();
#define TMNO    orat_no(); 
#define TMPRINT orat_report();

#else

#define TMRESET
#define TMYES
#define TMNO
#define TMPRINT
#endif

#endif /* INCLUDED_oratime_h */

