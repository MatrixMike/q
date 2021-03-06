/* O R D C H
 *
 * Copyright (C) 1993, Duncan Roe & Associates P/L
 * Copyright (C) 2012,2014 Duncan Roe
 *
 * This routine processes a normal chr (or a special that was preceded
 * by ^C), overwriting/inserting as appropriate.
 */
#include <stdio.h>
#include <signal.h>
#include <memory.h>
#include "alledit.h"
#include "macros.h"
#include "fmode.h"
#include "cmndcmmn.h"
#include "scrnedit.h"

void
ordch(unsigned char chr, scrbuf5 *scbuf)
{
/*
 * Parameters
 * ==========
 *
 * chr   - the character
 * scbuf  - pointer to the scrnedit buffer
 */
  int i, k;                        /* Scratch */
  unsigned char *p, *q;
/*
 * Apart from SCRDIT on encountering a tab (^I),
 * this is the only Q function that can increase the value of the indent point.
 * Do so now, if at the indent point and we have a space.
 */
  if (INDENT && scbuf->bcurs == ndntch && chr == SPACE)
  {
    ndntch++;
/*
 * Move indent over any spaces which now follow, unless inserting ...
 */
    if (!insert)
    {
      for (;;)
      {
        if (scbuf->bcurs == scbuf->bchars)
          break;                   /* B no more chars follow */
        if (scbuf->bdata[scbuf->bcurs + 1] != SPACE)
          break;                   /* B not a space following */
        scbuf->bdata[scbuf->bcurs] = SPACE;
        scbuf->bcurs++;
        ndntch++;
      }
    }
  }
  if (scbuf->bcurs == scbuf->bchars || insert)
  {
    if (scbuf->bchars == scbuf->bmxch)
    {
      fprintf(stderr, "\a%s",
        curmac >= 0 ? "\r\nNo room in line for insert" : "");
      mctrst = false;
      cmover = true;               /* Warn notmac in case input from U-use */
      notmac(true);                   /* Want macro diagnostics */
      return;
    }                              /* if (scbuf->bchars == scbuf->bmxch) */
    if (scbuf->bcurs < scbuf->bchars) /* A real insert */
    {
/* We have to zot the r/h portion of line (incl. cursor chr) up 1. */
/* Use a FOR loop to do the overlapping r/h move */
      p = &scbuf->bdata[scbuf->bchars - 1]; /* Last pick up char */
      q = &scbuf->bdata[scbuf->bchars]; /* Last set down char */
      k = scbuf->bchars - scbuf->bcurs; /* # chars to move */
      for (i = 0; i < k; i++)
        *q-- = *p--;
    }
    scbuf->bchars++;               /* Increase line length */
    if (mxchrs < scbuf->bchars)
      mxchrs = scbuf->bchars;      /* For ^R */
  }
  scbuf->bdata[scbuf->bcurs] = chr; /* Store chr */
  scbuf->bcurs++;                  /* Up cursor */
  return;                          /* Finished */
}
