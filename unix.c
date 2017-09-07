/*
 * udd unix.c file - unix (or system level) functions
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include "defs.h"
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>

#undef OLDTTY
#ifdef OLDTTY

#include <sgtty.h>
static struct sgttyb out, cur;
#define TTY_GET(X) ioctl(0, TIOCGETP, (X))
#define TTY_SET(X) ioctl(0, TIOCSETP, (X))

#else

#include <termios.h>
static struct termios out, cur;
#define TTY_GET(X) tcgetattr(0, (X))
#define TTY_SET(X) tcsetattr(0, TCSANOW, (X))

#endif


void unix_init()

{
  TTY_GET(&out);
  TTY_GET(&cur);
  signal(SIGINT, SIG_IGN);
  /*signal(SIGQUIT, SIG_IGN); /*DEBUG XXX CDC CORE DUMP*/
  signal(SIGHUP, sighup);
#ifdef SIGTSTP
  signal(SIGTSTP, sigstop);
#endif
}

void unix_exit(n)

int n;

{
  TTY_SET(&out);
  exit(n);
}

void sigstop()

{
  TTY_SET(&out);
  printf("[Suspending...]\r\n");
  kill(getpid(), SIGSTOP);
  printf("[Resuming...]\r\n");
  TTY_SET(&cur);
}

void unix_tty_cook()

{
#ifdef OLDTTY
  cur.sg_flags |= ECHO|CRMOD;
  cur.sg_flags &= ~CBREAK;
#else
  cur.c_iflag |= ICRNL;
  cur.c_oflag |= ONLCR;
  cur.c_lflag |= ECHO | ICANON;
#endif
  TTY_SET(&cur);
}

void unix_tty_pswd()

{
#ifdef OLDTTY
  cur.sg_flags |= CRMOD;
  cur.sg_flags &= ~ECHO & ~CBREAK;
#else
  cur.c_iflag |= ICRNL;
  cur.c_oflag |= ONLCR;
  cur.c_lflag |= ICANON;
  cur.c_lflag &= ~ECHO;
#endif
  TTY_SET(&cur);
}

void unix_tty_dgn()

{
#ifdef OLDTTY
  cur.sg_flags |= CBREAK;
  cur.sg_flags &= ~ECHO & ~CRMOD;
#else
  cur.c_iflag &= ~ICRNL;
  cur.c_oflag &= ~ONLCR;
  cur.c_lflag &= ~(ICANON | ECHO);
#endif
  TTY_SET(&cur);
}


char *month[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep",
		  "Oct","Nov","Dec"};

char *unix_date(dat)

long dat;

{
  struct tm *t;
  static char buf[12];
  t = localtime(&dat);
  sprintf(buf, "%02d-%s-%02.2d", t->tm_mday, month[t->tm_mon], 
          t->tm_year % 100);
  return(buf);
}

void sighup()

{
  if (autosave != 0) {
#ifdef L_STRICT
    chr_save(NOPE);    /* try and save'em - XXX the old ^Z trick */
#else
    chr_save(YEP);
#endif
  }
  unix_exit(0);
}
