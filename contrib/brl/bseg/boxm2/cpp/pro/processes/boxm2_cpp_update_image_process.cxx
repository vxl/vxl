// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_update_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <vcl_fstream.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_transform.h>
#include <vil/vil_math.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/cpp/algo/boxm2_update_image_functor.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_update_image_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}

bool boxm2_cpp_update_image_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_update_image_process_globals;

  //process takes 4 inputs
  // 0) scene
  // 1) cache
  // 2) camera
  // 3) image
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  input_types_[3] = "vil_image_view_base_sptr";
  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_update_image_process(bprb_func_process& pro)
{
    using namespace boxm2_cpp_update_image_process_globals;

    if ( pro.n_inputs() < n_inputs_ ){
        vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
        return false;
    }
    //get the inputs
    unsigned i = 0;
    boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
    vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
    vil_image_view_base_sptr in_img=pro.get_input<vil_image_view_base_sptr>(i++);
    vil_image_view_base_sptr float_image=boxm2_util::prepare_input_image(in_img);
    if (vil_image_view<float> * input_image=dynamic_cast<vil_image_view<float> * > (float_image.ptr()))
    {
        bool foundDataType = false;
        bool foundNumObsType = false;

        vcl_string data_type;
        vcl_string num_obs_type;
        vcl_vector<vcl_string> apps = scene->appearances();
        for (unsigned int i=0; i<apps.size(); ++i) {
            if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
            {
                data_type = apps[i];
                foundDataType = true;
            }
            else if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
            {
                data_type = apps[i];
                foundDataType = true;
            }
            else if ( apps[i] == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() )
            {
                num_obs_type = apps[i];
                foundNumObsType = true;
            }
        }
        if (!foundDataType) {
            vcl_cout<<"BOXM2_OCL_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
            return false;
        }
        vcl_vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
        if (vis_order.empty())
        {
            vcl_cout<<" None of the blocks are visible from this viewpoint"<<vcl_endl;
            return true;
        }

        unsigned int num_passes=3;
        bool success=true;
        boxm2_update_pass0_functor pass0;
        boxm2_update_pass1_functor pass1;
        boxm2_update_pass2_functor pass2;

        boxm2_update_data_functor update_data;
        vil_image_view<float> pre_img(input_image->ni(),input_image->nj());
        vil_image_view<float> vis_img(input_image->ni(),input_image->nj());
        vil_image_view<float> proc_norm_img(input_image->ni(),input_image->nj());

        proc_norm_img.fill(0.0f);

        for (unsigned int pass_no=0;pass_no<num_passes;pass_no++)
        {
            vcl_vector<boxm2_block_id>::iterator id;
            pre_img.fill(0.0f);
            vis_img.fill(1.0f);

            vcl_cout<<"Pass "<<pass_no<<' ';
            for (id = vis_order.begin(); id != vis_order.end(); ++id)
            {
                vcl_cout<<"Block id "<<(*id)<<' ';
                boxm2_block *     blk   = cache->get_block(*id);
                boxm2_data_base *  alph = cache->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
                boxm2_data_base *  mog  = cache->get_data_base(*id,data_type);
                boxm2_data_base *  nobs  = cache->get_data_base(*id,num_obs_type);
                int alphaTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
                int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
                boxm2_data_base *  aux  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_AUX>::prefix(),alph->buffer_length()/alphaTypeSize*auxTypeSize);

                vcl_vector<boxm2_data_base*> datas;
                datas.push_back(aux);
                datas.push_back(alph);
                datas.push_back(mog);
                boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
                scene_info_wrapper->info=scene->get_blk_metadata(*id);
                //pass 0
                if (pass_no==0)
                {
                    pass0.init_data(datas,input_image);
                    success=success && cast_ray_per_block<boxm2_update_pass0_functor>(pass0,
                        scene_info_wrapper->info,
                        blk,
                        cam,
                        input_image->ni(),
                        input_image->nj());
                }
                //pass 1
                else if (pass_no==1)
                {
                    pass1.init_data(datas,&pre_img,&vis_img);
                    success=success && 
                            cast_ray_per_block<boxm2_update_pass1_functor>(pass1,
                                                                           scene_info_wrapper->info,
                                                                           blk,
                                                                           cam,
                                                                           input_image->ni(),
                                                                           input_image->nj());
                }
                else if (pass_no==2)
                {
                    pass2.init_data(datas,&pre_img,&vis_img, & proc_norm_img);
                    success=success && cast_ray_per_block<boxm2_update_pass2_functor>(pass2,
                                                                                      scene_info_wrapper->info,
                                                                                      blk,
                                                                                      cam,
                                                                                      input_image->ni(),
                                                                                      input_image->nj());
                }
            }
            if (pass_no==1)
                vil_math_image_sum<float,float,float>(pre_img,vis_img,proc_norm_img);
            vcl_cout<<vcl_endl;
        }
        vcl_vector<boxm2_block_id>::iterator id;
        for (id = vis_order.begin(); id != vis_order.end(); ++id)
        {
            boxm2_block     *  blk   = cache->get_block(*id);
            boxm2_data_base *  alph  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
            boxm2_data_base *  mog   = cache->get_data_base(*id,data_type);
            boxm2_data_base *  nobs  = cache->get_data_base(*id,num_obs_type);
            int alphaTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
            int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
            boxm2_data_base *  aux  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_AUX>::prefix(),alph->buffer_length()/alphaTypeSize*auxTypeSize);
            vcl_vector<boxm2_data_base*> datas;
            datas.push_back(aux);
            datas.push_back(alph);
            datas.push_back(mog);
            datas.push_back(nobs);
            boxm2_update_data_functor data_functor;
            data_functor.init_data(datas, float(blk->sub_block_dim().x()), blk->max_level());
            int data_buff_length = (int) (alph->buffer_length()/alphaTypeSize);
            boxm2_data_serial_iterator<boxm2_update_data_functor>(data_buff_length,data_functor);
        }
        return true;
    }
    return false;
}
