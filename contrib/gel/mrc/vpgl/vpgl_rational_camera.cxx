// This is gel/mrc/vpgl/vpgl_rational_camera.cxx
// Eventually this will be core/vil/file_formats/ ???

//:
// \file
// \date 2003/12/11
// \author: mlaymon
//
// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997-2003 TargetJr Consortium
//                            GE Global Research
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>

#include <vcl_cstdio.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

#include "vpgl_rational_camera.h"

// Note that the rational matrix used in this camera class is quite
// different from a general camera matrix.  The rational matrix here
// contains 4 rows.  Let X = (x,y,z,t) be the projective coordinates
// of a 3D point in space.  Let (u,v) be the affine coordinates of the
// 2D point which is the projection of this 3D point, as seen through
// the rational camera.  Then u = n1(X)/d1(X),  and v = n2(X)/d2(X),
// where ni, di are 4-variate homogeneous polynomials in X.  The
// rational matrix in this case is the coefficient matrix of polynomials
// n1, d1, n2, d2.  Rows 1 & 2 contain the coefficients of n1 & d1, and
// rows 3 & 4 contain the coefficients of n2 & d2.
//
//It is also worth noting the the coefficients are ordered according to, e.g.,
//
// n1(x,y,z) = M(0,0)*x^3 + M(0,1)*x^2y + M(0,2)*x^2z + .. M(0,9)x +.. M(0,19);
//
// see the definition of ::PowerVector for the complete specification

//------------------------------------------------------------------------
//   Constructors.
//------------------------------------------------------------------------

const int vpgl_rational_camera::O_SCALE_  = 0;
const int vpgl_rational_camera::O_OFFSET_ = 1;

const double vpgl_rational_camera::DEFAULT_SCALE_VALUE  = 1.0;
const double vpgl_rational_camera::DEFAULT_OFFSET_VALUE  = 0.0;

int vpgl_rational_camera::O_SCALE() {return O_SCALE_; }
int vpgl_rational_camera::O_OFFSET() {return O_OFFSET_; }

//:
// Create a camera with mat as the matrix containing the numerator and
// denominator vectors for the two image coordinates.  So, the matrix
// should have 4 rows.
vpgl_rational_camera::vpgl_rational_camera(vnl_matrix<double> const & rational_matrix)
    : world_scale_(false), image_scale_(false), global_(true)
{
  rational_matrix_ = rational_matrix;

  init_scale_vectors();
}

vpgl_rational_camera::vpgl_rational_camera(
  vnl_matrix<double> const &mat,
  vnl_vector<double> const &scx,
  vnl_vector<double> const &scy,
  vnl_vector<double> const &scz,
  vnl_vector<double> const &scu,
  vnl_vector<double> const &scv
 )
    : world_scale_(true), image_scale_(true), global_(true)
{
  rational_matrix_ = mat;
  scale_x_ = scx;
  scale_y_ = scy;
  scale_z_ = scz;
  scale_u_ = scu;
  scale_v_ = scv;

  //  init_pt.resize(3);
  init_pt[0] = scale_x_[O_OFFSET()];
  init_pt[1] = scale_y_[O_OFFSET()];
  init_pt[2] = scale_z_[O_OFFSET()];
}

//:
// Construct from four vectors describing the polynomials for the
// numerator and denominator for both u and v.
vpgl_rational_camera::vpgl_rational_camera(double *Un, double *Ud, double *Vn, double *Vd)
    : world_scale_(false), image_scale_(false), global_(true)
{
  vnl_matrix<double> matrix(4,20);
  for (int i=0; i<20; i++) matrix.put(0,i,Un[i]);
  for (int i=0; i<20; i++) matrix.put(1,i,Ud[i]);
  for (int i=0; i<20; i++) matrix.put(2,i,Vn[i]);
  for (int i=0; i<20; i++) matrix.put(3,i,Vd[i]);
  rational_matrix_ = matrix;

  init_scale_vectors();
}

////////////////////////////////////////////////////////////////////////
//: Initialize scale vectors.
//
// \param scale scale value.  Default value is 1.
// \param offset offset value.  Default value is 0.
//
///////////////////////////////////////////////////////////////////////
void vpgl_rational_camera::init_scale_vectors(double scale, double offset)
{
    vnl_vector<double> vals(2);
    vals[O_SCALE_] = scale;
    vals[O_OFFSET()] = offset;

    scale_x_ = vals;
    scale_y_ = vals;
    scale_z_ = vals;
    scale_u_ = vals;
    scale_v_ = vals;

    set_init_pt(scale_x_[O_OFFSET()],
                scale_y_[O_OFFSET()],
                scale_z_[O_OFFSET()]);
}

