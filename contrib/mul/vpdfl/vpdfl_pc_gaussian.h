// This is mul/vpdfl/vpdfl_pc_gaussian.h
#ifndef vpdfl_pc_gaussian_h
#define vpdfl_pc_gaussian_h
//:
// \file
// \brief Interface for Multi-variate Principle Component Gaussian PDF.
// \author Ian Scott
// \date 21-Jul-2000
//
// \verbatim
//  Modifications
//   23 April 2001 IMS - Ported to VXL
// \endverbatim
//=======================================================================

#include <iostream>
#include <iosfwd>
#include <vpdfl/vpdfl_gaussian.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================

class vpdfl_pc_gaussian_builder;

//: Multi-variate principal Component Gaussian PDF.
// The principal components can have general covariance.
// The complementary components (least significant eigenvalues) have a
// spherical distribution.
class vpdfl_pc_gaussian : public vpdfl_gaussian
{
  //: The number of dimensions to have full Covariance (i.e. elliptical shape)
  // The rest are assumed spherical
  unsigned partition_;

  //: The constant when calculating the Mahalanobis dist in the principal space.
  double log_k_principal_;

  //: Pre-calculate log_k_principal_.
  // Should only be called after setting all other data values.
  void calcPartLogK();

  //: How the inherited set() function chooses the partition.
  // If NULL, set(const vnl_vector<double>&,const vnl_matrix<double>& evecs,const vnl_vector<double>&)
  // will fail.
  vpdfl_pc_gaussian_builder * partition_chooser_;

 public:
  //: Dflt ctor
  vpdfl_pc_gaussian();

  //: Destructor
  ~vpdfl_pc_gaussian() override;

  //: Number of principal components
  // i.e. the number of dimensions to have full Covariance (i.e. elliptical shape)
  unsigned n_principal_components() const {return partition_;}

  //: Return sampler object for this PDF
  //  Object is created on heap. Caller responsible for deletion.
  // The sampler object is a vpdfl_gaussian_sampler.
  // There is no vpdfl_pc_gaussian_sampler.
  virtual vpdfl_sampler_base* sampler() const;

  //: how the inherited set() function chooses the partition
  const vpdfl_pc_gaussian_builder * partition_chooser() const;

  //: Set the how the inherited set() function chooses the partition
  // If not NULL, a local copy of the builder will be taken.
  void set_partition_chooser(const vpdfl_pc_gaussian_builder *);

  //: Initialise safely
  // The partition between principal components space and complementary space is
  // defined by the length of the Eigenvalues vector (evals.)
  // Calculates the variance, and checks that
  // the Eigenvalues are ordered and the Eigenvectors are unit normal
  // Turn off assertions to remove error checking.
  void set(const vnl_vector<double>& mean,
           const vnl_matrix<double>& evecs,
           const vnl_vector<double>& evals,
           double complementEVal);

  //: Initialise safely as you would a vpdfl_gaussian
  // Calculates the variance, and checks that
  // the Eigenvalues are ordered and the Eigenvectors are unit normal
  // Turn off assertions to remove error checking.
  // This function takes a fully specified set of Eigenvectors and Eigenvalues, and truncates
  // the principal space as defined in partition_chooser() which must not be null.
  void set(const vnl_vector<double>& mean,
                   const vnl_matrix<double>& evecs,
                   const vnl_vector<double>& evals) override;

  //: log of normalisation constant for Gaussian
  double log_k_principal() const { return log_k_principal_; }

  //: Return Mahalanobis and Euclidean distances from centroid to input.
  // Strictly it is the normalised Mahalanobis distance (-log_p()) from the input projected into the
  // principal space to the centroid, and the Euclidean distance from the input
  // to the input projected into the principal space.
  // Also, the two values are the squares of the distances.
  void get_distances(double &mahalDIFS, double &euclidDFFS, const vnl_vector<double>& x) const;

  //: Calculate the log probability density at position x.
  // You could use vpdfl_gaussian::log_p() which would give the same answer,
  // but this method, only rotates into the principal components, not the entire rotated space,
  // so saving considerable time.
  double log_p(const vnl_vector<double>& x) const override;


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
//=======================================================================

#endif // vpdfl_pc_gaussian_h
