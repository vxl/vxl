// This is mul/vpdfl/vpdfl_pc_gaussian.cxx
//:
// \file
// \brief Implementation of Multi-variate principal Component Gaussian PDF.
// \author Tim Cootes
// \date 21-Jul-2000
//
// \verbatim
//  Modifications
//    IMS   Converted to VXL 23 April 2000
// \endverbatim

#include <string>
#include <iostream>
#include <cstdlib>
#include "vpdfl_pc_gaussian.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_indent.h>
#include <vnl/vnl_math.h>
#include <mbl/mbl_matxvec.h>
#include <vpdfl/vpdfl_pc_gaussian_builder.h>
#include <vpdfl/vpdfl_gaussian_sampler.h>
#include <vpdfl/vpdfl_gaussian.h>

//=======================================================================

//: Dflt ctor
vpdfl_pc_gaussian::vpdfl_pc_gaussian()
  : partition_(0), log_k_principal_(0.0), partition_chooser_(nullptr)
{
}
//=======================================================================

//: Destructor
vpdfl_pc_gaussian::~vpdfl_pc_gaussian()
{
  delete partition_chooser_;
}

//=======================================================================

//: Calculate the log probability density at position x.
// You could use vpdfl_gaussian::log_p() which would give the same answer,
// but this method, only rotates into the principal components, not the entire rotated space,
// so saving considerable time.
double vpdfl_pc_gaussian::log_p(const vnl_vector<double>& x) const
{
  unsigned int m = n_principal_components();
  unsigned int n = n_dims();
  assert(x.size() == n);

  if (m+1>=n) // it is probably not worth the speed up unless we avoid calculating more than one basis vector.
    return vpdfl_gaussian::log_p(x);

  double mahalDIFS, euclidDFFS;
  get_distances(mahalDIFS, euclidDFFS, x);

  return log_k() - log_k_principal() -euclidDFFS/(2 * (eigenvals()(m+1))) - mahalDIFS;
}

//=======================================================================

//: Return Mahalanobis and Euclidean distances from centroid to input.
// Strictly it is the normalised Mahalanobis distance (-log_p()) from the input projected into the
// principal space to the centroid, and the Euclidean distance from the input
// to the input projected into the principal space.
// Also, the two values are the squares of the distances.
void vpdfl_pc_gaussian::get_distances(double &mahalDIFS, double &euclidDFFS, const vnl_vector<double>& x) const
{
  const unsigned int m = n_principal_components();
  const unsigned int n = n_dims();

  assert(x.size() == n);

  dx_ = x;
  dx_ -= mean();

  if (b_.size()!=m) b_.set_size(m);


  // Rotate dx_ into co-ordinate frame of axes of Gaussian
  // b_ = dx_' * P
  // This function will only use the first b_.size() columns of eigenvecs();
  mbl_matxvec_prod_vm(dx_, eigenvecs(), b_);

  const double* b_data = b_.data_block();
  const double* v_data = eigenvals().data_block();

  double sum=0.0;

  int i=m;
  double db, sumBSq=0.0;
  while (i--)
  {
    db = b_data[i];
    sum+=(db*db)/v_data[i];
    sumBSq+=(db*db);
  }

  mahalDIFS = - log_k_principal() + 0.5*sum;
  if ( n!=m)
  {
    i=n;
    const double* dx_data = dx_.data_block() ;
    double ddx, sumDxSq=0.0;
    while (i--)
    {
      ddx = dx_data[i];
      sumDxSq+=(ddx*ddx);
    }

    //By Pythagoras sum(squares(b_i)) + sum(squares(c_i)) = sum(squares(d_i)),
    // where b_ is d_ projected into the principal component space
    // and c_ is d_ projected into the complementary component space
    // because b_ and c_ are perpendicular.
    euclidDFFS = sumDxSq - sumBSq;
    if (euclidDFFS < 0) euclidDFFS = 0;
  }
  else
    euclidDFFS = 0.0;
}


//: Pre-calculate the constant needed when evaluating Mahalanobis Distance
void vpdfl_pc_gaussian::calcPartLogK()
{
  const double *v_data = eigenvals().data_block();
  double log_v_sum = 0.0;
  const unsigned& n = partition_;

  for (unsigned int i=0;i<n;i++) log_v_sum+=std::log(v_data[i]);

  log_k_principal_ = -0.5 * (n*std::log(vnl_math::twopi) + log_v_sum);
}


