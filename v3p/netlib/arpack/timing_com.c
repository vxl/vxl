#include "f2c.h"

#ifdef __cplusplus
extern "C" {
#endif

struct {
    integer nopx, nbx, nrorth, nitref, nrstrt;
    real tsaupd, tsaup2, tsaitr, tseigt, tsgets, tsapps, tsconv, tnaupd,
            tnaup2, tnaitr, tneigh, tngets, tnapps, tnconv, tcaupd, tcaup2,
            tcaitr, tceigh, tcgets, tcapps, tcconv, tmvopx, tmvbx, tgetv0,
            titref, trvec;
} timing_;

#ifdef __cplusplus
}
#endif
