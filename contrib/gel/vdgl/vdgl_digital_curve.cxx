#ifdef __GNUC__
#pragma implementation
#endif

#include <vsol/vsol_point_2d.h>

#include "vdgl_digital_curve.h"


vdgl_digital_curve::vdgl_digital_curve( vdgl_interpolator_sptr interpolator)
  : interpolator_( interpolator)
{
  int j = 0;
  if (!interpolator) { int i = i / j; }
}

vsol_spatial_object_2d_sptr vdgl_digital_curve::clone(void) const
{
  return new vdgl_digital_curve(interpolator_);
}

double vdgl_digital_curve::get_x( const double s) const
{
  int i= interpolator_->get_edgel_chain()->size() - 1;
  double index= (s<0) ? 0.0 : (s>=1) ? i : s*i;

  return interpolator_->get_x(index);
}

double vdgl_digital_curve::get_y( const double s) const
{
  int i= interpolator_->get_edgel_chain()->size() - 1;
  double index= (s<0) ? 0.0 : (s>=1) ? i : s*i;

  return interpolator_->get_y(index);
}

double vdgl_digital_curve::get_theta( const double s) const
{
  int i= interpolator_->get_edgel_chain()->size() - 1;
  double index= (s<0) ? 0.0 : (s>=1) ? i : s*i;

  return interpolator_->get_theta(index);
}

vsol_point_2d_sptr vdgl_digital_curve::p0() const
{
  return new vsol_point_2d( get_x(0), get_y(0));
}

vsol_point_2d_sptr vdgl_digital_curve::p1() const
{
  return new vsol_point_2d( get_x(1), get_y(1));
}

double vdgl_digital_curve::length() const
{
  return interpolator_->get_length();
}

void vdgl_digital_curve::set_p0(const vsol_point_2d_sptr &p)
{
  
  vcl_cerr << "vdgl_digital_curve::set_p0() not allowed and ignored..." << vcl_endl;
  int i = 0;
  interpolator_->get_edgel_chain()->set_edgel(i, vdgl_edgel ( p->x(), p->y() ) );
}

void vdgl_digital_curve::set_p1(const vsol_point_2d_sptr &p )
{
  vcl_cerr << "vdgl_digital_curve::set_p1() not allowed and ignored..." << vcl_endl;
  int i = interpolator_->get_edgel_chain()->size() - 1;
  interpolator_->get_edgel_chain()->set_edgel(i, vdgl_edgel ( p->x(), p->y() ) );

}

