/*
 * udd opr.c file - Operator program
 *
 * 03-Sep-89, Chuck Cranor (author, inspired by jrich's vax version)
 *
 * V5.1C (see VERSION file for history)
 *
 * Operator program version:  */
#define VERS "OPR  V2.1C-03  26-Oct-89"

#include "defs.h"
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/file.h>

static int loaded = NOPE;

opr_main()

{
  int tmp, tmp2, tmp3;
  int newc;
  int done = NOPE;
  FILE *fp, *fopen();
  char buf[BUFSIZ], nbuf[NAMELEN];
  int dgn, lvl, lcv;
  struct chr *cptr;
  printf("%s\r\n", VERS);
  while (done == NOPE) {
    printf("Type 'L' for a list of options, 'E' to return.\r\n");
  otop:
    printf("opr> ");
    tmp2 = getchar();
    if (tmp2 < 0)
      tmp2 = 'E';
    if (islower(tmp2))
      tmp2 = toupper(tmp2);
    if (tmp2 == '\r' || tmp2 == ' ') {
      printf("\r\n");
      goto otop;
    }
    switch(tmp2) {
    case 'E':
      printf("Exit\r\n");
      done = YEP;
      break;
    case 'L':
      printf("List options\r\n");
      printf("Operator commands supported:\r\n\n");
      printf("E\tExit to main program\r\n");
      printf("L\tlist options\r\n");
      printf("I\tINITIALIZE files (will create data files as needed)\r\n");
      printf("S\tset dungeon start location, or close dungeon\r\n");
      printf("D\tset number of valid dungeons [1..9]\r\n");
      printf("R\tRe-install a level in the main database\r\n");
      printf("X\teXtract level from main database\r\n");
      printf("M\tprint groovy map of a level (needs overstrike)\r\n");
      printf("C\tclean char file (nuke old chars)\r\n");
      printf("<\tread in a character, lock it\r\n");
      printf(">\twrite current character to database (unlocks it)\r\n");
      printf("A\tset/print current char's attribute\r\n");
      printf("N\tset/print current char's names\r\n");
      printf("\r\n");
      break;
    case 'R':
      printf("Re-install level\r\n");
      printf("opr: WARNING: old version of level will be overwritten!\r\n");
      if (n_dlvl < 0)
	dlvl_init();
      unix_tty_cook();
      printf("Level file (<CR> to abort) : ");
      if (!fgets(buf, BUFSIZ, stdin))
        strcpy(buf, "\n");
      buf[strlen(buf) - 1] = 0;
      if (buf[0] == 0) {
	printf("Aborted.\r\n");
        unix_tty_dgn();
	break;
      }
      printf("Enter dungeon number and level number : ");
      if (!fgets(nbuf, NAMELEN, stdin) ||
           sscanf(nbuf, "%d%d", &dgn, &lvl) != 2)
        dgn = -1;
      unix_tty_dgn();
      fflush(stdin);
      if (dgn < 1 || dgn > n_dlvl) {
	printf("opr: no such dungeon!\r\n");
	break;
      }
      if (lvl < 1 || lvl > 20) {
	printf("opr: level out of range!\r\n");
	break;
      }
      if ((fp = fopen(buf, "r")) == NULL) {
	fprintf(stderr, "opr: can't open %s!\r\n", buf);
	perror("fopen");
	printf("\r");
	break;
      }
      for (tmp = 0; tmp < 400 ; tmp++)
	dd.dmap[tmp] = 0;
      printf("Loading level...\r\n");
      for (tmp = 1 ; tmp <= 40 ; tmp++) {
        if (!fgets(buf, BUFSIZ, stdin))
          strcpy(buf, "\n");
	buf[strlen(buf) - 1] = 0;
	if (strlen(buf) < 40)
	  strcat(buf,"                                        ");
	if (tmp % 2 == 1) 
	  for (lcv = 1; lcv <= 39 ; lcv += 2) {
	    switch(buf[lcv]) {
	    case ' ':
	      break;
	    case '-':
	      dd.dmap[(tmp / 2) * 20 + lcv / 2] |= 4;
	      break;
	    case '=':
	      dd.dmap[(tmp / 2) * 20 + lcv / 2] |= 8;
	      break;
	    case '~':
	      dd.dmap[(tmp / 2) * 20 + lcv / 2] |= 12;
	      break;
	    default:
	      printf("opr: Warning: line %d, unknown char!\r\n", tmp);
	    }
	  }
	else
	  for (lcv = 0 ; lcv <= 38 ; lcv += 2) {
	    switch(buf[lcv]) {
	    case ' ':
	      break;
	    case '|':
	      dd.dmap[((tmp - 1) / 2) * 20 + lcv / 2] |= 1;
	      break;
	    case '!':
	      dd.dmap[((tmp - 1) / 2) * 20 + lcv / 2] |= 2;
	      break;
	    case ':':
	      dd.dmap[((tmp - 1) / 2) * 20 + lcv / 2] |= 3;
	      break;
	    default:
	      printf("opr: Warning: line %d, unknown char!\r\n", tmp);
	    }
	    if (buf[lcv+1] != ' ') {
	      if (buf[lcv+1] < 'a' || buf[lcv+1] > 'o') 
		printf("opr: Warning: line %d, unknown special!\r\n", tmp);
	      else
		dd.dmap[((tmp - 1) / 2) * 20 + lcv / 2] |= 
		  (buf[lcv+1] - 'a' + 1) * 16;
	    }
	  }
      }
      fclose(fp);
      printf("opr: map loaded!\r\n");
      printf("Are you sure you want to overwrite the old level <no> : ");
      tmp = getchar();
      if (tmp != 'y' && tmp != 'Y') {
	printf("No\r\n");
	break;
      }
      printf("Yes\r\n");
      printf("Overwriting...\r\n");
      while ((tmp = lock_open(FIL_LVL, O_RDWR, FIL_LVL_LD, 
			    FIL_LVL_LK, 60)) < 0 &&
	     (tmp == -2)) {
	printf("[File locked - sleeping]\r\n");
	sleep(roll(1,2));
      }
      if (tmp < 0) {
	printf("opr: error accessing level file %s!\r\n", FIL_LVL);
	break;
      }
      lseek(tmp, sizeof(n_dlvl) + (dgn - 1) * (DNAM_SZ + sizeof(dd.dstart) + 
					       8000) + DNAM_SZ
	    + sizeof(dd.dstart) + 400 * (lvl - 1), L_SET);
      write(tmp, dd.dmap, 400);
      lock_close(tmp, FIL_LVL_LD, FIL_LVL_LK);
      dlvl_init();
      printf("opr: Done!\r\n\n");
      break;
    case 'X':
    case 'M':
      if (tmp2 == 'X')
	printf("Extract level\r\n");
      else {
	printf("Map level\r\n");
	printf("opr: NOTE: This requires overstrike!\r\n");
      }
      if (n_dlvl < 0)
	dlvl_init();
      printf("Output file : ");
      unix_tty_cook();
      if (!fgets(buf, BUFSIZ, stdin))
        strcpy(buf, "\n");
      unix_tty_dgn();
      buf[strlen(buf) - 1] = 0;
      if (buf[0] == 0)
	break;
      if ((fp =  fopen(buf, "w")) == NULL) {
	fprintf(stderr,"opr: can't open file for output!\r\n%s:" , buf);
	perror("fopen");
	break;
      }
      printf("Which dungeon and level : ");
      unix_tty_cook();
      if (!fgets(nbuf, NAMELEN, stdin) ||
           sscanf(nbuf, "%d%d", &dgn, &lvl) != 2)
        dgn = -1;
      unix_tty_dgn();
      fflush(stdin);
      if (dgn < 1 || dgn > n_dlvl || lvl < 1 || lvl > 20) {
	printf("That's sort of impossible!\r\n");
	fclose(fp);
	unlink(buf);
	break;
      }
      dlvl_loadum(dgn, lvl);
      for (tmp = 1; tmp <= 20 ; tmp++) {
	for (lcv = 1 ; lcv <= 20; lcv++) 
	  if (tmp2 == 'X')
	    fprintf(fp, " %1.1s"," -=~" + 
		    ((dd.dmap[(tmp - 1) * 20 + (lcv - 1)] / 4) & 3));
	  else
	    fprintf(fp," %s", 
		    (((dd.dmap[(tmp - 1) * 20 + (lcv - 1)] / 4) & 3) == 1) ?
		    "_" : " ");
	fprintf(fp, "\r\n");
	for (lcv = 1 ; lcv <= 20 ; lcv++) {
	  fprintf(fp,"%1.1s", " |!:" + 
		  (dd.dmap[(tmp - 1) * 20 + (lcv - 1)] & 3));
	  if (dd.dmap[(tmp - 1) * 20 + (lcv - 1)] / 16 == 0)
	    fprintf(fp," ");
	  else
	    fprintf(fp,"%1.1s", " abcdefghijklmnop" + 
		    dd.dmap[(tmp - 1) * 20 + (lcv - 1)] / 16);
	}
	fprintf(fp, "| %d\r%s", tmp, (tmp2 == 'X') ? "\n" : "");
      }
      if (tmp2 == 'X') 
	fprintf(fp, " - - - - - - - - - - - - - - - - - - - -\r\n");
      else
	fprintf(fp, " _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\r\n\n");
      fprintf(fp, " 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0\r\n");
      fclose(fp);
      printf("opr: Done!\r\n\n");
      break;
    case 'I':
      printf("Init files\r\n\n");
      printf("Checking systems...\r\n");
      printf("The character file is %s.\r\n", FIL_CHR);
      tmp = open(FIL_CHR, O_RDONLY);
      if (tmp < 0) {
	printf("The character file can't be opened!\r\n");
	if (errno != ENOENT) {
	  printf("You need to check why.  The error follows:\r\n");
	  perror("open");
	  printf("\r");
	} else {
	  printf("This is because it does not exist!\r\n");
	  printf("Attempting to create the file...\r\n");
	  tmp = open(FIL_CHR, O_CREAT|O_WRONLY, 0644);
	  if (tmp < 0) {
	    printf("Still no luck, the error follows:\r\n");
	    perror("open");
	    printf("\r");
	  } else {
	    lcv = 0;
	    write(tmp, (char *) &lcv, sizeof(lcv));
	    printf("It worked!\r\n");
	    close(tmp);
	  }
	}
      } else {
	read(tmp, (char *) &lcv, sizeof(lcv));
	printf("There are currently %d slots in the file.\r\n", ntohl(lcv));
	close(tmp);
      }
      printf("The note/message of the day file is %s.\r\n", FIL_NOT);
      printf("\t(you can put any message you want in this file)\r\n");
      printf("The list of winners is in the ORB file (%s).\r\n", FIL_ORB);
      printf("\t(this file doesn't exist until someone wins!)\r\n");
      printf("The structure of the dungeons is stored in the level file.\r\n");
      printf("The level file is %s.\r\n", FIL_LVL);
      tmp = open(FIL_LVL, O_RDONLY);
      if (tmp < 0 && errno == ENOENT) {
	printf("The level file doesn't exist!\r\n");
	printf("The level file needs to be set up correctly...\r\n");
	printf("I'll attempt to create a null one, but you'll need to\r\n");
	printf("load some levels in the file.\r\n");
	tmp = open(FIL_LVL, O_CREAT|O_WRONLY, 0644);
	lcv = 0;
	write(tmp, (char *) &lcv, sizeof(lcv));
	close(tmp);
      } else {
	read(tmp, (char *) &lcv, sizeof(lcv));
	printf("There are currently %d dungeons enabled.\r\n", ntohl(lcv));
	close(tmp);
      }
      printf("The instructions file is %s.\r\n", FIL_INS);
      tmp = open(FIL_INS, O_RDONLY);
      if (tmp < 0) {
	printf("I can't open the instructions file.\r\n");
	perror("open");
	printf("\rToo bad for your players...\r\n");
      } else {
	printf("It is there!\r\n");
	close(tmp);
      }
      printf("\r\nEnd of INIT function.\r\n\n");
      break;
    case 'S':
    case 'D':
      if (n_dlvl < 0)
	dlvl_init();
      if (tmp2 == 'S') 
	printf("Set dungeon\r\n");
      else 
	printf("Change number of dungeons\r\n");
      printf("Current number of dungeons is %d.\r\n", n_dlvl);
      printf("%sumber [1..9, or anything else to abort] : ",
	     (tmp2 == 'D') ? "New n" : "N");
      tmp = getchar();
      if (tmp < '1' || tmp > '9') {
	printf("Abort\r\n");
	break;
      }
      printf("%c\r\n", tmp);
      tmp = tmp - '1' + 1;
      if (tmp2 == 'S') {
	if (tmp > n_dlvl) {
	  printf("There are not that many dungeons active!\r\n");
	  break;
	}
	lcv = sizeof(n_dlvl) + (tmp - 1) * (DNAM_SZ + sizeof(dd.dstart) +8000);
	lseek(dfd, lcv, L_SET);
	read(dfd, buf, DNAM_SZ);
	strcpy(dd.dnam, buf);
	printf("Dungeon name <%s> : ", buf);
	unix_tty_cook();
        if (!fgets(buf, DNAM_SZ - 1, stdin))
          strcpy(buf, "\n");
	buf[strlen(buf) - 1] = 0;
	if (buf[0] != 0) 
	  strcpy(dd.dnam, buf);
	printf("The start code is formed by:\r\n");
	printf("\t   code = 20 * (start_row - 1) + (start_col - 1)\r\n");
	printf("\tor code = 400 to close the dungeon\r\n");
	read(dfd, (char *) &(dd.dstart), sizeof(dd.dstart));
	tmp = ntohs(dd.dstart);
	printf("Start code <%d> : ", tmp);
	fflush(stdin);
        if (!fgets(buf, BUFSIZ, stdin))
          strcpy(buf, "\n");
	buf[strlen(buf)  - 1] = 0;
	if (buf[0] != 0) {
	  tmp = atoi(buf);
	  dd.dstart = htons(tmp);
	}
	printf("Setting %s's start code to %d.\r\n", dd.dnam, tmp);
	unix_tty_dgn();
      }
      while ((tmp3 = lock_open(FIL_LVL, O_RDWR, FIL_LVL_LD, 
			    FIL_LVL_LK, 60)) < 0 &&
	     (tmp3 == -2)) {
	printf("[File locked - sleeping]\r\n");
	sleep(roll(1,2));
      }
      if (tmp3 < 0) {
	printf("opr: error on access to file??\r\n");
	perror("lock_open");
	printf("\r");
	break;
      }
      if (tmp2 == 'S') {
	lseek(tmp3, lcv, L_SET);
	write(tmp3, dd.dnam, DNAM_SZ);
	write(tmp3, (char *)&(dd.dstart), sizeof(dd.dstart));
      } else {
	lseek(tmp3, 0, L_SET);
	n_dlvl = tmp;
	tmp = htonl(tmp);
	write(tmp3, (char *) &tmp, sizeof(tmp));
      }
      lock_close(tmp3, FIL_LVL_LD, FIL_LVL_LK);
      dlvl_init();
      printf("opr: Done!\r\n\n");
      break;
    case 'C':
      printf("Clean the database\r\n");
      printf("Delete chars not accessed in how many days (<cr> to abort)? ");
      unix_tty_cook();
      if (!fgets(buf, BUFSIZ, stdin))
         strcpy(buf, "\n");
      unix_tty_dgn();
      fflush(stdin);
      buf[strlen(buf) - 1] = 0;
      if (buf[0] == 0) {
	printf("Aborted.\r\n\n");
	break;
      }
      tmp = atoi(buf);
      if (tmp < 1) {
	printf("opr: Too short! I won't do that.\r\n");
	break;
      }
      tmp = tmp * 24 * 60 * 60; /* convert to secs */
      tmp = time(0) - tmp;
      sprintf(buf,"%s_NEW%d", FIL_CHR, getpid());
      if ((tmp3 = open(buf, O_CREAT|O_RDWR, 0644)) < 0) {
	printf("opr: can't open new file %s!\r\n", buf);
	perror("open");
	printf("\r");
	break;
      }
      close(tmp3);
      while ((tmp3 = lock_open(buf, O_RDWR, FIL_CHR_LD, 
			     FIL_CHR_LK, 60)) < 0 &&
	     (tmp3 == -2)) {
	printf("[File locked - sleeping]\r\n");
	sleep(roll(1,2));
      }
      if (tmp3 < 0) {
	printf("opr: unexpected error opening %s!\r\n", buf);
	perror("lock_open");
	break;
      }
      printf("opr: Cleaning...\r\n");
      chr_rset();
      newc = 0;
      write(tmp3, (char *) &newc, sizeof(newc));
      while ((cptr = chr_scan()) != NULL) {
	if (cptr->c[0] == 0)
	  continue;
	if (ntohl(cptr->c[60]) < tmp) {
	  printf("%s is history!\r\n", cptr->nam[0]);
	  continue;
	}
	newc++;
	write(tmp3, (char *) cptr, sizeof(struct chr));
      }
      lseek(tmp3, 0, L_SET);
      write(tmp3, (char *) &newc, sizeof(newc));
      printf("\r\nopr: recalculating size of file...\r\n");
      printf("opr: number of slots = %d\r\n", newc);
      printf("opr: Linking in new char file...\r\n");
      if (unlink(FIL_CHR) < 0) {
	printf("Can't unlink old char file???\r\n");
	unlink(buf);
	lock_close(tmp3, FIL_CHR_LD, FIL_CHR_LK);
	break;
      }
      link(buf, FIL_CHR);
      unlink(buf);
      lock_close(tmp3, FIL_CHR_LD, FIL_CHR_LK);
      printf("opr: Done!\r\n\n");
      chr_rset();
      break;
    case '<':
      printf("Read in current char\r\n");
      if (loaded == YEP) {
	printf("opr: %s is already loaded and locked in.\r\n", u.n[0]);
	printf("opr: Please '>' unload him first.\r\n");
	break;
      }
      printf("Character name (<cr> = abort)? ");
      unix_tty_cook();
      if (!fgets(buf, BUFSIZ, stdin))
         strcpy(buf, "\n");
      unix_tty_dgn();
      buf[strlen(buf) - 1] = 0;
      if (buf[0] == 0) {
	printf("Aborted.\r\n");
	break;
      }
      if ((tmp = chr_load(buf, LOCK)) == YEP) {
	loaded = YEP;
	printf("Loaded!\r\n");
      } else {
	if (tmp == MAYBE) {
	  printf("Char is locked!\r\n");
	  printf("Break lock (dangerous) ? ");
	  tmp = getchar();
	  if (tmp == 'y' || tmp == 'Y') {
	    printf("Yes\r\n");
	    chr_load(buf, NOLOCK);
	    u.c[57] = 1;
	    chr_save(YEP);
	    printf("Lock broken...\r\n");
	    break;
	  } else
	    printf("No\r\n");
	}
	printf("Load failed!\r\n");
	break;
      }
      printf("opr: %s's life is in your hands!\r\n", buf);
      break;
    case '>':
      printf("Save current char\r\n");
      if (loaded == NOPE) {
	printf("Why don't you load a char first??\r\n");
	break;
      }
      printf("Are you sure you want to save %s ? ", u.n[0]);
      tmp = getchar();
      if (tmp != 'y' && tmp != 'Y') {
	printf("No\r\n");
	break;
      }
      printf("Yes\r\n");
      if (chr_save(YEP) == NOPE) 
	printf("It failed!\r\n\n");
      else {
	printf("opr: Done!\r\n\n");
	loaded = NOPE;
      }
	break;
    case 'A':
    case 'N':
	if (loaded == NOPE) {
	  printf("\r\nLoad something first!\r\n");
	  break;
	}
      if (tmp2 == 'A')  {
	printf("Attribute set/print\r\n");
	printf("Attribute number (<cr> = abort) ? ");
      }
      else {
	printf("Name set/print\r\n");
	printf("Which name, 0 = name, 1 = secret name (<cr> = abort) ? ");
      }
      unix_tty_cook();
      if (!fgets(buf, BUFSIZ, stdin))
         strcpy(buf, "\n");
      unix_tty_dgn();
      buf[strlen(buf) - 1] = 0;
      if (buf[0] == 0) {
	printf("Aborted.\r\n");
	break;
      }
      tmp = atoi(buf);
      if (tmp < 0 || (tmp2 == 'A' && tmp > 64) || 
	             (tmp2 != 'A' && tmp > 1)) {
	printf("Out of range!\r\n");
	break;
      }
      printf("Current value is:\r\n");
      if (tmp2 == 'A') 
	printf("  u.c[%d] = %d\r\n", tmp, u.c[tmp]);
      else
	printf("  u.n[%d] = %s\r\n", tmp, u.n[tmp]);
      printf("New value (<cr> to keep old) ? ");
      unix_tty_cook();
      if (!fgets(buf, BUFSIZ, stdin))
         strcpy(buf, "\n");
      unix_tty_dgn();
      buf[strlen(buf) - 1] = 0;
      if (buf[0] == 0) {
	printf("\r\n");
	break;
      }
      if (tmp2 == 'A')
	u.c[tmp] = atoi(buf);
      else
	strncpy(u.n[tmp], buf, NAMELEN);
      printf("opr: Done!\r\n\n");
      break;
    default:
      printf("%%Illegal option.\r\n");
      break;
    }
  }
}
