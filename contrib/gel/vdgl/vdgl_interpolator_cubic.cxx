// This is gel/vdgl/vdgl_interpolator_cubic.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vdgl_interpolator_cubic.h"
#include <vcl_cassert.h>
#include <vcl_cmath.h> // for sqrt()
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_inverse.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>


vdgl_interpolator_cubic::vdgl_interpolator_cubic(vdgl_edgel_chain_sptr chain)
  : vdgl_interpolator(chain)
{
  recompute_all();
}

vdgl_interpolator_cubic::~vdgl_interpolator_cubic()
{
}


double vdgl_interpolator_cubic::get_x(double index)
{
  const int N = chain_->size();
  int a= int(index)-1;
  if (index == N-2) --a; // take previous interval if we are on edgel N-2
  int b= a+1;
  int c= b+1;
  int d= c+1;
  assert(a >= 0 && d < N);

  vdgl_edgel ae(chain_->edgel(a));
  vdgl_edgel be(chain_->edgel(b));
  vdgl_edgel ce(chain_->edgel(c));
  vdgl_edgel de(chain_->edgel(d));

  vnl_vector_fixed<double,4> A;
  vnl_matrix_fixed<double,4,4> M;

  A(0) = ae.get_x(); A(1) = be.get_x();
  A(2) = ce.get_x(); A(3) = de.get_x();

  M(0,0)=  a*a*a; M(0,1)= a*a; M(0,2)= a; M(0,3)= 1;
  M(1,0)=  b*b*b; M(1,1)= b*b; M(1,2)= b; M(1,3)= 1;
  M(2,0)=  c*c*c; M(2,1)= c*c; M(2,2)= c; M(2,3)= 1;
  M(3,0)=  d*d*d; M(3,1)= d*d; M(3,2)= d; M(3,3)= 1;

  //  Solving A = M * P for P
  vnl_vector_fixed<double,4> P = vnl_inverse(M) * A;

  return P(0) * index * index * index  + P(1) *  index * index + P(2) * index  + P(3);
}


double vdgl_interpolator_cubic::get_y(double index)
{
  const int N = chain_->size();
  int a= int(index)-1;
  if (index == N-2) --a; // take previous interval if we are on edgel N-2
  int b= a+1;
  int c= b+1;
  int d= c+1;
  assert(a >= 0 && d < N);

  vdgl_edgel ae(chain_->edgel(a));
  vdgl_edgel be(chain_->edgel(b));
  vdgl_edgel ce(chain_->edgel(c));
  vdgl_edgel de(chain_->edgel(d));

  vnl_vector_fixed<double,4> A;
  vnl_matrix_fixed<double,4,4> M;

  A(0) = ae.get_y(); A(1) = be.get_y();
  A(2) = ce.get_y(); A(3) = de.get_y();

  M(0,0)=  a*a*a; M(0,1)= b*b; M(0,2)= a; M(0,3)= 1;
  M(1,0)=  b*b*b; M(1,1)= b*b; M(1,2)= b; M(1,3)= 1;
  M(2,0)=  c*c*c; M(2,1)= c*c; M(2,2)= c; M(2,3)= 1;
  M(3,0)=  d*d*d; M(3,1)= d*d; M(3,2)= d; M(3,3)= 1;

  //  Solving A = M * P for P
  vnl_vector_fixed<double,4> P = vnl_inverse(M) * A;

  return P(0) * index * index * index  + P(1) *  index * index + P(2) * index  + P(3);
}

