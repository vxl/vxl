#ifndef msm_no_limiter_h_
#define msm_no_limiter_h_
//:
// \file
// \author Tim Cootes
// \brief No limits applied to parameters

#include <msm/msm_param_limiter.h>
#include <vnl/vnl_vector.h>
#include <vcl_iosfwd.h>

//: No limits applied to parameters
class msm_no_limiter : public msm_param_limiter
{
 public:
  msm_no_limiter();

  virtual ~msm_no_limiter() {}

  //: Define variance on each parameter. Does nothing.
  virtual void set_param_var(const vnl_vector<double>& v);

  //: Set the limits so that a given proportion pass
  //  Does nothing.
  virtual void set_acceptance(double prop, unsigned n_modes=0);

  //: Apply limit to parameter vector b. Does nothing.
  virtual void apply_limit(vnl_vector<double>& b) const;

  //: Define number of SDs to limit at
  void set_n_sds(double n_sds);

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual msm_param_limiter* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // msm_no_limiter_h_
