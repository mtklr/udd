/*
 * udd swb.c file - main switchboard program
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 * Current version/patchlevel: */
/*#define VERS "UDD V5.1C-06  22-Oct-89" */
#define VERS "UDD V5.1C-07  01-Feb-2001" 

#define MAIN

#include "defs.h"
#include "data.h"
#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include <pwd.h>

main(argc, argv)

int argc;
char **argv;

{
  int alevel = swb_ppnok();
  int done = NOPE;
  int tmp, tmp2, sts[7];
  int sav, sav2;
  int wiz = swb_wiz();
  char *cmds = "RCPKIEOLSFMD", *cp;
  char nbuf[NAMELEN], *np;
  struct chr *cptr;
  struct passwd *pptr;
  ddd = *argv;
  srandom(time(0));
  dfd = n_dlvl = -1;
  if (argc != 1) {
    printf("%s: This command takes no options.\r\n");
    exit(1);
  }
  if (alevel == NOPE) {
    printf("?You are restricted from this program.\r\n");
    exit(1);
  }
  u.c[64] = 0;
  unix_init();
  unix_tty_dgn();
  printf("\n\n\n\n\rWelcome to %s!  This is  %s.\n\r", ddd, VERS);
  printf("\r\n\n");
  swb_note("Current note:", FIL_NOT, 0);
  swb_note("ORB finders:", FIL_ORB, 1);
  printf("\r\n");
  while (done == NOPE) {
    printf("Hit \"L\" for a list of options.\r\n");
  prompt_top:
    printf("%s> ", ddd);
    tmp2 = getchar();
    if (tmp2 < 0)
      tmp2 = 'E';
    if (islower(tmp2))
      tmp2 = toupper(tmp2);
    if (tmp2 == ' ' || tmp2 == '\r') {
      printf("\r\n");
      goto prompt_top;
    }
    cp = index(cmds, tmp2);
    if (cp == NULL) {
      printf("\r\n%%Illegal option.\r\n");
      goto prompt_top;
    }
    switch(tmp2) {
    case 'R':    /* Run */
      printf("Run\r\n");
      if (alevel == MAYBE) {
	printf("%%Restricted option.\r\n");
	break;
      }
      printf("What is your name noble Sir ? ");
      unix_tty_cook();
      if (!fgets(u.n[0], NAMELEN, stdin))
        strcpy(u.n[0], "\n");
      unix_tty_dgn();
      fflush(stdin);
      u.n[0][strlen(u.n[0])-1] = 0;   /* nuke the \n */
      if (chr_load(u.n[0], NOLOCK) == NOPE) {
	printf("%%No such character.\r\n");
	break;
      }
      printf("%s's SECRET NAME is : ", u.n[0]);
      unix_tty_pswd();
      if (!fgets(nbuf, NAMELEN, stdin))
        strcpy(nbuf, "\n");
      unix_tty_dgn();
      printf("\r\n");
      nbuf[strlen(nbuf)-1] = 0;
      if (strcmp(nbuf, u.n[1]) != 0) {
	printf("%%\007Body snatcher!!!!\007\r\n");
	printf("%s> Exit\r\n", ddd);
	unix_exit(1);
      }
      if (chr_load(u.n[0], LOCK) != YEP) {
	printf("%s: Sorry, that character is in use (locked).\r\n", ddd);
	break;
      }
      if (strcmp(nbuf, u.n[1]) != 0) { /* not very possible, but... */
	printf("%s: Sorry, that character is in use (locked).\r\n", ddd);
	chr_save(YEP);                 /* will unlock char */
	break;
      }
      u.c[64] = DGN_NEWADV;
      autosave = 1;
      if (u.c[15] == 0)
	u.c[18] = dlvl_choose();
       else
        dlvl_get_start(u.c[18]);
      u.c[60] = time(0);
      printf("You are now descending into the dungeon:\r\n");
      printf("Please wait while we force open the main door..\r\n");
      printf("\n");
      dgn_main();
      autosave = 0;
      if (u.c[64] != SWB_CREATE) 
        break;
    printf("\r\n%s> ", ddd);
    u.c[64] = XXX_NORM;
    case 'C':    /* Create */
    create:
      printf("Create\r\n");
      if (alevel == MAYBE) {
	printf("%%Restricted option.\r\n");
	break;
      }
      printf("\r\n\n\n\n");
    create_top1:
      printf("Press <CR> to run this character, <LF> to try again\r\n");
      printf("or \"Q\" to quit.\r\n\n");
    create_top2:
      for (tmp = 1; tmp <= 6 ; tmp++) {
	sts[tmp] = roll(1,8) + roll(1,6) + 4;
	printf("%.3s %02d ", st + (tmp - 1) * 3, sts[tmp]);
      }
      sav = getchar();
      if (sav < 0 || sav == 'q' || sav == 'Q') {
	printf("\r\n");
	break;
      }
      if (sav == '\n') {
	printf("\r");
	goto create_top2;
      }
      if (sav != '\r') {
	printf("Read the directions!!!\007\r\n");
        sleep(3);
	fflush(stdin);
        printf("\n\n\n\n");
	goto create_top1;
      }
    create_top3:
      printf("\r\nWhat is your name noble Sir ? ");
      unix_tty_cook();
      if (!fgets(nbuf, NAMELEN, stdin))
        strcpy(nbuf, "\n");
      unix_tty_dgn();
      fflush(stdin);
      nbuf[strlen(nbuf) - 1] = 0;
      np = nbuf;
      while (*np != 0) {
	if (*np < ' ' || *np > '~') {
	  printf("Try using normal characters!\007\r\n");
	  goto create_top3;
	}
	np++;
      }
      if (strlen(nbuf) == 0 || chr_load(nbuf, NOLOCK) == YEP) {
	if (nbuf[0] == 0)  {
	  printf("No-one but a monster has no name.\r\n");
          goto create_top3;
	} else {
	  printf("\"How dare you steal my name!\", quoth the might %s.",
		 nbuf);
	  printf("\r\nPlease choose another name.\r\n");
	  goto create_top3;
	}
      }
      strcpy(u.n[0], nbuf);
      u.c[0] = 1;
      for (tmp = 1 ; tmp < 7 ; tmp++)
	u.c[tmp] = sts[tmp];
      printf("What is %s's SECRET NAME : ", nbuf);
      unix_tty_pswd();
      if (!fgets(u.n[1], NAMELEN, stdin))
        strcpy(u.n[1], "\n");
      unix_tty_dgn();
      fflush(stdin);
      printf("\r\n");
      u.n[1][strlen(u.n[1]) - 1] = 0;
      u.c[57] = 0;
      u.c[48] = getgid();
      u.c[49] = getuid();
      if (chr_new() == NOPE) {     /* try and claim a slot */
	printf("\"How dare you steal my name!\", quoth the might %s.",
	       u.n[0]);
	printf("\r\nPlease choose another name.\r\n");
	goto create_top3;
      }
    create_top4:
      printf("What character class--(F)ighter, (C)leric, or (M)agic user? ");
      sav = getchar();
      if (islower(sav))
	sav = toupper(sav);
      if (sav == 'F')
	u.c[7] = 0;
      else if (sav == 'C')
	u.c[7] = 1;
      else if (sav == 'M')
	u.c[7] = 2;
      else {
	printf("\r\nWake up, Jose!!!\r\n");
	goto create_top4;
      }
      printf("%s\r\n", class2[u.c[7]]);
      u.c[11] = u.c[10] = 6 + 2 * (2 - u.c[7]);
      u.c[8] = 1;
      for (tmp = 12 ; tmp < 65 ; tmp++)
	u.c[tmp] = 0;
      u.c[57] = 1; /* don't let us unlock by mistake */
      u.c[59] = u.c[60] = time(0);
      if (u.c[4] > 14)
	u.c[10] = u.c[11] = u.c[10] + u.c[4] - 14;
      u.c[19] = 15;
      u.c[20] = 2;  /* ??? */
      u.c[18] = dlvl_choose();
      if (u.c[7] == 2) {
	u.c[25] = u.c[31] = 3;
	u.c[24] = -1;
      }
      if (u.c[7] == 1)
	u.c[25] = u.c[31] = 2;
      u.c[48] = getgid();
      u.c[49] = getuid();
      u.c[9] = 0;
      if (chr_save(NOPE) != YEP) {     /* should just update our slot */
	printf("%s: Internal error: Can't update char file!\r\n", ddd);
	printf("%s: This should not happen!\r\n", ddd);
	break;
      }
      printf("You are now descending into the dungeon:\r\n");
      printf("Please wait while we force open the main door...\r\n");
      u.c[64] = DGN_NEWADV;
      autosave = 1;
      dgn_main();
      autosave = 0;
      if (u.c[64] == SWB_CREATE) {
        printf("\r\n%s> ", ddd);
        u.c[64] = XXX_NORM;
        goto create;
      }
      break;
    case 'P':
    case 'M':
    case 'D':
      if (tmp2 == 'P')
	printf("List All Players\r\n");
      else if (tmp2 == 'M')
	printf("List My Players\r\n");
      else {
	printf("List Players in a Dungeon\r\n");
	printf("Dungeon # : ");
	tmp = getchar();
	if (tmp < '0' || tmp > '9') {
	  printf("A number please!\007\r\n");
	  break;
	}
	printf("%c\r\n", tmp);
	tmp = tmp - '0';
      }
      chr_rset();
      printf("Player     STR INT WIS CON DEX CHA LVL DGN RING ");
      printf("EXP         User          Type\r\n");
      while((cptr = chr_scan()) != NULL) {
	if (cptr->c[0] == 0)   /* dead? */
	  continue;
	if (cptr->c[62] != 0 && wiz == 0)
	  continue;
	if (tmp2 == 'M' && ntohl(cptr->c[49]) != getuid()) /* only me? */
	  continue;
	if (tmp2 == 'D' && ntohl(cptr->c[18]) != tmp)      /* dungeon? */
	  continue;
	printf("%-10.10s ", cptr->nam[0]);
	for (tmp = 1 ; tmp < 7; tmp++)
	  printf("%02d  ", ntohl(cptr->c[tmp]));
        printf("%02d   %1d  ", ntohl(cptr->c[8]), ntohl(cptr->c[18]));
        if (cptr->c[51] == 0)
          printf("none ");
        else
          printf(" %02d  ", ntohl(cptr->c[51]));
	printf("%-11d ", ntohl(cptr->c[9]));
	if ((pptr = getpwuid(ntohl(cptr->c[49]))) == NULL)
	  printf("u%-7d ", ntohl(cptr->c[49]));
	else
	  printf("%-8s ", pptr->pw_name);
	printf("%c%c%c%c ", (cptr->c[15] == 0) ? ' ' : '*', 
	       (cptr->c[61] == 0) ? ' ' : '+',
	       (cptr->c[62] == 0) ? ' ' : '@',
	       (cptr->c[57] == 0) ? ' ' : 'L');
	tmp = ntohl(cptr->c[7]);
	if (tmp == 1) 
	  printf("CLRC\r\n");
	else if (tmp == 2)
	  printf("MAGE\r\n");
	else if (cptr->c[25] + cptr->c[26] + cptr->c[27] + cptr->c[28] != 0)
	    printf("F/MU\r\n");
	else if (cptr->c[31] + cptr->c[32] + cptr->c[33] + cptr->c[34]
		 != 0)
	  printf("HERO\r\n");
	else printf("FGTR\r\n");
      }
      printf("\r\n");
      break;
    case 'K':
      printf("Kill\r\n");
      if (alevel == MAYBE) {
	printf("%%Restricted option.\r\n");
	break;
      }
      printf("What is your name noble Sir? ");
      unix_tty_cook();
      if (!fgets(nbuf, NAMELEN, stdin))
        strcpy(nbuf, "\n");
      unix_tty_dgn();
      nbuf[strlen(nbuf) - 1] = 0;
      if (chr_load(nbuf, NOLOCK) == NOPE) {
	printf("%%No such character.\r\n");
	break;
      }
      if (u.c[57] != 0) {
	printf("That character is LOCKED (in use)!\r\n");
	printf("Sorry, can't kill a locked character...\r\n");
	break;
      }
      if (wiz == 0) {
	unix_tty_pswd();
	printf("%s's SECRET NAME is? ", nbuf);
        if (!fgets(nbuf, NAMELEN, stdin))
          strcpy(nbuf, "\n");
	unix_tty_dgn();
	printf("\r\n");
	nbuf[strlen(nbuf) - 1] = 0;
	if (strncmp(u.n[1], nbuf, NAMELEN) != 0) {
	  printf("%%\007Grave robber\007!\007!\007!\r\n");
	  printf("%s> Exit\r\n", ddd);
	  unix_exit(1);
	}
      } else
        strcpy(nbuf, u.n[1]);
      printf("Are you sure you wish to die? ");
      tmp = getchar();
      if (tmp == 'y' || tmp == 'Y') {
	printf("Yes\r\n");
	if (chr_load(u.n[0], LOCK) != YEP) {
	  printf("Sorry, that character was just locked...\r\n");
	  printf("Try again later.\r\n");
	  break;
	}
	printf("Goodbye life.....ARRRGGG.G..G.. .   .    .\r\n");
	if (strcmp(nbuf, u.n[1]) == 0)      /* diff char most likely */
	  chr_lck_nuke(NUKE);
	else
	  chr_save(YEP);               /* someone else char with same
					  name, created between the above
					  two calls to chr_load... not 
					  very likely */
      } else
	printf("No\r\n");
      printf("\r\n");
      break;
    case 'I':
      printf("Instructions\r\n");
      swb_note("Current instruction file:", FIL_INS, wiz);
      break;
    case 'E':
      printf("Exit\r\n");
      unix_exit(0);
      break;
    case 'O':
      printf("Operator\r\n");
      if (wiz == 0) {
	printf("%% 'Operator' is a privileged command.\r\n");
	break;
      }
      opr_main();
      break;
    case 'L':
      printf("List Options\r\n");
      printf("Options are:\r\n\n");
      printf("R\tRun a character\r\n");
      printf("C\tCreate a character\r\n");
      printf("P\tList all current players\r\n");
      printf("M\tList my own characters\r\n");
      printf("D\tList all players in one dungeon\r\n");
      printf("K\tKill a character\r\n");
      printf("S\tList all player's status (dates, et al.)\r\n");
      printf("F\tFind experience needed for a level\r\n");
      printf("I\tInstructions\r\n");
      printf("E\tExit program\r\n");
      printf("O\tGo to operator program (privileged option)\r\n");
      printf("L\tList options (this list)\r\n");
      printf("\r\n");
      break;
    case 'F':
      printf("Find experience for level\r\n");
      printf("For what character class Sire?\r\n");
    find_top:
      printf(" (M)agician, (C)leric, or (F)ighter ? ");
      tmp = getchar();
      if (tmp < 0)
	break;
      if (islower(tmp))
	tmp = toupper(tmp);
      if (tmp == 'M') {
	printf("Magician\r\n");
	u.c[7] = 2;
      } else if (tmp == 'C') {
	printf("Cleric\r\n");
	u.c[7] = 1;
      } else if (tmp == 'F') {
	printf("Fighter\r\n");
	u.c[7] = 0;
      } else {
	printf("\r\nEgad\007!  Try again.\r\n");
	goto find_top;
      }
      printf("For level ? ");
      unix_tty_cook();
      if (!fgets(nbuf, NAMELEN, stdin))
        strcpy(nbuf, "\n");
      nbuf[strlen(nbuf) - 1] = 0;
      tmp = atoi(nbuf);
      unix_tty_dgn();
      fflush(stdin);
      if (tmp < 1 || tmp > 1000) 
	printf("That's a little out of my range...\r\n");
      else
	printf("You need %d experience for level %d.\r\n", utl_exp(tmp), tmp);
      break;
    case 'S':
      printf("Status\r\n");
      chr_rset();
      printf("Player       Created   Last Run   Total_Gold  Experience    ");
      printf("User          Type\r\n");
      while ((cptr = chr_scan()) != NULL) {
	if (cptr->c[0] == 0)
	  continue;
	if (cptr->c[62] != 0 && wiz == 0) 
	  continue;
	printf("%-10.10s  %s  ", cptr->nam[0], unix_date(ntohl(cptr->c[59])));
	printf("%s  ", unix_date(ntohl(cptr->c[60])));
	printf("%-10d  ", ntohl(cptr->c[13]));
	printf("%-10d    ", ntohl(cptr->c[9]));
	if ((pptr = getpwuid(ntohl(cptr->c[49]))) == NULL)
	  printf("u%-7d  ", ntohl(cptr->c[49]));
	else
	  printf("%-8s ", pptr->pw_name);
	printf("%c%c%c%c ", (cptr->c[15] == 0) ? ' ' : '*', 
	       (cptr->c[61] == 0) ? ' ' : '+',
	       (cptr->c[62] == 0) ? ' ' : '@',
	       (cptr->c[57] == 0) ? ' ' : 'L');
	tmp = ntohl(cptr->c[7]);
	if (tmp == 1) 
	  printf("CLRC\r\n");
	else if (tmp == 2)
	  printf("MAGE\r\n");
	else if (cptr->c[25] + cptr->c[26] + cptr->c[27] + cptr->c[28] != 0)
	    printf("F/MU\r\n");
	else if (cptr->c[31] + cptr->c[32] + cptr->c[33] + cptr->c[34]
		 != 0)
	  printf("HERO\r\n");
	else printf("FGTR\r\n");
      }
      printf("\r\n");
      break;
    default:
      printf("\r\n%d: This can't happen!\r\n", ddd);
    }
  }
}

