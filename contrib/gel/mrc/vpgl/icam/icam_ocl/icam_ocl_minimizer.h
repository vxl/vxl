// This is icam_ocl_minimizer.h
#ifndef icam_ocl_minimizer_h
#define icam_ocl_minimizer_h
//:
// \file
// \brief  icam_minimizer with opencl implementation for some methods
// \author J.L. Mundy
// \date  November 10, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include <icam/icam_minimizer.h>
#include <icam_ocl/icam_ocl_search_manager.h>

class icam_ocl_minimizer : public icam_minimizer
{
 public:
  //: Constructor with all the parameters
  icam_ocl_minimizer( const vil_image_view<float>& source_img,
                      const vil_image_view<float>& dest_img,
                      const icam_depth_transform& dt,
                      unsigned min_level_size = 16,
                      unsigned box_reduction_k = 2,
                      double axis_search_cone_multiplier = 10.0,
                      double polar_range_multiplier = 2.0,
                      double local_min_thresh = 0.05,
                      vcl_string const& base_path = "",
                      bool verbose=false)
    : icam_minimizer(source_img,
                     dest_img,
                     dt,
                     min_level_size,
                     box_reduction_k,
                     axis_search_cone_multiplier,
                     polar_range_multiplier,
                     local_min_thresh,
                     base_path,
                     verbose)
    {search_mgr_ = icam_ocl_search_manager::instance();}

  //: Constructor, when source image is not known yet
  icam_ocl_minimizer(const vil_image_view<float>& dest_img,
                     const icam_depth_transform& dt,
                     unsigned min_level_size = 16,
                     unsigned box_reduction_k = 2,
                     double axis_search_cone_multiplier = 10.0,
                     double polar_range_multiplier = 2.0,
                     double local_min_thresh = 0.05,
                     vcl_string const& base_path = "",
                     bool verbose=false)
    : icam_minimizer(dest_img,
                     dt,
                     min_level_size,
                     box_reduction_k,
                     axis_search_cone_multiplier,
                     polar_range_multiplier,
                     local_min_thresh,
                     base_path,
                     verbose)
    {search_mgr_ = icam_ocl_search_manager::instance();}

  //: set rotation search kernel source code path.
  // must be done before executing rotation searches
  void set_rot_kernel_path(vcl_string const& rot_kernel_path)
    {rot_kernel_path_ = rot_kernel_path;}

  //: set GPU workgroup size.
  // must be done before executing rotation searches
  void set_workgroup_size(unsigned wgsize) {wgsize_ = wgsize;}

  //:
  // this function is called once for a given set of rotation searches
  // and is called internally in the virtual functions below
  bool init_opencl_rotation(unsigned level);

  //:
  // this function is called many times for a given level
  // as in searching over various translation values (trans)
  virtual bool
    exhaustive_rotation_search(vgl_vector_3d<double> const& trans,
                               unsigned level,
                               double min_allowed_overlap,
                               vgl_rotation_3d<double>& min_rot,
                               double& min_cost,
                               double& min_overlap_fraction,
                               bool setup,
                               bool finish);

  //:
  // this function is called once for a given set of rotation searches
  // at intermediate pyramid levels and is called internally in the
  // virtual function below
  bool init_opencl_rotation(vgl_rotation_3d<double> const& initial_rot,
                            unsigned initial_level,
                            unsigned search_level);

  //:
  // this function is called many times for a given level
  // and initial rotation, typically in searching over translation
  virtual bool initialized_rot_search(vgl_vector_3d<double> const& trans,
                                      vgl_rotation_3d<double>& initial_rot,
                                      unsigned initial_level,
                                      unsigned search_level,
                                      double min_allowed_overlap,
                                      vgl_rotation_3d<double>& min_rot,
                                      double& min_cost,
                                      double& min_overlap_fraction,
                                      bool setup,
                                      bool finish);

  //: this function is called once when finished
  bool finish_opencl();
 private:
  unsigned wgsize_;
  vcl_string rot_kernel_path_;
  bool run_rotation_kernel();
  icam_ocl_search_manager* search_mgr_;
};

#endif
