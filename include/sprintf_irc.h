/*
 * $Id: sprintf_irc.h,v 1.0 2000/09/14 05:21:56 lusky Exp $ 
 */

#ifndef SPRINTF_IRC
#define SPRINTF_IRC

#include <stdarg.h>

/*=============================================================================
 * Proto types
 */

extern int vsprintf_irc(register char *str, register const char *format,
    register va_list);
extern int ircsprintf(register char *str, register const char *format, ...);

#endif /* SPRINTF_IRC */
