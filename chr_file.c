/*
 * udd chr_file.c file - character file access routines
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
#include <signal.h>

struct chr *c;
int numc = -1;
int numc2 = -1;
int cfd = -1;

int chr_load(name, lock)

char *name;
int lock;

{
  int tmp;
  chr_rset();
  while ((c = chr_scan()) != NULL) 
    if (c->c[0] != 0 && strcmp(c->nam[0], name) == 0) {
      strncpy(u.n[0], c->nam[0], NAMELEN);
      strncpy(u.n[1], c->nam[1], NAMELEN);
      for (tmp = 0 ; tmp < 65; tmp++)
	u.c[tmp] = ntohl(c->c[tmp]);
      if (lock == LOCK && u.c[57] != 0) 
	return(MAYBE);       /* lock failed! */
      if (lock == LOCK && chr_lck_nuke(LOCK) == NOPE)
	return(MAYBE);       /* lock failed! */
      if (lock == LOCK)
        u.c[57] = 1;
      return(YEP);
    }
  return(NOPE);
}

void chr_rset()

{
  if (cfd >= 0)
    close(cfd);    /* always close in case of a char file clean */
  if ((cfd = open(FIL_CHR, O_RDONLY)) < 0)  {
    fprintf(stderr,"Can't open char file %s!\r\n", FIL_CHR);
    perror("open");
    fprintf(stderr,"Please consult an operator...\r\n");
    unix_exit(NOPE);
  }
  lseek(cfd, 0, L_SET);
  if (read(cfd, (char *) &numc, sizeof(numc)) < 0) {
    fprintf(stderr, "Can't find # of chars in file!\r\n");
    perror("read");
    unix_exit(NOPE);
  }
  numc2 = numc = ntohl(numc);
}


struct chr *chr_scan()

{
  static struct chr ccc;
  if (numc2-- < 1)
    return(NULL);
  if (read(cfd, (char *) &ccc, sizeof(struct chr)) < 0) {
    fprintf(stderr, "Unexpected read error (this shouldn't happen)\r\n");
    perror("read");
  }
  return(&ccc);
}

int chr_save(unlock)

int unlock;

{
  struct chr sav, *cp;
  int cwfp;
  int tmp;
  if (u.c[57] == 0) {
    printf("%s: internal error!  Tried to save unlocked char!\r\n", ddd);
    return(NOPE);
  }
#ifdef SIGTSTP
  signal(SIGTSTP, SIG_IGN);
#endif
  while((cwfp = lock_open(FIL_CHR, O_RDWR, FIL_CHR_LD, FIL_CHR_LK, 3)) < 0
	&& (cwfp == -2)) {
    printf("[File busy - please wait]\r\n");
    sleep(roll(1,2));
  }
  chr_rset();
  while ((cp = chr_scan()) != NULL) 
    if (cp->c[0] != 0 && strcmp(u.n[0], cp->nam[0]) == 0)
      break;
  if (cp == NULL) {
    printf("%s: Can't find you in the file!  Save failed!\r\n", ddd);
    lock_close(cwfp, FIL_CHR_LD, FIL_CHR_LK);
#ifdef SIGTSTP
    signal(SIGTSTP, sigstop);
#endif
    return(NOPE);
  }
  if (cp->c[57] == 0) {
    printf("%s: Internal error!  Char in file is not locked!!\r\n");
    lock_close(cwfp, FIL_CHR_LD, FIL_CHR_LK);
#ifdef SIGTSTP
    signal(SIGTSTP, sigstop);
#endif
    return(NOPE);
  }
  tmp = sizeof(long) + (numc - numc2 - 1) * sizeof(struct chr);
  lseek(cwfp, tmp, L_SET);
  strncpy(sav.nam[0], u.n[0], NAMELEN);
  strncpy(sav.nam[1], u.n[1], NAMELEN);
  for (tmp = 0 ; tmp < 65 ; tmp++)
    sav.c[tmp] = htonl(u.c[tmp]);
  if (unlock == YEP)
    u.c[57] = sav.c[57] = 0;   /* save forces unlock! */
  if (write(cwfp, (char *) &sav, sizeof(struct chr)) < 0) {
    fprintf(stderr,"Error saving char: ");
    perror("write");
    lock_close(cwfp, FIL_CHR_LD, FIL_CHR_LK);
    chr_rset();
#ifdef SIGTSTP
    signal(SIGTSTP, sigstop);
#endif
    return(NOPE);
  }
  lock_close(cwfp, FIL_CHR_LD, FIL_CHR_LK);
  chr_rset();
  autosave = 0;
#ifdef SIGTSTP
  signal(SIGTSTP, sigstop);
#endif
  return(YEP);
}

