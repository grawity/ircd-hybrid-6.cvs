/*
 *  ircd-hybrid: an advanced Internet Relay Chat Daemon(ircd).
 *  m_whois.c: Shows who a user is.
 *
 *  Copyright (C) 2002 by the past and present ircd coders, and others.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *  USA
 *
 *   $Id$
 */

#include "common.h"
#include "m_operspylog.h"
#include "m_commands.h"
#include "client.h"
#include "channel.h"
#include "hash.h"
#include "struct.h"
#include "ircd.h"
#include "numeric.h"
#include "s_serv.h"
#include "send.h"
#include "list.h"
#include "irc_string.h"

#include <string.h>

static char buf[BUFSIZE];

/*
** m_whois
**      parv[0] = sender prefix
**      parv[1] = nickname masklist
*/
int m_whois (struct Client *cptr, struct Client *sptr, int parc, char *parv[])
{
  static anUser UnknownUser = {
    NULL,       /* next */
    NULL,       /* channel */
    NULL,       /* invited */
    NULL,       /* away */
    0,          /* last */
    1,          /* refcount */
    0,          /* joined */
    "<Unknown>"         /* server */
  };
  Link  *lp;
  anUser        *user;
  struct Client *acptr, *a2cptr;
  aChannel *chptr;
  char  *nick, *name;
  /* char  *tmp; */
  char  *p = NULL;
  int   found, len, mlen;
  static time_t last_used = 0L;
  int found_mode;
  int whois_len = 4;

#ifdef OPERSPY
  char osnuh[NICKLEN + 1 + USERLEN + 1 + HOSTLEN + 1 + HOSTLEN + 1];
  int OperSpyWhois = 0;
#endif


  if (parc < 2)
  {
    sendto_one (sptr, form_str (ERR_NONICKNAMEGIVEN), me.name, parv[0]);
    return 0;
  }

  if (parc > 2)
  {
    if (hunt_server (cptr, sptr, ":%s WHOIS %s :%s", 1, parc, parv) !=
        HUNTED_ISME)
    return 0;
    parv[1] = parv[2];
  }

  if (!IsAnOper (sptr) && !MyConnect (sptr))	/* pace non local requests */
  {
    if ((last_used + WHOIS_WAIT) > CurrentTime)
    {
    /* Unfortunately, returning anything to a non local
     * request =might= increase sendq to be usable in a split hack
     * Sorry gang ;-( - Dianora
     */
      return 0;
    }
    else
    {
      last_used = CurrentTime;
    }
  }

  /* Multiple whois from remote hosts, can be used
   * to flood a server off. One could argue that multiple whois on
   * local server could remain. Lets think about that, for now
   * removing it totally. 
   * -Dianora 
   */

  /*  for (tmp = parv[1]; (nick = strtoken(&p, tmp, ",")); tmp = NULL) */
  nick = parv[1];
  p = strchr (parv[1], ',');
  if (p) *p = '\0';
  {
    int       invis, member;
    found = 0;

#ifdef OPERSPY
    if ((nick[0] == WHOIS_PREFIX) && (IsSetOperOSpy (cptr)))
    {
      OperSpyWhois = 1;
      whois_len = 5;
      nick++;
    }
#endif

    (void) collapse (nick);
    acptr = hash_find_client (nick, (struct Client *) NULL);
    if (!acptr)
    {
      sendto_one (sptr, form_str (ERR_NOSUCHNICK), me.name, parv[0], nick);
      return 0;
      /*              continue; */
    }
    if (!IsPerson (acptr))
    {
      sendto_one (sptr, form_str (RPL_ENDOFWHOIS),
                  me.name, parv[0], parv[1]); 
      return 0;
    }      
    /* continue; */

    user = acptr->user ? acptr->user : &UnknownUser;
    name = (!*acptr->name) ? "?" : acptr->name;
    invis = IsInvisible (acptr);
    member = (user->channel) ? 1 : 0;
    a2cptr = find_server (user->server);

    sendto_one (sptr, form_str (RPL_WHOISUSER), me.name,
		parv[0], name, acptr->username, acptr->host, acptr->info);

    mlen = strlen (me.name) + strlen (parv[0]) + 6 + strlen (name);

    for (len = 0, *buf = '\0', lp = user->channel; lp; lp = lp->next)
    {
      chptr = lp->value.chptr;
      if (ShowChannel (sptr, chptr)
#ifdef OPERSPY
          || OperSpyWhois
#endif
         )
      {
        if (len + strlen (chptr->chname) > (size_t) BUFSIZE - whois_len - mlen)
        {
	  sendto_one (sptr,":%s %d %s %s :%s",
	              me.name, RPL_WHOISCHANNELS, parv[0], name, buf);
	  *buf = '\0';
          len = 0;
        }
        found_mode = user_channel_mode (acptr, chptr);
#ifdef OPERSPY
	if (OperSpyWhois && !ShowChannel (sptr, chptr))
	{
	  *(buf + len++) = WHOIS_PREFIX;
	}
#endif

#ifdef HIDE_OPS
	if (is_chan_op (sptr, chptr))
#endif  
        {
          if (found_mode & CHFL_CHANOP)
            *(buf + len++) = '@';
          else if (found_mode & CHFL_VOICE)
            *(buf + len++) = '+';
        }
        if (len)
          *(buf + len) = '\0';
        (void) strcpy (buf + len, chptr->chname);
        len += strlen (chptr->chname);
        (void) strcat (buf + len, " ");
        len++;
      }
    }
    if (buf[0] != '\0')
      sendto_one (sptr, form_str (RPL_WHOISCHANNELS),
                  me.name, parv[0], name, buf);

#ifdef SERVERHIDE
    if (!(IsAnOper (sptr) || acptr == sptr))
      sendto_one (sptr, form_str (RPL_WHOISSERVER),
                  me.name, parv[0], name, NETWORK_NAME, NETWORK_DESC);
    else
#endif
      sendto_one (sptr, form_str (RPL_WHOISSERVER),
                  me.name, parv[0], name, user->server, 
                  a2cptr ? a2cptr->info : "*Not On This Net*");
    if (user->away)
      sendto_one (sptr, form_str (RPL_AWAY), me.name,
                  parv[0], name, user->away);

    if (IsAnOper (acptr))
      sendto_one (sptr, form_str (RPL_WHOISOPERATOR), me.name, parv[0], name);

#ifndef HIDDEN_ADMIN
    if (IsSetOperAdmin (acptr) && (acptr->umodes & FLAGS_ADMIN))
      sendto_one (sptr, form_str (RPL_WHOISADMIN), me.name, parv[0], name);
#endif /* #ifndef HIDDEN_ADMIN */

#ifdef WHOIS_NOTICE
    if ((MyOper (acptr)) && ((acptr)->umodes & FLAGS_SPY) &&
#ifndef SHOW_REMOTE_WHOIS
	(MyConnect (sptr)) && (IsPerson (sptr)) &&
#endif
        (acptr != sptr))
    { 
#ifdef SHOW_REMOTE_WHOIS
      if (MyConnect (sptr))
      {
#endif /* SHOW_REMOTE_WHOIS */
        sendto_one (acptr,
                    ":%s NOTICE %s :*** Notice -- %s (%s@%s) is doing a /whois on you.",
                    me.name, acptr->name, parv[0], sptr->username,
                    sptr->host);
#ifdef SHOW_REMOTE_WHOIS
      }
      else
      {
        sendto_one (acptr, 
                    ":%s NOTICE %s :*** Notice -- %s (%s@%s) is doing a /whois on you. [%s]",
                    me.name, acptr->name, parv[0], sptr->username,
                    sptr->host, sptr->user->server);
      }
#endif /* SHOW_REMOTE_WHOIS */
    }
#endif /* #ifdef WHOIS_NOTICE */

    if ((acptr->user
#ifdef SERVERHIDE
         && IsAnOper (sptr)
#endif
         && MyConnect (acptr)))
    {
#ifdef WHOISACTUALLY
      if (!MyOper (acptr))
      { 
        const char* ip;
#ifdef HIDE_SPOOF_IPS
        if (IsIPSpoof(acptr))
          ip = "255.255.255.255";
        else
#endif
        ip = inetntoa((const char*) &acptr->ip);
        sendto_one(sptr, form_str(RPL_WHOISACTUALLY),
                   me.name, parv[0], name, ip);
      } 
#endif /* WHOISACTUALLY */
      sendto_one (sptr, form_str (RPL_WHOISIDLE),
                  me.name, parv[0], name,
                  CurrentTime - user->last, acptr->firsttime);
    }
    sendto_one (sptr, form_str (RPL_ENDOFWHOIS), me.name, parv[0], parv[1]);

#ifdef OPERSPY
    if (OperSpyWhois)
    { 
      if (!MyConnect (acptr))
      {
        ircsprintf (osnuh, "%s!%s@%s %s", acptr->name, acptr->username,
                    acptr->host, acptr->user->server);
      }
      else
        ircsprintf (osnuh, "%s!%s@%s", acptr->name, acptr->username,
                    acptr->host);
      operspy_log (cptr, "WHOIS", osnuh);
    }
#endif
    return 0;
  }
  return 0;
}
