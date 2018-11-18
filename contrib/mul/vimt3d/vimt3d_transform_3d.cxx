// This is mul/vimt3d/vimt3d_transform_3d.cxx
#include <iostream>
#include <cstdlib>
#include <sstream>
#include "vimt3d_transform_3d.h"
//:
// \file
// \brief A class to define and apply a 3D transformation up to affine.
// \author Graham Vincent, Tim Cootes

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_indent.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_determinant.h>
#include <vnl/vnl_math.h>
#include <vul/vul_string.h>
#include <vul/vul_sprintf.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_sequence.h>


//=======================================================================

vnl_matrix<double> vimt3d_transform_3d::matrix() const
{
  vnl_matrix<double> M(4,4);
  matrix(M);
  return M;
}

//=======================================================================

void vimt3d_transform_3d::matrix(vnl_matrix<double>& M) const
{
#if 0 //grv
  if ((M.rows()!=4) || (M.columns()!=4)) M.resize(4,4);
#endif
  M.set_size(4,4);
  double**m_data = M.data_array();
  m_data[0][0]=xx_; m_data[0][1]=xy_; m_data[0][2]=xz_; m_data[0][3]=xt_;
  m_data[1][0]=yx_; m_data[1][1]=yy_; m_data[1][2]=yz_; m_data[1][3]=yt_;
  m_data[2][0]=zx_; m_data[2][1]=zy_; m_data[2][2]=zz_; m_data[2][3]=zt_;
  m_data[3][0]=tx_; m_data[3][1]=ty_; m_data[3][2]=tz_; m_data[3][3]=tt_;
}

//=======================================================================
// Define the transform in terms of a 4x4 homogeneous matrix.
void vimt3d_transform_3d::set_matrix(const vnl_matrix<double>& M)
{
  if (M.rows()!=4 || M.cols()!=4)
    mbl_exception_error(mbl_exception_abort("vimt3d_transform_3d::set_matrix(matrix): input matrix must be 4x4"));

  form_=Affine;
  xx_=M[0][0]; xy_=M[0][1]; xz_=M[0][2]; xt_=M[0][3];
  yx_=M[1][0]; yy_=M[1][1]; yz_=M[1][2]; yt_=M[1][3];
  zx_=M[2][0]; zy_=M[2][1]; zz_=M[2][2]; zt_=M[2][3];
  tx_=M[3][0]; ty_=M[3][1]; tz_=M[3][2]; tt_=M[3][3];
}

//=======================================================================
// See also vnl_rotation_matrix(), vgl_rotation_3d, and vnl_quaternion
void vimt3d_transform_3d::angles(double& phi_x, double& phi_y, double& phi_z) const
{
  // NB: in affine case will probably have to store s_x, s_y, s_z etc somewhere else!
  // also won't work properly in rigid body case either!
  double det=+xx_*yy_*zz_-xx_*zy_*yz_-yx_*xy_*zz_+yx_*zy_*xz_+zx_*xy_*yz_-zx_*yy_*xz_;

  double xlen = std::sqrt(xx_*xx_ + yx_*yx_ + zx_*zx_)* vnl_math::sgn(det);
  double ylen = std::sqrt(xy_*xy_ + yy_*yy_ + zy_*zy_)* vnl_math::sgn(det);
  double zlen = std::sqrt(xz_*xz_ + yz_*yz_ + zz_*zz_)* vnl_math::sgn(det);

  double xx3 = xx_ / xlen;
  double xy3 = xy_ / ylen;
  double xz3 = xz_ / zlen;
  double yz3 = yz_ / zlen;
  double zz3 = zz_ / zlen;

  phi_x = std::atan2(-yz3,zz3);
  phi_y = std::atan2(-xz3*std::cos(phi_x),zz3);
  phi_z=std::atan2(-xy3,xx3);

  // nb the equation for phi_z doesn't work in affine case
  // because sy and sx aren't necessarily the same

  // calc scaling factor
  // ie assuming similarity transform here
  // assume s is always positive
  // to recover original angle
  double s;
  if (std::sin(phi_y) < 1e-20)
    s = 1.0;
  else
    s = std::fabs( xz3/ (-1*std::sin(phi_y) ) );

#ifdef DEBUG
  std::cout<<"s= "<<s<<std::endl;
#endif

  // the angles may be wrong by +-vnl_math::pi - we can
  // only tell by checking against the signs
  // of the original entries in the rotation matrix
  if (std::fabs(std::sin(phi_y)*s + xz3) > 1e-6)
  {
    if (phi_y > 0)
      phi_y -= vnl_math::pi;
    else
      phi_y += vnl_math::pi;
    //phi_y *= -1;
  }

  const double cos_y = std::cos(phi_y);

  if (std::fabs(std::sin(phi_x)*cos_y*s + yz3) > 1e-6 ||
      std::fabs(std::cos(phi_x)*cos_y*s - zz3) > 1e-6)
  {
    if (phi_x > 0)
      phi_x -= vnl_math::pi;
    else
      phi_x += vnl_math::pi;
  }

  if (std::fabs(std::cos(phi_z)*cos_y*s - xx3) > 1e-6 ||
      std::fabs(std::sin(phi_z)*cos_y*s + xy3) > 1e-6)
  {
    if (phi_z > 0)
      phi_z -= vnl_math::pi;
    else
      phi_z += vnl_math::pi;
  }

  // now compress the angles towards zero as much as possible
  // (we can add +-vnl_math::pi to each angle and negate phi_y without changing
  // the rotation matrix)
  int count = 0;
  if (std::fabs(phi_x) > vnl_math::pi/2) ++count;
  if (std::fabs(phi_y) > vnl_math::pi/2) ++count;
  if (std::fabs(phi_z) > vnl_math::pi/2) ++count;

  if (count > 1)
  {
    if (phi_x > 0)
      phi_x -= vnl_math::pi;
    else
      phi_x += vnl_math::pi;

    phi_y=-phi_y;
    if (phi_y > 0)
      phi_y -= vnl_math::pi;
    else
      phi_y += vnl_math::pi;

    if (phi_z > 0)
      phi_z -= vnl_math::pi;
    else
      phi_z += vnl_math::pi;
  }
}

