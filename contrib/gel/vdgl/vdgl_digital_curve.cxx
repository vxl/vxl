// This is gel/vdgl/vdgl_digital_curve.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vdgl_digital_curve.h"
#include <vcl_cassert.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsol/vsol_point_2d.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_interpolator_cubic.h>


vdgl_digital_curve::vdgl_digital_curve()
  : interpolator_(new vdgl_interpolator_linear(new vdgl_edgel_chain))
{
  // This object must have an interpolator, even if it is useless
  assert(interpolator_);
}

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
  vdgl_edgel_chain_sptr ec = interpolator_->get_edgel_chain();
  return new vsol_point_2d(ec->edgel(0).get_pt());
#if 0 // Note that the following will fail for a cubic interpolator!
  return new vsol_point_2d(get_x(0), get_y(0));
#endif // 0
}

vsol_point_2d_sptr vdgl_digital_curve::p1() const
{
  vdgl_edgel_chain_sptr ec = interpolator_->get_edgel_chain();
  return new vsol_point_2d(ec->edgel(ec->size()-1).get_pt());
#if 0 // Note that the following will fail for a cubic interpolator!
  return new vsol_point_2d(get_x(1), get_y(1));
#endif // 0
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

bool vdgl_digital_curve::operator==(const vdgl_digital_curve &other) const
{
  if (this==&other)
    return true;
  //same order of interpolation?
  if (this->order() != other.order())
    return false;
  //are the edgel chains equal?
  vdgl_edgel_chain_sptr ec = this->get_interpolator()->get_edgel_chain();
  vdgl_edgel_chain_sptr eco = other.get_interpolator()->get_edgel_chain();
  bool edgel_chains_eq = *ec == *eco;
  //are the endpoints equal
  bool epts_eq = this->vsol_curve_2d::endpoints_equal(other);
  return edgel_chains_eq&&epts_eq;
}

bool vdgl_digital_curve::operator==(const vsol_spatial_object_2d& obj) const
{
  if (!(obj.spatial_type() == vsol_spatial_object_2d::CURVE))
    return false;
  if (!(((vsol_curve_2d const&)obj).curve_type() == vsol_curve_2d::DIGITAL_CURVE))
    return false;
  return *this == (vdgl_digital_curve const&)(vsol_curve_2d const&)obj;
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vdgl_digital_curve::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, this->order());
  vsl_b_write(os, this->get_interpolator()->get_edgel_chain());
  vsl_b_write(os, true);
}

//: Binary load self from stream (not typically used)
void vdgl_digital_curve::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   default:
    assert(!"vdgl_digital_curve I/O: version should be 1");
   case 1:
    short order; 
    vsl_b_read(is, order);
    vdgl_edgel_chain_sptr ec;
    vsl_b_read(is, ec);
    bool endpoints;
    vsl_b_read(is, endpoints);
    assert(endpoints);
    if (ec)
    {
      switch (order)
      {
       case 1:
        interpolator_ = new vdgl_interpolator_linear(ec);
        break;
       case 3:
        interpolator_ = new vdgl_interpolator_cubic(ec);
        break;
       default:
        return;
      }
    }
  }
}

//: Return IO version number;
short vdgl_digital_curve::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vdgl_digital_curve::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//: Return a platform independent string identifying the class
vcl_string vdgl_digital_curve::is_a() const
{
  return vcl_string("vdgl_digital_curve");
}

//: Return true if the argument matches the string identifying the class or any parent class
bool vdgl_digital_curve::is_class(const vcl_string& cls) const
{
  return cls==vdgl_digital_curve::is_a();
}

//: Binary save vdgl_digital_curve* to stream.
void vsl_b_write(vsl_b_ostream &os, const vdgl_digital_curve* dc)
{
  if (!dc){
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    dc->b_write(os);
  }
}

//: Binary load vdgl_digital_curve* from stream.
void vsl_b_read(vsl_b_istream &is, vdgl_digital_curve* &dc)
{
  delete dc;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    dc = new vdgl_digital_curve();
    dc->b_read(is);
  }
  else
    dc = 0;
}

//: Stream operator
vcl_ostream& operator<<(vcl_ostream& s, const vdgl_digital_curve& dc)
{
  s << "[order: " << dc.order() << ' ' << *(dc.get_interpolator()->get_edgel_chain())
    << "]";
  return s;
}
