// This is mul/pdf1d/pdf1d_flat_sampler.h
#ifndef pdf1d_flat_sampler_h
#define pdf1d_flat_sampler_h

//:
// \file
// \author Tim Cootes
// \brief Sampler class for Univariate flat distributions

#include <string>
#include <vnl/vnl_random.h>
#include <pdf1d/pdf1d_sampler.h>
class pdf1d_flat;

//=======================================================================

//: Samples from Univariate flat distributions
class pdf1d_flat_sampler :public pdf1d_sampler
{
 protected:
  //: The random number generator
  vnl_random rng_;
 public:

  //: Dflt ctor
  pdf1d_flat_sampler();

  //: Destructor
  ~pdf1d_flat_sampler() override;

  //: Set model for which this is an instance
  // Error check that it is an axis flat.
  void set_model(const pdf1d_pdf&) override;

  //: Draw random sample from Gaussian distribution
  double sample() override;

  //: Fill x with samples possibly chosen so as to represent the distribution
  //  5 or fewer samples requested, they are spaced out equally.
  void regular_samples(vnl_vector<double>& x) override;

  //: Reseeds the internal random number generator
  // To achieve quasi-random initialisation use;
  // \code
  // #include <vcl_compiler.h>
  // #include <iostream>
  // #include <ctime>
  // ..
  // sampler.reseed(std::time(0));
  // \endcode
  void reseed(unsigned long) override;


  //: Return a reference to the pdf model
  // This is properly cast.
  const pdf1d_flat& flat() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  pdf1d_sampler* clone() const override;
};

#endif // pdf1d_flat_sampler_h
