/*
 * udd dlvl_file.c file - level file routines
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include "defs.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>

void dlvl_init()

{
  if (dfd >= 0)
    close(dfd);
  if ((dfd = open(FIL_LVL, O_RDONLY)) < 0) {
    fprintf(stderr, "%s: Can't open level file %s!\r\n", ddd, FIL_LVL);
    perror("open");
    fprintf(stderr, "\r?Program lost, sorry.\r\n");
    unix_exit(1);
  }
  if (read(dfd, (char *) &n_dlvl, sizeof(n_dlvl)) != sizeof(n_dlvl)) {
    fprintf(stderr,"%s: Can't read # of dungeons!\r\n", ddd);
    perror("read");
    fprintf(stderr,"\r?Program lost, sorry.\r\n");
    unix_exit(1);
  }
  n_dlvl = ntohl(n_dlvl);
}

int dlvl_choose()

{
  int tmp, tmp2;
  tmp2 = 0;
  if (dfd < 0)
    dlvl_init();
  printf("You may now choose the Dungeon for your adventure.\r\n\n");
  printf("The choices are:\r\n");
  for (tmp = 0 ; tmp < n_dlvl ; tmp++) {
    lseek(dfd, sizeof(n_dlvl) + tmp * (DNAM_SZ + sizeof(dd.dstart) + 8000), 
	  L_SET);
    read(dfd, dd.dnam, DNAM_SZ);
    read(dfd, (char *) &(dd.dstart), sizeof(dd.dstart));
    dd.dstart = ntohs(dd.dstart);
    printf("[%d] %s%s\r\n", tmp + 1, dd.dnam,
	   (dd.dstart == 400) ? " (closed for repairs)" : "");
    if (dd.dstart == 400)
      tmp2++;
  }
  if (tmp2 >= n_dlvl) {
    printf("Gee, all dungeons are closed?  Time to punt...\r\n");
    printf("You better find a dungeon operator!\r\n");
    return;
  }
  printf("Press the number of the dungeon, or <CR> to ");
  if (u.c[18] == 0)
    printf("let me choose.\r\n");
  else
    printf("continue in the same dungeon.\r\n");
 ctop:
  printf("Dungeon number ? ");
  tmp = getchar();
  if (tmp < 0)
    tmp = '1';
  dd.dstart = 400;
  if (u.c[18] != 0 && tmp == '\r')
    tmp = u.c[18] + '1' - 1;
  if (tmp == '\r') 
    while (dd.dstart == 400) { 
      tmp2 = roll(1, n_dlvl);
      lseek(dfd, sizeof(n_dlvl) + (tmp2 - 1) * (DNAM_SZ + 8000 + 
						sizeof(dd.dstart)), L_SET);
      read(dfd, dd.dnam, DNAM_SZ);
      read(dfd, (char *) &(dd.dstart), sizeof(dd.dstart));
      dd.dstart = ntohs(dd.dstart);
      if (dd.dstart != 400) {
	printf("%s\r\n", dd.dnam);
	return(tmp2);
      }
    }
  tmp = tmp - '1' + 1;
  if (tmp < 1 || tmp > n_dlvl) {
    printf("Great.  Now how about a valid answer?\r\n");
    goto ctop;
  }
  lseek(dfd, sizeof(n_dlvl) + (tmp - 1) * (DNAM_SZ + 8000 + 
					   sizeof(dd.dstart)), L_SET);
  read(dfd, dd.dnam, DNAM_SZ);
  read(dfd, (char *) &(dd.dstart), sizeof(dd.dstart));
  dd.dstart = ntohs(dd.dstart);
  printf("%s\r\n", dd.dnam);
  if (dd.dstart == 400) {
    printf("That dungeon is CLOSED for repairs.\r\n");
    if (u.c[61] == 1) {
      printf("However, since you are in debug mode, you may enter...\r\n");
      dd.dstart = 0;
    } else
      goto ctop;
  }
  return(tmp);
}


int dlvl_loadum(dno, lvl)

int dno, lvl;

{
  int tmp, tmp2;
  if (dfd < 0)
    dlvl_init();
  if (lvl < 1 || lvl > 20) {
    printf("%s: Warning: bogus level request (internal error)!\r\n", ddd);
  }
  tmp = sizeof(n_dlvl) + (dno - 1) * (DNAM_SZ + 8000 + sizeof(dd.dstart));
  tmp += DNAM_SZ + sizeof(dd.dstart);       /* skip name, starting location */
  tmp += 400 * (lvl - 1);    /* find lvl */
  lseek(dfd, tmp, L_SET);
  tmp2 = read(dfd, dd.dmap, sizeof(dd.dmap));
  if (tmp2 < 0)
    return(tmp2);
  else
    return(tmp);
}

void dlvl_get_start(dno)

int dno;

{
  int tmp;
  if (dfd < 0)
    dlvl_init();
  tmp = sizeof(n_dlvl) + (dno - 1) * (DNAM_SZ + 8000 + sizeof(dd.dstart));
  tmp += DNAM_SZ;
  lseek(dfd, tmp, L_SET);
  tmp = read(dfd, (char *) &(dd.dstart), sizeof(dd.dstart));
  if (tmp < 0)
    perror("dlvl_get_start: read");
  dd.dstart = ntohs(dd.dstart);
}
