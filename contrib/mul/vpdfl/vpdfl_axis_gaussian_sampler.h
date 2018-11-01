// This is mul/vpdfl/vpdfl_axis_gaussian_sampler.h
#ifndef vpdfl_axis_gaussian_sampler_h
#define vpdfl_axis_gaussian_sampler_h
//:
// \file
// \author Ian Scott
// \date 19-Apr-2001
// \brief Sampler class for Multi-Variate axis aligned Gaussian.

#include <string>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_random.h>
#include <vpdfl/vpdfl_sampler_base.h>
class vpdfl_axis_gaussian;

//=======================================================================

//: Samples from an axis aligned Gaussian PDF
class vpdfl_axis_gaussian_sampler :public vpdfl_sampler_base
{
 protected:
  //: The random number generator
  vnl_random rng_;
 public:

  //: Dflt ctor
  vpdfl_axis_gaussian_sampler();

  //: Destructor
  ~vpdfl_axis_gaussian_sampler() override;

  //: Set model for which this is an instance
  // Error check that it is an axis gaussian.
  void set_model(const vpdfl_pdf_base&) override;

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
  const vpdfl_axis_gaussian& axis_gaussian() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  vpdfl_sampler_base* clone() const override;
};

#endif // vpdfl_axis_gaussian_sampler_h
