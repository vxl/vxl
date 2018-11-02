#include <iostream>
#include <cstdlib>
#include "msm_ref_shape_instance.h"
//:
// \file
// \brief Representation of an instance of a shape model in ref frame.
// \author Tim Cootes

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/algo/vnl_cholesky.h>
#include <vnl/algo/vnl_svd.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <msm/msm_ref_shape_model.h>
#include <msm/msm_no_limiter.h>
#include <mbl/mbl_matxvec.h>
#include <mbl/mbl_matrix_products.h>

//=======================================================================
// Dflt ctor
//=======================================================================

msm_ref_shape_instance::msm_ref_shape_instance()
  : model_(nullptr),use_prior_(false)
{
  param_limiter_ = msm_no_limiter();
}

//=======================================================================
// Destructor
//=======================================================================

msm_ref_shape_instance::~msm_ref_shape_instance() = default;

//: Set up model (retains pointer to model)
void msm_ref_shape_instance::set_shape_model(const msm_ref_shape_model& model)
{
  model_=&model;

  b_.set_size(model.n_modes());
  b_.fill(0);

  param_limiter_ = model.param_limiter().clone();

  points_valid_=false;
}

//: Define limits on parameters (clone taken)
void msm_ref_shape_instance::set_param_limiter(const msm_param_limiter& limiter)
{
  param_limiter_ = limiter;
}

//: When true, use Gaussian prior on params in fit_to_points*
void msm_ref_shape_instance::set_use_prior(bool b)
{
  use_prior_ = b;
}

//: Define parameters
void msm_ref_shape_instance::set_params(const vnl_vector<double>& b)
{
  assert(b.size()<=model().n_modes());
  b_=b;
  points_valid_=false;
}

//: Set all shape parameters to zero
void msm_ref_shape_instance::set_to_mean()
{
  if (b_.size()==0) return;
  b_.fill(0.0);
  points_valid_=false;
}


//: Current shape in model frame (uses lazy evaluation)
const msm_points& msm_ref_shape_instance::points()
{
  if (points_valid_) return points_;

  // Need to recalculate points_
  if (b_.size()==0)
    points_.vector()=model().mean();
  else
  {
    // Use only b_.size() modes.
    mbl_matxvec_prod_mv(model().modes(),b_,points_.vector());
    points_.vector() += model().mean();
  }
  points_valid_=true;
  return points_;
}

//: Finds parameters and pose to best match to points
//  All points equally weighted.
//  If res_pt>0, and use_prior(), then effect of
//  Gaussian prior is to scale parameters by
//  mode_var/(mode_var+pt_var).
void msm_ref_shape_instance::fit_to_points(const msm_points& pts,
                                           double pt_var)
{
  // Catch case when fitting to self
  if (&pts == &points_) return;

  if (b_.size()==0) return;

  // Estimate shape parameters
  tmp_points_=pts;
  tmp_points_.vector()-=model().mean();
  mbl_matxvec_prod_vm(tmp_points_.vector(),model().modes(),b_);

  if (use_prior_ && pt_var>0.0)
  {
    const vnl_vector<double>& var = model().mode_var();
    for (unsigned i=0;i<b_.size();++i)
      b_[i]*=var[i]/(var[i]+pt_var);
  }

  param_limiter().apply_limit(b_);

  points_valid_=false;
}

void msm_calc_WP(const vnl_matrix<double>& P,
                 const vnl_vector<double>& wts,
                 unsigned n_modes,
                 vnl_matrix<double>& WP)
{
  unsigned nr = P.rows();
  assert(nr==wts.size()*2);
  WP.set_size(nr,n_modes);

  double const*const* PData = P.data_array();
  double ** WPData = WP.data_array();
  const double* w=wts.data_block();

  for (unsigned i=0;i<nr;i+=2,++w)
  {
    const double* x=PData[i];
    const double* y=PData[i+1];
    double* wx=WPData[i];
    double* wy=WPData[i+1];
    for (unsigned j=0;j<n_modes;++j)
    {
      wx[j]=w[0]*x[j];
      wy[j]=w[0]*y[j];
    }
  }
}

// Premultiply P by block diagonal composed of wt_mat
void msm_calc_WP(const vnl_matrix<double>& P,
                 const std::vector<msm_wt_mat_2d>& wt_mat,
                 unsigned n_modes,
                 vnl_matrix<double>& WP)
{
  unsigned nr = P.rows();
  assert(nr==wt_mat.size()*2);
  WP.set_size(nr,n_modes);

  double const*const* PData = P.data_array();
  double ** WPData = WP.data_array();
  auto w=wt_mat.begin();

  for (unsigned i=0;i<nr;i+=2,++w)
  {
    const double* x=PData[i];
    const double* y=PData[i+1];
    double* wx=WPData[i];
    double* wy=WPData[i+1];
    double w11=w->m11();
    double w12=w->m12();
    double w22=w->m22();
    for (unsigned j=0;j<n_modes;++j)
    {
      wx[j]=w11*x[j] + w12*y[j];
      wy[j]=w12*x[j] + w22*y[j];
    }
  }
}

// Solves Mb=rhs for b where M is assumed symmetric
void msm_solve_sym_eqn(const vnl_matrix<double>& M,
                       const vnl_vector<double>& rhs,
                       vnl_vector<double>& b)
{
  vnl_cholesky chol(M,vnl_cholesky::estimate_condition);
  if (chol.rcond()>1.0e-6)
  {
    chol.solve(rhs,&b);
  }
  else
  {
    // Solve using SVD
    double tol=1e-8;
    vnl_svd<double> svd(M);
    svd.zero_out_relative(tol);
    b = svd.solve(rhs);
  }
}

