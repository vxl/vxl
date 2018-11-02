#ifndef msm_ellipsoid_limiter_h_
#define msm_ellipsoid_limiter_h_
//:
// \file
// \author Tim Cootes
// \brief Force param.s to lie in ellipsoid defined by variances.

#include <iostream>
#include <iosfwd>
#include <msm/msm_param_limiter.h>
#include <vnl/vnl_vector.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Force param.s to lie in ellipsoid defined by variances.
//  An axis aligned ellipsoid is defined by the variances along
//  each dimension and the n_sds parameter.
//  If the supplied vector b is outside this ellipsoid,
//  b is set to the nearest point on the surface of
//  the ellipsoid.
class msm_ellipsoid_limiter : public msm_param_limiter
{
 private:
  //: Var. on each mode
  vnl_vector<double> mode_var_;

  //: Max. allowed mahalanobis distance.
  double M_max_;

  //: Proportion of Gaussian to be accepted
  //  Used to set M_max when mode_var defined.
  double accept_prop_;

 public:

  msm_ellipsoid_limiter();

  ~msm_ellipsoid_limiter() override = default;

  //: Define variance on each parameter
  void set_param_var(const vnl_vector<double>& v) override;

  //: Set the limits so that a given proportion pass
  //  Where the parameters are described by a pdf, choose
  //  limits so that on average a proportion prop (in [0,1])
  //  are acceptable when using n_modes modes. If n_modes==0,
  //  then assume all available modes to be used.
  void set_acceptance(double prop, unsigned n_modes=0) override;

  //: Apply limit to parameter vector b
  void apply_limit(vnl_vector<double>& b) const override;

  //: Define number of SDs to limit at
  void set_n_sds(double n_sds);

  //: Return square of Mahalanobis distance to origin
  double mahalanobis(const vnl_vector<double>& b) const;

  //: Given initial b on ellipsoid, move on surface towards y
  //  Finds closest point to y in tangent plane at initial b
  //  Replaces b with normalised version of this.
  //  Returns square of length of movement relative to smallest
  //  mode variance.
  double slide_closer(vnl_vector<double>& b,
                      const vnl_vector<double>& y) const;

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  msm_param_limiter* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;

  //: Initialise from a text stream.
  // The default implementation is for attribute-less normalisers,
  // and throws if it finds any data in the stream.
  void config_from_stream(std::istream &is) override;
};

#endif // msm_ellipsoid_limiter_h_