//: Sets up scaling of world coordinates
void vpgl_rational_camera::SetWorldScale(
    vnl_vector<double> &scx,
    vnl_vector<double> &scy,
    vnl_vector<double> &scz)
{
  world_scale_ = true;
  scale_x_ = scx;
  scale_y_ = scy;
  scale_z_ = scz;
}

//: Sets up scaling of image coordinates
void vpgl_rational_camera::SetImageScale(
  vnl_vector<double> &scu,
  vnl_vector<double> &scv
 )
{
  image_scale_ = true;
  scale_u_ = scu;
  scale_v_ = scv;
}

void vpgl_rational_camera::SetTranslation(double u, double v)
{
  image_scale_ = true;
  scale_u_[O_OFFSET()] = u;
  scale_v_[O_OFFSET()] = v;
}

double vpgl_rational_camera::GetTU()
{
  if (image_scale_) return scale_u_[O_OFFSET()];
  else return 0.0;
}

double vpgl_rational_camera::GetTV()
{
  if (image_scale_) return scale_v_[O_OFFSET()];
  else return 0.0;
}

//: Makes the inverse of a scaling transformation
vnl_vector<double> vpgl_rational_camera::scale_inverse(vnl_vector<double> &trans)
{
  vnl_vector<double> scaleinv(2);
  double scale = 1.0 / trans[O_SCALE()];
  scaleinv[O_SCALE()]  = scale;
  scaleinv[O_OFFSET()] = - trans[O_OFFSET()] * scale;
  return scaleinv;
}

//: Product of scales, first scale1, then scale2
vnl_vector<double> vpgl_rational_camera::scale_product(
  vnl_vector<double> &scale2,
  vnl_vector<double> &scale1
 )
{
  vnl_vector<double> scaleprod(2);
  scaleprod[O_SCALE()]  = scale1[O_SCALE()] * scale2[O_SCALE()];
  scaleprod[O_OFFSET()] = scale2[O_OFFSET()] + scale2[O_SCALE()] * scale1[O_OFFSET()];
  return scaleprod;
}

//: Add a further scaling to the image
void vpgl_rational_camera::rescale_image(
  vnl_vector<double> &scu,
  vnl_vector<double> &scv)
{
  scale_u_ = scale_product(scu, scale_u_);
  scale_v_ = scale_product(scv, scale_v_);
}

//: Add a further scaling to the world coordinates
void vpgl_rational_camera::RescaleWorld(
  vnl_vector<double> &scx,
  vnl_vector<double> &scy,
  vnl_vector<double> &scz
 )
{
  scale_x_ = scale_product(scx, scale_x_);
  scale_y_ = scale_product(scy, scale_y_);
  scale_z_ = scale_product(scz, scale_z_);
}

//------------------------------------------------------------------------
//   World To Image methods
//------------------------------------------------------------------------

//: Scale the world point according to the present scale factors
void vpgl_rational_camera::ScaleWorldPoint(vnl_vector<double>& xyz)
{
   xyz[0] = (xyz[0] - scale_x_[O_OFFSET()]) / scale_x_[O_SCALE()];
   xyz[1] = (xyz[1] - scale_y_[O_OFFSET()]) / scale_y_[O_SCALE()];
   xyz[2] = (xyz[2] - scale_z_[O_OFFSET()]) / scale_z_[O_SCALE()];
}

//: Scale the world point according to the present scale factors
void vpgl_rational_camera::ScaleImagePoint(double &u, double &v)
{
   u = u * scale_u_[O_SCALE()] + scale_u_[O_OFFSET()];
   v = v * scale_v_[O_SCALE()] + scale_v_[O_OFFSET()];
}

///////////////////////////////////////////////////////////////////////////
//: Project a 3D point to the image plane with this camera.
// Was WorldToImage(const vnl_vector<double>&, double& ix, double& iy)
//
// \param world_3d_pt point to be projected
// \param image_u
// \param image_v
// \param time time value - Used for ??? MAL 6nov2003
//
///////////////////////////////////////////////////////////////////////////
void vpgl_rational_camera::world_to_image(
    vnl_vector<double> const& world_3d_pt,
    double& image_u,
    double& image_v,
    double time)
{
    static vcl_string method_name = "vpgl_rational_camera::world_to_image: ";

  // First, check for pre-scaling
  vnl_vector<double> point = world_3d_pt;

// #define rh_DEBUG
#ifdef rh_DEBUG
  vcl_cout << method_name << vcl_endl
           << "Input point = " << point[0] << ','
           << point[1] << ','
           << point[2] << vcl_endl;
#endif

  // Scale the world point
  if (world_scale_) {
     ScaleWorldPoint(point);
  }

#ifdef rh_DEBUG
  vcl_cout << "Scaled point = " << point[0] << ','
           << point[1] << ','
           << point[2] << vcl_endl;
#endif

  vnl_vector<double> power_vect = PowerVector(point);
  vnl_vector<double> rational_image_point = rational_matrix_ * power_vect;
  image_u = rational_image_point[0]/rational_image_point[1];
  image_v = rational_image_point[2]/rational_image_point[3];

#ifdef rh_DEBUG
  vcl_cout << "Image point = " << image_u << ',' << image_v << vcl_endl;
#endif

  // Scale the image point
  if (image_scale_)
     ScaleImagePoint(image_u, image_v);

#ifdef rh_DEBUG
    vcl_cout << " point = " << image_u << ',' << image_v << vcl_endl;
#endif
}

