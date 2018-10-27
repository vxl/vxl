#ifndef boxm2_vecf_skin_scene_h_
#define boxm2_vecf_skin_scene_h_
//:
// \file
// \brief  boxm2_vecf_skin_scene models the surface of the face
//
// \author J.L. Mundy
// \date   3 Dec 2015
//
// For the time being, the skin map to target is only global transformations
//
#include <string>
#include <vector>
#include <map>
#include <iosfwd>
#include <boxm2/boxm2_block.h>
#include <boxm2/vecf/boxm2_vecf_articulated_scene.h>
#include <boxm2/vecf/boxm2_vecf_articulated_params.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include "boxm2_vecf_skin_params.h"
#include "boxm2_vecf_skin.h"
#include <vgl/vgl_point_3d.h>


class boxm2_vecf_skin_scene : public boxm2_vecf_articulated_scene
{
 public:
  enum anat_type { SKIN, NO_TYPE};
 boxm2_vecf_skin_scene(): skin_data_(nullptr),boxm2_vecf_articulated_scene(){}

  //: set parameters
  bool set_params(boxm2_vecf_articulated_params const& params) override;
  void set_skin_params(boxm2_vecf_skin_params const& params) {params_ = params;}

  //: construct from scene file specification, use exising database unless initialize == true
  // otherwise compute voxel contents from the skin parameters
  boxm2_vecf_skin_scene(std::string const& scene_file);

  boxm2_vecf_skin_scene(std::string const& scene_file, std::string const& geometry_file);

  //: find the inverse vector field for unrefined target block centers
  void inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const& unrefined_target_pts) override;

  //: refine target cells to match the refinement level of the source block
  int prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index) override;


  //: map skin data to the target scene
  void map_to_target(boxm2_scene_sptr target_scene) override;

  //: compute an inverse vector field for rotation of skin
  void inverse_vector_field(std::vector<vgl_vector_3d<double> >& vfield, std::vector<bool>& valid) const;

  bool inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const override;
  bool apply_vector_field(cell_info const& target_cell, vgl_vector_3d<double> const& inv_vf) override;


  //: test the anat_type (SKIN) of the voxel that contains a global point
  bool is_type_global(vgl_point_3d<double> const& global_pt, anat_type type) const;

 //: test the anat_type (SKIN) of a given data index
 bool is_type_data_index(unsigned data_index, anat_type type) const;

  //: set up pointers to source block databases
 void extract_block_data();

 //: interpolate the alpha and appearance data around the vector field source location
 void interpolate_vector_field(vgl_point_3d<double> const& src, unsigned sindx, unsigned dindx, unsigned tindx,
                               std::vector<vgl_point_3d<double> > & cell_centers,
                               std::map<unsigned, std::vector<unsigned> >& cell_neighbor_cell_index,
                               std::map<unsigned, std::vector<unsigned> >&cell_neighbor_data_index);


 void apply_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf, std::vector<bool> const& valid);

 // find nearest cell and return the data index of the nearest cell (found depth is for debug, remove at some point)
 bool find_nearest_data_index(boxm2_vecf_skin_scene::anat_type type, vgl_point_3d<double> const& probe, double cell_len, unsigned& data_indx, int& found_depth) const;

  //re-create geometry according to params_
  void rebuild();

  // store the neigbors of each cell for each anatomical component in a vector;
  void cache_neighbors();

  void create_anatomy_labels();

  void export_point_cloud(std::ostream& ostr) const;

  void export_point_cloud_with_appearance(std::ostream& ostr) const;

  //should be implemented on params class
  bool vfield_params_change_check(const boxm2_vecf_skin_params& params){return false;}

  // ============   skin methods ================

 //: cache index of neighboring cells
 void find_cell_neigborhoods();
 //: set manible anatomy flags
 void cache_cell_centers_from_anatomy_labels();

 //: scan dense set of points on the manbible
 void build_skin();
 //: assign appearance to parts of the skin
 void paint_skin();
 //: assign appearance from skin point cloud with appearance
 void repaint_skin();

  //: members
  // =============  skin ===============
  boxm2_data_base* skin_base_;
  boxm2_data<BOXM2_PIXEL>::datatype* skin_data_;        // is voxel a skin point

  std::vector<cell_info> box_cell_centers_;       // cell centers in the target block

  boxm2_vecf_skin_params params_;               // parameter struct

  boxm2_vecf_skin skin_geo_;

  std::vector<vgl_point_3d<double> > skin_cell_centers_; // centers of skin cells
  std::vector<unsigned> skin_cell_data_index_;           // corresponding data indices
  //      cell_index          cell_index
  std::map<unsigned, std::vector<unsigned> > cell_neighbor_cell_index_; // neighbors of each skin voxel
  //     data_index cell_index
  std::map<unsigned, unsigned > data_index_to_cell_index_;             // data index to cell index
  //      data_index          data_index
  std::map<unsigned, std::vector<unsigned> > cell_neighbor_data_index_; // data index to neighbor data indices

private:
  void extract_scene_metadata();

 //: assign target cell centers that map to the source scene bounding box
  void determine_target_box_cell_centers();
};

#endif // boxm2_vecf_skin_scene_h_
