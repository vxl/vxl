#include <iostream>
#include <sstream>
#include "msm_ellipsoid_limiter.h"
//:
// \file
// \author Tim Cootes
// \brief Force param.s to lie in ellipsoid defined by variances.

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
msm_ellipsoid_limiter::msm_ellipsoid_limiter()
  : M_max_(3.0),accept_prop_(0.98)
{
}

//: Define number of SDs to limit at
void msm_ellipsoid_limiter::set_n_sds(double n_sds)
{
  M_max_ = n_sds*n_sds;
}

//: Define variance on each parameter
void msm_ellipsoid_limiter::set_param_var(const vnl_vector<double>& v)
{
  mode_var_=v;
  set_acceptance(accept_prop_);
}

//: Set the limits so that a given proportion pass
//  Where the parameters are described by a pdf, choose
//  limits so that on average a proportion prop (in [0,1])
//  are acceptable when using n_modes modes. If n_modes==0,
//  then assume all available modes to be used.
void msm_ellipsoid_limiter::set_acceptance(double prop,
                                           unsigned n_modes)
{
  assert(prop>=0 && prop<=1.0);
  accept_prop_ = prop;
  if (n_modes==0) n_modes=mode_var_.size();
  if (n_modes==0)
  {
    M_max_=0.0;
    return;
  }

  M_max_ = msm_chi2_for_cum_prob(prop,n_modes);
}

//: Return square of Mahalanobis distance to origin
double msm_ellipsoid_limiter::mahalanobis(const vnl_vector<double>& b) const
{
  // Compute Mahalanobis distance
  double M=0.0;
  for (unsigned i=0;i<b.size();++i)
    M += b[i]*b[i]/mode_var_[i];
  return M;
}

//: Given initial b on ellipsoid, move on surface towards y
//  Finds closest point to y in tangent plane at initial b
//  Replaces b with normalised version of this.
double msm_ellipsoid_limiter::slide_closer(vnl_vector<double>& b,
                                           const vnl_vector<double>& y) const
{
  unsigned n=b.size();
  vnl_vector<double> u(n);  // u is normal vector at b
  for (unsigned i=0;i<n;++i) u[i]=b[i]/mode_var_[i];

  double s = (dot_product(u,y)-M_max_)/u.squared_magnitude();

  // Calculate point on tangent plane closest to y
  vnl_vector<double> b1 = y-s*u;

  // Scale it so that it is on the ellipsoid
  b1 *= std::sqrt(M_max_/mahalanobis(b1));

  double d2=vnl_vector_ssd(b,b1)/n;

  b=b1;
  return d2/mode_var_[n-1];  // Scaled relative to smallest variance
}


//: Apply limit to parameter vector b
void msm_ellipsoid_limiter::apply_limit(vnl_vector<double>& b) const
{
  // Compute Mahalanobis distance
  double M=mahalanobis(b);

  if (M<M_max_) return;  // Inside ellipsoid, so return.

  // b is outside ellipsoid, so need to move to closest point
  // on the ellipsoid.

  vnl_vector<double> b0=b;

  // Initial estimate: shrink towards origin so that M=M_max_
  b*=std::sqrt(M_max_/M);

  // Now slide around ellipsoid by moving to nearest point
  // to b0 on the tangent plane at current b, then normalising
  // onto the ellipsoid

  unsigned n_its=0;
  while (slide_closer(b,b0)>1e-6 && n_its<10) { n_its++; };
}

//=======================================================================
//: Print class to os
void msm_ellipsoid_limiter::print_summary(std::ostream& os) const
{
  os<<" { M_max: "<<M_max_ <<" accept_prop: "<<accept_prop_<<" } ";
}

constexpr static short version_no = 1;

//: Save class to binary file stream
void msm_ellipsoid_limiter::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no);
  vsl_b_write(bfs,mode_var_);
  vsl_b_write(bfs,accept_prop_);
  vsl_b_write(bfs,M_max_);
}


//: Load class from binary file stream
void msm_ellipsoid_limiter::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,mode_var_);
      vsl_b_read(bfs,accept_prop_);
      vsl_b_read(bfs,M_max_);
      break;
    default:
      std::cerr << "msm_ellipsoid_limiter::b_read() :\n"
               << "Unexpected version number " << version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

std::string msm_ellipsoid_limiter::is_a() const
{
  return std::string("msm_ellipsoid_limiter");
}

//: Create a copy on the heap and return base class pointer
msm_param_limiter* msm_ellipsoid_limiter::clone() const
{
  return new msm_ellipsoid_limiter(*this);
}

//: Initialise from a text stream.
// The default implementation is for attribute-less normalisers,
// and throws if it finds any data in the stream.
void msm_ellipsoid_limiter::config_from_stream(std::istream &is)
{
  std::string s = mbl_parse_block(is);

  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  accept_prop_=vul_string_atof(props.get_optional_property("accept_prop","0.98"));

  mbl_read_props_look_for_unused_props(
      "msm_ellipsoid_limiter::config_from_stream", props, mbl_read_props_type());
}
