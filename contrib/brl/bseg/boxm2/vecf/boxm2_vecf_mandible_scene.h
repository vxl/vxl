#ifndef boxm2_vecf_mandible_scene_h_
#define boxm2_vecf_mandible_scene_h_
//:
// \file
// \brief  boxm2_vecf_mandible_scene models the mandible
//
// \author J.L. Mundy
// \date   1 Nov 2015
//
// For the time being the mandible shape will not be adjusted except for the
// global affine transformation of the skull (including the mandible)
//
#include <string>
#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <boxm2/boxm2_block.h>
#include <boxm2/vecf/boxm2_vecf_articulated_scene.h>
#include <boxm2/vecf/boxm2_vecf_articulated_params.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_data_base.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include "boxm2_vecf_mandible_params.h"
#include "boxm2_vecf_mandible.h"
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class boxm2_vecf_mandible_scene : public boxm2_vecf_articulated_scene
{
 public:
  enum anat_type { MANDIBLE, LEFT_RAMUS, LEFT_ANGLE, BODY, RIGHT_ANGLE, RIGHT_RAMUS, NO_TYPE};
 boxm2_vecf_mandible_scene(): boxm2_vecf_articulated_scene(),mandible_data_(nullptr), left_ramus_(nullptr), left_angle_(nullptr),body_(nullptr),right_angle_(nullptr),
    right_ramus_(nullptr), extrinsic_only_(false){}

  //: set parameters
  bool set_params(boxm2_vecf_articulated_params const& params) override;

  // construct from existing scene block database
  boxm2_vecf_mandible_scene(std::string const& scene_file);

  // otherwise compute voxel contents from the mandible parameters
  boxm2_vecf_mandible_scene(std::string const& scene_file, std::string const& geometry_file);

  boxm2_vecf_mandible_scene(std::string const& scene_file, std::string const& geometry_file, std::string const& params_file);

  //: compute inverse vector field for unrefined target cells
  void inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const& unrefined_target_pts) override;

  //: map mandible data to the target scene
  void map_to_target(boxm2_scene_sptr target_scene) override;

  //: compute an inverse vector field for rotation of mandible

  void inverse_vector_field(std::vector<vgl_vector_3d<double> >& vf, std::vector<bool>& valid) const;

  bool inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const override;
  bool coupled_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const override;
  bool apply_vector_field(cell_info const& target_cell, vgl_vector_3d<double> const& inv_vf) override;


  //: refine target cells to match the refinement level of the source block
  int prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index) override;


  //:return a reference to the mandible geometry
  const boxm2_vecf_mandible& mandible_geo() const {return mandible_geo_;}

  //: test the anat_type (LEFT_RAMUS, LEFT_ANGLE, ... ) of the voxel that contains a global point
 bool is_type_global(vgl_point_3d<double> const& global_pt, anat_type type) const;

 //: test the anat_type (LEFT_RAMUS, LEFT_ANGLE, ... ) of a given data index
 bool is_type_data_index(unsigned data_index, anat_type type) const;

 //: set up pointers to source block databases particular to this subclass
 void extract_block_data();

 //: interpolate the alpha and appearance data around the vector field source location
 void interpolate_vector_field(vgl_point_3d<double> const& src, unsigned sindx, unsigned dindx, unsigned tindx,
                                std::vector<vgl_point_3d<double> > & cell_centers,
                                std::map<unsigned, std::vector<unsigned> >& cell_neighbor_cell_index,
                               std::map<unsigned, std::vector<unsigned> >&cell_neighbor_data_index);


 void apply_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf, std::vector<bool> const& valid);

 // find nearest cell and return the data index of the nearest cell
 bool find_nearest_data_index(boxm2_vecf_mandible_scene::anat_type type, vgl_point_3d<double> const& probe, double cell_len, unsigned& data_indx,
                              int& found_depth) const;

  //re-create geometry according to params_
  void rebuild();
  //check for intrinsic parameter change
  bool vfield_params_change_check(const boxm2_vecf_mandible_params& params);
  // store the neigbors of each cell for each anatomical component in a vector;
  void cache_neighbors();

 // ============   mandible methods ================
 //: construct manidble from parameters
 void create_mandible();
 //:read block eye data and reset indices
 void recreate_mandible();
 //: cache index of neighboring cells
 void find_cell_neigborhoods();
 //: set manible anatomy flags
 void cache_cell_centers_from_anatomy_labels();
 //: scan dense set of points on the manbible
 void build_mandible();
 void build_left_ramus();
#if 0
 void build_left_angle();
 void build_body();
 void build_right_angle();
 void build_right_ramus();
#endif
 void create_left_ramus();
#if 0
 void create_left_angle();
 void create_body();
 void create_right_angle();
 void create_right_ramus();
#endif

 //: assign appearance to parts of the mandible
 void paint_mandible();
 void paint_left_ramus();
#if 0
 void paint_left_angle();
 void paint_body();
 void paint_right_angle();
 void paint_right_ramus();
#endif
 void reset_buffers();

 void find_left_ramus_cell_neigborhoods();
#if 0
 void find_left_angle_cell_neigborhoods();
 void find_body_cell_neigborhoods();
 void find_right_angle_cell_neigborhoods();
 void find_right_ramus_cell_neigborhoods();
