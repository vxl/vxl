#include "boxm2_update_functions.h"
#include <boxm2/cpp/algo/boxm2_cone_update_image_functor.h>
#include "boxm2_cast_cone_ray_function.h"
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>
#include <boxm2/cpp/algo/boxm2_update_image_functor.h>
#include <boxm2/cpp/algo/boxm2_update_with_shadow_functor.h>
#include <boxm2/cpp/algo/boxm2_update_using_quality_functor.h>
#include <vil/vil_math.h>
#include <vil/vil_save.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bsta/bsta_gauss_sf1.h>

bool boxm2_update_cone_image(boxm2_scene_sptr & scene,
                             const std::string& data_type,
                             const std::string& num_obs_type,
                             const vpgl_camera_double_sptr& cam ,
                             vil_image_view<float> * input_image,
                             unsigned int  /*roi_ni*/,
                             unsigned int  /*roi_nj*/,
                             unsigned int  /*roi_ni0*/,
                             unsigned int  /*roi_nj0*/)
{
    boxm2_cache_sptr cache=boxm2_cache::instance();
    std::vector<boxm2_block_id> vis_order;
    if (auto* pcam = // assignment, not comparison
        dynamic_cast<vpgl_perspective_camera<double>* >(cam.ptr()))
    {
        vis_order=scene->get_vis_blocks(pcam);
    }
    else
    {
        vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
    }
    if (vis_order.empty())
    {
        std::cout<<"None of the blocks are visible from this viewpoint"<<std::endl;
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
        std::vector<boxm2_block_id>::iterator id;
        pre_img.fill(0.0f);
        vis_img.fill(1.0f);

        std::cout<<"Pass "<<pass_no<<' ';
        for (id = vis_order.begin(); id != vis_order.end(); ++id)
        {
            std::cout<<"Block id "<<(*id)<<' ';
            boxm2_block     *  blk   = cache->get_block(scene,*id);
            boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_GAMMA>::prefix(),0,false);
            boxm2_data_base *  mog  = cache->get_data_base(scene,*id,data_type,0,false);
            boxm2_data_base *  nobs = cache->get_data_base(scene,*id,num_obs_type,0,false);
            int alphaTypeSize       = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAMMA>::prefix());
            int auxTypeSize         = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
            // check for invalid parameters
            if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
            {
                std::cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
                return false;
            }

            boxm2_data_base *  aux  = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX>::prefix(),alph->buffer_length()/alphaTypeSize*auxTypeSize);

            std::vector<boxm2_data_base*> datas;
            datas.push_back(aux);
            datas.push_back(alph);
            datas.push_back(mog);
            datas.push_back(nobs);
            auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
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

    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        boxm2_block     *  blk   = cache->get_block(scene,*id);
        boxm2_data_base *  alph  = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_GAMMA>::prefix(),0,false);
        boxm2_data_base *  mog   = cache->get_data_base(scene,*id,data_type,0,false);
        boxm2_data_base *  nobs  = cache->get_data_base(scene,*id,num_obs_type,0,false);
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAMMA>::prefix());
        int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
        boxm2_data_base *  aux  = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX>::prefix(),alph->buffer_length()/alphaTypeSize*auxTypeSize);
        std::vector<boxm2_data_base*> datas;
        datas.push_back(aux);
        datas.push_back(alph);
        datas.push_back(mog);
        datas.push_back(nobs);
        boxm2_cone_update_data_functor data_functor;
        data_functor.init_data(datas, float(blk->sub_block_dim().x()), blk->max_level());
        int data_buff_length = (int) (alph->buffer_length()/alphaTypeSize);
        boxm2_data_serial_iterator<boxm2_cone_update_data_functor>(data_buff_length,data_functor);
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX>::prefix());
    }
    return true;
}