double vdgl_interpolator_cubic::get_grad(double index)
{
  const int N = chain_->size();
  int a= int(index)-1;
  if (index == N-2) --a; // take previous interval if we are on edgel N-2
  int b= a+1;
  int c= b+1;
  int d= c+1;
  assert(a >= 0 && d < N);

  vdgl_edgel ae(chain_->edgel(a));
  vdgl_edgel be(chain_->edgel(b));
  vdgl_edgel ce(chain_->edgel(c));
  vdgl_edgel de(chain_->edgel(d));

  vnl_vector_fixed<double,4> A;
  vnl_matrix_fixed<double,4,4> M;

  A(0) = ae.get_grad(); A(1) = be.get_grad();
  A(2) = ce.get_grad(); A(3) = de.get_grad();

  M(0,0)=  a*a*a; M(0,1)= a*a; M(0,2)= a; M(0,3)= 1;
  M(1,0)=  b*b*b; M(1,1)= b*b; M(1,2)= b; M(1,3)= 1;
  M(2,0)=  c*c*c; M(2,1)= c*c; M(2,2)= c; M(2,3)= 1;
  M(3,0)=  d*d*d; M(3,1)= d*d; M(3,2)= d; M(3,3)= 1;

  //  Solving A = M * P for P
  vnl_vector_fixed<double,4> P = vnl_inverse(M) * A;

  return P(0) * index * index * index  + P(1) *  index * index + P(2) * index + P(3);
}

double vdgl_interpolator_cubic::get_theta(double index)
{
  const int N = chain_->size();
  int a= int(index)-1;
  if (index == N-2) --a; // take previous interval if we are on edgel N-2
  int b= a+1;
  int c= b+1;
  int d= c+1;
  assert(a >= 0 && d < N);

  vdgl_edgel ae(chain_->edgel(a));
  vdgl_edgel be(chain_->edgel(b));
  vdgl_edgel ce(chain_->edgel(c));
  vdgl_edgel de(chain_->edgel(d));

  vnl_vector_fixed<double,4> A;
  vnl_matrix_fixed<double,4,4> M;

  A(0) = ae.get_theta(); A(1) = be.get_theta();
  A(2) = ce.get_theta(); A(3) = de.get_theta();

  M(0,0)=  a*a*a; M(0,1)= a*a; M(0,2)= a; M(0,3)= 1;
  M(1,0)=  b*b*b; M(1,1)= b*b; M(1,2)= b; M(1,3)= 1;
  M(2,0)=  c*c*c; M(2,1)= c*c; M(2,2)= c; M(2,3)= 1;
  M(3,0)=  d*d*d; M(3,1)= d*d; M(3,2)= d; M(3,3)= 1;

  //  Solving A = M * P for P
  vnl_vector_fixed<double,4> P = vnl_inverse(M) * A;

  return P(0) * index * index * index  + P(1) *  index * index + P(2) * index + P(3);
}

//: Compute the angle using four adjacent edgels.
//  This is done by fitting a 3rd degree spline through these 4 points,
//  and calculating the derived point at index.
double vdgl_interpolator_cubic::get_tangent_angle(double index)
{
  const int N = chain_->size();
  int a= int(index)-1;
  if (index == N-2) --a; // take previous interval if we are on edgel N-2
  int b= a+1;
  int c= b+1;
  int d= c+1;
  assert(a >= 0 && d < N);

  vdgl_edgel ae = chain_->edgel(a),
             be = chain_->edgel(b),
             ce = chain_->edgel(c),
             de = chain_->edgel(d);

  double xa = ae.x(), ya = ae.y(),
         xb = be.x(), yb = be.y(),
         xc = ce.x(), yc = ce.y(),
         xd = de.x(), yd = de.y(),

         x2 = xd-4.5*xc+9*xb-5.5*xa,
         y2 = yd-4.5*yc+9*yb-5.5*ya,
         x1 = -x2-0.5*xc+4*xb-3.5*xa,
         y1 = -y2-0.5*yc+4*yb-3.5*ya,
         x0 = x2/2+0.75*xc-3*xb+2.25*xa,
         y0 = y2/2+0.75*yc-3*yb+2.25*ya,

         xp = x0*index*index + x1*index + x2/3, // derived point at index
         yp = y0*index*index + y1*index + y2/3;

  return 180.0*vnl_math::one_over_pi*vcl_atan2(yp, xp);
}


