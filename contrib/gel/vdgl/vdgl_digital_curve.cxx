// This is gel/vdgl/vdgl_digital_curve.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vdgl_digital_curve.h"
#include <vcl_cassert.h>
#include <vsol/vsol_point_2d.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator_linear.h>

vdgl_digital_curve::vdgl_digital_curve( vdgl_interpolator_sptr interpolator)
  : interpolator_(interpolator)
{
  assert(interpolator);
}

vdgl_digital_curve::vdgl_digital_curve(vsol_point_2d_sptr const& p0,
                                       vsol_point_2d_sptr const& p1)
{
  if (!p0||!p1)
    {
      vdgl_edgel_chain_sptr ec = new vdgl_edgel_chain();
      interpolator_ = new vdgl_interpolator_linear(ec);
      return;
    }
  vdgl_edgel_chain* ec = new vdgl_edgel_chain(p0->x(), p0->y(),
                                              p1->x(), p1->y());
  interpolator_ = new vdgl_interpolator_linear(ec);
}

vsol_spatial_object_2d* vdgl_digital_curve::clone(void) const
{
  return new vdgl_digital_curve(interpolator_);
}

//: Return a platform independent string identifying the class
vcl_string vdgl_digital_curve::is_a() const
{
  return vcl_string("vdgl_digital_curve");
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

double vdgl_digital_curve::get_grad( const double s) const
{
  int i= interpolator_->get_edgel_chain()->size() - 1;
  double index= (s<0) ? 0.0 : (s>=1) ? i : s*i;

  return interpolator_->get_grad(index);
}

double vdgl_digital_curve::get_theta( const double s) const
{
  int i= interpolator_->get_edgel_chain()->size() - 1;
  double index= (s<0) ? 0.0 : (s>=1) ? i : s*i;

  return interpolator_->get_theta(index);
}

double vdgl_digital_curve::get_tangent_angle(const double s) const
{
  int i= interpolator_->get_edgel_chain()->size() - 1;
  double index= (s<0) ? 0.0 : (s>=1) ? i : s*i;
  return interpolator_->get_tangent_angle(index);
}

int vdgl_digital_curve::n_pts() const
{
  return interpolator_->get_edgel_chain()->size();
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
  vcl_cerr << "vdgl_digital_curve::set_p0() not allowed and ignored...\n";
  int i = 0;
  interpolator_->get_edgel_chain()->set_edgel(i, vdgl_edgel ( p->x(), p->y() ) );
}

void vdgl_digital_curve::set_p1(const vsol_point_2d_sptr &p )
{
  vcl_cerr << "vdgl_digital_curve::set_p1() not allowed and ignored...\n";
  int i = interpolator_->get_edgel_chain()->size() - 1;
  interpolator_->get_edgel_chain()->set_edgel(i, vdgl_edgel ( p->x(), p->y() ) );
}

bool vdgl_digital_curve::split(vsol_point_2d_sptr const& v,
                               vdgl_digital_curve_sptr& dc1,
                               vdgl_digital_curve_sptr& dc2)
{
  vdgl_edgel_chain_sptr ec = interpolator_->get_edgel_chain();
  vdgl_edgel_chain_sptr ec1, ec2;
  if (! ec->split(v->x(), v->y(), ec1, ec2)) return false;
  dc1 = new vdgl_digital_curve(new vdgl_interpolator_linear(ec1));
  dc2 = new vdgl_digital_curve(new vdgl_interpolator_linear(ec2));
  return true;
}

//: scan all the points on the curve and compute the bounds.
void vdgl_digital_curve::compute_bounding_box(void) const
{
  set_bounding_box(   interpolator_->get_min_x(), interpolator_->get_min_y());
  add_to_bounding_box(interpolator_->get_max_x(), interpolator_->get_max_y());
}
