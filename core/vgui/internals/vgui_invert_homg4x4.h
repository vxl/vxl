#ifndef vgui_invert_homg4x4_h_
#define vgui_invert_homg4x4_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgui_invert_homg4x4
// .INCLUDE vgui/internals/vgui_invert_homg4x4.h
// .FILE internals/vgui_invert_homg4x4.cxx
// @author fsm@robots.ox.ac.uk

//: A, B are 4x4 matrices.
bool vgui_invert_homg4x4(double const * const *A, double **B);
bool vgui_invert_homg4x4(double const A[4][4], double B[4][4]);

#endif // vgui_invert_homg4x4_h_
