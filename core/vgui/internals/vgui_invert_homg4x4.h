// This is core/vgui/internals/vgui_invert_homg4x4.h
#ifndef vgui_invert_homg4x4_h_
#define vgui_invert_homg4x4_h_
//:
// \file
// \author fsm

//: A, B are 4x4 matrices.
bool vgui_invert_homg4x4(double const * const *A, double **B);

//: A, B are 4x4 matrices.
bool vgui_invert_homg4x4(double const A[4][4], double B[4][4]);

#endif // vgui_invert_homg4x4_h_
