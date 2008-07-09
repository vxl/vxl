#include "bvxm_update_multiscale_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_camera.h>

#include <multiscale/bvxm_multi_scale_voxel_world.h>
#include <multiscale/bvxm_multiscale_util.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <vpgl/vpgl_rational_camera.h>

bvxm_update_multiscale_process::bvxm_update_multiscale_process()
{
  //process takes 6 inputs
  //input[0]: The observation image
  //input[1]: The camera of the observation
  //input[2]: The voxel world
  //input[3]: The apperance model type :this input must be either apm_mog_grey or apm_mog_rgb
  //          any other string will initialize the value for apm_mog_grey
  //input[4]: The bin index to be updated
  //input[5]  The image scale


  input_data_.resize(6,brdb_value_sptr(0));
  input_types_.resize(6);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "bvxm_multi_scale_voxel_world_sptr";
  input_types_[3] = "vcl_string";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  //input_types_[6] = "unsigned";


  //output vector has 2 outputs
  //output[0] : The updated probability map
  //output[1] : The mask of image pixels used in update
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1]= "vil_image_view_base_sptr";
}


bool bvxm_update_multiscale_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //get the inputs
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr img = input0->value();

  brdb_value_t<vpgl_camera_double_sptr>* input1 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[1].ptr());
  vpgl_camera_double_sptr camera = input1->value();

  brdb_value_t<bvxm_multi_scale_voxel_world_sptr>* input2 =
    static_cast<brdb_value_t<bvxm_multi_scale_voxel_world_sptr>* >(input_data_[2].ptr());
  bvxm_multi_scale_voxel_world_sptr world = input2->value();

  brdb_value_t<vcl_string>* input3 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[3].ptr());
  vcl_string voxel_type = input3->value();

  brdb_value_t<unsigned>* input4 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[4].ptr());
  unsigned bin_index = input4->value();

  brdb_value_t<unsigned>* input5 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[5].ptr());
  unsigned curr_scale = input5->value();

    //brdb_value_t<unsigned>* input6 =
    //static_cast<brdb_value_t<unsigned>* >(input_data_[6].ptr());
    unsigned max_scale = world->get_params()->max_scale();



  typedef bvxm_voxel_traits<APM_MOG_GREY>::obs_datatype obs_datatype;

  if (voxel_type == "apm_mog_rgb")
  typedef bvxm_voxel_traits<APM_MOG_RGB>::obs_datatype obs_datatype;

  vcl_vector<vil_image_view<float> > prob_map_vec;
  vcl_vector<vil_image_view<bool> >mask_vec;


  for (unsigned scale = curr_scale;scale < max_scale;scale++)
  {
     vil_image_view<float> prob_map(img->ni(),img->nj(),1);
      vil_image_view<bool> mask(img->ni(),img->nj(),1);
      
      if(scale!=curr_scale)
      {
          img=bvxm_multiscale_util::downsample_image_by_two(img);
          camera=bvxm_multiscale_util::downsample_camera( camera, scale);
          prob_map.set_size(img->ni(),img->nj());
          mask.set_size(img->ni(),img->nj());
      }
      bvxm_image_metadata observation(img,camera);
      //create metadata:

      //update

      bool result;

      if (voxel_type == "apm_mog_rgb")
          result = world->update<APM_MOG_RGB>(observation, prob_map, mask, bin_index,scale);
      else
          result = world->update<APM_MOG_GREY>(observation, prob_map, mask, bin_index,scale);

      vcl_cout<<"update done ";
      vcl_cout.flush();

      prob_map_vec.push_back(prob_map);
      mask_vec.push_back(mask);
      if (!result){
          vcl_cerr << "error bvxm_update_multiscale_process: failed to update observation\n";
          return false;
      }
  }

  //store output
  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(prob_map_vec[0]));
  output_data_[0] = output0;

    brdb_value_sptr output1 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<bool>(mask_vec[0]));
  output_data_[1] = output1;

  return true;
}


vpgl_camera_double_sptr bvxm_update_multiscale_process::downsample_camera_by_two(vpgl_camera_double_sptr camera)
{
    if ( vpgl_rational_camera<double>* rat_camera = dynamic_cast<vpgl_rational_camera<double>*> (camera.as_pointer()))
    {
        vpgl_rational_camera<double>* new_rat_camera=new vpgl_rational_camera<double>(*rat_camera);
        double u_s,v_s;
        rat_camera->image_scale(u_s,v_s);
        new_rat_camera->set_image_scale(u_s/2,v_s/2);

        double u_off,v_off;
        rat_camera->image_offset(u_off,v_off);
        new_rat_camera->set_image_offset(u_off/2,v_off/2);

        return new_rat_camera;
    }
    else
    {
        return NULL;
    }
}
