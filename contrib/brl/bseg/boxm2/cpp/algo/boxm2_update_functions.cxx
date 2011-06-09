#include "boxm2_update_functions.h"
#include <boxm2/cpp/algo/boxm2_cone_update_image_functor.h>
#include "boxm2_cast_cone_ray_function.h"
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>
#include <boxm2/cpp/algo/boxm2_update_image_functor.h>
#include <vil/vil_math.h>
#include <vul/vul_timer.h>

bool boxm2_update_cone_image(boxm2_scene_sptr & scene,
                             vcl_string data_type,
                             vcl_string num_obs_type,
                             vpgl_camera_double_sptr cam ,
                             vil_image_view<float> * input_image,
                             unsigned int roi_ni,
                             unsigned int roi_nj,
                             unsigned int roi_ni0,
                             unsigned int roi_nj0)
{
    boxm2_cache_sptr cache=boxm2_cache::instance();
    vcl_vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_perspective_camera<double>*>(cam.ptr()));
    if (vis_order.empty())
    {
        vcl_cout<<" None of the blocks are visible from this viewpoint"<<vcl_endl;
        return true;
    }

    unsigned int num_passes=2;

    vil_image_view<float> pre_img(input_image->ni(),input_image->nj());
    vil_image_view<float> vis_img(input_image->ni(),input_image->nj());
    vil_image_view<float> proc_norm_img(input_image->ni(),input_image->nj());
    proc_norm_img.fill(0.0);

    bool success = true;
    for (unsigned int pass_no=0;pass_no<num_passes;++pass_no)
    {
        vcl_vector<boxm2_block_id>::iterator id;
        pre_img.fill(0.0f);
        vis_img.fill(1.0f);

        vcl_cout<<"Pass "<<pass_no<<' ';
        for (id = vis_order.begin(); id != vis_order.end(); ++id)
        {
            vcl_cout<<"Block id "<<(*id)<<' ';
            boxm2_block     *  blk   = cache->get_block(*id);
            boxm2_data_base *  alph = cache->get_data_base(*id,boxm2_data_traits<BOXM2_GAMMA>::prefix(),0,false);
            boxm2_data_base *  mog  = cache->get_data_base(*id,data_type,0,false);
            boxm2_data_base *  nobs = cache->get_data_base(*id,num_obs_type,0,false);
            int alphaTypeSize       = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAMMA>::prefix());
            int auxTypeSize         = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
            boxm2_data_base *  aux  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_AUX>::prefix(),alph->buffer_length()/alphaTypeSize*auxTypeSize);

            vcl_vector<boxm2_data_base*> datas;
            datas.push_back(aux);
            datas.push_back(alph);
            datas.push_back(mog);
            datas.push_back(nobs);
            boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
            scene_info_wrapper->info=scene->get_blk_metadata(*id);
#if 0
            // pass 0
            if (pass_no==0)
            {
                boxm2_cone_update_pass0_functor pass0;
                pass0.init_data(datas,input_image);
                success=success && cast_cone_ray_per_block<boxm2_cone_update_pass0_functor>
                                       (pass0,
                                        scene_info_wrapper->info,
                                        blk,
                                        cam,
                                        input_image->ni(),
                                        input_image->nj());
            }
#endif
            // pass 1
            if (pass_no==0)
            {
                boxm2_cone_update_pass1_functor pass1;
                pass1.init_data(datas,&pre_img,&vis_img,input_image);
                success=success && cast_cone_ray_per_block<boxm2_cone_update_pass1_functor>
                                       (pass1,
                                        scene_info_wrapper->info,
                                        blk,
                                        cam,
                                        input_image->ni(),
                                        input_image->nj());
            }
            // pass 2
            else if (pass_no==1)
            {
                boxm2_cone_update_pass2_functor pass2;
                pass2.init_data(datas,&pre_img,&vis_img, & proc_norm_img, input_image);
                success=success && cast_cone_ray_per_block<boxm2_cone_update_pass2_functor>
                                       (pass2,
                                        scene_info_wrapper->info,
                                        blk,
                                        cam,
                                        input_image->ni(),
                                        input_image->nj());
            }
        }
        if (pass_no==0)
            vil_math_image_sum<float,float,float>(pre_img,vis_img,proc_norm_img);
    }

    vcl_vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        boxm2_block     *  blk   = cache->get_block(*id);
        boxm2_data_base *  alph  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_GAMMA>::prefix(),0,false);
        boxm2_data_base *  mog   = cache->get_data_base(*id,data_type,0,false);
        boxm2_data_base *  nobs  = cache->get_data_base(*id,num_obs_type,0,false);
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAMMA>::prefix());
        int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
        boxm2_data_base *  aux  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_AUX>::prefix(),alph->buffer_length()/alphaTypeSize*auxTypeSize);
        vcl_vector<boxm2_data_base*> datas;
        datas.push_back(aux);
        datas.push_back(alph);
        datas.push_back(mog);
        datas.push_back(nobs);
        boxm2_cone_update_data_functor data_functor;
        data_functor.init_data(datas, float(blk->sub_block_dim().x()), blk->max_level());
        int data_buff_length = (int) (alph->buffer_length()/alphaTypeSize);
        boxm2_data_serial_iterator<boxm2_cone_update_data_functor>(data_buff_length,data_functor);
        cache->remove_data_base(*id,boxm2_data_traits<BOXM2_AUX>::prefix());
    }
    return true;
}

