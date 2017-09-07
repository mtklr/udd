/*
 * udd data.h file
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

struct monst mm[NMONST] = {  /* 21 = NMONST */
/*                       DEPTH  MIN         */
/*  NAME            DIE MAX MIN LVL         */
  { "NULL",           1, 20,  1,  1 },
  { "Skeleton",       4,  1,  9,  1 },
  { "Zombie",         6,  1, 13,  2 },
  { "Ghoul",          8,  1, 20,  2 },
  { "Wraith",        10,  1, 20,  3 }, 
  { "Spectre",       12,  3, 20,  4 },
  { "Vampire",       14,  4, 20,  5 },
  { "Kobold",         3,  1,  8,  1 },
  { "Gnoll",          4,  1, 12,  1 },
  { "Goblin",         3,  1, 10,  1 },
  { "Orc",            5,  1, 20,  1 },
  { "Dwarf",          5,  1, 20,  1 }, 
  { "Harpie",         6,  2, 20,  2 },
  { "Fighter",        6,  1, 20,  1 },
  { "Bugbear",        7,  2, 20,  3 },
  { "Doppleganger",   8,  3, 20,  3 },
  { "Minotaur",      10,  3, 20,  3 },
  { "Ogre",          10,  3, 20,  3 },
  { "Giant",         12,  4, 20,  4 },
  { "Balrog",        16,  7, 20,  7 },
  { "Dragon",        20,  9, 20,  8 }
};


int q[5][3] = {
  {  0,  0,  0 },
  {  0, -1,  0 },
  {  0,  0,  1 },
  {  0,  1,  0 },
  {  0,  0, -1 }
};

char *vaz[5] = { " I :", "       ", "IIIIIII", "II---II", "II...II" };
char *maz[4] = { " ", "I", " ", ":" };

char *st = "STRINTWISCONDEXCHA";

char *spc[16] = { "  \\", "  /", "\\ /", "EXC", "PIT", "TPT",
                    "FNT", "ALT", "DGN", "DGN", "ORB", "ELV",
                    "THR", "SAF", "RCK" };

char *class[] = { "Warrior", "Cleric", "Magician" };
char *class2[] = { "Fighter", "Cleric", "Magician" };

char *arm[] = { "Plate", "Chain", "Leather" };

char *wep[] = { "Sword", "Mace", "Dagger" };

char *sp[9][7] = {
  { "", "", "", "", "", "", "" },
  { "", "Magic missile", "Charm", "Shield", "Sleep", "Protection from Evil",
      "Light" },
  { "", "Phantasmal forces", "Web", "Lightning bolt", "Strength", 
      "Levitate", "Invisibility" },
  { "", "Fireball", "Confuse", "Pass-wall", "Hold monster", "Fear", 
      "Continual light" },
  { "", "Teleport", "Power word kill", "Prismatic wall", "Time stop",
      "Wall of fire", "Summon demon" },

  { "", "Protect from Evil", "Light", "Cure light wounds", "Turn Undead",
      "", "" },
  { "", "Detect traps", "Silence", "Pray", "Hold monster", "", "" },
  { "", "Cure serious wounds", "Dispell undead", "Continual light",
      "Plague", "", "" },
  { "", "Holy word", "Finger of Death", "Blade barrier", "Raise dead",
      "", "" }
};




