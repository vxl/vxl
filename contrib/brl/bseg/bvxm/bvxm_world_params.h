#ifndef bvxm_world_params_h_
#define bvxm_world_params_h_
//:
// \file
// \brief
// \author Thomas Pollard
// \date January 12, 2008
//
// \verbatim
//  Modifications
//   Apr 18, 2008 Ozge C. Ozcanli - added the basis vectors to be used in transforming the local coordinate system of the world to actual 3D world
//                                  check the method voxel_index_to_xyz() in bvxm_voxel_world class for their use
//                                - also added a version number and updated binary load and save methods to read and write new parameters
//                                  since there was no version number previously, the parameters written previous to this update won't be readable
//                                  the version number will be 2 and one may switch to previous version temporarily and rebuilt to read the previous version and write back in version two
// \endverbatim

#include <string>
#include <iostream>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>

#include <vsl/vsl_binary_io.h>
#include <vgl/io/vgl_io_vector_3d.h>
#include <vgl/io/vgl_io_point_3d.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>

class bvxm_world_params : public vbl_ref_count
{
 public:

  bvxm_world_params();
  ~bvxm_world_params() override;

  //enum appearance_model_type { apm_unknown, mog_grey, mog_rgb };

  void set_params(
    const std::string& model_dir,
    const vgl_point_3d<float>& corner,
    const vgl_vector_3d<unsigned int>& num_voxels,
    float voxel_length,
    const vpgl_lvcs_sptr& lvcs = vpgl_lvcs_sptr(nullptr),
    float min_ocp_prob = 0.001f,
    float max_ocp_prob = 0.999f,
    unsigned max_scale = 1,
    vgl_vector_3d<float> basex = vgl_vector_3d<float>(1.0f,0.0f,0.0f),
    vgl_vector_3d<float> basey = vgl_vector_3d<float>(0.0f,1.0f,0.0f),
    vgl_vector_3d<float> basez = vgl_vector_3d<float>(0.0f,0.0f,1.0f));

  inline std::string model_dir() const { return model_dir_; }
  inline vgl_point_3d<float> corner() const { return corner_; }
  inline void set_corner(vgl_point_3d<float>& new_c) { corner_ = new_c; }

  inline vgl_point_3d<float> rpc_origin() const { return rpc_origin_; }
  inline void set_rpc_origin(vgl_point_3d<float>& new_rpc_origin) {
    vgl_point_3d<float> old_corner = corner();
    vgl_point_3d<float> new_corner(
      old_corner.x() + new_rpc_origin.x() - rpc_origin_.x(),
      old_corner.y() + new_rpc_origin.y() - rpc_origin_.y(),
      old_corner.z() + new_rpc_origin.z() - rpc_origin_.z());
    set_corner(new_corner);
    rpc_origin_ = new_rpc_origin;
  }

  inline vgl_vector_3d<unsigned int> num_voxels(unsigned scale=0) {
    vgl_vector_3d<unsigned int> num_voxels_scaled;
    double divisor= 1.0 / double(1 << scale); // actually, inverse of divisor
    num_voxels_scaled.set((unsigned int)(num_voxels_.x()*divisor),
                          (unsigned int)(num_voxels_.y()*divisor),
                          (unsigned int)(num_voxels_.z()*divisor));
    return num_voxels_scaled;
  }

  inline float voxel_length(unsigned scale=0) {
    return float(1<<scale)*voxel_length_; }

  inline vgl_vector_3d<float> base_x() const { return base_x_; }
  inline vgl_vector_3d<float> base_y() const { return base_y_; }
  inline vgl_vector_3d<float> base_z() const { return base_z_; }

  inline void set_base_x(vgl_vector_3d<float>& basex) { base_x_ = basex; }
  inline void set_base_y(vgl_vector_3d<float>& basey) { base_y_ = basey; }
  inline void set_base_z(vgl_vector_3d<float>& basez) { base_z_ = basez; }

  inline void set_model_dir(std::string model_dir) {model_dir_ = model_dir;}

  inline float min_occupancy_prob() const { return min_occupancy_prob_; }
  inline float max_occupancy_prob() const { return max_occupancy_prob_; }
  inline vpgl_lvcs_sptr lvcs() { return lvcs_; }

  inline float edges_n_normal() const { return edges_n_normal_; }
  inline void increment_edges_n_normal(float increment) { edges_n_normal_ += increment; }

  inline unsigned max_scale() const { return max_scale_; }
  vgl_box_3d<double> world_box_local();

  vgl_point_3d<float> center();

  //: Serial I/O format version
  virtual unsigned version() const { return 2; }

  //: Binary save parameters to stream.
  void b_write(vsl_b_ostream & os) const;

  //: Binary load parameters from stream.
  void b_read(vsl_b_istream & is);

  //: write as xml file to be passed to bvxm_create_world process
  void write_xml(std::string const& filename, std::string const& lvcs_filename);

 protected:

  std::string model_dir_;
  vgl_point_3d<float> corner_;
  vgl_point_3d<float> rpc_origin_;
  vgl_vector_3d<unsigned int> num_voxels_;
  float voxel_length_;
  vpgl_lvcs_sptr lvcs_;
  float min_occupancy_prob_;
  float max_occupancy_prob_;
  float edges_n_normal_;

  vgl_vector_3d<float> base_x_;
  vgl_vector_3d<float> base_y_;
  vgl_vector_3d<float> base_z_;

  unsigned max_scale_;

 private:

  friend std::ostream&  operator << (std::ostream& os, bvxm_world_params const& params);
  friend std::istream& operator >> (std::istream& is, bvxm_world_params &params);
};

std::ostream&  operator << (std::ostream& os, bvxm_world_params const& params);
std::istream& operator >> (std::istream& is, bvxm_world_params &params);

typedef vbl_smart_ptr<bvxm_world_params> bvxm_world_params_sptr;

#endif // bvxm_world_params_h_
