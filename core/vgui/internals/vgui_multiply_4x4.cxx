// This is core/vgui/internals/vgui_multiply_4x4.cxx
//:
// \file
// \author fsm

#include "vgui_multiply_4x4.h"

void vgui_multiply_4x4(double const A[4][4], double const B[4][4], double M[4][4])
{
  for (unsigned i=0; i<4; ++i) {
    for (unsigned k=0; k<4; ++k) {
      M[i][k] = 0;
      for (unsigned j=0; j<4; ++j)
        M[i][k] += A[i][j] * B[j][k];
    }
  }
}
