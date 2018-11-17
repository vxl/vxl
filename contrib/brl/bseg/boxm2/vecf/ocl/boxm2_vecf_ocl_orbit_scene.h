#ifndef boxm2_vecf_ocl_orbit_scene_h_
#define boxm2_vecf_ocl_orbit_scene_h_
//:
// \file
// \brief  boxm2_vecf_ocl_orbit_scene models the human eye region
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
#include "../boxm2_vecf_orbit_params.h"
#include <bocl/bocl_kernel.h>
#include "../boxm2_vecf_eyelid.h"
#include "../boxm2_vecf_eyelid_crease.h"
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/vecf/ocl/boxm2_vecf_ocl_orbit_vector_field.h>
#include "boxm2_vecf_ocl_transform_scene.h"
#include "boxm2_vecf_ocl_vector_field_adaptor.h"
//#define DEBUG_CL
class boxm2_vecf_ocl_orbit_scene : public boxm2_vecf_articulated_scene
{
friend class boxm2_vecf_ocl_appearance_extractor; //the appearance extractor needs to signal a change to the original model when its apm is updated
 public:
  enum anat_type { SPHERE, IRIS, PUPIL, UPPER_LID, LOWER_LID, EYELID_CREASE, NO_TYPE};
 boxm2_vecf_ocl_orbit_scene(): alpha_data_(nullptr), app_data_(nullptr), nobs_data_(nullptr), sphere_(nullptr), iris_(nullptr), pupil_(nullptr),
                               eyelid_(nullptr), target_alpha_data_(nullptr),target_app_data_(nullptr), target_nobs_data_(nullptr), extrinsic_only_(false),target_blk_(nullptr),target_data_extracted_(false),boxm2_vecf_articulated_scene(),sigma_(0.5f),device_(nullptr),opencl_cache_(nullptr),scene_transformer_(nullptr){
 }

  //: set parameters
  ~boxm2_vecf_ocl_orbit_scene() override= default;
  bool set_params(boxm2_vecf_articulated_params const& params) override;

  //: construct from scene file specification, use exising database unless initialize == true
  // otherwise scan a spherical shell to define the voxel surface

  boxm2_vecf_ocl_orbit_scene(std::string const& scene_file, const bocl_device_sptr& device, const boxm2_opencl_cache_sptr& opencl_cache, bool is_single_instance = true, bool is_right = false);

