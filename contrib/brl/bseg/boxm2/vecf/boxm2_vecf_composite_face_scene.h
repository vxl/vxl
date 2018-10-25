#ifndef boxm2_vecf_composite_face_scene_h_
#define boxm2_vecf_composite_face_scene_h_
#include <vector>
#include <string>
#include <vgl/vgl_box_3d.h>
#include <boxm2/boxm2_scene.h>
#include "boxm2_vecf_mandible_scene.h"
#include "boxm2_vecf_cranium_scene.h"
#include "boxm2_vecf_skin_scene.h"
#include "boxm2_vecf_middle_fat_pocket_scene.h"
#include "boxm2_vecf_articulated_params.h"
#include "boxm2_vecf_composite_face_params.h"
#include "boxm2_vecf_articulated_scene.h"
#include "boxm2_vecf_mouth.h"

class boxm2_vecf_composite_face_scene : public boxm2_vecf_articulated_scene{
public:
  enum comp_type {MANDIBLE, CRANIUM, SKIN, NO_TYPE};
  boxm2_vecf_composite_face_scene(std::string const& face_scene_paths);

  void map_to_target(boxm2_scene_sptr target) override;

  bool set_params(boxm2_vecf_articulated_params const& params) override;
  boxm2_vecf_composite_face_params const& params() const {return params_;}

  //: extract target cells, potentially with inverse mapping for a global transformation
  void extract_unrefined_cell_info() override;

  //: find the inverse vector field for unrefined target block centers
  void inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const& unrefined_target_pts) override;

  //: refine target cells to match the refinement level of the source block
  int prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index) override;


  //: inverse vector field over all components
  bool inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const override {
    std::string type; return inverse_vector_field(target_pt, inv_vf, type);
  }

  bool apply_vector_field(cell_info const& target_cell, vgl_vector_3d<double> const& inv_vf) override{ return false;}

  bool inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf, std::string& anatomy_type) const;

  //: compute the composite vector field over all components
  void inverse_vector_field(std::vector<vgl_vector_3d<double> >& vfield, std::vector<std::string>& type) const;

  //: apply the vector field
  void apply_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vfield, std::vector<std::string> const& type);

  //: get the cell centers
  void extract_target_cell_centers();

  //: compute target box from point cloud
  // the point cloud is a target to compare with source
  void compute_target_box( std::string const& pc_path);
  vgl_box_3d<double> target_box() const {return target_box_;}
  // for debugging purposes
  void set_target_box(vgl_box_3d<double> const& target_box){ target_box_ = target_box;}

  //: construct the target scene from the bounding box
  boxm2_scene_sptr construct_target_scene(std::string const& scene_dir,std::string const& scene_name, std::string const& data_path, double sub_block_len,
                                         bool save_scene_xml = true);

 private:
  vgl_box_3d<double> target_box_;
  vgl_box_3d<double> mandible_skin_coupling_box_;
  vgl_box_3d<double> middle_fat_pocket_skin_coupling_box_;
  boxm2_vecf_mouth mouth_geo_;
  boxm2_vecf_composite_face_params params_;
  boxm2_vecf_articulated_scene_sptr mandible_;
  boxm2_vecf_articulated_scene_sptr cranium_;
  boxm2_vecf_articulated_scene_sptr skin_;
  boxm2_vecf_articulated_scene_sptr middle_fat_pocket_;
  std::vector<cell_info> target_cell_centers_;       // cell centers in the refined target block
};

#endif
