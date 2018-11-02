// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_batch_update_opt2_phongs_processes.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  Processes to update the scene using a set of data blocks in a batch mode, migrated from the version in boxm to boxm2
//
// \author Ozge C. Ozcanli
// \date May 12, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/boxm2_util.h>

#include <boxm2/cpp/algo/boxm2_batch_opt2_phongs_functors.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>

#include <vil/vil_math.h>
#include <bsta/bsta_gauss_sf1.h>

//: create a norm img = pre_inf+vis_inf;
namespace boxm2_cpp_pre_infinity_opt2_phongs_process_globals
{
    constexpr unsigned n_inputs_ = 9;
    constexpr unsigned n_outputs_ = 1;
}

bool boxm2_cpp_pre_infinity_opt2_phongs_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_cpp_pre_infinity_opt2_phongs_process_globals;

    //process takes 7 inputs
    // 0) scene
    // 1) cache
    // 2) camera
    // 3) image
    // 4) image identifier
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "boxm2_scene_sptr";
    input_types_[1] = "boxm2_cache_sptr";
    input_types_[2] = "vpgl_camera_double_sptr";
    input_types_[3] = "unsigned";     // ni
    input_types_[4] = "unsigned";     // nj
    input_types_[5] = "vcl_string";   //viewing direection image identifier
    input_types_[6] = "vcl_string";  //seg_len and image intensity aux  identifier
    input_types_[7] = "float"; // Sun Elevation
    input_types_[8] = "float"; // Sun Azimuthal
    // process has 0 output:
    std::vector<std::string>  output_types_(n_outputs_);
    output_types_[0] = "vil_image_view_base_sptr";

    bool good = pro.set_input_types(input_types_) &&
        pro.set_output_types(output_types_);
    return good;
}

