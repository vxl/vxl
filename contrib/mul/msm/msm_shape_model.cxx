#include "msm_shape_model.h"
//:
// \file
// \brief Contains mean/modes etc of a shape model
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vcl_cstdlib.h>  // for vcl_atoi() & vcl_abort()
#include <vcl_cassert.h>

//=======================================================================
// Dflt ctor
//=======================================================================

msm_shape_model::msm_shape_model()
{
}

//=======================================================================
// Destructor
//=======================================================================

msm_shape_model::~msm_shape_model()
{
}

//: Set up model
void msm_shape_model::set(const msm_points& mean,
                          const vnl_matrix<double>& modes,
                          const vnl_vector<double>& mode_var,
                          const vnl_vector<double>& default_pose,
                          const msm_aligner& aligner,
                          const msm_param_limiter& param_limiter)
{
  msm_ref_shape_model::set(mean,modes,mode_var,param_limiter);

  assert(default_pose.size()==aligner.size());
  default_pose_ = default_pose;
  aligner_ = aligner;
}

//: Equality test
bool msm_shape_model::operator==(const msm_shape_model& model)
{
  if (model.mean_.size()!=mean_.size()) return false;
  if (model.mode_var_.size()!=mode_var_.size()) return false;
  if (mean_.size()==0) return true;  // Empty models

  const double epsilon = 1e-3;

  double ssd1=vnl_vector_ssd(model.mean_.vector(),mean_.vector());
  if (vcl_fabs(ssd1/mean_.size())>epsilon) return false;

  double ssd2=vnl_vector_ssd(model.mode_var_,mode_var_);
  if (vcl_fabs(ssd2/mode_var_.size())>epsilon) return false;

  double max_d = (modes_-model.modes_).absolute_value_max();
  if (max_d>epsilon) return false;

  return true;
}


//=======================================================================
// Method: version_no
//=======================================================================

short msm_shape_model::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string msm_shape_model::is_a() const
{
  return vcl_string("msm_shape_model");
}

//=======================================================================
// Method: print
//=======================================================================

  // required if data is present in this class
void msm_shape_model::print_summary(vcl_ostream& os) const
{
  os << "n_pts: "<<size()<<" n_modes: "<<n_modes()<<vcl_endl;
  vsl_indent_inc(os);
  os << vsl_indent() << " aligner: ";
  if (aligner_.isDefined()) os<<aligner_; else os<<"-";
  os << vcl_endl << vsl_indent() << " param_limiter: ";
  if (param_limiter_.isDefined())
    os<<param_limiter_; else os<<"-";
  os<<vcl_endl;
  vsl_indent_dec(os);
}

//=======================================================================
// Method: save
//=======================================================================

  // required if data is present in this class
void msm_shape_model::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  msm_ref_shape_model::b_write(bfs);  // Save base
  vsl_b_write(bfs,default_pose_);
  vsl_b_write(bfs,aligner_);
}

//=======================================================================
// Method: load
//=======================================================================

  // required if data is present in this class
void msm_shape_model::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      msm_ref_shape_model::b_read(bfs); // Load base
      vsl_b_read(bfs,default_pose_);
      vsl_b_read(bfs,aligner_);
      break;
    default:
      vcl_cerr << "msm_shape_model::b_read() :\n"
               << "Unexpected version number " << version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const msm_shape_model& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, msm_shape_model& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const msm_shape_model& b)
{
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const msm_shape_model& b)
{
 os << b;
}
