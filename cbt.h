/*
 * udd cbt.h file
 *
 * 03-Sep-89, Chuck Cranor
 *
 * V5.1C (see VERSION file for history)
 *
 */

/* ONLY useful to CBT routines... */
/* should not be used by external stuff */

#ifdef CBTMAIN
#define CBT_EXTERN 
#else
#define CBT_EXTERN extern
#endif

struct slev {
int (*ms)();
int (*cs)();
};

CBT_EXTERN int l, m, m1, m_str, m_arm, m2, m2_old;
CBT_EXTERN char *mnam;
CBT_EXTERN int d, dead, dice, i1, i1_old, i2, in;
CBT_EXTERN int autoevade, gone, hflag, mskip;
CBT_EXTERN int ms1(), ms2(), ms3(), ms4(), cs1(), cs2(), cs3(), cs4();
CBT_EXTERN struct slev fns[4];