//=======================================================================

void vimt3d_transform_3d::params(vnl_vector<double>& v) const
{
  switch (form_)
  {
   case (Identity):
    v.set_size(0);
    break;
   case (Translation):
    if (v.size()!=3) v.set_size(3);
    v[0]=xt_; v[1]=yt_; v[2]=zt_;
    break;
   case (ZoomOnly):
    if (v.size()!=6) v.set_size(6);
    v[0]=xx_; v[1]=yy_; v[2]=zz_;
    v[3]=xt_; v[4]=yt_; v[5]=zt_;
    break;
   case (RigidBody):
    if (v.size()!=6) v.set_size(6);
    angles(v[0],v[1],v[2]);
    v[3]=xt_; v[4]=yt_; v[5]=zt_;
    break;
   case (Similarity): // not sure this is right - kds
                      // I think it's fixed now -dac
    if (v.size()!=7) v.set_size(7);
    angles(v[1],v[2],v[3]);
    // compute scaling factor
    v[0]= xx_/ ( std::cos( v[2] ) *std::cos( v[3] ) );
    v[4]=xt_; v[5]=yt_; v[6]=zt_;
    break;
   case (Affine):     // not sure this is right - kds
                      // I'm sure it's not correct -dac
    {
      v.set_size(9);
      // computation of angles doesn't work unless
      // sx, sy, sz are all the same!

      angles(v[3],v[4],v[5]);
      // try to compute scaling factors
      double det=+xx_*yy_*zz_-xx_*zy_*yz_-yx_*xy_*zz_+yx_*zy_*xz_+zx_*xy_*yz_-zx_*yy_*xz_;
      v[0]=std::sqrt(xx_*xx_ + yx_*yx_ + zx_*zx_)* vnl_math::sgn(det);
      v[1]=std::sqrt(xy_*xy_ + yy_*yy_ + zy_*zy_)* vnl_math::sgn(det);
      v[2]=std::sqrt(xz_*xz_ + yz_*yz_ + zz_*zz_)* vnl_math::sgn(det);
      v[6]=xt_; v[7]=yt_; v[8]=zt_;
      break;
    }
   default:
    mbl_exception_error(mbl_exception_abort(
      vul_sprintf("vimt3d_transform_3d::params() Unexpected form: %d", form_) ));
  }
}


//=======================================================================
void vimt3d_transform_3d::simplify(double tol /*=1e-10*/)
{
  double rx, ry, rz;
  double sx, sy, sz;
  double det;
  switch (form_)
  {
   case Affine:
    { // Not really true affine, because shear is forbidden.
      angles(rx, ry, rz);
      double matrix_form[]= {xx_, yx_, zx_, xy_, yy_, zy_, xz_, yz_, zz_};
      vnl_matrix_fixed<double, 3, 3> X(matrix_form);
      vnl_matrix_fixed<double, 3, 3> S2 = X.transpose() * X;
      // if X=R*S (where S is a diagonal matrix) then X'X = S'*R'*R*S
      // if R is a rotation matrix then R'*R=I and so X'X = S'*S = [s_x^2 0 0; 0 s_y^2 0; 0 0 s_z^2]
      if (S2(0,1)*S2(0,1) + S2(0,2)*S2(0,2) + S2(1,0)*S2(1,0) +
          S2(1,2)*S2(1,2) + S2(2,0)*S2(2,0) + S2(2,1)*S2(2,1) >= tol*tol*6)
        return;

      // mirroring if det is negative;
      double mirror=vnl_math::sgn(vnl_determinant(X[0], X[1], X[2]));

      sx = std::sqrt(std::abs(S2(0,0))) * mirror;
      sy = std::sqrt(std::abs(S2(1,1))) * mirror;
      sz = std::sqrt(std::abs(S2(2,2))) * mirror;
      if (vnl_math::sqr(sx-sy) +  vnl_math::sqr(sx-sz) < tol*tol)
        this->set_similarity(sx, rx, ry, rz,
                             xt_, yt_, zt_ );
      else if (rx*rx+ry*ry+rz*rz < tol*tol)
        this->set_zoom_only(sx, sy, sz,
                            xt_, yt_, zt_);
      else
        return;
      simplify(tol);
      return;
    }
   case Similarity:
    angles(rx, ry, rz);
    det=+xx_*yy_*zz_-xx_*zy_*yz_-yx_*xy_*zz_+yx_*zy_*xz_+zx_*xy_*yz_-zx_*yy_*xz_;
    sx=std::sqrt(xx_*xx_ + yx_*yx_ + zx_*zx_)* vnl_math::sgn(det);
    if (rx*rx+ry*ry+rz*rz < tol*tol)
      this->set_zoom_only(sx, xt_, yt_, zt_);
    else if (vnl_math::sqr(sx-1.0) < tol*tol)
      this->set_rigid_body(rx, ry, rz, xt_, yt_, zt_);
    else
      return;
    simplify(tol);
    return;

   case RigidBody:
    angles(rx, ry, rz);
    if (rx*rx+ry*ry+rz*rz >= tol*tol)
      return;
    this->set_translation(xt_, yt_, zt_);
    simplify(tol);
    return;
   case ZoomOnly:
    if (vnl_math::sqr(xx_-1.0) + vnl_math::sqr(yy_-1.0) + vnl_math::sqr(zz_-1.0) >= tol*tol)
      return;
    set_translation(xt_, yt_, zt_);
   case Translation:
    if (xt_*xt_+yt_*yt_+zt_*zt_<tol*tol)
      set_identity();
    return;
   case Identity:
    return;
   default:
    mbl_exception_error(mbl_exception_abort(
      vul_sprintf("vimt3d_transform_3d::simplify() Unexpected form: %d", form_) ));
  }
}

