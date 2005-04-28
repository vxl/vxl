// This is mul/mbl/mbl_rvm_regression_builder.cxx
#include "mbl_rvm_regression_builder.h"
//:
// \file
// \brief Object to train Relevence Vector Machines for regression
// \author Tim Cootes

#include <vcl_cmath.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>
#include <mbl/mbl_matxvec.h>
#include <mbl/mbl_matrix_products.h>
#include <vcl_iostream.h>

//=======================================================================
// Note on indexing
// index[i] gives the index (from 0..n-1) of the selected vectors
// The offset weight is always included.
// The alpha's thus only apply to the n vector weights, not the offset
//=======================================================================
// Dflt ctor
//=======================================================================

mbl_rvm_regression_builder::mbl_rvm_regression_builder()
{
}

//=======================================================================
// Destructor
//=======================================================================

mbl_rvm_regression_builder::~mbl_rvm_regression_builder()
{
}

//: Compute design matrix F from subset of elements in kernel matrix
void mbl_rvm_regression_builder::design_matrix(const vnl_matrix<double>& K,
                     const vcl_vector<int>& index,
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
// \param sqr_width returns variance term for gaussian kernel
void mbl_rvm_regression_builder::gauss_build(
             mbl_data_wrapper<vnl_vector<double> >& data,
             double var, const vnl_vector<double>& targets,
             vcl_vector<int>& index,
             vnl_vector<double>& weights,
             double &sqr_width)
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
      double d = exp(k*vnl_vector_ssd(vi,data.current()));
      K(i,j)=d; K(j,i)=d;
    }
  }
  for (unsigned i=0;i<n;++i) K(i,i)=1.0;

  build(K,targets,index,weights,sqr_width);
}

//: Perform one iteration of optimisation
bool mbl_rvm_regression_builder::update_step(const vnl_matrix<double>& F,
                   const vnl_vector<double>& targets,
                   const vcl_vector<int>& index0,
                   const vcl_vector<double>& alpha0,
                   double sqr_width0,
                   vcl_vector<int>& index,
                   vcl_vector<double>& alpha,
                   double &sqr_width)
{
  unsigned n0 = alpha0.size();
  assert(F.rows()==targets.size());
  assert(F.cols()==n0+1);
  vnl_matrix<double> K_inv;
  mbl_matrix_product_at_b(K_inv,F,F);  // K_inv=F'F
  K_inv/=sqr_width0;
  for (unsigned i=0;i<n0;++i) K_inv(i+1,i+1)+=alpha0[i];
  // K_inv = F'F/var + diag(alpha0)

  vnl_svd<double> svd(K_inv);
  S_ = svd.inverse();

  vnl_vector<double> t2(n0+1);
  mbl_matxvec_prod_vm(targets,F,t2);  // t2=F'targets  (n+1)
  mbl_matxvec_prod_mv(S_,t2,mean_wts_);     // mean=S*t2 (n+1)
  mean_wts_/=sqr_width0;


//   // ---------------------
//   // Estimate p(t|alpha,var)
//   vnl_vector<double> a_inv(n0+1);
//   a_inv[0]=0.0;
//   for (unsigned i=0;i<n0;++i) a_inv[i+1]=1.0/alpha0[i];
//   vnl_matrix<double> FAF;
//   mbl_matrix_product_adb(FAF,F,a_inv,F.transpose());
//   for (unsigned i=0;i<FAF.rows();++i) FAF(i,i)+=1.0/sqr_width0;
//   vnl_svd<double> FAFsvd(FAF);
//   vnl_matrix<double> FAFinv=FAFsvd.inverse();
//   vnl_vector<double> Xt=FAFinv*targets;
//   double M = dot_product(Xt,targets);
//   double det=FAFsvd.determinant_magnitude();
//   vcl_cout<<"M="<<M<<"  -log(p)="<<M+vcl_log(det)<<vcl_endl;
//   // ---------------------

  // Compute new alphas and elliminate very large values
  alpha.resize(0);
  index.resize(0);
  double sum=0.0;
  double change=0.0;
  for (unsigned i=0;i<n0;++i)
  {
    double a=vcl_max(0.0,1.0-alpha0[i]*S_(i+1,i+1));
    sum+=a;
    if (vcl_fabs(mean_wts_[i+1])<1e-4) continue;
    double mi2 = mean_wts_[i+1]*mean_wts_[i+1];
    a/=mi2;

    if (a>1e8) continue;

    alpha.push_back(a);
    index.push_back(index0[i]);
    change+=vcl_fabs(a-alpha0[i]);
  }
  // Update estimate of sqr_width
  vnl_vector<double> Fm;
  mbl_matxvec_prod_mv(F,mean_wts_,Fm);     // Fm=F*mean
  double sum_sqr_error=vnl_vector_ssd(targets,Fm);
  sqr_width = sum_sqr_error/(targets.size()-sum);
// vcl_cout<<"Sum sqr error = "<<sum_sqr_error<<vcl_endl;
  change+=vcl_fabs(sqr_width-sqr_width0);

  // Decide if optimisation completed
  if (alpha.size()!=alpha0.size()) return true;
  return (change/n0>1e-2);
}

//: Train RVM given a distance matrix and set of target values
// \param kernel_matrix (i,j) element gives kernel function between i and j training vectors
// \param targets[i] gives value at vector i
// \param index returns indices of selected vectors
// \param weights returns weights for selected vectors
// \param sqr_width returns variance term for gaussian kernel
void mbl_rvm_regression_builder::build(
             const vnl_matrix<double>& kernel_matrix,
             const vnl_vector<double>& targets,
             vcl_vector<int>& index,
             vnl_vector<double>& weights,
             double &sqr_width)
{
  unsigned n0=targets.size();
  assert(kernel_matrix.rows()==n0);
  assert(kernel_matrix.cols()==n0);

  // Initialise to use all samples with equal weights
  index.resize(n0);
  vcl_vector<double> alpha(n0),new_alpha;
  vcl_vector<int> new_index;
  for (unsigned i=0;i<n0;++i)  { index[i]=i; alpha[i]=1e-4; }
  sqr_width = 0.01;
  double new_sqr_width;

  vnl_matrix<double> F;
  design_matrix(kernel_matrix,index,F);
  int max_its=500;
  int n_its=0;
  while (update_step(F,targets,index,alpha,sqr_width,
                     new_index,new_alpha,new_sqr_width)  && n_its<max_its)
  {
    index    = new_index;
    alpha    = new_alpha;
    sqr_width= new_sqr_width;
    design_matrix(kernel_matrix,index,F);
    n_its++;
  }

  if (n_its>=max_its)
    vcl_cerr<<"mbl_rvm_regression_builder::build() Too many iterations. Convergence failure."<<vcl_endl;

  weights=mean_wts_;
}
