/*
 * m_info.c 
 *
 * $Id$
 */
#define DEFINE_M_INFO_DATA
#include "m_commands.h"
#include "m_info.h"
#include "channel.h"
#include "client.h"
#include "common.h"
#include "irc_string.h"
#include "ircd.h"
#include "numeric.h"
#include "s_serv.h"
#include "s_user.h"
#include "send.h"
#include "struct.h"

#include <time.h>
#include <string.h>


/*
** m_info
**  parv[0] = sender prefix
**  parv[1] = servername
*/

int
m_info(aClient *cptr, aClient *sptr, int parc, char *parv[])

{
  char **text = infotext;
  static time_t last_used=0L;
  Info *infoptr;

  if(IsServer(sptr))
    return 0;
  if (hunt_server(cptr,sptr,":%s INFO :%s",1,parc,parv) == HUNTED_ISME)
  {
    sendto_realops_flags(FLAGS_SPY, "info requested by %s (%s@%s) [%s]",
      sptr->name, sptr->username, sptr->host,
      sptr->user->server);

    if (!IsAnOper(sptr))
    {
      /* reject non local requests */
      if (!MyConnect(sptr))
        return 0;
      if ((last_used + PACE_WAIT) > CurrentTime)
      {
        /* safe enough to give this on a local connect only */
        sendto_one(sptr,form_str(RPL_LOAD2HI),me.name,parv[0]);
        return 0;
      }
      else
        last_used = CurrentTime;
    } /* if (!IsAnOper(sptr)) */

    while (*text)
      sendto_one(sptr, form_str(RPL_INFO), me.name, parv[0], *text++);

    /*
     * Now send them a list of all our configuration options
     * (mostly from config.h)
     */
    if (IsAnOper(sptr))
    {
      sendto_one(sptr, form_str(RPL_INFO), me.name, parv[0], "Compile-time configuration options:");

      for (infoptr = MyInformation; infoptr->name; infoptr++)
      {
        if (infoptr->intvalue)
          sendto_one(sptr,
            ":%s %d %s :%-30s %-5d [%-30s]",
            me.name,
            RPL_INFO,
            parv[0],
            infoptr->name,
            infoptr->intvalue,
            infoptr->desc);
        else
          sendto_one(sptr,
            ":%s %d %s :%-30s %-5s [%-30s]",
            me.name,
            RPL_INFO,
            parv[0],
            infoptr->name,
            infoptr->strvalue,
            infoptr->desc);
      }

      sendto_one(sptr, form_str(RPL_INFO), me.name, parv[0], "");

#ifndef SERVERHIDE
      if (IsSetOperAdmin(cptr))
        {
      sendto_one(sptr,
        ":%s %d %s :Compiled on [%s]",
        me.name,
        RPL_INFO,
        parv[0],
        platform);
        } /* if (IsSetOperAdmin(cptr)) - No need to tell the real hostsnames to the world */
#endif
      sendto_one(sptr, form_str(RPL_INFO), me.name, parv[0], "");

    } /* if (IsAnOper(sptr)) */

    sendto_one(sptr,
      ":%s %d %s :Birth Date: %s, compile #%s",
      me.name,
      RPL_INFO,
      parv[0],
      creation,
      generation);

    sendto_one(sptr,
      ":%s %d %s :On-line since %s",
      me.name,
      RPL_INFO,
      parv[0],
      myctime(me.firsttime));

    sendto_one(sptr, form_str(RPL_ENDOFINFO), me.name, parv[0]);
  } /* if (hunt_server(cptr,sptr,":%s INFO :%s",1,parc,parv) == HUNTED_ISME) */

  return 0;
} /* m_info() */