bool boxm2_cpp_pre_infinity_opt2_phongs_process(bprb_func_process& pro)
{
    using namespace boxm2_cpp_pre_infinity_opt2_phongs_process_globals;

    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
        return false;
    }
    //get the inputs
    unsigned i = 0;
    boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
    vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);

    auto ni = pro.get_input<unsigned>(i++);
    auto nj = pro.get_input<unsigned>(i++);

    std::string dir_identifier = pro.get_input<std::string>(i++);
    std::string img_identifier = pro.get_input<std::string>(i++);

    auto sun_elev=pro.get_input<float>(i++);
    auto sun_azim=pro.get_input<float>(i++);

    std::vector<boxm2_block_id> vis_order=scene->get_vis_blocks(reinterpret_cast<vpgl_generic_camera<double>*>(cam.ptr()));
    if (vis_order.empty())
    {
        std::cout<<" None of the blocks are visible from this viewpoint"<<std::endl;
        return true;
    }
    vil_image_view<float> pre_inf_img(ni,nj);
    vil_image_view<float> vis_inf_img(ni,nj);

    pre_inf_img.fill(0.0);
    vis_inf_img.fill(1.0);
    std::vector<boxm2_block_id>::iterator id;

    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        std::cout<<"Block id "<<(*id)<<' ';
        boxm2_block *     blk = cache->get_block(scene,*id);
        boxm2_data_base *  alph = cache->get_data_base(scene, *id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,true);
        boxm2_data_base *  float8_phongs = cache->get_data_base(scene, *id,boxm2_data_traits<BOXM2_FLOAT8>::prefix(),0,true);
        // call get_data_base method with num_bytes = 0 to read from disc
        boxm2_data_base *aux0 = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(img_identifier));
        boxm2_data_base *aux1 = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix(img_identifier));

        boxm2_data_base *aux0_view = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(dir_identifier));
        boxm2_data_base *aux1_view = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix(dir_identifier));
        boxm2_data_base *aux2_view = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX2>::prefix(dir_identifier));
        boxm2_data_base *aux3_view = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX3>::prefix(dir_identifier));

        std::vector<boxm2_data_base*> datas;
        datas.push_back(aux0);
        datas.push_back(aux1);
        datas.push_back(aux0_view);
        datas.push_back(aux1_view);
        datas.push_back(aux2_view);
        datas.push_back(aux3_view);
        datas.push_back(alph);
        datas.push_back(float8_phongs);
        auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
        scene_info_wrapper->info=scene->get_blk_metadata(*id);

        boxm2_batch_update_phongs_pass1_functor pass1;
        pass1.init_data(datas,&pre_inf_img,&vis_inf_img,sun_elev,sun_azim);
        cast_ray_per_block<boxm2_batch_update_phongs_pass1_functor>(pass1,
                                                                    scene_info_wrapper->info,
                                                                    blk,cam,ni,nj);

        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_FLOAT8>::prefix());
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(img_identifier));
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix(img_identifier));

        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(dir_identifier));
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix(dir_identifier));
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX2>::prefix(dir_identifier));
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX3>::prefix(dir_identifier));
    }
    // compute beta denominator
    auto *  norm_img= new vil_image_view<float>(ni,nj);
    // pre_inf + vis * 1 // assume PI = 1 for all the colors (uniform distribution)
    vil_math_image_sum<float,float,float>(pre_inf_img,vis_inf_img,*norm_img);

    pro.set_output_val<vil_image_view_base_sptr>(0,norm_img);
    pre_inf_img.fill(0.0);
    vis_inf_img.fill(1.0);

    int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
    int aux0TypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());


    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        std::cout<<"Block id "<<(*id)<<' ';
        boxm2_block *     blk = cache->get_block(scene,*id);
        boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,true);
        boxm2_data_base *  phongs_model_base = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_FLOAT8>::prefix(),0,false);
        // call get_data_base method with num_bytes = 0 to read from disc
        boxm2_data_base *aux0 = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(img_identifier));
        boxm2_data_base *aux1 = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix(img_identifier));
        boxm2_data_base *aux0_view = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(dir_identifier));
        boxm2_data_base *aux1_view = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix(dir_identifier));
        boxm2_data_base *aux2_view = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX2>::prefix(dir_identifier));
        boxm2_data_base *aux3_view = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX3>::prefix(dir_identifier));
        // generate aux in a writable mode
        boxm2_data_base *aux = cache->get_data_base(scene,*id, boxm2_data_traits<BOXM2_AUX>::prefix(img_identifier),aux0->buffer_length()/aux0TypeSize*auxTypeSize,false);

        std::vector<boxm2_data_base*> datas;
        datas.push_back(aux0);
        datas.push_back(aux1);
        datas.push_back(aux0_view);
        datas.push_back(aux1_view);
        datas.push_back(aux2_view);
        datas.push_back(aux3_view);
        datas.push_back(alph);
        datas.push_back(phongs_model_base);
        datas.push_back(aux);
        auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
        scene_info_wrapper->info=scene->get_blk_metadata(*id);

        boxm2_batch_update_opt2_phongs_pass2_functor pass2;
        pass2.init_data(datas,&pre_inf_img,&vis_inf_img,sun_elev,sun_azim, norm_img);
        cast_ray_per_block<boxm2_batch_update_opt2_phongs_pass2_functor>(pass2,
                                                                         scene_info_wrapper->info,
                                                                         blk,
                                                                         cam,
                                                                         ni,nj);

        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_FLOAT8>::prefix());
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(img_identifier));
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix(img_identifier));

        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(dir_identifier));
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix(dir_identifier));
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX2>::prefix(dir_identifier));
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX3>::prefix(dir_identifier));
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX>::prefix(img_identifier));
    }

    return true;
}

//: run batch update
namespace boxm2_cpp_batch_update_opt2_phongs_process_globals
{
    constexpr unsigned n_inputs_ = 6;
    constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_batch_update_opt2_phongs_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_cpp_batch_update_opt2_phongs_process_globals;

