/************************************************************************
 *   IRC - Internet Relay Chat, include/h.h
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
 * "h.h". - Headers file.
 *
 * Most of the externs and prototypes thrown in here to 'cleanup' things.
 * -avalon
 *
 * $Id: h.h,v 1.65 1999/07/21 22:16:01 db Exp $
 *
 */
#ifndef INCLUDED_h_h
#define INCLUDED_h_h
#ifndef INCLUDED_sys_types_h
#include <sys/types.h>
#define INCLUDED_sys_types_h
#endif
#ifndef INCLUDED_fdlist_h
#include "fdlist.h"
#endif

struct Class;
struct Channel;
struct ConfItem;
struct User;
struct stats;
struct SLink;
struct Message;
struct Server;

/* 
 * GLOBAL - global variables
 */
extern int    lifesux;
extern struct Counter Count;

extern time_t NOW;
extern time_t nextconnect;
extern time_t nextping;
extern time_t timeofday;
extern struct Client* GlobalClientList;
extern struct Client  me;
extern struct Client* local[];
extern struct Channel* channel;
extern struct stats* ircstp;
extern  int     bootopt;

extern  int     debuglevel;
extern  int     highest_fd;
extern  int     debuglevel;
extern  int     debugtty;
extern  char*   debugmode;


extern  time_t  check_fdlists (time_t);

extern struct Client* find_chasing (struct Client *, char *, int *);
extern struct Client* find_client(const char* name, struct Client* client);
extern struct Client* find_name (char *, struct Client *);
extern struct Client* find_person (char *, struct Client *);
extern struct Client* find_server(const char* name, struct Client* dflt_client);
extern struct Client* find_userhost (char *, char *, struct Client *, int *);


#ifdef  GLINES
extern struct ConfItem* find_gkill(struct Client* client);
extern struct ConfItem* find_is_glined(const char* host, const char* name);
extern  void    flush_glines(void);             
extern  void    report_glines(struct Client *); 
#endif

extern  int     rehash (struct Client *, struct Client *, int);
extern  void    report_error_on_tty(const char* message); /* ircd.c */

extern void        clear_scache_hash_table(void);
extern const char* find_or_add(const char* name);
extern void        count_scache(int *,unsigned long *);
extern void        list_scache(struct Client *, struct Client *,int, char **);

extern void     dummy(int signo);

extern  char    *form_str (int);
extern  void    get_my_name (struct Client *, char *, int);

extern  void    terminate (void);
extern  int     send_queued(struct Client*);

extern  void    send_capabilities(struct Client *,int);

extern  int     deliver_it (struct Client *, char *, int);

extern  int     parse (struct Client *, char *, char *);
extern  void    init_tree_parse (struct Message *);

extern  int     do_numeric (int, struct Client *, struct Client *, int, char **);

extern  int     dopacket (struct Client *, char *, int);

#ifdef FLUD
void    free_fluders();
void    free_fludees();
#endif /* FLUD */

#ifdef ANTI_SPAMBOT
#define MIN_SPAM_NUM 5
#define MIN_SPAM_TIME 60
#endif

#ifdef IDLE_CHECK
#define MIN_IDLETIME 1800
#endif


#endif /* INCLUDED_h_h */

