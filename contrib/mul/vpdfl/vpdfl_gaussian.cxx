// This is mul/vpdfl/vpdfl_gaussian.cxx
//:
// \file
// \brief Multi-variate Gaussian PDF with arbitrary axes.
// \author Tim Cootes
// \date 16-Oct-1998
//
// \verbatim
//  Modifications
//    IMS   Converted to VXL 18 April 2000
// \endverbatim

#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include "vpdfl_gaussian.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vsl/vsl_indent.h>
#include <vnl/vnl_math.h>
#include <mbl/mbl_matxvec.h>
#include <mbl/mbl_matrix_products.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vpdfl/vpdfl_gaussian_sampler.h>
#include <vpdfl/vpdfl_sampler_base.h>
#include <vpdfl/vpdfl_prob_chi2.h>

//=======================================================================
static inline bool almostEqualsOne(double value)
{
  const double upper = 1 + 1e-06;
  const double lower = 1 - 1e-06;
  return value > lower && value < upper;
}
//=======================================================================

#ifndef NDEBUG
static inline bool columnsAreUnitNorm(const vnl_matrix<double>& vecs)
{
  const int m = vecs.rows();
  const int n = vecs.cols();
  for (int j=0; j<n; j++)
  {
    double sumsq = 0.0;
    for (int i=0; i<m; i++)
      sumsq += vnl_math::sqr(vecs(i,j));
    if (!almostEqualsOne(sumsq)) return false;
  }
  return true;
}
#endif // NDEBUG
//=======================================================================
#if 0
static bool vectorHasDescendingOrder(const vnl_vector<double>& v)
{
  int n = v.size();
  for (int i = 1; i < n; i++)
    if (v(i-1) < v(i)) return false;
  return true;
}
#endif

//=======================================================================

vpdfl_gaussian::vpdfl_gaussian() = default;

//=======================================================================

vpdfl_gaussian::~vpdfl_gaussian() = default;

//=======================================================================

void vpdfl_gaussian::calcLogK()
{
  const double *v_data = evals_.data_block();
  int n = n_dims();
  double log_v_sum = 0.0;
  for (int i=0;i<n;i++) log_v_sum+=std::log(v_data[i]);

  log_k_ = -0.5 * (n*std::log(vnl_math::twopi) + log_v_sum);
}

//: Initialise safely
// Calculates the variance, and checks that
// the Eigenvalues are ordered and the Eigenvectors are unit normal
// Turn off assertions to remove error checking.
void vpdfl_gaussian::set(const vnl_vector<double>& mean,
                         const vnl_matrix<double>& evecs,
                         const vnl_vector<double>& evals)
{
  const unsigned int m = evecs.rows();
  const unsigned int n = evecs.cols();
  assert(evals.size() == m);
  assert(mean.size() == m);

// Ensure that every Eigenvector has a unit norm;
  assert(columnsAreUnitNorm(evecs));
// Ensure that every Eigenvalues are properly ordered is a unit norm;
//assert(vectorHasDescendingOrder(evals));

  // calculate the variance
  // Notionally - apply the inverse diagonalisation to get
  // back to the Covariance matrix, and select the diagonal
  // Efficiently - Var(i) = Sum Wij * Li * Wij
  vnl_vector<double> v(m);
  for (unsigned int i=0; i<m; i++)
  {
    double &vi = v(i);
    vi = 0.0;
    for (unsigned int j=0; j<n; j++)
      vi += vnl_math::sqr(evecs(i,j)) * evals(i);
  }

  set(mean, v, evecs, evals);
}

//=======================================================================

void vpdfl_gaussian::set(const vnl_vector<double>& m,
                         const vnl_vector<double>& v,
                         const vnl_matrix<double>& evecs,
                         const vnl_vector<double>& evals)
{
  set_mean(m);
  set_variance(v);

  evecs_ = evecs;
  evals_ = evals;

  calcLogK();
}