//: Initialise safely
// The partition between principal components space and complementary space is
// defined by the length of the Eigenvalues vector (evals.)
// Calculates the variance, and checks that
// the Eigenvalues are ordered and the Eigenvectors are unit normal
// Turn off assertions to remove error checking.
void vpdfl_pc_gaussian::set(const vnl_vector<double>& mean,
                            const vnl_matrix<double>& evecs,
                            const vnl_vector<double>& evals,
                            double complementEVal)
{
  partition_ = evals.size();
  // The partition from full covariance to spherical must be between 0 and the total number of dimensions
  assert (partition_ <= evecs.cols());
  // The Eigenvector matrix should be square (and full rank but we don't test for that)
  assert (evecs.cols() == evecs.rows());

  unsigned int n = evecs.cols();

  vnl_vector<double> allEVals(n);

  // Fill in the complementary space Eigenvalues
  for (unsigned int i = 0; i < partition_; i++)
  {
    allEVals(i) = evals(i);
  }
  for (unsigned int i = partition_; i < n; i++)
  {
    allEVals(i) = complementEVal;
  }
  vpdfl_gaussian::set(mean, evecs, allEVals);

  calcPartLogK();
}

//=======================================================================

//: Initialise safely as you would a vpdfl_gaussian.
// Calculates the variance, and checks that
// the Eigenvalues are ordered and the Eigenvectors are unit normal
// Turn off assertions to remove error checking.
void vpdfl_pc_gaussian::set(const vnl_vector<double>& mean,  const vnl_matrix<double>& evecs, const vnl_vector<double>& evals)
{
#ifndef NDEBUG
  if (!partition_chooser_)
  {
    std::cerr << "ERROR: vpdfl_pc_gaussian::set()\nUsing this function requires"
             << " partition_chooser_ to be set to a real builder\n\n";
    std::abort();
  }
#endif

  int n_principal_components = partition_chooser_->decide_partition(evals);
  int n = mean.size();
  vnl_vector<double> principalEVals(n_principal_components);

  // Apply threshold to variance
  for (int i=0;i<n_principal_components;++i)
    principalEVals(i)=evals(i);

  double eVsum = 0.0; // The sum of the complementary space eigenvalues.
  for (int i=n_principal_components; i < n; i++)
    eVsum += evals(i);

    // The Eigenvalue of the complementary space basis vectors
  double complementaryEVals = eVsum / (n - n_principal_components);

  set(mean, evecs, principalEVals, complementaryEVals);
}

//=======================================================================

//: Return instance of this PDF
vpdfl_sampler_base* vpdfl_pc_gaussian::sampler() const
{
  auto *i = new vpdfl_gaussian_sampler;
  i->set_model(*this);
  return i;
}

//=======================================================================

std::string vpdfl_pc_gaussian::is_a() const
{
  return std::string("vpdfl_pc_gaussian");
}

//=======================================================================

bool vpdfl_pc_gaussian::is_class(std::string const& s) const
{
  return vpdfl_gaussian::is_class(s) || s==vpdfl_pc_gaussian::is_a();
}

//=======================================================================

short vpdfl_pc_gaussian::version_no() const
{
  return 2;
}

//=======================================================================

vpdfl_pdf_base* vpdfl_pc_gaussian::clone() const
{
  return new vpdfl_pc_gaussian(*this);
}

//=======================================================================

void vpdfl_pc_gaussian::print_summary(std::ostream& os) const
{
  os << '\n' << vsl_indent() << "Partition at: " << partition_
     << "  Log(k) for principal space: "<< log_k_principal_ << '\n'
     << vsl_indent() <<  "Partition Chooser: " ;
  if (partition_chooser_) os << *partition_chooser_ << '\n';
  else os << "NULL\n";
  os << vsl_indent();
  vpdfl_gaussian::print_summary(os);
}

//=======================================================================

void vpdfl_pc_gaussian::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
  vpdfl_gaussian::b_write(bfs);
  vsl_b_write(bfs,partition_);
  vsl_b_write(bfs,log_k_principal_);
  vsl_b_write(bfs,static_cast<vpdfl_builder_base *>(partition_chooser_));
}

//=======================================================================

void vpdfl_pc_gaussian::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  std::string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_pc_gaussian &)\n"
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
      vpdfl_gaussian::b_read(bfs);
      vsl_b_read(bfs,partition_);
      vsl_b_read(bfs,log_k_principal_);
      partition_chooser_ = nullptr;
      break;
    case (2):
      vpdfl_gaussian::b_read(bfs);
      vsl_b_read(bfs,partition_);
      vsl_b_read(bfs,log_k_principal_);
      {
        vpdfl_builder_base * c = nullptr;
        vsl_b_read(bfs,c);
        assert(!c || c->is_class("vpdfl_pc_gaussian_builder"));
        partition_chooser_ = static_cast<vpdfl_pc_gaussian_builder *>(c);
      }
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_pc_gaussian &)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//=======================================================================

const vpdfl_pc_gaussian_builder * vpdfl_pc_gaussian::partition_chooser() const
{
  return partition_chooser_;
}

//=======================================================================

void vpdfl_pc_gaussian::set_partition_chooser(
  const vpdfl_pc_gaussian_builder * partition_chooser)
{
  delete partition_chooser_;
  if (partition_chooser)
    partition_chooser_ = static_cast<vpdfl_pc_gaussian_builder *>(partition_chooser->clone());
  else
    partition_chooser_ = nullptr;
}
