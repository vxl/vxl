#include <boxm2/cpp/pro/boxm2_cpp_change_detection_uncertainity_process.h>

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_save.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/cpp/algo/boxm2_change_detection_functor.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <vil/vil_math.h>
//directory utility
#include <vcl_where_root_dir.h>


bool boxm2_cpp_change_detection_uncertainity_process::execute(vcl_vector<brdb_value_sptr>& input, vcl_vector<brdb_value_sptr>& output)
{
  vcl_cout<<"CPP update argcounts:"<<input.size()<<vcl_endl;

  //1. get the arguments blocks/camera/img etc from the input vector
  int i = 0;
  brdb_value_t<boxm2_scene_sptr>* brdb_scene      = static_cast<brdb_value_t<boxm2_scene_sptr>* >( input[i++].ptr() );
  boxm2_scene_sptr  scene                         = brdb_scene->value();
  //camera
  brdb_value_t<vpgl_camera_double_sptr>* brdb_cam = static_cast<brdb_value_t<vpgl_camera_double_sptr>* >( input[i++].ptr() );
  vpgl_camera_double_sptr cam = brdb_cam->value();

  //in image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_inimg = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[i++].ptr() );
  vil_image_view_base_sptr inimg = brdb_inimg->value();
  vil_image_view<float>* input_image = static_cast<vil_image_view<float>* >(inimg.ptr());
  //exp image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_expimg = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[i++].ptr() );
  vil_image_view_base_sptr expimg = brdb_expimg->value();
  vil_image_view<float>* image_ = static_cast<vil_image_view<float>* >(expimg.ptr());
  image_->fill(0.0f);

  vil_image_view<float>* vis_img_ = new vil_image_view<float>(image_->ni(),image_->nj());
  vis_img_->fill(1.0f);

  vcl_vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_perspective_camera<double>*>(cam.ptr()));
  if (vis_order.empty())
  {
    vcl_cout<<" None of the blocks are visible from this viewpoint"<<vcl_endl;
    return true;
  }

  boxm2_change_detection_with_uncertainity_functor cd_wu_functor(input_image->ni(),input_image->nj());

  vcl_vector<boxm2_block_id>::iterator id; 
  for(id = vis_order.begin(); id != vis_order.end(); ++id) 
  {

      boxm2_block *     blk  = this->cache_->get_block(*id);
      boxm2_data_base *  alph = this->cache_->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
      boxm2_data_base *  mog  = this->cache_->get_data_base(*id,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
      vcl_vector<boxm2_data_base*> datas;
      datas.push_back(alph);
      datas.push_back(mog);

      boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
      scene_info_wrapper->info=scene->get_blk_metadata(*id);
      scene_info_wrapper->info->tree_buffer_length = blk->tree_buff_length();
      scene_info_wrapper->info->data_buffer_length = 65536;
      scene_info_wrapper->info->num_buffer = blk->num_buffers();

      cd_wu_functor.set_data(datas,input_image,image_);
      cast_ray_per_block<boxm2_change_detection_with_uncertainity_functor>(cd_wu_functor,scene_info_wrapper->info,blk,cam,image_->ni(),image_->nj());

  }
  
  cd_wu_functor.finish();

  vcl_cout<<"Execution time: "<<" ms"<<vcl_endl;

  return true;
}


bool boxm2_cpp_change_detection_uncertainity_process::clean()
{
  if (image_) delete image_;
  image_ = 0;
  return true;
}

