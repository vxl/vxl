#include "msm_shape_model_builder.h"
//:
// \file
// \brief Object to build a msm_shape_model
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vcl_cstdlib.h>  // for vcl_atoi() & vcl_abort()
#include <mbl/mbl_data_array_wrapper.h>
#include <mcal/mcal_pca.h>
#include <mbl/mbl_exception.h>

//=======================================================================
// Dflt ctor
//=======================================================================

msm_shape_model_builder::msm_shape_model_builder()
  : min_modes_(0),max_modes_(9999),var_prop_(0.98)
{
}

//=======================================================================
// Destructor
//=======================================================================

msm_shape_model_builder::~msm_shape_model_builder()
{
}

//: Set up model
void msm_shape_model_builder::set_aligner(
           const msm_aligner& aligner)
{
  aligner_ = aligner;
}

//: Define parameter limiter.
void msm_shape_model_builder::set_param_limiter(const msm_param_limiter& p)
{
  param_limiter_=p;
}

void msm_shape_model_builder::set_mode_choice(unsigned min, unsigned max,
                    double var_proportion)
{
  min_modes_ = min;
  max_modes_ = max;
  var_prop_ = var_proportion;
}


//: Builds the model from the supplied examples
void msm_shape_model_builder::build_model(
                   const vcl_vector<msm_points>& shapes,
                   msm_shape_model& shape_model)
{
  // Align shapes and estimate mean pose
  msm_points ref_mean_shape;
  vcl_vector<vnl_vector<double> > pose_to_ref;
  vnl_vector<double> average_pose;
  aligner().align_set(shapes,ref_mean_shape,pose_to_ref,average_pose);

  // Generate vectors corresponding to aligned shapes
  unsigned n = shapes.size();
  vcl_vector<vnl_vector<double> > aligned_shape_vec(n);
  msm_points aligned_shape;
  for (unsigned i=0;i<n;++i)
  {
    aligner().apply_transform(shapes[i],pose_to_ref[i],aligned_shape);
    aligned_shape_vec[i]=aligned_shape.vector();
  }

  mcal_pca pca;
  pca.set_mode_choice(min_modes_,max_modes_,var_prop_);

  vnl_matrix<double> modes;
  vnl_vector<double> mode_var;

  mbl_data_array_wrapper<vnl_vector<double> >
    data(&aligned_shape_vec[0],n);

  pca.build_about_mean(data,ref_mean_shape.vector(),
                            modes,mode_var);

  param_limiter_->set_param_var(mode_var);

  shape_model.set(ref_mean_shape,modes,mode_var,average_pose,
                  aligner(),param_limiter());
}

//: Builds the model from the points loaded from given files
void msm_shape_model_builder::build_from_files(
                   const vcl_string& points_dir,
                   const vcl_vector<vcl_string>& filenames,
                   msm_shape_model& shape_model)
{
  unsigned n=filenames.size();
  vcl_vector<msm_points> shapes(n);
  for (unsigned i=0;i<n;++i)
  {
    vcl_string path = points_dir+"/"+filenames[i];
    if (!shapes[i].read_text_file(path))
    {
      mbl_exception_parse_error x("Failed to load points from "+path);
      mbl_exception_error(x);
    }
  }
  build_model(shapes,shape_model);
}


//=======================================================================
// Method: version_no
//=======================================================================

short msm_shape_model_builder::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string msm_shape_model_builder::is_a() const
{
  return vcl_string("msm_shape_model_builder");
}

//=======================================================================
// Method: print
//=======================================================================

  // required if data is present in this class
void msm_shape_model_builder::print_summary(vcl_ostream& os) const
{
  os<<vcl_endl;
  os<<vsl_indent()<<"aligner: ";
  if (aligner_.isDefined()) os<<aligner_; else os<<"-";
  os<<vsl_indent()<< "param_limiter: ";
  if (param_limiter_.isDefined())
    os<<param_limiter_; else os<<"-";
  os<<vsl_indent()<<"min_modes: "<<min_modes_<<'\n'
    <<vsl_indent()<<"max_modes: "<<max_modes_<<'\n'
    <<vsl_indent()<<"var_prop: "<<var_prop_;
}

//=======================================================================
// Method: save
//=======================================================================

  // required if data is present in this class
void msm_shape_model_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,param_limiter_);
  vsl_b_write(bfs,aligner_);
  vsl_b_write(bfs,min_modes_);
  vsl_b_write(bfs,max_modes_);
  vsl_b_write(bfs,var_prop_);
}

//=======================================================================
// Method: load
//=======================================================================

  // required if data is present in this class
void msm_shape_model_builder::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,aligner_);
      vsl_b_read(bfs,param_limiter_);
      vsl_b_read(bfs,min_modes_);
      vsl_b_read(bfs,max_modes_);
      vsl_b_read(bfs,var_prop_);
      break;
    default:
      vcl_cerr << "msm_shape_model_builder::b_read() :\n"
               << "Unexpected version number " << version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const msm_shape_model_builder& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, msm_shape_model_builder& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const msm_shape_model_builder& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const msm_shape_model_builder& b)
{
 os << b;
}