bool boxm2_update_image(boxm2_scene_sptr & scene,
                        const std::string& data_type,int appTypeSize,
                        const std::string& num_obs_type,
                        const vpgl_camera_double_sptr& cam ,
                        vil_image_view<float> * input_image,
                        unsigned int  /*roi_ni*/,
                        unsigned int  /*roi_nj*/,
                        unsigned int  /*roi_ni0*/,
                        unsigned int  /*roi_nj0*/)
{
    boxm2_cache_sptr cache=boxm2_cache::instance();
    std::vector<boxm2_block_id> vis_order;
    if (auto* pcam = // assignment, not comparison
        dynamic_cast<vpgl_perspective_camera<double>* >(cam.ptr()))
    {
        vis_order=scene->get_vis_blocks(pcam);
    }
    else
    {
        vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
    }
    if (vis_order.empty())
    {
        std::cout<<" None of the blocks are visible from this viewpoint"<<std::endl;
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
        std::vector<boxm2_block_id>::iterator id;
        pre_img.fill(0.0f);
        vis_img.fill(1.0f);

        std::cout<<"Pass "<<pass_no<<' ';
        for (id = vis_order.begin(); id != vis_order.end(); ++id)
        {
            std::cout<<"Block id "<<(*id)<<' ';
            boxm2_block *     blk   = cache->get_block(scene,*id);
            boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
            boxm2_data_base *  mog  = cache->get_data_base(scene,*id,data_type,alph->buffer_length()/alphaTypeSize*appTypeSize,false);
            boxm2_data_base *  nobs  = cache->get_data_base(scene,*id,num_obs_type,alph->buffer_length()/alphaTypeSize*nobsTypeSize,false);
            int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
            boxm2_data_base *  aux  = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX>::prefix(),alph->buffer_length()/alphaTypeSize*auxTypeSize);

            std::vector<boxm2_data_base*> datas;
            datas.push_back(aux);
            datas.push_back(alph);
            datas.push_back(mog);
            datas.push_back(nobs);
            auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
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
              if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != std::string::npos)
              {
                boxm2_update_pass1_functor<BOXM2_GAUSS_GREY> pass1;
                pass1.init_data(datas,&pre_img,&vis_img);
                success=success&&cast_ray_per_block<boxm2_update_pass1_functor<BOXM2_GAUSS_GREY> >
                  (pass1,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
              }
              else if (data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != std::string::npos)
              {
                boxm2_update_pass1_functor<BOXM2_MOG3_GREY> pass1;
                pass1.init_data(datas,&pre_img,&vis_img);
                success=success&&cast_ray_per_block<boxm2_update_pass1_functor<BOXM2_MOG3_GREY> >
                  (pass1,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
              }
            }
            // pass 2
            else if (pass_no==2)
            {
              if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != std::string::npos)
              {
                boxm2_update_pass2_functor<BOXM2_GAUSS_GREY> pass2;
                pass2.init_data(datas,&pre_img,&vis_img, & proc_norm_img);
                success=success&&cast_ray_per_block<boxm2_update_pass2_functor<BOXM2_GAUSS_GREY> >
                  (pass2,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
              }
              else if (data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != std::string::npos)
              {
                boxm2_update_pass2_functor<BOXM2_MOG3_GREY> pass2;
                pass2.init_data(datas,&pre_img,&vis_img, & proc_norm_img);
                success=success&&cast_ray_per_block<boxm2_update_pass2_functor<BOXM2_MOG3_GREY> >
                  (pass2,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
              }
            }
        }
        if (pass_no==1)
            vil_math_image_sum<float,float,float>(pre_img,vis_img,proc_norm_img);
#ifdef DEBUG
        for (unsigned i=0;i<vis_img.ni();i++)
        {
            for (unsigned j=0;j<vis_img.nj();j++)
                std::cout<<proc_norm_img(i,j)<<' ';

            std::cout<<std::endl;
        }
#endif
    }
    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        boxm2_block     *  blk   = cache->get_block(scene,*id);
        boxm2_data_base *  alph  = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
        boxm2_data_base *  mog   = cache->get_data_base(scene,*id,data_type,0,false);
        boxm2_data_base *  nobs  = cache->get_data_base(scene,*id,num_obs_type,0,false);
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
        boxm2_data_base *  aux  = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX>::prefix(),alph->buffer_length()/alphaTypeSize*auxTypeSize);
        std::vector<boxm2_data_base*> datas;
        datas.push_back(aux);
        datas.push_back(alph);
        datas.push_back(mog);
        datas.push_back(nobs);
        int data_buff_length = (int) (alph->buffer_length()/alphaTypeSize);
        if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != std::string::npos)
        {
          boxm2_update_data_functor<BOXM2_GAUSS_GREY> data_functor;
          data_functor.init_data(datas, float(blk->sub_block_dim().x()), blk->max_level());
          boxm2_data_serial_iterator<boxm2_update_data_functor<BOXM2_GAUSS_GREY> >(data_buff_length,data_functor);
        }
        else if (data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != std::string::npos)
        {
          boxm2_update_data_functor<BOXM2_MOG3_GREY> data_functor;
          data_functor.init_data(datas, float(blk->sub_block_dim().x()), blk->max_level());
          boxm2_data_serial_iterator<boxm2_update_data_functor<BOXM2_MOG3_GREY> >(data_buff_length,data_functor);
        }

        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX>::prefix());
    }
    return true;
}


