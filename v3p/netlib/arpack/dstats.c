/* arpack/dstats.f -- translated by f2c (version 20090411).
   You must link the resulting object file with libf2c:
        on Microsoft Windows system, link with libf2c.lib;
        on Linux or Unix systems, link with .../path/to/libf2c.a -lm
        or, if you install libf2c.a in a standard place, with -lf2c -lm
        -- in that order, at the end of the command line, as in
                cc *.o -lf2c -lm
        Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

                http://www.netlib.org/f2c/libf2c.zip
*/

#ifdef __cplusplus
extern "C" {
#endif
#include "v3p_netlib.h"

/* Common Block Declarations */

Extern struct {
    integer nopx, nbx, nrorth, nitref, nrstrt;
    real tsaupd, tsaup2, tsaitr, tseigt, tsgets, tsapps, tsconv, tnaupd,
            tnaup2, tnaitr, tneigh, tngets, tnapps, tnconv, tcaupd, tcaup2,
            tcaitr, tceigh, tcgets, tcapps, tcconv, tmvopx, tmvbx, tgetv0,
            titref, trvec;
} timing_;

#define timing_1 timing_


/* \SCCS Information: @(#) */
/* FILE: stats.F   SID: 2.1   DATE OF SID: 4/19/96   RELEASE: 2 */
/*     %---------------------------------------------% */
/*     | Initialize statistic and timing information | */
/*     | for symmetric Arnoldi code.                 | */
/*     %---------------------------------------------% */
/*<       subroutine dstats >*/
/* Subroutine */ int dstats_()
{
/*     %--------------------------------% */
/*     | See stat.doc for documentation | */
/*     %--------------------------------% */
/*<       include   'stat.h' >*/
/*<       nopx   = 0 >*/
/*     %-----------------------% */
/*     | Executable Statements | */
/*     %-----------------------% */
/*     %--------------------------------% */
/*     | See stat.doc for documentation | */
/*     %--------------------------------% */

/* \SCCS Information: @(#) */
/* FILE: stat.h   SID: 2.2   DATE OF SID: 11/16/95   RELEASE: 2 */

/*<       save       t0, t1, t2, t3, t4, t5 >*/

/*<       integer    nopx, nbx, nrorth, nitref, nrstrt >*/
/*<        >*/
/*<        >*/
    timing_1.nopx = 0;
/*<       nbx    = 0 >*/
    timing_1.nbx = 0;
/*<       nrorth = 0 >*/
    timing_1.nrorth = 0;
/*<       nitref = 0 >*/
    timing_1.nitref = 0;
/*<       nrstrt = 0 >*/
    timing_1.nrstrt = 0;
/*<       tsaupd = 0.0D+0 >*/
    timing_1.tsaupd = (float)0.;
/*<       tsaup2 = 0.0D+0 >*/
    timing_1.tsaup2 = (float)0.;
/*<       tsaitr = 0.0D+0 >*/
    timing_1.tsaitr = (float)0.;
/*<       tseigt = 0.0D+0 >*/
    timing_1.tseigt = (float)0.;
/*<       tsgets = 0.0D+0 >*/
    timing_1.tsgets = (float)0.;
/*<       tsapps = 0.0D+0 >*/
    timing_1.tsapps = (float)0.;
/*<       tsconv = 0.0D+0 >*/
    timing_1.tsconv = (float)0.;
/*<       titref = 0.0D+0 >*/
    timing_1.titref = (float)0.;
/*<       tgetv0 = 0.0D+0 >*/
    timing_1.tgetv0 = (float)0.;
/*<       trvec  = 0.0D+0 >*/
    timing_1.trvec = (float)0.;
/*     %----------------------------------------------------% */
/*     | User time including reverse communication overhead | */
/*     %----------------------------------------------------% */
/*<       tmvopx = 0.0D+0 >*/
    timing_1.tmvopx = (float)0.;
/*<       tmvbx  = 0.0D+0 >*/
    timing_1.tmvbx = (float)0.;
/*<       return >*/
    return 0;

/*     End of dstats */

/*<       end >*/
} /* dstats_ */

#ifdef __cplusplus
        }
#endif
