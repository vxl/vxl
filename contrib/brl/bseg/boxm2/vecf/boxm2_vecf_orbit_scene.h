#ifndef boxm2_vecf_orbit_scene_h_
#define boxm2_vecf_orbit_scene_h_
//:
// \file
// \brief  boxm2_vecf_orbit_scene models the human eye region
//
// \author J.L. Mundy
// \date   27 Mar 2015
//
//  The eye sclara surface is constructed from  layers of voxels
//  that are within a diagonal cell radius from the sphere surface.
//  The voxels are assigned by brute force scanning of a bounding box
//  around the sphere that is enlarged by one cell side length.
//  The interior of the eye is empty.
//  Currently, the appearance model is BOXM2_MOG3_GREY
// but probably a single Gaussian mode would suffice.
// The voxels of the eye surface have byte labels corresponding
// to the different anatomical structure. It is possible for
// a single voxel to have multiple labels such as the iris and
// the pupil. That is, if one considers the pupil to be a subset
// of the iris, both iris and pupil labels will be true on the
// pupil.
//
// A vector field for eye rotation is defined internally
// to obtain a given pointing direction, specified in
// boxm2_vecf_eye_params.h. An additional vector field is defined by
// the backwards translation from each target scene cell to the
// corresponding source cell. The vector operation is defined in the figure.
//
//   [.] target cell
//     \
//      \  translation vector (-params_.offset_)
//       \
//        \
//         \
// source   v
//  scene  [.]------------->[.] source cell
//         s0    rotation    s1
//             vector field
//
// The reverse translation vector from target to source scene defines the tail position,
// s0, of the rotation vector. The inverse rotation vector field element is then added to
// the s0 position to reach the actual source cell s1. The neigbors of s1 are
// used to interpolate the required target appearance and alpha data. Both s0
// and s1 must be elements of the spherical shell defined above.
// If s0 is not on the spherical shell then the target cell contents is unchanged.
// Note that eye rotation always takes a shell cell into another shell cell.
//
#include <string>
#include <vector>
#include <iostream>
#include <boxm2/boxm2_block.h>
#include <boxm2/vecf/boxm2_vecf_articulated_scene.h>
#include <boxm2/vecf/boxm2_vecf_articulated_params.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include "boxm2_vecf_orbit_params.h"
#include "boxm2_vecf_eyelid.h"
#include "boxm2_vecf_eyelid_crease.h"
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