int chr_lck_nuke(lock)  /* lock/nuke spot n - 1 */

int lock;

{
  int cwfp;
  int loc;
  int retval = NOPE;
  struct chr zap, zold;
  if (lock == LOCK) {
    for (cwfp = 0; cwfp < 65 ; cwfp++)
      zap.c[cwfp] = c->c[cwfp];
    strcpy(zap.nam[0], u.n[0]);
    strcpy(zap.nam[1], u.n[1]);
  } else
    for (cwfp = 0 ; cwfp < 65 ; cwfp++)
      zap.c[cwfp] = 0;
  while((cwfp = lock_open(FIL_CHR, O_RDWR, FIL_CHR_LD, FIL_CHR_LK, 3)) < 0
	&& (cwfp == -2)) {
    printf("[File busy - please wait]\r\n");
    sleep(roll(1,2));
  }
  loc = sizeof(long) + (numc - numc2 - 1) *sizeof(struct chr);
  lseek(cwfp, loc, L_SET);
  if (lock == LOCK) {
    read(cwfp, (char *) &zold, sizeof(struct chr));
    if (zold.c[57] == 0) {   /* not locked */
      retval = YEP;
      zap.c[57] = 1;
      c->c[57] = 1;
      lseek(cwfp, loc, L_SET);
      write(cwfp, (char *) &zap, sizeof(struct chr));
    }
  } else {
    write(cwfp, (char *) &zap, sizeof(struct chr));
    retval = YEP;
  }
  lock_close(cwfp, FIL_CHR_LD, FIL_CHR_LK);
  return(retval);
}


int chr_new()      /* save u. in file, lock it */

{
  int cwfp, tmp;
  struct chr *cp, cpout;
  chr_rset();
  while((cwfp = lock_open(FIL_CHR, O_RDWR, FIL_CHR_LD, FIL_CHR_LK, 3)) < 0
	&& (cwfp == -2)) {
    printf("[File busy - please wait]\r\n");
    sleep(roll(1,2));
  }
  if (cwfp < 0) {
    printf("%d: Can't get access to char file %s.\r\n", ddd, FIL_CHR);
    return(NOPE);
  }
  while ((cp = chr_scan()) != NULL)
    if (cp->c[0] != 0 && strcmp(u.n[0], cp->nam[0]) == 0) /* name steal */
      break;
  if (cp != NULL) {
    lock_close(cwfp, FIL_CHR_LD, FIL_CHR_LK);
    return(NOPE);
  }
  u.c[57] = 1;
  chr_rset();
  while ((cp = chr_scan()) != NULL) 
    if (cp->c[0] == 0)
      break;
  if (cp != NULL) {
    tmp = sizeof(long) + (numc - numc2 - 1) * sizeof(struct chr);
    lseek(cwfp, tmp, L_SET);
  } else {
    numc++;
    tmp = htonl(numc);
    lseek(cwfp, 0, L_SET);
    write(cwfp, (char *) &tmp, sizeof(tmp));
    tmp = sizeof(long) + (numc - 1) * sizeof(struct chr);
    lseek(cwfp, tmp, L_SET);
  } 
  strncpy(cpout.nam[0], u.n[0], NAMELEN);
  strncpy(cpout.nam[1], u.n[1], NAMELEN);
  for (tmp = 0 ; tmp < 65 ; tmp++)
    cpout.c[tmp] = htonl(u.c[tmp]);
  if (write(cwfp, (char *) &cpout, sizeof(struct chr)) < 0) {
    fprintf(stderr,"Error saving char: ");
    perror("write");
    fprintf(stderr,"\r");
    lock_close(cwfp, FIL_CHR_LD, FIL_CHR_LK);
    chr_rset();
    return(NOPE);
  }
  lock_close(cwfp, FIL_CHR_LD, FIL_CHR_LK);
  chr_rset();
  return(YEP);
} 
