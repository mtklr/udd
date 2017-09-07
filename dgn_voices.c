/*
 * udd dgn_voices.c file - random voices
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#define NVOCS 12
#define ADDDIR 9

char *vocs[] = {
  "You hear a mysterious sound from behind....." ,
  "A voice says 'PLUGH'....." ,
  "Beware the Gulf of Nazguk!!!!   A voice screams in the distance." ,
  "You hear footsteps and breathing behind you...." ,
  "A cold wind suddenly springs up and dies...." ,
  "\"TURN BACK!!!!\"  A voice screams, \"For you too will die ahead!!!...\"",
  "Suddenly, there is silence........." ,
  "A mutilated body lies on the floor nearby." ,
  "The room vibrates as if an army is passing by." ,
  "A resonant voice says \"May I take your hat and goat Sir?\"." ,
  "You hear slurping noises to the " ,
  "You hear rustling noises from the " 
};

char *dirs[] = { "north.", "east.", "south.", "west." };

void dgn_voices() 

{
  int i;
  if ((random() % 101) > 5)
    return;
  i = random() % NVOCS;
  printf("%s", vocs[i]);
  if (i > ADDDIR)
    printf("%s", dirs[ random() % 4 ]);
  printf("\r\n");
}
