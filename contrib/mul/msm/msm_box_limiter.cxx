#include <iostream>
#include <sstream>
#include "msm_box_limiter.h"
//:
// \file
// \author Tim Cootes
// \brief Apply limits to each parameter independently

#include <vnl/io/vnl_io_vector.h>
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <vul/vul_string.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//=======================================================================
msm_box_limiter::msm_box_limiter()
  : n_sds_(3.0),accept_prop_(0.98)
{
}

//: Define number of SDs to limit at
void msm_box_limiter::set_n_sds(double n_sds)
{
  n_sds_=n_sds;
}

//: Define variance on each parameter
void msm_box_limiter::set_param_var(const vnl_vector<double>& v)
{
  mode_sd_.set_size(v.size());
  for (unsigned i=0;i<v.size();++i) mode_sd_[i]=std::sqrt(v[i]);
  set_acceptance(accept_prop_);
}

//: Set the limits so that a given proportion pass
void msm_box_limiter::set_acceptance(double prop, unsigned n_modes)
{
  assert(prop>=0 && prop<=1.0);
  accept_prop_ = prop;
  if (n_modes==0) n_modes=mode_sd_.size();

  if (n_modes==0) return;

  // Assume independence and estimate proportion,p, to pass in each
  // dimension, thus prop = p^n;
  double p = std::pow(prop,1.0/n_modes);

  // In each dimension x^2 is distributed as chi-squared with one d.o.f.
  double t = msm_chi2_for_cum_prob(p,1);

  set_n_sds(std::sqrt(t));
}


//: Apply limit to parameter vector b
void msm_box_limiter::apply_limit(vnl_vector<double>& b) const
{
  for (unsigned i=0;i<b.size();++i)
  {
    double m = mode_sd_[i]*n_sds_;
    if (b[i]>m) b[i]=m;
    else
    if (b[i]<-m) b[i]=-m;
  }
}

//=======================================================================
//: Print class to os
void msm_box_limiter::print_summary(std::ostream& os) const
{
  os<<"{ n_sds: "<<n_sds_
    <<" accept_prop: "<<accept_prop_<<" } ";
}

constexpr static short version_no = 1;

//: Save class to binary file stream
void msm_box_limiter::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no);
  vsl_b_write(bfs,mode_sd_);
  vsl_b_write(bfs,accept_prop_);
  vsl_b_write(bfs,n_sds_);
}


//: Load class from binary file stream
void msm_box_limiter::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,mode_sd_);
      vsl_b_read(bfs,accept_prop_);
      vsl_b_read(bfs,n_sds_);
      break;
    default:
      std::cerr << "msm_box_limiter::b_read() :\n"
               << "Unexpected version number " << version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

std::string msm_box_limiter::is_a() const
{
  return std::string("msm_box_limiter");
}

//: Create a copy on the heap and return base class pointer
msm_param_limiter* msm_box_limiter::clone() const
{
  return new msm_box_limiter(*this);
}


//: Initialise from a text stream.
// The default implementation is for attribute-less normalisers,
// and throws if it finds any data in the stream.
void msm_box_limiter::config_from_stream(std::istream &is)
{
  std::string s = mbl_parse_block(is);

  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  accept_prop_=vul_string_atof(props.get_optional_property("accept_prop","0.98"));

  mbl_read_props_look_for_unused_props(
      "msm_ellipsoid_limiter::config_from_stream", props, mbl_read_props_type());
}