bool boxm2_update_with_shadow(boxm2_scene_sptr & scene,
                              const std::string& data_type,int appTypeSize,
                              const std::string& num_obs_type,
                              const vpgl_camera_double_sptr& cam ,
                              float shadow_prior,
                              float shadow_sigma,
                              vil_image_view<float>  * input_image,
                              unsigned int  /*roi_ni*/,
                              unsigned int  /*roi_nj*/,
                              unsigned int  /*roi_ni0*/,
                              unsigned int  /*roi_nj0*/)
{
    boxm2_cache_sptr cache=boxm2_cache::instance();
    std::vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
    if (vis_order.empty())
    {
        std::cout<<" None of the blocks are visible from this viewpoint"<<std::endl;
        return true;
    }

    // compute alternate appearance probability for each pixel in the image
    vil_image_view<float> alt_prob_img(input_image->ni(), input_image->nj());
    alt_prob_img.fill(0.0f);
    if (shadow_prior > 0.0f) {
      bsta_gauss_sf1 single_mode(0.0f, shadow_sigma*shadow_sigma);
      for (unsigned i = 0; i < input_image->ni(); i++) {
        for (unsigned j = 0; j < input_image->nj(); j++) {
          alt_prob_img(i,j) = 2*single_mode.prob_density((*input_image)(i,j))*shadow_prior;
        }
      }
      vil_save(alt_prob_img, "shadow_density_img.tiff");
    }
    float model_prior = 1.0f - shadow_prior;
    std::cout << "In boxm2_update_with_shadow: model prior is: " << model_prior << std::endl;

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
        std::vector<boxm2_block_id>::iterator id;
        pre_img.fill(0.0f);
        vis_img.fill(1.0f);

        std::cout<<"Pass "<<pass_no<<' ';
        for (id = vis_order.begin(); id != vis_order.end(); ++id)
        {
            std::cout<<"Block id "<<(*id)<<' ';
            boxm2_block *     blk   = cache->get_block(scene,*id);
            boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
            boxm2_data_base *  mog  = cache->get_data_base(scene,*id,data_type,alph->buffer_length()/alphaTypeSize*appTypeSize,false);
            boxm2_data_base *  nobs  = cache->get_data_base(scene,*id,num_obs_type,alph->buffer_length()/alphaTypeSize*nobsTypeSize,false);
            int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
            boxm2_data_base *  aux  = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX>::prefix(),alph->buffer_length()/alphaTypeSize*auxTypeSize);

            std::vector<boxm2_data_base*> datas;
            datas.push_back(aux);
            datas.push_back(alph);
            datas.push_back(mog);
            datas.push_back(nobs);
            auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
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
              if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != std::string::npos)
              {
                boxm2_update_pass1_functor<BOXM2_GAUSS_GREY> pass1;
                pass1.init_data(datas,&pre_img,&vis_img);
                success=success&&cast_ray_per_block<boxm2_update_pass1_functor<BOXM2_GAUSS_GREY> >
                  (pass1,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
              }
              else if (data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != std::string::npos)
              {
                boxm2_update_pass1_functor<BOXM2_MOG3_GREY> pass1;
                pass1.init_data(datas,&pre_img,&vis_img);
                success=success&&cast_ray_per_block<boxm2_update_pass1_functor<BOXM2_MOG3_GREY> >
                  (pass1,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
              }
            }
            // pass 2
            else if (pass_no==2)
            {
              if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != std::string::npos)
              {
                boxm2_update_with_shadow_pass2_functor<BOXM2_GAUSS_GREY> pass2;
                pass2.init_data(datas,&pre_img,&vis_img, &proc_norm_img, &alt_prob_img, model_prior);
                success=success&&cast_ray_per_block<boxm2_update_with_shadow_pass2_functor<BOXM2_GAUSS_GREY> >
                  (pass2,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
              }
              else if (data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != std::string::npos)
              {
                boxm2_update_with_shadow_pass2_functor<BOXM2_MOG3_GREY> pass2;
                pass2.init_data(datas,&pre_img,&vis_img, &proc_norm_img, &alt_prob_img, model_prior);
                success=success&&cast_ray_per_block<boxm2_update_with_shadow_pass2_functor<BOXM2_MOG3_GREY> >
                  (pass2,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
              }
            }
        }
        if (pass_no==1)
            vil_math_image_sum<float,float,float>(pre_img,vis_img,proc_norm_img);
#ifdef DEBUG
        for (unsigned i=0;i<vis_img.ni();i++)
        {
            for (unsigned j=0;j<vis_img.nj();j++)
                std::cout<<proc_norm_img(i,j)<<' ';

            std::cout<<std::endl;
        }
#endif
    }
    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        boxm2_block     *  blk   = cache->get_block(scene,*id);
        boxm2_data_base *  alph  = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
        boxm2_data_base *  mog   = cache->get_data_base(scene,*id,data_type,0,false);
        boxm2_data_base *  nobs  = cache->get_data_base(scene,*id,num_obs_type,0,false);
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
        boxm2_data_base *  aux  = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX>::prefix(),alph->buffer_length()/alphaTypeSize*auxTypeSize);
        std::vector<boxm2_data_base*> datas;
        datas.push_back(aux);
        datas.push_back(alph);
        datas.push_back(mog);
        datas.push_back(nobs);
        int data_buff_length = (int) (alph->buffer_length()/alphaTypeSize);
        if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != std::string::npos)
        {
          boxm2_update_with_shadow_functor<BOXM2_GAUSS_GREY> data_functor;
          data_functor.init_data(datas, shadow_sigma, float(blk->sub_block_dim().x()), blk->max_level());
          boxm2_data_serial_iterator<boxm2_update_with_shadow_functor<BOXM2_GAUSS_GREY> >(data_buff_length,data_functor);
        }
        else if (data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != std::string::npos)
        {
          boxm2_update_with_shadow_functor<BOXM2_MOG3_GREY> data_functor;
          data_functor.init_data(datas, shadow_sigma, float(blk->sub_block_dim().x()), blk->max_level());
          boxm2_data_serial_iterator<boxm2_update_with_shadow_functor<BOXM2_MOG3_GREY> >(data_buff_length,data_functor);
        }

        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX>::prefix());
    }
    return true;
}

