// This is core/vgui/internals/vgui_multiply_4x4.h
#ifndef vgui_multiply_4x4_h_
#define vgui_multiply_4x4_h_
// .NAME vgui_multiply_4x4
// .INCLUDE vgui/internals/vgui_multiply_4x4.h
// .FILE internals/vgui_multiply_4x4.cxx
// @author fsm

// this will fail if M overlaps with A or B.
void vgui_multiply_4x4(double const A[4][4], double const B[4][4], double M[4][4]);

#endif // vgui_multiply_4x4_h_
