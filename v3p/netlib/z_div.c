#include "f2c.h"

/* Modified by Peter Vanroose, June 2001, to allow c being equal to a or b */

#ifdef KR_headers
extern VOID sig_die();
VOID z_div(c, a, b) doublecomplex *a, *b, *c;
#else
extern void sig_die(char*, int);
void z_div(doublecomplex *c, doublecomplex *a, doublecomplex *b)
#endif
{
  double ratio, den;
  double abr, abi;
  double ar = a->r, ai = a->i;

  if( (abr = b->r) < 0.)
    abr = - abr;
  if( (abi = b->i) < 0.)
    abi = - abi;
  if( abr <= abi ) {
    if(abi == 0) {
      sig_die("complex division by zero", 1);
    }
    ratio = (double)b->r / b->i ;
    den = b->i * (1 + ratio*ratio);
    c->r = (ar*ratio + ai) / den;
    c->i = (ai*ratio - ar) / den;
  }

  else {
    ratio = (double)b->i / b->r ;
    den = b->r * (1 + ratio*ratio);
    c->r = (ar + ai*ratio) / den;
    c->i = (ai - ar*ratio) / den;
  }
}
