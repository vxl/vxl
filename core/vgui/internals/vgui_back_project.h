// This is core/vgui/internals/vgui_back_project.h
#ifndef vgui_back_project_h_
#define vgui_back_project_h_
//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  Given a 3x4 camera, project a point x back onto a plane pi.

//: Given a 3x4 camera, project a point x back onto a plane pi.
// The given point x is in normalize device coordinates.
// The computed point X is in object coordinates.
bool vgui_back_project_d3d4d4d4d4_d4(double const x[3],
                                     double const P0[4],
                                     double const P1[4],
                                     double const P2[4],
                                     double const pi[4],
                                     double X[4]);

//: Easy version - gets the projection matrices for you.
bool vgui_back_project_d4d4_d4(double const x[3], double const pi[4], double X[4]);

//: Easy version - backproject from viewport coordinates.
bool vgui_back_project_iid4_d4(int vx, int vy, double const pi[4], double X[4]);

//: Easy version - backproject from viewport coordinates to (x,y,0,1)
bool vgui_back_project_ii_dd(int vx, int vy, double *x, double *y);

#endif // vgui_back_project_h_
