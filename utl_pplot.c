/*
 * udd utl_pplot.c file - position plot
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include <stdio.h>
#include "defs.h"

char obuf[BUFSIZ], *obp = obuf, tbuf[80], tbuf2[80], tbuf3[80];
int ccpos;         /* current cursor position */
int cobps;         /* current end of obuf position */

#define fnd3(c, z) vaz[fni1(u.p[c][z], 4) + 1]
#define fnd2(c, z) maz[fni1(u.p[c][z], 3)]
#define fnd1(c, z) ((fni1(u.p[c][z], 3) == 0) ? " " : "I")

/* generate opt. output */
int optout(s)

char *s;

{
  char *sp;
  int firstc = 1, lcv;
  sp = s + strlen(s) - 1;
  while(*sp == ' ' && sp > s)
    *sp-- = '\0';
  sp = s;
#ifdef OUT_DEBUG
  printf("%s\r\n", s);
#endif
  obuf[cobps++] = '\n';
  obuf[cobps] = '\0';
  while (*sp == ' ') {
    sp++;
    firstc++;
  }
  if (*sp == '\0') {
    obuf[cobps] = '\0';
    return(ccpos);
  }
  if ((ccpos / 2) > firstc) {          /* too far to backspace */
    obuf[cobps++] = '\r';
    strcpy(obp + cobps, s);
    ccpos = strlen(s) + 1;
    cobps += ccpos - 1;
    return(ccpos);
  }
  if (ccpos > firstc) {                /* backspace */
    for (lcv = 0; lcv < ccpos - firstc; lcv++)
      obuf[cobps++] = '\b';
    ccpos = firstc;
  }
  while (ccpos < firstc) {             /* space over if needed */
    obuf[cobps++] = ' ';
    ccpos++;
  }
  obuf[cobps] = '\0';
  strcat(obp + cobps, sp);
  lcv = strlen(sp);
  ccpos += lcv;
  cobps += lcv;
  return(ccpos);
}


int fni(r)                       /* analize room into u.i */

int r;

{
  u.i[1] = u.i[3] = (r & 3);           /* sec/nonsec top */
  r = r >> 2;
  u.i[2] = u.i[4] = (r & 3);           /* sec/nonsec side */
  r = r >> 2;
  if (u.c[37] < 1 && u.i[3] == 3 && rnd() < 0.85)
    u.i[3] = 1;
  if (u.c[37] < 1 && u.i[4] == 3 && rnd() < 0.85)
    u.i[4] = 1;
  u.i[5] = (r & 15);                   /* special */
  r = r >> 4;
  u.i[6] = (r & 1);
  r = r >> 1;
  u.i[7] = (r & 1);
  u.i[8] = r >> 1;
  return(u.i[5]);
}

int fni1(r, a)                   /* analize into u.i1 */

int r, a;

{
  u.i1[1] = u.i1[3] = (r & 3);
  r = r >> 2;
  u.i1[2] = u.i1[4] = (r & 3);
  r = r >> 2;
  if (u.c[37] < 1 && u.i1[3] == 3 && rnd() < 0.85)
    u.i1[3] = 1;
  if (u.c[37] < 1 && u.i1[4] == 3 && rnd() < 0.85)
    u.i1[4] = 1;
  u.i1[5] = (r & 15);
  r = r >> 4;
  u.i1[6] = (r & 1);
  r = r >> 1;
  u.i1[7] = (r & 1);
  u.i1[8] = r >> 1;
  if (a < 9)
    return(u.i1[a]);
  if ((u.i1[5] | u.i1[6] | u.i1[7]) == 0)
    return(0);
  return(1);
}


char *fnp(a, b, p)                     /* put B in A at P */

char *a, *b;
int p;

{
  int la, lb, lcv;
  la = strlen(a);
  lb = strlen(b);
  if (p > la)                          /* off the edge ? */
    strcat(a, b);
  else if ((p - 1) + lb > la) {        /* will go off end? */
    strcpy(a + (p - 1), b); 
    return(a);
  } else {                             /* must be all the way in a */
    for (lcv = 0 ; lcv < lb ; lcv++)
      a[(p - 1) + lcv] = b[lcv];
  }
  return(a);
}
    
char *fnp1(a, b, p)                    /* cond put B in A at P */

char *a, *b;
int p;

{
  int la, lb, lcv, null;
  char *ap;
  la = strlen(a);
  lb = strlen(b);
  if (p > la)
    strcat(a, b);
  else {
    null = (lb + (p - 1) > la) ? 1 : 0;
    ap = a + (p - 1);
    for (lcv = 0 ; lcv < lb ; lcv++)
      if (p + lcv > la)
	*ap++ = *b++;
      else {
	if (*ap != ' ') {
	  ap++;
	  b++;
	} else 
	  *ap++ = *b++;
      }
    if (null != 0)
      *ap = '\0';
  }
  return(a);
}


void utl_pplot(flag)

int flag;