bool boxm2_update_using_quality(boxm2_scene_sptr & scene,
                                const std::string& data_type,int appTypeSize,
                                const std::string& num_obs_type,
                                const vpgl_camera_double_sptr& cam ,
                                vil_image_view<float>  * input_image,
                                vil_image_view<float>& quality_img,
                                unsigned int  /*roi_ni*/,
                                unsigned int  /*roi_nj*/,
                                unsigned int  /*roi_ni0*/,
                                unsigned int  /*roi_nj0*/)
{
    boxm2_cache_sptr cache=boxm2_cache::instance();
    std::vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
    if (vis_order.empty())
    {
        std::cout<<" None of the blocks are visible from this viewpoint"<<std::endl;
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
        std::vector<boxm2_block_id>::iterator id;
        pre_img.fill(0.0f);
        vis_img.fill(1.0f);

        std::cout<<"Pass "<<pass_no<<' ';
        for (id = vis_order.begin(); id != vis_order.end(); ++id)
        {
            std::cout<<"Block id "<<(*id)<<' ';
            boxm2_block *     blk   = cache->get_block(scene,*id);
            boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
            boxm2_data_base *  mog  = cache->get_data_base(scene,*id,data_type,alph->buffer_length()/alphaTypeSize*appTypeSize,false);
            boxm2_data_base *  nobs  = cache->get_data_base(scene,*id,num_obs_type,alph->buffer_length()/alphaTypeSize*nobsTypeSize,false);
            int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
            boxm2_data_base *  aux  = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX>::prefix(),alph->buffer_length()/alphaTypeSize*auxTypeSize);
            std::vector<boxm2_data_base*> datas;
            datas.push_back(aux);
            datas.push_back(alph);
            datas.push_back(mog);
            datas.push_back(nobs);
            auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
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
              if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != std::string::npos)
              {
               boxm2_update_pass1_functor<BOXM2_GAUSS_GREY> pass1;
               pass1.init_data(datas,&pre_img,&vis_img);
               success=success&&cast_ray_per_block<boxm2_update_pass1_functor<BOXM2_GAUSS_GREY> >
                 (pass1,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
              }
              else if (data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != std::string::npos)
              {
                boxm2_update_pass1_functor<BOXM2_MOG3_GREY> pass1;
                pass1.init_data(datas,&pre_img,&vis_img);
                success=success&&cast_ray_per_block<boxm2_update_pass1_functor<BOXM2_MOG3_GREY> >
                 (pass1,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
              }
            }
            // pass 2
            else if (pass_no==2)
            {
              if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != std::string::npos)
              {
                boxm2_update_using_quality_pass2_functor<BOXM2_GAUSS_GREY> pass2;
                pass2.init_data(datas,&pre_img,&vis_img, &proc_norm_img, &quality_img);
                success=success&&cast_ray_per_block<boxm2_update_using_quality_pass2_functor<BOXM2_GAUSS_GREY> >
                 (pass2,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
              }
              else if (data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != std::string::npos)
              {
                boxm2_update_using_quality_pass2_functor<BOXM2_MOG3_GREY> pass2;
                pass2.init_data(datas,&pre_img,&vis_img, &proc_norm_img, &quality_img);
                success=success&&cast_ray_per_block<boxm2_update_using_quality_pass2_functor<BOXM2_MOG3_GREY> >
                 (pass2,scene_info_wrapper->info,blk,cam,input_image->ni(),input_image->nj());
              }
            }
        }
        if (pass_no==1)
            vil_math_image_sum<float,float,float>(pre_img,vis_img,proc_norm_img);
#ifdef DEBUG
        for (unsigned i=0;i<vis_img.ni();i++)
        {
            for (unsigned j=0;j<vis_img.nj();j++)
                std::cout<<proc_norm_img(i,j)<<' ';
            std::cout<<std::endl;
        }
#endif
    }
    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        boxm2_block     *  blk   = cache->get_block(scene,*id);
        boxm2_data_base *  alph  = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
        boxm2_data_base *  mog   = cache->get_data_base(scene,*id,data_type,0,false);
        boxm2_data_base *  nobs  = cache->get_data_base(scene,*id,num_obs_type,0,false);
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
        boxm2_data_base *  aux  = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX>::prefix(),alph->buffer_length()/alphaTypeSize*auxTypeSize);
        std::vector<boxm2_data_base*> datas;
        datas.push_back(aux);
        datas.push_back(alph);
        datas.push_back(mog);
        datas.push_back(nobs);
        int data_buff_length = (int) (alph->buffer_length()/alphaTypeSize);
        if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != std::string::npos)
        {
            boxm2_update_using_quality_functor<BOXM2_GAUSS_GREY> data_functor;
            data_functor.init_data(datas, float(blk->sub_block_dim().x()), blk->max_level());
            boxm2_data_serial_iterator<boxm2_update_using_quality_functor<BOXM2_GAUSS_GREY> >(data_buff_length,data_functor);
        }
        else if (data_type.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != std::string::npos)
        {
            boxm2_update_using_quality_functor<BOXM2_MOG3_GREY> data_functor;
            data_functor.init_data(datas, float(blk->sub_block_dim().x()), blk->max_level());
            boxm2_data_serial_iterator<boxm2_update_using_quality_functor<BOXM2_MOG3_GREY> >(data_buff_length,data_functor);
        }
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX>::prefix());
    }
    return true;
}
