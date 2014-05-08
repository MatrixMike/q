fd y /* C macros that differ from default FTN ones
N202 ^NC^NS^N\q ^ND^N^<4003>/scmacs.c^J^NU
N203 ^NC^NS^N\u ^ND^N^<4003>/scmacs.c^J^NU
t3 36 81
O Y
FC N
ft_
N ' ' !make^J^NU
N A '^H^NA^NS^NL^L^N^<421>^@^ND^N^U^Z^Y^Y^Y^NE^E^NG ^NU^X ^NU ;Continuations
N H '^NC^N\^?^?^NU ; Outdent amount (should match ^N^I)
N I '  ^NU ; indent amount
N Y '^NE^H^NA^NS^N^<422>^L^E/*  */^Y^Y^Y^NU ;C comment
N Z '^Z^b^B^S^J
N417 ^NBa ^@^NBa^N^<417>
N420 ^Y^Y^Y^NG ^D^Y^X^T^E^N^<417>
N421 ^NBb ^@^NBb^N^<421>
N422 ^NOb^Z^NB2^NS^T^I^I^NB2^I^@^E/*  */^Y^Y^Y^NU
* n 0   "beautify" C source. Comments are aligned on cc36 or one space
*       after source end if the line is too long, or if the comment is
*       too long then it's aligned to finish on cc72. If neither can be
*       done, comment is split off and put at start of previous line. N0
*       sets up tab b to the current file pos'n and tab c to be cc2...
N 0 '^NC^NS^N\t3 36 73^N^<415>'
N 1 '^NC^NS^N\t3 36 81^N^<415>'
N 6 '^H^O^NA^N^<420>^NOa^Z^N^<420> ;Continue comment nxt lin
N415 ^J^ND^N^<4000>^NM^<403>^Ufb^J^NFbo y^Ji1^Jx^NOc^[^ND^N^<404>o n^Jy ' * /*' ' *',,,,1^Jg tb^Jfm ^ND^N^<403>^J^NU
*      a) if "case" found at cc 1, look for next
*      b) remember where cursor is, move to line start (i.e.indent)
*      c) if we are not now before remembered position, ^L white space
*      d) look for next
N403 !! SCRATCH - used by ^N0,
* n404: Main loop on lines. Remember cursor pos'n (so we can detect if
*       we get back to line start, move to comment start, next line if
*       now at end, backup to delimiter, jump off to submacro if now at
*       line start.
N404 ^[m-^J^NC^NU^NOa^O^NA^N^<414>^B^NPa^N^<407>^N^<405>
* n414 - line has no trailing comment but check for it being just /*
N414 ^H^NG/^NS^N^<404>^X^NG*^NS^N^<404>^H^N^<405>
* n407: Enter a different submacro if not past tab 2. Otherwise, check
*       for 2 spaces before delimiter and if found, delete the second.
*       If 2 spaces were not found, enter another submacro. Has been
*       split at n413, where n412 may jump to
N407 ^NP2^N^<413>^N^<406>
N413 ^Y^NG ^NS^N^<410>^Y^NG ^NS^N^<410>^F^?^N^<407>
* n410: Check comment doesn't extend beyond cc72, split off if it does.
N410 ^Z^NP3^NL^J^N^<404>X^H^O^B^T^L^Jr-1 -2^J^N^<404>
* n406: If before tab 2, insert spaces until at tab 2, if there is room
N406 ^NB2^NS^N^<412>^Z^NP3^N^<412>^NB3^NL^J^N^<404>X^H^O^B^NE^E ^N^<406>
* n405: Start of comment sequence. Move this comment to line start
N405 ^L^J^N^<411>
* n411: if this line starts '*', indent it one space only. Otherwise,
*       have the line re-examined as a normal line. We take some care
*       that if the indent is already right, the the line is not
*       modified.
N411 ^[m-^J^NC^NU^E^NG*^NS^NL^NJ^<10>X^[g-1^J^N^<404>^NBc^NS^NL^NJ^<10>X^NPc^NS^N^<411>^L ^J^N^<411>
* n412: The comment starts at cc36 (now). Just in case it started off
*        there and was too long, check...
N412 ^Z^NP3^NL^J^N^<404>X^H^O^B^N^<413>
*
* n212 - Put a box around the current block of comments. Can invoke from last
*        line in modify mode or just after in command mode.
* TABS USED
*
* a - line# after last line
* b - line# of first line
* c - maximum line length (pos'n of space before */)
*
* MACROS DEFINED
*
* n212, n677 down to n671
*
N212 ^[^NFam-1^J^ND^N^<677>^N\^NOc^[^ND^N^<676>^[^NFb^ND^N^<675>^ND^N^<673>i ta^J^ND^N^<672>^J^[i tb^J^L^ND^N^<672>^J^[g ta^Jg+2^J^NU
* n677 - Return if non-comment-line else incremented return (all in modify mode)
N677 ^NG/^NS^NU^X^NG*^NS^NU^X^NG ^NS^NU^Z^Y^NG/^NS^NU^Y^NG*^NS^NU^Y^NG ^NI^NU
* n676 - Modify lines back up the file, advancing tab c to max line length.
*        Return in edit mode when non-comment line found, positioned in the
*        non-comment line
N676 m-2^J^ND^N^<677>^NU^NPc^NS^NL^NOcX^[^N^<676>
* n675 - Modify each comment line to push its trailing *\ out to the maximum
*        length
N675 ^N[a^NS^NUm-^J^Z^B^Y^E^ND^N^<674>^J^N^<675>
* n674 - pad line out for n675
N674 ^NBc^NS^NU ^N^<674>
* n673 - Construct a /*--- */ line of the right lenght, put in n672
N673 i ta^J/*  */^B^Y^E^ND^N^<671>^H^L^NM^<672>^[^NU
* n671 - fill line with minuses
N671 ^NBc^NS^NU-^N^<671>
z