{
  int lcv, lcv3, x, y;
  if (flag == YEP) {
    for (lcv = 37; lcv <= 47 ; lcv++)
      if (u.c[lcv] > 0)
	u.c[lcv]--;
      if (u.c[51] < 0)
	u.c[51] = 0;
      u.c[11] += u.c[51];
  }
  printf("\r\n\n");
  x = u.c[16];
  y = u.c[17];
  for (lcv = 0 ; lcv < 11 ; lcv++) {
    if (u.c[SPLBASE+lcv] > 0)
      printf("%.4s:", lcv*4 + "LGHTPROTSHLDPRAYDTRPSLNCLEVTSTRGFEARINVSTMST");
  }
  printf("\r\n\n");
  for (lcv = -1 ; lcv <= 2 ; lcv++)
    for (lcv3 = -1 ; lcv3 <= 2 ; lcv3++) 
      if (x+lcv > 21 || y+lcv3 > 21)
	u.p[lcv+2][lcv3+2] = 0;
      else
	u.p[lcv+2][lcv3+2] = u.l[x+lcv][y+lcv3];
  ccpos = 1;
  cobps = 0;
  if (x != 1 && fni1(u.p[2][2], 4) == 0) {       /* print top if one */
    strcpy(tbuf,"      ");
    strcat(tbuf,fnd3(1,2));
    optout(tbuf);
    strcpy(tbuf,"      ");
    strcpy(tbuf2, tbuf);
    strcat(tbuf, fnd1(1,2));
    strcat(tbuf, "     ");
    strcat(tbuf, fnd1(1,3));
    strcat(tbuf2, fnd2(1,2));
    strcat(tbuf2, "     ");
    strcat(tbuf2, fnd2(1,3));
    if (u.c[37] > 0) 
      if (fni1(u.p[1][2], 9) != 0)     /* light spell */
	tbuf2[9] = '*';
    optout(tbuf);
    optout(tbuf2);
    optout(tbuf);
  }
  if (y != 1 && fni1(u.p[2][2], 3) == 0) {
    strcpy(tbuf, fnd3(2,1));
  }
  else {
    strcpy(tbuf, "       ");
  }
  fnp1(tbuf, fnd3(2,2), 7);
  if (fni1(u.p[2][3], 3) == 0)
    fnp1(tbuf, fnd3(2,3), 13);
  if (u.i[4] == 0) {
    fnp1(tbuf, fnd1(1,2), 7);
    fnp1(tbuf, fnd1(1,3), 13);
  }
  optout(tbuf);
  strcpy(tbuf, "                   ");
  strcpy(tbuf2,"                   ");
  if (y != 1 && fni1(u.p[2][2], 3) == 0) {
    fnp1(tbuf, fnd1(2,1), 1);
    fnp1(tbuf2,fnd2(2,1), 1);
    if (u.c[37] > 0 && fni1(u.p[2][1], 9) != 0)
      fnp(tbuf2, "*", 4);
  }
  fnp1(tbuf, fnd1(2,2), 7);
  fnp1(tbuf2,fnd2(2,2), 7);
  fnp1(tbuf, fnd1(2,3),13);
  fnp1(tbuf2,fnd2(2,3),13);
  if (tbuf[12] == ' ') {
    fnp1(tbuf, fnd1(2,4), 19);
    fnp1(tbuf2,fnd2(2,4), 19);
    if (u.c[37] > 0 && fni1(u.p[2][3], 9) != 0)
      fnp(tbuf2, "*", 16);
  }
  fnp1(tbuf2, "X", 10);
  strcpy(tbuf3, tbuf);
  if (u.i[7] == 1)
    fnp(tbuf, "$", 11);
  if (u.i[6] == 1)
    fnp(tbuf, "#", 11);
  if ((u.i[6] | u.i[7]) == 0 && u.i[5] != 0) 
    fnp(tbuf,spc[u.i[5] - 1], 9);
  optout(tbuf);
  optout(tbuf2);
  optout(tbuf3);
  lcv3 = strlen(tbuf3);
  if (7 > lcv3 || tbuf3[6]  == ' ')
    strcpy(tbuf,fnd3(3,1));
  else
    strcpy(tbuf,"       ");
  fnp1(tbuf, fnd3(3,2), 7);
  if (13 > lcv3 || tbuf3[12] == ' ')
    fnp1(tbuf, fnd3(3,3), 13);
  if (tbuf[9] == ' ') {
    fnp1(tbuf, fnd1(3,2), 7);
    fnp1(tbuf, fnd1(3,3),13);
  }
  optout(tbuf);
  lcv = strlen(tbuf);
  if (10 > lcv || tbuf[9] == ' ') {
    strcpy(tbuf,"      ");
    strcat(tbuf,fnd1(3,2));
    strcat(tbuf,"     ");
    strcat(tbuf,fnd1(3,3));
    strcpy(tbuf2,"      ");
    strcat(tbuf2,fnd2(3,2));
    strcat(tbuf2,"     ");
    strcat(tbuf2,fnd2(3,3));
    if (u.c[37] > 0 && fni1(u.p[3][2], 9) != 0)
      fnp(tbuf2, "*", 10);
    strcpy(tbuf3, "      ");
    strcat(tbuf3,fnd3(4,2));
    optout(tbuf);
    optout(tbuf2);
    optout(tbuf);
    optout(tbuf3);
  }
  printf("%s\n\r", obuf);
}

