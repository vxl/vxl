// This is gel/vdgl/vdgl_interpolator_linear.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vdgl_interpolator_linear.h"
#include <vdgl/vdgl_edgel_chain.h>
#include <vsol/vsol_point_2d.h>
#include <vcl_cmath.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vcl_cassert.h>


vdgl_interpolator_linear::vdgl_interpolator_linear( vdgl_edgel_chain_sptr chain)
  : vdgl_interpolator( chain)
{
  recompute_all();
}

vdgl_interpolator_linear::~vdgl_interpolator_linear()
{
}

vsol_point_2d_sptr vdgl_interpolator_linear::closest_point_on_curve ( vsol_point_2d_sptr p )
{
  double min_distance = -1.0;
  int index = -1;
  for ( unsigned int i=0; i< chain_->size(); ++i)
  {
    vgl_point_2d<double> curve_point = chain_->edgel(i).get_pt();
    double d = p->distance ( vsol_point_2d ( curve_point ) );
    if ( min_distance < 0 || d < min_distance )
    {
      index = i;
      min_distance = d;
    }
  }
  if ( index == -1 )
    return NULL;
  else
    return new vsol_point_2d ( chain_->edgel(index).get_pt() );
}


double vdgl_interpolator_linear::get_x( const double index)
{
  assert(index >= 0 && index < chain_->size());
  int a = int(index); // round down
  double d = index- a;

  double ae = chain_->edgel(a).get_x();
  if (d==0) return ae; // exactly at an edgel
  double be = chain_->edgel(a+1).get_x();

  return be*d+ae*(1-d);
}


double vdgl_interpolator_linear::get_y( const double index)
{
  assert(index >= 0 && index < chain_->size());
  int a = int(index); // round down
  double d = index- a;

  double ae = chain_->edgel(a).get_y();
  if (d==0) return ae; // exactly at an edgel
  double be = chain_->edgel(a+1).get_y();

  return be*d+ae*(1-d);
}

//: linearly interpolate the gradient magnitude
//
double vdgl_interpolator_linear::get_grad( const double index)
{
  assert(index >= 0 && index < chain_->size());
  int a = int(index); // round down
  double d = index-a;

  double ae = chain_->edgel(a).get_grad();
  if (d==0) return ae; // exactly at an edgel
  double be = chain_->edgel(a+1).get_grad();

  return be*d+ae*(1-d);
}

//: linearly interpolate the gradient direction
//
double vdgl_interpolator_linear::get_theta( const double index)
{
  assert(index >= 0 && index < chain_->size());
  int a = int(index); // round down
  double d = index-a;

  double ae = chain_->edgel(a).get_theta();
  if (d==0) return ae; // exactly at an edgel
  double be = chain_->edgel(a+1).get_theta();

  return be*d+ae*(1-d);
}

//: Compute the angle using two adjacent edgels
//  (TargetJr used different computations at internal points and at endpoints
//  For endpoints the geometric tangent was used, but image gradient directions
//  were used for internal points on the chain.)
//  Here we use direct geometric angle computation for all points
//  The image-based directions are likely less accurate
//
double vdgl_interpolator_linear::get_tangent_angle( const double index)
{
  int N = chain_->size();
  assert(index >= 0 && index <= chain_->size() - 1);
  if (N==1)
  {
    vcl_cout << " vdgl_interpolator_linear::get_theta(..) -"
             << " can't compute angle for a chain with one edgel\n";
    return 0;
  }
  int a = int(index); // round down
  // if index is exactly at N-1, a+1 is invalid, so take the preceding interval:
  if (a == N-1) --a;
  assert(a>=0 && a<N-1); // just in case... this should not happen.
  double xi = (*chain_)[a].x(), yi = (*chain_)[a].y();
  double xip = (*chain_)[a+1].x(), yip = (*chain_)[a+1].y();
  double angle = 180.0*vnl_math::one_over_pi*vcl_atan2((yip-yi), (xip-xi));
  return angle;
}


double vdgl_interpolator_linear::get_curvature( const double index)
{
  int a = int(index); // round down

  if ( a == index ) // if exactly at an edgel, curvature is undefined
    return vnl_numeric_traits<double>::maxval;
  else
    return 0; // curvature of straight line segments is always zero
}


double vdgl_interpolator_linear::get_length()
{
  // length is cached (because it's expensive to compute)
  if ( older( chain_.ptr()))
    recompute_all();

  return lengthcache_;
}


double vdgl_interpolator_linear::get_min_x()
{
  if ( older( chain_.ptr()))
    recompute_all();

  return minxcache_;
}


double vdgl_interpolator_linear::get_max_x()
{
  if ( older( chain_.ptr()))
    recompute_all();

  return maxxcache_;
}


double vdgl_interpolator_linear::get_min_y()
{
  if ( older( chain_.ptr()))
    recompute_all();

  return minycache_;
}


double vdgl_interpolator_linear::get_max_y()
{
  if ( older( chain_.ptr()))
    recompute_all();

  return maxycache_;
}

// cache maintenance

void vdgl_interpolator_linear::recompute_all()
{
  recompute_length();
  recompute_bbox();

  touch();
}

void vdgl_interpolator_linear::recompute_length()
{
  lengthcache_= 0;

  for ( unsigned int i=0; i< chain_->size(); ++i)
  {
    unsigned int j = i==0 ? chain_->size()-1 : i-1;
    vgl_point_2d<double> p1= chain_->edgel(j).get_pt();
    vgl_point_2d<double> p2= chain_->edgel(i).get_pt();

    lengthcache_ += length(p2-p1);
  }
}

void vdgl_interpolator_linear::recompute_bbox()
{
  if ( chain_->size() == 0 )
  {
    minxcache_=  maxxcache_= minycache_= maxycache_ = 0.0;
    return;
  }
  minxcache_= chain_->edgel( 0).get_x();
  maxxcache_= chain_->edgel( 0).get_x();
  minycache_= chain_->edgel( 0).get_y();
  maxycache_= chain_->edgel( 0).get_y();

  for (unsigned int i=1; i< chain_->size(); ++i)
  {
    if (chain_->edgel(i).get_x()< minxcache_) minxcache_= chain_->edgel(i).get_x();
    if (chain_->edgel(i).get_x()> maxxcache_) maxxcache_= chain_->edgel(i).get_x();
    if (chain_->edgel(i).get_y()< minycache_) minycache_= chain_->edgel(i).get_y();
    if (chain_->edgel(i).get_y()> maxycache_) maxycache_= chain_->edgel(i).get_y();
  }
}