//=======================================================================

void vimt3d_transform_3d::setCheck(int n1,int n2,const char* str) const
{
  if (n1==n2) return;
  std::ostringstream ss;
  ss << "vimt3d_transform_3d::set() " << n1 << " parameters required for "
     << str << ". Passed " << n2;
  mbl_exception_error(mbl_exception_abort(ss.str()));
}

//=======================================================================

void vimt3d_transform_3d::set(const vnl_vector<double>& v, Form form)
{
  int n=v.size();

  switch (form)
  {
   case (Identity):
    set_identity();
    break;
   case (Translation):
    setCheck(3,n,"Translation");
    set_translation(v[0],v[1],v[2]);
    break;
   case (ZoomOnly):
    setCheck(6,n,"ZoomOnly");
    set_zoom_only( v[0],v[1],v[2],
                   v[3],v[4],v[5]);
    break;
   case (RigidBody):
    setCheck(6,n,"RigidBody");
    set_rigid_body( v[0],v[1],v[2],
                    v[3],v[4],v[5]);
    break;
   case (Similarity):
    setCheck(7,n,"Similarity");
    set_similarity( v[0],v[1],v[2],
                    v[3],v[4],v[5], v[6]);
    form_ = Similarity;
    inv_uptodate_=false;
    break;
   case (Affine): // not sure this is right - kds
                  // it works unless you call params()
                  // later which gives the wrong answer
                  // in affine case -dac
    setCheck(9,n,"Affine");
    set_affine( v[0],v[1],v[2],
                v[3],v[4],v[5],
                v[6],v[7],v[8]);
    form_ = Affine;
    inv_uptodate_=false;
    break;
   default:
    mbl_exception_error(mbl_exception_abort(
      vul_sprintf("vimt3d_transform_3d::set() Unexpected form: %d", form_) ));
  }
}

//=======================================================================
// See also vnl_rotation_matrix(), vgl_rotation_3d, and vnl_quaternion
void vimt3d_transform_3d::setRotMat( double r_x, double r_y, double r_z )
{
  double sinx=std::sin(r_x);
  double siny=std::sin(r_y);
  double sinz=std::sin(r_z);
  double cosx=std::cos(r_x);
  double cosy=std::cos(r_y);
  double cosz=std::cos(r_z);

  // set R_mat = Rx*Ry*Rz
  xx_ =  cosy*cosz;
  xy_ = -cosy*sinz;
  xz_ = -siny;
  yx_ =  cosx*sinz - sinx*siny*cosz;
  yy_ =  cosx*cosz + sinx*siny*sinz;
  yz_ = -sinx*cosy;
  zx_ =  sinx*sinz + cosx*siny*cosz;
  zy_ =  sinx*cosz - cosx*siny*sinz;
  zz_ =  cosx*cosy;
}

//=======================================================================

void vimt3d_transform_3d::set_origin( const vgl_point_3d<double> & p )
{
  xt_=p.x()*tt_;
  yt_=p.y()*tt_;
  zt_=p.z()*tt_;

  if (form_ == Identity) form_=Translation;

  inv_uptodate_=false;
}

//=======================================================================

void vimt3d_transform_3d::set_identity()
{
  if (form_==Identity) return;
  form_=Identity;

  xx_=yy_=zz_=tt_=1;
  xy_=xz_=xt_=0;
  yx_=yz_=yt_=0;
  zx_=zy_=zt_=0;
  tx_=ty_=tz_=0;
  inv_uptodate_=false;
}

//=======================================================================

void vimt3d_transform_3d::set_translation(double t_x, double t_y, double t_z)
{
  if (t_x==0 && t_y==0 && t_z==0)
    set_identity();
  else
  {
    form_=Translation;

    // Set translation (first 3 elements of final column)
    xt_=t_x;
    yt_=t_y;
    zt_=t_z;

    // Set all other elements to defaults
    xx_=yy_=zz_=tt_=1;
    xy_=xz_=yx_=yz_=zx_=zy_=0;
    tx_=ty_=tz_=0;
  }

  inv_uptodate_=false;
}

//=======================================================================

void vimt3d_transform_3d::set_zoom_only(double s_x, double s_y, double s_z,
                                        double t_x, double t_y, double t_z)
{
  form_=ZoomOnly;

  // Set scaling (first 3 diagonal elements)
  xx_=s_x;
  yy_=s_y;
  zz_=s_z;

  // Set translation (first 3 elements of final column)
  xt_=t_x;
  yt_=t_y;
  zt_=t_z;

  // Set all other elements to defaults
  tx_=ty_=tz_=0;
  xy_=xz_=yx_=yz_=zx_=zy_=0;
  tt_=1;

  inv_uptodate_=false;
}

//=======================================================================

void vimt3d_transform_3d::set_rigid_body(double r_x, double r_y, double r_z,
                                         double t_x, double t_y, double t_z)
{
  if (r_x==0.0 && r_y==0.0 && r_z==0.0)
  {
    set_translation(t_x,t_y,t_z);
  }
  else
  {
    form_=RigidBody;

    // Set rotation matrix
    setRotMat(r_x,r_y,r_z);

    // Set translation (first 3 elements of final column)
    xt_=t_x;
    yt_=t_y;
    zt_=t_z;

    // Set all other elements to defaults
    tx_=0; ty_=0; tz_=0; tt_=1;
  }

  inv_uptodate_=false;
}

