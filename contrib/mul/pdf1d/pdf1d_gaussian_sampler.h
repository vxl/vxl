// This is mul/pdf1d/pdf1d_gaussian_sampler.h
#ifndef pdf1d_gaussian_sampler_h
#define pdf1d_gaussian_sampler_h

//:
// \file
// \author Tim Cootes
// \brief Sampler class for Univariate gaussian

#include <string>
#include <vnl/vnl_random.h>
#include <pdf1d/pdf1d_sampler.h>
class pdf1d_gaussian;

//=======================================================================

//: Samples from Univariate Gaussian
class pdf1d_gaussian_sampler :public pdf1d_sampler
{
 protected:
  //: The random number generator
  vnl_random rng_;
 public:

  //: Dflt ctor
  pdf1d_gaussian_sampler();

  //: Destructor
  virtual ~pdf1d_gaussian_sampler();

  //: Set model for which this is an instance
  // Error check that it is an axis gaussian.
  virtual void set_model(const pdf1d_pdf&);

  //: Draw random sample from Gaussian distribution
  virtual double sample();

  //: Fill x with samples possibly chosen so as to represent the distribution
  //  5 or fewer samples requested, they are spaced out equally.
  virtual void regular_samples(vnl_vector<double>& x);

  //: Reseeds the internal random number generator
  // To achieve quasi-random initialisation use;
  // \code
  // #include <vcl_compiler.h>
  // #include <iostream>
  // #include <ctime>
  // ..
  // sampler.reseed(std::time(0));
  // \endcode
  virtual void reseed(unsigned long);


  //: Return a reference to the pdf model
  // This is properly cast.
  const pdf1d_gaussian& gaussian() const;

  //: Name of the class
  virtual std::string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(std::string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_sampler* clone() const;
};

#endif // pdf1d_gaussian_sampler_h
