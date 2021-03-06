/* N E W M A C
 *
 * Copyright (C) 1981,1999,2011, D. C. Roe
 * Copyright (C) 2012,2014 Duncan Roe
 *
 * Written by Duncan Roe while a staff member & part time student at
 * Caulfield Institute of Technology, Melbourne, Australia.
 * Support from Des Fitzgerald & Associates gratefully acknowledged.
 * Project started 1980.
 *
 * This routine stores a new SCREENEDIT macro definition as read by
 * the mainline
 */
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "alledit.h"
#include "edmast.h"
#include "macros.h"
#include "fmode.h"
#include "tabs.h"
#include "alu.h"

/* Macros */
#define GIVE_UP return 0

/* Instantiate externals */
long ALU_memory[01000] = { 0 };
double FPU_memory[01000] = { 0 };

/* Static Variables */

static unsigned char alubuf[64];
static scrbuf5 aluscrbuf;

int
newmac()
{
  int mcchrs;                      /* Chars in macro def'n */
  unsigned char p[14];
/*
 * Q calls us when it sees the NEWMACRO command. We read the next
 * token off the command line - if it is "-" then return a -ve value
 * instructing Q to display / write to a file the macro  expansions.
 * Otherwise, validate the macro ID & read the expansion,  which may not
 * be null. Return 0 if any error occurs, having first o/p an
 * explanatory message. If all OK, return +ve.
 */
/* Get macro name or minus */
  if (scrdtk(1, p, 13, oldcom))
  {
    fprintf(stderr, "%s. reading macro name (scrdtk)", strerror(errno));
    GIVE_UP;
  }
/* A single-character macro name stands for itself (B defines the ^B
 * macro, for instance). A 2-character name must be valid octal, and
 * like the 1-character name is followed by a quoted macro definition.
 * 3 or more characters must be valid octal, but the macro definition is
 * read as raw text.
 * Any macro may be defined by any available method, but the pseudo
 * macros may not be defined. These occupy macros 0100 - 0177 on a
 * standard-Ascii system */
  if (oldcom->toklen == 1)
  {
    verb = p[0];                   /* Get the character typed */
    if (verb >= 'A')
    {
      if (verb <= '_')             /* In range A - _ */
        verb -= 0100;              /* Convert to a control */
      else
      {
        fprintf(stderr, "illegal macro name \"%c\"", (char)verb);
        GIVE_UP;
      }                            /* verb <= '_' else */
    }                              /* if (verb >= 'A') */
    else
/*
 * Look for non control char macro (63-macro enhancement) Anything
 * except an actual control is ok.
 * If we have a "-", return a -ve value so Q can list / type the
 * currently defined macros...
 */
    {
      if (verb == '-')
        return -1;                 /* R macro expansions wanted */
      if (verb == '@')
        verb = '-';                /* Fudge to define "-" macro */
      if (verb < SPACE)
      {                            /* An actual control - illegal */
        fprintf(stderr, "illegal macro name \"^%c\"", (char)(verb + 0100));
        GIVE_UP;
      }
    }                              /* if (verb >= 'A') else */
  }                                /* if (oldcom->toklen == 1) */
  else
  {
/* Look for N--. This is a request to type / list only the alu memory locns */
    if (oldcom->toklen == 2 && !strcmp((char *)p, "--"))
    {
      alu_macros_only = true;
      return -1;
    }                 /* if (oldcom->toklen == 2 && !strcmp((char *)p, "--")) */

    if (!(oldcom->octok))          /* Will pick up zero-length name */
    {
      fprintf(stderr, "macro name \"%s\" neither octal nor single-char", p);
      GIVE_UP;
    }                              /* J not octal after all */
    verb = oldcom->octval;
  }                                /* if (oldcom->toklen == 1) else */
/*
 * Get the macro definition.
 * This is quoted text unless macro name specified by octal >=3 digits
 */
  if (oldcom->toklen <= 2)
  {

/* Old-style macro def'n - macro text is quoted */
    if (scrdtk(2, (unsigned char *)buf, BUFMAX, oldcom))
    {
      fprintf(stderr, "%s. reading macro text (scrdtk)", strerror(errno));
      GIVE_UP;
    }                              /* Get text of macro expansion */
    mcchrs = oldcom->toklen;       /* Remember length of expansion */

/* Check no more tokens */
    scrdtk(1, 0, 0, oldcom);
    if (oldcom->toktyp != eoltok)
    {
      fprintf(stderr, "Too many arguments for this command");
      GIVE_UP;
    }
  }                                /* if (oldcom->toklen <= 2) */
  else
  {
/* Check for being in pseudomacro region or out of range */
    if (
      (!verb || verb > TOPMAC || (verb >= FIRST_PSEUDO && verb <= LAST_PSEUDO))
      && (verb & 07000) != 07000 && verb != 04007 && (verb & 013000) != 013000
      && verb != 04010)
    {
      fprintf(stderr, "Macro %o is reserved or out of range", (int)verb);
      GIVE_UP;
    }
    if (scrdtk(4, (unsigned char *)buf, BUFMAX, oldcom))
    {
      fprintf(stderr, "%s. reading macro text (scrdtk)", strerror(errno));
      GIVE_UP;
    }                              /* R bad token */
    mcchrs = oldcom->toklen;       /* Remember length of expansion */
  }                                /* if (oldcom->toklen <= 2) else */
/*
 * Ensure non-null def'n
 */
  if (!mcchrs)
  {
    fprintf(stderr, "null macro specified");
    GIVE_UP;
  }

/* Setting an ALU or FPU memory location? */
  if ((verb & 07000) == 07000 || (verb & 013000) == 013000)
  {
    char *endptr;
    int idx = verb & 0777;
    long oldval = ALU_memory[idx];
    double oldfpval = FPU_memory[idx];
    char *strbuf = buf;

/* Parse out token from supplied buffer to allow slash star comments */
    scrdtk(5, 0, 0, &aluscrbuf);
    aluscrbuf.bchars = snprintf((char *)aluscrbuf.bdata, BUFMAX, "%s", buf);
    if (aluscrbuf.bchars > 0)
    {
      scrdtk(1, alubuf, sizeof alubuf - 1, &aluscrbuf);
      if (aluscrbuf.toktyp == nortok)
      {
        scrdtk(1, 0, 0, &aluscrbuf);
        if (aluscrbuf.toktyp == eoltok)
          strbuf = (char *)alubuf;
      }                            /* if (aluscrbuf.toktyp == nortok) */
    }                              /* if (aluscrbuf.bchars >0) */

    errno = 0;
    if (verb < 010000)
    {
      ALU_memory[idx] = strtol(strbuf, &endptr, 0);
      if (errno)
      {
        if (errno == ERANGE)
        {
/* Attempt unsigned Hex or Octal entry */
          char *q = buf;

          while (*q == ' ')
            q++;
          if (*q == '0')
          {
            errno = 0;
            *(unsigned long *)(&ALU_memory[idx]) = strtoul(strbuf, &endptr, 0);
          }                        /* if (*q == '0') */
        }                          /* if (errno == ERANGE) */
        if (errno)
        {
          fprintf(stderr, "%s. %s (strtol)", strerror(errno), buf);
          GIVE_UP;
        }                          /* if (errno) */
      }                            /* if (errno) */
      if (!*endptr || (oldcom->toklen == 2 &&
        toupper((unsigned char)buf[0]) == 'T' &&
        gettab(buf[1], false, &ALU_memory[idx], false)))
      {
        if (oldval && !BRIEF && WARN_NONZERO_MEMORY)
          printf("Warning - value was previously %ld\r\n", oldval);
        return 1;                  /* All chars parsed */
      }
      if (*endptr && !(oldcom->toklen == 2 &&
        toupper((unsigned char)buf[0]) == 'T'))
        fprintf(stderr, "Illegal character '%c' in number \"%s\"", *endptr,
          buf);
      else
        fprintf(stderr, "Invalid number format");
      GIVE_UP;
    }                              /* if (verb < 010000) */
    else
    {
      FPU_memory[idx] = strtod(strbuf, &endptr);
      if (errno)
      {
        fprintf(stderr, "%s. %s (strtod)", strerror(errno), buf);
        GIVE_UP;
      }                            /* if (errno) */
      if (!*endptr)
      {
        if (oldfpval != 0.0 && !BRIEF && WARN_NONZERO_MEMORY)
        {
          printf("%s", "Warning - value was previously ");
          printf(FPformat, oldfpval);
          printf("\r\n");
        }
        return 1;                  /* OK */
      }                            /* if (!*endptr) */
          fprintf(stderr, "Illegal character '%c' in number \"%s\"", *endptr,
            buf);
          GIVE_UP;
    }                              /* if (verb < 010000) else */
  }                                /* if (verb & 07000 == 07000) */

/* Defining floating-point format? */
  if (verb == 04007)
  {
    if (mcchrs > sizeof FPformat - 1)
    {
      fprintf(stderr, "%s", "Format string too long");
      GIVE_UP;
    }                              /* if (mcchrs > sizeof FPformat -1) */
    strncpy(FPformat, buf, mcchrs);
    FPformat[mcchrs] = 0;
    return 1;
  }                                /* if (verb == 04007) */
/* Defining date format? */
  if (verb == 04010)
  {
    if (mcchrs > sizeof DTformat - 1)
    {
      fprintf(stderr, "%s", "Format string too long");
      GIVE_UP;
    }                              /* if (mcchrs > sizeof FPformat -1) */
    strncpy(DTformat, buf, mcchrs);
    DTformat[mcchrs] = 0;
    return 1;
  }                                /* if (verb == 04010) */

/* Advise user if an existing macro being overwritten */
  if (scmacs[verb] && (curmac < 0 || !BRIEF))
    printf("Warning - overwriting old macro\r\n");
  return newmac2(false) ? 1 : 0;
}
