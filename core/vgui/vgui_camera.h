#ifndef vgui_camera_h_
#define vgui_camera_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME    vgui_camera
// .LIBRARY vgui
// .HEADER  vxl package
// .INCLUDE vgui/vgui_camera.h
// .FILE    vgui_camera.cxx
//
// .SECTION Description
//    vgui_camera is a utility class which allows 3D models to projected into
//     a GL context given a known camera projection matrix.  Note comments in
//     code about clipping planes which is really rather important if you care
//     about such things.  Use the class in conjunction with a vgui_load (if
//     you dare), or a vgui_mult to change the GL_PROJECTION_MATRIX
//     appropriately.
//
// .SECTION Author
//     Geoffrey Cross, Oxford RRG, 03 Nov 99
//
// .SECTION Modifications
//     991103 Geoff Initial version.
//
//-----------------------------------------------------------------------------

class PMatrix;

class vgui_camera {
public:
  vgui_camera();
  vgui_camera( const vnl_matrix<double> &P);

  void set_pmatrix( const vnl_matrix<double> &m);

  // Plug this matrix into a vgui_load or vgui_mult tableau.
  vnl_matrix<double> get_glprojmatrix( const int imagesizex= 720, const int imagesizey= 576) const;

protected:
  vnl_matrix<double> pmatrix;
};

#endif // vgui_camera_h_
