// This is core/vgui/vgui_projection_inspector.cxx
#include "vgui_projection_inspector.h"
//:
// \file
// \author fsm
// \brief  See vgui_projection_inspector.h for a description of this file.

#include <vcl_cassert.h>
#include <vcl_cstdlib.h> // vcl_abort()
#include <vcl_iostream.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_matlab_print.h>

#include <vgui/vgui_gl.h>

//------------------------------------------------------------------------------

vgui_projection_inspector::vgui_projection_inspector()
{
  inspect();
}

vgui_projection_inspector::~vgui_projection_inspector()
{
}

void vgui_projection_inspector::print(vcl_ostream& s) const
{
  s << "vgui_projection_inspector: {\n"
    << "VP = " << vp[0] << ' ' << vp[1] << ' ' << vp[2] << ' ' << vp[3] << '\n';
  vnl_matlab_print(s, P, "P");
  vnl_matlab_print(s, M, "M");
  s << "}\n";
}

//------------------------------------------------------------------------------

bool vgui_projection_inspector::back_project(double const x[3],
                                             double const p[4],
                                             double X[4]) const
{
  // get total 4x4 projection matrix :
  vnl_matrix_fixed<double,4,4> T = P*M;

  // get rows of corresponding 3x4 projection matrix :
  vnl_vector_fixed<double,4> a(T.get_row(0));
  vnl_vector_fixed<double,4> b(T.get_row(1));
  vnl_vector_fixed<double,4> c(T.get_row(3));

  // convert viewport coordinates to device coordinates :
  double devx = 2*(x[0]-vp[0]*x[2])/vp[2] - x[2];
  double devy = 2*(x[1]-vp[1]*x[2])/vp[3] - x[2];
  double devw = x[2];

  // compute (dual) pl\"ucker coordinates of backprojected line :
  vnl_matrix_fixed<double,4,4> omega = devx*outer_product(b,c) + devy*outer_product(c,a) + devw*outer_product(a,b);
  omega -= omega.transpose();

  // (un)dualize them :
  double omegad_[16]={
    0          , +omega(2,3), -omega(1,3), +omega(1,2),
    -omega(2,3), 0          , +omega(0,3), -omega(0,2),
    +omega(1,3), -omega(0,3), 0          , +omega(0,1),
    -omega(1,2), +omega(0,2), -omega(0,1), 0
  };
  vnl_matrix_fixed<double,4,4> omegad(omegad_);

  // backproject :
  vnl_vector_fixed<double,4> X_ = omegad * vnl_vector_fixed<double,4>(p);

  // normalize :
  double mag = X_.two_norm();
  if (mag == 0)
  {
    vcl_cerr << "mag=0, X_=" << X_ << vcl_endl;
    return false;
  }

  // done. normalize (making sure last component is >= 0).
  if (X_[3]<0)
    X_ *= (-1.0/mag);
  else
    X_ *= (1.0/mag);
  X[0] = X_[0];
  X[1] = X_[1];
  X[2] = X_[2];
  X[3] = X_[3];

  return true;
}

vnl_vector<double> vgui_projection_inspector::back_project(vnl_vector<double> const &x,
                                                           vnl_vector_fixed<double,4> const &p) const
{
  if (x.size() == 2)
  {
    vnl_vector_fixed<double,3> x_;
    x_[0]=x[0];
    x_[1]=x[1];
    x_[2]=1;
    vnl_vector_fixed<double,4> X_ = back_project(x_,p);
    return (X_/X_[3]).extract(3,0);
  }
  x.assert_size(3);

  vnl_vector_fixed<double,4> X;
  if (!back_project(x.data_block(), p.data_block(), X.data_block()))
    X.fill(0);
  return X.as_ref();
}

vnl_vector<double> vgui_projection_inspector::back_project(double x,double y,
                                                           vnl_vector_fixed<double,4> const &p) const
{
  vnl_vector<double> xy(2, x,y);
  return back_project(xy,p);
}

vnl_vector<double> vgui_projection_inspector::back_project(double x,double y,double z,
                                                           vnl_vector_fixed<double,4> const &p) const
{
  vnl_vector<double> xyz(3, x,y,z);
  return back_project(xyz,p);
}

//------------------------------------------------------------------------------

// returns true iff M has the form
// * * * *
// * * * *
// * * * *
// 0 0 0 *
static bool is_affine(const vnl_matrix_fixed<double,4,4> &M)
{
  return M(3,0)==0 && M(3,1)==0 && M(3,2)==0 && M(3,3)!=0;
}

