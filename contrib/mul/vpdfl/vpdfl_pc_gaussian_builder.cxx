// This is mul/vpdfl/vpdfl_pc_gaussian_builder.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Interface for Multi-variate Principle Component gaussian PDF Builder.
// \author Ian Scott
// \date 21-Jul-2000
//
// Modifications
// \verbatim
// 23 April 2001 IMS - Ported to VXL
// \endverbatim

//=======================================================================
// inclusions
//=======================================================================

#include "vpdfl_pc_gaussian_builder.h"

#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <mbl/mbl_data_wrapper.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_c_vector.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vpdfl/vpdfl_gaussian_builder.h>
#include <vpdfl/vpdfl_pdf_base.h>
#include <vpdfl/vpdfl_pc_gaussian.h>

//=======================================================================

vpdfl_pc_gaussian_builder::vpdfl_pc_gaussian_builder() :
  partitionMethod_(vpdfl_pc_gaussian_builder::fixed),
  proportionOfVariance_(0),
  fixed_partition_(1)
{
}

//=======================================================================

vpdfl_pc_gaussian_builder::~vpdfl_pc_gaussian_builder()
{
}

//=======================================================================

//: Use proportion of variance to decide on the number of principle components.
// Specify the proportion (between 0 and 1).
// The default setting uses a fixed number of principle components.
void vpdfl_pc_gaussian_builder::set_proportion_partition( double proportion)
{
  assert(proportion >= 0.0);
  assert(proportion <= 1.0);

  proportionOfVariance_ = proportion;
  partitionMethod_ = proportionate;
}

//=======================================================================

//: Set the number of principle components when using fixed partition.
void vpdfl_pc_gaussian_builder::set_fixed_partition(int n_principle_components)
{
  assert(n_principle_components >=0);
  fixed_partition_ = n_principle_components;
  partitionMethod_ = vpdfl_pc_gaussian_builder::fixed;
}

//=======================================================================

vpdfl_pc_gaussian& vpdfl_pc_gaussian_builder::gaussian(vpdfl_pdf_base& model) const
{
    // need a vpdfl_gaussian
  assert(model.is_class("vpdfl_pc_gaussian"));
  return static_cast<vpdfl_pc_gaussian&>( model);
}

//=======================================================================

vpdfl_pdf_base* vpdfl_pc_gaussian_builder::new_model() const
{
  return new vpdfl_pc_gaussian();
}

//=======================================================================

void vpdfl_pc_gaussian_builder::build(vpdfl_pdf_base& model,
                                      const vnl_vector<double>& mean) const
{
  vpdfl_pc_gaussian& g = gaussian(model);
  int n = mean.size();

  // Generate an identity matrix for eigenvectors
  vnl_matrix<double> P(n,n);
  P.fill(0);
  P.fill_diagonal(1.0);

  g.set(mean,P,vnl_vector<double>(0), min_var());
}

#if 0 // this doesn't work
    //: Build model from mean and covariance
void vpdfl_pc_gaussian_builder::buildFromCovar(vpdfl_pc_gaussian& g,
                                               const vnl_vector<double>& mean,
                                               const vnl_matrix<double>& S,
                                               unsigned nPrinComps) const
{
  int n = mean.size();
  vnl_matrix<double> evecs;
  vnl_vector<double> evals;

  NR_CalcSymEigens(S,evecs,evals,0);
  vnl_vector<double> principleEVals(nPrinComps);

  // Apply threshold to variance
  for (int i=1;i<=nPrinComps;++i)
    if (evals(i)<min_var())
      principleEVals(i)=min_var();
    else
      principleEVals(i)=evals(i);

  double sum = 0.0; // The sum of the complementary space eigenvalues.
  for (int i=nPrinComps+1; i <= n; i++)
    sum += evals(i);

    // The Eigenvalue of the complementary space basis vectors
  double complementaryEVals = sum / (n - nPrinComps);

  if (complementaryEVals < min_var()) complementaryEVals = min_var();

  g.set(mean, evecs, principleEVals, complementaryEVals);
}
#endif


//: replace any eigenvalues that are less than zero, with zero.
// Small negative eigenvalues can be generated due to rounding errors.
// This function assumes that the eigenvalues are stored in descending order.
static void eValsFloorZero(vnl_vector<double> &v)
{
  int n = v.size();
  double *v_data = v.data_block();
  int i=n-1;
  while (i && v_data[i] < 0.0)
  {
    v_data[i]=0.0;
    i--;
  }
}


