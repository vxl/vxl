// This is core/vgui/internals/vgui_draw_line.h
#ifndef vgui_draw_line_h_
#define vgui_draw_line_h_
//:
// \file
// \author fsm
// \brief  Draw infinite lines.

//------------------------------------------------------------------------------

//: draw infinite line spanned in space by two points.
// From total projection matrix $T = P \cdot M$.
bool vgui_draw_line(double const T[4][4], double const X[4], double const Y[4]);

//: draw infinite line spanned in space by two points.
// From projection and modelview matrices, P and M.
bool vgui_draw_line(double const P[4][4], double const M[4][4], double const X[4], double const Y[4]);

//: draw infinite line spanned in space by two points (easy)
bool vgui_draw_line(double const X[4], double const Y[4]);

//------------------------------------------------------------------------------

//: draw infinite line $ax+by+cw=0, z=0$ from total projection matrix $T=P \cdot M$.
bool vgui_draw_line(double const T[4][4], double a, double b, double c);

//: draw infinite line $ax+by+cw=0, z=0$ from projection and modelview matrices, P and M.
bool vgui_draw_line(double const P[4][4], double const M[4][4], double a, double b, double c);

//: draw infinite line $ax+by+cw=0, z=0$ (easy)
bool vgui_draw_line(double a, double b, double c);

#endif // vgui_draw_line_h_
