// This is mul/vpdfl/vpdfl_sampler_base.h
#ifndef vpdfl_sampler_base_h
#define vpdfl_sampler_base_h
//:
// \file
// \author Ian Scott
// \date 19-Apr-2001
// \brief Base class for Multi-Variate random sampler classes.

#include <string>
#include <vector>
#include <iosfwd>
#include <vnl/vnl_vector.h>

//=======================================================================

class vpdfl_pdf_base;

//: Base class for Multi-Variate Random Sampler classes.
// This is really a multivariate random number generator whose
// outputs have the PDF of the relevant vpdfl_pdf_base.
// Sampler objects should only exist as long as their pdf object,
// and are not meant to be persistent.

class vpdfl_sampler_base
{
 protected:
  const vpdfl_pdf_base *pdf_model_;
 public:

  // Dflt ctor
  vpdfl_sampler_base();

  // Destructor
  virtual ~vpdfl_sampler_base();

  //: PDF of which this is an instance
  const vpdfl_pdf_base& model() const;

  //: Set model for which this is an instance
  virtual void set_model(const vpdfl_pdf_base&);

  //: Draw random sample from distribution
  virtual void sample(vnl_vector<double>& x)=0;

  //: Fill x with samples drawn from distribution
  virtual void get_samples(std::vector<vnl_vector<double> >& x);

  //: Fill x with samples possibly chosen so as to represent the distribution
  //  Generate a set of pseudo-random samples, chosen so as to be suitable
  //  to represent the distribution.  This is meant to be used for estimating
  //  continuous integrals with sampled approximations.  Where there are
  //  multiple peaks (e.g. kernel or mixture models), it is preferred that
  //  the number of samples from each component is roughly proportional
  //  to the weight for the component.  When small numbers are requested,
  //  this can be done explicitly.
  //  The default is simply to call sample() for each element of x
  virtual void regular_samples(std::vector<vnl_vector<double> >& x);

  //: Fill x with samples possibly chosen so as to represent the distribution
  //  As regular_samples(x), but p[i] is set to p(x[i])
  virtual void regular_samples_and_prob(std::vector<vnl_vector<double> >& x,
                                        vnl_vector<double>& p);

  //: Reseeds the internal random number generator
  // To achieve quasi-random initialisation use;
  // \code
  // #include <vcl_compiler.h>
  // #include <iostream>
  // #include <ctime>
  // ..
  // sampler.reseed(std::time(0));
  // \endcode
  virtual void reseed(unsigned long)=0;


  //: Name of the class
  virtual std::string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(std::string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual vpdfl_sampler_base* clone() const = 0;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const;
};


//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const vpdfl_sampler_base& b);

//: Stream output operator for class pointer
void vsl_print_summary(std::ostream& os,const vpdfl_sampler_base* b);


#endif // vpdfl_sampler_base_h
