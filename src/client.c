/************************************************************************
 *   IRC - Internet Relay Chat, src/s_misc.c
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Computing Center
 *
 *   See file AUTHORS in IRC package for additional names of
 *   the programmers.
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
 *  $Id: client.c,v 1.4 1999/07/11 23:06:32 tomh Exp $
 */
#include "client.h"
#include "struct.h"
#include "res.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "h.h"
#include "fdlist.h"
#include "s_bsd.h"
#include "send.h"
#include "hash.h"

#include <assert.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netdb.h>

/* LINKLIST */
extern aClient *local_cptr_list;
extern aClient *oper_cptr_list;
extern aClient *serv_cptr_list;

static void exit_one_client (aClient *,aClient *,aClient *,char *);
static void recurse_send_quits(aClient *, aClient *, aClient *, char *, char *);
static void remove_dependents (aClient *, aClient *, aClient *, char *, char *);

/*
 * check_registered_user - is used to cancel message, if the
 * originator is a server or not registered yet. In other
 * words, passing this test, *MUST* guarantee that the
 * sptr->user exists (not checked after this--let there
 * be coredumps to catch bugs... this is intentional --msa ;)
 *
 * There is this nagging feeling... should this NOT_REGISTERED
 * error really be sent to remote users? This happening means
 * that remote servers have this user registered, although this
 * one has it not... Not really users fault... Perhaps this
 * error message should be restricted to local clients and some
 * other thing generated for remotes...
 */
int check_registered_user(aClient* client)
{
  if (!IsRegisteredUser(client))
    {
      sendto_one(client, err_str(ERR_NOTREGISTERED), me.name, "*");
      return -1;
    }
  return 0;
}

/*
 * check_registered user cancels message, if 'x' is not
 * registered (e.g. we don't know yet whether a server
 * or user)
 */
int check_registered(aClient* client)
{
  if (!IsRegistered(client))
    {
      sendto_one(client, err_str(ERR_NOTREGISTERED), me.name, "*");
      return -1;
    }
  return 0;
}

/*
 * release_client_dns_reply - remove client dns_reply references
 *
 */
void release_client_dns_reply(struct Client* client)
{
  assert(0 != client);
  if (client->dns_reply) {
    --client->dns_reply->ref_count;
    client->dns_reply = 0;
  }
}

/*
 * get_client_name
 *      Return the name of the client for various tracking and
 *      admin purposes. The main purpose of this function is to
 *      return the "socket host" name of the client, if that
 *        differs from the advertised name (other than case).
 *        But, this can be used to any client structure.
 *
 * NOTE 1:
 *        Watch out the allocation of "nbuf", if either sptr->name
 *        or sptr->sockhost gets changed into pointers instead of
 *        directly allocated within the structure...
 *
 * NOTE 2:
 *        Function return either a pointer to the structure (sptr) or
 *        to internal buffer (nbuf). *NEVER* use the returned pointer
 *        to modify what it points!!!
 */
const char* get_client_name(struct Client* client, int showip)
{
  static char nbuf[HOSTLEN * 2 + USERLEN + 5];
  char        t_port[7];
  char        t_user[USERLEN + 2];
  char        t_host[HOSTLEN + 1];
  /* t_id[4]; */
  /*
   * The idea here is to build our text first, then do the
   * ircsprintf(), as all these conditionals are getting very
   * hairy.  -- FlashMan
   */

  t_port[0]='\0';
  t_user[0]='\0';
  t_host[0]='\0';
  /*  t_id[0]='\0'; */

  if (MyConnect(client))
    {
      /* Check for ident */

      /* original -db */
      /* (void)strcpy(t_id, "(+)"); */

      /* Check for a username (listening ports don't have usernames) */
      if (client->username[0])
        strcpy(t_user, client->username);
      if (t_user[0])
        strcat(t_user, "@");

      /* Check for a port number, needed for listening ports */
      if (client->flags & FLAGS_LISTEN)
        sprintf(t_port, "/%u", client->port);

      /* And finally, let's get the host information, ip or name */
      switch (showip)
        {
          case TRUE:
            strcpy(t_host, inetntoa((char *)&client->ip));
            break;
          case HIDEME:
            strcpy(t_host, "255.255.255.255");
            break;
          default:
            strcpy(t_host, client->host);
        }

      /* Now we add everything together */
      /*      (void)ircsprintf(nbuf, "%s[%s%s%s%s]", client->name, t_id,
        t_user, t_host, t_port); */
      ircsprintf(nbuf, "%s[%s%s%s]", client->name, t_user, t_host, t_port);
    }
  else
    {
      /* As pointed out by Adel Mezibra 
       * Neph|l|m@EFnet. Was missing a return here.
       */
      return client->name;
    }

  if (irccmp(client->name, client->host) || t_port[0])
    return nbuf;
  return client->name;
}