//: Sets the transformation to be rotation, followed by translation.
// The transformation is separable affine.
// \param q  Unit quaternion defining rotation
void vimt3d_transform_3d::set_rigid_body(const vnl_quaternion<double>& q,
                                         double t_x, double t_y, double t_z)
{
  if (q.angle()==0.0)
  {
    set_translation(t_x,t_y,t_z);
    return;
  }
  vnl_matrix_fixed<double,3,3> R = q.rotation_matrix_transpose();
  form_=RigidBody;

  // Set rotation terms from R, which is transpose of Rot mat.
  xx_=R[0][0];  xy_= R[1][0]; xz_ = R[2][0];
  yx_=R[0][1];  yy_= R[1][1]; yz_ = R[2][1];
  zx_=R[0][2];  zy_= R[1][2]; zz_ = R[2][2];

  // Set translation
  xt_=t_x;
  yt_=t_y;
  zt_=t_z;

  inv_uptodate_=false;
}

//: Sets the transformation to be similarity: scale, rotation, followed by translation.
// The transformation is separable affine.
// \param unit_q  Unit quaternion defining rotation
void vimt3d_transform_3d::set_similarity(double s, const vnl_quaternion<double>& q,
                      double t_x, double t_y, double t_z)
{
  if (q.angle()==0.0)
  {
    set_zoom_only(s,t_x,t_y,t_z);
    return;
  }
  vnl_matrix_fixed<double,3,3> R = q.rotation_matrix_transpose();
  form_=RigidBody;

  // Set scale/rotation terms from R, which is transpose of Rot mat.
  xx_=s*R[0][0];  xy_= s*R[1][0]; xz_ = s*R[2][0];
  yx_=s*R[0][1];  yy_= s*R[1][1]; yz_ = s*R[2][1];
  zx_=s*R[0][2];  zy_= s*R[1][2]; zz_ = s*R[2][2];

  // Set translation
  xt_=t_x;
  yt_=t_y;
  zt_=t_z;

  inv_uptodate_=false;
}
//=======================================================================

void vimt3d_transform_3d::set_similarity(double s,
                                         double r_x, double r_y, double r_z,
                                         double t_x, double t_y, double t_z)
{
  if (s==1.0)
  {
    set_rigid_body(r_x,r_y,r_z,t_x,t_y,t_z);
  }
  else
  {
    form_=Similarity;

    // Set rotation matrix
    setRotMat(r_x,r_y,r_z);

#ifdef DEBUG // debug test
    double r_x1, r_y1, r_z1;
    angles( r_x1, r_y1, r_z1 );
    std::cout << "r_x = " << r_x  << '\n'
             << "r_x1= " << r_x1 << '\n'
             << "r_y = " << r_y  << '\n'
             << "r_y1= " << r_y1 << '\n'
             << "r_z = " << r_z  << '\n'
             << "r_z1= " << r_z1 << std::endl;
#endif

    // Account for scaling (this actually means that scaling was done BEFORE rotation)
    xx_*=s;  xy_*=s;  xz_*=s;
    yx_*=s;  yy_*=s;  yz_*=s;
    zx_*=s;  zy_*=s;  zz_*=s;

    // Set translation (first 3 elements of final column)
    xt_=t_x;
    yt_=t_y;
    zt_=t_z;

    // Set all other elements to defaults
    tx_=0; ty_=0; tz_=0; tt_=1;
  }
  inv_uptodate_=false;
}

//=======================================================================

void vimt3d_transform_3d::set_affine(double s_x, double s_y, double s_z,
                                     double r_x, double r_y, double r_z,
                                     double t_x, double t_y, double t_z)
{
  form_=Affine;

  // Set rotation matrix
  setRotMat(r_x,r_y,r_z);

  // Account for scaling (this actually means that scaling was done BEFORE rotation)
  xx_*=s_x;  xy_*=s_y;  xz_*=s_z;
  yx_*=s_x;  yy_*=s_y;  yz_*=s_z;
  zx_*=s_x;  zy_*=s_y;  zz_*=s_z;

  // Set translation (first 3 elements of final column)
  xt_=t_x;
  yt_=t_y;
  zt_=t_z;

  // Set all other elements to defaults
  tx_=0; ty_=0; tz_=0; tt_=1;

  inv_uptodate_=false;
}
//=======================================================================

void vimt3d_transform_3d::set_affine(double s_x, double s_y, double s_z,
                                     vgl_vector_3d<double> c_x,
                                     vgl_vector_3d<double> c_y,
                                     vgl_vector_3d<double> c_z,
                                     double t_x, double t_y, double t_z)
{
  form_=Affine;

  // Set rotation matrix
  xx_ = c_x.x(); xy_ = c_y.x(); xz_ = c_z.x();
  yx_ = c_x.y(); yy_ = c_y.y(); yz_ = c_z.y();
  zx_ = c_x.z(); zy_ = c_y.z(); zz_ = c_z.z();


  // Account for scaling (this actually means that scaling was done BEFORE rotation)
  xx_*=s_x;  xy_*=s_y;  xz_*=s_z;
  yx_*=s_x;  yy_*=s_y;  yz_*=s_z;
  zx_*=s_x;  zy_*=s_y;  zz_*=s_z;

  // Set translation (first 3 elements of final column)
  xt_=t_x;
  yt_=t_y;
  zt_=t_z;

  // Set all other elements to defaults
  tx_=0; ty_=0; tz_=0; tt_=1;

  inv_uptodate_=false;
}

