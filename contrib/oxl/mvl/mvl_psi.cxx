// This is oxl/mvl/mvl_psi.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "mvl_psi.h"

#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>

#if 0
# define mvl_psi_temp_name(l, r) U l = r
#else
# define mvl_psi_temp_name(l, r) U const &l = r
#endif

// Evaluate the cubic constraint on \psi(X) = (a,b,c,d,e)
// by computing the determinant of [ e e b ; d c b ; d a a ]
// The image of psi is cut out by the vanishing of this determinant.
template <mvl_typename U>
U mvl_psi_constraint(U const abcde[5])
{
  //     [ e e b ]
  // det [ d c b ]
  //     [ e a a ]
  mvl_psi_temp_name(a, abcde[0]);
  mvl_psi_temp_name(b, abcde[1]);
  mvl_psi_temp_name(c, abcde[2]);
  mvl_psi_temp_name(d, abcde[3]);
  mvl_psi_temp_name(e, abcde[4]);

  return a*b*d - a*b*e + a*c*e - a*d*e - b*c*d + b*d*e;
}

// Compute coefficient vector of the cubic form f(s*p + t*q).
template <mvl_typename U>
void mvl_psi_constraint_restrict(U const p[5],
                                 U const q[5],
                                 U coeffs[4])
{
  // The coefficients of a homogeneous cubic polynomial in
  // two variables can be computed from the values which it
  // takes at the four points (1, 0), (0, 1), (1, 1), (1, -1).
  //
  // [ c0 ]   [  2     0     0     0 ] [ v0 ]
  // [ c1 ] = [  0    -2     1    -1 ] [ v1 ]
  // [ c2 ]   [ -2     0     1     1 ] [ v2 ]
  // [ c3 ]   [  0     2     0     0 ] [ v3 ]

  // evaluate.
  U values[4]; {
    values[0] = mvl_psi_constraint(p);
    values[1] = mvl_psi_constraint(q);
    U tmp[5];
    for (int i=0; i<5; ++i) tmp[i] = p[i] + q[i];
    values[2] = mvl_psi_constraint(tmp);
    for (int i=0; i<5; ++i) tmp[i] = p[i] - q[i];
    values[3] = mvl_psi_constraint(tmp);
  }

  // recover coefficients.
  coeffs[0] =  2 * values[0];
  coeffs[1] =                - 2 * values[1] + values[2] - values[3];
  coeffs[2] = -2 * values[0]                 + values[2] + values[3];
  coeffs[3] =                  2 * values[1];
}

template <mvl_typename U>
void mvl_psi_apply(U const XYZT[4], U abcde[5])
{
  mvl_psi_temp_name(X, XYZT[0]);
  mvl_psi_temp_name(Y, XYZT[1]);
  mvl_psi_temp_name(Z, XYZT[2]);
  mvl_psi_temp_name(W, XYZT[3]);

  abcde[0] = X*Y - X*W;
  abcde[1] = X*Z - X*W;
  abcde[2] = Y*Z - X*W;
  abcde[3] = Y*W - X*W;
  abcde[4] = Z*W - X*W;
}

template <mvl_typename U>
void mvl_psi_invert_direct(U const abcde[5], U XYZT[4], int which)
{
  mvl_psi_temp_name(a, abcde[0]);
  mvl_psi_temp_name(b, abcde[1]);
  mvl_psi_temp_name(c, abcde[2]);
  mvl_psi_temp_name(d, abcde[3]);
  mvl_psi_temp_name(e, abcde[4]);

  switch (which) {
  case 0:
    //% X=1
    //ux=[
    //  1
    //  (c-d)/(b  )
    //  (c-e)/(a  )
    //  (d-e)/(a-b)
    //  ];
    XYZT[0] = 1;
    XYZT[1] = (c-d)/(b  );
    XYZT[2] = (c-e)/(a  );
    XYZT[3] = (d-e)/(a-b);
    break;

  case 1:
    //% Y=1
    //uy=[
    //  (b  )/(c-d)
    //  1
    //  (b-e)/(a-d)
    //  ( -e)/(a-c)
    //  ];
    XYZT[0] = (b  )/(c-d);
    XYZT[1] = 1;
    XYZT[2] = (b-e)/(a-d);
    XYZT[3] = ( -e)/(a-c);
    break;

  case 2:
    //% Z=1
    //uz=[
    //  (a  )/(c-e)
    //  (a-d)/(b-e)
    //  1
    //  ( -d)/(b-c)
    //  ];
    XYZT[0] = (a  )/(c-e);
    XYZT[1] = (a-d)/(b-e);
    XYZT[2] = 1;
    XYZT[3] = ( -d)/(b-c);
    break;

  case 3:
    //% T=1
    //ut=[
    //  (a-b)/(d-e)
    //  (a-c)/( -e)
    //  (b-c)/( -d)
    //  1
    //  ];
    XYZT[0] = (a-b)/(d-e);
    XYZT[1] = (a-c)/( -e);
    XYZT[2] = (b-c)/( -d);
    XYZT[3] = 1;
    break;

  default:
    vcl_abort();
  };
}

