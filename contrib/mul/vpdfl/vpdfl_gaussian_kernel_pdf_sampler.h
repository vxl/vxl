// This is mul/vpdfl/vpdfl_gaussian_kernel_pdf_sampler.h
#ifndef vpdfl_gaussian_kernel_pdf_sampler_h
#define vpdfl_gaussian_kernel_pdf_sampler_h
//:
// \file
// \author Tim Cootes
// \brief Sampler class for gaussian kernel PDF.

#include <string>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_random.h>
#include <vpdfl/vpdfl_sampler_base.h>
class vpdfl_gaussian_kernel_pdf;

//=======================================================================

//: Samples from an axis aligned Gaussian PDF
class vpdfl_gaussian_kernel_pdf_sampler :public vpdfl_sampler_base
{
 protected:
  //: The random number generator
  vnl_random rng_;
 public:

  // Dflt ctor
  vpdfl_gaussian_kernel_pdf_sampler();

  // Destructor
  ~vpdfl_gaussian_kernel_pdf_sampler() override;

  //: Set model for which this is an instance
  // Error check that it is an axis gaussian.
  void set_model(const vpdfl_pdf_base&) override;

  //: Draw random sample from j-th component of distribution
  void sample_component(vnl_vector<double>& x, int j);

  //: Draw random sample from distribution
  void sample(vnl_vector<double>& x) override;

  //: Fill x with samples possibly chosen so as to represent the distribution
  //  Sample sequentially from each component.
  void regular_samples(std::vector<vnl_vector<double> >& x) override;

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
  const vpdfl_gaussian_kernel_pdf& gaussian_kernel_pdf() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  vpdfl_sampler_base* clone() const override;
};

#endif // vpdfl_gaussian_kernel_pdf_sampler_h
