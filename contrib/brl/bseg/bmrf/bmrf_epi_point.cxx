// This is brl/bseg/bmrf/bmrf_epi_point.cxx
#include "bmrf_epi_point.h"
//:
// \file
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>

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
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
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

