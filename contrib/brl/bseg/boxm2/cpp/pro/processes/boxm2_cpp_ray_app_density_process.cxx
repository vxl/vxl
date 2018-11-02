// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_ray_app_density_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for probing along a ray in the scene.
//
// \author Vishal Jain
// \date June 3, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_ray_probe_functor.h>
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_ray_app_density_process_globals
{
    constexpr unsigned n_inputs_ = 7;
    constexpr unsigned n_outputs_ = 1;
    std::size_t lthreads[2]={8,8};
}

bool boxm2_cpp_ray_app_density_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_cpp_ray_app_density_process_globals;

    //process takes 1 input
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "boxm2_scene_sptr";
    input_types_[1] = "boxm2_cache_sptr";
    input_types_[2] = "vpgl_camera_double_sptr";
    input_types_[3] = "unsigned";
    input_types_[4] = "unsigned";
    input_types_[5] = "float";
    input_types_[6] = "vcl_string";// if identifier string is empty, then only one appearance model


    // process has 1 output:
    // output[0]: scene sptr
    std::vector<std::string>  output_types_(n_outputs_);
    output_types_[0] = "bbas_1d_array_float_sptr"; //seg_len

    bool good = pro.set_input_types(input_types_) &&
        pro.set_output_types(output_types_);
    // in case the 6th input is not set
    brdb_value_sptr idx = new brdb_value_t<std::string>("");
    pro.set_input(5, idx);
    return good;
}

bool boxm2_cpp_ray_app_density_process(bprb_func_process& pro)
{
    using namespace boxm2_cpp_ray_app_density_process_globals;

    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }
    //get the inputs
    unsigned i = 0;
    boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
    vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
    auto pi=pro.get_input<unsigned>(i++);
    auto pj=pro.get_input<unsigned>(i++);
    auto intensity=pro.get_input<float>(i++);
    std::string identifier = pro.get_input<std::string>(i);

    bool foundDataType = false;
    std::string data_type;
    std::vector<std::string> apps = scene->appearances();
    for (const auto & app : apps) {
        if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
        {
            data_type = app;
            foundDataType = true;
        }
        else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
        {
            data_type = app;
            foundDataType = true;
        }
    }
    if (!foundDataType) {
        std::cout<<"BOXM2_CPP_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
        return false;
    }

    if (identifier.size() > 0) {
        data_type += "_" + identifier;
    }

    std::vector<boxm2_block_id> vis_order=scene->get_vis_blocks((vpgl_generic_camera<double>*)(cam.ptr()));
    std::vector<boxm2_block_id>::iterator id;

    std::vector<float> app_density;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        std::cout<<"Block Id "<<(*id)<<std::endl;
        boxm2_block *     blk = cache->get_block(scene,*id);
        boxm2_data_base *  mog = cache->get_data_base(scene,*id,data_type);
        std::vector<boxm2_data_base*> datas;
        datas.push_back(mog);

        auto *scene_info_wrapper=new boxm2_scene_info_wrapper();
        scene_info_wrapper->info=scene->get_blk_metadata(*id);
        boxm2_ray_app_density_functor ray_app_density_functor;
        ray_app_density_functor.init_data(datas,app_density,intensity);

        cast_ray_per_block<boxm2_ray_app_density_functor>(ray_app_density_functor,scene_info_wrapper->info,blk,cam,pi+1,pj+1,pi,pj);
    }

    bbas_1d_array_float_sptr app_density_array  =new bbas_1d_array_float(app_density.size());

    for (unsigned i=0;i<app_density.size();i++)
    {
        app_density_array->data_array[i]=app_density[i];
    }
    // store scene smaprt pointer
    pro.set_output_val<bbas_1d_array_float_sptr>(0, app_density_array);
    return true;
}
