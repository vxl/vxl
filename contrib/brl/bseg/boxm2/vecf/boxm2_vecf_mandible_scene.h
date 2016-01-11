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
#include <boxm2/boxm2_block.h>
#include <boxm2/vecf/boxm2_vecf_articulated_scene.h>
#include <boxm2/vecf/boxm2_vecf_articulated_params.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_data_base.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include "boxm2_vecf_mandible_params.h"
#include "boxm2_vecf_mandible.h"
#include <vgl/vgl_point_3d.h>
#include <vcl_set.h>

class boxm2_vecf_mandible_scene : public boxm2_vecf_articulated_scene
{
 public:
  enum anat_type { MANDIBLE, LEFT_RAMUS, LEFT_ANGLE, BODY, RIGHT_ANGLE, RIGHT_RAMUS, NO_TYPE};
 boxm2_vecf_mandible_scene(): alpha_base_(0), app_base_(0), nobs_base_(0), mandible_data_(0), left_ramus_(0), left_angle_(0),
                           body_(0),right_angle_(0), right_ramus_(0), target_alpha_base_(0),target_app_base_(0), target_nobs_base_(0), extrinsic_only_(false),target_blk_(0),target_data_extracted_(false),boxm2_vecf_articulated_scene(),sigma_(0.5f){}

  //: set parameters
  bool set_params(boxm2_vecf_articulated_params const& params);

  //: construct from scene file specification, use exising database unless initialize == true
  // otherwise compute voxel contents from the mandible parameters
  boxm2_vecf_mandible_scene(vcl_string const& scene_file, vcl_string const& geometry_file);

  boxm2_vecf_mandible_scene(vcl_string const& scene_file, vcl_string const& geometry_file, vcl_string const& params_file);


  //: map mandible data to the target scene
  void map_to_target(boxm2_scene_sptr target_scene);

  //: compute an inverse vector field for rotation of mandible
  void inverse_vector_field(vgl_rotation_3d<double> const& rot, vcl_vector<vgl_vector_3d<double> >& vfield,
                            vcl_vector<bool>& valid) const;

  //: refine target cells to match the refinement level of the source block
  void prerefine_target(boxm2_scene_sptr target_scene,vgl_rotation_3d<double> const& rot);

  //:return a reference to the mandible geometry
  const boxm2_vecf_mandible& mandible_geo() const {return mandible_geo_;}

  //: test the anat_type (LEFT_RAMUS, LEFT_ANGLE, ... ) of the voxel that contains a global point
 bool is_type_global(vgl_point_3d<double> const& global_pt, anat_type type) const;

 //: test the anat_type (LEFT_RAMUS, LEFT_ANGLE, ... ) of a given data index
 bool is_type_data_index(unsigned data_index, anat_type type) const;

 //: tree subblock size in mm
 double subblock_len() const { if(blk_)return (blk_->sub_block_dim()).x(); return 0.0;}
  //: set up pointers to source block databases
 void extract_block_data();
  //: set up pointers to target block databases
 void extract_target_block_data(boxm2_scene_sptr target_scene);
 //: initialize the source block data
 void fill_block();
 //: initialize the full target block (not currently used )
 void fill_target_block();
 //: interpolate the alpha and appearance data around the vector field source location
 void interpolate_vector_field(vgl_point_3d<double> const& src, unsigned sindx, unsigned dindx, unsigned tindx,
                                vcl_vector<vgl_point_3d<double> > & cell_centers,
                                vcl_map<unsigned, vcl_vector<unsigned> >& cell_neighbor_cell_index,
                               vcl_map<unsigned, vcl_vector<unsigned> >&cell_neighbor_data_index);


 void apply_vector_field_to_target(vcl_vector<vgl_vector_3d<double> > const& vf, vcl_vector<bool> const& valid);

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
 void recreate_left_ramus();
#if 0
 void recreate_left_angle();
 void recreate_body();
 void recreate_right_angle();
 void recreate_right_ramus();
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
  boxm2_block_sptr blk_;                     // the source block
  boxm2_block_sptr target_blk_;              // the target block
  // cached databases
  // source dbs
  boxm2_data_base* alpha_base_;
  boxm2_data_base* app_base_;
  boxm2_data_base* nobs_base_;
  boxm2_data_base* mandible_base_;
  // target dbs
  boxm2_data_base* target_alpha_base_;
  boxm2_data_base* target_app_base_;
  boxm2_data_base* target_nobs_base_;
  // standard data buffers
  boxm2_data<BOXM2_ALPHA>::datatype* alpha_data_;  // source alpha database
  boxm2_data<BOXM2_MOG3_GREY>::datatype* app_data_;// source appearance database
  boxm2_data<BOXM2_NUM_OBS>::datatype* nobs_data_;         // source nobs database
  boxm2_data<BOXM2_ALPHA>::datatype* target_alpha_data_;   //target alpha database
  boxm2_data<BOXM2_MOG3_GREY>::datatype* target_app_data_; //target appearance database
  boxm2_data<BOXM2_NUM_OBS>::datatype* target_nobs_data_;  //target nobs

  vcl_vector<cell_info> box_cell_centers_;       // cell centers in the target block

