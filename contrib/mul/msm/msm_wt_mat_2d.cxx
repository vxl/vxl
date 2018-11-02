#include <iostream>
#include <cmath>
#include "msm_wt_mat_2d.h"
//:
// \file
// \brief Represents 2x2 symmetric matrix, used as weight matrix
// \author Tim Cootes

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Sets axis (eigenvector) of matrix and var along each
//  Sets to s1*u*u' + s2*v*v', where u is the unit vector
//  (u1 u2)/|u1,u2|, and v is the unit vector orthogonal to
//  this.  u is then an eigenvector with associated eigenvalue s1,
//  v is the other eigenvector, with eigenvalue s2
void msm_wt_mat_2d::set_axes(double u1, double u2, double s1, double s2)
{
  double uu = u1*u1, uv=u1*u2, vv=u2*u2;
  double L2=uu+vv;
  uu/=L2; uv/=L2; vv/=L2;  // Normalise to unit.
  // uu' = {uu uv; uv vv},  vv'={vv -uv; -uv uu}
  m11_ = s1*uu + s2*vv;
  m12_ = (s1-s2)*uv;
  m22_ = s1*vv + s2*uu;
}

//: Returns inverse (or pseudo-inverse)
msm_wt_mat_2d msm_wt_mat_2d::inverse() const
{
  double D=det();
  if (std::fabs(D)>1e-8)
    return msm_wt_mat_2d(m22_/D,-m12_/D,m11_/D);

  // Small det implies this = (u v)'*(u v)
  // Inverse can be shown to be this/(u^2+v^2) = this/(m11_+m22_)
  double s=1.0/(m11_+m22_);
  return msm_wt_mat_2d(s*m11_,s*std::sqrt(m11_*m22_),s*m22_);
}

//: Calculate eigenvalues
void msm_wt_mat_2d::eigen_values(double& EV1, double& EV2)
{
  double dac=m11_-m22_;
  double d=0.5*std::sqrt(dac*dac+4*m12_*m12_);
  double hac = 0.5*(m11_+m22_);
  EV1=hac+d;
  EV2=hac-d;
}

//: Calculate eigenvector associated with largest eigenvalue.
//  Other evec given by (-evec1.y(),evec1.x())
void msm_wt_mat_2d::eigen_vector(vgl_vector_2d<double>& evec1, double& eval1, double& eval2)
{
  double dac=m11_-m22_;
  double d=0.5*std::sqrt(dac*dac+4*m12_*m12_);
  double hac = 0.5*(m11_+m22_);
  eval1=hac+d;
  eval2=hac-d;
  evec1=vgl_vector_2d<double>(m12_,eval1-m11_);
  double L=evec1.length();
  if (L>1e-6) evec1/=L;
  else evec1=vgl_vector_2d<double>(1,0);
}


//: Calculates W2=T'WT where T is 2x2 matrix (a,-b;b,a)
msm_wt_mat_2d msm_wt_mat_2d::transform_by(double a, double b) const
{
  double a2=a*a,ab=a*b,b2=b*b;
  return msm_wt_mat_2d(a2*m11_+2*ab*m12_+b2*m22_,
                       (a2-b2)*m12_+ab*(m22_-m11_),
                       a2*m22_-2*ab*m12_+b2*m11_);
}

//: Post multiply by W
msm_wt_mat_2d msm_wt_mat_2d::operator*(msm_wt_mat_2d& W) const
{
  return msm_wt_mat_2d(m11_*W.m11()+m12_*W.m21(),
                       m11_*W.m21()+m12_*W.m22(),
                       m12_*W.m21()+m22_*W.m22());
}

//: Multiply this by scalar
msm_wt_mat_2d& msm_wt_mat_2d::operator*=(double s)
{
  m11_*=s;
  m12_*=s;
  m22_*=s;
  return *this;
}

//: Add W to this
msm_wt_mat_2d& msm_wt_mat_2d::operator+=(const msm_wt_mat_2d& W)
{
  m11_+=W.m11_;
  m12_+=W.m12_;
  m22_+=W.m22_;
  return *this;
}

//: Equality test
bool msm_wt_mat_2d::operator==(const msm_wt_mat_2d& W)
{
  return (std::fabs(m11_-W.m11_)<1e-8) &&
         (std::fabs(m12_-W.m12_)<1e-8) &&
         (std::fabs(m22_-W.m22_)<1e-8);
}


//=======================================================================
// Method: print
//=======================================================================

void msm_wt_mat_2d::print_summary(std::ostream& os) const
{
  os << "{ "<<m11_<<' '<<m12_<<" ; "<<m12_<<' '<<m22_<<" } ";
}

//=======================================================================
// Method: save
//=======================================================================
void msm_wt_mat_2d::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,m11_);
  vsl_b_write(bfs,m12_);
  vsl_b_write(bfs,m22_);
}

//=======================================================================
// Method: load
//=======================================================================
void msm_wt_mat_2d::b_read(vsl_b_istream& bfs)
{
  vsl_b_read(bfs,m11_);
  vsl_b_read(bfs,m12_);
  vsl_b_read(bfs,m22_);
}


//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const msm_wt_mat_2d& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, msm_wt_mat_2d& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const msm_wt_mat_2d& b)
{
  b.print_summary(os);
  return os;
}

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_wt_mat_2d& b)
{
 os << b;
}
