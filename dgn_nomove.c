/*
 * udd dgn_nomove.c file - dungeon non-movement commands
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include "defs.h"

int dgn_nomove(cmd)

int cmd;

{
  int in;
  switch (cmd) {
    case 1:
      printf("Cast\r\n");
      u.c[64] = CBT_CAST;
      return(cbt_main());
    case 2:
      printf("Return ORB\r\n");
      if (u.c[50] != 1)
        printf("You don't have an orb to drop, stupid!\r\n");
      else {
        printf("Are you sure? ");
        in = getchar();
        if (in == 'y' || in == 'Y') {
          printf("Yes\r\nThe ORB disappears as you drop it.\r\n");
          u.c[50] = 0;
        } else
        printf("No\r\nI didn't think so!\r\n");
      }
      u.c[64] = DGN_PROMPT;
      return(NOPE);
    case 3:
      printf("Stats\r\n");
      utl_stat();
      utl_eqp();
      return(NOPE);
    case 4:
      printf("Status\r\n");
      utl_status();
      return(NOPE);
    case 5:
      utl_pplot(NOPE);
      return(NOPE);
    case 6: 
      printf("Let me die!\r\n");
      printf("Are you sure? ");
      in = getchar();
      if (in == 'y' || in == 'Y') {
        printf("Yes\r\nGood bye cruel world!!\r\n");
        printf("ARGGGG.GG..GG...G.... .  .   .    .\r\n");
        return(utl_death());
      } else
      printf("No\r\nIt's not all that bad...I guess.\r\n");
      u.c[64] = DGN_PROMPT;
      return(NOPE);
    case 7:
      printf("Quit\r\n");
      printf("Are you sure? ");
      in = getchar();
      if (in == 'y' || in == 'Y') {
        printf("Yes\r\nSaving your position sire...\r\n");
        u.c[64] = XXX_NORM;
        return(chr_save(YEP)); 
      } else
      printf("No\r\n");
      u.c[64] = DGN_PROMPT;
      return(NOPE);
    case 8:
      printf("Utter\r\n");
      printf("\"Oh, Great Ghod!  Get me the F___ out of here!\"\r\n");
      u.c[12] *= 1 - (rnd() * rnd());  /* steal gold */
      if (rnd() > 0.25) {              /* do we hate it? */
        printf("\"F___ off little twerp!\"\r\n\n");  /* tell it */
        if (--u.c[11] < 1)             /* give a hit. Did we kill it? */
          return(utl_death());         /* ***YES*** */
        u.c[64] = DGN_PROMPT;          /* too bad */
        return(NOPE);
      }
      u.c[4]--;                        /* CON -- */
      u.c[11]*= 2/3.0;                 /* get hits */
      if (u.c[4] < 1 || u.c[11] < 1)
        return(utl_death());
      in = roll(1,10);
      if (in < 4)
        u.c[22+in] = 0; 
      else
        if (in < 7)
          u.c[50 + in - 3] = 0;
        else
          if (in == 8)
            u.c[51] = 0;
      for (in = 15 ; in <= 17 ; in++) {
        u.c[in] += roll(1,5) - 3;
        if (u.c[in] < 1)
          u.c[in] = 1;
        else
          if (u.c[in] > 20)
            u.c[in] = 20;
      }
      utl_inilvl();
      printf("\"Well, I can always try, but it will cost you.\"\r\n");
      u.c[64] = DGN_NEWLOC;
      return(NOPE);
    case 9:
      printf("Help\r\n");
      printf("Commands are:\r\n\n");
      printf("5, S\tstay and wait a turn\n\r");
      printf("8, W\tMove north\n\r");
      printf("6, D\tMove east\n\r");
      printf("4, A\tMove south\n\r");
      printf("2, X\tMove west\n\r");
      printf("C\tCast spell\n\r");
      printf("R\tDrop the ORB if you've got it\n\r");
      printf("<CR>\tList stats and equipment\n\r");
      printf("<LF>\tList experience, gold, hits, and spells\n\r");
      printf("<ESC>\tReplot your position\n\r");
      printf("K\tFall on your sword\n\r");
      printf("Q\tTemporarily quit expedition\n\r");
      printf("\t (it will be restarted where you left)\n\r");
      printf("U\tUtter a pray for escape to the Nameless God\n\r");
      printf("H\tHelp (type this list)\n\r");
      u.c[64] = DGN_PROMPT;
      return(NOPE);
    default:
      printf("panic: nomove invalid command: %d\n", cmd);
      unix_exit(1);
  }
  return(NOPE);
}
