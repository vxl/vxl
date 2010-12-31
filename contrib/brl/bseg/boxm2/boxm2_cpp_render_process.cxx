#include <boxm2/boxm2_cpp_render_process.h>

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_save.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/boxm2_render_exp_image_functor.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>


bool boxm2_cpp_render_process::execute(vcl_vector<brdb_value_sptr>& input, vcl_vector<brdb_value_sptr>& output)
{
  vcl_cout<<"CPP RENDER argcounts:"<<input.size()<<vcl_endl;

  //1. get the arguments blocks/camera/img etc from the input vector
  int i = 0;
  brdb_value_t<boxm2_scene_info_wrapper_sptr>* info      = static_cast<brdb_value_t<boxm2_scene_info_wrapper_sptr>* >( input[i++].ptr() );
  boxm2_scene_info_wrapper_sptr  info_sptr                = info->value();
  brdb_value_t<boxm2_block_sptr>* blk                = static_cast<brdb_value_t<boxm2_block_sptr>* >( input[i++].ptr() );
  boxm2_block_sptr blk_sptr                          = blk->value();

  vcl_vector<boxm2_data_base_sptr> datas;
  brdb_value_t<boxm2_data_base_sptr>* alpha          = static_cast<brdb_value_t<boxm2_data_base_sptr>* >( input[i++].ptr() );
  boxm2_data_base_sptr alpha_sptr                    = alpha->value();
  brdb_value_t<boxm2_data_base_sptr>* mogs           = static_cast<brdb_value_t<boxm2_data_base_sptr>* >( input[i++].ptr() );
  boxm2_data_base_sptr mogs_sptr                     = mogs->value();

  datas.push_back(alpha_sptr);
  datas.push_back(mogs_sptr);

  //camera
  brdb_value_t<vpgl_camera_double_sptr>* brdb_cam = static_cast<brdb_value_t<vpgl_camera_double_sptr>* >( input[i++].ptr() );
  vpgl_camera_double_sptr cam = brdb_cam->value();

  //exp image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_expimg = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[i++].ptr() );
  vil_image_view_base_sptr expimg = brdb_expimg->value();
  vil_image_view<float>* image_ = static_cast<vil_image_view<float>* >(expimg.ptr());

  //vis image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_vis = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[i++].ptr() );
  vil_image_view_base_sptr visimg = brdb_vis->value();
  vil_image_view<float>* vis_img_ = static_cast<vil_image_view<float>* >(visimg.ptr());


  boxm2_render_exp_image(info_sptr->info,blk_sptr,datas,cam,image_,vis_img_,image_->ni(),image_->nj());



  vcl_cout<<"Execution time: "<<" ms"<<vcl_endl;

  return true;
}


bool boxm2_cpp_render_process::clean()
{
  if (image_) delete image_;
  if (vis_img_) delete vis_img_;
  image_ = 0;
  vis_img_ = 0;
  return true;
}

