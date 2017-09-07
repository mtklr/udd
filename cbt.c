/*
 * udd cbt.c file - combat program
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#define CBTMAIN
#include "defs.h"
#include "cbt.h"
#include <math.h>
#include <ctype.h>

struct slev fns[4] = {
  {ms1, cs1},
  {ms2, cs2},
  {ms3, cs3},
  {ms4, cs4}
};


int cbt_main()

{
  int ok, s1, tmp;
  int aflag = 0;
  dead = -1;
  if (u.c[64] == CBT_ALTR) {  /* ask for tough one! */
    aflag = 1;
    u.c[64] = CBT_NORM;
  }
  switch(u.c[64]) {
  case CBT_NORM:
    if (u.c[63] != 0) 
      switch (u.c[63]) {
      case SPC_THR:
	m = 11;
	mnam = "Dwarf Lord";
	m1 = u.c[15] + 10 + roll(1,6);
	m_str = roll(1, mm[m].m) + 5;
	m_arm = roll(1, m1);
	break;
      case SPC_ALT:
	m = 18 + roll(1,2);
	mnam = "Demon Prince";
	m1 = u.c[15] + 5 + roll(1,20);
	m_str = roll(1,mm[m].m) + 5;
	m_arm = roll(1, m1);
	break;
      case SPC_DGN1:
      case SPC_DGN2:
	m = 20;
	mnam = "Dragon Lord";
	m1 = u.c[15] + roll(1,20);
	m_str = roll(1,mm[m].m);
	m_arm = roll(1, m1) - 1;
	break;
      default:
	printf("cbt: FAT. ERROR, spec unknown!\r\n");
	unix_exit(1);
      }
    else {  /* no special */
      ok = 0;
      while (ok == 0) {
	ok = 1;
	if (aflag == 1) { /* ask for nasty (currently undead) */
	  m = (int) (pow((double)(rnd()), (double)(2.0 - 0.04 * u.c[15]))
	    * 6.0 + 1.0);
	} else {
	  m = roll(1,20);
	  if (m > 6)
	    m = (int)(6.0 + pow((double)(rnd()), (double)(2.0 - 0.04 * u.c[15]))
	      * 14.0 + 1.0);
	  else
	    m = (int) (pow((double)(rnd()), (double)(2.0 - 0.04 * u.c[15]))
	      * 6.0 + 1.0);
	  if (mm[m].mindunlvl > u.c[15])
	    ok = 0;
	}
	if (mm[m].maxdunlvl < u.c[15])
	  ok = 0;
	mnam = mm[m].nam;
	m1 = roll(1, (int)(u.c[15] * 1.5)) + (int)(rnd()*rnd()*rnd()*3);
	if (m1 < mm[m].minmonlvl)
	  ok = 0;
      }
      m_str = roll(1, mm[m].m);
      m_arm = roll(1, m1) - 1;
      l = m1;
      s1 = 1; /* s1 ???XXX*/
      if (mm[m].m + m1 >= 1.2 * u.c[10] && u.c[11] > 1)
	if (roll(1,20) <= u.c[6]) {
	  printf("A level %d %s takes one look at you...", m1, mnam);
	  sleep(1);
	  printf(" snarls and knocks you cold.\r\n");
	  u.c[11] = u.c[11] / 2.0;
	  sleep(2);
	  printf("You wake up feeling half dead.\r\n");
	  printf("It has stalked off.\r\n");
	  u.i[6] = u.i[7] = u.i[8] = 0;
	  u.l[u.c[16]][u.c[17]] = 16*u.i[5] + 4*u.i[2] + u.i[1];
	  if (u.i[5] == 0) {
	    u.c[64] = DGN_PROMPT;
	    return(NOPE);
	  }
	  return(spc_main());
	}
    }
    m2 = m2_old = roll(m1, mm[m].m);
    printf("You have encountered a level %d %s!\r\n", m1, mnam);
    if (u.c[61] == 1)
      printf("STR: %2d, ARM: %2d, HITS: %2d, HDIE: %2d\r\n", m_str, m_arm,
	     m2, mm[m].m);
    ok = dead = 0;
    if (u.c[42] > 0 && roll(1,20) < 15+u.c[8] - m1)
      ok = 1;
    /*if (roll(1,20) <= u.c[2] + u.c[5] / 2.0) 
      ok = 1;  */
    if (roll(1,20) + m1 <= ((u.c[2] + u.c[5]) / 2.0) + u.c[8])  /*CDC XXX???*/
      ok = 1;
    mskip = 0;     /* for spells */
    autoevade = 0;
    gone = 0;
    while (dead == 0) {
      if (ok != 0) {
      ask:
	if (autoevade == 0) {
	  printf("Press (F)ight%s or (E)vade: ", 
		 (u.c[31]+u.c[32]+u.c[33]+u.c[34] == 0) ? ""  : ", (C)ast,");
	  in = getchar();
	  if (islower(in))
	    in = toupper(in);
        } else 
	  in = 'E';
	if (in < 0)
	  in = 'E';
	if (in == 'C' && u.c[31]+u.c[32]+u.c[33]+u.c[34] == 0)
	  in = '?';
	if (in != 'F' && in != 'C' && in != 'E') {
	  printf("The %s is not amused...\r\n", mnam);
	  goto ask;
	}
	if (in == 'E') {
	  if (autoevade == 0) {
	    (rnd() < 0.9) ? printf("Evade\r\n") : printf("Run away\r\n");
	  } else {
	    autoevade = 0;
	  }
	  if (((u.i[1] == 1 || u.c[1] == 3) && (u.i[2] == 1 || u.i[2] == 3))
	      && ((u.l[u.c[16]][u.c[17]+1] & 1) == 1)
	      && ((u.l[u.c[16]+1][u.c[17]] & 4) == 4)) {
	    printf("There's no where to go!\r\n");
	  } else {
            tmp = u.c[52]; /* XXX make evade possible if no cloak */
            if (tmp < 1)
              tmp = 1;
	    if (roll(2,10) <= u.c[5] && sqrt(tmp/25.0)*25.0 >=
		roll(1,25)) {
	      printf("You made it!\r\n");
	      ok = 0;
	      while (ok == 0) {
		ok = 1;
		in = roll(1,4);
		if (in == 1 && u.i[2] == 1) { 
		  ok = 0;
		  continue;
		}
		if (in == 2  && fni1(u.l[u.c[16]][u.c[17]+1],1) == 1) {
		  ok = 0;
		  continue;
		}
		if (in == 3  && fni1(u.l[u.c[16]+1][u.c[17]],2) == 1) {
		  ok = 0;
		  continue;
		}
		if (in == 4 && u.i[1] == 1) {
		  ok = 0;
		  continue;
		}
		if (rnd() > 0.5)
		  u.i[6] = u.i[7] = u.i[8] = 0;
		u.c[64] = DGN_AMOVE;
		u.c[63] = in;
		return(NOPE);
	      }
	    }
	  }
	  printf("You're rooted to the spot... The %s bites back!\r\n", mnam);
	}
	if (in == 'C') {
	  printf("Cast\r\n");
	  if (cbt_cast() == YEP)
	    return(YEP);
	}
	if (in == 'F') {
	  printf("Fight\r\n");
	  i1 = 3 * u.c[5] + u.c[1] + u.c[22] + u.c[8] * (5 - u.c[7])
	    - m1 * mm[m].m / 2.0;
	  /* 3% per DEX, 1% per STR, +1% * wep, +3,4,5% for level,
	     -1...10% per Mlevel */
	  if (u.c[40] > 0)
	    i1 += 5;         /* 5% for PRAY spell */
	  i1_old = i1;       /* save value */
	  i1 *= (u.c[10] + u.c[11]) / (2.0 * (double) u.c[10]);
	                     /* deduct for hits taken */
	  dice = roll(1, 100);
	  if (u.c[61] == 1)
	    printf("CHANCE: %d%% [%d%% max], DIE: %d\r\n", i1, i1_old, dice);
	  if (dice > i1) {
	    printf("You missed%s.\r\n", (dice <= i1_old) ?
		   " due to your weakened condition" : "");
	  } else {
	    d = roll(1, 8 - 2* u.c[7] + u.c[22]);/* base on class/wep */
	    i2 = u.c[1];     /* base STR */
	    if (u.c[44] > 0)
	      i2 += 3;       /* STR spell */
	    if (i2 > 14)
	      d += roll(1, i2 - 14);   /* bonus! */
	    else if (i2 < 7)
	      d -= roll(1, 7 - i2);    /* weakling! */
	    if (u.c[61] == 1)
	      printf("FORCE: %d\r\n", d);
	    if (m_arm > 0)
	      d -= roll(1, m_arm)+roll(1,m_arm);   /*one for ARM,one for shld*/
	    if (d < 1)
	      printf("It fends you off.\r\n");
	    else 
	      cbt_uhitm(d);
	  }
	}
      }
      ok = 1;
      d = 0;
      if (mskip == 1) {
	mskip = 0;
	continue;            /* monst skips a turn */
      }
      if (dead == 0) {
	if (m == 20) {
	  printf("The %s breathes fire at you!\007\r\n", mnam);
	  sleep(2);
	  d = roll(2,20) + m1;
	  if (roll(1,20) >= (17 - u.c[8] * 0.5)) {
	    printf("You partially dodge it.\r\n");
	    d = d * 0.5;
	  }
	}
	if (d == 0) {
	  i1 = 50 + mm[m].m + m_str + m_arm + u.c[7] * 10 +
	    m1 * (mm[m].m / 2.0) - u.c[8] * (5 - u.c[7]);
	  /* chance is 50% + monster, mage+20%, cleric+10%, +1..10% mlevel
	     - 3, 4, 5% per level */
	  if (u.c[38] > 0)
	    i1 -= 10;        /* PROT spell */
	  if (u.c[39] > 0)
	    i1 -= 20;        /* shield */
	  if (u.c[40] > 0)
	    i1 -= 10;        /* pray */
	  if (u.c[5] > 14)   /* dex bonus */
	    i1 -= (2 * (u.c[5] - 14));
	  i1_old = i1;
          i1 = i1 * ((m2_old + m2) / (double)(2 * m2_old)); /* hits taken */
	  dice = roll(1,100);
	  if (u.c[61] == 1)
	    printf("CHANCE: %d%% [%d%% max], DIE: %d\r\n", i1, i1_old, dice);
	  if (dice > i1) {
	    printf("It missed%s.\r\n", (dice <= i1_old) ?
		   " due to its weakened condition" : "");
	    continue;        /* for while (dead == 0) */
	  }
	}
	if (m > 3 && m < 7)  { /* high level undead ? */
	  i2 = 10 * (m - 3);
	  if (u.c[38] > 0)
	    i2 -= 5;         /* prot from evil */
	  if (u.c[40] > 0)
	    i2 -= 5;         /* pray spell */
	  if (roll(1,100) <= i2) {
	    printf("The %s drains an energy level!\r\n", mnam);
	    u.c[9] = utl_exp(u.c[8]) - 1;
	    if (utl_chklvl() == YEP)
	      return(YEP);
            continue;
	  }
	}
	if (d == 0)
	  d = roll(1, mm[m].m) + m1;
	if (m == 19)  /* balrog */
	  if (roll(1,3) < 3) 
	    printf("The %s uses it's sword!\r\n", mnam);
	  else {
	    printf("The %s uses it's whip!\007\r\n", mnam);
	    d *= 1.5;
	  }
	if (u.c[61] == 1)
	  printf("FORCE: %d\r\n", d);
	d -= 1 + roll(1, u.c[24]);
	if (d <= 0)
	  printf("You block with your shield.\r\n");
	else {
	  d -= roll(1, u.c[23]) + 2 - u.c[7];
	  if (d <= 0)
	    printf("Your armor protects you.\r\n");
	  else {
	    printf("It did %d point%s to you.\r\n", d, (d == 1) ? "" : "s");
	    u.c[11] -= d;
	    if (u.c[11] < 1) {
	      printf("You died!\r\n");
	      return(utl_death());
	    }
	  }
	}
      if (m == 15)           /* doppelganger */
	if (roll(1,4) == 3) {
	  printf("The %s looks just like you!\r\n", mnam);
	  if (roll(1,20) < u.c[2] + u.c[8] - m1)
	    printf("You see through its trick.\r\n");
	  else {
	    printf("You're confused!\r\n");
	    ok = 0;
	  }
	}
      if (m == 12)           /* harpie */
	if (roll(1,3) == 1) {
	  printf("The %s charms you with her voice...\r\n", mnam);
	  if (roll(1,20) < u.c[2] + u.c[8] - m1)
	    printf("But you resist her death song!\r\n");
	  else {
	    printf("You're hopelessly in love!\r\n");
	    ok = 0;
	  }
	}
      }
    }
    u.i[6] = 0;
    if (gone == 0) {
      d = (mm[m].m * m1 + m_str * m_arm) * 10 / (double) u.c[8];
      printf("You got %d experience point%s.\r\n", d, (d == 1) ? "" : "s");
      u.c[9] += d;
      sleep(1);
      utl_chklvl();
    }
    utl_pplot(NOPE);
    if (u.c[63] != 0 || aflag != 0) {
      u.c[63] = m1;
      return(NOPE);          /* called from spc_main()? */
    }
    u.c[63] = l;
    if (u.i[7] != 0)
      return(trs_main());
    u.c[64] = 1;
    u.l[u.c[16]][u.c[17]] = 16 * u.i[5] + 4 * u.i[2] + u.i[1];
    if (u.i[5] == 0) {
      u.c[64] = DGN_PROMPT;
      return(NOPE);
    }
    return(spc_main());
  case CBT_CAST:
    if (cbt_cast() == YEP)
      return(YEP);
    break;
  default:
    printf("cbt: unknown cmd\r\n");
    unix_exit(1);
  }
  return(NOPE);
}

