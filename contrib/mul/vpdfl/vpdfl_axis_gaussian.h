// This is mul/vpdfl/vpdfl_axis_gaussian.h
#ifndef vpdfl_axis_gaussian_h
#define vpdfl_axis_gaussian_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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

#include <vpdfl/vpdfl_pdf_base.h>
#include <vcl_iosfwd.h>

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
  virtual ~vpdfl_axis_gaussian();

  void set(const vnl_vector<double>& mean, const vnl_vector<double>& var);

  //: Constant offset for log probability
  double log_k() const { return log_k_; }

  //: SD for each dimension
  const vnl_vector<double>& sd() const { return sd_; }

  //: Log of probability density at x
  virtual double log_p(const vnl_vector<double>& x) const;

  //: Gradient and value of PDF at x
  //  Computes gradient of PDF at x, and returns the prob at x in p
  virtual void gradient(vnl_vector<double>& g,
                        const vnl_vector<double>& x,
                        double& p) const;

  //: Create a sampler object on the heap
  // Caller is responsible for deletion.
  virtual vpdfl_sampler_base* new_sampler() const;

  //: Compute threshold for PDF to pass a given proportion
  virtual double log_prob_thresh(double pass_proportion) const;

  //: Compute nearest point to x which has a density above a threshold
  //  If log_p(x)>log_p_min then x unchanged.  Otherwise x is moved
  //  (typically up the gradient) until log_p(x)>=log_p_min.
  // \param x This may be modified to the nearest plausible position.
  // \param log_p_min lower threshold for log_p(x)
  virtual void nearest_plausible(vnl_vector<double>& x, double log_p_min) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer.
  // Caller is responsible for deletion
  virtual vpdfl_pdf_base* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // vpdfl_axis_gaussian_h
