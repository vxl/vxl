// This is oxl/vgui/internals/vgui_un_project.h
#ifndef vgui_un_project_h_
#define vgui_un_project_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
// .NAME vgui_un_project
// .INCLUDE vgui/internals/vgui_un_project.h
// .FILE internals/vgui_un_project.cxx
// @author fsm@robots.ox.ac.uk

// compute the preimage Y of a point X under a 4x4 homography H.
bool vgui_un_project(double const * const *H, double const X[4], double Y[4]);
bool vgui_un_project(double const H[4][4], double const X[4], double Y[4]);
bool vgui_un_project(double const H[16], double const X[4], double Y[4]);

#endif // vgui_un_project_h_