///////////////////////////////////////////////////////////////////////////
//: Projects a 3D point to the image plane with this camera.
// Was WorldToImage(const double& x, const double& y, const double& z,
//                  double& ix, double& iy)
//
// \param x x value of point to be projected
// \param y y value of point to be projected
// \param z z value of point to be projected
// \param image_u
// \param image_v
// \param time time value - Used for ??? MAL 6nov2003
//
///////////////////////////////////////////////////////////////////////////
void vpgl_rational_camera::world_to_image(
    const double x,
    const double y,
    const double z,
    double& du,
    double& dv,
    double time)
{
  vnl_vector<double> xyz(3);
  xyz[0] = x;
  xyz[1] = y;
  xyz[2] = z;
  world_to_image(xyz, du, dv, time);
}

//-----------------------------------------------------------------
//: Inverse mapping function
//
// For the present we use the default.  However, we should do
// something like what is done in ImageToSurface
//
// NOTE: corresponding image_to_world method in both BasicCamera
// and vpgl_basic_camera just print out "Can't compute ImageToWorld"

void vpgl_rational_camera::image_to_world(
    vnl_vector<double> & ray3d_origin,
    vnl_vector<double> &world,
    double u,
    double v)
{
  vpgl_basic_camera::image_to_world(ray3d_origin, world, u, v);
}


// NOT IMPLEMENTED.  NO METHOD ImageToSurface IN
// NEW BASE CLASS vpgl_basic_camera.
#if 0
boolean vpgl_rational_camera::ImageToSurface(
  float ix, float iy,
  vnl_vector<float> &surface_pt, Surface *surf)
{
  // Back project image to surface point.
  // The basic routine will not converge without initialization.
  // We provide initialization from the camera offsets.
  // No initialization provided at present.
  return BasicCamera::ImageToSurface(ix, iy, surface_pt, surf);
}
#endif

//-----------------------------------------------------------------------

vnl_vector<double> vpgl_rational_camera::PowerVector(const vnl_vector<double>& point3d)
{
  // PowerVector should actually take a vector of 4 values, but we assume the
  // homogenizing variable is 1, and pass only the x, y & z values in a vector.

  double x = point3d[0];
  double y = point3d[1];
  double z = point3d[2];

  // Get the various powers, assumes t = 1.0
  double t  = 1.0;
  double xx = x*x;
  double xy = x*y;
  double xz = x*z;
  double yy = y*y;
  double yz = y*z;
  double zz = z*z;
  double xxx = x*xx;
  double xxy = x*xy;
  double xxz = x*xz;
  double xyy = x*yy;
  double xyz = x*yz;
  double xzz = x*zz;
  double yyy = y*yy;
  double yyz = y*yz;
  double yzz = y*zz;
  double zzz = z*zz;

  // Some other names for various of the variables
  double xtt = x*t*t;
  double ytt = y*t*t;
  double ztt = z*t*t;
  double ttt = t*t*t;
  double xxt = xx*t;
  double xyt = xy*t;
  double xzt = xz*t;
  double yyt = yy*t;
  double yzt = yz*t;
  double zzt = zz*t;

  // Declare the vector
  vnl_vector<double> V(20);

  // Fill it out
  V.put( 0, xxx);
  V.put( 1, xxy);
  V.put( 2, xxz);
  V.put( 3, xxt);
  V.put( 4, xyy);
  V.put( 5, xyz);
  V.put( 6, xyt);
  V.put( 7, xzz);
  V.put( 8, xzt);
  V.put( 9, xtt);
  V.put(10, yyy);
  V.put(11, yyz);
  V.put(12, yyt);
  V.put(13, yzz);
  V.put(14, yzt);
  V.put(15, ytt);
  V.put(16, zzz);
  V.put(17, zzt);
  V.put(18, ztt);
  V.put(19, ttt);

  // Return it
  return V;
}

// fills mat with the this camera matrix.
void vpgl_rational_camera::GetMatrix(vnl_matrix<double>& mat)
{
    mat = rational_matrix_;
}

