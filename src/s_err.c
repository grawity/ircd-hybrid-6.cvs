/************************************************************************
 *   IRC - Internet Relay Chat, src/s_err.c
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
 */

/*
 *	I kind of modernized this code a bit. -Dianora
 */

#include "struct.h"
#include "numeric.h"
#include "h.h"

#ifndef lint
static  char sccsid[] = "@(#)s_err.c	1.11 5/17/93 (C) 1992 Darren Reed";
static char *rcs_version = "$Id: s_err.c,v 1.2 1998/09/24 02:33:33 db Exp $";
#endif

static	char	numbuff[512];

#ifdef CUSTOM_ERR
#include "s_err.h"
#else
static	char *	replies[] = {
/* 000 */	(char *)NULL,
/* 001	RPL_WELCOME, */	":%s 001 %s :Welcome to the Internet Relay Network %s",
/* 002	RPL_YOURHOST,*/	":%s 002 %s :Your host is %s, running version %s",
/* 003 	RPL_CREATED, */ ":%s 003 %s :This server was created %s",
/* 004 	RPL_MYINFO, */ ":%s 004 %s %s %s oiwszcrkfydn biklmnopstv",
/* 005 */	(char *)NULL,
/* 006 */	(char *)NULL,
/* 007 */	(char *)NULL,
/* 008 */	(char *)NULL,
/* 009 */	(char *)NULL,
/* 010 */	(char *)NULL,
/* 011 */	(char *)NULL,
/* 012 */	(char *)NULL,
/* 013 */	(char *)NULL,
/* 014 */	(char *)NULL,
/* 015 */	(char *)NULL,
/* 016 */	(char *)NULL,
/* 017 */	(char *)NULL,
/* 018 */	(char *)NULL,
/* 019 */	(char *)NULL,
/* 020 */	(char *)NULL,
/* 021 */	(char *)NULL,
/* 022 */	(char *)NULL,
/* 023 */	(char *)NULL,
/* 024 */	(char *)NULL,
/* 025 */	(char *)NULL,
/* 026 */	(char *)NULL,
/* 027 */	(char *)NULL,
/* 028 */	(char *)NULL,
/* 029 */	(char *)NULL,
/* 030 */	(char *)NULL,
/* 031 */	(char *)NULL,
/* 032 */	(char *)NULL,
/* 033 */	(char *)NULL,
/* 034 */	(char *)NULL,
/* 035 */	(char *)NULL,
/* 036 */	(char *)NULL,
/* 037 */	(char *)NULL,
/* 038 */	(char *)NULL,
/* 039 */	(char *)NULL,
/* 040 */	(char *)NULL,
/* 041 */	(char *)NULL,
/* 042 */	(char *)NULL,
/* 043 */	(char *)NULL,
/* 044 */	(char *)NULL,
/* 045 */	(char *)NULL,
/* 046 */	(char *)NULL,
/* 047 */	(char *)NULL,
/* 048 */	(char *)NULL,
/* 049 */	(char *)NULL,
/* 050 */	(char *)NULL,
/* 051 */	(char *)NULL,
/* 052 */	(char *)NULL,
/* 053 */	(char *)NULL,
/* 054 */	(char *)NULL,
/* 055 */	(char *)NULL,
/* 056 */	(char *)NULL,
/* 057 */	(char *)NULL,
/* 058 */	(char *)NULL,
/* 059 */	(char *)NULL,
/* 060 */	(char *)NULL,
/* 061 */	(char *)NULL,
/* 062 */	(char *)NULL,
/* 063 */	(char *)NULL,
/* 064 */	(char *)NULL,
/* 065 */	(char *)NULL,
/* 066 */	(char *)NULL,
/* 067 */	(char *)NULL,
/* 068 */	(char *)NULL,
/* 069 */	(char *)NULL,
/* 070 */	(char *)NULL,
/* 071 */	(char *)NULL,
/* 072 */	(char *)NULL,
/* 073 */	(char *)NULL,
/* 074 */	(char *)NULL,
/* 075 */	(char *)NULL,
/* 076 */	(char *)NULL,
/* 077 */	(char *)NULL,
/* 078 */	(char *)NULL,
/* 079 */	(char *)NULL,
/* 080 */	(char *)NULL,
/* 081 */	(char *)NULL,
/* 082 */	(char *)NULL,
/* 083 */	(char *)NULL,
/* 084 */	(char *)NULL,
/* 085 */	(char *)NULL,
/* 086 */	(char *)NULL,
/* 087 */	(char *)NULL,
/* 088 */	(char *)NULL,
/* 089 */	(char *)NULL,
/* 090 */	(char *)NULL,
/* 091 */	(char *)NULL,
/* 092 */	(char *)NULL,
/* 093 */	(char *)NULL,
/* 094 */	(char *)NULL,
/* 095 */	(char *)NULL,
/* 096 */	(char *)NULL,
/* 097 */	(char *)NULL,
/* 098 */	(char *)NULL,
/* 099 */	(char *)NULL,
/* 100 */	(char *)NULL,
/* 101 */	(char *)NULL,
/* 102 */	(char *)NULL,
/* 103 */	(char *)NULL,
/* 104 */	(char *)NULL,
/* 105 */	(char *)NULL,
/* 106 */	(char *)NULL,
/* 107 */	(char *)NULL,
/* 108 */	(char *)NULL,
/* 109 */	(char *)NULL,
/* 110 */	(char *)NULL,
/* 111 */	(char *)NULL,
/* 112 */	(char *)NULL,
/* 113 */	(char *)NULL,
/* 114 */	(char *)NULL,
/* 115 */	(char *)NULL,
/* 116 */	(char *)NULL,
/* 117 */	(char *)NULL,
/* 118 */	(char *)NULL,
/* 119 */	(char *)NULL,
/* 120 */	(char *)NULL,
/* 121 */	(char *)NULL,
/* 122 */	(char *)NULL,
/* 123 */	(char *)NULL,
/* 124 */	(char *)NULL,
/* 125 */	(char *)NULL,
/* 126 */	(char *)NULL,
/* 127 */	(char *)NULL,
/* 128 */	(char *)NULL,
/* 129 */	(char *)NULL,
/* 130 */	(char *)NULL,
/* 131 */	(char *)NULL,
/* 132 */	(char *)NULL,
/* 133 */	(char *)NULL,
/* 134 */	(char *)NULL,
/* 135 */	(char *)NULL,
/* 136 */	(char *)NULL,
/* 137 */	(char *)NULL,
/* 138 */	(char *)NULL,
/* 139 */	(char *)NULL,
/* 140 */	(char *)NULL,
/* 141 */	(char *)NULL,
/* 142 */	(char *)NULL,
/* 143 */	(char *)NULL,
/* 144 */	(char *)NULL,
/* 145 */	(char *)NULL,
/* 146 */	(char *)NULL,
/* 147 */	(char *)NULL,
/* 148 */	(char *)NULL,
/* 149 */	(char *)NULL,
/* 150 */	(char *)NULL,
/* 151 */	(char *)NULL,
/* 152 */	(char *)NULL,
/* 153 */	(char *)NULL,
/* 154 */	(char *)NULL,
/* 155 */	(char *)NULL,
/* 156 */	(char *)NULL,
/* 157 */	(char *)NULL,
/* 158 */	(char *)NULL,
/* 159 */	(char *)NULL,
/* 160 */	(char *)NULL,
/* 161 */	(char *)NULL,
/* 162 */	(char *)NULL,
/* 163 */	(char *)NULL,
/* 164 */	(char *)NULL,
/* 165 */	(char *)NULL,
/* 166 */	(char *)NULL,
/* 167 */	(char *)NULL,
/* 168 */	(char *)NULL,
/* 169 */	(char *)NULL,
/* 170 */	(char *)NULL,
/* 171 */	(char *)NULL,
/* 172 */	(char *)NULL,
/* 173 */	(char *)NULL,
/* 174 */	(char *)NULL,
/* 175 */	(char *)NULL,
/* 176 */	(char *)NULL,
/* 177 */	(char *)NULL,
/* 178 */	(char *)NULL,
/* 179 */	(char *)NULL,
/* 180 */	(char *)NULL,
/* 181 */	(char *)NULL,
/* 182 */	(char *)NULL,
/* 183 */	(char *)NULL,
/* 184 */	(char *)NULL,
/* 185 */	(char *)NULL,
/* 186 */	(char *)NULL,
/* 187 */	(char *)NULL,
/* 188 */	(char *)NULL,
/* 189 */	(char *)NULL,
/* 190 */	(char *)NULL,
/* 191 */	(char *)NULL,
/* 192 */	(char *)NULL,
/* 193 */	(char *)NULL,
/* 194 */	(char *)NULL,
/* 195 */	(char *)NULL,
/* 196 */	(char *)NULL,
/* 197 */	(char *)NULL,
/* 198 */	(char *)NULL,
/* 199 */	(char *)NULL,
/* 200 RPL_TRACELINK, */	":%s 200 %s Link %s%s %s %s",
/* 201 RPL_TRACECONNECTING, */	":%s 201 %s Try. %d %s",
/* 202 RPL_TRACEHANDSHAKE, */	":%s 202 %s H.S. %d %s",
/* 203 RPL_TRACEUNKNOWN, */	":%s 203 %s ???? %d %s %d",
/* 204 RPL_TRACEOPERATOR, */	":%s 204 %s Oper %d %s",
/* 205 RPL_TRACEUSER, */	":%s 205 %s User %d %s 0 %d",
/* 206 RPL_TRACESERVER, */	":%s 206 %s Serv %d %dS %dC %s %s!%s@%s",
/* 207 */	(char *)NULL,
/* 208 RPL_TRACENEWTYPE, */	":%s 208 %s <newtype> 0 %s",
/* 209 RPL_TRACECLASS, */	":%s 209 %s Class %d %d",
/* 210 */	(char *)NULL,
/* 211 RPL_STATSLINKINFO, */	(char *)NULL,
/* 212 RPL_STATSCOMMANDS, */	":%s 212 %s %s %u %u",
/* 213 RPL_STATSCLINE, */	":%s 213 %s %c %s * %s %d %d",
/* 214 RPL_STATSNLINE, */	":%s 214 %s %c %s * %s %d %d",
/* 215 RPL_STATSILINE, */	":%s 215 %s %c %s %s@%s * %d %d",
/* 216 RPL_STATSKLINE, */	":%s 216 %s %c %s * %s %s",
/* 217 RPL_STATSQLINE, */	":%s 217 %s %s %s * * 0 0",
/* 218 RPL_STATSYLINE, */	":%s 218 %s %c %d %d %d %d %ld",
/* 219 RPL_ENDOFSTATS, */	":%s 219 %s %c :End of /STATS report",
/* 220 */	 (char *)NULL,
/* 221 RPL_UMODEIS, */		":%s 221 %s %s",
/* 222 RPL_STATSXLINE, */	":%s 222 %s %s %s * * 0 0",
/* 223 */	(char *)NULL,
/* 224 */	(char *)NULL,
/* 225 */	(char *)NULL,
/* 226 */	(char *)NULL,
/* 227 */	(char *)NULL,
/* 228 */	(char *)NULL,
/* 229 */	(char *)NULL,
/* 230 */	(char *)NULL,
/* 231 */	(char *)NULL,
/* 232 */	(char *)NULL,
/* 233 */	(char *)NULL,
/* 234 RPL_SERVLIST, */ (char *)NULL,
/* 235 RPL_SERVLISTEND, */ (char *)NULL,
/* 236 */	(char *)NULL,
/* 237 */	(char *)NULL,
/* 238 */	(char *)NULL,
/* 239 */	(char *)NULL,
/* 240 */	(char *)NULL,
/* 241 RPL_STATSLLINE, */	":%s 241 %s %c %s * %s %d %d",
/* 242 RPL_STATSUPTIME,*/	":%s 242 %s :Server Up %d days, %d:%02d:%02d",
/* 243 RPL_STATSOLINE, */	":%s 243 %s %c %s * %s %s %d",
/* 244 RPL_STATSHLINE, */	":%s 244 %s %c %s * %s %d %d", 
/* 245 RPL_STATSSLINE, */	":%s 245 %s %c %s * %s %d %d", 
/* 246 */	(char *)NULL,
/* 247 */	(char *)NULL,
/* 248 */	(char *)NULL,
/* 249 */	(char *)NULL,
#ifdef HIGHEST_CONNECTION
/* 250 RPL_STATSCONN, */
                    ":%s 250 %s :Highest connection count: %d (%d clients)",
#else
/* 250 */	(char *)NULL,
#endif
/* 251 RPL_LUSERCLIENT, */
		":%s 251 %s :There are %d users and %d invisible on %d servers",
/* 252 RPL_LUSEROP, */		":%s 252 %s %d :IRC Operators online",
/* 253 RPL_LUSERUNKNOWN, */	":%s 253 %s %d :unknown connection(s)",
/* 254 RPL_LUSERCHANNELS, */	":%s 254 %s %d :channels formed",
/* 255 RPL_LUSERME, */		":%s 255 %s :I have %d clients and %d servers",
/* 256 RPL_ADMINME, */		":%s 256 %s :Administrative info about %s",
/* 257 RPL_ADMINLOC1, */	":%s 257 %s :%s",
/* 258 RPL_ADMINLOC2, */	":%s 258 %s :%s",
/* 259 RPL_ADMINEMAIL, */	":%s 259 %s :%s",
/* 260 */	(char *)NULL,
/* 261 RPL_TRACELOG, */		":%s 261 %s File %s %d",
/* 262 RPL_ENDOFTRACE, */	":%s 262 %s %s :End of TRACE",
/* 263 RPL_LOAD2HI, */		
":%s 263 %s :Server load is temporarily too heavy. Please wait a while and try again.",
/* 264 */	(char *)NULL,
/* 265 RPL_LOCALUSERS, */	":%s 265 %s :Current local  users: %d  Max: %d",
/* 266 RPL_GLOBALUSERS, */	":%s 266 %s :Current global users: %d  Max: %d",
/* 267 */	(char *)NULL,
/* 268 */	(char *)NULL,
/* 269 */	(char *)NULL,
/* 270 */	(char *)NULL,
/* 271 */	(char *)NULL,
/* 272 */	(char *)NULL,
/* 273 */	(char *)NULL,
/* 274 */	(char *)NULL,
/* 275 */	(char *)NULL,
/* 276 */	(char *)NULL,
/* 277 */	(char *)NULL,
/* 278 */	(char *)NULL,
/* 279 */	(char *)NULL,
/* 280 */	(char *)NULL,
/* 281 */	(char *)NULL,
/* 281 */	(char *)NULL,
/* 282 */	(char *)NULL,
/* 283 */	(char *)NULL,
/* 284 */	(char *)NULL,
/* 285 */	(char *)NULL,
/* 286 */	(char *)NULL,
/* 287 */	(char *)NULL,
/* 288 */	(char *)NULL,
/* 289 */	(char *)NULL,
/* 290 */	(char *)NULL,
/* 291 */	(char *)NULL,
/* 292 */	(char *)NULL,
/* 293 */	(char *)NULL,
/* 294 */	(char *)NULL,
/* 295 */	(char *)NULL,
/* 296 */	(char *)NULL,
/* 298 */	(char *)NULL,
/* 299 */	(char *)NULL,
/* 300 RPL_NONE, */	 (char *)NULL,
/* 301 RPL_AWAY, */	 ":%s 301 %s %s :%s",
/* 302 RPL_USERHOST, */	 ":%s 302 %s :",
/* 303 RPL_ISON, */	":%s 303 %s :",
/* 304 RPL_TEXT, */	 (char *)NULL,
/* 305 RPL_UNAWAY, */	":%s 305 %s :You are no longer marked as being away",
/* 306 RPL_NOWAWAY, */	":%s 306 %s :You have been marked as being away",
/* 307 */	(char *)NULL,
/* 308 */	(char *)NULL,
/* 309 */	(char *)NULL,
/* 310 */	(char *)NULL,
/* 311 RPL_WHOISUSER, */	":%s 311 %s %s %s %s * :%s",
/* 312 RPL_WHOISSERVER, */	":%s 312 %s %s %s :%s",
/* 313 RPL_WHOISOPERATOR, */	":%s 313 %s %s :is an IRC Operator",
/* 314 RPL_WHOWASUSER, */	":%s 314 %s %s %s %s * :%s",
/* 315 RPL_ENDOFWHO, */		":%s 315 %s %s :End of /WHO list.",
/* 316 RPL_WHOISCHANOP, */	(char *)NULL,
/* 317 RPL_WHOISIDLE, */	":%s 317 %s %s %ld %ld :seconds idle, signon time",
/* 318 RPL_ENDOFWHOIS, */	":%s 318 %s %s :End of /WHOIS list.",
/* 319 RPL_WHOISCHANNELS, */	":%s 319 %s %s :%s",
/* 320 */	(char *)NULL,
/* 321 RPL_LISTSTART, */	":%s 321 %s Channel :Users  Name",
/* 322 RPL_LIST, */		":%s 322 %s %s %d :%s",
/* 323 RPL_LISTEND, */		":%s 323 %s :End of /LIST",
/* 324 RPL_CHANNELMODEIS, */	":%s 324 %s %s %s %s",
/* 325 */	(char *)NULL,
/* 326 */	(char *)NULL,
/* 327 */	(char *)NULL,
/* 328 */	(char *)NULL,
/* 329 RPL_CREATIONTIME, */	 ":%s 329 %s %s %lu", 
/* 330 */	(char *)NULL,
/* 331 RPL_NOTOPIC, */		":%s 331 %s %s :No topic is set.",
/* 332 RPL_TOPIC, */		":%s 332 %s %s :%s",
#ifdef TOPIC_INFO
/* 333 RPL_TOPICWHOTIME, */	":%s 333 %s %s %s %lu",
#else
/* 333 */       (char *)NULL,
#endif
/* 334 */	(char *)NULL,
/* 335 */	(char *)NULL,
/* 336 */	(char *)NULL,
/* 337 */	(char *)NULL,
/* 338 */	(char *)NULL,
/* 339 */	(char *)NULL,
/* 340 */	(char *)NULL,
/* 341 RPL_INVITING, */		":%s 341 %s %s %s",
/* 342 RPL_SUMMONING, */ (char *)NULL,
/* 343 */	(char *)NULL,
/* 344 */	(char *)NULL,
/* 345 */	(char *)NULL,
/* 346 */	(char *)NULL,
/* 347 */	(char *)NULL,
#ifdef BAN_INFO
/* 348 RPL_EXCEPTLIST */	":%s 348 %s %s %s %s %lu",
#else
/* 348 RPL_EXCEPTLIST */	":%s 348 %s %s %s",
#endif
/* 349 RPL_ENDOFEXCEPTLIST */	":%s 349 %s %s :End of Channel Exception List",
/* 350 */	(char *)NULL,
/* 351 RPL_VERSION, */		":%s 351 %s %s.%s %s :%s",
/* 352 RPL_WHOREPLY, */		":%s 352 %s %s %s %s %s %s %s :%d %s",
/* 353 RPL_NAMREPLY, */		":%s 353 %s %s",
/* 354 */	(char *)NULL,
/* 355 */	(char *)NULL,
/* 356 */	(char *)NULL,
/* 357 */	(char *)NULL,
/* 358 */	(char *)NULL,
/* 359 */	(char *)NULL,
/* 360 */	(char *)NULL,
/* 361 RPL_KILLDONE, */ (char *)NULL,
/* 362 RPL_CLOSING, */		":%s 362 %s %s :Closed. Status = %d",
/* 363 RPL_CLOSEEND, */		":%s 363 %s %d: Connections Closed",
/* 364 RPL_LINKS, */		":%s 364 %s %s %s :%d %s",
/* 365 RPL_ENDOFLINKS, */	":%s 365 %s %s :End of /LINKS list.",
/* 366 RPL_ENDOFNAMES, */	":%s 366 %s %s :End of /NAMES list.",
#ifdef BAN_INFO
/* 367 RPL_BANLIST, */		":%s 367 %s %s %s %s %lu",
#else
/* 367 RPL_BANLIST, */		":%s 367 %s %s %s",
#endif
/* 368 RPL_ENDOFBANLIST, */	":%s 368 %s %s :End of Channel Ban List",
/* 369 RPL_ENDOFWHOWAS, */	":%s 369 %s %s :End of WHOWAS",
/* 370 */	(char *)NULL,
/* 371 RPL_INFO, */		":%s 371 %s :%s",
/* 372 RPL_MOTD, */		":%s 372 %s :- %s",
/* 373 RPL_INFOSTART, */	":%s 373 %s :Server INFO",
/* 374 RPL_ENDOFINFO, */	":%s 374 %s :End of /INFO list.",
/* 375 RPL_MOTDSTART, */	":%s 375 %s :- %s Message of the Day - ",
/* 376 RPL_ENDOFMOTD, */	":%s 376 %s :End of /MOTD command.",
/* 377 */	(char *)NULL,
/* 378 */	(char *)NULL,
/* 379 */	(char *)NULL,
/* 380 */	(char *)NULL,
/* 381 RPL_YOUREOPER, */	":%s 381 %s :You have entered... the Twilight Zone!.",
/* 382 RPL_REHASHING, */	":%s 382 %s %s :Rehashing",
/* 383 */	(char *)NULL,
/* 384 RPL_MYPORTIS, */		":%s 384 %s %d :Port to local server is\r\n",
/* 385 RPL_NOTOPERANYMORE, */	(char *)NULL,
/* 386 */	(char *)NULL,
/* 387 */	(char *)NULL,
/* 388 */	(char *)NULL,
/* 389 */	(char *)NULL,
/* 390 */	(char *)NULL,
/* 391 RPL_TIME, */		":%s 391 %s %s :%s",
/* 392 */	(char *)NULL,
/* 393 */	(char *)NULL,
/* 394 */	(char *)NULL,
/* 395 */	(char *)NULL,
/* 396 */	(char *)NULL,
/* 397 */	(char *)NULL,
/* 398 */	(char *)NULL,
/* 399 */	(char *)NULL,
/* 400 */	(char *)NULL,
/* 401 ERR_NOSUCHNICK, */	":%s 401 %s %s :No such nick/channel",
/* 402 ERR_NOSUCHSERVER, */	":%s 402 %s %s :No such server",
/* 403 ERR_NOSUCHCHANNEL, */	":%s 403 %s %s :No such channel",
/* 404 ERR_CANNOTSENDTOCHAN, */	":%s 404 %s %s :Cannot send to channel",
/* 405 ERR_TOOMANYCHANNELS, */	":%s 405 %s %s :You have joined too many channels",
/* 406 ERR_WASNOSUCHNICK, */	":%s 406 %s %s :There was no such nickname",
/* 407 ERR_TOOMANYTARGETS, */
		":%s 407 %s %s :Duplicate recipients. No message delivered",
/* 408 */	(char *)NULL,
/* 409 ERR_NOORIGIN, */		":%s 409 %s :No origin specified",
/* 410 */	(char *)NULL,
/* 411 ERR_NORECIPIENT, */	":%s 411 %s :No recipient given (%s)",
/* 412 ERR_NOTEXTTOSEND, */	":%s 412 %s :No text to send",
/* 413 ERR_NOTOPLEVEL, */	":%s 413 %s %s :No toplevel domain specified",
/* 414 ERR_WILDTOPLEVEL, */	":%s 414 %s %s :Wildcard in toplevel Domain",
/* 415 */	(char *)NULL,
/* 416 */	(char *)NULL,
/* 417 */	(char *)NULL,
/* 418 */	(char *)NULL,
/* 419 */	(char *)NULL,
/* 420 */	(char *)NULL,
/* 421 ERR_UNKNOWNCOMMAND, */	":%s 421 %s %s :Unknown command",
/* 422 ERR_NOMOTD, */		":%s 422 %s :MOTD File is missing",
/* 423 ERR_NOADMININFO, */
		":%s 423 %s %s :No administrative info available",
/* 424 ERR_FILEERROR, */	":%s 424 %s :File error doing %s on %s",
/* 425 */	(char *)NULL,
/* 426 */	(char *)NULL,
/* 427 */	(char *)NULL,
/* 428 */	(char *)NULL,
/* 429 */	(char *)NULL,
/* 430 */	(char *)NULL,
/* 431 ERR_NONICKNAMEGIVEN, */	":%s 431 %s :No nickname given",
/* 432 ERR_ERRONEUSNICKNAME, */	":%s 432 %s %s :Erroneus Nickname",
/* 433 ERR_NICKNAMEINUSE, */	":%s 433 %s %s :Nickname is already in use.",
/* 434 */	(char *)NULL,
/* 435 */	(char *)NULL,
/* 436 ERR_NICKCOLLISION, */	":%s 436 %s %s :Nickname collision KILL",
/* 437 */	(char *)NULL,
/* 438 */	(char *)NULL,
/* 439 */	(char *)NULL,
/* 440 */	(char *)NULL,
/* 441 ERR_USERNOTINCHANNEL, */	":%s 441 %s %s %s :They aren't on that channel",
/* 442 ERR_NOTONCHANNEL, */	":%s 442 %s %s :You're not on that channel",
/* 443 ERR_USERONCHANNEL, */	":%s 443 %s %s %s :is already on channel",
/* 444 ERR_NOLOGIN, */		":%s 444 %s %s :User not logged in",
/* 445 ERR_SUMMONDISABLED, */	":%s 445 %s :SUMMON has been removed",
/* 446 ERR_USERSDISABLED, */	":%s 446 %s :USERS has been removed",
/* 447 */	(char *)NULL,
/* 448 */	(char *)NULL,
/* 449 */	(char *)NULL,
/* 450 */	(char *)NULL,
/* 451 ERR_NOTREGISTERED, */	":%s 451 %s :You have not registered",
/* 452 */	(char *)NULL,
/* 453 */	(char *)NULL,
/* 454 */	(char *)NULL,
/* 455 */	(char *)NULL,
/* 456 */	(char *)NULL,
/* 457 */	(char *)NULL,
/* 458 */	(char *)NULL,
/* 459 */	(char *)NULL,
/* 460 */	(char *)NULL,
/* 461 ERR_NEEDMOREPARAMS, */	":%s 461 %s %s :Not enough parameters",
/* 462 ERR_ALREADYREGISTRED, */	":%s 462 %s :You may not reregister",
/* 463 ERR_NOPERMFORHOST, */	":%s 463 %s :Your host isn't among the privileged",
/* 464 ERR_PASSWDMISMATCH, */	":%s 464 %s :Password Incorrect",
/* 465 ERR_YOUREBANNEDCREEP, */	":%s 465 %s :You are banned from this server- %s",
/* 466 ERR_YOUWILLBEBANNED, */	(char *)NULL,
/* 467 ERR_KEYSET, */		":%s 467 %s %s :Channel key already set",
/* 468 */	(char *)NULL,
/* 469 */	(char *)NULL,
/* 470 */	(char *)NULL,
/* 471 ERR_CHANNELISFULL, */	":%s 471 %s %s :Cannot join channel (+l)",
/* 472 ERR_UNKNOWNMODE  , */ 	":%s 472 %s %c :is unknown mode char to me",
/* 473 ERR_INVITEONLYCHAN, */	":%s 473 %s %s :Cannot join channel (+i)",
/* 474 ERR_BANNEDFROMCHAN, */	":%s 474 %s %s :Cannot join channel (+b)",
/* 475 ERR_BADCHANNELKEY, */	":%s 475 %s %s :Cannot join channel (+k)",
/* 476 ERR_BADCHANMASK, */	":%s 476 %s %s :Bad Channel Mask",
#ifdef NO_JOIN_ON_SPLIT
/* 477 ERR_NOJOINSPLIT */	":%s 477 %s %s :Cannot join channel on split",
#else
/* 477 */	(char *)NULL,
#endif
/* 478 ERR_BANLISTFULL, */	":%s 478 %s :Channel ban list is full",
/* 479 */	(char *)NULL,
/* 480 */	(char *)NULL,
/* 481 ERR_NOPRIVILEGES, */
		":%s 481 %s :Permission Denied- You're not an IRC operator",
/* 482 ERR_CHANOPRIVSNEEDED, */	":%s 482 %s %s :You're not channel operator",
/* 483 ERR_CANTKILLSERVER, */	":%s 483 %s :You cant kill a server!",
/* 484 */	(char *)NULL,
/* 485 */	(char *)NULL,
/* 486 */	(char *)NULL,
/* 487 */	(char *)NULL,
/* 488 */	(char *)NULL,
/* 489 */	(char *)NULL,
/* 490 */	(char *)NULL,
/* 491 ERR_NOOPERHOST, */	":%s 491 %s :Only few of mere mortals may try to enter the twilight zone",
/* 492 */	(char *)NULL,
/* 493 */	(char *)NULL,
/* 494 */	(char *)NULL,
/* 495 */	(char *)NULL,
/* 496 */	(char *)NULL,
/* 497 */	(char *)NULL,
/* 498 */	(char *)NULL,
/* 499 */	(char *)NULL,
/* 500 */	(char *)NULL,
/* 501 ERR_UMODEUNKNOWNFLAG, */	":%s 501 %s :Unknown MODE flag",
/* 502 ERR_USERSDONTMATCH, */ 	":%s 502 %s :Cant change mode for other users",
/* 503 ERR_GHOSTEDCLIENT, */	":%s 503 %s :Message could not be delivered to %s",
/* 504 LAST ERR_LAST_ERR_MSG, */	":%s 504 %s :Last Error Message",
		(char *)NULL
};
#endif /* CUSTOM_ERR */

