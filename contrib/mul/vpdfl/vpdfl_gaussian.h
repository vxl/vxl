// This is mul/vpdfl/vpdfl_gaussian.h
#ifndef vpdfl_gaussian_h
#define vpdfl_gaussian_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Multi-variate gaussian PDF with arbitrary axes.
// \author Tim Cootes
// \date 16-Oct-98
// \verbatim
//  Modifications
//   IMS   Converted to VXL 18 April 2000
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <vpdfl/vpdfl_pdf_base.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Class for multi-variate gaussians with arbitrary axes.
//  Covariance matrix is represented by its eigenvectors and values
class vpdfl_gaussian : public vpdfl_pdf_base
{
  vnl_matrix<double> evecs_;
  vnl_vector<double> evals_;
  double log_k_;
  void calcLogK();

  //: Calculate (x-mu)' * Sigma^-1 * (x-mu)
  // This is the Mahalanobis distance squared from the mean.
  double dx_sigma_dx(const vnl_vector<double>& x) const;

 protected: // Workspace may be accessed by sub-classes
  //: Workspace
  // The difference between an input vector and the mean
  mutable vnl_vector<double> dx_;
  //: Workspace
  // Usually the input vector after normalisation.
  mutable vnl_vector<double> b_;


 public:
  //: Dflt ctor
  vpdfl_gaussian();

  //: Destructor
  ~vpdfl_gaussian() override;

  //: Initialise
  // WARNING - the error checking for inconsistent parameters is not
  // foolproof.
  void set(const vnl_vector<double>& mean,
           const vnl_vector<double>& variance,
           const vnl_matrix<double>& evecs,
           const vnl_vector<double>& evals);

  //: Initialise safely
  // Calculates the variance, and checks that
  // the Eigenvalues are ordered and the Eigenvectors are unit normal
  //
  // Turn off assertions to remove error checking.
  //
  // This functions should only be used by builders.
  virtual void set(const vnl_vector<double>& mean,
                   const vnl_matrix<double>& evecs,
                   const vnl_vector<double>& evals);

  //: Initialise from mean and covariance matrix
  //  Note, eigenvectors computed from covar, and those corresponding
  //  to evals smaller than min_eval are truncated
  //
  // This functions should only be used by builders.
  void set(const vnl_vector<double>& mean,
           const vnl_matrix<double>& covar,
           double min_eval = 1e-6);

  //: Modify just the mean of the distribution
  // This functions should only be used by builders.
  void set_mean(const vnl_vector<double>& mean);

  //: Eigenvectors of covariance matrix
  // List ordering corresponds to eVals();
  const vnl_matrix<double>& eigenvecs() const { return evecs_; }

  //: Eigenvalues of covariance matrix
  // The list is ordered - largest Eigenvalues first.
  const vnl_vector<double>& eigenvals() const { return evals_; }

  //: The Covariance matrix of the Gaussian.
  // This value is calculated on the fly each time so calling this function
  // may not be very efficient
  vnl_matrix<double> covariance() const;

  //: log of normalisation constant for gaussian
  double log_k() const { return log_k_; }

  //: Create a sampler object on the heap
  // Caller is responsible for deletion.
  vpdfl_sampler_base* new_sampler() const override;

  //: Log of probability density at x
  // This value is also the Normalised Mahalanobis distance
  // from the centroid to the given vector.
  double log_p(const vnl_vector<double>& x) const override;

  //: Gradient of PDF at x
  void gradient(vnl_vector<double>& g,
                        const vnl_vector<double>& x,
                        double& p) const override;

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

  //: Create a copy on the heap and return base class pointer
  vpdfl_pdf_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;


  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // vpdfl_gaussian_h
