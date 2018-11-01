// This is oxl/mvl/mvl_psi.h
#ifndef mvl_psi_h_
#define mvl_psi_h_
//:
// \file
// \author fsm
// You don't need to know what these functions are for.

#define mvl_typename class

template <mvl_typename U>
U mvl_psi_constraint(U const abcde[5]);

template <mvl_typename U>
void mvl_psi_constraint_restrict(U const p[5], U const q[5], U coeffs[4]);

template <mvl_typename U>
void mvl_psi_apply(U const XYZT[4], U abcde[5]);

template <mvl_typename U>
void psi_invert_direct(U const abcde[5], U XYZT[4], int which);

template <mvl_typename U>
void psi_invert_direct(U const abcde[5], U XYZT[4]);

template <mvl_typename U>
void psi_invert_design(U const abcde[5], U XYZT[4]);

template <mvl_typename U>
void mvl_psi_invert(U const abcde[5], U XYZT[4]);

#endif // mvl_psi_h_
