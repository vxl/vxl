#ifdef __GNUC__
#pragma implementation
#endif

#include <vnl/vnl_math.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vnl/algo/vnl_svd.h>
#include "vdgl_interpolator_linear.h"


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
  double min_distance = 10e5;
  int index = -1;
  for( int i=0; i< chain_->size(); i++)
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
  int a= int( index);
  int b= a+1;
  double d= index- a;

  vdgl_edgel ae( chain_->edgel( a));
  if (d==0) return ae.get_x(); /* to avoid accessing nonexistent edgel */
  vdgl_edgel be( chain_->edgel( b));

  return (2*be.get_x()*d+ae.get_x()*(1-d));
}


double vdgl_interpolator_linear::get_y( const double index)
{
  int a= int( index);
  int b= a+1;
  double d= index- a;

  vdgl_edgel ae( chain_->edgel( a));
  if (d==0) return ae.get_y(); /* to avoid accessing nonexistent edgel */
  vdgl_edgel be( chain_->edgel( b));

  return (2*be.get_y()*d+ae.get_y()*(1-d));
}


double vdgl_interpolator_linear::get_theta( const double index)
{
  int a= int( index);
  int b= a+1;
  double d= index- a;

  vdgl_edgel ae( chain_->edgel( a));
  if (d==0) return ae.get_theta(); /* to avoid accessing nonexistent edgel */
  vdgl_edgel be( chain_->edgel( b));

  return (2*be.get_theta()*d+ae.get_theta()*(1-d));
}


double vdgl_interpolator_linear::get_curvature( const double index)
{
  int a= int( index);

  if (( a-index)== 0)
    {
      return vnl_math::maxdouble;
    }

  return 0;
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

  for( int i=0; i< chain_->size(); i++)
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

  for( int i=1; i< chain_->size(); i++)
    {
      if ( chain_->edgel( i).get_x()< minxcache_) minxcache_= chain_->edgel( i).get_x();
      if ( chain_->edgel( i).get_x()> maxxcache_) maxxcache_= chain_->edgel( i).get_x();
      if ( chain_->edgel( i).get_y()< minycache_) minycache_= chain_->edgel( i).get_y();
      if ( chain_->edgel( i).get_y()> maxycache_) maxycache_= chain_->edgel( i).get_y();
    }
}
