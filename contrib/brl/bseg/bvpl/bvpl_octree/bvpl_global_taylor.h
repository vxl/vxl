// This is brl/bseg/bvpl/bvpl_octree/bvpl_global_taylor.h
#ifndef bvpl_global_taylor_h
#define bvpl_global_taylor_h
//:
// \file
// \brief A class to compute taylor features across different boxm_scenes
// \author Isabel Restrepo mir@lems.brown.edu
// \date  11-Apr-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boxm/boxm_scene.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <vbl/vbl_ref_count.h>

//Forward declaration
class bvpl_global_corners;


class bvpl_global_taylor : public vbl_ref_count
{
  
public:
  
  //: Constructor  from xml file
  bvpl_global_taylor(const vcl_string &path);

  //: Init auxiliary scenes and smallest cell length values
  void init();

  //: Compute the 10 taylor kernels for this scene at current block. The output is saved to the projection scene as a 10-d vector
  void compute_taylor_coefficients(int scene_id, int block_i, int block_j, int block_k);

  //: Extract a particular coefficient scene
  void extract_coefficient_scene(int scene_id, int coefficient_id, boxm_scene<boct_tree<short, float > > *coeff_scene);

  //: Threshold non-salient features according to Harris' measure
  void threshold_corners(int scene_id, int block_i, int block_j, int block_k, double k);
  
  //: Write to taylor_global_info.xml
  void xml_write();

  // Accessors
  vcl_string path_out() const { return path_out_; }
  vcl_vector<vcl_string> scenes() const { return scenes_; }
  vcl_vector<vcl_string> aux_dirs() const { return aux_dirs_; }
  vcl_string aux_dirs( unsigned i ) const { return aux_dirs_[i]; }
  vcl_vector<double> cell_lengths() const { return finest_cell_length_; }
  //vcl_vector<bool> training_scenes() const { return training_scenes_; }

  boxm_scene_base_sptr load_scene (int scene_id);
  //boxm_scene_base_sptr load_train_scene (int scene_id);
  boxm_scene_base_sptr load_valid_scene (int scene_id);
  boxm_scene_base_sptr load_projection_scene (int scene_id);
  
  vcl_string xml_path() { return path_out_ + "/taylor_global_info.xml"; }
  
  friend class bvpl_global_corners;
  
protected:
  
  //: A vector to hold scene paths
  vcl_vector<vcl_string> scenes_;
  //: A vector to hold paths to keep any kind of auxiliary scene or info(must be in the same order as scenes_)
  vcl_vector<vcl_string> aux_dirs_;
  //: A vector to hold the finest cell length of the corresponding scene scene
  vcl_vector<double> finest_cell_length_;
  //: A vector that indicates whether a scene should be used as train (True) or test(False). Order is equivalent to scenes_
  //vcl_vector<bool> training_scenes_;

  //: A vector to 2-degree taylor approximation kernels
  //  The order is I0, Ix, Iy, Iz, Ixx, Iyy, Izz, Ixy, Ixz, Iyz
  bvpl_kernel_vector_sptr kernel_vector_;

  //: Path to kernel files
  vcl_string kernels_path_;
  //: Path to xml info file
  vcl_string path_out_;

};

typedef vbl_smart_ptr<bvpl_global_taylor > bvpl_global_taylor_sptr;


#endif
