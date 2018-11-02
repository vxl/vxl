#ifndef boxm2_vecf_skull_scene_h_
#define boxm2_vecf_skull_scene_h_

#include <iostream>
#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm2/boxm2_scene.h>
#include "boxm2_vecf_mandible_scene.h"
#include "boxm2_vecf_cranium_scene.h"
#include "boxm2_vecf_head_model.h"
#include "boxm2_vecf_articulated_params.h"
#include "boxm2_vecf_skull_params.h"
#include "boxm2_vecf_articulated_scene.h"

class boxm2_vecf_skull_scene : public boxm2_vecf_articulated_scene{

public:
  boxm2_vecf_skull_scene(std::string const& scene_path, std::string const& geo_path);
  ~boxm2_vecf_skull_scene() override{delete mandible_scene_; delete cranium_scene_;}
  void map_to_target(boxm2_scene_sptr target) override;

  bool set_params(boxm2_vecf_articulated_params const& params) override;
  boxm2_vecf_skull_params const& get_params() const {return params_;}
  //: refine target cells to match the refinement level of the source block
  int prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index) override{return -1;}//FIXME
  //: compute inverse vector field for unrefined sub_block centers
  void inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const& unrefined_target_pts) override{}//FIXME
  bool inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const override{return false;}//FIXME
  bool apply_vector_field(cell_info const& target_cell, vgl_vector_3d<double> const& inv_vf) override{return false;}//FIXME

private:
  bool target_data_extracted_;
  boxm2_vecf_skull_params params_;
  boxm2_vecf_mandible_scene* mandible_scene_;
  boxm2_vecf_cranium_scene* cranium_scene_;
  std::string scene_path;
};

#endif