const char* get_client_host(struct Client* client)
{
  static char nbuf[HOSTLEN * 2 + USERLEN + 5];
  
  if (!MyConnect(client))
    return client->name;
  if (!client->dns_reply)
    return get_client_name(client, FALSE);
  else
    {
      if(IsGotId(client))
        {
          ircsprintf(nbuf, "%s[(+)%-.*s@%-.*s]",
                     client->name, USERLEN, client->username,
                     HOSTLEN, client->host);
        }
      else
        {
          ircsprintf(nbuf, "%s[%-.*s@%-.*s]",
                     client->name, USERLEN, client->username,
                     HOSTLEN, client->host);
        }

    }
  return nbuf;
}


/*
** exit_client
**        This is old "m_bye". Name  changed, because this is not a
**        protocol function, but a general server utility function.
**
**        This function exits a client of *any* type (user, server, etc)
**        from this server. Also, this generates all necessary prototol
**        messages that this exit may cause.
**
**   1) If the client is a local client, then this implicitly
**        exits all other clients depending on this connection (e.g.
**        remote clients having 'from'-field that points to this.
**
**   2) If the client is a remote client, then only this is exited.
**
** For convenience, this function returns a suitable value for
** m_function return value:
**
**        FLUSH_BUFFER        if (cptr == sptr)
**        0                if (cptr != sptr)
*/
int        exit_client(
aClient *cptr,        /*
                ** The local client originating the exit or NULL, if this
                ** exit is generated by this server for internal reasons.
                ** This will not get any of the generated messages.

                */
aClient *sptr,        /* Client exiting */
aClient *from,        /* Client firing off this Exit, never NULL! */
char        *comment        /* Reason for the exit */
                   )
{
  aClient        *acptr;
  aClient        *next;
#ifdef        FNAME_USERLOG
  time_t        on_for;
#endif
  char        comment1[HOSTLEN + HOSTLEN + 2];
  if (MyConnect(sptr))
    {
#ifdef LIMIT_UH
      if(sptr->flags & FLAGS_IPHASH)
        remove_one_ip(sptr);
#else
      if(sptr->flags & FLAGS_IPHASH)
        remove_one_ip(sptr->ip.s_addr);
#endif
      if (IsAnOper(sptr))
        {
          delfrom_fdlist(sptr->fd, &oper_fdlist);
          /* LINKLIST */
          /* oh for in-line functions... */
          {
            aClient *prev_cptr=(aClient *)NULL;
            aClient *cur_cptr = oper_cptr_list;
            while(cur_cptr) 
              {
                if(sptr == cur_cptr)
                  {
                    if(prev_cptr)
                      prev_cptr->next_oper_client = cur_cptr->next_oper_client;
                    else
                      oper_cptr_list = cur_cptr->next_oper_client;
                    cur_cptr->next_oper_client = (aClient *)NULL;
                    break;
                  }
                else
                  prev_cptr = cur_cptr;
                cur_cptr = cur_cptr->next_oper_client;
              }
          }
        }
      if (IsClient(sptr))
        {
          Count.local--;

          /* LINKLIST */
          /* oh for in-line functions... */
          if(IsPerson(sptr))        /* a little extra paranoia */
            {
              if(sptr->previous_local_client)
                sptr->previous_local_client->next_local_client =
                  sptr->next_local_client;
              else
                {
                  if(local_cptr_list == sptr)
                    {
                      local_cptr_list = sptr->next_local_client;
                    }
                }

              if(sptr->next_local_client)
                sptr->next_local_client->previous_local_client =
                  sptr->previous_local_client;

              sptr->previous_local_client = sptr->next_local_client = 
                (aClient *)NULL;
            }
        }
      if (IsServer(sptr))
        {
          Count.myserver--;
          delfrom_fdlist(sptr->fd, &serv_fdlist);

          /* LINKLIST */
          /* oh for in-line functions... */
          {
            aClient *prev_cptr = (aClient *)NULL;
            aClient *cur_cptr = serv_cptr_list;
            while(cur_cptr)
              {
                if(sptr == cur_cptr)
                  {
                    if(prev_cptr)
                      prev_cptr->next_server_client =
                        cur_cptr->next_server_client;
                    else
                      serv_cptr_list = cur_cptr->next_server_client;
                    cur_cptr->next_server_client = (aClient *)NULL;
                    break;
                  }
                else
                  prev_cptr = cur_cptr;
                cur_cptr = cur_cptr->next_server_client;
              }
          }
        }
      sptr->flags |= FLAGS_CLOSING;
      if (IsPerson(sptr))
        {
          sendto_realops_lev(CCONN_LEV, "Client exiting: %s (%s@%s) [%s] [%s]",
                    sptr->name, sptr->username, sptr->host,
#ifdef WINTRHAWK
                    comment,
#else
                    (sptr->flags & FLAGS_NORMALEX) ?  "Client Quit" : comment,
#endif /* WINTRHAWK */
                    inetntoa((char *)&sptr->ip));
        }
#ifdef FNAME_USERLOG
          on_for = timeofday - sptr->firsttime;
# if defined(USE_SYSLOG) && defined(SYSLOG_USERS)
          if (IsPerson(sptr))
            syslog(LOG_NOTICE, "%s (%3ld:%02ld:%02ld): %s!%s@%s %ld/%ld\n",
                   myctime(sptr->firsttime),
                   on_for / 3600, (on_for % 3600)/60,
                   on_for % 60, sptr->name,
                   sptr->user->username, sptr->user->host,
                   sptr->sendK, sptr->receiveK);
# else
          {
            char        linebuf[300];
            static int        logfile = -1;
            static long        lasttime;

            /*
             * This conditional makes the logfile active only after
             * it's been created - thus logging can be turned off by
             * removing the file.
             *
             * stop NFS hangs...most systems should be able to open a
             * file in 3 seconds. -avalon (curtesy of wumpus)
             *
             * Keep the logfile open, syncing it every 10 seconds
             * -Taner
             */
            if (IsPerson(sptr))
              {
                if (logfile == -1)
                  {
                    logfile = open(FNAME_USERLOG, O_WRONLY|O_APPEND);
                  }
                (void)ircsprintf(linebuf,
                                 "%s (%3d:%02d:%02d): %s!%s@%s %d/%d\n",
                                 myctime(sptr->firsttime), on_for / 3600,
                                 (on_for % 3600)/60, on_for % 60,
                                 sptr->name,
                                 sptr->username,
                                 sptr->host,
                                 sptr->sendK,
                                 sptr->receiveK);
                (void)write(logfile, linebuf, strlen(linebuf));
                /*
                 * Resync the file evey 10 seconds
                 */
                if (timeofday - lasttime > 10)
                  {
                    (void)close(logfile);
                    logfile = -1;
                    lasttime = timeofday;
                  }
              }
          }
# endif
#endif
          if (sptr->fd >= 0)
            {
              if (cptr != NULL && sptr != cptr)
                sendto_one(sptr, "ERROR :Closing Link: %s %s (%s)",
                           sptr->host, sptr->name, comment);
              else
                sendto_one(sptr, "ERROR :Closing Link: %s (%s)",
                           sptr->host, comment);
            }
          /*
          ** Currently only server connections can have
          ** depending remote clients here, but it does no
          ** harm to check for all local clients. In
          ** future some other clients than servers might
          ** have remotes too...
          **
          ** Close the Client connection first and mark it
          ** so that no messages are attempted to send to it.
          ** (The following *must* make MyConnect(sptr) == FALSE!).
          ** It also makes sptr->from == NULL, thus it's unnecessary
          ** to test whether "sptr != acptr" in the following loops.
          */

          close_connection(sptr);
    }

  if(IsServer(sptr))
    {        
      /* I'm paranoid -Dianora */
      if((sptr->serv) && (sptr->serv->up))
        (void)strcpy(comment1, sptr->serv->up);
      else
        (void)strcpy(comment1, "<Unknown>" );

      (void)strcat(comment1," ");
      (void)strcat(comment1, sptr->name);

      remove_dependents(cptr, sptr, from, comment, comment1);

      if (sptr->servptr == &me)
        {
          sendto_ops("%s was connected for %lu seconds.  %lu/%lu sendK/recvK.",
                     sptr->name, timeofday - sptr->firsttime,
                     sptr->sendK, sptr->receiveK);
#ifdef USE_SYSLOG
          syslog(LOG_NOTICE, "%s was connected for %lu seconds.  %lu/%lu sendK/recvK.", sptr->name, timeofday - sptr->firsttime, sptr->sendK, sptr->receiveK);
#endif

              /* Just for paranoia... this shouldn't be necessary if the
              ** remove_dependents() stuff works, but it's still good
              ** to do it.    MyConnect(sptr) has been set to false,
              ** so we look at servptr, which should be ok  -orabidoo
              */
              for (acptr = GlobalClientList; acptr; acptr = next)
                {
                  next = acptr->next;
                  if (!IsServer(acptr) && acptr->from == sptr)
                    {
                      ts_warn("Dependent client %s not on llist!?",
                              acptr->name);
                      exit_one_client(NULL, acptr, &me, comment1);
                    }
                }
              /*
              ** Second SQUIT all servers behind this link
              */
              for (acptr = GlobalClientList; acptr; acptr = next)
                {
                  next = acptr->next;
                  if (IsServer(acptr) && acptr->from == sptr)
                    {
                      ts_warn("Dependent server %s not on llist!?", 
                                     acptr->name);
                      exit_one_client(NULL, acptr, &me, me.name);
                    }
                }
            }
        }

  exit_one_client(cptr, sptr, from, comment);
  return cptr == sptr ? FLUSH_BUFFER : 0;
}

