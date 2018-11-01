// This is mul/vpdfl/vpdfl_axis_gaussian.h
#ifndef vpdfl_axis_gaussian_h
#define vpdfl_axis_gaussian_h
//:
// \file
// \brief Multi-variate Gaussian PDF, with a diagonal covariance matrix
// \author Tim Cootes
// \date 12-Apr-2001
// \verbatim
//  Modifications
//   19 April 2001 IMS - Moved sampling functionality into a sampler class
//   20 April 2001 IMS - Implemented log_prob_thresh()
//   20 April 2001 IMS - Implemented log_p_min in nearest_plausible
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <vpdfl/vpdfl_pdf_base.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Multi-variate Gaussian PDF, with a diagonal covariance matrix
class vpdfl_axis_gaussian : public vpdfl_pdf_base
{
  double log_k_;
  vnl_vector<double> sd_;

  void calcLogK();
  void calcSD();

  //: Calculate (x-mu)' * Sigma^-1 * (x-mu)
  double dx_sigma_dx(const vnl_vector<double> &x) const;

 public:

  // Dflt ctor
  vpdfl_axis_gaussian();

  // Destructor
  ~vpdfl_axis_gaussian() override;

  void set(const vnl_vector<double>& mean, const vnl_vector<double>& var);

  //: Constant offset for log probability
  double log_k() const { return log_k_; }

  //: SD for each dimension
  const vnl_vector<double>& sd() const { return sd_; }

  //: Log of probability density at x
  double log_p(const vnl_vector<double>& x) const override;

  //: Gradient and value of PDF at x
  //  Computes gradient of PDF at x, and returns the prob at x in p
  void gradient(vnl_vector<double>& g,
                        const vnl_vector<double>& x,
                        double& p) const override;

  //: Gradient and value of log(p(x)) at x
  //  Computes gradient df/dx of f(x)=log(p(x)) at x.
  //  Result is vector of same dimensionality as x.
  void gradient_logp(vnl_vector<double>& g,
                             const vnl_vector<double>& x) const override;

  //: Create a sampler object on the heap
  // Caller is responsible for deletion.
  vpdfl_sampler_base* new_sampler() const override;

  //: Compute threshold for PDF to pass a given proportion
  double log_prob_thresh(double pass_proportion) const override;

  //: Compute nearest point to x which has a density above a threshold
  //  If log_p(x)>log_p_min then x unchanged.  Otherwise x is moved
  //  directly towards the mean (i.e. to the nearest plausible point using a
  //  Mahalanobis distance) until log_p(x)=log_p_min.
  // \param x This may be modified to the nearest plausible position.
  // \param log_p_min lower threshold for log_p(x)
  void nearest_plausible(vnl_vector<double>& x, double log_p_min) const override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer.
  // Caller is responsible for deletion
  vpdfl_pdf_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // vpdfl_axis_gaussian_h
