// This is ./oxl/vgui/vgui_camera.cxx
#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file
// \author Geoffrey Cross, Oxford RRG
// \date   03 Nov 99
// \brief  See vgui_camera.h for a description of this file.


#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

#include "vgui_camera.h"

// Default ctor
vgui_camera::vgui_camera()
  : pmatrix(3,4)
{
}

vgui_camera::vgui_camera( const vnl_matrix<double> &m)
  : pmatrix(3,4)
{
  set_pmatrix( m);
}

void vgui_camera::set_pmatrix( const vnl_matrix<double> &m)
{
  pmatrix= m;
}

// Note: this will return a GL_PROJECTION_MATRIX with the assumption that
//   you have an euclidean reconstruction.  The result is that the front and
//   back clipping planes will be PARALLEL (note: not projective frame!) to 
//   the image plane. 
vnl_matrix<double> vgui_camera::get_glprojmatrix( const int imagesizex, const int imagesizey) const
{
  vnl_matrix<double> C(4,4);

  // undo the viewport transformation.
  C(0,0)= 2.0/imagesizex; C(0,1)= 0;               C(0,2)= 0; C(0,3)= -1;
  C(1,0)= 0;              C(1,1)= -2.0/imagesizey; C(1,2)= 0; C(1,3)= 1;
  C(2,0)= 0;              C(2,1)= 0;               C(2,2)= 2; C(2,3)= -1;
  C(3,0)= 0;              C(3,1)= 0;               C(3,2)= 0; C(3,3)= 1;

  // the projection matrix sets the first, second and fourth row of the
  //   GL_PROJECTION_MATRIX.  The third row defines the front and back 
  //   z-axis clipping planes.
  vnl_matrix<double> Pinit(4,4);

  Pinit.set_row( 0, pmatrix.get_row(0));
  Pinit.set_row( 1, pmatrix.get_row(1));
  Pinit.set_row( 3, pmatrix.get_row(2));

  Pinit(2,0)= 0;
  Pinit(2,1)= 0;
  Pinit(2,2)= 0;
  Pinit(2,3)= 0;

  vnl_matrix<double> P( C*Pinit);
  vnl_vector<double> ABC( pmatrix.get_row(2));

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