//=======================================================================

void vimt3d_transform_3d::set_affine(const vgl_point_3d<double>& p,
                                     const vgl_vector_3d<double>& u,
                                     const vgl_vector_3d<double>& v,
                                     const vgl_vector_3d<double>& w)
{
  form_=Affine;

#ifndef NDEBUG
  const double tol=1e-6;

  // Get normalized vectors
  vgl_vector_3d<double> uh = normalized(u);
  vgl_vector_3d<double> vh = normalized(v);
  vgl_vector_3d<double> wh = normalized(w);

  // Test for orthogonality of input vectors
  assert(std::fabs(dot_product(uh,vh))<tol);
  assert(std::fabs(dot_product(vh,wh))<tol);
  assert(std::fabs(dot_product(wh,uh))<tol);

  // Test for right-handedness of input vectors
  assert(std::fabs((cross_product(uh,vh)-wh).length())<tol);
#endif

  // Set rotation and scaling (this actually means that scaling was done BEFORE rotation)
  xx_=u.x();  xy_=v.x();  xz_=w.x();
  yx_=u.y();  yy_=v.y();  yz_=w.y();
  zx_=u.z();  zy_=v.z();  zz_=w.z();

  // Set translation (first 3 elements of final column)
  xt_=p.x();
  yt_=p.y();
  zt_=p.z();

  // Set final row with default values (for all transforms up to affine)
  tx_=0; ty_=0; tz_=0; tt_=1;

  inv_uptodate_=false;
}

//=======================================================================

vimt3d_transform_3d vimt3d_transform_3d::inverse() const
{
  if (!inv_uptodate_) calcInverse();

  vimt3d_transform_3d inv;

  inv.xx_ = xx2_; inv.xy_ = xy2_; inv.xz_ = xz2_; inv.xt_ = xt2_;
  inv.yx_ = yx2_; inv.yy_ = yy2_; inv.yz_ = yz2_; inv.yt_ = yt2_;
  inv.zx_ = zx2_; inv.zy_ = zy2_; inv.zz_ = zz2_; inv.zt_ = zt2_;
  inv.tx_ = tx2_; inv.ty_ = ty2_; inv.tz_ = tz2_; inv.tt_ = tt2_;

  inv.xx2_ = xx_; inv.xy2_ = xy_; inv.xz2_ = xz_; inv.xt2_ = xt_;
  inv.yx2_ = yx_; inv.yy2_ = yy_; inv.yz2_ = yz_; inv.yt2_ = yt_;
  inv.zx2_ = zx_; inv.zy2_ = zy_; inv.zz2_ = zz_; inv.zt2_ = zt_;
  inv.tx2_ = tx_; inv.ty2_ = ty_; inv.tz2_ = tz_; inv.tt2_ = tt_;

  inv.form_ = form_;
  inv.inv_uptodate_ = true;

  return inv;
}

//=======================================================================

void vimt3d_transform_3d::calcInverse() const
{
  xx2_=yy2_=zz2_=tt2_=1;
  xy2_ = xz2_= xt2_ = yx2_ = yz2_= yt2_ = zx2_ = zy2_ = zt2_ = tx2_ = ty2_ = tz2_ = 0;

  switch (form_)
  {
   case Identity :
    break;
   case Translation :
    xt2_=-xt_;
    yt2_=-yt_;
    zt2_=-zt_;
    break;
   case ZoomOnly :
    xx2_=1.0/xx_;
    yy2_=1.0/yy_;
    zz2_=1.0/zz_;
    xt2_=-xt_/xx_;
    yt2_=-yt_/yy_;
    zt2_=-zt_/zz_;
    break;
   case RigidBody :
    // transpose x,y,z part
    xx2_=xx_;
    xy2_=yx_;
    xz2_=zx_;
    yx2_=xy_;
    yy2_=yy_;
    yz2_=zy_;
    zx2_=xz_;
    zy2_=yz_;
    zz2_=zz_;
    xt2_=-(xx2_*xt_ + xy2_*yt_ + xz2_*zt_);
    yt2_=-(yx2_*xt_ + yy2_*yt_ + yz2_*zt_);
    zt2_=-(zx2_*xt_ + zy2_*yt_ + zz2_*zt_);
    break;
   case Similarity :
   case Affine : {
      // affine inverse (plugged in from symbolic matlab)
      double det=-xx_*yy_*zz_+xx_*zy_*yz_+yx_*xy_*zz_-yx_*zy_*xz_-zx_*xy_*yz_+zx_*yy_*xz_;
      if (det==0)
      {
        std::cerr<<"vimt3d_transform_3d() : No inverse exists for this affine transform (det==0)\n";
        std::abort();
      }

      xx2_=(-yy_*zz_+zy_*yz_)/det;
      xy2_=( xy_*zz_-zy_*xz_)/det;
      xz2_=(-xy_*yz_+yy_*xz_)/det;
      xt2_=(xy_*yz_*zt_-xy_*yt_*zz_-yy_*xz_*zt_+yy_*xt_*zz_+zy_*xz_*yt_-zy_*xt_*yz_)/det;

      yx2_=( yx_*zz_-zx_*yz_)/det;
      yy2_=(-xx_*zz_+zx_*xz_)/det;
      yz2_=( xx_*yz_-yx_*xz_)/det;
      yt2_=(-xx_*yz_*zt_+xx_*yt_*zz_+yx_*xz_*zt_-yx_*xt_*zz_-zx_*xz_*yt_+zx_*xt_*yz_)/det;

      zx2_=(-yx_*zy_+zx_*yy_)/det;
      zy2_=( xx_*zy_-zx_*xy_)/det;
      zz2_=(-xx_*yy_+yx_*xy_)/det;
      zt2_=( xx_*yy_*zt_-xx_*yt_*zy_-yx_*xy_*zt_+yx_*xt_*zy_+zx_*xy_*yt_-zx_*xt_*yy_)/det;

      break; }
   default:
    mbl_exception_error(mbl_exception_abort(
      vul_sprintf("vimt3d_transform_3d::calcInverse() Unexpected form: %d", form_) ));
  }

  inv_uptodate_=true;
}

