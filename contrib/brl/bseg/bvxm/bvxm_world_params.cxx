#include "bvxm_world_params.h"
//:
// \file
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vpgl/io/vpgl_io_lvcs.h>

//---------------------------------------------------
bvxm_world_params::bvxm_world_params()
{
  model_dir_ = "NONE";
  corner_ = vgl_point_3d<float>(0.0,0.0,0.0);
  num_voxels_ = vgl_vector_3d<unsigned int>(10,10,10);
  voxel_length_ = 1.0;
  base_x_ = vgl_vector_3d<float>(1.0f,0.0f,0.0f);
  base_y_ = vgl_vector_3d<float>(0.0f,1.0f,0.0f);
  base_z_ = vgl_vector_3d<float>(0.0f,0.0f,1.0f);
  rpc_origin_ = vgl_point_3d<float>(0.0,0.0,0.0);
  edges_n_normal_ = 0.0f;
  max_scale_ = 1;
};


//---------------------------------------------------
bvxm_world_params::~bvxm_world_params()
= default;;


//---------------------------------------------------
void
bvxm_world_params::set_params(
  const std::string& model_dir,
  const vgl_point_3d<float>& corner,
  const vgl_vector_3d<unsigned int>& num_voxels,
  float voxel_length,
  const vpgl_lvcs_sptr& lvcs,
  float min_ocp_prob,
  float max_ocp_prob,
  unsigned max_scale,
  vgl_vector_3d<float> basex,
  vgl_vector_3d<float> basey,
  vgl_vector_3d<float> basez)
{
  model_dir_ = model_dir;
  corner_ = corner;
  num_voxels_ = num_voxels;
  voxel_length_ = voxel_length;
  lvcs_ = lvcs;
  min_occupancy_prob_ = min_ocp_prob;
  max_occupancy_prob_ = max_ocp_prob;
  max_scale_=max_scale;
  base_x_ = basex;
  base_y_ = basey;
  base_z_ = basez;
};

vgl_box_3d<double> bvxm_world_params::world_box_local()
{
  double xdim = double(num_voxels_.x())*voxel_length_;
  double ydim = double(num_voxels_.y())*voxel_length_;
  double zdim = double(num_voxels_.z())*voxel_length_;
  double c[3];
  c[0] = corner_.x();
  c[1] = corner_.y();
  c[2] = corner_.z();
  vgl_box_3d<double> box(c, xdim, ydim, zdim, vgl_box_3d<double>::min_pos);
  return box;
}

void bvxm_world_params::b_write(vsl_b_ostream & os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os,model_dir_);
  vsl_b_write(os,corner_);
  vsl_b_write(os,num_voxels_);
  vsl_b_write(os,voxel_length_);
  vsl_b_write(os, *lvcs_);
  vsl_b_write(os,min_occupancy_prob_);
  vsl_b_write(os,max_occupancy_prob_);
  vsl_b_write(os, base_x_);
  vsl_b_write(os, base_y_);
  vsl_b_write(os, base_z_);
  vsl_b_write(os, max_scale_);
}


void bvxm_world_params::b_read(vsl_b_istream & is)
{
  unsigned ver = version();
  switch (ver)
  {
   case 1:
    vsl_b_read(is,model_dir_);
    vsl_b_read(is,corner_);
    vsl_b_read(is,num_voxels_);
    vsl_b_read(is,voxel_length_);
    lvcs_ = new vpgl_lvcs();
    vsl_b_read(is, *lvcs_);
    vsl_b_read(is,min_occupancy_prob_);
    vsl_b_read(is,max_occupancy_prob_);
    break;
   default:
    vsl_b_read(is, ver);
    switch (ver) {
     case 2:
      vsl_b_read(is,model_dir_);
      vsl_b_read(is,corner_);
      vsl_b_read(is,num_voxels_);
      vsl_b_read(is,voxel_length_);
      lvcs_ = new vpgl_lvcs();
      vsl_b_read(is, *lvcs_);
      vsl_b_read(is,min_occupancy_prob_);
      vsl_b_read(is,max_occupancy_prob_);
      vsl_b_read(is, base_x_);
      vsl_b_read(is, base_y_);
      vsl_b_read(is, base_z_);
      break;
     default:
      std::cout << "In bvxm_world_params::b_read() - Version not supported\n";
    }
  }
}

//: output world_params to stream
std::ostream&  operator << (std::ostream& os, bvxm_world_params const& params)
{
  if (!params.lvcs_) {
    os << params.model_dir_ << std::endl
       << params.corner_.x() << ' ' << params.corner_.y() << ' ' << params.corner_.z() << std::endl
       << params.num_voxels_.x() << ' ' << params.num_voxels_.y() << ' ' << params.num_voxels_.z() << std::endl
       << params.voxel_length_ << std::endl
       << params.min_occupancy_prob_ << std::endl
       << params.max_occupancy_prob_ << std::endl;
  }
  else {
    os << params.model_dir_ << std::endl
       << params.corner_.x() << ' ' << params.corner_.y() << ' ' << params.corner_.z() << std::endl
       << params.num_voxels_.x() << ' ' << params.num_voxels_.y() << ' ' << params.num_voxels_.z() << std::endl
       << params.voxel_length_ << std::endl
       << *(params.lvcs_) << std::endl
       << params.min_occupancy_prob_ << std::endl
       << params.max_occupancy_prob_ << std::endl;
  }

  return os;
}

//: input world_params from stream
std::istream& operator >> (std::istream& is, bvxm_world_params &params)
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

//: write as xml file to be passed to bvxm_create_world process
void bvxm_world_params::write_xml(std::string const& filename, std::string const& lvcs_filename)
{
  std::ofstream ofs(filename.c_str());
  if (!ofs) {
    std::cerr << "In bvxm_world_params::write_xml() -- cannot open file:" << filename << '\n';
    return;
  }
  ofs << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
      << "<bvxmCreateVoxelWorldProcess>\n"
      << "  <input_directory type=\"string\" value=\"" << this->model_dir() << "\"></input_directory>\n"
      << "  <corner_x type=\"float\" value=\"" << this->corner().x() << "\"></corner_x>\n"
      << "  <corner_y type=\"float\" value=\"" << this->corner().y() << "\"></corner_y>\n"
      << "  <corner_z type=\"float\" value=\"" << this->corner().z() << "\"></corner_z>\n"
      << "  <voxel_dim_x type=\"unsigned\" value=\"" << this->num_voxels().x() << "\"></voxel_dim_x>\n"
      << "  <voxel_dim_y type=\"unsigned\" value=\"" << this->num_voxels().y() << "\"></voxel_dim_y>\n"
      << "  <voxel_dim_z type=\"unsigned\" value=\"" << this->num_voxels().z() << "\"></voxel_dim_z>\n"
      << "  <voxel_length type=\"float\" value=\"" << this->voxel_length() << "\"></voxel_length>\n"
      << "  <lvcs type=\"string\" value=\"" << lvcs_filename << "\"></lvcs>\n"
      << "  <apm_type type=\"unsigned\" value=\"1\"></apm_type>\n"
      << "  <min_ocp_prob type=\"float\" value=\"" << this->min_occupancy_prob() << "\"></min_ocp_prob>\n"
      << "  <max_ocp_prob type=\"float\" value=\"" << this->max_occupancy_prob() << "\"></max_ocp_prob>\n"
      << "  <max_scale type=\"unsigned\" value=\"" << this->max_scale() << "\"></max_scale>\n"
      << "</bvxmCreateVoxelWorldProcess>\n";
  ofs.close();
}
