/*
 * udd lock.c file - file locking (unix), semi-based on mmdf locker.
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include "defs.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>

/* 
 * lock_open: open with simple link-based file locking.
 * on failure: -1 if fixed error, -2 if we should try again later.
 */

int lock_open(file, mode, lockdir, lockfile, maxtime)

char *file, *lockdir, *lockfile;
int mode, maxtime;

{
  int fd, fd2, rv;
  char lfile[MAXPATHLEN], ltmpfile[MAXPATHLEN];
  struct stat st;
  time_t now;

  if (strlen(lockdir) + strlen(lockfile) + 16 > MAXPATHLEN)
    return(-1);

 ReTry:
  fd = open(file, mode, 0777);
  if (fd < 0)
    return(-1);

  strcpy(lfile, lockdir);
  strcat(lfile, "/");
  strcat(lfile, lockfile);

  strcpy(ltmpfile, lockdir);
  strcat(ltmpfile, "/");
  strcat(ltmpfile, "tmpXXXXXX");

  time(&now);
  if (stat(lfile, &st) >= 0) {
    /* infinite or under max? */
    if (maxtime <= 0 || (now - st.st_mtime) <= maxtime) { 
      close(fd);
      return(-2);
    }
    /* stale lock... (shouldn't happen often) */
    unlink(lfile);
    sleep(5 + (getpid() % 64));	/* semi-random backoff */
    close(fd);
    goto ReTry;
  }
  
  fd2 = mkstemp(ltmpfile);
  if (fd2 < 0) {
    close(fd);
    return(-1);
  }
  close(fd2);

  rv = link(ltmpfile, lfile);
  unlink(ltmpfile);
  if (rv < 0) {
    close(fd);
    return(-2);
  }

  /* done! */
  return(fd);
}

/*
 * lock_close: close and unlock
 */

int lock_close(fd, lockdir, lockfile)

int fd;
char *lockdir, *lockfile;

{
  char lfile[MAXPATHLEN];
  
  if (strlen(lockdir) + strlen(lockfile) + 16 > MAXPATHLEN)
    return(-1);
  
  strcpy(lfile, lockdir);
  strcat(lfile, "/");
  strcat(lfile, lockfile);

  unlink(lfile);
  return(close(fd));
}