//=======================================================================

bool vimt3d_transform_3d::operator==( const vimt3d_transform_3d& t) const
{
  return
    xx_ == t.xx_ &&
    yy_ == t.yy_ &&
    zz_ == t.zz_ &&
    tt_ == t.tt_ &&
    xy_ == t.xy_ &&
    xz_ == t.xz_ &&
    xt_ == t.xt_ &&
    yx_ == t.yx_ &&
    yz_ == t.yz_ &&
    yt_ == t.yt_ &&
    zx_ == t.zx_ &&
    zy_ == t.zy_ &&
    zt_ == t.zt_ &&
    tx_ == t.tx_ &&
    ty_ == t.ty_ &&
    tz_ == t.tz_ ;
}

//=======================================================================
//: Calculates the product LR
// \param L  Transform
// \param R  Transform
// \return Transform LR = R followed by L
vimt3d_transform_3d operator*(const vimt3d_transform_3d& L, const vimt3d_transform_3d& R)
{
  vimt3d_transform_3d T;

  if (L.form() == vimt3d_transform_3d::Identity)
    return R;
  else
  if (R.form() == vimt3d_transform_3d::Identity)
    return L;
  else
  {
  /// full multiplication - inefficient but works for
  // arbitrary 4*4 transformation matrix
    T.xx_ = L.xx_*R.xx_ + L.xy_*R.yx_+ L.xz_*R.zx_ + L.xt_*R.tx_;
    T.xy_ = L.xx_*R.xy_ + L.xy_*R.yy_+ L.xz_*R.zy_ + L.xt_*R.ty_;
    T.xz_ = L.xx_*R.xz_ + L.xy_*R.yz_+ L.xz_*R.zz_ + L.xt_*R.tz_;
    T.xt_ = L.xx_*R.xt_ + L.xy_*R.yt_+ L.xz_*R.zt_ + L.xt_*R.tt_;

    T.yx_ = L.yx_*R.xx_ + L.yy_*R.yx_+ L.yz_*R.zx_ + L.yt_*R.tx_;
    T.yy_ = L.yx_*R.xy_ + L.yy_*R.yy_+ L.yz_*R.zy_ + L.yt_*R.ty_;
    T.yz_ = L.yx_*R.xz_ + L.yy_*R.yz_+ L.yz_*R.zz_ + L.yt_*R.tz_;
    T.yt_ = L.yx_*R.xt_ + L.yy_*R.yt_+ L.yz_*R.zt_ + L.yt_*R.tt_;

    T.zx_ = L.zx_*R.xx_ + L.zy_*R.yx_+ L.zz_*R.zx_ + L.zt_*R.tx_;
    T.zy_ = L.zx_*R.xy_ + L.zy_*R.yy_+ L.zz_*R.zy_ + L.zt_*R.ty_;
    T.zz_ = L.zx_*R.xz_ + L.zy_*R.yz_+ L.zz_*R.zz_ + L.zt_*R.tz_;
    T.zt_ = L.zx_*R.xt_ + L.zy_*R.yt_+ L.zz_*R.zt_ + L.zt_*R.tt_;

    T.tx_ = L.tx_*R.xx_ + L.ty_*R.yx_+ L.tz_*R.zx_ + L.tt_*R.tx_;
    T.ty_ = L.tx_*R.xy_ + L.ty_*R.yy_+ L.tz_*R.zy_ + L.tt_*R.ty_;
    T.tz_ = L.tx_*R.xz_ + L.ty_*R.yz_+ L.tz_*R.zz_ + L.tt_*R.tz_;
    T.tt_ = L.tx_*R.xt_ + L.ty_*R.yt_+ L.tz_*R.zt_ + L.tt_*R.tt_;

    // now set the type using the type of L and R
    // not sure this right - kds
    if (R.form() == L.form())
      T.form_ = R.form();
    else
    {
      if (R.form() == vimt3d_transform_3d::Affine ||
          L.form() == vimt3d_transform_3d::Affine)
          T.form_ = vimt3d_transform_3d::Affine;
      else
      if (R.form() == vimt3d_transform_3d::Similarity ||
          L.form() == vimt3d_transform_3d::Similarity)
          T.form_ = vimt3d_transform_3d::Similarity;
      else
      if (R.form() == vimt3d_transform_3d::RigidBody ||
          L.form() == vimt3d_transform_3d::RigidBody)
      {
        if (R.form() == vimt3d_transform_3d::ZoomOnly)
        {
          if (R.xx_ == R.yy_ &&
              R.xx_ == R.zz_)
            T.form_ = vimt3d_transform_3d::Similarity;
          else
            T.form_ = vimt3d_transform_3d::Affine;
        }
        else
        if (L.form() == vimt3d_transform_3d::ZoomOnly)
        {
          if (L.xx_ == L.yy_ &&
              L.xx_ == L.zz_)
            T.form_ = vimt3d_transform_3d::Similarity;
          else
            T.form_ = vimt3d_transform_3d::Affine;
        }
        else
          T.form_ = vimt3d_transform_3d::RigidBody;
      }
      else
      if (R.form() == vimt3d_transform_3d::ZoomOnly ||
          L.form() == vimt3d_transform_3d::ZoomOnly)
          T.form_ = vimt3d_transform_3d::ZoomOnly;
      else
        T.form_ = vimt3d_transform_3d::Translation;
    }
  }

  T.inv_uptodate_ = false;

  return T;
}