/*
 * The observant will note that err_str and rpl_str
 * could be replaced by one function now. 
 * -Dianora
 */

char	*err_str(int numeric)
{
  register char *nptr;

  if ( (numeric < 401) || (numeric > ERR_LAST_ERR_MSG))
    {
      (void)ircsprintf(numbuff,
		       ":%%s %d %%s :INTERNAL ERROR: BAD NUMERIC! %d",
		       numeric, numeric);
      return(numbuff);
    }

  nptr = replies[numeric];
  if (!nptr)
    {
      (void)ircsprintf(numbuff,
		       ":%%s %d %%s :NO ERROR FOR NUMERIC ERROR %d",
		       numeric, numeric);
      return(numbuff);
    }
  else
    return(nptr);
}


char	*rpl_str(int numeric)
{
  register char *nptr;
  
  if((numeric < 0) || (numeric >399))
    {
      (void)ircsprintf(numbuff,
		       ":%%s %d %%s :INTERNAL REPLY ERROR: BAD NUMERIC! %d",
		       numeric, numeric);
      return(numbuff);
    }

  nptr = replies[numeric];
  
  if (!nptr)
    {
      (void)ircsprintf(numbuff,
		       ":%%s %d %%s :NO REPLY FOR NUMERIC ERROR %d",
		       numeric, numeric);
      return(numbuff);
    }
  else
    return(nptr);
}