  boxm2_vecf_mandible_params params_;               // parameter struct
  // =============  mandible ===============
  int mandible_size_; // used for debug
  boxm2_data<BOXM2_PIXEL>::datatype* mandible_data_;        // is voxel a mandible point
  boxm2_data<BOXM2_PIXEL>* left_ramus_;      // is voxel a left_ramus point
  boxm2_data<BOXM2_PIXEL>* left_angle_;      // is voxel a left_angle point
  boxm2_data<BOXM2_PIXEL>* body_;            // is voxel a body point
  boxm2_data<BOXM2_PIXEL>* right_angle_;     // is voxel a right_angle point
  boxm2_data<BOXM2_PIXEL>* right_ramus_;     // is voxel a right_ramus point

  //manible
  vcl_vector<vgl_point_3d<double> > mandible_cell_centers_; // centers of mandible voxels
  vcl_vector<unsigned> mandible_cell_data_index_;           // corresponding data indices
  //      cell_index          cell_index
  vcl_map<unsigned, vcl_vector<unsigned> > cell_neighbor_cell_index_; // neighbors of each mandible voxel
  //     data_index cell_index
  vcl_map<unsigned, unsigned > data_index_to_cell_index_;             // data index to cell index
  //      data_index          data_index
  vcl_map<unsigned, vcl_vector<unsigned> > cell_neighbor_data_index_; // data index to neighbor data indices

  // left_ramus
  vcl_vector<vgl_point_3d<double> > left_ramus_cell_centers_; // centers of left_ramus voxels
  vcl_vector<unsigned> left_ramus_cell_data_index_;           // corresponding data indices
    //      cell_index          cell_index
  vcl_map<unsigned, vcl_vector<unsigned> > left_ramus_cell_neighbor_cell_index_; // neighbors of each left_ramus voxel
  //     data_index cell_index
  vcl_map<unsigned, unsigned > left_ramus_data_index_to_cell_index_;             // data index to left_ramus index
  //      data_index          data_index
  vcl_map<unsigned, vcl_vector<unsigned> > left_ramus_cell_neighbor_data_index_; // data index to neighbor data indices

  // left_angle
  vcl_vector<vgl_point_3d<double> > left_angle_cell_centers_; // centers of left_angle voxels
  vcl_vector<unsigned> left_angle_cell_data_index_;           // corresponding data indices
    //      cell_index          cell_index
  vcl_map<unsigned, vcl_vector<unsigned> > left_angle_cell_neighbor_cell_index_; // neighbors of each left_angle voxel
  //     data_index cell_index
  vcl_map<unsigned, unsigned > left_angle_data_index_to_cell_index_;             // data index to left_angle index
  //      data_index          data_index
  vcl_map<unsigned, vcl_vector<unsigned> > left_angle_cell_neighbor_data_index_; // data index to neighbor data indices

  // body
  vcl_vector<vgl_point_3d<double> > body_cell_centers_; // centers of body voxels
  vcl_vector<unsigned> body_cell_data_index_;           // corresponding data indices
    //      cell_index          cell_index
  vcl_map<unsigned, vcl_vector<unsigned> > body_cell_neighbor_cell_index_; // neighbors of each body voxel
  //     data_index cell_index
  vcl_map<unsigned, unsigned > body_data_index_to_cell_index_;             // data index to body index
  //      data_index          data_index
  vcl_map<unsigned, vcl_vector<unsigned> > body_cell_neighbor_data_index_; // data index to neighbor data indices

  // right_angle
  vcl_vector<vgl_point_3d<double> > right_angle_cell_centers_; // centers of right_angle voxels
  vcl_vector<unsigned> right_angle_cell_data_index_;           // corresponding data indices
    //      cell_index          cell_index
  vcl_map<unsigned, vcl_vector<unsigned> > right_angle_cell_neighbor_cell_index_; // neighbors of each right_angle voxel
  //     data_index cell_index
  vcl_map<unsigned, unsigned > right_angle_data_index_to_cell_index_;             // data index to right_angle index
  //      data_index          data_index
  vcl_map<unsigned, vcl_vector<unsigned> > right_angle_cell_neighbor_data_index_; // data index to neighbor data indices

  // right_ramus
  vcl_vector<vgl_point_3d<double> > right_ramus_cell_centers_; // centers of right_ramus voxels
  vcl_vector<unsigned> right_ramus_cell_data_index_;           // corresponding data indices
    //      cell_index          cell_index
  vcl_map<unsigned, vcl_vector<unsigned> > right_ramus_cell_neighbor_cell_index_; // neighbors of each right_ramus voxel
  //     data_index cell_index
  vcl_map<unsigned, unsigned > right_ramus_data_index_to_cell_index_;             // data index to right_ramus index
  //      data_index          data_index
  vcl_map<unsigned, vcl_vector<unsigned> > right_ramus_cell_neighbor_data_index_; // data index to neighbor data indices

private:
  bool extrinsic_only_;
  bool intrinsic_change_;
  bool target_data_extracted_;
  float sigma_;

 //: assign target cell centers that map to the source scene bounding box
  void determine_target_box_cell_centers();
};

#endif // boxm2_vecf_mandible_scene_h_