//=======================================================================
void vimt3d_transform_3d::print_summary(std::ostream& o) const
{
  o << vsl_indent()<< "Form: ";
  vsl_indent_inc(o);
  switch (form_)
  {
   case Identity:
    o << "Identity\n";
    break;

   case Translation: {
    vnl_vector<double> p(3);
    params(p);
    o << "Translation (" << p(0) << ',' << p(1) << ',' << p(2) << ")\n";
    break; }

   case ZoomOnly: {
    vnl_vector<double> p(6);
    params(p);
    o << "ZoomOnly\n"
      << vsl_indent()<< "scale factor = (" << p(0) << ',' << p(1) << ',' << p(2) << ")\n"
      << vsl_indent() << "translation = (" << p(3) << ',' << p(4) << ',' << p(5) << ")\n";
    break; }

   case RigidBody: {
    vnl_vector<double> p(6);
    params(p);
    o << "RigidBody\n"
      << vsl_indent()<< "angles = " << p(0) << ',' << p(1) << ',' << p(2) << '\n'
      << vsl_indent()<< "translation = (" << p(3) << ',' << p(4) << ',' << p(5) << ")\n";
    break; }

   case Similarity: {
    vnl_vector<double> p(7);
    params(p);
    // not sure this is right - kds
    // returns euler angles of rotation
    // which might not be same as rotations specified by set command
    // cos euler angles not unique
    o << "Similarity\n"
      << vsl_indent()<< "scale factor = " << p(0) << '\n'
      << vsl_indent()<< "angles = " << p(1) << ',' << p(2) << ',' << p(3) << '\n'
      << vsl_indent()<< "translation = (" << p(4) << ',' << p(5) << ',' << p(6) << ")\n";
    break; }
   case Affine: {
    vnl_vector<double> p(9);
    params(p);
    // not sure this is right - kds
    // params(p) call is broken for affine
    // only works if sx=sy=sz in set command
    o << "Affine\n"
      << vsl_indent()<< "scale factors = " << p(0) << ',' << p(1) << ',' << p(2) << '\n'
      << vsl_indent()<< "angles = " << p(3) << ',' << p(4) << ',' << p(5) << '\n'
      << vsl_indent()<< "translation = (" << p(6) << ',' << p(7) << ',' << p(8) << ")\n";
    break; }
   default:
    mbl_exception_error(mbl_exception_abort(
      vul_sprintf("vimt3d_transform_3d::print_summary() Unexpected form: %d", form_) ));
  }
  vsl_indent_dec(o);
}

//=======================================================================
// Print class to os
void vimt3d_transform_3d::print_all(std::ostream& os) const
{
  os << vsl_indent() << "Form: ";
  switch (form_)
  {
   case Identity:
    os << "Identity\n";
    break;

   case Translation:
    os << "Translation\n";
    break;

   case ZoomOnly:
    os << "ZoomOnly\n";
    break;

   case RigidBody:
    os << "RigidBody\n";
    break;

   case Similarity:
    os << "Similarity\n";
    break;

   case Affine:
    os << "Affine\n";
    break;
   default:
    mbl_exception_error(mbl_exception_abort(
      vul_sprintf("vimt3d_transform_3d::print_all() Unexpected form: %d", form_) ));
  }

  os << vsl_indent() << "Matrix:\n";
  vsl_indent_inc(os);
  os << vsl_indent() << xx_ << ' ' << xy_ << ' ' << xz_ << ' ' << xt_ << '\n'
     << vsl_indent() << yx_ << ' ' << yy_ << ' ' << yz_ << ' ' << yt_ << '\n'
     << vsl_indent() << zx_ << ' ' << zy_ << ' ' << zz_ << ' ' << zt_ << '\n'
     << vsl_indent() << tx_ << ' ' << ty_ << ' ' << tz_ << ' ' << tt_ << '\n';
  vsl_indent_dec(os);
}


//=======================================================================
void vimt3d_transform_3d::config(std::istream& is)
{
  mbl_read_props_type props = mbl_read_props_ws(is);
  std::string form = props.get_required_property("form");
  vul_string_downcase(form);

  bool done=false;

  if (form == "identity")
  {
    done=true;
    form_ = Identity;
  }
  else if (form == "translation")
    form_ = Translation;
  else if (form == "zoomonly")
    form_ = ZoomOnly;
  else if (form == "rigidbody")
    form_ = RigidBody;
  else if (form == "similarity")
    form_ = Similarity;
  else if (form == "affine")
    form_ = Affine;
  else
    throw mbl_exception_parse_error("Unknown transformation: \"" + form + "\"");

  std::string vector = props.get_optional_property("vector");
  if (!vector.empty())
  {
    std::istringstream ss(vector);

    std::vector<double> vec1;
    mbl_parse_sequence(ss, std::back_inserter(vec1), double());
    if (vec1.empty())
      throw mbl_exception_parse_error("Could not find elements for transformation vector: \""+vector+"\"");
    vnl_vector<double> vec2(&vec1.front(), vec1.size());
    try
    { // translate exception abort into parse error.
      this->set(vec2, form_);
    }
    catch (mbl_exception_abort & e)
    {
      throw mbl_exception_parse_error(e.what());
    }
    done = true;
  }
  if (!done && form_==Translation)
  {
    this->set_translation(
      vul_string_atof(props.get_required_property("t_x")),
      vul_string_atof(props.get_required_property("t_y")),
      vul_string_atof(props.get_required_property("t_z")) );
    done = true;
  }
  if (!done && form_==ZoomOnly)
  {
    std::string s_str = props.get_optional_property("s");
    if (!s_str.empty())
      this->set_zoom_only(
        vul_string_atof(s_str),
        vul_string_atof(props.get_optional_property("t_x")),
        vul_string_atof(props.get_optional_property("t_y")),
        vul_string_atof(props.get_optional_property("t_z")) );
    else
      this->set_zoom_only(
        vul_string_atof(props.get_optional_property("s_x")),
        vul_string_atof(props.get_optional_property("s_y")),
        vul_string_atof(props.get_optional_property("s_z")),
        vul_string_atof(props.get_optional_property("t_x")),
        vul_string_atof(props.get_optional_property("t_y")),
        vul_string_atof(props.get_optional_property("t_z")) );
    done = true;
  }
  if (!done && form_==RigidBody)
  {
    set_rigid_body(
      vul_string_atof(props.get_optional_property("r_x")),
      vul_string_atof(props.get_optional_property("r_y")),
      vul_string_atof(props.get_optional_property("r_z")),
      vul_string_atof(props.get_optional_property("t_x")),
      vul_string_atof(props.get_optional_property("t_y")),
      vul_string_atof(props.get_optional_property("t_z")) );
    done = true;
  }

  if (!done) throw mbl_exception_parse_error("Not enough transformation values specified");

  mbl_read_props_look_for_unused_props(
    "vimt3d_transform_3d::config", props, mbl_read_props_type());
  return;
}


