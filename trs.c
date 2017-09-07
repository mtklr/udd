/*
 * udd trs.c file - treasure program
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include "defs.h"
#include <math.h>

#define TRS_NORM  0
#define TRS_CHEST 1
#define TRS_OBJ   2
  
int trs_main()

{
  int l = u.c[63];
  int tmp, typ = TRS_NORM;
  int trs;
  double adj;
  if (u.c[61] == 1)
    printf("trs: l = %d\r\n", l);
  tmp = roll(1, 100);
  if (tmp > 80 && tmp <= 90)
    typ = TRS_CHEST;
  else if (tmp > 90)
    typ = TRS_OBJ;
  switch (typ) {
  case TRS_CHEST:
    if ((trs = trs_chest()) == YEP)
      return(YEP);
    if (trs == MAYBE)
      break;
    /* fall through */
  case TRS_OBJ:
    if (trs_obj() == YEP)
      return(YEP);
    break;
  case TRS_NORM:
    if (tmp <= 30) {
      printf("You see a pile of silver...\r\n");
      trs = 100 * rnd() * l + 10;
    } else 
      if (tmp <= 40) {
	printf("You see a pile of gold...\r\n");
	trs = 500 * rnd() * l + 50;
      } else
	if (tmp <= 60) {
	  printf("You see a pile of platinum...\r\n");
	  trs = 1000 * rnd() * l + 100;
	} else
	  if (tmp <= 72) {
	    printf("You see some gems...\r\n");
	    trs = 500.0 * sqrt((double)rnd()) * l + 150.0;
	  } else {
	    printf("You see a jewel...\r\n");
	    trs = 6000.0 * rnd() * rnd() * rnd() * l + 500;
	  }
  trs_top:
    utl_dtrp();
    printf("Press <CR> to pick it up, <LF> to leave it behind: ");
    tmp = getchar();
    printf("\r\n");
    if (tmp < 0)
      tmp = '\n';
    if (tmp == '\n') {
      u.c[64] = XXX_NORM;
      break;
    }
    if (tmp != '\r') {
      printf("DUMMY!\007\r\n");
      goto trs_top;
    }
    if (u.i[8] == 1)
      if (cbt_ohitu((int) (u.c[15] / 1.2)) == YEP)
	return(YEP);
    printf("The treasure is worth %d gold.\r\n", trs);
    u.c[12] += trs;
    u.c[64] = XXX_NORM;
    adj = u.c[15] / (double) u.c[8];
    if (adj > 1.0)
      adj = 1.0;
    u.c[21] += trs * adj;
    break;
  default:
    printf("trs: internal error\r\n");
    unix_exit(1);
  }
  u.i[6] = u.i[7] = u.i[8] = 0;
  u.l[u.c[16]][u.c[17]] = 16*u.i[5] + 4*u.i[2] + u.i[1];
  if (u.i[5] == 0) {
    u.c[64] = DGN_PROMPT;
    return(NOPE);
  }
  utl_pplot(NOPE);
  return(spc_main());
}
