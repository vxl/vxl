#ifndef _bvxm_world_params_h_
#define _bvxm_world_params_h_

//:
// \file
// \brief
// \author Thomas Pollard
// \date 1/12/08

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
    float max_ocp_prob = 0.999);

  inline vcl_string model_dir(){ return model_dir_; }
  inline vgl_point_3d<float> corner(){ return corner_; }
  inline void set_corner(vgl_point_3d<float>& new_c){ corner_ = new_c; }

  inline vgl_vector_3d<unsigned int> num_voxels(){ return num_voxels_; }
  inline float voxel_length(){ return voxel_length_; }
  inline float min_occupancy_prob(){ return min_occupancy_prob_;}
  inline float max_occupancy_prob(){ return max_occupancy_prob_;}
  inline bgeo_lvcs_sptr lvcs(){return lvcs_;}
  vgl_box_3d<double> world_box_local();

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

private:

  friend vcl_ostream&  operator << (vcl_ostream& os, bvxm_world_params const& params);
  friend vcl_istream& operator >> (vcl_istream& is, bvxm_world_params &params);

};


vcl_ostream&  operator << (vcl_ostream& os, bvxm_world_params const& params);
vcl_istream& operator >> (vcl_istream& is, bvxm_world_params &params);

typedef vbl_smart_ptr<bvxm_world_params> bvxm_world_params_sptr;

#endif // _bvxm_world_params_h_
