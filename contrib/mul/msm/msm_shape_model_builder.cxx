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
#include <mcal/mcal_extract_mode.h>
#include <mbl/mbl_exception.h>

//=======================================================================
// Dflt ctor
//=======================================================================

msm_shape_model_builder::msm_shape_model_builder()
  : var_prop_(0.98),min_modes_(0),max_modes_(9999)
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

//: Each point p controls two elements 2p,2p+1
inline void msm_elements_from_pts_used(
                   const vcl_vector<vcl_vector<unsigned> >& pts_used,
                   vcl_vector<vcl_vector<unsigned> >& used)
{
  used.resize(pts_used.size());
  for (unsigned i=0;i<pts_used.size();++i)
  {
    used[i].empty();
    used[i].reserve(2*pts_used[i].size());
    for (unsigned j=0;j<pts_used[i].size();++j)
    {
      used[i].push_back(2*pts_used[i][j]);
      used[i].push_back(2*pts_used[i][j]+1);
    }
  }
}

//: Builds the model, using subsets of elements for some modes
//  Builds a shape model, allowing control of which elements may
//  be varied in some of the modes.  This allows construction
//  of models where some groups of points are semi-independent
//  of the others.
//  \param pts_used[i] indicates the set of elements to be used for
//  mode i (or all if \p pts_used[i] is empty).
//  Modes beyond \p pts_used.size() will use all elements.
//  Builds at least \p pts_used.size() modes. Number defined by
//  max_modes and var_prop.
void msm_shape_model_builder::build_model(
                  const vcl_vector<msm_points>& shapes,
                  const vcl_vector<vcl_vector<unsigned> >& pts_used,
                  msm_shape_model& shape_model)
{
  // Align shapes and estimate mean pose
  msm_points ref_mean_shape;
  vcl_vector<vnl_vector<double> > pose_to_ref;
  vnl_vector<double> average_pose;
  aligner().align_set(shapes,ref_mean_shape,pose_to_ref,average_pose);

  // Generate vectors corresponding to aligned shapes
  unsigned n = shapes.size();
  vcl_vector<vnl_vector<double> > dshape_vec(n);
  msm_points aligned_shape;
  for (unsigned i=0;i<n;++i)
  {
    aligner().apply_transform(shapes[i],pose_to_ref[i],aligned_shape);
    dshape_vec[i]=aligned_shape.vector()-ref_mean_shape.vector();
  }

  // Set up indication for which elements to be used
  // pt i corresponds to elements 2i,2i+1
  vcl_vector<vcl_vector<unsigned> > used;
  msm_elements_from_pts_used(pts_used,used);

  vnl_matrix<double> modes;
  vnl_vector<double> mode_var;

  mcal_extract_modes(dshape_vec,used,
                     max_modes_,var_prop_,
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
  vcl_vector<msm_points> shapes(filenames.size());
  msm_load_shapes(points_dir,filenames,shapes);
  build_model(shapes,shape_model);
}

//: Counts number of examples with each class ID
//  Assumes IDs run from 0.  Ignores any elements with \p id[i]<0.
//  On exit n_per_class[j] indicates number of class j
//  It is resized to cope with the largest ID number. Some elements
//  may be zero.
static void msm_count_classes(const vcl_vector<int>& id,
                              vcl_vector<unsigned>& n_per_class)
{
  int max_id = 0;
  for (unsigned i=0;i<id.size();++i)
    if (id[i]>max_id) max_id=id[i];

  n_per_class.resize(1+max_id,0u);

  for (unsigned i=0;i<id.size();++i)
    if (id[i]>=0) n_per_class[id[i]]++;
}


//: Builds shape model from within-class variation
//  \param shape[i] belongs to class \p id[i].
//  Aligns all shapes to a common mean.
//  Computes the average covariance about each class mean,
//  and builds shape modes from this.
//
//  If \p id[i]<0, then shape is
//  used for building global mean, but not for within class model.
void msm_shape_model_builder::build_within_class_model(
                   const vcl_vector<msm_points>& shapes,
                   const vcl_vector<int>& id,
                   msm_shape_model& shape_model)
{
  // Align shapes and estimate mean pose
  msm_points ref_mean_shape;
  vcl_vector<vnl_vector<double> > pose_to_ref;
  vnl_vector<double> average_pose;
  aligner().align_set(shapes,ref_mean_shape,pose_to_ref,average_pose);

  vcl_vector<unsigned> n_per_class;
  msm_count_classes(id,n_per_class);
  vcl_vector<vnl_vector<double> > class_mean(n_per_class.size());

  // Initialise sums for class means
  for (unsigned j=0;j<n_per_class.size();++j)
    if (n_per_class[j]>0)
    {
      class_mean[j].set_size(2*ref_mean_shape.size());
      class_mean[j].fill(0.0);
    }

  // Generate vectors corresponding to aligned shapes
  unsigned n = shapes.size();
  vcl_vector<vnl_vector<double> > dshape_vec;
  vcl_vector<int> valid_id;
  dshape_vec.reserve(n);
  valid_id.reserve(n);

  msm_points aligned_shape;
  for (unsigned i=0;i<n;++i)
  {
    if (id[i]<0) continue;  // Ignore unknown class id
    aligner().apply_transform(shapes[i],pose_to_ref[i],aligned_shape);
    dshape_vec.push_back(aligned_shape.vector());
    valid_id.push_back(id[i]);

    class_mean[id[i]]+=aligned_shape.vector();
  }

  // Compute the mean for each class from the sums
  for (unsigned j=0;j<n_per_class.size();++j)
    if (n_per_class[j]>0) class_mean[j]/=n_per_class[j];

  // Remove mean from each example
  for (unsigned i=0;i<dshape_vec.size();++i)
    dshape_vec[i]-=class_mean[valid_id[i]];

  // Vectors are now about a zero mean.
  // Apply PCA to this data to compute the modes.
  mcal_pca pca;
  pca.set_mode_choice(min_modes_,max_modes_,var_prop_);

  vnl_matrix<double> modes;
  vnl_vector<double> mode_var;

  mbl_data_array_wrapper<vnl_vector<double> > data(dshape_vec);
  vnl_vector<double> zero_mean(2*ref_mean_shape.size(),0.0);

  pca.build_about_mean(data,zero_mean, modes,mode_var);

  param_limiter_->set_param_var(mode_var);

  shape_model.set(ref_mean_shape,modes,mode_var,average_pose,
                  aligner(),param_limiter());
}

//: Builds shape model from within-class variation
//  \param shape[i] belongs to class \p id[i].
//  Aligns all shapes to a common mean.
//  Computes the average covariance about each class mean,
//  and builds shape modes from this.
//
//  If \p id[i]<0, then shape is
//  used for building global mean, but not for within class model.
//
//  \param pts_used[i] indicates which points will be controlled by mode i.
void msm_shape_model_builder::build_within_class_model(
                   const vcl_vector<msm_points>& shapes,
                   const vcl_vector<int>& id,
                   const vcl_vector<vcl_vector<unsigned> >& pts_used,
                   msm_shape_model& shape_model)
{
  // Align shapes and estimate mean pose
  msm_points ref_mean_shape;
  vcl_vector<vnl_vector<double> > pose_to_ref;
  vnl_vector<double> average_pose;
  aligner().align_set(shapes,ref_mean_shape,pose_to_ref,average_pose);

  vcl_vector<unsigned> n_per_class;
  msm_count_classes(id,n_per_class);
  vcl_vector<vnl_vector<double> > class_mean(n_per_class.size());

  // Initialise sums for class means
  for (unsigned j=0;j<n_per_class.size();++j)
    if (n_per_class[j]>0)
    {
      class_mean[j].set_size(2*ref_mean_shape.size());
      class_mean[j].fill(0.0);
    }

  // Generate vectors corresponding to aligned shapes
  unsigned n = shapes.size();
  vcl_vector<vnl_vector<double> > dshape_vec;
  vcl_vector<int> valid_id;
  dshape_vec.reserve(n);
  valid_id.reserve(n);

  msm_points aligned_shape;
  for (unsigned i=0;i<n;++i)
  {
    if (id[i]<0) continue;  // Ignore unknown class id
    aligner().apply_transform(shapes[i],pose_to_ref[i],aligned_shape);
    dshape_vec.push_back(aligned_shape.vector());
    valid_id.push_back(id[i]);

    class_mean[id[i]]+=aligned_shape.vector();
  }

  // Compute the mean for each class from the sums
  for (unsigned j=0;j<n_per_class.size();++j)
    if (n_per_class[j]>0) class_mean[j]/=n_per_class[j];

  // Remove mean from each example
  for (unsigned i=0;i<dshape_vec.size();++i)
    dshape_vec[i]-=class_mean[valid_id[i]];

  // Set up indication for which elements to be used
  // pt i corresponds to elements 2i,2i+1
  vcl_vector<vcl_vector<unsigned> > used;
  msm_elements_from_pts_used(pts_used,used);

  vnl_matrix<double> modes;
  vnl_vector<double> mode_var;

  mcal_extract_modes(dshape_vec,used,
                     max_modes_,var_prop_,
                     modes,mode_var);

  param_limiter_->set_param_var(mode_var);

  shape_model.set(ref_mean_shape,modes,mode_var,average_pose,
                  aligner(),param_limiter());
}


//: Loads all shapes from \p points_dir/filenames[i].
//  Throws mbl_exception_parse_error if fails.
void msm_load_shapes(const vcl_string& points_dir,
                     const vcl_vector<vcl_string>& filenames,
                     vcl_vector<msm_points>& shapes)
{
  unsigned n=filenames.size();
  shapes.resize(n);
  for (unsigned i=0;i<n;++i)
  {
    vcl_string path = points_dir+"/"+filenames[i];
    if (!shapes[i].read_text_file(path))
    {
      mbl_exception_parse_error x("Failed to load points from "+path);
      mbl_exception_error(x);
    }
  }
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
  os<<'\n'<<vsl_indent()<<"aligner: ";
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
  vsl_b_write(bfs,aligner_);
  vsl_b_write(bfs,param_limiter_);
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
