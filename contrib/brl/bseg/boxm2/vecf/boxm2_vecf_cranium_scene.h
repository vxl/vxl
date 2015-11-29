#ifndef boxm2_vecf_cranium_scene_h_
#define boxm2_vecf_cranium_scene_h_
//:
// \file
// \brief  boxm2_vecf_cranium_scene models the cranium
//
// \author J.L. Mundy
// \date   6 Nov 2015
//
// For the time being the cranium shape will not be adjusted except for the
// global affine transformation of the skull (including the cranium)
//
#include <boxm2/boxm2_block.h>
#include <boxm2/vecf/boxm2_vecf_articulated_scene.h>
#include <boxm2/vecf/boxm2_vecf_articulated_params.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include "boxm2_vecf_cranium_params.h"
#include "boxm2_vecf_cranium.h"
#include <vgl/vgl_point_3d.h>
#include <vcl_set.h>

class boxm2_vecf_cranium_scene : public boxm2_vecf_articulated_scene
{
 public:
  enum anat_type { CRANIUM, NO_TYPE};
 boxm2_vecf_cranium_scene(): source_model_exists_(false), alpha_data_(0), app_data_(0), nobs_data_(0), cranium_(0),target_alpha_data_(0),target_app_data_(0), target_nobs_data_(0), extrinsic_only_(false),target_blk_(0),target_data_extracted_(false),boxm2_vecf_articulated_scene(),sigma_(0.5f){}

  //: set parameters
  bool set_params(boxm2_vecf_articulated_params const& params);

  //: construct from scene file specification, use exising database unless initialize == true
  // otherwise compute voxel contents from the cranium parameters
  boxm2_vecf_cranium_scene(vcl_string const& scene_file);

  boxm2_vecf_cranium_scene(vcl_string const& scene_file, vcl_string const& geometry_file);

  boxm2_vecf_cranium_scene(vcl_string const& scene_file, vcl_string const& geometry_file, vcl_string const& params_file);

  //: map cranium data to the target scene
  void map_to_target(boxm2_scene_sptr target_scene);

  //: compute an inverse vector field for rotation of cranium
  void inverse_vector_field(vgl_rotation_3d<double> const& rot, vcl_vector<vgl_vector_3d<double> >& vfield,
                            vcl_vector<bool>& valid) const;

  //: test the anat_type (CRANIUM) of the voxel that contains a global point
  bool is_type_global(vgl_point_3d<double> const& global_pt, anat_type type) const;

 //: test the anat_type (CRANIUM) of a given data index
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
 bool find_nearest_data_index(boxm2_vecf_cranium_scene::anat_type type, vgl_point_3d<double> const& probe, unsigned& data_indx) const;
 
  //re-create geometry according to params_
  void rebuild();
  //check for intrinsic parameter change
  bool vfield_params_change_check(const boxm2_vecf_cranium_params& params);
  // store the neigbors of each cell for each anatomical component in a vector;
  void cache_neighbors();

 // ============   cranium methods ================
 //: construct manidble from parameters
 void create_cranium();
 //:read block eye data and reset indices
 void recreate_cranium();
 //: cache index of neighboring cells
 void find_cell_neigborhoods();
 //: set manible anatomy flags
 void cache_cell_centers_from_anatomy_labels();
 //: scan dense set of points on the manbible
 void build_cranium();
 //: assign appearance to parts of the cranium
 void paint_cranium();
 void reset_buffers();

  //: members
 boxm2_vecf_cranium cranium_geo_;
  boxm2_block* blk_;                     // the source block
  boxm2_block* target_blk_;              // the target block
  boxm2_data<BOXM2_ALPHA>* alpha_data_;  // source alpha database
  boxm2_data<BOXM2_MOG3_GREY>* app_data_;// source appearance database
  boxm2_data<BOXM2_NUM_OBS>* nobs_data_;         // source nobs database
  boxm2_data<BOXM2_ALPHA>* target_alpha_data_;   //target alpha database
  boxm2_data<BOXM2_MOG3_GREY>* target_app_data_; //target appearance database
  boxm2_data<BOXM2_NUM_OBS>* target_nobs_data_;  //target nobs
  vcl_vector<cell_info> box_cell_centers_;       // cell centers in the target block

  boxm2_vecf_cranium_params params_;               // parameter struct
  // =============  manible ===============
  boxm2_data<BOXM2_PIXEL>* cranium_;        // is voxel a cranium point

  vcl_vector<vgl_point_3d<double> > cranium_cell_centers_; // centers of spherical shell voxels
  vcl_vector<unsigned> cranium_cell_data_index_;           // corresponding data indices
  //      cell_index          cell_index
  vcl_map<unsigned, vcl_vector<unsigned> > cell_neighbor_cell_index_; // neighbors of each cranium voxel
  //     data_index cell_index
  vcl_map<unsigned, unsigned > data_index_to_cell_index_;             // data index to cell index
  //      data_index          data_index
  vcl_map<unsigned, vcl_vector<unsigned> > cell_neighbor_data_index_; // data index to neighbor data indices

private:
  bool source_model_exists_;
  bool extrinsic_only_;
  bool intrinsic_change_;
  bool target_data_extracted_;
  float sigma_;

 //: assign target cell centers that map to the source scene bounding box
  void determine_target_box_cell_centers();
};

#endif // boxm2_vecf_cranium_scene_h_
