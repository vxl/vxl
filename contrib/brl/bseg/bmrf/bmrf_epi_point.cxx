//:
// \file
#include "bmrf_epi_point.h"

// constructors
bmrf_epi_point::bmrf_epi_point(vgl_point_2d<double> &p,
                               const double alpha,
                               const double s,
                               const double grad_mag,
                               const double grad_ang,
                               const double tan_ang)
{
  p_ = p;
  alpha_ = alpha;
  s_ = s;
  grad_mag_ = grad_mag;
  grad_ang_ = grad_ang;
  tan_ang_ = tan_ang;
}

bmrf_epi_point::bmrf_epi_point(const double x, const double y,
                               const double alpha,
                               const double s,
                               const double grad_mag,
                               const double grad_ang,
                               const double tan_ang)
{
  p_.set(x, y);
  alpha_ = alpha;
  s_ = s;
  grad_mag_ = grad_mag;
  grad_ang_ = grad_ang;
  tan_ang_ = tan_ang;
}


bmrf_epi_point::bmrf_epi_point()
{
  this->set(0,0);
  alpha_ = 0;
  s_ = 0;
  grad_mag_ = -1;
  grad_ang_ = 0;
  tan_ang_ = 0;
}

//: Binary save self to stream.
void bmrf_epi_point::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, p_.x());
  vsl_b_write(os, p_.y());
  vsl_b_write(os, alpha_);
  vsl_b_write(os, s_);
  vsl_b_write(os, grad_mag_);
  vsl_b_write(os, grad_ang_);
  vsl_b_write(os, tan_ang_);
}

//: Binary load self from stream.
void bmrf_epi_point::b_read(vsl_b_istream &is)
{
  if(!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    {
      double x=0, y=0;
      vsl_b_read(is, x);
      vsl_b_read(is, y);
      this->p_.set(x, y);
      vsl_b_read(is, alpha_);
      vsl_b_read(is, s_);
      vsl_b_read(is,  this->grad_mag_);
      vsl_b_read(is, this->grad_ang_);
      vsl_b_read(is, this->tan_ang_);
    }
  }
}
//: Return IO version number;
short bmrf_epi_point::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void bmrf_epi_point::print_summary(vcl_ostream &os) const
{
  os << *this;
}

  //: Return a platform independent string identifying the class
vcl_string bmrf_epi_point::is_a() const
{
  return vcl_string("bmrf_epi_point");
}

  //: Return true if the argument matches the string identifying the class or any parent class
bool bmrf_epi_point::is_class(const vcl_string& cls) const
{
  return cls==bmrf_epi_point::is_a();
}

//external functions
vcl_ostream& operator<<(vcl_ostream& s, bmrf_epi_point const& ep)
{
  s << '(' << (int)ep.x() << ' ' << (int)ep.y() << ")[" << ep.alpha()
    << ' ' << ep.s() << ' '  << ep.grad_mag() << ' ' << ep.grad_ang() << ' '
    << ep.tan_ang() << "]\n";

  return s;
}

//: Binary save bmrf_epi_point to stream.
void
vsl_b_write(vsl_b_ostream &os, bmrf_epi_point_sptr const& ep)
{
  if (!ep){
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    ep->b_write(os);
  }
}


//: Binary load bmrf_epi_point from stream.
void
vsl_b_read(vsl_b_istream &is, bmrf_epi_point_sptr &ep)
{
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr){
    ep = new bmrf_epi_point();
    ep->b_read(is);
  }
  else
    ep = 0;
}