//: Modify just the mean of the distribution
// This functions should only be used by builders.
void vpdfl_gaussian::set_mean(const vnl_vector<double>& mean)
{
  vpdfl_pdf_base::set_mean(mean);
}


//=======================================================================
//: Initialise from mean and covariance matrix
//  Note, eigenvectors computed from covar, and those corresponding
//  to evals smaller than min_eval are truncated
void vpdfl_gaussian::set(const vnl_vector<double>& mean,
                         const vnl_matrix<double>& covar,
                         double min_eval)
{
  unsigned int n = mean.size();
  assert(covar.rows()==n && covar.cols()==n);

  vnl_matrix<double> evecs;
  vnl_vector<double> evals;

  vnl_symmetric_eigensystem_compute(covar, evecs, evals);
  // eigenvalues are lowest first here
  evals.flip();
  evecs.fliplr();
  // eigenvalues are highest first now

  // Apply threshold to variance
  for (unsigned int i=0;i<n;++i)
    if (evals(i)<min_eval) evals(i)=min_eval;

  set(mean, evecs, evals);
}


//=======================================================================

vnl_matrix<double> vpdfl_gaussian::covariance() const
{
  vnl_matrix<double> Cov;
  mbl_matrix_product_adb(Cov, evecs_, evals_, evecs_.transpose());
  return Cov;
}

//=======================================================================

vpdfl_sampler_base* vpdfl_gaussian::new_sampler() const
{
  auto *i = new vpdfl_gaussian_sampler;
  i->set_model(*this);
  return i;
}

//=======================================================================

//: Calculate (x-mu)' * Sigma^-1 * (x-mu)
// This is the Mahalanobis distance squared from the mean.
double vpdfl_gaussian::dx_sigma_dx(const vnl_vector<double>& x) const
{
  unsigned int n = n_dims();
 #ifndef NDEBUG
  if (n!=x.size())
  {
    std::cerr<<"ERROR: vpdfl_gaussian::dx_sigma_dx: Target vector has "
            <<n<<" dimensions, not the required "<<n_dims()<<'\n';
    std::abort();
  }
#endif

  b_.set_size(n);

  dx_=x;
  dx_-=mean();
  // Rotate dx_ into co-ordinate frame of axes of Gaussian
  // b_ = P'dx_
  mbl_matxvec_prod_vm(dx_, eigenvecs(),b_);

  const double* b_data = b_.data_block() ;
  const double* v_data = eigenvals().data_block() ;

  double sum=0.0;

  unsigned int i=n;
  while (i-- != 0)
  {
    double db=b_data[i];
    sum+=(db*db)/v_data[i];
  }
  return sum;
}

// Probability densities:
double vpdfl_gaussian::log_p(const vnl_vector<double>& x) const
{
  return log_k() - 0.5*dx_sigma_dx(x);
}

//=======================================================================

void vpdfl_gaussian::gradient(vnl_vector<double>& g,
                              const vnl_vector<double>& x,
                              double& p) const
{
  unsigned int n = n_dims();
  dx_ = x;
  dx_ -= mean();

  if (b_.size()!=n) b_.set_size(n);

  // Rotate dx_ into co-ordinate frame of axes of Gaussian
  // b_ = P'dx_
  mbl_matxvec_prod_vm(dx_, eigenvecs(),b_);

  if (g.size()!=n) g.set_size(n);

  double* b_data = b_.data_block();
  const double* v_data = eigenvals().data_block();

  double sum=0.0;

  for (unsigned int i=0;i<n;++i)
  {
    double db=b_data[i];
    sum+=(db*db)/v_data[i];
    // Record gradient in dx_
    b_data[i]/=v_data[i];
  }

  p = std::exp(log_k() - 0.5*sum);

  b_*=(-1.0*p);

  // Rotate back into x frame
  mbl_matxvec_prod_mv(eigenvecs(),b_,g);
}

// ====================================================================

