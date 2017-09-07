/*
 * udd defs.h file
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

#ifdef MAIN
#define EXTERN
#else
#define EXTERN extern
#endif

/*
 * C O N F I G U R A T I O N   S E C T I O N
 *
 */

/*
 * define L_STRICT if you want a character to be left in a locked
 * state after a SIGHUP (default=undef).
 */

#undef  L_STRICT 

/*
 * file names: FIL_XXX     - full path of file
 *             FIL_XXX_LK  - if you are using links for locking this is the 
 *                           lock file name, otherwise it is ignored.
 *             FIL_XXX_LD  - if you are using links for locking this is the 
 *                           lock file directory, otherwise it is ignored.
 * files: CHR - Character file
 *        NOT - message of the day
 *        ORB - winners file
 *        INS - instruction file
 *        LVL - dungeon structure file
 */

#define FIL_CHR "./char.dat"
#define FIL_CHR_LK "char.lck"
#define FIL_CHR_LD "."
#define FIL_NOT "./not.txt"
#define FIL_ORB "./orb.txt"
#define FIL_INS "./ins.txt"
#define FIL_LVL "./lvl.dat"
#define FIL_LVL_LK "lvl.lck"
#define FIL_LVL_LD "."

/*
 * E N D   O F   C O N F I G U R A T I O N   S E C T I O N
 *
 */

/* don't edit below this line */

/* generic constants */

#define NMONST 21
#define NAMELEN 64
#define SPLBASE 37
#define DNAM_SZ 20
#define CBT_NORM 1
#define CBT_ALTR 2
#define CBT_CAST 3
#define DGN_NEWADV 0
#define DGN_NEWLOC 1
#define DGN_PROMPT 2
#define DGN_AMOVE 3
#define SPC_NORM 1
#define SPC_TPTNOW 20   /* must be > 15 or < 1  to avoid conflict */
#define SPC_DNS  1
#define SPC_UPS  2
#define SPC_UDS  3
#define SPC_EXC  4
#define SPC_PIT  5
#define SPC_TPT  6
#define SPC_FTN  7
#define SPC_ALT  8
#define SPC_DGN1 9
#define SPC_DGN2 10
#define SPC_ORB  11
#define SPC_ELV  12
#define SPC_THR  13
#define SPC_SAF  14
#define SPC_RCK  15
#define SWB_CREATE 2
#define XXX_NORM 1
#define NOPE 1
#define YEP  2
#define MAYBE 3
#define LOCK 4
#define NOLOCK 5
#define NUKE 6
#define rnd() ((random() % 1000) / 1000.0)  /* basic emulation */

struct monst {
  char *nam;
  int m, mindunlvl, maxdunlvl, minmonlvl;
};

struct state {
  char n[2][NAMELEN];                 /* name, secret name */
  long int c[65];                /* various stuff */
  int l[22][22];                 /* current level map */
  int i[9], i1[9];               /* room analysis */
  int p[5][5];                   /* current pos */
};

struct chr {
  char nam[2][NAMELEN];
  long int c[65];
};

struct dgnstr {
  char dnam[DNAM_SZ];
  short int dstart;
  unsigned char dmap[400];
};


/* pre-defined data structs in data.h */
extern struct monst mm[NMONST];  /* monster matrix */
extern int q[5][3];        /* direction vectors */
extern char *vaz[5];             /* maze parts */
extern char *maz[4];             /* more parts */
extern char *st;                 /* stat names */
extern char *spc[16];            /* specials */
extern char *class[];            /* classes */
extern char *class2[];           /* classes */
extern char *arm[];              /* arm types */
extern char *wep[];              /* weps */
extern char *sp[9][7];           /* spell names */

/* regular game state vars */

EXTERN struct state u;           /* me! */
EXTERN struct dgnstr dd;         /* current dungeon map */
EXTERN char *ddd;                /* our name */
EXTERN int n_dlvl;               /* # of dungeons (for operator prog) */
EXTERN int dfd;                  /* RDONLY fd for lvl file */
EXTERN int autosave;             /* try save if sighup */

/* J. Random functions */

extern int cbt_cast(), cbt_main(), cbt_ohitu(), cbt_ok(), cbt_uhitm();
extern int chr_lck_nuke(), chr_load(), chr_save(), chr_new();
extern void chr_rset();
extern struct chr *chr_scan();
extern int cs1(), cs2(), cs3(), cs3();
extern int dgn_main(), dgn_nomove(), dgn_voices();
extern int dlvl_choose(), dlvl_loadum();
extern void dlvl_init(), dlvl_get_start();
extern int fni(), fni1();
extern char *fnp(), *fnp1();
extern int lock_open(), lock_close();
extern int ms1(), ms2(), ms3(), ms4();
extern int opr_main(), optout(), roll();
extern void sighup(), sigstop(), swb_note();
extern int spc_main(), swb_ppnok(), swb_wiz(), trs_chest(), trs_main(), 
  trs_obj();
extern char *unix_date();
extern void unix_exit(), unix_init(), unix_tty_cook(), unix_tty_dgn(),
  unix_tty_pswd();
extern int utl_adj_ex(), utl_chklvl(), utl_death(), utl_escape(), utl_exp(),
  utl_populate(), utl_winner();
extern void utl_adj_st(), utl_dtrp(), utl_eqp(), utl_inilvl(), utl_pplot(),
  utl_prtspl(), utl_sprog(), utl_stat(), utl_status();

