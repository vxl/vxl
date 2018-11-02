#include <iostream>
#include <cstdlib>
#include "msm_ref_shape_model.h"
//:
// \file
// \brief Contains mean/modes etc of a shape model
// \author Tim Cootes

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>


//=======================================================================
// Dflt ctor
//=======================================================================

msm_ref_shape_model::msm_ref_shape_model()
  : mean_points_scale_(0)
{
}

//=======================================================================
// Destructor
//=======================================================================

msm_ref_shape_model::~msm_ref_shape_model() = default;

//: Set up model
void msm_ref_shape_model::set(const msm_points& mean,
                              const vnl_matrix<double>& modes,
                              const vnl_vector<double>& mode_var,
                              const msm_param_limiter& param_limiter)
{
  assert(mean.vector().size()%2==0);
  assert(mode_var.size()==modes.columns());
  if (mode_var.size()!=0)
  {
    assert(mean.size()*2==modes.rows());
  }

  mean_ = mean;
  mean_points_scale_ = mean.scale();
  modes_ = modes;
  mode_var_ = mode_var;
  param_limiter_ = param_limiter;
}

//: Equality test
bool msm_ref_shape_model::operator==(const msm_ref_shape_model& model) const
{
  if (model.mean_.size()!=mean_.size()) return false;
  if (model.mode_var_.size()!=mode_var_.size()) return false;
  if (mean_.size()==0) return true;  // Empty models

  const double epsilon = 1e-3;

  double ssd1=vnl_vector_ssd(model.mean_.vector(),mean_.vector());
  if (std::fabs(ssd1/mean_.size())>epsilon) return false;

  double ssd2=vnl_vector_ssd(model.mode_var_,mode_var_);
  if (std::fabs(ssd2/mode_var_.size())>epsilon) return false;

  double max_d = (modes_-model.modes_).absolute_value_max();
  if (max_d>epsilon) return false;

  return true;
}


//=======================================================================
// Method: version_no
//=======================================================================

short msm_ref_shape_model::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string msm_ref_shape_model::is_a() const
{
  return std::string("msm_ref_shape_model");
}

//=======================================================================
// Method: print
//=======================================================================

  // required if data is present in this class
void msm_ref_shape_model::print_summary(std::ostream& os) const
{
  os << "n_pts: "<<size()<<" n_modes: "<<n_modes()<<std::endl;
  vsl_indent_inc(os);
  os << std::endl << vsl_indent() << " param_limiter: ";
  if (param_limiter_.isDefined())
    os<<param_limiter_; else os<<"-";
  os<<std::endl;
  vsl_indent_dec(os);
}

//=======================================================================
// Method: save
//=======================================================================

  // required if data is present in this class
void msm_ref_shape_model::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,mean_);
  vsl_b_write(bfs,modes_);
  vsl_b_write(bfs,mode_var_);
  vsl_b_write(bfs,param_limiter_);
}

//=======================================================================
// Method: load
//=======================================================================

  // required if data is present in this class
void msm_ref_shape_model::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,mean_);
      vsl_b_read(bfs,modes_);
      vsl_b_read(bfs,mode_var_);
      vsl_b_read(bfs,param_limiter_);
      break;
    default:
      std::cerr << "msm_ref_shape_model::b_read() :\n"
               << "Unexpected version number " << version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }

  mean_points_scale_=mean_.scale();
}


//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const msm_ref_shape_model& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, msm_ref_shape_model& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const msm_ref_shape_model& b)
{
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_ref_shape_model& b)
{
 os << b;
}