double vpdfl_gaussian::log_prob_thresh(double pass_proportion) const
{
  // The Mahalanobis distance of n-D Gaussian is distributed as Chi^2(n),
  // by definition, Chi^2 is the sum of independent Normal RVs.
  return log_k() - 0.5 * vpdfl_chi2_for_cum_prob (pass_proportion, n_dims());
}

//=======================================================================

void vpdfl_gaussian::nearest_plausible(vnl_vector<double>& x, double log_p_min) const
{
  // calculate radius of plausible region in standard deviations.
  log_p_min -= log_k();
  assert(log_p_min <0); // Check sd_limit is positive and real.
  const double sd_limit_sqr = -2.0*log_p_min;
  const double x_dist_sqr = dx_sigma_dx(x);

  unsigned int n = n_dims();

  if (sd_limit_sqr >= x_dist_sqr) return;

  const double corrective_factor = std::sqrt(sd_limit_sqr / x_dist_sqr);

  for (unsigned i=0;i<n;++i)
    x(i) = ((x(i)-mean()(i)) * corrective_factor) + mean()(i);
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string vpdfl_gaussian::is_a() const
{
  static std::string class_name_ = "vpdfl_gaussian";
  return class_name_;
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_gaussian::is_class(std::string const& s) const
{
  return vpdfl_pdf_base::is_class(s) || s==vpdfl_gaussian::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short vpdfl_gaussian::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

vpdfl_pdf_base* vpdfl_gaussian::clone() const
{
  return new vpdfl_gaussian(*this);
}

//=======================================================================
// Method: print
//=======================================================================

#if 0 // commented out
static void ShowStartVec(std::ostream& os, const vnl_vector<double>& v)
{
  int n = 3;
  if (n>v.size()) n=v.size();
  os<<'(';
  for (int i=0;i<n;++i) os<<v(i)<<' ';
  if (v.size()>n) os<<"...";
  os<<")\n";
}

static void ShowStartMat(std::ostream& os, const vnl_matrix<double>& A)
{
  os << A.rows() << " by " << A.cols() << " Matrix\n";

  int m = 3, n= 3;
  if (m>A.rows()) m=A.rows();
  if (n>A.cols()) n=A.cols();
  vsl_indent_inc(os);

  for (int i=0;i<m;++i)
  {
    os<<vsl_indent()<<'(';

    for ( int j=0; j<n; ++j)
      os<<A(i,j)<<' ';
    if (A.cols()>n) os<<"...";
    os<<")\n";
  }
  if (A.rows()>m) os <<vsl_indent()<<"(...\n";

  vsl_indent_dec(os);
}
#endif // commented out

void vpdfl_gaussian::print_summary(std::ostream& os) const
{
  vpdfl_pdf_base::print_summary(os);
  os << '\n';
  if (n_dims()!=1)
  {
    os<<vsl_indent()<<"Eigenvectors: "; vsl_print_summary(os, eigenvecs() );
    os<<vsl_indent()<<"log_k: "<< log_k_ << '\n';
//  os<<vsl_indent()<<"Covariance: "; ShowStartMat(os, covariance() );
  }
}

//=======================================================================
// Method: save
//=======================================================================

void vpdfl_gaussian::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
  vpdfl_pdf_base::b_write(bfs);
  vsl_b_write(bfs,evecs_);
  vsl_b_write(bfs,evals_);
  vsl_b_write(bfs,log_k_);
}

//=======================================================================
// Method: load
//=======================================================================

void vpdfl_gaussian::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  std::string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_gaussian &)\n"
             << "           Attempted to load object of type "
             << name <<" into object of type " << is_a() << '\n';
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vpdfl_pdf_base::b_read(bfs);
      vsl_b_read(bfs,evecs_);
      vsl_b_read(bfs,evals_);
      vsl_b_read(bfs,log_k_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_gaussian &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//==================< end of vpdfl_gaussian.cxx >====================