#endif
  //: members
 boxm2_vecf_mandible mandible_geo_;

 std::vector<cell_info> box_cell_centers_;       // cell centers in the target block

 boxm2_vecf_mandible_params params_;               // parameter struct
 // =============  mandible ===============
 int mandible_size_; // used for debug
 boxm2_data_base* mandible_base_;
 boxm2_data<BOXM2_PIXEL>::datatype* mandible_data_;        // is voxel a mandible point
 boxm2_data<BOXM2_PIXEL>* left_ramus_;      // is voxel a left_ramus point
 boxm2_data<BOXM2_PIXEL>* left_angle_;      // is voxel a left_angle point
 boxm2_data<BOXM2_PIXEL>* body_;            // is voxel a body point
 boxm2_data<BOXM2_PIXEL>* right_angle_;     // is voxel a right_angle point
 boxm2_data<BOXM2_PIXEL>* right_ramus_;     // is voxel a right_ramus point

 //manible
 std::vector<vgl_point_3d<double> > mandible_cell_centers_; // centers of mandible voxels
 std::vector<unsigned> mandible_cell_data_index_;           // corresponding data indices
 //      cell_index          cell_index
 std::map<unsigned, std::vector<unsigned> > cell_neighbor_cell_index_; // neighbors of each mandible voxel
 //     data_index cell_index
 std::map<unsigned, unsigned > data_index_to_cell_index_;             // data index to cell index
 //      data_index          data_index
 std::map<unsigned, std::vector<unsigned> > cell_neighbor_data_index_; // data index to neighbor data indices

 // left_ramus
 std::vector<vgl_point_3d<double> > left_ramus_cell_centers_; // centers of left_ramus voxels
 std::vector<unsigned> left_ramus_cell_data_index_;           // corresponding data indices
 //      cell_index          cell_index
 std::map<unsigned, std::vector<unsigned> > left_ramus_cell_neighbor_cell_index_; // neighbors of each left_ramus voxel
 //     data_index cell_index
 std::map<unsigned, unsigned > left_ramus_data_index_to_cell_index_;             // data index to left_ramus index
 //      data_index          data_index
 std::map<unsigned, std::vector<unsigned> > left_ramus_cell_neighbor_data_index_; // data index to neighbor data indices

 // left_angle
 std::vector<vgl_point_3d<double> > left_angle_cell_centers_; // centers of left_angle voxels
 std::vector<unsigned> left_angle_cell_data_index_;           // corresponding data indices
 //      cell_index          cell_index
 std::map<unsigned, std::vector<unsigned> > left_angle_cell_neighbor_cell_index_; // neighbors of each left_angle voxel
 //     data_index cell_index
 std::map<unsigned, unsigned > left_angle_data_index_to_cell_index_;             // data index to left_angle index
 //      data_index          data_index
 std::map<unsigned, std::vector<unsigned> > left_angle_cell_neighbor_data_index_; // data index to neighbor data indices

 // body
 std::vector<vgl_point_3d<double> > body_cell_centers_; // centers of body voxels
 std::vector<unsigned> body_cell_data_index_;           // corresponding data indices
 //      cell_index          cell_index
 std::map<unsigned, std::vector<unsigned> > body_cell_neighbor_cell_index_; // neighbors of each body voxel
 //     data_index cell_index
 std::map<unsigned, unsigned > body_data_index_to_cell_index_;             // data index to body index
 //      data_index          data_index
 std::map<unsigned, std::vector<unsigned> > body_cell_neighbor_data_index_; // data index to neighbor data indices

 // right_angle
 std::vector<vgl_point_3d<double> > right_angle_cell_centers_; // centers of right_angle voxels
 std::vector<unsigned> right_angle_cell_data_index_;           // corresponding data indices
 //      cell_index          cell_index
 std::map<unsigned, std::vector<unsigned> > right_angle_cell_neighbor_cell_index_; // neighbors of each right_angle voxel
 //     data_index cell_index
 std::map<unsigned, unsigned > right_angle_data_index_to_cell_index_;             // data index to right_angle index
 //      data_index          data_index
 std::map<unsigned, std::vector<unsigned> > right_angle_cell_neighbor_data_index_; // data index to neighbor data indices

 // right_ramus
 std::vector<vgl_point_3d<double> > right_ramus_cell_centers_; // centers of right_ramus voxels
 std::vector<unsigned> right_ramus_cell_data_index_;           // corresponding data indices
 //      cell_index          cell_index
 std::map<unsigned, std::vector<unsigned> > right_ramus_cell_neighbor_cell_index_; // neighbors of each right_ramus voxel
 //     data_index cell_index
 std::map<unsigned, unsigned > right_ramus_data_index_to_cell_index_;             // data index to right_ramus index
 //      data_index          data_index
 std::map<unsigned, std::vector<unsigned> > right_ramus_cell_neighbor_data_index_; // data index to neighbor data indices

 private:
 bool extrinsic_only_;
 bool intrinsic_change_;

 //: assign target cell centers that map to the source scene bounding box
 void determine_target_box_cell_centers();
};

#endif // boxm2_vecf_mandible_scene_h_
