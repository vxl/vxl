#ifndef vpdfl_pc_gaussian_h
#define vpdfl_pc_gaussian_h
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \author Ian Scott
// \date 21-Jul-2000
// \brief Interface for Multi-variate Principle Component gaussian PDF.
// Modifications
// 23 April 2001 IMS - Ported to VXL

//=======================================================================
// inclusions
#include <vpdfl/vpdfl_gaussian.h>

//=======================================================================

//: Multi-variate principal Component gaussian PDF.
// The principal components can have general covariance.
// The complemenary compenents (least significant eigenvalues) have a
// spherical distribution.
class vpdfl_pc_gaussian : public vpdfl_gaussian  {
private:
    //: The number of dimensions to have full Covariance (i.e. elliptical shape)
    // The rest are assumed spherical
  unsigned partition_;

    //: The constant when calculaing the Mahal dist in the principal space.
  double log_k_principal_;

    //: Precalculate log_k_principal_.
    // Should only be called after setting all other data values.
  void calcPartLogK();

public:
    //: Dflt ctor
  vpdfl_pc_gaussian() {};

    //: Destructor
  virtual ~vpdfl_pc_gaussian() {};

  //: Number of principal components
    // i.e. the number of dimensions to have full Covariance (i.e. elliptical shape)
  unsigned n_principal_components() const {return partition_;}

    //: Return sampler object for this PDF
    //  Object is created on heap. Caller responsible for deletion.
    // The sampler object is a vpdfl_gaussian_sampler.
    // There is no vpdfl_pc_gaussian_sampler.
  virtual vpdfl_sampler_base* sampler() const;


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
    // the principal space as defined in IS_PCAGaussianBuilder::fixedPartition().
  virtual void set(const vnl_vector<double>& mean,
                   const vnl_matrix<double>& evecs,
                   const vnl_vector<double>& evals);


    //: log of normalisation constant for gaussian
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
  double log_p(const vnl_vector<double>& x) const;


    //: Version number for I/O
  short version_no() const;

    //: Name of the class
  virtual vcl_string is_a() const;

    //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

    //: Create a copy on the heap and return base class pointer
  virtual vpdfl_pdf_base* clone() const;

    //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

    //: Save class to binary file stream
    //!in: bfs: Target binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;


    //: Load class from binary file stream
    //!out: bfs: Target binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};
//=======================================================================

#endif
