// This is mul/pdf1d/pdf1d_exponential_sampler.h
#ifndef pdf1d_exponential_sampler_h
#define pdf1d_exponential_sampler_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Tim Cootes
// \brief Sampler class for Univariate exponential distributions

#include <vcl_string.h>
#include <vnl/vnl_random.h>
#include <pdf1d/pdf1d_sampler.h>


class pdf1d_exponential;
//=======================================================================

//: Samples from Univariate exponential distributions
class pdf1d_exponential_sampler :public pdf1d_sampler
{
 protected:
  //: The random number generator
  vnl_random rng_;
 public:

  //: Dflt ctor
  pdf1d_exponential_sampler();

  //: Destructor
  virtual ~pdf1d_exponential_sampler();

  //: Set model for which this is an instance
  // Error check that it is an axis exponential.
  virtual void set_model(const pdf1d_pdf&);

  //: Draw random sample from Gaussian distribution
  virtual double sample();

  //: Fill x with samples possibly chosen so as to represent the distribution
  //  5 or fewer samples requested, they are spaced out equally.
  virtual void regular_samples(vnl_vector<double>& x);

  //: Reseeds the internal random number generator
  // To achieve quasi-random initialisation use;
  // \code
  // #include <vcl_ctime.h>
  // ..
  // sampler.reseed(vcl_time(0));
  // \endcode
  virtual void reseed(unsigned long);


  //: Return a reference to the pdf model
  // This is properly cast.
  const pdf1d_exponential& exponential() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_sampler* clone() const;
};

#endif // pdf1d_exponential_sampler_h
