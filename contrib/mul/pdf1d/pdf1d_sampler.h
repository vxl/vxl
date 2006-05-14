// This is mul/pdf1d/pdf1d_sampler.h
#ifndef pdf1d_sampler_h
#define pdf1d_sampler_h

//:
// \file
// \author Tim Cootes
// \brief Base class for Univariate random sampler classes.

#include <vnl/vnl_vector.h>
#include <vcl_string.h>
#include <vcl_iosfwd.h>
class pdf1d_pdf;

//=======================================================================

//: Base class for Univariate Random Sampler classes.
// This is really a random number generator whose
// outputs have the PDF of the relevant pdf1d_pdf.
// Sampler objects should only exist as long as their pdf object,
// and are not meant to be persistent.

class pdf1d_sampler
{
 protected:
  const pdf1d_pdf *pdf_model_;
 public:

  //: Dflt ctor
  pdf1d_sampler();

  //: Destructor
  virtual ~pdf1d_sampler();

  //: PDF of which this is an instance
  const pdf1d_pdf& model() const;

  //: Set model for which this is an instance
  virtual void set_model(const pdf1d_pdf&);

  //: Draw random sample from distribution
  virtual double sample()=0;

  //: Fill x with samples drawn from distribution
  virtual void get_samples(vnl_vector<double>& x);

  //: Fill x with samples possibly chosen so as to represent the distribution
  //  Generate a set of pseudo-random samples, chosen so as to be suitable
  //  to represent the distribution.  This is meant to be used for estimating
  //  continuous integrals with sampled approximations.  Where there are
  //  multiple peaks (e.g. kernel or mixture models), it is preferred that
  //  the number of samples from each component is roughly proportional
  //  to the weight for the component.  When small numbers are requested,
  //  this can be done explicitly.
  //  The default is simply to call sample() for each element of x
  virtual void regular_samples(vnl_vector<double>& x);

  //: Fill x with samples possibly chosen so as to represent the distribution
  //  As regular_samples(x), but p[i] is set to p(x[i])
  virtual void regular_samples_and_prob(vnl_vector<double>& x, vnl_vector<double>& p);

  //: Reseeds the internal random number generator
  // To achieve quasi-random initialisation use;
  // \code
  // #include <vcl_ctime.h>
  // ..
  // sampler.reseed(vcl_time(0));
  // \endcode
  virtual void reseed(unsigned long)=0;


  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_sampler* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;
};


//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const pdf1d_sampler& b);

//: Stream output operator for class pointer
void vsl_print_summary(vcl_ostream& os,const pdf1d_sampler* b);


#endif // pdf1d_sampler_h
