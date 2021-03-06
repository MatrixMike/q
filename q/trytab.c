/* T R Y T A B
 *
 * Copyright (C) 1981, D. C. Roe
 * Copyright (C) 2012,2014 Duncan Roe
 *
 * Written by Duncan Roe while a staff member & part time student at
 * Caulfield Institute of Technology, Melbourne, Australia.
 * Support from Des Fitzgerald & Associates gratefully acknowledged.
 * Project started 1980.
 *
 * Tries for the T<tabid> or -TO <line_number> number formats
 (usually after decimal conversion has failed).
 The # of the tab must be outside those that ^I can jump to
 */
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include "alledit.h"
#include "edmast.h"
#include "tabs.h"

/* Macros */
#define GIVE_UP goto errlbl

bool
trytab(unsigned char *zbuf, scrbuf5 *scline)
{
  long savlst;                     /* Former value of LSTLIN */

/* Look for -TO */
  if (scline->toklen && !strncasecmp((char *)zbuf, "-TO", scline->toklen))
  {
    if (lstlin <= 0)
    {
      fprintf(stderr, "-TO not allowed in this position");
      GIVE_UP;
    }                              /* if (lstlin <= 0) */
    savlst = lstlin;               /* Will be overwritten by GETLIN */
    if (!getlin(1, 0))
      GIVE_UP;                     /* J back, error reported */
    scline->decval = lstlin - savlst + 1; /* C/vert to a # of lines */
    lstlin = savlst;               /* Reinstate */
    if (scline->decval > 0)
      return true;                 /* Finished - -TO successful */
    fprintf(stderr, "-TO a previous line not allowed");
    GIVE_UP;
  }                                /* if (scline->toklen && ... */

/* Not -TO, try for a Tab ID */
  if (scline->toklen != 2 || toupper(zbuf[0]) != 'T')
    return false;
  return gettab(zbuf[1], true, &scline->decval, false);
errlbl:
  fprintf(stderr, " or ");
  return 0;
}
