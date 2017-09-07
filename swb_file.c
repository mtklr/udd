/*
 * udd swb_file.c file - switchboard's file routines
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include <stdio.h>
#include "defs.h"

char buf[BUFSIZ];

/*
 * stuff you might want to change...
 *
 */

int swb_ppnok()    /* NOPE=can't run program, MAYBE=can't Create, Run or
		           Kill, YEP=full access */

{
  return(YEP);
}


int swb_wiz()      /* 1=access to operator program and no password kill */
		   /* 0=normal user */

{
  if (getuid() == 54171)       /* uid 54171 = chuck */
    return(1);
  else
    return(0);
}



/* DO NOT EDIT BELOW THIS LINE */

void swb_note(msg, file, wiz)

int wiz;
char *msg, *file;

{
  FILE *fopen(), *fp;
  if ((fp = fopen(file, "r")) == NULL)
    return;
  printf("%s\r\n", msg);
  while (fgets(buf, BUFSIZ, fp) != NULL) 
    if (buf[0] != '!' || wiz != 0)
      printf("%s\r", buf);
  printf("\n");
  fclose(fp);
}
