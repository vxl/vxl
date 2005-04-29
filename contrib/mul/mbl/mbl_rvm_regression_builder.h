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
                   double &error_var);
public:
    //: Dflt ctor
  mbl_rvm_regression_builder();

    //: Destructor
  virtual ~mbl_rvm_regression_builder();

    //: Train RVM given a set of vectors and set of target values
    //  Resulting RVM has form f(x)=w[0]+sum w[i+1]K(x,data[index[i]])
    //  where K(x,y)=exp(-|x-y|^2/2var), and index.size() gives
    //  the number of the selected vectors.
    //  Note that on exit, weights.size()==index.size()+1
    //  weights[0] is the constant offset, and weights[i+1]
    //  corresponds to selected input vector index[i].
    // \param data[i] training vectors
    // \param targets[i] gives value at vector i
    // \param index returns indices of selected vectors
    // \param weights returns weights for selected vectors
    // \param error_var returns estimated error variance for resulting function
  void gauss_build(mbl_data_wrapper<vnl_vector<double> >& data,
             double var,
             const vnl_vector<double>& targets,
             vcl_vector<int>& index,
             vnl_vector<double>& weights,
             double &error_var);

    //: Train RVM given a distance matrix and set of target values
    //  Resulting RVM has form f(x)=w[0]+sum w[i+1]K(x,vec[index[i]])
    //  where K(x,y) is the kernel function, and index.size() gives
    //  the number of the selected vectors.
    //  Assuming the original data is vec[i], then on input we should
    //  have kernel_matrix(i,j)=K(vec[i],vec[j])
    //  Note that on exit, weights.size()==index.size()+1
    //  weights[0] is the constant offset, and weights[i+1]
    //  corresponds to selected input vector index[i].
    //
    // The algorithm involves inverting an (n+1)x(n+1) matrix,
    // where n is the number of vectors to consider as relevant
    // vectors.  This doesn't have to be all the samples.
    // For efficiency, one can provide the kernel matrix as an
    // m x n matrix, where m is the number of samples to test
    // against (targets.size()==m) but n<=m samples are to be
    // considered as potential relevant vectors (ie the first n).
    // \param kernel_matrix (i,j) element gives kernel function between i and j training vectors
    // \param targets[i] gives value at vector i
    // \param index returns indices of selected vectors
    // \param weights returns weights for selected vectors
    // \param error_var returns estimated error variance for resulting function
  void build(const vnl_matrix<double>& kernel_matrix,
             const vnl_vector<double>& targets,
             vcl_vector<int>& index,
             vnl_vector<double>& weights,
             double &error_var);

};

#endif


