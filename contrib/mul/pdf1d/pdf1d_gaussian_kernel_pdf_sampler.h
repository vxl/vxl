#ifndef pdf1d_gaussian_kernel_pdf_sampler_h_
#define pdf1d_gaussian_kernel_pdf_sampler_h_
//:
// \file
// \brief Implements sampling for a gaussian kernel_pdf model
// \author Tim Cootes and Ian Scott

#include <vnl/vnl_random.h>
#include <pdf1d/pdf1d_sampler.h>
#include <pdf1d/pdf1d_gaussian_kernel_pdf.h>
class pdf1d_gaussian_kernel_pdf;

//: Generates random samples from a gaussian kernel estimate
class pdf1d_gaussian_kernel_pdf_sampler : public pdf1d_sampler
{
 protected:
  //: The random number generator
  vnl_random rng_;
 public:

  //: Dflt ctor
  pdf1d_gaussian_kernel_pdf_sampler();

  //: Destructor
  ~pdf1d_gaussian_kernel_pdf_sampler() override;

  const pdf1d_gaussian_kernel_pdf& gaussian_kernel_pdf() const;

  //: Draw random sample from distribution
  double sample() override;

    //: Fill x with samples possibly chosen so as to represent the distribution
    //  Samples equal numbers from each kernel.
  void regular_samples(vnl_vector<double>& x) override;

  //: Reseeds the static random number generator (one per derived class)
  void reseed(unsigned long) override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  pdf1d_sampler* clone() const override;
};

#endif // pdf1d_gaussian_kernel_pdf_sampler_h_