void vpgl_rational_camera::GetScaleX(vnl_vector<double>& sclx)
{
    sclx = scale_x_;
}

void vpgl_rational_camera::GetScaleY(vnl_vector<double>& scly)
{ scly = scale_y_; }

void vpgl_rational_camera::GetScaleZ(vnl_vector<double>& sclz)
{ sclz = scale_z_; }

void vpgl_rational_camera::GetScaleU(vnl_vector<double>& sclu)
{ sclu = scale_u_; }

void vpgl_rational_camera::GetScaleV(vnl_vector<double>& sclv)
{ sclv = scale_v_; }

void vpgl_rational_camera::PrintData(vcl_ostream& strm) const
{
    strm << "Rational Camera Parameters :\n"

         << "  Matrix :\n"
         << rational_matrix_ << '\n'

         << "  Scale x, y, z :\t"
         << scale_x_ << ' '
         << scale_y_ << ' '
         << scale_z_ << '\n'

         << "  Scale u, v :\t"
         << scale_u_ << ' '
         << scale_v_ << ' '
         << global_ << '\n';
}

  // VIRTUAL METHOD FROM BASE CLASS BasicCameraInterface
  // Skip for now.  vpgl_basic_camera has no such method.
#if 0
BasicCameraInterface* vpgl_rational_camera::Copy() const
{
    vpgl_rational_camera* ret = new vpgl_rational_camera(
      this->rational_matrix_,
      this->scale_x_,
      this->scale_y_,
      this->scale_z_,
      this->scale_u_,
      this->scale_v_
     );

    ret->global_ = this->global_;

    return ret;
}
#endif

// REMOVE THESE METHODS FOR NOW.  SUPER-CLASS vpgl_basic_camera
// IMPLEMENTS methods get_init_pt AND set_init_pt.
#if 0
//------------------------------------------------------------
//: Computes the initial point as the center of the x, y, z domain as defined by the offset values
void vpgl_rational_camera::get_init_pt(IUE_vector<float>& p)
{
  p.set_x(init_pt_->x());
  p.set_y(init_pt_->y());
  p.set_z(init_pt_->z());
  return;
}

//------------------------------------------------------------
//: Set the initial point to the one specified
void vpgl_rational_camera::set_init_pt(IUE_vector<float>* init_pt)
{
  if (init_pt_ && init_pt)
  {
    init_pt_->set_x(init_pt->x());
    init_pt_->set_y(init_pt->y());
    init_pt_->set_z(init_pt->z());
  }
}
#endif

//------------------------------------------------------------
//: Set the initial point to the one specified
void vpgl_rational_camera::set_init_pt(double x, double y, double z)
{
    init_pt[0] = x;
    init_pt[1] = y;
    init_pt[2] = z;
}

///////////////////////////////////////////////////////
//:
// Verify camera using upper left, upper right,
// lower right, lower left of an image.  Right now, method just prints
// out the results of calling world_to_image for each corner.
// Moved from vpgl class to remove dependency on
// rational camera class from vil.
//
//  \param UL vector containing longitude, latitude and elevation for upper left
//  \param UR vector containing longitude, latitude and elevation for upper right
//  \param LR vector containing longitude, latitude and elevation for lower right
//  \param LL vector containing longitude, latitude and elevation for lower left
//
///////////////////////////////////////////////////////
void vpgl_rational_camera::verify_camera(
    vnl_vector<double> UL,
    vnl_vector<double> UR,
    vnl_vector<double> LR,
    vnl_vector<double> LL)
{
  // Output
  vcl_cout << "---------------------------------------------------\n"
           << "           VERIFYING RATIONAL CAMERA\n";

  // Project them
  double time = 0;
  double x, y;

  this->world_to_image(UL, x, y, time);
  vcl_fprintf(stdout, "Upper Left  (%14.6e, %14.6e, %14.6e) -> (%13.5e, %13.5e)\n",
              UL[0], UL[1], UL[2], x, y);

  this->world_to_image(UR, x, y, time);
  vcl_fprintf(stdout, "Upper Right (%14.6e, %14.6e, %14.6e) -> (%13.5e, %13.5e)\n",
              UR[0], UR[1], UR[2], x, y);

  this->world_to_image(LR, x, y, time);
  vcl_fprintf(stdout, "Lower Right (%14.6e, %14.6e, %14.6e) -> (%13.5e, %13.5e)\n",
              LR[0], LR[1], LR[2], x, y);

  this->world_to_image(LL, x, y, time);
  vcl_fprintf(stdout, "Lower Left  (%14.6e, %14.6e, %14.6e) -> (%13.5e, %13.5e)\n",
              LL[0], LL[1], LL[2], x, y);

  vcl_cout << "---------------------------------------------------\n";
}
