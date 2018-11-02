// This is mul/mbl/mbl_rvm_regression_builder.cxx
#include <cmath>
#include <iostream>
#include <algorithm>
#include "mbl_rvm_regression_builder.h"
//:
// \file
// \brief Object to train Relevance Vector Machines for regression
// \author Tim Cootes

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/algo/vnl_svd.h>
#include <mbl/mbl_matxvec.h>
#include <mbl/mbl_matrix_products.h>
#include <cassert>

//=======================================================================
// Note on indexing
// index[i] gives the index (from 0..n-1) of the selected vectors
// The offset weight is always included.
// The alpha's thus only apply to the n vector weights, not the offset
//=======================================================================
// Dflt ctor
//=======================================================================

mbl_rvm_regression_builder::mbl_rvm_regression_builder() = default;

//=======================================================================
// Destructor
//=======================================================================

mbl_rvm_regression_builder::~mbl_rvm_regression_builder() = default;

//: Compute design matrix F from subset of elements in kernel matrix
void mbl_rvm_regression_builder::design_matrix(const vnl_matrix<double>& K,
                                               const std::vector<int>& index,
                                               vnl_matrix<double>& F)
{
  unsigned n=index.size();
  unsigned ns=K.rows();
  F.set_size(ns,n+1);
  for (unsigned i=0;i<ns;++i)
  {
    F(i,0)=1.0;
    for (unsigned j=0;j<n;++j)
    {
      F(i,j+1)=K(i,index[j]);
    }
  }
}

//: Train RVM given a set of vectors and set of target values
// Uses gaussian kernel function with variance var
// \param data[i] training vectors
// \param targets[i] gives value at vector i
// \param index returns indices of selected vectors
// \param weights returns weights for selected vectors
// \param error_var returns variance term for gaussian kernel
void mbl_rvm_regression_builder::gauss_build(
             mbl_data_wrapper<vnl_vector<double> >& data,
             double var, const vnl_vector<double>& targets,
             std::vector<int>& index,
             vnl_vector<double>& weights,
             double &error_var)
{
  assert(data.size()==targets.size());
  unsigned n = data.size();
  vnl_matrix<double> K(n,n);
  double k = -1.0/2*var;
  // Construct kernel matrix
  for (unsigned i=1;i<n;++i)
  {
    data.set_index(i);
    vnl_vector<double> vi = data.current();
    for (unsigned j=0;j<i;++j)
    {
      data.set_index(j);
      double d = std::exp(k*vnl_vector_ssd(vi,data.current()));
      K(i,j)=d; K(j,i)=d;
    }
  }
  for (unsigned i=0;i<n;++i) K(i,i)=1.0;

  build(K,targets,index,weights,error_var);
}

//: Perform one iteration of optimisation
bool mbl_rvm_regression_builder::update_step(const vnl_matrix<double>& F,
                                             const vnl_vector<double>& targets,
                                             const std::vector<int>& index0,
                                             const std::vector<double>& alpha0,
                                             double error_var0,
                                             std::vector<int>& index,
                                             std::vector<double>& alpha,
                                             double &error_var)
{
  unsigned n0 = alpha0.size();
  assert(F.rows()==targets.size());
  assert(F.cols()==n0+1);
  vnl_matrix<double> K_inv;
  mbl_matrix_product_at_b(K_inv,F,F);  // K_inv=F'F
  K_inv/=error_var0;
  for (unsigned i=0;i<n0;++i) K_inv(i+1,i+1)+=alpha0[i];
  // K_inv = F'F/var + diag(alpha0)

  vnl_svd<double> svd(K_inv);
  S_ = svd.inverse();

  vnl_vector<double> t2(n0+1);
  mbl_matxvec_prod_vm(targets,F,t2);  // t2=F'targets  (n+1)
  mbl_matxvec_prod_mv(S_,t2,mean_wts_);     // mean=S*t2 (n+1)
  mean_wts_/=error_var0;

#if 0
  // ---------------------
  // Estimate p(t|alpha,var)
  vnl_vector<double> a_inv(n0+1);
  a_inv[0]=0.0;
  for (unsigned i=0;i<n0;++i) a_inv[i+1]=1.0/alpha0[i];
  vnl_matrix<double> FAF;
  mbl_matrix_product_adb(FAF,F,a_inv,F.transpose());
  for (unsigned i=0;i<FAF.rows();++i) FAF(i,i)+=1.0/error_var0;
  vnl_svd<double> FAFsvd(FAF);
  vnl_matrix<double> FAFinv=FAFsvd.inverse();
  vnl_vector<double> Xt=FAFinv*targets;
  double M = dot_product(Xt,targets);
  double det=FAFsvd.determinant_magnitude();
  std::cout<<"M="<<M<<"  -log(p)="<<M+std::log(det)<<std::endl;
  // ---------------------
#endif // 0

  // Compute new alphas and eliminate very large values
  alpha.resize(0);
  index.resize(0);
  double sum=0.0;
  double change=0.0;
  for (unsigned i=0;i<n0;++i)
  {
    double a=std::max(0.0,1.0-alpha0[i]*S_(i+1,i+1));
    sum+=a;
    if (std::fabs(mean_wts_[i+1])<1e-4) continue;
    double mi2 = mean_wts_[i+1]*mean_wts_[i+1];
    a/=mi2;

    if (a>1e8) continue;

    alpha.push_back(a);
    index.push_back(index0[i]);
    change+=std::fabs(a-alpha0[i]);
  }
  // Update estimate of error_var
  vnl_vector<double> Fm;
  mbl_matxvec_prod_mv(F,mean_wts_,Fm);     // Fm=F*mean
  double sum_sqr_error=vnl_vector_ssd(targets,Fm);
  error_var = sum_sqr_error/(targets.size()-sum);
// std::cout<<"Sum sqr error = "<<sum_sqr_error<<std::endl;
  change+=std::fabs(error_var-error_var0);

  // Decide if optimisation completed
  if (alpha.size()!=alpha0.size()) return true;
  return change/n0 > 0.01;
}

//: Train RVM given a distance matrix and set of target values
// \param kernel_matrix (i,j) element gives kernel function between i and j training vectors
// \param targets[i] gives value at vector i
// \param index returns indices of selected vectors
// \param weights returns weights for selected vectors
// \param error_var returns variance term for gaussian kernel
void mbl_rvm_regression_builder::build(
             const vnl_matrix<double>& kernel_matrix,
             const vnl_vector<double>& targets,
             std::vector<int>& index,
             vnl_vector<double>& weights,
             double &error_var)
{
  assert(kernel_matrix.rows()==targets.size());
  assert(kernel_matrix.cols()<=targets.size());
  unsigned n0=kernel_matrix.cols();

  // Initialise to use all n0 samples with equal weights
  index.resize(n0);
  std::vector<double> alpha(n0),new_alpha;
  std::vector<int> new_index;
  for (unsigned i=0;i<n0;++i)  { index[i]=i; alpha[i]=1e-4; }
  error_var = 0.01;
  double new_error_var;

  vnl_matrix<double> F;
  design_matrix(kernel_matrix,index,F);
  int max_its=500;
  int n_its=0;
  while (update_step(F,targets,index,alpha,error_var,
                     new_index,new_alpha,new_error_var)  && n_its<max_its)
  {
    index    = new_index;
    alpha    = new_alpha;
    error_var= new_error_var;
    design_matrix(kernel_matrix,index,F);
    n_its++;
  }

  if (n_its>=max_its)
    std::cerr<<"mbl_rvm_regression_builder::build() Too many iterations. Convergence failure.\n";

  weights=mean_wts_;
}
