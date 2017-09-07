/*
 * udd spc.c file - special location program
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#include "defs.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>

/*char *color[] = { "white", "green", "blue", "red", "black" };*/
char *color[] = { "white", "blue", "green", "red", "black" };

int spc_main()

{
  int sav, sav2, tmp, tmp2;
  double dtmp;
  char tmpbuf[NAMELEN];
  int spc = u.i[5];
  if (u.c[64] == SPC_TPTNOW)
    spc = SPC_TPTNOW;
  switch (spc) {
  case SPC_UPS:
  case SPC_DNS:
  case SPC_UDS:
    printf("You have found a circular stairway...\r\n");
  s_top:
    printf("Do you want to %s%sor stay on the (S)ame level: ",
	   (spc != 1) ? "go (U)p, " : "",
	   (spc != 2) ? "go (D)own, " : "");
    tmp2 = getchar();
    if (tmp2 < 0)
      tmp2 = 'S';
    if (tmp2 == 'S' || tmp2 == 's') {
      printf("Same\r\n");
      break;
    }
    if (spc != 1 && (tmp2 == 'u' || tmp2 == 'U')) {
      printf("Up\r\n");
      u.c[15]--;
      if (u.c[15] < 1)
	return(utl_escape());
      utl_inilvl();
      u.c[64] = DGN_NEWLOC;
      return(NOPE);
    }
    if (spc != 2 && (tmp2 == 'd' || tmp2 == 'D')) {
      printf("Down\r\n");
      if (u.c[15] >= 20) {
	printf("You find the way is blocked by an iron gate...\r\n");
	break;
      }
      u.c[15]++;
      utl_inilvl();
      u.c[64] = DGN_NEWLOC;
      return(NOPE);
    }
    printf("\r\nDo you have a hearing problem\007?\r\n");
    goto s_top;
  case SPC_EXC:
    printf("You have found the Excelsior Transporter...\r\n");
    if (u.c[50] == 1) {
      printf("But the controls just went dead!\r\n");
      break;
    }
    if (u.c[47] > 0) {
      printf("But the controls seem frozen!\r\n");
      break;
    }
    exc_top:
    unix_tty_cook();
    printf("What level do you want to go to?\r\n");
    printf("(1 to 20, or 0 to stay here)? ");
    if (!fgets(tmpbuf, NAMELEN, stdin))
      strcpy(tmpbuf, "0\n");
    tmp = sscanf(tmpbuf, "%d", &tmp2);
    fflush(stdin);
    unix_tty_dgn();
    if (tmp == 1 && tmp2 == 0)
      break;
    if (tmp != 1 || (tmp == 1 && (tmp2 < 1 || tmp2 > 20))) {
      printf("Try again bird brain\007!\r\n");
      goto exc_top;
    }
    tmp = (abs(u.c[15] - tmp2) + 1) * ((tmp2 * u.c[15])/2.0) * 25;
    if (tmp > u.c[12]) {
      printf("You need %d gold for that trip.\r\n", tmp);
      break;
    }
    u.c[12] -= tmp;
    printf("It cost %d gold, you have %d left.\r\n", tmp, u.c[12]);
    sav = u.c[15];
    u.c[15] = tmp2;
    utl_inilvl();
    u.c[16] = -1;
    for (tmp = 1 ; tmp <= 20; tmp++)
      for (tmp2 = 1 ; tmp2 <= 20 ; tmp2++)
	if ((u.l[tmp][tmp2] & 240) == 64) {
	  u.c[16] = tmp;
	  u.c[17] = tmp2;
	}
    if (u.c[16] < 0) {
      u.c[16] = roll(1,20);
      u.c[17] = roll(1,20);
    }
    u.c[63] = fni(u.l[tmp][tmp2]);
    if (roll(3,7) > (u.c[2] + u.c[3]) / 2) {
      printf("The transporter malfunctioned!\r\n");
      if (cbt_ohitu((abs(sav - u.c[15])+0.5*(u.c[15] +sav)) *
		     (rnd() * rnd() * rnd() / 2.0)) == YEP)
	return(YEP);
    }
    u.c[64] = DGN_NEWLOC;
    return(NOPE);
  case SPC_PIT:
    tmp = NOPE;
    if (u.c[43] > 0)
      printf("You are hovering over a pit...\r\n");
    else {
      if (roll(3,6) + 3 > u.c[5] && 
	  (sqrt((double)u.c[52] /25.0) * 25.0) < roll(1,20)) {
	printf("You fell in a pit!\r\n");
	tmp = YEP;
      } else
      printf("You are on the brink of a pit...\r\n");
    }
    if (u.c[15] == 20 && tmp == NOPE) {
      printf("It's bottomless, watch out!\r\n");
      break;
    }
    if (tmp == NOPE) {
    pit_top:
      printf("Press <CR> to climb down, <LF> to walk around: ");
      tmp2 = getchar();
      printf("\r\n");
      if (tmp2 < 0)
	tmp2 = 10;
      if (tmp2 == '\n')
	break;
      if (tmp2 != '\r') {
	printf("Come on\007!\r\n");
	goto pit_top;
      }
    }
    if (u.c[15] == 20) {
      printf("It's bottomless\007!!!  IIIEEEEEEEEEEEEE!!!!!!... .  .   .\r\n");
      return(utl_death());
    }
    if (tmp == YEP || (u.c[43] < 1 && roll(3,6)+1 >= u.c[5])) {
      if (tmp != YEP)
	printf("You slipped down the side!\r\n");
      if (cbt_ohitu(u.c[15]) == YEP)
	return(YEP);
    } else
      printf("You made it!\r\n");
    u.c[15]++;
    utl_inilvl();
    u.c[64] = DGN_NEWLOC;
    return(NOPE);
  case SPC_TPT:
  case SPC_TPTNOW:
    if (spc == SPC_TPT)
      printf("Z\007ZAP!  You've been teleported!\r\n");
    sleep(2);
  tpt_top:
    tmp = u.c[16];
    tmp2 = u.c[17];
    if (((tmp + tmp2) & 1) == 0)
      u.c[15]--;
    else
      u.c[15]++;
    if (u.c[15] < 1)
      u.c[15] = 1;
    else if (u.c[15] > 20)
      u.c[15] = 20;
    tmp = tmp + u.c[15] * 7 + tmp2 * 13;
    tmp2 = tmp2 + u.c[15] * 6 + tmp * 17;
    while (tmp > 20)
      tmp -= 20;
    while (tmp2 > 20)
      tmp2 -= 20;
    u.c[16] = tmp;
    u.c[17] = tmp2;
    if (rnd() > 0.8)
      goto tpt_top;
    utl_inilvl();
    u.c[64] = DGN_NEWLOC;
    return(NOPE);
  case SPC_FTN:
    tmp = roll(1,5);
    printf("You see a Fountain with %s water...\r\n", color[tmp - 1]);
  ftn_top:
    printf("Press <CR> to take a drink, <LF> to walk by: ");
    tmp2 = getchar();
    printf("\r\n");
    if (tmp2 < 0)
      tmp2 = 10;
    if (tmp2 == '\n') 
      break;
    if (tmp2 != '\r') {
      printf("Sluggard!  Follow directions.\007\r\n");
      goto ftn_top;
    }
    dtmp = rnd();
    if (dtmp  >= 0.4 + (0.1 * tmp)) {
      printf("It tastes good!\r\n");
      u.c[11] += roll(1, 3 * u.c[15]);
      if (u.c[11] > u.c[10])
	u.c[11] = u.c[10];
      printf("You now have %d hit points.\r\n", u.c[11]);
      break;
    }
    if (dtmp <= 0.1 * tmp) {
      printf("Poison!  Gurgle..  ..  ..   .\r\n");
      if (cbt_ohitu(u.c[15]) == YEP)
	return(YEP);
      printf("You only have %d hit point%s.\r\n", u.c[11], 
	     (u.c[11] == 1) ? "" : "s");
      break;
    }
    if (dtmp > 0.6) {
      printf("It's just water.\r\n");
      break;
    }
    if (rnd() > 0.5) {
      if (utl_adj_ex() == YEP) 
	return(YEP);
    } else 
      utl_adj_st();
    break;
  case SPC_ALT:
  alt_top1:
    printf("You have found a Holy Altar...\r\n");
    tmp = roll(1,20);        /* desc ok if = 1 */
  alt_top2:
    printf("Do you wish to (W)orship%s or (I)gnore it? ",
	   (tmp == 1) ? ", (D)esicrate," : "");
    sav2 = getchar();
    if (sav2 < 0)
      sav2 = 'I';
    if (islower(sav2))
      sav2 = toupper(sav2);
    if (sav2 == 'D' && tmp == 1) {
      printf("Desecrate\r\n");
      printf("You do vile and unspeakable things to the altar.\r\n");
      if (roll(1,4) != 2) {
	printf("Nothing happens here.\r\n");
	if (roll(1,10) == 7)
	  printf("{Hope that made you feel better.}\r\n");
	break;
      }
      if (roll(1,20) <= 3) {
	printf("A Voice booms out \"I shall be avenged.\"\r\n");
	u.c[63] = spc;
	u.c[64] = CBT_NORM;
	if (cbt_main() == YEP)
	  return(YEP);
	goto alt_top1;
      }
      printf("The sound of thunder shatters the air.\r\n");
      printf("The altar crumbles to dust before your eyes.\r\n");
      u.i[5] = 0;
      if (roll(1,10) < 3)
	u.i[5] = SPC_PIT;
      u.l[u.c[16]][u.c[17]] = 16*u.i[5] + 4*u.i[2] + u.i[1];
      if (roll(1,4) != 4) {
	u.c[64] = DGN_NEWLOC;
	return(NOPE);
      }
      printf("Something seems to be left behind...\r\n");
      u.c[63] = u.c[15] + 10;
      u.i[7] = 1;
      if (roll(1,10) == 3)
	u.i[8] = 1;
      return(trs_main());
    }
    if (sav2 == 'W') {
      printf("Worship\r\n");
      printf("Press <CR> to give money, <LF> to just pray: ");
      tmp = getchar();
      if (tmp < 0 || (tmp == '\n' && rnd() > 0.4)) {
	printf("\r\n");
	break;
      }
      if (tmp == '\n')
	sav2 = 'I';
      if (tmp == '\r') {
	printf("\r\n");
      alt_top3:
	printf("How much of your %d gold ? ", u.c[12]);
	unix_tty_cook();
        if (!fgets(tmpbuf, NAMELEN, stdin))
          strcpy(tmpbuf, "\n");
        sav = sscanf(tmpbuf, "%d", &tmp2);
	unix_tty_dgn();
	fflush(stdin);
	if (sav != 1 || tmp2 < 0 || tmp2 < 50 || tmp2 < 0.1 * u.c[12]) {
	  printf("How dare you insult us, you ");
          goto trash;
	} else {
	  if (tmp2 > u.c[12]) {
	    printf("You don't have that much!\r\n");
	    goto alt_top3;
	  }
	  u.c[12] -= tmp2;
	  if (rnd() > 0.9) {
	    if (utl_adj_ex() == YEP)
	      return(YEP);
	    break;
	  }
	  if (rnd() > 0.9) {
	    utl_adj_st();
	    break;
	  }
	  if (rnd() > 0.5) {
	    printf("Thank you for your donation.\r\n");
	    break;
	  }
	  tmp = rnd() * rnd() * 11.0 + 1;
	  tmp2 = rnd() * (tmp2 / ((double)u.c[12] + 1.0)) * 20.0 + 
	    rnd() * 20.0 + 1;
	  if (u.c[36+tmp] < 0)
	    u.c[36+tmp] = tmp2;
	  else
	    u.c[36+tmp] += tmp2;
	  printf("You've been heard.\r\n");
	  break;
	}
      } /* tmp == '\r' (worship) */
    } /* sav2 == 'W' */
    if (sav2 == 'I') {
      printf("Ignore\r\n");
      if (rnd() > 0.7)
	break;
trash:
      printf("Dirty Pagan Trash!\r\n");
      u.c[64] = CBT_ALTR;
      u.c[63] = 0;
      if (cbt_main() == YEP)
	return(YEP);
      goto alt_top1;
    }
    printf("\r\nIgnorant pagan\007!\r\n");
    goto alt_top2;
  case SPC_DGN1:
  case SPC_DGN2:
    sav = u.c[15];
    tmp = u.c[16];
    tmp2 = u.c[17];
    sav2 = u.i[5] = 0;
    u.i[7] = 1;
    u.c[63] = spc;
    u.c[64] = CBT_NORM;
    printf("You have encountered a Dragon in it's lair\007!\r\n");
    if (cbt_main() == YEP)
      return(YEP);
    if (u.c[15] == sav && u.c[16] == tmp && u.c[17] == tmp2) {
      if (spc == SPC_DGN2)
	sav2 = SPC_ORB;
    } else {
      u.i[7] = 0;
    }
    if (u.i[7] != 0) 
      if (trs_main() == YEP)
	return(YEP);         /* this is safe because u.i[5] == 0 */
    u.i[5] = sav2;
    u.l[tmp][tmp2] = 16*u.i[5] + 4*u.i[2] + u.i[1];
    if (u.i[5] != SPC_ORB)
      break;
    utl_pplot();
  case SPC_ORB:
    u.i[5] = 0;
    u.l[u.c[16]][u.c[17]] = 4 * u.i[2] + u.i[1];
    printf("You have found the \007ORB!!!!\r\n");
  orb_top:
    printf("Press <CR> to pick it up, <LF> to leave it: ");
    tmp2 = getchar();
    if (tmp2 < 0)
      tmp2 = 10;
    printf("\r\n");
    if (tmp2 == '\r') {
      printf("You've got it!\r\n");
      u.c[50] = 1;
      break;
    }
    if (tmp2 != '\n') {
      printf("?What?\007\r\n");
      goto orb_top;
    }
    printf("Lost your chance, Mister!\r\n");
    break;
  case SPC_ELV:
    printf("You feel heavy for a moment, but the sensation disappears...\r\n");
    if (u.c[15] == 1)
      return(utl_escape());
    u.c[15]--;
    utl_inilvl();
    u.c[64] = DGN_NEWLOC;
    return(NOPE);
  case SPC_THR:
    printf("You see a massive throne covered with jewels and\r\n");
    printf("mysterious runes....\r\n");
  thr_top:
    printf("Do you want to (S)it on it, (P)ry out some jewels,\r\n");
    printf("(R)ead the runes, or totally (I)gnore it: ");
    tmp2 = getchar();
    if (tmp2 < 0)
      tmp2 = 'I';
    if (islower(tmp2))
      tmp2 = toupper(tmp2);
    if (tmp2 == 'I') {
      printf("Ignore\r\n");
      break;
    }
    if (tmp2 == 'S') {
      printf("Sit down\r\n");
      sleep(1);
      if (rnd() < 0.05) {
	u.c[9] = utl_exp(u.c[8]+1);
	printf("A loud gong sounds.\r\n");
	utl_chklvl();
	break;
      }
      if (rnd() < 0.9) {
	printf("Nothing happens...\r\n");
	break;
      }
      if (rnd() <= 0.1) {
	u.c[64] = SPC_TPTNOW;
	printf("Z\007ZAP!  You've been teleported!\r\n");
	return(spc_main());
      }
      printf("The Dwarven King returns....\r\n");
      u.c[63] = spc;
      u.c[64] = CBT_NORM;
      if (cbt_main() == YEP)
	return(YEP);
      break;
    }
    if (tmp2 == 'P') {
      printf("Pry\r\n");
      sleep(1);
      if (rnd() < 0.1) {
	printf("The Dwarven King returns....\r\n");
	u.c[63] = spc;
	u.c[64] = CBT_NORM;
	if (cbt_main() == YEP)
	  return(YEP);
	break;
      }
      if (rnd() > 0.3) {
	printf("They won't come off!\r\n");
	break;
      }
      printf("They pop into your greedy hands!\r\n");
      tmp = 6000.0 * rnd() * rnd() * u.c[15] + 500.0;
      printf("They are worth %d gold!\r\n", tmp);
      u.c[12] += tmp;
      dtmp = u.c[15] / (double) u.c[8];
      if (dtmp > 1.0)
	dtmp = 1.0;
      u.c[21] += tmp * dtmp;
      break;
    }
    if (tmp2 != 'R') {
      printf("\r\nREAD THE DIRECTIONS, STUPID\007!\r\n");
      goto thr_top;
    }
    printf("Read\r\n");
    if (rnd() > 0.1) {
      printf("You don't understand them.\r\n");
      break;
    }
    if (rnd() > 0.5) {
      printf("The Dwarven King returns...\r\n");
      u.c[63] = spc;
      u.c[64] = CBT_NORM;
      if (cbt_main() == YEP)
	return(YEP);
      break;
    }
    printf("The letters blur before your eyes.\r\n");
    printf("You feel giddy as power surges in you.\r\n");
    sleep(2);
    printf("You seem to have changed.\r\n");
    tmp =  31 + rnd() * rnd() * 4.0;
    u.c[tmp]++;
    if (rnd() > 0.666)
      u.c[tmp-6] += 1;
    break;
  case SPC_SAF:
    if (u.c[58] == 0)
      u.c[58] = roll(1,4) + 10 * roll(1,4);
    if(u.c[61] == 1) {
      printf("[Combo = %d]\r\n", u.c[58]);
    }
  saf_top1:
    printf("You see a small door with four colored lights in a row.\r\n");
    printf("They are red, green, blue, and orange...\r\n");
  saf_top2:
    printf("Press <CR> to try a combination, <LF> to avoid it: ");
    tmp2 = getchar();
    if (tmp2 < 0)
      tmp2 = 10;
    printf("\r\n");
    if (tmp2 == '\n')
      break;
    if (tmp2 != '\r') {
      printf("Trying to be cute\007?\r\n");
      goto saf_top2;
    }
    tmp = 0;
    sav = 0;
    printf("Press two colors: ");
    while (tmp < 2 && tmp != -1) {
      tmp2 = getchar();
      if (islower(tmp2))
	tmp2 = toupper(tmp2);
      switch(tmp2) {
      case 'R':
	printf("Red ");
	sav += 1;
	break;
      case 'G':
	printf("Green ");
	sav += 2;
	break;
      case 'B':
	printf("Blue ");
	sav += 3;
	break;
      case 'O':
	printf("Orange ");
	sav += 4;
	break;
      default:
	printf("\r\nThat's not a legal color\007!\r\n");
	printf("Try red, green, blue, or orange.\r\n");
	tmp = -2;
      }
      tmp++;
      sav *= 10;
    }
    if (tmp < 0)
      goto saf_top2;
    if (sav == u.c[58] * 10) {
      printf("\r\nYou got it\007!\r\n");
      u.c[58] = 0;
      tmp = 5000 * rnd() * u.c[15] + 1000;
      printf("%d worth in gems and jewels pour out\007!\r\n", tmp);
      u.c[12] += tmp;
      dtmp = u.c[15] / (double) u.c[8];
      if (dtmp > 1.0)
	dtmp = 1.0;
      u.c[21] += tmp * dtmp;
      u.c[64] = DGN_NEWLOC;
      return(NOPE);
    }
    printf("\r\nAn electric shock jolts your body\007!\r\n");
    if (cbt_ohitu(u.c[15] * 2) == YEP)
      return(YEP);
    goto saf_top1;
  case SPC_RCK:
    printf("You're in solid rock!!!!!!!!\r\n");
    printf("You die of course...\r\n");
    sleep(2);
    return(utl_death());
  default:
    printf("spc: unknown special %d!\r\n", spc);
    unix_exit(1);
  }
  u.c[64] = DGN_PROMPT;
  return(NOPE);
}
