#ifndef msm_param_limiter_h_
#define msm_param_limiter_h_
//:
// \file
// \author Tim Cootes
// \brief Base for objects with apply limits to parameters

#include <string>
#include <iostream>
#include <memory>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_fwd.h>
#include <vnl/vnl_fwd.h>

//: Base for objects with apply limits to parameters.
//  Derived classes implement apply_limit(b) function, which applies
//  constraints to vector b based on variances.
class msm_param_limiter
{
 public:

  virtual ~msm_param_limiter() = default;

  //: Define variance on each parameter
  virtual void set_param_var(const vnl_vector<double>& v)=0;

  //: Set the limits so that a given proportion pass
  //  Where the parameters are described by a pdf, choose
  //  limits so that on average a proportion prop (in [0,1])
  //  are acceptable when using n_modes modes. If n_modes==0,
  //  then assume all available modes to be used.
  virtual void set_acceptance(double prop, unsigned n_modes=0) = 0;

  //: Apply limit to parameter vector b
  virtual void apply_limit(vnl_vector<double>& b) const = 0;

  //: Name of the class
  virtual std::string is_a() const = 0;

  //: Create a copy on the heap and return base class pointer
  virtual msm_param_limiter* clone() const = 0;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const=0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) = 0;

  //: Create a concrete msm_param_limiter-derived object, from a text specification.
  static std::unique_ptr<msm_param_limiter> create_from_stream(std::istream &is);

  //: Initialise from a text stream.
  // The default implementation is for attribute-less normalisers,
  // and throws if it finds any data in the stream.
  virtual void config_from_stream(std::istream &is);
};

//: Allows derived class to be loaded by base-class pointer
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(bfs,base_ptr);".  This loads derived class
//  objects from the disk, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const msm_param_limiter& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_param_limiter& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_param_limiter& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const msm_param_limiter& b);

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const msm_param_limiter* b);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_param_limiter& b);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_param_limiter* b);


//: Returns X such that P(chi<X | dof==n)==p
// The value of Chi-Squared such that the probability
// that a random variable drawn from a chi-2 distribution
// is less than Chi_Squared is p.
// \param p  Target probability
// \param n  Number of dimensions
// \param tol  Tolerance of result (default = 0.001)
double msm_chi2_for_cum_prob(double p, int n, double tol=0.001);

#endif // msm_param_limiter_h_
