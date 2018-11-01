// This is core/vgui/internals/vgui_un_project.h
#ifndef vgui_un_project_h_
#define vgui_un_project_h_
//:
// \file
// \author fsm

//: Compute the preimage Y of a point X under a 4x4 homography H.
bool vgui_un_project(double const * const *H, double const X[4], double Y[4]);
//: Compute the preimage Y of a point X under a 4x4 homography H.
bool vgui_un_project(double const H[4][4], double const X[4], double Y[4]);
//: Compute the preimage Y of a point X under a 4x4 homography H.
bool vgui_un_project(double const H[16], double const X[4], double Y[4]);

#endif // vgui_un_project_h_
