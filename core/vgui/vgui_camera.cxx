// This is core/vgui/vgui_camera.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Geoffrey Cross, Oxford RRG
// \date   03 Nov 99
// \brief  See vgui_camera.h for a description of this file.

#include "vgui_camera.h"
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

//----------------------------------------------------------------------------
//: Plug this matrix into a vgui_loader_tableau.
//  Note: this will return a GL_PROJECTION_MATRIX with the assumption that
//  you have an euclidean reconstruction.  The result is that the front and
//  back clipping planes will be PARALLEL (note: not projective frame!) to
//  the image plane.
vnl_matrix_fixed<double,3,4> vgui_camera::get_glprojmatrix(int imagesizex,
                                                           int imagesizey) const
{
  vnl_matrix_fixed<double,3,4> C;

  // undo the viewport transformation.
  C(0,0)= 2.0/imagesizex; C(0,1)= 0;               C(0,2)= 0; C(0,3)= -1;
  C(1,0)= 0;              C(1,1)= -2.0/imagesizey; C(1,2)= 0; C(1,3)= 1;
  C(2,0)= 0;              C(2,1)= 0;               C(2,2)= 2; C(2,3)= -1;

  // the projection matrix sets the first, second and fourth row of the
  //   GL_PROJECTION_MATRIX.  The third row defines the front and back
  //   z-axis clipping planes.
  vnl_matrix_fixed<double,4,4> Pinit;

  Pinit.set_row( 0, pmatrix.get_row(0));
  Pinit.set_row( 1, pmatrix.get_row(1));
  Pinit.set_row( 3, pmatrix.get_row(2));

  Pinit(2,0)= 0;
  Pinit(2,1)= 0;
  Pinit(2,2)= 0;
  Pinit(2,3)= 0;

  vnl_matrix_fixed<double,3,4> P = C*Pinit;
  vnl_vector_fixed<double,4> ABC = pmatrix.get_row(2);

  // this currently sets the back clipping plane to pass through
  //   point [1 0 -0.5] for no real reason... just that it seems
  //   to work for all my vrml models.  Geoff.
  double d1=
    (ABC(0)-pmatrix(2,0))* 1 +
    (ABC(1)-pmatrix(2,1))* 0 +
    (ABC(2)-pmatrix(2,2))* -0.5 + pmatrix(2,3);

  ABC(3)= d1;

  P.set_row( 2, ABC);

  return P;
}