  //: refine target cells to match the refinement level of the source block
  int prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index) override{return -1;}//FIXME
  //: compute inverse vector field for unrefined sub_block centers
  void inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const& unrefined_target_pts) override{}//FIXME
  bool inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const override{return false;}//FIXME
  bool apply_vector_field(cell_info const& target_cell, vgl_vector_3d<double> const& inv_vf) override{return false;}//FIXME

  bool map_orbit_to_target_single_pass(const boxm2_scene_sptr& target_scene);
  //: map eye data to the target scene
  void map_to_target(boxm2_scene_sptr target_scene) override;

  //: extract the appearance from the target scene
  void extract_appearance_from_target(boxm2_scene_sptr target_scene);

  //: compute an inverse vector field for rotation of the eye globe
  void inverse_vector_field_eye(vgl_rotation_3d<double> const& rot, std::vector<vgl_vector_3d<double> >& vfield,
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
  boxm2_data<BOXM2_VIS_SCORE>* source_vis_score_data_;  //target nobs
  boxm2_data<BOXM2_GAUSS_RGB>* target_color_data_;
  std::vector<cell_info> box_cell_centers_;       // cell centers in the target block
  boxm2_vecf_orbit_params params_;               // parameter struct
  bool is_right_;
  vgl_vector_3d<double> estimated_look_dir_;
  double estimated_dt_;
  // =============  eye ===============
  boxm2_data<BOXM2_PIXEL>* sphere_;      // is voxel a eye sphere point?
  boxm2_data<BOXM2_PIXEL>* iris_;        // is voxel an iris point
  boxm2_data<BOXM2_PIXEL>* pupil_;       // is voxel a pupil point
  boxm2_data<BOXM2_FLOAT>* radial_distance_;       // is voxel a pupil point

  std::vector<vgl_point_3d<double> > sphere_cell_centers_; // centers of spherical shell voxels
  std::vector<unsigned> sphere_cell_data_index_;           // corresponding data indices
  //      cell_index          cell_index
  std::map<unsigned, std::vector<unsigned> > cell_neighbor_cell_index_; // neighbors of each shell voxel
  //     data_index cell_index
  std::map<unsigned, unsigned > data_index_to_cell_index_;             // data index to shell index
  //      data_index          data_index
  std::map<unsigned, std::vector<unsigned> > cell_neighbor_data_index_; // data index to neighbor data indices

  std::vector<vgl_point_3d<double> > iris_cell_centers_;               // center of iris cells
  std::vector<unsigned> iris_cell_data_index_;                         // corresponding data index

  std::vector<vgl_point_3d<double> > pupil_cell_centers_;              // center of pupil cells
  std::vector<unsigned> pupil_cell_data_index_;                        // corresponding data index

  //==== upper eyelid ======
  boxm2_vecf_eyelid eyelid_geo_;

  boxm2_data<BOXM2_PIXEL>* eyelid_;      // is voxel an eyelid point?

  std::vector<vgl_point_3d<double> > eyelid_cell_centers_; // centers of spherical shell voxels
  std::vector<unsigned> eyelid_cell_data_index_;           // corresponding data indices
    //      cell_index          cell_index
  std::map<unsigned, std::vector<unsigned> > eyelid_cell_neighbor_cell_index_; // neighbors of each shell voxel
  //     data_index cell_index
  std::map<unsigned, unsigned > eyelid_data_index_to_cell_index_;             // data index to shell index
  //      data_index          data_index
  std::map<unsigned, std::vector<unsigned> > eyelid_cell_neighbor_data_index_; // data index to neighbor data indices

  //==== lower_eyelid ======
  boxm2_vecf_eyelid lower_eyelid_geo_;

  boxm2_data<BOXM2_PIXEL>* lower_eyelid_;      // is voxel an eyelid point?

  std::vector<vgl_point_3d<double> > lower_eyelid_cell_centers_; // centers of spherical shell voxels
  std::vector<unsigned> lower_eyelid_cell_data_index_;           // corresponding data indices
    //      cell_index          cell_index
  std::map<unsigned, std::vector<unsigned> > lower_eyelid_cell_neighbor_cell_index_; // neighbors of each shell voxel
  //     data_index cell_index
  std::map<unsigned, unsigned > lower_eyelid_data_index_to_cell_index_;             // data index to shell index
  //      data_index          data_index
  std::map<unsigned, std::vector<unsigned> > lower_eyelid_cell_neighbor_data_index_; // data index to neighbor data indices

  //==== eyelid crease ======
  boxm2_vecf_eyelid_crease eyelid_crease_geo_;

  boxm2_data<BOXM2_PIXEL>* eyelid_crease_;      // is voxel an eyelid point?

  std::vector<vgl_point_3d<double> > eyelid_crease_cell_centers_; // centers of spherical shell voxels
  std::vector<unsigned> eyelid_crease_cell_data_index_;           // corresponding data indices
    //      cell_index          cell_index
  std::map<unsigned, std::vector<unsigned> > eyelid_crease_cell_neighbor_cell_index_; // neighbors of each shell voxel
  //     data_index cell_index
  std::map<unsigned, unsigned > eyelid_crease_data_index_to_cell_index_;             // data index to shell index
  //      data_index          data_index
  std::map<unsigned, std::vector<unsigned> > eyelid_crease_cell_neighbor_data_index_; // data index to neighbor data indices
  vnl_vector_fixed<unsigned char,8> random_color(bool yuv = true);
  bool update_source_gpu_buffers(bool write = false);
private:
  bool compile_kernels();
  bool get_scene_appearance( std::string& options);
  bool update_target_gpu_buffers(const boxm2_scene_sptr& target_scene, const boxm2_block_id& id,bool write = false);

  std::string app_type_,color_app_type_id_;
  bool extrinsic_only_;
  bool intrinsic_change_;
  bool target_data_extracted_;
  bool care_;
  float sigma_;
  bocl_mem* target_pts_;
  bocl_mem* source_pts_;
  bocl_mem *target_app_base_,*target_color_base_,*target_vis_base_,*target_nobs_base_,*target_alpha_base_;
  bocl_mem* source_app_base_,*source_color_base_,*source_vis_base_,*source_nobs_base_,*source_alpha_base_;
  bocl_mem* sphere_base_,*eyelid_base_,*eyelid_crease_base_,*lower_eyelid_base_,*pupil_base_,*iris_base_;
  bocl_mem_sptr eyelid_crease_geo_cl_,eyelid_geo_cl_,lower_eyelid_geo_cl_;
  float eyelid_cl_buf_[21];
  float eyelid_crease_cl_buf_[16];
  float lower_eyelid_cl_buf_[16];
  bocl_mem_sptr centerX_;
  bocl_mem_sptr centerY_;
  bocl_mem_sptr centerZ_;

  cl_uchar lookup_arr_[256];
  bocl_mem_sptr octree_depth_;
  bocl_mem_sptr lookup_;


  boxm2_opencl_cache_sptr opencl_cache_;
  bocl_device_sptr device_;
  boxm2_vecf_ocl_orbit_vector_field vect_field_;
  boxm2_vecf_ocl_transform_scene_sptr scene_transformer_;
  std::vector<bocl_kernel*> kernels;
  cl_command_queue queue_;
 //: assign target cell centers that map to the source scene bounding box
  void determine_target_box_cell_centers();
};

#endif // boxm2_vecf_ocl_orbit_scene_h_
