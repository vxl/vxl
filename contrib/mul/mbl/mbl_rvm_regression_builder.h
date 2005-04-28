// This is mul/mbl/mbl_rvm_regression_builder.h
#ifndef mbl_rvm_regression_builder_h_
#define mbl_rvm_regression_builder_h_
//:
// \file
// \brief Object to train Relevence Vector Machines for regression
// \author Tim Cootes

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <mbl/mbl_data_wrapper.h>

//=======================================================================
//: Object to train Relevence Vector Machines for regression.
// Trains Relevance Vector Machines (see papers by Michael Tipping)
// for regression.
class mbl_rvm_regression_builder
{
private:
  //: Record of mean weights
  vnl_vector<double> mean_wts_;

  //: Record of covariance in weights
  vnl_matrix<double> S_;

  //: Compute design matrix F from subset of elements in kernel matrix
  //  Uses gaussian distance expression with variance var
  void design_matrix(const vnl_matrix<double>& kernel_matrix,
                     const vcl_vector<int>& index,
                     vnl_matrix<double>& F);

  //: Perform one iteration of optimisation
  bool update_step(const vnl_matrix<double>& design_matrix,
                   const vnl_vector<double>& targets,
                   const vcl_vector<int>& index0,
                   const vcl_vector<double>& alpha0,
                   double sqr_width0,
                   vcl_vector<int>& index,
                   vcl_vector<double>& alpha,
                   double &sqr_width);
public:
    //: Dflt ctor
  mbl_rvm_regression_builder();

    //: Destructor
  virtual ~mbl_rvm_regression_builder();

    //: Train RVM given a set of vectors and set of target values
    // \param data[i] training vectors
    // \param targets[i] gives value at vector i
    // \param index returns indices of selected vectors
    // \param weights returns weights for selected vectors
    // \param sqr_width returns variance term for gaussian kernel
  void gauss_build(mbl_data_wrapper<vnl_vector<double> >& data,
             double var,
             const vnl_vector<double>& targets,
             vcl_vector<int>& index,
             vnl_vector<double>& weights,
             double &sqr_width);

    //: Train RVM given a distance matrix and set of target values
    // \param kernel_matrix (i,j) element gives kernel function between i and j training vectors
    // \param targets[i] gives value at vector i
    // \param index returns indices of selected vectors
    // \param weights returns weights for selected vectors
    // \param sqr_width returns variance term for gaussian kernel
  void build(const vnl_matrix<double>& kernel_matrix,
             const vnl_vector<double>& targets,
             vcl_vector<int>& index,
             vnl_vector<double>& weights,
             double &sqr_width);

};

#endif