bool boxm2_update_image(boxm2_scene_sptr & scene,
                             vcl_string data_type,int appTypeSize,
                             vcl_string num_obs_type,
                             vpgl_camera_double_sptr cam ,
                             vil_image_view<float> * input_image,
                             unsigned int roi_ni,
                             unsigned int roi_nj,
                             unsigned int roi_ni0,
                             unsigned int roi_nj0)
{
    boxm2_cache_sptr cache=boxm2_cache::instance();
    vcl_vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
    if (vis_order.empty())
    {
        vcl_cout<<" None of the blocks are visible from this viewpoint"<<vcl_endl;
        return true;
    }

    unsigned int num_passes=3;

    vil_image_view<float> pre_img(input_image->ni(),input_image->nj());
    vil_image_view<float> vis_img(input_image->ni(),input_image->nj());
    vil_image_view<float> proc_norm_img(input_image->ni(),input_image->nj());
    proc_norm_img.fill(0.0);
    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());	 
    int nobsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS>::prefix());	 

    bool success = true;
    for (unsigned int pass_no=0;pass_no<num_passes;++pass_no)
    {
        vcl_vector<boxm2_block_id>::iterator id;
        pre_img.fill(0.0f);
        vis_img.fill(1.0f);

        vcl_cout<<"Pass "<<pass_no<<' ';
        for (id = vis_order.begin(); id != vis_order.end(); ++id)
        {
            vcl_cout<<"Block id "<<(*id)<<' ';
            boxm2_block *     blk   = cache->get_block(*id);
            boxm2_data_base *  alph = cache->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
            boxm2_data_base *  mog  = cache->get_data_base(*id,data_type,alph->buffer_length()/alphaTypeSize*appTypeSize,false);
            boxm2_data_base *  nobs  = cache->get_data_base(*id,num_obs_type,alph->buffer_length()/alphaTypeSize*nobsTypeSize,false);
            int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
            boxm2_data_base *  aux  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_AUX>::prefix(),alph->buffer_length()/alphaTypeSize*auxTypeSize);

            vcl_vector<boxm2_data_base*> datas;
            datas.push_back(aux);
            datas.push_back(alph);
            datas.push_back(mog);
            datas.push_back(nobs);
            boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
            scene_info_wrapper->info=scene->get_blk_metadata(*id);
            // pass 0
            if (pass_no==0)
            {
                boxm2_update_pass0_functor pass0;
                pass0.init_data(datas,input_image);
                success=success && cast_ray_per_block<boxm2_update_pass0_functor>
                                       (pass0,
                                        scene_info_wrapper->info,
                                        blk,
                                        cam,
                                        input_image->ni(),
                                        input_image->nj());
            }
            // pass 1
            else if (pass_no==1)
            {
                boxm2_update_pass1_functor pass1;
                pass1.init_data(datas,&pre_img,&vis_img);
                success=success &&
                    cast_ray_per_block<boxm2_update_pass1_functor>
                                       (pass1,
                                        scene_info_wrapper->info,
                                        blk,
                                        cam,
                                        input_image->ni(),
                                        input_image->nj());
            }
            // pass 2
            else if (pass_no==2)
            {
                boxm2_update_pass2_functor pass2;
                pass2.init_data(datas,&pre_img,&vis_img, & proc_norm_img);
                success=success && cast_ray_per_block<boxm2_update_pass2_functor>
                                       (pass2,
                                        scene_info_wrapper->info,
                                        blk,
                                        cam,
                                        input_image->ni(),
                                        input_image->nj());
            }
        }
        if (pass_no==1)
            vil_math_image_sum<float,float,float>(pre_img,vis_img,proc_norm_img);
#ifdef DEBUG
        for (unsigned i=0;i<vis_img.ni();i++)
        {
            for (unsigned j=0;j<vis_img.nj();j++)
                vcl_cout<<proc_norm_img(i,j)<<' ';

            vcl_cout<<vcl_endl;
        }
#endif
    }
    vcl_vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        boxm2_block     *  blk   = cache->get_block(*id);
        boxm2_data_base *  alph  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
        boxm2_data_base *  mog   = cache->get_data_base(*id,data_type,0,false);
        boxm2_data_base *  nobs  = cache->get_data_base(*id,num_obs_type,0,false);
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
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
        cache->remove_data_base(*id,boxm2_data_traits<BOXM2_AUX>::prefix());
    }
    return true;
}