// Solve weighted version of Pb=dx, ie P'WPb=P'Wdx
void msm_solve_for_b(const vnl_matrix<double>& P,
                     const vnl_vector<double>& var,
                     const vnl_vector<double>& wts,
                     const vnl_vector<double>& dx,
                     unsigned n_modes,
                     vnl_vector<double>& b, bool use_prior)
{
  vnl_matrix<double> WP;
  msm_calc_WP(P,wts,n_modes,WP);

  vnl_vector<double> PtWdx(n_modes);
  mbl_matxvec_prod_vm(dx,WP,PtWdx);

  vnl_matrix<double> PtWP;
  mbl_matrix_product_at_b(PtWP,P,WP,n_modes);

  if (use_prior)  // Add 1/var to diagonal of PtWP
    for (unsigned i=0;i<n_modes;++i) PtWP(i,i)+=1.0/var(i);

  // Solves (PtWP)b = PtWdx
  msm_solve_sym_eqn(PtWP,PtWdx,b);
}

// Solve weighted version of Pb=dx, ie P'WPb=P'Wdx
// W is block diagonal, with blocks wt_mat[i] (symmetrix 2x2)
void msm_solve_for_b(const vnl_matrix<double>& P,
                     const vnl_vector<double>& var,
                     const std::vector<msm_wt_mat_2d>& wt_mat,
                     const vnl_vector<double>& dx,
                     unsigned n_modes,
                     vnl_vector<double>& b, bool use_prior)
{
  vnl_matrix<double> WP;
  msm_calc_WP(P,wt_mat,n_modes,WP);

  vnl_vector<double> PtWdx(n_modes);
  mbl_matxvec_prod_vm(dx,WP,PtWdx);

  vnl_matrix<double> PtWP;
  mbl_matrix_product_at_b(PtWP,P,WP,n_modes);

  if (use_prior)  // Add 1/var to diagonal of PtWP
    for (unsigned i=0;i<n_modes;++i) PtWP(i,i)+=1.0/var(i);

  // Solves (PtWP)b = PtWdx
  msm_solve_sym_eqn(PtWP,PtWdx,b);
}

//: Finds parameters and pose to best match to points
//  Errors on point i are weighted by wts[i]
void msm_ref_shape_instance::fit_to_points_wt(const msm_points& pts,
                                              const vnl_vector<double>& wts)
{
  // Catch case when fitting to self
  if (&pts == &points_) return;
  if (b_.size()==0) return;

  tmp_points_.vector()=pts.vector();
  tmp_points_.vector()-=model().mean();

  // Now must solve weighted linear equation P'WPb=P'Wdx
  msm_solve_for_b(model().modes(),model().mode_var(),
                  wts,tmp_points_.vector(),
                  b_.size(),b_,use_prior_);

  param_limiter().apply_limit(b_);

  points_valid_=false;
}

#if 0
// Calculates W2=T'WT where T is 2x2 matrix (a,-b;b,a)
void msm_transform_wt_mat(const vnl_double_2x2& W,
                          double a, double b, vnl_double_2x2& W2)
{
  W2(0,0)=a*a*W[0][0]+2*a*b*W[0][1]+b*b*W[1][1];
  W2(0,1)=a*a*W[0][1]+a*b*(W(1,1)-W[0][0])-b*b*W[0][1];
  W2(1,0)=W2(0,1);
  W2(1,1)=a*a*W[1][1]-2*a*b*W[0][1]+b*b*W[0][0];
}
#endif // 0

//: Finds parameters and pose to best match to points
//  Errors on point i are weighted by wt_mat[i] in target frame
void msm_ref_shape_instance::fit_to_points_wt_mat(const msm_points& pts,
                                                  const std::vector<msm_wt_mat_2d>& wt_mat)
{
  // Catch case when fitting to self
  if (&pts == &points_) return;
  if (b_.size()==0) return;

  assert(wt_mat.size()==model().size());

  tmp_points_.vector()=pts.vector();
  tmp_points_.vector()-=model().mean();

  // Now must solve weighted linear equation P'WPb=P'Wdx
  msm_solve_for_b(model().modes(),model().mode_var(),
                  wt_mat,tmp_points_.vector(),
                  b_.size(),b_,use_prior_);

  param_limiter().apply_limit(b_);

  points_valid_=false;
}


//=======================================================================
// Method: version_no
//=======================================================================

short msm_ref_shape_instance::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string msm_ref_shape_instance::is_a() const
{
  return std::string("msm_ref_shape_instance");
}

//=======================================================================
// Method: print
//=======================================================================

  // required if data is present in this class
void msm_ref_shape_instance::print_summary(std::ostream& os) const
{
  os << is_a();
}

//=======================================================================
// Method: save
//=======================================================================

  // required if data is present in this class
void msm_ref_shape_instance::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,b_);
  vsl_b_write(bfs,param_limiter_);
  vsl_b_write(bfs,use_prior_);
}

//=======================================================================
// Method: load
//=======================================================================

  // required if data is present in this class
void msm_ref_shape_instance::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,b_);
      vsl_b_read(bfs,param_limiter_);
      vsl_b_read(bfs,use_prior_);
      break;
    default:
      std::cerr << "msm_ref_shape_instance::b_read() :\n"
               << "Unexpected version number " << version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }

  points_valid_=false;
  points_valid_=false;
}


//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const msm_ref_shape_instance& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, msm_ref_shape_instance& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const msm_ref_shape_instance& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_ref_shape_instance& b)
{
 os << b;
}
