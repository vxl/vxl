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
  virtual ~pdf1d_gaussian_kernel_pdf_sampler();

  const pdf1d_gaussian_kernel_pdf& gaussian_kernel_pdf() const;

  //: Draw random sample from distribution
  virtual double sample();

    //: Fill x with samples possibly chosen so as to represent the distribution
    //  Samples equal numbers from each kernel.
  virtual void regular_samples(vnl_vector<double>& x);

  //: Reseeds the static random number generator (one per derived class)
  virtual void reseed(unsigned long);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_sampler* clone() const;
};

#endif // pdf1d_gaussian_kernel_pdf_sampler_h_
