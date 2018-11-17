#ifndef boxm2_vecf_eye_scene_h_
#define boxm2_vecf_eye_scene_h_
//:
// \file
// \brief  boxm2_vecf_eye_scene models the human eye
//
// \author J.L. Mundy
// \date   21 Nov 2014
//
//  The eye surface is constructed from two layers of voxels one
// at the eye radius and one at eye radius-1.0mm. The double
// layer insures that there is complete coverage in spite
// of voxel quantization effects. The voxels on the eye surface are
// assigned by brute force scanning of the two spherical surfaces
// and adding voxels to the surface set if they contain one or more points
// on within the spherical shell. The interior of the eye
// is empty.
//  Currently, the appearance model is BOXM2_MOG3_GREY
// but probably a single Gaussian mode would suffice.
// The voxels of the eye surface have byte labels corresponding
// to the different anatomical structure. It is possible for
// a single voxel to have multiple labels such as the iris and
// the pupil. That is, if one considers the pupil to be a subset
// of the iris, both iris and pupil labels will be true on the
// pupil.
//
// A vector field is defined internally corresponding to the rotation
// of the eye to obtain a given pointing direction, specified in
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
#include <iostream>
#include <string>
#include <vector>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/algo/vgl_rotation_3d.h>
#include "boxm2_vecf_eye_params.h"
class boxm2_vecf_eye_scene : public boxm2_scene
{
 public:
 boxm2_vecf_eye_scene(): alpha_data_(nullptr), app_data_(nullptr), nobs_data_(nullptr), sphere_(nullptr), sphere_dist_(nullptr), iris_(nullptr), pupil_(nullptr),
    target_alpha_data_(nullptr),target_app_data_(nullptr), target_nobs_data_(nullptr){}

  //: set parameters
  void set_params(boxm2_vecf_eye_params const& params){ params_ = params;}

  //: construct from scene file specification, use exising database unless initialize == true
  // otherwise scan a spherical shell to define the voxel surface
  boxm2_vecf_eye_scene(std::string const& scene_file, bool initialize = false);

  //: map eye data to the target scene
  void map_to_target(const boxm2_scene_sptr& target_scene, std::string const& app_id="");

  //: compute an inverse vector field defined at sphere points (debug helper)
  std::vector<vgl_vector_3d<double> > inverse_vector_field(vgl_rotation_3d<double> const& rot) const;

  boxm2_scene_sptr scene(){ return base_model_;}

private:
  boxm2_scene_sptr base_model_;
  //:scan spherical shell
  void create_eye();
  //:read block eye data and reset indices
  void recreate_eye();
  //: set up pointers to source block databases
  void extract_block_data();
  //: set up pointers to target block databases
  void extract_target_block_data(boxm2_scene_sptr target_scene, std::string const& app_id);
  //: initialize the source block data
  void fill_block();
  //: initialize the full target block (not currently used )
  void fill_target_block();
  //: cache index of neighboring cells
  void find_cell_neigborhoods();
  //: set sphere surface, sclera, iris and pupil cell indices
  void reset_indices();
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
  //: closest point in the spherical shell
  vgl_point_3d<double> closest_point_on_shell(vgl_point_3d<double> const& p) const;
  //: scan over target cells and interpolate appearance and alpha from source
  void apply_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf);
  boxm2_block* blk_;                     // the source block
  boxm2_block* target_blk_;              // the target block
  boxm2_data<BOXM2_ALPHA>* alpha_data_;  // source alpha database
  boxm2_data<BOXM2_MOG3_GREY>* app_data_;// source appearance database
  boxm2_data<BOXM2_NUM_OBS>* nobs_data_; // source nobs database
  boxm2_data<BOXM2_PIXEL>* sphere_;      // is voxel a surface point?
  boxm2_data<BOXM2_FLOAT>* sphere_dist_; // distance to nearst sphere shell point
  boxm2_data<BOXM2_PIXEL>* iris_;        // is voxel an iris point
  boxm2_data<BOXM2_PIXEL>* pupil_;       // is voxel a pupil point

  boxm2_data<BOXM2_ALPHA>* target_alpha_data_;   //target alpha database
  boxm2_data<BOXM2_MOG3_GREY>* target_app_data_; //target appearance database
  boxm2_data<BOXM2_NUM_OBS>* target_nobs_data_;  //target nobs

  boxm2_vecf_eye_params params_;                          // parameter struct
  std::vector<vgl_point_3d<double> > sphere_cell_centers_; // centers of spherical shell voxels
  std::vector<unsigned> sphere_cell_data_index_;           // corresponding data indices
    //      cell_index          cell_index
  std::map<unsigned, std::vector<unsigned> > cell_neighbor_cell_index_; // neighbors of each shell voxel
  //     data_index cell_index
  std::map<unsigned, unsigned > data_index_to_cell_index_;             // data index to shell index
  //      data_index          data_index
  std::map<unsigned, std::vector<unsigned> > cell_neighbor_data_index_; // data index to neighbor data indices
  //      data_index          distance
  std::map<unsigned, std::vector<double> > cell_neighbor_distance_;     // data index to neighbor distances
  std::vector<double> closest_sphere_distance_norm_;                   // closest spherical shell distance/(side length)

  std::vector<vgl_point_3d<double> > iris_cell_centers_;               // center of iris cells
  std::vector<unsigned> iris_cell_data_index_;                         // corresponding data index

  std::vector<vgl_point_3d<double> > pupil_cell_centers_;              // center of pupil cells
  std::vector<unsigned> pupil_cell_data_index_;                        // corresponding data index
};
#endif // boxm2_vecf_eye_scene_h_