void vpdfl_pc_gaussian_builder::build(vpdfl_pdf_base& model,
                                      mbl_data_wrapper<vnl_vector<double> >& data) const
{
  vpdfl_pc_gaussian& g = gaussian(model);

  unsigned long n_samples = data.size();
  assert (n_samples>=2L);

  int n = data.current().size();

  vnl_vector<double> mean;
//vnl_matrix<double> evecs;
//vnl_vector<double> evals;
  vnl_matrix<double> evecs(n,n);
  vnl_vector<double> evals(n);
  vnl_matrix<double> S;

  meanCovar(mean,S,data);

  vnl_symmetric_eigensystem_compute(S, evecs, evals);
  // eigenvalues are lowest first here
  evals.flip();
  evecs.fliplr();
  // eigenvalues are highest first now

  int n_principle_components = decide_partition(evals, n_samples, 0);

  vnl_vector<double> principleEVals(n_principle_components);

  // Apply threshold to variance
  for (int i=0;i<n_principle_components;++i)
    if (evals(i)<min_var())
      principleEVals(i)=min_var();
    else
      principleEVals(i)=evals(i);

  double eVsum = 0.0; // The sum of the complementary space eigenvalues.
  for (int i=n_principle_components; i < n; i++)
    eVsum += evals(i);

    // The Eigenvalue of the complementary space basis vectors
  double complementaryEVals = eVsum / (n - n_principle_components);

  if (complementaryEVals < min_var()) complementaryEVals = min_var();

  g.set(mean, evecs, principleEVals, complementaryEVals);
}

//: Computes mean and covariance of given data
void vpdfl_pc_gaussian_builder::mean_covar(vnl_vector<double>& mean, vnl_matrix<double>& S,
                                           mbl_data_wrapper<vnl_vector<double> >& data) const
{
  unsigned long n_samples = data.size();

  assert (n_samples!=0L);

  int n_dims = data.current().size();
  vnl_vector<double> sum(n_dims);
  sum.fill(0);

  S.set_size(0,0);

  data.reset();
  for (unsigned long i=0;i<n_samples;i++)
  {
    sum += data.current();
    updateCovar(S,data.current(),1.0);

    data.next();
  }

  mean = sum;
  mean/=n_samples;
  S/=n_samples;
  updateCovar(S,mean,-1.0);
}


void vpdfl_pc_gaussian_builder::weighted_build(vpdfl_pdf_base& model,
                                               mbl_data_wrapper<vnl_vector<double> >& data,
                                               const vcl_vector<double>& wts) const
{
  vpdfl_pc_gaussian& g = gaussian(model);

  unsigned long n_samples = data.size();

  if (n_samples<2L)
  {
    vcl_cerr<<"vpdfl_gaussian_builder::weighted_build() Too few examples available.\n";
    vcl_abort();
  }

  data.reset();
  const int n = data.current().size();
  vnl_vector<double> sum(n);
  sum.fill(0.0);
  vnl_matrix<double> evecs(n,n);
  vnl_vector<double> evals(n);
  vnl_matrix<double> S;
  double w_sum = 0.0;
  double w;
  unsigned actual_samples = 0;

  for (unsigned long i=0;i<n_samples;i++)
  {
    w = wts[i];
    if (w != 0.0) // Common case - save time.
    {
      actual_samples ++;
      w_sum += w;
      data.current().assert_finite();
      sum += w*data.current();
      updateCovar(S,data.current(),w);
    }
    data.next();
  }

  updateCovar(S,sum,-1.0/w_sum);
  S*=actual_samples/((actual_samples - 1) *w_sum);
  sum/=w_sum;
  // now sum = weighted mean
  // and S = weighted covariance corrected for unbiased rather than ML result.


  vnl_symmetric_eigensystem_compute(S, evecs, evals);
  // eigenvalues are lowest first here
  evals.flip();
  evecs.fliplr();
  // eigenvalues are highest first now

#if 0
  vcl_cerr << 'S' << S <<vcl_endl
           << "evals" << evals <<vcl_endl
           << "evecs" << evecs <<vcl_endl;
#endif

  eValsFloorZero(evals);

  int n_principle_components = decide_partition(evals, n);

  vnl_vector<double> principleEVals(n_principle_components);

  // Apply threshold to variance
  for (int i=0;i<n_principle_components;++i)
    if (evals(i)<min_var())
      principleEVals(i)=min_var();
    else
      principleEVals(i)=evals(i);
  double eVsum = 0.0; // The sum of the complementary space eigenvalues.
  for (int i=n_principle_components; i < n; i++)
    eVsum += evals(i);

    // The Eigenvalue of the complementary space basis vectors
  double complementaryEVals;
  if (n_principle_components != n) // avoid divide by 0
    complementaryEVals = eVsum / (n - n_principle_components);
  else
    complementaryEVals = 0.0; // actual could be any value.

  if (complementaryEVals < min_var()) complementaryEVals = min_var();

  g.set(sum, evecs, principleEVals, complementaryEVals);
}