int cbt_cast()

{
  int in2, lvl = 0, spl = 0;
  if (u.c[64] == CBT_CAST) {
    hflag = 1;
    u.c[64] = DGN_PROMPT;
  }
  else
    hflag = 0;
  while (spl == 0) {
s_top:
    printf("Level:");
    in2 = getchar();
    if (in2 < 0)
      in2 = '\r';
    if (in2 == '\r') {
      printf("\r\n");
      return(NOPE);
    }
    if (in2 < '1' || in2 > '4') {
      printf("A number for 1 to 4 you jester!\r\n");
      goto s_top;
    }
    lvl = in2 - '0';
    if (u.c[30+lvl] < 1) {
      printf("You have no more level %d spells.\r\n", lvl);
      return(NOPE);
    }
    printf(" Spell # ");
    in2 = getchar();
    if (in2 == '\r')
      return(NOPE);
    if (in2 < '1' || (in2 > '4' && u.c[7] == 1) ||
                     (in2 > '6' && u.c[7] != 1)) 
      utl_prtspl(u.c[7], lvl);
    else
      spl = in2 - '0';
  }
  u.c[30+lvl]--;
  if (u.c[7] != 1) {
    printf("%s\r\n", sp[lvl][spl]);
    return( (*fns[lvl - 1].ms)(spl) );
  } else {
    printf("%s\r\n", sp[lvl+4][spl]);
    return( (*fns[lvl - 1].cs)(spl) );
  }
}


