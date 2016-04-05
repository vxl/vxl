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

template <class T_data, unsigned DIM>
boxm_apm_type bvpl_taylor_apm();

namespace bvpl_global_tylor_defaults
{
  const std::string kernel_names[10] = {"I0", "Ix", "Iy", "Iz", "Ixx", "Iyy", "Izz", "Ixy", "Ixz", "Iyz" };
;
}

template<class T_data, unsigned DIM>
class bvpl_global_taylor : public vbl_ref_count
{
 public:

  //: Constructor  from xml file
  bvpl_global_taylor(const std::string &path, const std::string kernel_names[]);

  //: Init auxiliary scenes and smallest cell length values
  void init();

  //: Compute the DIM taylor kernels for this scene at current block. The output is saved to the projection scene as a DIM-d vector
  void compute_taylor_coefficients(int scene_id, int block_i, int block_j, int block_k);

  //: Compute reconstruction error
  bool compute_approximation_error(int scene_id, int block_i, int block_j, int block_k);

  //: Extract a particular coefficient scene
  void extract_coefficient_scene(int scene_id, int coefficient_id, boxm_scene<boct_tree<short, float > > *coeff_scene);

  //: Threshold non-salient features according to Harris' measure
  void threshold_corners(int scene_id, int block_i, int block_j, int block_k, double k);

  //: Write to taylor_global_info.xml
  void xml_write();

  // Accessors
  std::string path_out() const { return path_out_; }
  std::vector<std::string> scenes() const { return scenes_; }
  std::vector<std::string> aux_dirs() const { return aux_dirs_; }
  std::string aux_dirs( unsigned i ) const { return aux_dirs_[i]; }
  std::vector<double> cell_lengths() const { return finest_cell_length_; }
  //std::vector<bool> training_scenes() const { return training_scenes_; }

  boxm_scene_base_sptr load_scene (int scene_id);
  boxm_scene_base_sptr load_error_scene (int scene_id);
  boxm_scene_base_sptr load_valid_scene (int scene_id);
  boxm_scene_base_sptr load_projection_scene (int scene_id);

  std::string xml_path() { return path_out_ + "/taylor_global_info.xml"; }

  friend class bvpl_global_corners;

 protected:

  //: A vector to hold scene paths
  std::vector<std::string> scenes_;
  //: A vector to hold paths to keep any kind of auxiliary scene or info(must be in the same order as scenes_)
  std::vector<std::string> aux_dirs_;
  //: A vector to hold the finest cell length of the corresponding scene scene
  std::vector<double> finest_cell_length_;
  //: A vector that indicates whether a scene should be used as train (True) or test(False). Order is equivalent to scenes_
  //std::vector<bool> training_scenes_;

  //: A vector to 2-degree taylor approximation kernels
  //  The order is I0, Ix, Iy, Iz, Ixx, Iyy, Izz, Ixy, Ixz, Iyz (therefore this class works for dimensions 1-10)
  bvpl_kernel_vector_sptr kernel_vector_;

  //: Path to kernel files
  std::string kernels_path_;
  //: Path to xml info file
  std::string path_out_;
};

#include <vbl/vbl_smart_ptr.h>
// This does not really seem like a good idea to me ... - PVr.
typedef vbl_smart_ptr<bvpl_global_taylor<double,10> > bvpl_global_taylor_sptr;


#endif
