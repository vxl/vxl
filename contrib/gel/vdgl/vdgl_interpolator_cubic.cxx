// This is gel/vdgl/vdgl_interpolator_cubic.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vdgl_interpolator_cubic.h"
#include <vcl_cmath.h> // for sqrt()
#include <vnl/vnl_matrix.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vnl/algo/vnl_svd.h>


vdgl_interpolator_cubic::vdgl_interpolator_cubic( vdgl_edgel_chain_sptr chain)
  : vdgl_interpolator( chain)
{
  recompute_all();
}

vdgl_interpolator_cubic::~vdgl_interpolator_cubic()
{
}


double vdgl_interpolator_cubic::get_x( const double index)
{
  int a= int( index);
  int b= a+1;
  int c= b+1;
  int d= c+1;

  vdgl_edgel ae( chain_->edgel( a));
  vdgl_edgel be( chain_->edgel( b));
  vdgl_edgel ce( chain_->edgel( c));
  vdgl_edgel de( chain_->edgel( d));


  vnl_matrix<double> A(4,1);
  vnl_matrix<double> M(4,4);

  A(0,0) = ae.get_x(); A(1,0) = be.get_x();
  A(2,0) = ce.get_x(); A(3,0) = de.get_x();

  M(0,0)=  a*a*a; M(0,1)= a*a; M(0,2)= a; M(0,3)= 1;
  M(1,0)=  b*b*b; M(1,1)= b*b; M(1,2)= b; M(1,3)= 1;
  M(2,0)=  c*c*c; M(2,1)= c*c; M(2,2)= c; M(2,3)= 1;
  M(3,0)=  d*d*d; M(3,1)= d*d; M(3,2)= d; M(3,3)= 1;


  vnl_svd<double> svd_of_M(M);

//  Solving A = M * P for P
//  vnl_matrix<double> inv_M(4,4);
//  inv_M = svd.inverse();
//  vnl_matrix<double> P(4,1) = inv_M * A;

  vnl_matrix<double> P = svd_of_M.solve(A);

  return P(0,0) * index * index * index  + P(1,0) *  index * index + P(2,0) * index  + P(3,0);
}


double vdgl_interpolator_cubic::get_y( const double index)
{
  int a= int( index);
  int b= a+1;
  int c= b+1;
  int d= c+1;

  vdgl_edgel ae( chain_->edgel( a));
  vdgl_edgel be( chain_->edgel( b));
  vdgl_edgel ce( chain_->edgel( c));
  vdgl_edgel de( chain_->edgel( d));

  vnl_matrix<double> A(4,1);
  vnl_matrix<double> M(4,4);

  A(0,0) = ae.get_y(); A(1,0) = be.get_y();
  A(2,0) = ce.get_y(); A(3,0) = de.get_y();

  M(0,0)=  a*a*a; M(0,1)= b*b; M(0,2)= a; M(0,3)= 1;
  M(1,0)=  b*b*b; M(1,1)= b*b; M(1,2)= b; M(1,3)= 1;
  M(2,0)=  c*c*c; M(2,1)= c*c; M(2,2)= c; M(2,3)= 1;
  M(3,0)=  d*d*d; M(3,1)= d*d; M(3,2)= d; M(3,3)= 1;

  vnl_svd<double> svd(M);

  vnl_matrix<double> P = svd.solve(A);

  return P(0,0) * index * index * index  + P(1,0) *  index * index + P(2,0) * index  + P(3,0);
}


double vdgl_interpolator_cubic::get_theta( const double index)
{
  int a= int( index);
  int b= a+1;
  int c= b+1;
  int d= c+1;

  vdgl_edgel ae( chain_->edgel( a));
  vdgl_edgel be( chain_->edgel( b));
  vdgl_edgel ce( chain_->edgel( c));
  vdgl_edgel de( chain_->edgel( d));

  vnl_matrix<double> A(4,1);
  vnl_matrix<double> M(4,4);

  A(0,0) = ae.get_theta(); A(1,0) = be.get_theta();
  A(2,0) = ce.get_theta(); A(3,0) = de.get_theta();

  M(0,0)=  a*a*a; M(0,1)= a*a; M(0,2)= a; M(0,3)= 1;
  M(1,0)=  b*b*b; M(1,1)= b*b; M(1,2)= b; M(1,3)= 1;
  M(2,0)=  c*c*c; M(2,1)= c*c; M(2,2)= c; M(2,3)= 1;
  M(3,0)=  d*d*d; M(3,1)= d*d; M(3,2)= d; M(3,3)= 1;

  vnl_svd<double> svd(M);
  vnl_matrix<double> P = svd.solve(A);

  return P(0,0) * index * index * index  + P(1,0) *  index * index + P(2,0) * index  + P(3,0);
}


