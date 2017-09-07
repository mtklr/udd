/*
 * udd trs_cobjs.c file - special treasure objects
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include "defs.h"
#include <math.h>

int trs_chest()

{
  int tmp, trs;
  double adj;
  printf("You have discovered a chest...\r\n");
 trs_c_top:
  utl_dtrp();
  printf("Press <CR> to open it, <LF> to leave it: ");
  tmp = getchar();
  printf("\r\n");
  if (tmp < 0)
    tmp = '\n';
  if (tmp == '\n') {
    u.c[64] = DGN_PROMPT;
    return(MAYBE);
  }
  if (tmp != '\r') {
    printf("STUPID DOLT\007!\r\n");
    goto trs_c_top;
  }
  if (u.i[8] == 1 && rnd() > 0.5) {
    printf("CHEST EXPLODES\007!\r\n");
    if (cbt_ohitu(u.c[63] + 5) == YEP)
      return(YEP);
  }
  trs = 3000.0 * rnd() * u.c[63] + 500;
  printf("It hold %d in gold.\r\n", trs);
  u.c[12] += trs;
  adj = u.c[15] / (double) u.c[8];
  if (adj > 1.0)
    adj = 1.0;
  u.c[21] += trs * adj;
  u.c[64] = XXX_NORM;
  return(NOPE);
}

int trs_obj()

{
  int rl, tmp, trs;
  do {
    rl = roll(1,8);
    switch (rl) {
    case 1:        /* wepon */
      printf("You have found a magic %s!\r\n", wep[u.c[7]]);
    top1:
      printf("Press <CR> to pick it up, <LF> to leave it behind: ");
      tmp = getchar();
      printf("\r\n");
      if (tmp < 0)
	tmp = 10;
      if (tmp == '\n') 
	break;
      if (tmp != '\r') {
	printf("THINK STUPID\007!\r\n");
	goto top1;
      }
      if (rnd() > 0.833) {
	printf("It's a hostile %s!\r\n", wep[u.c[7]]);
	if (cbt_ohitu(u.c[22]) == YEP)
	  return(YEP);
	break;
      }
      if (u.c[22] > u.c[63]) {
	printf("You already have a %s +%d.\r\n", wep[u.c[7]], u.c[22]);
	break;
      }
      u.c[22]++;
      printf("You have found a %s%s +%d.\r\n", 
	     (u.c[22] > 0) ? "Magic " : "", wep[u.c[7]], u.c[22]);
      break;
    case 2:        /* arm */
      if (u.c[23] < u.c[63] + 2)
	tmp = u.c[23] + 1;
      else
	tmp = u.c[23];
      printf("You have found %s%s Armor +%d.\r\n", 
	     (tmp > 0) ? "Magic " : "", arm[u.c[7]], tmp);
      if (tmp == u.c[23])
	printf("Too bad you already have one.\r\n");
      else
	u.c[23]++;
      break;
    case 3:        /* shield */
      if (u.c[24] < u.c[63] + 2)
	tmp = u.c[24] + 1;
      else
	tmp = u.c[24];
      printf("You have found a %sShield +%d.\r\n",
	     (tmp > 0) ? "Magic " : "", tmp);
      if (u.c[7] == 2) {
	printf("Too bad you can't use it!\r\n");
	break;
      }
      if (u.c[24] == tmp) 
	printf("You already have one of those.\r\n");
      u.c[24] = tmp;
      break;
    case 4:        /* book */
      printf("You have found a book...\r\n");
    top4:
      printf("Press <CR> to read it, <LF> to ignore it: ");
      tmp = getchar();
      printf("\r\n");
      if (tmp < 0)
	tmp = 10;
      if (tmp == '\n')
	break;
      if (tmp != '\r') {
	printf("BRAINLESS\007!\r\n");
	goto top4;
      }
      if (rnd() > 0.5) 
	utl_adj_st();
      else
	if (utl_adj_ex() == YEP)
	  return(YEP);
      break;
    case 5:        /* torch */
      printf("You found a magic torch.\r\nIt starts burning.\r\n");
      sleep(2);
      if (u.c[37] < 0)
	u.c[37] = roll(3,10);
      else
	u.c[37] += roll(3,10);
      u.i[7] = 0;
      if (u.i[5] == 0)
	utl_pplot(NOPE);
      u.c[64] = XXX_NORM;
      return(NOPE);
      break;
    case 6:        /* ring */
      trs = rnd() * rnd() * rnd() * u.c[63] + 1;
      printf("You have found a Ring of Regeneration +%d.\r\n", trs);
      if (u.c[51] >= trs) {
	printf("You already have a better one.\r\n");
	break;
      }
    top6:
      printf("Press <CR> to pick it up, <LF> to leave it behind: ");
      tmp = getchar();
      printf("\r\n");
      if (tmp < 0)
	tmp = 10;
      if (tmp == '\n')
	break;
      if (tmp == '\r') {
	u.c[51] = trs;
	break;
      }
      printf("TRY AGAIN CHOWDERHEAD\007!\r\n");
      goto top6;
    case 7:        /* cloak */
      trs = rnd() * rnd() * u.c[63] + 2;
      printf("You have found an Elven Cloak +%d.\r\n", trs);
      if (u.c[52] >= trs) {
	printf("You already have a better one.\r\n");
	break;
      }
    top7:
      printf("Press <CR> to put it on, <LF> to leave it behind: ");
      tmp = getchar();
      printf("\r\n");
      if (tmp < 0)
	tmp = 10;
      if (tmp == '\n')
	break;
      if (tmp == '\r') {
	u.c[52] = trs;
	break;
      }
      printf("Ever try a hearing aid?\r\n");
      goto top7;
    case 8:        /* boots */
      trs = rnd() * rnd() * u.c[63] + 2;
      printf("You have found a pair of Elven Boots +%d.\r\n", trs);
      if (u.c[53] >= trs) {
	printf("You already have a better pair.\r\n");
	break;
      }
    top8:
      printf("Press <CR> to put them on, <LF> to leave them behind: ");
      tmp = getchar();
      printf("\r\n");
      if (tmp < 0)
	tmp = 10;
      if (tmp == '\n')
	break;
      if (tmp == '\r') {
	u.c[53] = trs;
	break;
      }
      printf("HEY, LISTEN STUPID\007!\r\n");
      goto top8;
    default:
      printf("trs_cobjs: internal error!\r\n");
      unix_exit(1);
    }
  } while (rnd() > 0.8 - 0.02 * (u.c[15] - 1));
  u.c[64] = XXX_NORM;
  return(NOPE);
}