//=======================================================================

void vimt3d_transform_3d::b_write(vsl_b_ostream& bfs) const
{
  constexpr short version_no = 2;
  vsl_b_write(bfs,version_no);
  vsl_b_write(bfs,int(form_));
  vsl_b_write(bfs,xx_); vsl_b_write(bfs,xy_); vsl_b_write(bfs,xz_); vsl_b_write(bfs,xt_);
  vsl_b_write(bfs,yx_); vsl_b_write(bfs,yy_); vsl_b_write(bfs,yz_); vsl_b_write(bfs,yt_);
  vsl_b_write(bfs,zx_); vsl_b_write(bfs,zy_); vsl_b_write(bfs,zz_); vsl_b_write(bfs,zt_);
  vsl_b_write(bfs,tx_); vsl_b_write(bfs,ty_); vsl_b_write(bfs,tz_); vsl_b_write(bfs,tt_);
}

//=======================================================================

void vimt3d_transform_3d::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  int f;
  switch (version)
  {
   case 1:
    vsl_b_read(bfs,f);
    if (f==0) // Old Form enum had "Undefined" as the first value. It is never needed, and so was removed.
      set_identity();
    else
    {
      form_=Form(f-1);
      vsl_b_read(bfs,xx_); vsl_b_read(bfs,xy_); vsl_b_read(bfs,xz_); vsl_b_read(bfs,xt_);
      vsl_b_read(bfs,yx_); vsl_b_read(bfs,yy_); vsl_b_read(bfs,yz_); vsl_b_read(bfs,yt_);
      vsl_b_read(bfs,zx_); vsl_b_read(bfs,zy_); vsl_b_read(bfs,zz_); vsl_b_read(bfs,zt_);
      vsl_b_read(bfs,tx_); vsl_b_read(bfs,ty_); vsl_b_read(bfs,tz_); vsl_b_read(bfs,tt_);
    }
    break;
   case 2:
    vsl_b_read(bfs,f); form_=Form(f);
    vsl_b_read(bfs,xx_); vsl_b_read(bfs,xy_); vsl_b_read(bfs,xz_); vsl_b_read(bfs,xt_);
    vsl_b_read(bfs,yx_); vsl_b_read(bfs,yy_); vsl_b_read(bfs,yz_); vsl_b_read(bfs,yt_);
    vsl_b_read(bfs,zx_); vsl_b_read(bfs,zy_); vsl_b_read(bfs,zz_); vsl_b_read(bfs,zt_);
    vsl_b_read(bfs,tx_); vsl_b_read(bfs,ty_); vsl_b_read(bfs,tz_); vsl_b_read(bfs,tt_);
    break;
   default:
    std::cerr<<"vimt3d_transform_3d::load : Illegal version number "<<version<<'\n';
    std::abort();
  }

  inv_uptodate_ = false;
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const vimt3d_transform_3d& b)
{
  b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, vimt3d_transform_3d& b)
{
  b.b_read(bfs);
}

//=======================================================================

std::ostream& operator<<(std::ostream& os,const vimt3d_transform_3d& b)
{
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//========================================================================
// Test whether a 3D transform is zoom-only or lesser, i.e. there may
// be translation and (anisotropic) scaling but no rotation.
// This tests only for a commonly-occurring special case; there may
// be other zoom-only transforms that are not detected.
//========================================================================
bool vimt3d_transform_is_zoom_only(const vimt3d_transform_3d& transf,
                                   const double zero_tol/*=1e-9*/)
{
  // Check whether the top-left 3x3 submatrix part of the transform is
  // diagonal with strictly-positive elements. Such cases have zero rotation
  // and positive (possibly anisotropic) scaling.
  vnl_matrix<double> M = transf.matrix().extract(3,3,0,0);

  // Are any diagonal elements zero or negative?
  for (unsigned i=0; i<3; ++i)
    if (M(i,i)<=zero_tol) return false;

  // Are any off-diagonal elements non-zero?
  for (unsigned j=0; j<3; ++j)
  {
    for (unsigned i=0; i<3; ++i)
    {
      if (i!=j && std::fabs(M(i,j))>=zero_tol) return false;
    }
  }

  return true;
}