double vdgl_interpolator_cubic::get_curvature( const double index)
{
  int a= int( index);
  int b= a+1;
  int c= b+1;
  int d= c+1;

  vdgl_edgel ae( chain_->edgel( a));
  vdgl_edgel be( chain_->edgel( b));
  vdgl_edgel ce( chain_->edgel( c));
  vdgl_edgel de( chain_->edgel( d));

  vnl_matrix<double> A(4,1);
  vnl_matrix<double> M(4,4);

  A(0,0) = ae.get_x(); A(1,0) = be.get_x();
  A(2,0) = ce.get_x(); A(3,0) = de.get_x();

  M(0,0)=  a*a*a; M(0,1)= a*a; M(0,2)= a; M(0,3)= 1;
  M(1,0)=  b*b*b; M(1,1)= b*b; M(1,2)= b; M(1,3)= 1;
  M(2,0)=  c*c*c; M(2,1)= c*c; M(2,2)= c; M(2,3)= 1;
  M(3,0)=  d*d*d; M(3,1)= d*d; M(3,2)= d; M(3,3)= 1;

  vnl_svd<double> svd_of_M(M);

  vnl_matrix<double> P = svd_of_M.solve(A);

  double x_new= P(0,0) * index * index * index  + P(1,0) *  index * index + P(2,0) * index  + P(3,0);

  double t2 = 3 * P(0,0) * x_new * x_new + 2 * P(0,1) * x_new + P(0,2);
  double t3 = 1 + t2 * t2;
  return t2/t3/vcl_sqrt(t3);
}


double vdgl_interpolator_cubic::get_length()
{
  // length is cached (because it's expensive to compute)
  if ( older( chain_.ptr()))
    recompute_all();

  return lengthcache_;
}


double vdgl_interpolator_cubic::get_min_x()
{
  if ( older( chain_.ptr()))
    recompute_all();

  return minxcache_;
}


double vdgl_interpolator_cubic::get_max_x()
{
  if ( older( chain_.ptr()))
    recompute_all();

  return maxxcache_;
}


double vdgl_interpolator_cubic::get_min_y()
{
  if ( older( chain_.ptr()))
    recompute_all();

  return minycache_;
}


double vdgl_interpolator_cubic::get_max_y()
{
  if ( older( chain_.ptr()))
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
  lengthcache_= 0;

  for (int i=0; i< chain_->size(); i++)
  {
    int j = i==0 ? chain_->size()-1 : i-1;
    vgl_point_2d<double> p1= chain_->edgel(j).get_pt();
    vgl_point_2d<double> p2= chain_->edgel(i).get_pt();

    // NOTE THERE IS A PROBLEM HERE UNDER WINDOWS
    //   WHICH I WILL HAVE TO FIX AT SOME POINT - FIXME
    // Maybe solved now by using vgl_vector_2d<double> ? - PVr.
    lengthcache_ += length(p2-p1);
  }
}

void vdgl_interpolator_cubic::recompute_bbox()
{
  minxcache_= chain_->edgel( 0).get_x();
  maxxcache_= chain_->edgel( 0).get_x();
  minycache_= chain_->edgel( 0).get_y();
  maxycache_= chain_->edgel( 0).get_y();

  for (int i=1; i< chain_->size(); i++)
    {
      if ( chain_->edgel( i).get_x()< minxcache_) minxcache_= chain_->edgel( i).get_x();
      if ( chain_->edgel( i).get_x()> maxxcache_) maxxcache_= chain_->edgel( i).get_x();
      if ( chain_->edgel( i).get_y()< minycache_) minycache_= chain_->edgel( i).get_y();
      if ( chain_->edgel( i).get_y()> maxycache_) maxycache_= chain_->edgel( i).get_y();
    }
}
