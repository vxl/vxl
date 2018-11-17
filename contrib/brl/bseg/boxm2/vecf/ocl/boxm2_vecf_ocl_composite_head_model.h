#ifndef boxm2_vecf_ocl_composite_head_model_h_
#define boxm2_vecf_ocl_composite_head_model_h_

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm2/boxm2_scene.h>
#include "../boxm2_vecf_orbit_scene.h"
#include "boxm2_vecf_ocl_orbit_scene.h"
#include "boxm2_vecf_ocl_head_model.h"
#include "../boxm2_vecf_articulated_params.h"
#include "../boxm2_vecf_composite_head_parameters.h"
#include "../boxm2_vecf_articulated_scene.h"
#define USE_ORBIT_CL
#ifdef USE_ORBIT_CL
#define ORBIT boxm2_vecf_ocl_orbit_scene
#else
#define ORBIT boxm2_vecf_orbit_scene
#endif

class boxm2_vecf_ocl_composite_head_model : public boxm2_vecf_ocl_head_model{
friend class boxm2_vecf_ocl_appearance_extractor; //the appearance extractor needs to signal a change to the original model when its apm is updated
public:
  boxm2_vecf_ocl_composite_head_model(std::string const& head_model_path, std::string const& eye_model_path,const bocl_device_sptr& device,const boxm2_opencl_cache_sptr& opencl_cache,bool optimize =false);

  void map_to_target(boxm2_scene_sptr target) override;

  // virtual void inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const& unrefined_target_pts) ;
  // virtual int prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index) ;
  // virtual bool inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const;
  // virtual bool apply_vector_field(cell_info const& target_cell, vgl_vector_3d<double> const& inv_vf) ;

  void update_gpu_target(const boxm2_scene_sptr& target_scene);
  bool set_params(boxm2_vecf_articulated_params const& params) override;
  boxm2_vecf_composite_head_parameters const& get_params() const {return params_;}
  // retrieve the estimated look dir from imagery
  vgl_vector_3d<double> get_estimated_look_dir(bool is_right)
   { return is_right ? right_orbit_.estimated_look_dir_ : left_orbit_.estimated_look_dir_;}
  //cache the estimated look dir from imagery
  void set_estimated_look_dir(vgl_vector_3d<double>& l_dir, vgl_vector_3d<double>& r_dir)
   {left_orbit_.estimated_look_dir_ =l_dir; right_orbit_.estimated_look_dir_ = r_dir; }
  void set_estimated_dt(double dt_left, double  dt_right)
   {left_orbit_.estimated_dt_ = dt_left; right_orbit_.estimated_dt_ = dt_right; }


private:
  boxm2_vecf_composite_head_parameters params_;

  ORBIT right_orbit_;
  ORBIT left_orbit_;
  std::string scene_path;
};

#endif
