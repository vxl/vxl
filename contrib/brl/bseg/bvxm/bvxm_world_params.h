#ifndef _bvxm_world_params_h_
#define _bvxm_world_params_h_

//:
// \file
// \brief
// \author Thomas Pollard
// \date 1/12/08
//
// \verbatim
//  Modifications
//   04/18/2008 - Ozge C. Ozcanli - added the basis vectors to be used in transforming the local coordinate system of the world to actual 3D world
//                                  check the method voxel_index_to_xyz() in bvxm_voxel_world class for their use
//                                - also added a version number and updated binary load and save methods to read and write new parameters
//                                  since there was no version number previously, the parameters written previous to this update won't be readable
//                                  the version number will be 2 and one may switch to previous version temporarily and rebuilt to read the previous version and write back in version two
//
// \endverbatim

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vsl/vsl_binary_io.h>
#include <vgl/io/vgl_io_vector_3d.h>
#include <vgl/io/vgl_io_point_3d.h>
#include <bgeo/bgeo_lvcs.h>
#include <bgeo/bgeo_lvcs_sptr.h>

class bvxm_world_params : public vbl_ref_count {

public:

  bvxm_world_params();
  ~bvxm_world_params();

  //enum appearance_model_type {apm_unknown, mog_grey, mog_rgb};

  void set_params(
    const vcl_string& model_dir,
    const vgl_point_3d<float>& corner,
    const vgl_vector_3d<unsigned int>& num_voxels,
    float voxel_length,
    bgeo_lvcs_sptr lvcs = bgeo_lvcs_sptr(0),
    float min_ocp_prob = 0.001,
    float max_ocp_prob = 0.999,
    unsigned max_scale = 0,
    vgl_vector_3d<float> basex = vgl_vector_3d<float>(1.0f,0.0f,0.0f),
    vgl_vector_3d<float> basey = vgl_vector_3d<float>(0.0f,1.0f,0.0f),
    vgl_vector_3d<float> basez = vgl_vector_3d<float>(0.0f,0.0f,1.0f));

  inline vcl_string model_dir(){ return model_dir_; }
  inline vgl_point_3d<float> corner(){ return corner_; }
  inline void set_corner(vgl_point_3d<float>& new_c){ corner_ = new_c; }

  inline vgl_vector_3d<float> base_x(){ return base_x_; }
  inline vgl_vector_3d<float> base_y(){ return base_y_; }
  inline vgl_vector_3d<float> base_z(){ return base_z_; }

  inline void set_base_x(vgl_vector_3d<float>& basex) { base_x_ = basex; }
  inline void set_base_y(vgl_vector_3d<float>& basey) { base_y_ = basey; }
  inline void set_base_z(vgl_vector_3d<float>& basez) { base_z_ = basez; }

  inline vgl_vector_3d<unsigned int> num_voxels(){ return num_voxels_; }
  inline float voxel_length(){ return voxel_length_; }
  inline float min_occupancy_prob(){ return min_occupancy_prob_;}
  inline float max_occupancy_prob(){ return max_occupancy_prob_;}
  inline bgeo_lvcs_sptr lvcs(){return lvcs_;}

  inline unsigned max_scale(){return max_scale_;}
  vgl_box_3d<double> world_box_local();

  vgl_point_3d<float> center();

  //: Serial I/O format version
  virtual unsigned version() const {return 2;}

  //: Binary save parameters to stream.
  void b_write(vsl_b_ostream & os) const;
  
  //: Binary load parameters from stream.
  void b_read(vsl_b_istream & is);


protected:

  vcl_string model_dir_;
  vgl_point_3d<float> corner_;
  vgl_vector_3d<unsigned int> num_voxels_;
  float voxel_length_;
  bgeo_lvcs_sptr lvcs_;
  float min_occupancy_prob_;
  float max_occupancy_prob_;

  vgl_vector_3d<float> base_x_;
  vgl_vector_3d<float> base_y_;
  vgl_vector_3d<float> base_z_;

  unsigned max_scale_;

private:

  friend vcl_ostream&  operator << (vcl_ostream& os, bvxm_world_params const& params);
  friend vcl_istream& operator >> (vcl_istream& is, bvxm_world_params &params);

};

vcl_ostream&  operator << (vcl_ostream& os, bvxm_world_params const& params);
vcl_istream& operator >> (vcl_istream& is, bvxm_world_params &params);

typedef vbl_smart_ptr<bvxm_world_params> bvxm_world_params_sptr;

#endif // _bvxm_world_params_h_
