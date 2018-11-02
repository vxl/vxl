#include <iostream>
#include "icam_transform_2d.h"
//:
// \file
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_inverse.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vgl_point_2d<double> icam_transform_2d::origin() const
{
  return {t12_matrix_[0][2]/t12_matrix_[2][2],
                              t12_matrix_[1][2]/t12_matrix_[2][2]};
}

void icam_transform_2d::set_origin(vgl_point_2d<double> const& p)
{
    t12_matrix_[0][2] = p.x()*t12_matrix_[2][2];
    t12_matrix_[1][2] = p.y()*t12_matrix_[2][2];
    if (form_ == Identity) form_=Translation;
}

void icam_transform_2d::set_projective(vnl_double_3x3 const& m)
{
  t12_matrix_=m;
  form_ = Projective;
}

void icam_transform_2d::set_zoom_only(double s_x, double s_y, double t_x, double t_y)
{
  set_identity();
  t12_matrix_[0][0]=s_x;
  t12_matrix_[1][1]=s_y;
  set_translation(t_x,t_y);
  form_ = Affine;
}

void icam_transform_2d::params_of(vnl_vector<double>& v, Form form) const
{
  switch (form)
  {
   case (Identity):
    v.set_size(0);
    break;
   case (Translation):
    v.set_size(2);
    v(0)=t12_matrix_[0][2]; v(1)=t12_matrix_[1][2];
    break;
   case (RigidBody):
    v.set_size(3);
    v(0)=std::atan2(-t12_matrix_[0][1],t12_matrix_[0][0]); // Angle
    v(1)=t12_matrix_[0][2]; v(2)=t12_matrix_[1][2];
    break;
   case (Affine):
    v.set_size(6);
    v(0)=t12_matrix_[0][0]; v(1)=t12_matrix_[0][1]; v(2)=t12_matrix_[0][2];
    v(3)=t12_matrix_[1][0]; v(4)=t12_matrix_[1][1]; v(5)=t12_matrix_[1][2];
    break;
   case (Projective):
    v.set_size(9);
    v(0)=t12_matrix_[0][0]; v(1)=t12_matrix_[0][1]; v(2)=t12_matrix_[0][2];
    v(3)=t12_matrix_[1][0]; v(4)=t12_matrix_[1][1]; v(5)=t12_matrix_[1][2];
    v(6)=t12_matrix_[2][0]; v(7)=t12_matrix_[2][1]; v(8)=t12_matrix_[2][2];
    break;
   default:
    std::cerr<<"icam_transform_2d::params() Unexpected form: "<<int(form)<<'\n';
  }
}

void icam_transform_2d::set(vnl_vector<double> const& v, Form form)
{
  int n=v.size();
  switch (form)
  {
   case (Identity):
    set_identity();
    break;
   case (Translation):
    if (n<2) return;
    set_translation(v(0),v(1));
    break;
   case (RigidBody):
    if (n<3) return;
    set_rigid_body(v(0),v(1),v(2));
    break;
   case (Affine):
    if (n<6) return;
    vgl_h_matrix_2d<double>::set_affine(vnl_matrix<double>(2,3,6,v.begin()));
    form_ = Affine;
    break;
   case (Projective):
    if (n<9) return;
    vgl_h_matrix_2d<double>::set(vnl_matrix_fixed<double,3,3>(v.begin()));
    form_ = Projective;
    break;
   default:
    std::cerr<<"icam_transform_2d::set() Unexpected form: "<<int(form)<<'\n';
  }
}

void icam_transform_2d::set_rigid_body(double theta, double t_x, double t_y)
{
  if (theta==0.0)
    set_translation(t_x,t_y);
  else {
    set_rotation(theta);
    set_translation(t_x,t_y);
  }
  form_=RigidBody;
}

//: Sets to be 2D affine transformation T(x,y)=p+x.u+y.v
void icam_transform_2d::set_affine(vgl_point_2d<double> const& p,
                                   vgl_vector_2d<double> const& u,
                                   vgl_vector_2d<double> const& v)
{
  t12_matrix_[0][2] = p.x();
  t12_matrix_[1][2] = p.y();
  t12_matrix_[0][0] = u.x();
  t12_matrix_[1][0] = u.y();
  t12_matrix_[0][1] = v.x();
  t12_matrix_[1][1] = v.y();
  form_=Affine;
}

//: Sets to be 2D affine transformation using 2x3 matrix
void icam_transform_2d::set_affine(vnl_double_2x3 const& M23)
{
  if (M23(0,0)*M23(1,1) < M23(0,1)*M23(1,0)) {
    std::cerr << "icam_transform_2d::set_affine:\n"
             << "sub (2x2) matrix should have positive determinant\n";
  }
  vgl_h_matrix_2d<double>::set_affine(M23);
  form_=Affine;
}

vgl_vector_2d<double>
icam_transform_2d::delta(vgl_point_2d<double> const& p, vgl_vector_2d<double> const& dp) const
{
  switch (form_)
  {
   case Identity :
   case Translation:
    return dp;
   case RigidBody :
   case Affine :
    return {dp.x()*t12_matrix_[0][0]+dp.y()*t12_matrix_[0][1],
                                 dp.x()*t12_matrix_[1][0]+dp.y()*t12_matrix_[1][1]};
   case Projective :
    return operator()(p+dp)-operator()(p);
   default:
    std::cerr<<"icam_transform_2d::delta() : Unrecognised form: "<<int(form_)<<'\n';
    return {};
  }
}

icam_transform_2d icam_transform_2d::inverse() const
{
  return icam_transform_2d(vnl_inverse(t12_matrix_),form_);
}

vgl_point_2d<double> icam_transform_2d::operator()(double x, double y) const
{
  switch (form_)
  {
   case Identity :
    return {x,y};
   case Translation :
    return {x+t12_matrix_[0][2],y+t12_matrix_[1][2]};
   case RigidBody :
   case Affine :
    return vgl_point_2d<double>(x*t12_matrix_[0][0]+y*t12_matrix_[0][1]+t12_matrix_[0][2],
                                x*t12_matrix_[1][0]+y*t12_matrix_[1][1]+t12_matrix_[1][2]);
   case Projective : {
    double z=x*t12_matrix_[2][0]+y*t12_matrix_[2][1]+t12_matrix_[2][2];
    if (z==0) return vgl_point_2d<double>(1e99,1e99);
    else  return vgl_point_2d<double>((x*t12_matrix_[0][0]+y*t12_matrix_[0][1]+t12_matrix_[0][2])/z,
                                      (x*t12_matrix_[1][0]+y*t12_matrix_[1][1]+t12_matrix_[1][2])/z);
   }
   default:
    std::cerr<<"icam_transform_2d::operator() : Unrecognised form: "<<int(form_)<<'\n';
    return vgl_point_2d<double>();
  }
}
