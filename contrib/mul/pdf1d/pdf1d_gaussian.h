// This is mul/pdf1d/pdf1d_gaussian.h
#ifndef pdf1d_gaussian_h
#define pdf1d_gaussian_h


//:
// \file
// \brief Univariate gaussian PDF
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <pdf1d/pdf1d_pdf.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Class for univariate gaussian.
class pdf1d_gaussian : public pdf1d_pdf
{
  double sd_;
  double k_;
  double log_k_;
  void calc_log_k();

 public:
  //: Dflt ctor (creates unit gaussian centred at zero)
  pdf1d_gaussian();

  //: Dflt ctor (creates gaussian with given mean, variance)
  pdf1d_gaussian(double mean, double variance);

  //: Destructor
  ~pdf1d_gaussian() override;

  //: Return standard deviation
  double sd() const { return sd_; }

  //: Initialise
  void set(double mean, double variance);

  //: Modify just the mean of the distribution
  // This functions should only be used by builders.
  void set_mean(double mean);

  //: log of normalisation constant for gaussian
  double log_k() const { return log_k_; }

  //: Create a sampler object on the heap
  // Caller is responsible for deletion.
  pdf1d_sampler* new_sampler() const override;

  //: Log of probability density at x
  // This value is also the Normalised Mahalanobis distance
  // from the centroid to the given vector.
  double log_p(double x) const override;

  //: Cumulative Probability (P(x'<x) for x' drawn from the distribution)
  double cdf(double x) const override;

  //: Return true if cdf() uses an analytic implementation
  bool cdf_is_analytic() const override;

  //: Gradient of PDF at x
  double gradient(double x, double& p) const override;

  //: Compute threshold for PDF to pass a given proportion
  double log_prob_thresh(double pass_proportion) const override;

  //: Compute nearest point to x which has a density above a threshold
  //  If log_p(x)>log_p_min then x returned unchanged.  Otherwise x is moved
  //  (typically up the gradient) until log_p(x)>=log_p_min.
  // \param x This may be modified to the nearest plausible position.
  // \param log_p_min lower threshold for log_p(x)
  double nearest_plausible(double x, double log_p_min) const override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  pdf1d_pdf* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;


  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // pdf1d_gaussian_h
