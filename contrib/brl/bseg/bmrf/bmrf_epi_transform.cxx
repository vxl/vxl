// This is contrib/brl/bseg/bmrf/bmrf_epi_transform.cxx
//:
// \file

#include "bmrf_epi_transform.h"
#include <bmrf/bmrf_epi_transform_sptr.h>
#include <bmrf/bmrf_epi_point.h>
#include <bmrf/bmrf_epi_point_sptr.h>
#include <bmrf/bmrf_epi_seg.h>
#include <bmrf/bmrf_epi_seg_sptr.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>

struct bmrf_tranformed_epi_seg : public bmrf_epi_seg 
{
  bmrf_tranformed_epi_seg( const bmrf_epi_transform* xform,
                           const bmrf_epi_seg_sptr& ep,
                           double t, bool update_all )
    : bmrf_epi_seg(*ep)
  {
    for( vcl_vector<bmrf_epi_point_sptr>::iterator s_itr = this->seg_.begin();
         s_itr != seg_.end();  ++s_itr ){
      //vcl_cout << "(s,a): ("<<(*s_itr)->s()<<','<<(*s_itr)->alpha()<<") -> (";
      *s_itr = xform->apply(*s_itr, t, update_all);
      //vcl_cout << (*s_itr)->s()<<','<<(*s_itr)->alpha()<<")"<<vcl_endl;
    }
    this->limits_valid_ = false;
  }
};


//: Calculates the transformed value of s
double
bmrf_epi_transform::apply(double s0, double alpha, double t) const
{
  return s0/(1.0 - this->gamma(alpha)*t);
}


//: Calculates a transformed epi_point
bmrf_epi_point_sptr
bmrf_epi_transform::apply(const bmrf_epi_point_sptr& ep, double t, bool update_all) const
{
  bmrf_epi_point_sptr new_ep(new bmrf_epi_point(*ep.ptr()));
  new_ep->set_s(this->apply(ep->s(), ep->alpha(), t));
  if( update_all ){
    // update x, y, etc. here
    double ds = new_ep->s() - ep->s();
    new_ep->set( ep->x()+ds*vcl_cos(ep->alpha()), ep->y()+ds*vcl_sin(ep->alpha()) );
  }
  return new_ep;
}


//: Calculates a transformed epi_seg
bmrf_epi_seg_sptr
bmrf_epi_transform::apply(const bmrf_epi_seg_sptr& ep, double t, bool update_all) const
{
  bmrf_epi_seg_sptr new_seg(new bmrf_tranformed_epi_seg(this, ep, t, update_all));
  return new_seg;
}


//----------------bmrf_const_epi_transform functions --------------------

//: Constructor
bmrf_const_epi_transform::bmrf_const_epi_transform(double gamma)
 : gamma_(gamma)
{
}


//: Set the constant gamma value
void
bmrf_const_epi_transform::set_gamma(double gamma)
{
  gamma_ = gamma;
}


//: Returns the constant gamma value
double
bmrf_const_epi_transform::gamma(double alpha) const
{
  return gamma_;
}


//----------------bmrf_linear_epi_transform functions --------------------

//: Constructor
bmrf_linear_epi_transform::bmrf_linear_epi_transform(double m, double b)
 : m_(m), b_(b)
{
}


//: Set the parameters
void
bmrf_linear_epi_transform::set_params(double m, double b)
{
  m_ = m;
  b_ = b;
}


//: Returns the gamma value
double
bmrf_linear_epi_transform::gamma(double alpha) const
{
  return m_*alpha + b_;
}
