#ifndef msm_no_limiter_h_
#define msm_no_limiter_h_
//:
// \file
// \author Tim Cootes
// \brief No limits applied to parameters

#include <iostream>
#include <iosfwd>
#include <msm/msm_param_limiter.h>
#include <vnl/vnl_vector.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: No limits applied to parameters
class msm_no_limiter : public msm_param_limiter
{
 public:
  msm_no_limiter();

  ~msm_no_limiter() override = default;

  //: Define variance on each parameter. Does nothing.
  void set_param_var(const vnl_vector<double>& v) override;

  //: Set the limits so that a given proportion pass
  //  Does nothing.
  void set_acceptance(double prop, unsigned n_modes=0) override;

  //: Apply limit to parameter vector b. Does nothing.
  void apply_limit(vnl_vector<double>& b) const override;

  //: Define number of SDs to limit at
  void set_n_sds(double n_sds);

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
};

#endif // msm_no_limiter_h_
