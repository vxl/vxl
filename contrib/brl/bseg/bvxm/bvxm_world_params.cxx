
#include "bvxm_world_params.h"

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

//---------------------------------------------------
bvxm_world_params::bvxm_world_params()
{
  model_dir_ = "NONE";
  corner_ = vgl_point_3d<float>(0.0,0.0,0.0);
  num_voxels_ = vgl_vector_3d<unsigned int>(10,10,10);
  voxel_length_ = 1.0;
};


//---------------------------------------------------
bvxm_world_params::~bvxm_world_params()
{

};


//---------------------------------------------------
void 
bvxm_world_params::set_params(
  const vcl_string& model_dir,
  const vgl_point_3d<float>& corner,
  const vgl_vector_3d<unsigned int>& num_voxels,
  float voxel_length,
  bgeo_lvcs_sptr lvcs,
  float min_ocp_prob,
  float max_ocp_prob)
{

  model_dir_ = model_dir;
  corner_ = corner;
  num_voxels_ = num_voxels;
  voxel_length_ = voxel_length;
  lvcs_ = lvcs;
  min_occupancy_prob_ = min_ocp_prob;
  max_occupancy_prob_ = max_ocp_prob;

};

void bvxm_world_params::b_write(vsl_b_ostream & os) const
{
  vsl_b_write(os,model_dir_);
  vsl_b_write(os,corner_);
  vsl_b_write(os,num_voxels_);
  vsl_b_write(os,voxel_length_);
  lvcs_->b_write(os);
  vsl_b_write(os,min_occupancy_prob_);
  vsl_b_write(os,max_occupancy_prob_);

}


void bvxm_world_params::b_read(vsl_b_istream & is)
{
  vsl_b_read(is,model_dir_);
  vsl_b_read(is,corner_);
  vsl_b_read(is,num_voxels_);
  vsl_b_read(is,voxel_length_);
  lvcs_->b_read(is);
  vsl_b_read(is,min_occupancy_prob_);
  vsl_b_read(is,max_occupancy_prob_);

}

//: output world_params to stream
vcl_ostream&  operator << (vcl_ostream& os, bvxm_world_params const& params)
{
  os << params.model_dir_ << vcl_endl;
  os << params.corner_.x() << " " << params.corner_.y() << " " << params.corner_.z() << vcl_endl;
  os << params.num_voxels_.x() << " " << params.num_voxels_.y() << " " << params.num_voxels_.z() << vcl_endl;
  os << params.voxel_length_ << vcl_endl;
  os << *(params.lvcs_) << vcl_endl;
  os << params.min_occupancy_prob_ << vcl_endl;
  os << params.max_occupancy_prob_ << vcl_endl;

  return os;
}

//: input world_params from stream
vcl_istream& operator >> (vcl_istream& is, bvxm_world_params &params)
{
  is >> params.model_dir_;
  is >> params.corner_;
  is >> params.num_voxels_;
  is >> params.voxel_length_;
  is >> *(params.lvcs_);
  is >> params.min_occupancy_prob_;
  is >> params.max_occupancy_prob_;

  return is;
}