template <mvl_typename U>
void mvl_psi_invert_direct(U const abcde[5], U XYZT[4])
{
  mvl_psi_temp_name(a, abcde[0]);
  mvl_psi_temp_name(b, abcde[1]);
  mvl_psi_temp_name(c, abcde[2]);
  mvl_psi_temp_name(d, abcde[3]);
  mvl_psi_temp_name(e, abcde[4]);

  // c - d          b
  // c - e          a
  // d - e          a - b
  // b - e          a - d
  // a - c          e
  // b - c          d
  bool cd_b  = vcl_abs(c - d) <= vcl_abs(b);
  bool ce_a  = vcl_abs(c - e) <= vcl_abs(a);
  bool de_ab = vcl_abs(d - e) <= vcl_abs(a - b);
  bool be_ad = vcl_abs(b - e) <= vcl_abs(a - d);
  bool ac_e  = vcl_abs(a - c) <= vcl_abs(e);
  bool bc_d  = vcl_abs(b - c) <= vcl_abs(d);

  if (false)
    { }

  else if (cd_b && ce_a && de_ab)
    mvl_psi_invert_direct(abcde, XYZT, 0);

  else if (!cd_b && be_ad && !ac_e)
    mvl_psi_invert_direct(abcde, XYZT, 1);

  else if (!ce_a && !be_ad && !bc_d)
    mvl_psi_invert_direct(abcde, XYZT, 2);

  else if (!de_ab && ac_e && bc_d)
    mvl_psi_invert_direct(abcde, XYZT, 3);

  else
    assert(false); // did you get here? probably a round-off problem...
}

#include <vnl/vnl_matrix_ref.h>
#include <vnl/algo/vnl_svd_economy.h>

template <mvl_typename U>
void mvl_psi_invert_design(U const abcde[5], U XYZT[4])
{
  mvl_psi_temp_name(a, abcde[0]);
  mvl_psi_temp_name(b, abcde[1]);
  mvl_psi_temp_name(c, abcde[2]);
  mvl_psi_temp_name(d, abcde[3]);
  mvl_psi_temp_name(e, abcde[4]);

  U design[6*4] = {
    e-d, 0  , 0  , a-b, // X:T
    e-c, 0  , a  , 0  , // X:Z
    d-c, b  , 0  , 0  , // X:Y
    0  , e  , 0  , a-c, // Y:T
    0  , e-b, a-d, 0  , // Y:Z
    0  , 0  , d  , b-c  // Z:T
  };
  // capes_at_robots - much faster than full vnl_svd.
  vnl_svd_economy<U> svd(vnl_matrix_ref<U>(6, 4, design));
  svd.nullvector().copy_out(XYZT);
}

template <mvl_typename U>
void mvl_psi_invert(U const abcde[5], U XYZT[4])
{
#if 1 // capes_at_robots - mvl_psi_invert_direct fails occasionally : I think it is missing a case.
  mvl_psi_invert_design(abcde, XYZT);
#else
  mvl_psi_invert_direct(abcde, XYZT);
#endif
}

//----------------------------------------------------------------------

#define instantiate(U) \
template U mvl_psi_constraint(U const [5]); \
template void mvl_psi_constraint_restrict(U const [5], U const [5], U [4]); \
template void mvl_psi_apply(U const [4], U [5]); \
template void mvl_psi_invert_direct(U const [5], U [4], int); \
template void mvl_psi_invert_direct(U const [5], U [4]); \
template void mvl_psi_invert_design(U const [5], U [4]); \
template void mvl_psi_invert(U const [5], U [4])

instantiate(double);
//instantiate(vcl_complex<double>);
#undef instantiate
