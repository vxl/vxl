#ifndef pdf1d_epanech_kernel_pdf_sampler_h_
#define pdf1d_epanech_kernel_pdf_sampler_h_
//:
// \file
// \brief Implements sampling for an Epanechnikov kernel PDF
// \author Tim Cootes

#include <vnl/vnl_random.h>
#include <pdf1d/pdf1d_sampler.h>
#include <pdf1d/pdf1d_epanech_kernel_pdf.h>

class pdf1d_epanech_kernel_pdf;

//: Generates random samples from an Epanechnikov kernel estimate
class pdf1d_epanech_kernel_pdf_sampler : public pdf1d_sampler
{
 protected:
  //: The random number generator
  vnl_random rng_;
 public:

  //: Dflt ctor
  pdf1d_epanech_kernel_pdf_sampler();

  //: Destructor
  ~pdf1d_epanech_kernel_pdf_sampler() override;

  const pdf1d_epanech_kernel_pdf& epanech_kernel_pdf() const;

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

  //: Transform a unit uniform distribution x into an Epanech distribution y
  // $0 <= x <= 1  =>  -sqrt(5) <= y <= sqrt(5)$
  // Matlab found 6 solutions to
  // ${\frac {3}{20}}\,\left |{\it Dy}\right |\sqrt {5}\left (1-1/5\,{y}^{2}\right )=1$
  // The 6th, which has the correct properties is
  // $-1/2\,\sqrt [3]{-10\,\sqrt {5}x+5\,\sqrt {-5+20\,{x}^{2}}}-5/2\,
  // {\frac {1}{\sqrt [3]{-10\,\sqrt {5}x+5\,\sqrt {-5+20\,{x}^{2}}}}}-1/2\,
  // \sqrt {-1}\sqrt {3}\left (\sqrt [3]{-10\,\sqrt {5}x+5\,\sqrt {-5+20\,
  // {x}^{2}}}-5\,{\frac {1}{\sqrt [3]{-10\,\sqrt {5}x+5\,\sqrt {-5+20\,
  // {x}^{2}}}}}\right )$
  static double epan_transform(double x);
};

#endif // pdf1d_epanech_kernel_pdf_sampler_h_
