// This is mul/vpdfl/vpdfl_gaussian_sampler.h
#ifndef vpdfl_gaussian_sampler_h
#define vpdfl_gaussian_sampler_h
//:
// \file
// \author Ian Scott
// \date 19-Apr-2001
// \brief Sampler class for Multi-Variate Gaussian.

#include <string>
#include <iosfwd>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_random.h>
#include <vpdfl/vpdfl_sampler_base.h>
class vpdfl_gaussian;

//=======================================================================

//: Samples from a Gaussian PDF
class vpdfl_gaussian_sampler :public vpdfl_sampler_base
{
 protected:
  //: The random number generator
  vnl_random rng_;
  //: workspace variable
  vnl_vector<double> b_;
 public:

  //: Dflt ctor
  vpdfl_gaussian_sampler();

  //: Destructor
  ~vpdfl_gaussian_sampler() override;

  //: Draw random sample from Gaussian distribution
  void sample(vnl_vector<double>& x) override;

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
  const vpdfl_gaussian& gaussian() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  vpdfl_sampler_base* clone() const override;
};

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const vpdfl_gaussian_sampler& b);

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const vpdfl_gaussian_sampler* b);

#endif // vpdfl_gaussian_sampler_h