/*
** Recursively send QUITs and SQUITs for sptr and all its dependent clients
** and servers to those servers that need them.  A server needs the client
** QUITs if it can't figure them out from the SQUIT (ie pre-TS4) or if it
** isn't getting the SQUIT because of @#(*&@)# hostmasking.  With TS4, once
** a link gets a SQUIT, it doesn't need any QUIT/SQUITs for clients depending
** on that one -orabidoo
*/
static        void recurse_send_quits(aClient *cptr, 
                                aClient *sptr,
                                aClient *to,
                                char *comment,  /* for servers */
                                char *myname)
{
  aClient *acptr;

  /* If this server can handle quit storm (QS) removal
   * of dependents, just send the SQUIT -Dianora
   */

  if (IsCapable(to,CAP_QS))
    {
      if (match(myname, sptr->name))
        {
          for (acptr = sptr->serv->users; acptr; acptr = acptr->lnext)
            sendto_one(to, ":%s QUIT :%s", acptr->name, comment);
          for (acptr = sptr->serv->servers; acptr; acptr = acptr->lnext)
            recurse_send_quits(cptr, acptr, to, comment, myname);
        }
      else
        sendto_one(to, "SQUIT %s :%s", sptr->name, me.name);
    }
  else
    {
      for (acptr = sptr->serv->users; acptr; acptr = acptr->lnext)
        sendto_one(to, ":%s QUIT :%s", acptr->name, comment);
      for (acptr = sptr->serv->servers; acptr; acptr = acptr->lnext)
        recurse_send_quits(cptr, acptr, to, comment, myname);
      if (!match(myname, sptr->name))
        sendto_one(to, "SQUIT %s :%s", sptr->name, me.name);
    }
}