class boxm2_vecf_orbit_scene
: public boxm2_vecf_articulated_scene
{
public:
  enum anat_type { SPHERE, IRIS, PUPIL, UPPER_LID, LOWER_LID, EYELID_CREASE, NO_TYPE};
  boxm2_vecf_orbit_scene() :
    alpha_data_(nullptr),
    app_data_(nullptr),
    nobs_data_(nullptr),
    sphere_(nullptr),
    iris_(nullptr),
    pupil_(nullptr),
    target_alpha_data_(nullptr),
    eyelid_(nullptr),
    target_app_data_(nullptr),
    target_nobs_data_(nullptr),
    target_blk_(nullptr),
    extrinsic_only_(false),
    boxm2_vecf_articulated_scene(),
    target_data_extracted_(false),
    sigma_(0.5f)
  {}

  //: set parameters
  bool set_params(boxm2_vecf_articulated_params const& params) override;

  //: construct from scene file specification, use exising database unless initialize == true
  // otherwise scan a spherical shell to define the voxel surface
  boxm2_vecf_orbit_scene(std::string const& scene_file,
                         bool is_single_instance = true,
                         bool is_right = false);

  boxm2_vecf_orbit_scene(std::string const& scene_file,
                         const std::string& params_file,
                         bool is_single_instance = true,
                         bool is_right =false);

  //: map eye data to the target scene
  void map_to_target(boxm2_scene_sptr target_scene) override;

  //: extract the appearance from the target scene
  void extract_appearance_from_target(boxm2_scene_sptr target_scene);

  //: compute an inverse vector field for rotation of the eye globe
  void inverse_vector_field_eye(vgl_rotation_3d<double> const& rot,
                                std::vector<vgl_vector_3d<double> >& vfield,
                                std::vector<bool>& valid) const;

  //: compute an inverse vector field for opening/closing of the eyelid, (0.0 <= t <= 1.0)
  void inverse_vector_field_eyelid(double dt, std::vector<vgl_vector_3d<double> >& vfield, std::vector<unsigned char>& valid) const;

  //: static lower lid for now
  void inverse_vector_field_lower_eyelid(std::vector<vgl_vector_3d<double> >& vfield, std::vector<bool>& valid) const;

  //: static eyelid creasefor now
  void  inverse_vector_field_eyelid_crease(std::vector<vgl_vector_3d<double> >& vfield, std::vector<bool>& valid) const;

  //: test the anat_type (SPHERE, IRIS, ... ) of the voxel that contains a global point
 bool is_type_global(vgl_point_3d<double> const& global_pt, anat_type type) const;
 //: test the anat_type (SPHERE, IRIS, ... ) of a given data index
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
                                std::vector<vgl_point_3d<double> > & cell_centers,
                                std::map<unsigned, std::vector<unsigned> >& cell_neighbor_cell_index,
                               std::map<unsigned, std::vector<unsigned> >&cell_neighbor_data_index);


 // find nearest cell and return the data index of the nearest cell
  bool find_nearest_data_index(anat_type type, vgl_point_3d<double> const& probe, unsigned& data_indx) const;

  //re-create geometry according to params_
  void rebuild();
  //instantiate eyelids
  void init_eyelids();
  //check for intrinsic parameter change
  bool vfield_params_change_check(const boxm2_vecf_orbit_params& params);
  // store the neigbors of each cell for each anatomical component in a vector;
  void cache_neighbors();

  //: refine target cells to match the refinement level of the source block
  int prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index) override{return -1;}//FIXME
  //: compute inverse vector field for unrefined sub_block centers
  void inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const& unrefined_target_pts) override{}//FIXME

  bool inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const override{return false;}//FIXME
  bool apply_vector_field(cell_info const& target_cell, vgl_vector_3d<double> const& inv_vf) override{return false;}//FIXME

 // ============   eye methods ================
 //: construct eye sphere, iris and pupil
 void create_eye();
 //:read block eye data and reset indices
 void recreate_eye();
 //: cache index of neighboring cells
 void find_cell_neigborhoods();
 //: set sphere surface, sclera, iris and pupil cell indices
 void cache_cell_centers_from_anatomy_labels();
 //: scan dense set of points on the spherical shell to define surface voxels
 void build_sphere();
 //: scan dense set of point on iris to define iris voxels
 void build_iris();
 //: scan dense set of point on iris to define pupil voxels
  void build_pupil();
  //: assign appearance to sclera voxels
  void paint_sclera();
  //: assign appearance to iris voxels
  void paint_iris();
  //: assign appearance to pupil voxels
  void paint_pupil();


  //: scan over target cells and interpolate appearance and alpha from source
  void apply_eye_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf,
                                        std::vector<bool> const& valid);
  void apply_eyelid_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf,
                                           std::vector<unsigned char> const& valid);
  void apply_lower_eyelid_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf,
                                                 std::vector<bool> const& valid);

  void apply_eyelid_crease_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf,
                                                  std::vector<bool> const& valid);
  // ==================  eyelid methods ==============
  //: construct eyelid (voxelize and paint)
  void create_eyelid();
  //:read block eyelid data and reset indices
  void recreate_eyelid();
  //: cache index of neighboring cells
  void find_eyelid_cell_neigborhoods();
  //: scan dense set of points on the spherical shell to define surface voxels
  void build_eyelid();
  //: assign appearance to eyelid voxels
  void paint_eyelid();

  // ==================  lower eyelid methods ==============
  //: construct lower eyelid (voxelize and paint)
  void create_lower_eyelid();
  //:read block eyelid data and reset indices
  void recreate_lower_eyelid();
  //: cache index of neighboring cells
  void find_lower_eyelid_cell_neigborhoods();
  //: scan dense set of points on the spherical shell to define surface voxels
  void build_lower_eyelid();
  //: assign appearance to eyelid voxels
  void paint_lower_eyelid();

  // ==================  lower eyelid crease ==============
  //: construct eyelid crease (voxelize and paint)
  void create_eyelid_crease();
  //:read block eyelid data and reset indices
  void recreate_eyelid_crease();
  //: cache index of neighboring cells
  void find_eyelid_crease_cell_neigborhoods();
  //: scan dense set of points on the spherical shell to define surface voxels
  void build_eyelid_crease();
  //reset appearance and alpha_buffers
  void reset_buffers(bool color_only = false);
  //: assign appearance to eyelid voxels
  void paint_eyelid_crease();
  //: members

  boxm2_block* blk_;                     // the source block
  boxm2_block* target_blk_;              // the target block
  boxm2_data<BOXM2_ALPHA>* alpha_data_;  // source alpha database
  boxm2_data<BOXM2_MOG3_GREY>* app_data_;// source appearance database
  boxm2_data<BOXM2_GAUSS_RGB>* color_app_data_;// source appearance database
  boxm2_data<BOXM2_NUM_OBS>* nobs_data_; // source nobs database
  boxm2_data<BOXM2_ALPHA>* target_alpha_data_;   //target alpha database
  boxm2_data<BOXM2_MOG3_GREY>* target_app_data_; //target appearance database
  boxm2_data<BOXM2_NUM_OBS>* target_nobs_data_;  //target nobs
  boxm2_data<BOXM2_VIS_SCORE>* target_vis_score_data_;  //target nobs
  boxm2_data<BOXM2_GAUSS_RGB>* target_color_data_;
  std::vector<cell_info> box_cell_centers_;       // cell centers in the target block
  boxm2_vecf_orbit_params params_;               // parameter struct
  bool is_right_;
  vgl_vector_3d<double> estimated_look_dir_;
  // =============  eye ===============
  boxm2_data<BOXM2_PIXEL>* sphere_;      // is voxel a eye sphere point?
  boxm2_data<BOXM2_PIXEL>* iris_;        // is voxel an iris point
  boxm2_data<BOXM2_PIXEL>* pupil_;       // is voxel a pupil point
  boxm2_data<BOXM2_FLOAT>* radial_distance_;       // is voxel a pupil point

  // centers of spherical shell voxels
  std::vector<vgl_point_3d<double> > sphere_cell_centers_;

  // corresponding data indices
  std::vector<unsigned> sphere_cell_data_index_;

  //      cell_index          cell_index
  // neighbors of each shell voxel
  std::map<unsigned, std::vector<unsigned> > cell_neighbor_cell_index_;

  //     data_index cell_index
  // data index to shell index
  std::map<unsigned, unsigned > data_index_to_cell_index_;

  //      data_index          data_index
  // data index to neighbor data indices
  std::map<unsigned, std::vector<unsigned> > cell_neighbor_data_index_;

  // center of iris cells
  std::vector<vgl_point_3d<double> > iris_cell_centers_;

  // corresponding data index
  std::vector<unsigned> iris_cell_data_index_;

  // center of pupil cells
  std::vector<vgl_point_3d<double> > pupil_cell_centers_;

  // corresponding data index
  std::vector<unsigned> pupil_cell_data_index_;

  //==== upper eyelid ======
  boxm2_vecf_eyelid eyelid_geo_;

  boxm2_data<BOXM2_PIXEL>* eyelid_;      // is voxel an eyelid point?

  // centers of spherical shell voxels
  std::vector<vgl_point_3d<double> > eyelid_cell_centers_;

  // corresponding data indices
  std::vector<unsigned> eyelid_cell_data_index_;

  //      cell_index          cell_index
  // neighbors of each shell voxel
  std::map<unsigned, std::vector<unsigned> > eyelid_cell_neighbor_cell_index_;

  //     data_index cell_index
  // data index to shell index
  std::map<unsigned, unsigned > eyelid_data_index_to_cell_index_;

  //      data_index          data_index
  // data index to neighbor data indices
  std::map<unsigned, std::vector<unsigned> > eyelid_cell_neighbor_data_index_;

  //==== lower_eyelid ======
  boxm2_vecf_eyelid lower_eyelid_geo_;

  // is voxel an eyelid point?
  boxm2_data<BOXM2_PIXEL>* lower_eyelid_;

  // centers of spherical shell voxels
  std::vector<vgl_point_3d<double> > lower_eyelid_cell_centers_;

  // corresponding data indices
  std::vector<unsigned> lower_eyelid_cell_data_index_;

  //      cell_index          cell_index
  // neighbors of each shell voxel
  std::map<unsigned, std::vector<unsigned> > lower_eyelid_cell_neighbor_cell_index_;

  //     data_index cell_index
  // data index to shell index
  std::map<unsigned, unsigned > lower_eyelid_data_index_to_cell_index_;

  //      data_index          data_index
  // data index to neighbor data indices
  std::map<unsigned, std::vector<unsigned> > lower_eyelid_cell_neighbor_data_index_;

  //==== eyelid crease ======
  boxm2_vecf_eyelid_crease eyelid_crease_geo_;

  // is voxel an eyelid point?
  boxm2_data<BOXM2_PIXEL>* eyelid_crease_;

  // centers of spherical shell voxels
  std::vector<vgl_point_3d<double> > eyelid_crease_cell_centers_;
  // corresponding data indices
  std::vector<unsigned> eyelid_crease_cell_data_index_;

  //      cell_index          cell_index
  // neighbors of each shell voxel
  std::map<unsigned, std::vector<unsigned> > eyelid_crease_cell_neighbor_cell_index_;

  //     data_index cell_index
  // data index to shell index
  std::map<unsigned, unsigned > eyelid_crease_data_index_to_cell_index_;

  //      data_index          data_index
  // data index to neighbor data indices
  std::map<unsigned, std::vector<unsigned> > eyelid_crease_cell_neighbor_data_index_;
  vnl_vector_fixed<unsigned char,8> random_color(bool yuv = true);

private:
  bool extrinsic_only_;
  bool intrinsic_change_;
  bool target_data_extracted_;
  float sigma_;

 //: assign target cell centers that map to the source scene bounding box
  void determine_target_box_cell_centers();
};

#endif // boxm2_vecf_orbit_scene_h_
