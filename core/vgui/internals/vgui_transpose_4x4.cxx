// This is oxl/vgui/internals/vgui_transpose_4x4.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vgui_transpose_4x4.h"

void vgui_transpose_4x4(double A[4][4])
{
  double tmp;
#define swup(x, y) { tmp=x; x=y; y=tmp; }
  /* */
  swup(A[1][0], A[0][1]);
  swup(A[2][0], A[0][2]); swup(A[2][1], A[1][2]);
  swup(A[3][0], A[0][3]); swup(A[3][1], A[1][3]); swup(A[3][2], A[2][3]);
#undef swup
}