/* 
** Remove all clients that depend on sptr; assumes all (S)QUITs have
** already been sent.  we make sure to exit a server's dependent clients 
** and servers before the server itself; exit_one_client takes care of 
** actually removing things off llists.   tweaked from +CSr31  -orabidoo
*/
/*
 * added sanity test code.... sptr->serv might be NULL... -Dianora
 */
static        void recurse_remove_clients(aClient *sptr, char *comment)
{
  aClient *acptr;

  if (IsMe(sptr))
    return;

  if (!sptr->serv)        /* oooops. uh this is actually a major bug */
    return;

  while ( (acptr = sptr->serv->servers) )
    {
      recurse_remove_clients(acptr, comment);
      /*
      ** a server marked as "KILLED" won't send a SQUIT 
      ** in exit_one_client()   -orabidoo
      */
      acptr->flags |= FLAGS_KILLED;
      exit_one_client(NULL, acptr, &me, me.name);
    }

  while ( (acptr = sptr->serv->users) )
    {
      acptr->flags |= FLAGS_KILLED;
      exit_one_client(NULL, acptr, &me, comment);
    }
}

/*
** Remove *everything* that depends on sptr, from all lists, and sending
** all necessary QUITs and SQUITs.  sptr itself is still on the lists,
** and its SQUITs have been sent except for the upstream one  -orabidoo
*/
static        void remove_dependents(aClient *cptr, 
                               aClient *sptr,
                               aClient *from,
                               char *comment,
                               char *comment1)
{
  aClient *to;
  int i;
  aConfItem *aconf;
  static char myname[HOSTLEN+1];

  for (i=0; i<=highest_fd; i++)
    {
      if (!(to = local[i]) || !IsServer(to) || IsMe(to) ||
          to == sptr->from || (to == cptr && IsCapable(to,CAP_QS)))
        continue;
      /* MyConnect(sptr) is rotten at this point: if sptr
       * was mine, ->from is NULL.  we need to send a 
       * WALLOPS here only if we're "deflecting" a SQUIT
       * that hasn't hit its target  -orabidoo
       */
      /* The WALLOPS isn't needed here as pointed out by
       * comstud, since m_squit already does the notification.
       */
#if 0
      if (to != cptr &&        /* not to the originator */
          to != sptr->from && /* not to the destination */
          cptr != sptr->from        /* hasn't reached target */
          && sptr->servptr != &me) /* not mine [done in m_squit] */
        sendto_one(to, ":%s WALLOPS :Received SQUIT %s from %s (%s)",
                   me.name, sptr->name, get_client_name(from, FALSE), comment);

#endif
      if ((aconf = to->serv->nline))
        strncpyzt(myname, my_name_for_link(me.name, aconf), HOSTLEN+1);
      else
        strncpyzt(myname, me.name, HOSTLEN+1);
      recurse_send_quits(cptr, sptr, to, comment1, myname);
    }

  recurse_remove_clients(sptr, comment1);
}

