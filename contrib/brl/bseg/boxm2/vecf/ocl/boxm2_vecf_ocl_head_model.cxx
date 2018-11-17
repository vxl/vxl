#include "boxm2_vecf_ocl_head_model.h"
#include <algorithm>
#include <boct/boct_bit_tree.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <iostream>
#include <limits>
#include <utility>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_sphere_3d.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

boxm2_vecf_ocl_head_model::boxm2_vecf_ocl_head_model(std::string const& scene_file,const bocl_device_sptr& device,const boxm2_opencl_cache_sptr& opencl_cache,bool optimize, std::string color_apm_ident):

  boxm2_vecf_articulated_scene(scene_file,std::move(color_apm_ident)),
  scale_(1.0, 1.0, 1.0),opencl_cache_(opencl_cache),device_(device), scene_transformer_(base_model_,opencl_cache_,"",color_apm_id_),optimize_(optimize)
{
}

void boxm2_vecf_ocl_head_model::set_scale(vgl_vector_3d<double> scale) {
  if( (scale_-scale).length() >0.0001 ){
    scale_ = scale;
    intrinsic_change_ = true;
  }

}
bool boxm2_vecf_ocl_head_model::get_data(boxm2_scene_sptr scene, boxm2_block_id const& blk_id,
                                     boxm2_data_base** alpha_data,
                                     boxm2_data_base** app_data,
                                     boxm2_data_base** nobs_data)
{
  *alpha_data = boxm2_cache::instance()->get_data_base(scene, blk_id,
                                                       boxm2_data_traits<BOXM2_ALPHA>::prefix());

  *app_data = boxm2_cache::instance()->get_data_base(scene, blk_id,
                                                     boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());

  *nobs_data = boxm2_cache::instance()->get_data_base(scene, blk_id,
                                                      boxm2_data_traits<BOXM2_NUM_OBS>::prefix());

  return true;
}

void boxm2_vecf_ocl_head_model::map_to_target(boxm2_scene_sptr target_scene)
{
  intrinsic_change_ = false;
  vgl_rotation_3d<double> id; id.set_identity();
  vgl_vector_3d<double> null; null.set(0, 0, 0);
  vgl_vector_3d<double> inv_scale = vgl_vector_3d<double>(1.0/this->scale_.x(),1.0/this->scale_.y(),1.0/this->scale_.z());
  //   boxm2_vecf_ocl_transform_scene  tmp(base_model_,target_scene,this->opencl_cache_,"",color_apm_id_);
   //   tmp.transform_1_blk_interp_trilin(target_scene,id, null, inv_scale,true);
  bool transfer_data = !optimize_;
  scene_transformer_.transform_1_blk_interp_trilin(target_scene,id, null, inv_scale,transfer_data);
}

void boxm2_vecf_ocl_head_model::clear_target(boxm2_scene_sptr target_scene)
{
  // for each block of the target scene
  std::vector<boxm2_block_id> target_blocks = target_scene->get_block_ids();

  for (auto & target_block : target_blocks) {
    bocl_mem* color_app_db =  opencl_cache_->get_data(target_scene, target_block, boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_apm_id_));
    bocl_mem* nobs_db      =  opencl_cache_->get_data(target_scene, target_block, boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::prefix(color_apm_id_));
    bocl_mem* alpha_db     =  opencl_cache_->get_data(target_scene, target_block, boxm2_data_traits<BOXM2_ALPHA>::prefix());
    bocl_mem* gray_app_db  =  opencl_cache_->get_data(target_scene, target_block, boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());


    int status;
    boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype grey;
    grey.fill(128);

    cl_command_queue queue = clCreateCommandQueue(device_->context(),*(device_->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
    color_app_db->fill(queue, grey, "uchar8" );
    nobs_db-> zero_gpu_buffer(queue);
    alpha_db->zero_gpu_buffer(queue);
    gray_app_db->zero_gpu_buffer(queue);

    if (!this->optimize_ ){ //update cpu buffers also
      color_app_db->read_to_buffer(queue);
      gray_app_db ->read_to_buffer(queue);
      nobs_db     ->read_to_buffer(queue);
      alpha_db    ->read_to_buffer(queue);
      status = clFinish(queue);
    }
  } // for each target block
}
void boxm2_vecf_ocl_head_model::inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const&  /*unrefined_target_pts*/) {

  return ;
}
int boxm2_vecf_ocl_head_model::prerefine_target_sub_block(vgl_point_3d<double> const&  /*sub_block_pt*/, unsigned  /*pt_index*/) {
  return true;
}
bool boxm2_vecf_ocl_head_model::inverse_vector_field(vgl_point_3d<double> const&  /*target_pt*/, vgl_vector_3d<double>&  /*inv_vf*/)const {
  return true;
}
bool boxm2_vecf_ocl_head_model::apply_vector_field(cell_info const&  /*target_cell*/, vgl_vector_3d<double> const&  /*inv_vf*/) {
  return true;
}