//: Decide where to partition an Eigenvector space
// Returns the number of principle components to be used.
// Pass in the Eigenvalues (eVals), the number of samples
// that went to make up this Gaussian (nSamples), and the noise floor
// for the dataset. The method may use simplified algorithms if
// you indicate that the number of samples or noise floor is unknown
// (by setting the latter parameters to 0.)
unsigned vpdfl_pc_gaussian_builder::decide_partition(const vnl_vector<double>& eVals, unsigned /*nSamples*/ /*=0*/,
  double /*noise*/ /*=0.0*/) const
{
  assert (eVals.size() > 0);
  if (partitionMethod_ == vpdfl_pc_gaussian_builder::fixed)
  {
    return vnl_math_min(eVals.size(), (unsigned)fixed_partition()+1);;
  }
  else if (partitionMethod_ == proportionate)
  {
    double sum = vnl_c_vector<double>::sum(eVals.data_block(), eVals.size());
    assert (proportionOfVariance_ < 1.0 && proportionOfVariance_ > 0.0);
    double stopWhen = sum * proportionOfVariance_;
    sum = eVals(0);
    unsigned i=0;
    while (sum <= stopWhen)
    {
      i++;
      sum += eVals(i);
    }
    return i;
  }
  else
  {
    vcl_cerr << "vpdfl_pc_gaussian_builder::decide_partition(): Unexpected partition method: "
             << (short)partitionMethod_ <<vcl_endl;
    vcl_abort();
    return 0;
  }
}

//=======================================================================

vcl_string vpdfl_pc_gaussian_builder::is_a() const
{
  static vcl_string class_name_ = "vpdfl_pc_gaussian_builder";
  return class_name_;
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_pc_gaussian_builder::is_class(vcl_string const& s) const
{
  return vpdfl_gaussian_builder::is_class(s) || s==vpdfl_pc_gaussian_builder::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short vpdfl_pc_gaussian_builder::version_no() const
{
  return 2;
}

//=======================================================================
// Method: clone
//=======================================================================

vpdfl_builder_base* vpdfl_pc_gaussian_builder::clone() const
{
  return new vpdfl_pc_gaussian_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void vpdfl_pc_gaussian_builder::print_summary(vcl_ostream& os) const
{
  vpdfl_gaussian_builder::print_summary(os);
}

//=======================================================================
// Method: save
//=======================================================================

void vpdfl_pc_gaussian_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, is_a());
  vsl_b_write(bfs, version_no());
  vpdfl_gaussian_builder::b_write(bfs);
  vsl_b_write(bfs,(short)partitionMethod_);
  vsl_b_write(bfs, proportionOfVariance_);
  vsl_b_write(bfs, fixed_partition_);
}

//=======================================================================
// Method: load
//=======================================================================

void vpdfl_pc_gaussian_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  vcl_string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_pc_gaussian_builder &)\n"
             << "           Attempted to load object of type "
             << name <<" into object of type " << is_a() << vcl_endl;
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }

  short temp;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case 1:
      vpdfl_gaussian_builder::b_read(bfs);
      vsl_b_read(bfs, temp);
      partitionMethod_ = partitionMethods(temp);
      vsl_b_read(bfs, proportionOfVariance_);
      fixed_partition_ = 75;
      break;
    case 2:
      vpdfl_gaussian_builder::b_read(bfs);
      vsl_b_read(bfs, temp);
      partitionMethod_ = partitionMethods(temp);
      vsl_b_read(bfs, proportionOfVariance_);
      vsl_b_read(bfs, fixed_partition_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_pc_gaussian_builder &)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