/*
** Exit one client, local or remote. Assuming all dependents have
** been already removed, and socket closed for local client.
*/
static        void        exit_one_client(aClient *cptr,
                                aClient *sptr,
                                aClient *from,
                                char *comment)
{
  aClient *acptr;
  Link        *lp;

  if (IsServer(sptr))
    {
      if (sptr->servptr && sptr->servptr->serv)
        del_client_from_llist(&(sptr->servptr->serv->servers),
                                    sptr);
      else
        ts_warn("server %s without servptr!", sptr->name);
    }
  else if (sptr->servptr && sptr->servptr->serv)
      del_client_from_llist(&(sptr->servptr->serv->users), sptr);
  /* there are clients w/o a servptr: unregistered ones */

  /*
  **  For a server or user quitting, propogate the information to
  **  other servers (except to the one where is came from (cptr))
  */
  if (IsMe(sptr))
    {
      sendto_ops("ERROR: tried to exit me! : %s", comment);
      return;        /* ...must *never* exit self!! */
    }
  else if (IsServer(sptr))
    {
      /*
      ** Old sendto_serv_but_one() call removed because we now
      ** need to send different names to different servers
      ** (domain name matching)
      */
      /*
      ** The bulk of this is done in remove_dependents now, all
      ** we have left to do is send the SQUIT upstream.  -orabidoo
      */
      acptr = sptr->from;
      if (acptr && IsServer(acptr) && acptr != cptr && !IsMe(acptr) &&
          (sptr->flags & FLAGS_KILLED) == 0)
        sendto_one(acptr, ":%s SQUIT %s :%s", from->name, sptr->name, comment);
    }
  else if (!(IsPerson(sptr)))
      /* ...this test is *dubious*, would need
      ** some thought.. but for now it plugs a
      ** nasty hole in the server... --msa
      */
      ; /* Nothing */
  else if (sptr->name[0]) /* ...just clean all others with QUIT... */
    {
      /*
      ** If this exit is generated from "m_kill", then there
      ** is no sense in sending the QUIT--KILL's have been
      ** sent instead.
      */
      if ((sptr->flags & FLAGS_KILLED) == 0)
        {
          sendto_serv_butone(cptr,":%s QUIT :%s",
                             sptr->name, comment);
        }
      /*
      ** If a person is on a channel, send a QUIT notice
      ** to every client (person) on the same channel (so
      ** that the client can show the "**signoff" message).
      ** (Note: The notice is to the local clients *only*)
      */
      if (sptr->user)
        {
          sendto_common_channels(sptr, ":%s QUIT :%s",
                                   sptr->name, comment);

          while ((lp = sptr->user->channel))
            remove_user_from_channel(sptr,lp->value.chptr,0);
          
          /* Clean up invitefield */
          while ((lp = sptr->user->invited))
            del_invite(sptr, lp->value.chptr);
          /* again, this is all that is needed */
        }
    }
  
  /* 
   * Remove sptr from the client lists
   */
  del_from_client_hash_table(sptr->name, sptr);
  remove_client_from_list(sptr);
}
