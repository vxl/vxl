// This is core/vgui/vgui_camera.h
#ifndef vgui_camera_h_
#define vgui_camera_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Geoffrey Cross, Oxford RRG
// \date   03 Nov 99
// \brief Projects 3D models into a GL context given a camera projection matrix.
//
// \verbatim
//  Modifications
//   991103 Geoff Initial version.
//   26-APR-2002  K.Y.McGaul - Converted to doxygen style comments.
// \endverbatim

#include <vnl/vnl_matrix_fixed.h>

//: Projects 3D models into a GL context given a camera projection matrix.
//
//  vgui_camera is a utility class which allows 3D models to projected into
//  a GL context given a known camera projection matrix.  Note comments in
//  code about clipping planes which is really rather important if you care
//  about such things.  Use the class in conjunction with a vgui_load (if
//  you dare), or a vgui_mult to change the GL_PROJECTION_MATRIX
//  appropriately.
class vgui_camera
{
 public:
  //: Constructor - create a camera with a default projection matrix.
  vgui_camera() {}

  //: Constructor - create a camera with the given projection matrix.
  vgui_camera(vnl_matrix_fixed<double,3,4> const& P) : pmatrix(P) {}

  //: Set the projection matrix to the given matrix.
  void set_pmatrix(vnl_matrix_fixed<double,3,4> const& m) { pmatrix= m; }

  //: Plug this matrix into a vgui_loader_tableau.
  //  Note: this will return a GL_PROJECTION_MATRIX with the assumption that
  //  you have an euclidean reconstruction.  The result is that the front and
  //  back clipping planes will be PARALLEL (note: not projective frame!) to
  //  the image plane.
  vnl_matrix_fixed<double,3,4> get_glprojmatrix( const int imagesizex= 720,
                                                 const int imagesizey= 576) const;

 protected:
  //: The projection matrix.
  vnl_matrix_fixed<double,3,4> pmatrix;
};

#endif // vgui_camera_h_