    //process takes 6 inputs
    // 0) scene
    // 1) cache
    // 2) stream cache 1
    // 3) stream cache 2
    // 4) sun elevation angle
    // 5) sun azimuthal angle
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "boxm2_scene_sptr";
    input_types_[1] = "boxm2_cache_sptr";
    input_types_[2] = "boxm2_stream_cache_sptr";
    input_types_[3] = "boxm2_stream_cache_sptr";
    input_types_[4] = "float";
    input_types_[5] = "float";
    // process has no outputs
    std::vector<std::string>  output_types_(n_outputs_);

    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: moved over from boxm
bool boxm2_cpp_batch_update_opt2_phongs_process(bprb_func_process& pro)
{
    using namespace boxm2_cpp_batch_update_opt2_phongs_process_globals;

    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }
    // get the inputs
    unsigned i = 0;
    boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
    boxm2_stream_cache_sptr str_cache1 = pro.get_input<boxm2_stream_cache_sptr>(i++);  //: this is for aux1,aux2,aux3.
    boxm2_stream_cache_sptr str_cache2 = pro.get_input<boxm2_stream_cache_sptr>(i++); // this is for aux
    auto  sun_elev = pro.get_input<float>(i++); // this is for aux
    auto  sun_azim = pro.get_input<float>(i++); // this is for aux

    // assumes that the data of each image has been created in the data models previously
    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    // iterate the scene block by block and write to output
    std::vector<boxm2_block_id> blk_ids = scene->get_block_ids();
    std::vector<boxm2_block_id>::iterator id;
    id = blk_ids.begin();
    for (id = blk_ids.begin(); id != blk_ids.end(); ++id) {
        // pass num_bytes = 0 to make sure disc is read if not already in memory
        boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
        boxm2_data_base *  phongs = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_FLOAT8>::prefix(),0,false);
        std::cout << "buffer length of alpha: " << alph->buffer_length() << '\n'
                 << "buffer length of phongs: " << phongs->buffer_length() << std::endl;
        boxm2_batch_update_opt2_phongs_functor data_functor;
        data_functor.init_data(alph, phongs, str_cache1,str_cache2,sun_elev,sun_azim);
        int data_buff_length = (int) (alph->buffer_length()/alphaTypeSize);
        boxm2_data_serial_iterator<boxm2_batch_update_opt2_phongs_functor>(data_buff_length,data_functor);

        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_FLOAT8>::prefix());
    }
    return true;
}


//: run batch update
namespace boxm2_cpp_batch_update_nonray_process_globals
{
    constexpr unsigned n_inputs_ = 2;
    constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_batch_update_nonray_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_cpp_batch_update_nonray_process_globals;

    //process takes 4 inputs
    // 0) scene
    // 1) cache
    // 2) stream cache 1
    // 3) stream cache 2
    // 4) sun elevation angle
    // 5) sun azimuthal angle
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "boxm2_scene_sptr";
    input_types_[1] = "boxm2_cache_sptr";
    // process has no output:
    std::vector<std::string>  output_types_(n_outputs_);

    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: moved over from boxm
bool boxm2_cpp_batch_update_nonray_process(bprb_func_process& pro)
{
    using namespace boxm2_cpp_batch_update_nonray_process_globals;

    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }
    // get the inputs
    unsigned i = 0;
    boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);

    // assumes that the data of each image has been created in the data models previously
    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    // check for invalid parameters
    //This should never happen, it will result in division by zero later
    if( alphaTypeSize == 0 ) {
        std::cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
        return false;
    }

    // iterate the scene block by block and write to output
    std::vector<boxm2_block_id> blk_ids = scene->get_block_ids();
    std::vector<boxm2_block_id>::iterator id;
    id = blk_ids.begin();
    for (id = blk_ids.begin(); id != blk_ids.end(); ++id) {
        // pass num_bytes = 0 to make sure disc is read if not already in memory
        boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
        boxm2_data_base *  phongs = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_FLOAT8>::prefix("phongs_model"),0,false);
        boxm2_data_base *  air = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix("entropy_histo_air"),0,false);
        boxm2_data_base *  uncertainty = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix("uncertainty"),0,false);
        std::cout << "buffer length of alpha: " << alph->buffer_length() << '\n'
                 << "buffer length of phongs: " << phongs->buffer_length() << std::endl;
        boxm2_batch_update_nonray_phongs_functor data_functor;
        data_functor.init_data(alph, phongs, air,uncertainty);
        int data_buff_length = (int) (alph->buffer_length()/alphaTypeSize);
        boxm2_data_serial_iterator<boxm2_batch_update_nonray_phongs_functor>(data_buff_length,data_functor);

        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_FLOAT8>::prefix("phongs_model"));
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix("entropy_histo_air"));
        cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix("uncertainty"));
    }
    return true;
}
