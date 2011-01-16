#ifndef msm_box_limiter_h_
#define msm_box_limiter_h_
//:
// \file
// \author Tim Cootes
// \brief Apply limits to each parameter independently

#include <msm/msm_param_limiter.h>
#include <vnl/vnl_vector.h>
#include <vcl_iosfwd.h>

//: Apply limits to each parameter independently
//  Each parameter is forced to lie in range [-k*s_i,+k*s_i],
//  where s_i is the standard deviation for the i-th parameter.
class msm_box_limiter : public msm_param_limiter
{
 private:
  //: SD on each mode
  vnl_vector<double> mode_sd_;

  //: Number of SDs to limit at
  double n_sds_;

  //: Proportion of Gaussian to be accepted
  //  Used to set M_max when mode_var defined.
  double accept_prop_;

 public:

  msm_box_limiter();

  virtual ~msm_box_limiter() {}

  //: Define variance on each parameter
  virtual void set_param_var(const vnl_vector<double>& v);

  //: Set the limits so that a given proportion pass
  //  Where the parameters are described by a pdf, choose
  //  limits so that on average a proportion prop (in [0,1])
  //  are acceptable when using n_modes modes. If n_modes==0,
  //  then assume all available modes to be used.
  virtual void set_acceptance(double prop, unsigned n_modes=0);

  //: Apply limit to parameter vector b
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

  //: Initialise from a text stream.
  // The default implementation is for attribute-less normalisers,
  // and throws if it finds any data in the stream.
  virtual void config_from_stream(vcl_istream &is);
};

#endif // msm_box_limiter_h_