void vgui_projection_inspector::inspect()
{
  // get viewport :
  glGetIntegerv(GL_VIEWPORT,(GLint*)vp); // fixed

  // get projection and modelview matrices :
  {
    glGetDoublev(GL_PROJECTION_MATRIX,P.data_block());
    P.inplace_transpose();

    glGetDoublev(GL_MODELVIEW_MATRIX,M.data_block());
    M.inplace_transpose();
  }

  // compute total transformation T from world to clip coordinates :
  vnl_matrix_fixed<double,4,4> T = P*M;

  // if projection is scaling parallel to axes :
  // [ *     * ]
  // [   *   * ]
  // [     * * ]
  // [       * ]
  if (is_affine(T) &&
      T(1,0)==0 && T(0,1)==0 &&
      T(2,0)==0 && T(0,2)==0 &&
      T(2,1)==0 && T(1,2)==0 &&
      T(3,3)!=0)
  {
    //
    diagonal_scale_3d = true;

    // backproject the corners of the plane {z=0} (in clip coordinates) to
    // the plane {z=0} in world coordinates :
    x1 = ((-1)*T(3,3)-T(0,3))/T(0,0);
    y1 = ((-1)*T(3,3)-T(1,3))/T(1,1);

    x2 = ((+1)*T(3,3)-T(0,3))/T(0,0);
    y2 = ((+1)*T(3,3)-T(1,3))/T(1,1);

    //
    s[0] = T(0,0)/T(3,3);
    s[1] = T(1,1)/T(3,3);
    s[2] = T(2,2)/T(3,3);

    t[0] = T(0,3)/T(3,3);
    t[1] = T(1,3)/T(3,3);
    t[2] = T(2,3)/T(3,3);
  }

  //
  else
  {
    diagonal_scale_3d = false;
#ifdef DEBUG
     vcl_cerr << "T =\n" << T << '\n';
#endif
  }
}

void vgui_projection_inspector::window_to_image_coordinates(int x,int y,
                                                            float &xi,float &yi) const
{
  if (!diagonal_scale_3d)
  {
    vcl_cerr << "vgui_projection_inspector::window_to_image_coordinates() - ERROR: Need diagonal GL matrices\n";
    print(vcl_cerr);
    vcl_abort();
  }

  // size of viewport :
  int winx=vp[2];
  int winy=vp[3];

  // subtract offset of viewport :
  x -= vp[0];
  y -= vp[1];

  //
  xi = ((winx-x)*x1 + (     x)*x2)/winx;
  yi = ((winy-y)*y1 + (     y)*y2)/winy;
  //yi = ((     y)*y1 + (winy-y)*y2)/winy;
}

// This method computes the viewport coordinates of the projection of the point (ix, iy, 0, 1).
void vgui_projection_inspector::image_to_window_coordinates(float ix,float iy,float &wx,float &wy) const
{
  assert(diagonal_scale_3d);

  // compute total transformation T from world to clip coordinates :
  vnl_matrix_fixed<double,4,4> T = P*M;

  // world point :
  vnl_vector_fixed<double,4> img;
  img[0] = ix;
  img[1] = iy;
  img[2] = 0;
  img[3] = 1;

  // project :
  vnl_vector_fixed<double,4> win = T*img;

  // normalized coordinates :
  float nx = win[0]/win[3];
  float ny = win[1]/win[3];

  // convert normalised coordinates to viewport coordinates :
  wx = vp[0] + (nx - -1)/2 * vp[2];
  wy = vp[1] + (ny - -1)/2 * vp[3];
}

// fsm: what does this compute?
// pcp: why a token2D of course! see vgui_token2D for what that is,.
// fsm: ok, a vgui_token2D represents a coordinate transformation with
//      translation and scaling along the axis. but I don't understand
//      if I'm supposed to apply the scaling first, then the translation
//      or vice versa. as for the method, exactly which coordinate
//      transformation does it compute? that's what I wanted to know.
// pcp: what am i ? a mind reader ? here's what you wanted
//      to transform from window coordinates (x,y) to image coordinates (ix,iy)
//
//        ix = (x - token.offsetX) / token.scaleX;
//        iy = (y - token.offsetY) / token.scaleY;
// fsm: thank you.
//
//vgui_token2D is now deprecated.

//-----------------------------------------------------------------------------
bool vgui_projection_inspector::compute_as_2d_affine(int width, int height,
                                                     float* offsetX, float* offsetY,
                                                     float* scaleX, float* scaleY)
{
  if (!diagonal_scale_3d)
    return false;

  image_to_window_coordinates(0,0,*offsetX, *offsetY);

  float tmpX, tmpY;
  image_to_window_coordinates(width, height, tmpX, tmpY);

  *scaleX =  (tmpX - *offsetX)/width;
  *scaleY = -(tmpY - *offsetY)/height;

  return true;
}


//-----------------------------------------------------------------------------
bool vgui_projection_inspector::image_viewport(float& a0, float& b0,
                                               float& a1, float& b1)
{
  if (!diagonal_scale_3d)
    return false;

  a0 = x1;
  b0 = y1;
  a1 = x2;
  b1 = y2;

  return true;
}
