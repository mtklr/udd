/*
 * udd cbt_hit.c file - combat hit functions
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include "defs.h"
#include "cbt.h"

int cbt_uhitm(dam)

int dam;

{
  printf("You did %d point%s of damage.\r\n", dam, (dam == 1) ? "" : "s");
  m2 -= dam;
  if (m2 < 1) {
    printf("It died...\r\n");
    if (rnd() > 0.9) /* XXX */
      u.i[8] = 0;
    u.i[6] = 0;
    dead = 1;
  }
}

int cbt_ohitu(dam)

int dam;

{
  int pts;
  pts = rnd() * ( l * (4 - u.c[7])) + 1;
  printf("You suffer %d hit point%s.\r\n", pts, (pts == 1) ? "" : "s");
  u.c[11] -= pts;
  if (u.c[11] < 1) {
    printf("Your life has been terminated.\r\n");
    return(utl_death());
  }
  return(NOPE);
}
