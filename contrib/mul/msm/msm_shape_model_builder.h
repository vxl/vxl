#ifndef msm_shape_model_builder_h_
#define msm_shape_model_builder_h_
//:
// \file
// \brief Object to build a msm_shape_model
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <string>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_fwd.h>
#include <msm/msm_shape_model.h>
#include <msm/msm_param_limiter.h>

//: Object to build a msm_shape_model
class msm_shape_model_builder
{
 private:
  //: Object used to deal with global transformations
  mbl_cloneable_ptr<msm_aligner> aligner_;

  //: Default choice of parameter limiter
  mbl_cloneable_ptr<msm_param_limiter> param_limiter_;

  //: Choose n.modes to explain this proportion of variance
  double var_prop_;

  //: Min. number of modes to select
  unsigned min_modes_;

  //: Max. number of modes to use
  unsigned max_modes_;

  //: Define how to compute alignment of reference shape
  msm_aligner::ref_pose_source ref_pose_source_;

 public:

  // Dflt ctor
  msm_shape_model_builder();

  // Destructor
  ~msm_shape_model_builder();

  //: Set up aligner to be used
  void set_aligner(const msm_aligner& aligner);

  //: Define parameter limiter.
  void set_param_limiter(const msm_param_limiter&);

  //: Define limits on number of parameters to use in model
  // \param var_proportion  Proportion of variance in data to explain
  void set_mode_choice(unsigned min, unsigned max,
                       double var_proportion);

  //: Define how to compute alignment of reference shape
  void set_ref_pose_source(msm_aligner::ref_pose_source);


  //: Object used to deal with global transformations
  const msm_aligner& aligner() const { return aligner_; }

  //: Current object which limits parameters
  const msm_param_limiter& param_limiter() const
  { return param_limiter_; }

  //: Builds the model from the supplied examples
  void build_model(const std::vector<msm_points>& shapes,
                   msm_shape_model& shape_model);

  //: Builds the model, using subsets of elements for some modes
  //  Builds a shape model, allowing control of which elements may
  //  be varied in some of the modes.  This allows construction
  //  of models where some groups of points are semi-independent
  //  of the others.
  //  \param pts_used[i] indicates the set of points to be used for
  //  mode i (or all if \p pts_used[i] is empty).
  //  Modes beyond \p pts_used.size() will use all elements.
  //  Builds at least \p pts_used.size() modes. Number defined by
  //  max_modes and var_prop.
  void build_model(const std::vector<msm_points>& shapes,
                   const std::vector<std::vector<unsigned> >& pts_used,
                   msm_shape_model& shape_model);

  //: Builds shape model from within-class variation
  //  \param shapes[i] belongs to class \p id[i].
  //  Aligns all shapes to a common mean.
  //  Computes the average covariance about each class mean,
  //  and builds shape modes from this.
  //
  //  If \p id[i]<0, then shape is
  //  used for building global mean, but not for within class model.
  //
  //  \param pts_used[i] indicates which points will be controlled by mode i.
  void build_within_class_model(
                   const std::vector<msm_points>& shapes,
                   const std::vector<int>& id,
                   const std::vector<std::vector<unsigned> >& pts_used,
                   msm_shape_model& shape_model);

  //: Builds shape model from within-class variation
  //  \param shapes[i] belongs to class \p id[i].
  //  Aligns all shapes to a common mean.
  //  Computes the average covariance about each class mean,
  //  and builds shape modes from this.
  //
  //  If \p id[i]<0, then shape is
  //  used for building global mean, but not for within class model.
  void build_within_class_model(
                   const std::vector<msm_points>& shapes,
                   const std::vector<int>& id,
                   msm_shape_model& shape_model);

  //: Builds the model from the points loaded from given files
  //  Loads from \p points_dir/filenames[i].
  //  throws a mbl_exception_parse_error if fails to load in
  //  any of the files.
  void build_from_files(const std::string& points_dir,
                        const std::vector<std::string>& filenames,
                        msm_shape_model& shape_model);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const;

  //: Print class to os
  void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);

  //: Initialise from a text stream.
  // Expects something like:
  // \verbatim
  // {
  //   aligner: msm_similarity_aligner
  //   param_limiter: msm_ellipsoid_limiter { accept_prop: 0.98 }
  //   min_modes: 0 max_modes: 49 var_prop: 0.98
  // }
  // \endverbatim
  void config_from_stream(std::istream &is);
};

//: Loads all shapes from \p points_dir/filenames[i].
//  Throws mbl_exception_parse_error if fails.
void msm_load_shapes(const std::string& points_dir,
                     const std::vector<std::string>& filenames,
                     std::vector<msm_points>& shapes);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_shape_model_builder& pts);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_shape_model_builder& pts);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const msm_shape_model_builder& pts);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_shape_model_builder& pts);

#endif // msm_shape_model_builder_h_
