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

  for( int i=0; i< (chain_->size()-1); i++)
  {
          vgl_point_2d<double> p1= chain_->edgel( i).get_pt();
          vgl_point_2d<double> p2= chain_->edgel( i+1).get_pt();

          // NOTE THERE IS A PROBLEM HERE UNDER WINDOWS
          //   WHICH I WILL HAVE TO FIX AT SOME POINT
          lengthcache_ += 1;

          //vgl_point_2d<double> diff= p2-p1;

          //lengthcache_ += (p1- p2;//.distance( p2);
  }
//    lengthcache_+= chain_->edgel( i).get_pt().distance( chain_->edgel( i+1).get_pt());
}

void vdgl_interpolator_linear::recompute_bbox()
{
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