double vdgl_interpolator_cubic::get_curvature(double index)
{
  const int N = chain_->size();
  int a= int(index)-1;
  if (index == N-2) --a; // take previous interval if we are on edgel N-2
  int b= a+1;
  int c= b+1;
  int d= c+1;
  assert(a >= 0 && d < N);

  vdgl_edgel ae(chain_->edgel(a));
  vdgl_edgel be(chain_->edgel(b));
  vdgl_edgel ce(chain_->edgel(c));
  vdgl_edgel de(chain_->edgel(d));

  vnl_vector_fixed<double,4> A;
  vnl_matrix_fixed<double,4,4> M;

  A(0) = ae.get_x(); A(1) = be.get_x();
  A(2) = ce.get_x(); A(3) = de.get_x();

  M(0,0)=  a*a*a; M(0,1)= a*a; M(0,2)= a; M(0,3)= 1;
  M(1,0)=  b*b*b; M(1,1)= b*b; M(1,2)= b; M(1,3)= 1;
  M(2,0)=  c*c*c; M(2,1)= c*c; M(2,2)= c; M(2,3)= 1;
  M(3,0)=  d*d*d; M(3,1)= d*d; M(3,2)= d; M(3,3)= 1;

  //  Solving A = M * P for P
  vnl_vector_fixed<double,4> P = vnl_inverse(M) * A;

  double x_new= P(0) * index * index * index  + P(1) *  index * index + P(2) * index  + P(3);

  double t2 = 3 * P(0) * x_new * x_new + 2 * P(1) * x_new + P(2);
  double t3 = 1 + t2 * t2;
  return t2/t3/vcl_sqrt(t3);
}


double vdgl_interpolator_cubic::get_length()
{
  // length is cached (because it's expensive to compute)
  if ( older(chain_.ptr()) )
    recompute_all();

  return lengthcache_;
}


double vdgl_interpolator_cubic::get_min_x()
{
  if ( older(chain_.ptr()) )
    recompute_all();

  return minxcache_;
}


double vdgl_interpolator_cubic::get_max_x()
{
  if ( older(chain_.ptr()) )
    recompute_all();

  return maxxcache_;
}


double vdgl_interpolator_cubic::get_min_y()
{
  if ( older(chain_.ptr()) )
    recompute_all();

  return minycache_;
}


double vdgl_interpolator_cubic::get_max_y()
{
  if ( older(chain_.ptr()) )
    recompute_all();

  return maxycache_;
}

// cache maintenance

void vdgl_interpolator_cubic::recompute_all()
{
  recompute_length();
  recompute_bbox();

  touch();
}

void vdgl_interpolator_cubic::recompute_length()
{
  const int N = chain_->size();
  lengthcache_= 0;
  if (N <= 1) return;

  for (int i=N-1, j=i-1; j>=0; --i,--j)
  {
    vgl_point_2d<double> p1= chain_->edgel(j).get_pt();
    vgl_point_2d<double> p2= chain_->edgel(i).get_pt();

    lengthcache_ += length(p2-p1);
  }
}

void vdgl_interpolator_cubic::recompute_bbox()
{
  const int N = chain_->size();
  if (N == 0) return;
  minxcache_= chain_->edgel(0).get_x();
  maxxcache_= chain_->edgel(0).get_x();
  minycache_= chain_->edgel(0).get_y();
  maxycache_= chain_->edgel(0).get_y();

  for (int i=1; i< N; i++)
  {
    if (chain_->edgel(i).get_x()< minxcache_) minxcache_= chain_->edgel(i).get_x();
    if (chain_->edgel(i).get_x()> maxxcache_) maxxcache_= chain_->edgel(i).get_x();
    if (chain_->edgel(i).get_y()< minycache_) minycache_= chain_->edgel(i).get_y();
    if (chain_->edgel(i).get_y()> maxycache_) maxycache_= chain_->edgel(i).get_y();
  }
}
