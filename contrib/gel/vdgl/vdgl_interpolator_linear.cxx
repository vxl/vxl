// This is gel/vdgl/vdgl_interpolator_linear.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vdgl_interpolator_linear.h"
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vsol/vsol_point_2d.h>
#include <vnl/vnl_numeric_traits.h>
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
  double min_distance = 1e6;
  int index = -1;
  for ( int i=0; i< chain_->size(); i++)
  {
    vgl_point_2d<double> curve_point = chain_->edgel(i).get_pt();
    double d = p->distance ( vsol_point_2d ( curve_point ) );
    if ( d < min_distance )
    {
      index = i;
      min_distance = d;
    }
  }
  if ( index == -1 )
  {
    return NULL;
  }
  return new vsol_point_2d ( chain_->edgel(index).get_pt() );
}


double vdgl_interpolator_linear::get_x( const double index)
{
  assert(index >= 0 && index < chain_->size());
  int a= int( index); // round down
  double d= index- a;

  double ae = chain_->edgel( a).get_x();
  if (d==0) return ae; // exactly at an edgel
  double be = chain_->edgel( a+1).get_x();

  return be*d+ae*(1-d);
}


double vdgl_interpolator_linear::get_y( const double index)
{
  assert(index >= 0 && index < chain_->size());
  int a= int( index); // round down
  double d= index- a;

  double ae = chain_->edgel( a).get_y();
  if (d==0) return ae; // exactly at an edgel
  double be = chain_->edgel( a+1).get_y();

  return be*d+ae*(1-d);
}


double vdgl_interpolator_linear::get_theta( const double index)
{
  assert(index >= 0 && index < chain_->size());
  int a= int( index); // round down
  double d= index- a;

  double ae = chain_->edgel( a).get_theta();
  if (d==0) return ae; // exactly at an edgel
  double be = chain_->edgel( a+1).get_theta();

  return be*d+ae*(1-d);
}


double vdgl_interpolator_linear::get_curvature( const double index)
{
  int a= int( index); // round down

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

  for ( int i=0; i< chain_->size(); i++)
  {
    int j = i==0 ? chain_->size()-1 : i-1;
    vgl_point_2d<double> p1= chain_->edgel(j).get_pt();
    vgl_point_2d<double> p2= chain_->edgel(i).get_pt();

    // NOTE THERE IS A PROBLEM HERE UNDER WINDOWS
    //   WHICH I WILL HAVE TO FIX AT SOME POINT
    // Maybe solved now with the use of vgl_vector_2d<double> ? - PVr
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

  for ( int i=1; i< chain_->size(); i++)
  {
    if ( chain_->edgel( i).get_x()< minxcache_) minxcache_= chain_->edgel( i).get_x();
    if ( chain_->edgel( i).get_x()> maxxcache_) maxxcache_= chain_->edgel( i).get_x();
    if ( chain_->edgel( i).get_y()< minycache_) minycache_= chain_->edgel( i).get_y();
    if ( chain_->edgel( i).get_y()> maxycache_) maxycache_= chain_->edgel( i).get_y();
  }
}
