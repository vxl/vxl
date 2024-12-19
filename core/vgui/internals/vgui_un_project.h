// This is core/vgui/internals/vgui_un_project.h
#ifndef vgui_un_project_h_
#define vgui_un_project_h_
//:
// \file
// \author fsm

//: Compute the preimage Y of a point X under a 4x4 homography H.
bool
vgui_un_project(const double * const * H, const double X[4], double Y[4]);
//: Compute the preimage Y of a point X under a 4x4 homography H.
bool
vgui_un_project(const double H[4][4], const double X[4], double Y[4]);
//: Compute the preimage Y of a point X under a 4x4 homography H.
bool
vgui_un_project(const double H[16], const double X[4], double Y[4]);

#endif // vgui_un_project_h_
