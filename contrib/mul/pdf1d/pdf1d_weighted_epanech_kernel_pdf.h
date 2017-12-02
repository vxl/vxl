// This is mul/pdf1d/pdf1d_weighted_epanech_kernel_pdf.h
#ifndef pdf1d_weighted_epanech_kernel_pdf_h
#define pdf1d_weighted_epanech_kernel_pdf_h

//:
// \file
// \brief Univariate Weighted Epanechnikov kernel PDF
// \author Ian Scott

#include <pdf1d/pdf1d_pdf.h>
#include <pdf1d/pdf1d_weighted_kernel_pdf.h>

//: Univariate Epanechnikov kernel PDF $k(x)=0.75(1-0.2x^2)/\sqrt{2}$
class pdf1d_weighted_epanech_kernel_pdf : public pdf1d_weighted_kernel_pdf
{
 public:
  //: Dflt ctor
  pdf1d_weighted_epanech_kernel_pdf();

  //: Define n kernels centred at i*sep (i=0..n-1)
  pdf1d_weighted_epanech_kernel_pdf(int n, double sep, double width);

  //: Destructor
  virtual ~pdf1d_weighted_epanech_kernel_pdf();

  //: Create a sampler object on the heap
  // Caller is responsible for deletion.
  virtual pdf1d_sampler* new_sampler() const;

  //: Probability density at x
  virtual double operator()(double x) const;

  //: Log of probability density at x
  virtual double log_p(double x) const;

  //: Cumulative Probability (P(x'<x) for x' drawn from the distribution)
  virtual double cdf(double x) const;

  //: Return true if cdf() uses an analytic implementation
  virtual bool cdf_is_analytic() const;

  //: Gradient of PDF at x
  virtual double gradient(double x, double& p) const;

  //: Compute nearest point to x which has a density above a threshold
  //  If log_p(x)>log_p_min then x returned unchanged.  Otherwise x is moved
  //  (typically up the gradient) until log_p(x)>=log_p_min.
  // \param x This may be modified to the nearest plausible position.
  // \param log_p_min lower threshold for log_p(x)
  virtual double nearest_plausible(double x, double log_p_min) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual std::string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(std::string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_pdf* clone() const;

  friend class pdf1d_weighted_epanech_kernel_sampler;
};

#endif // pdf1d_weighted_epanech_kernel_pdf_h
