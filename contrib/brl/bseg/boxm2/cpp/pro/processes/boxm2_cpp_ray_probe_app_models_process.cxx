// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_ray_probe_app_models_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for probing along a ray in the scene.
//
// \author Vishal Jain
// \date June 3, 2011

#include <vcl_fstream.h>
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

namespace boxm2_cpp_ray_probe_app_models_process_globals
{
    const unsigned n_inputs_ = 6;
    const unsigned n_outputs_ = 5;
}

bool boxm2_cpp_ray_probe_app_models_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_cpp_ray_probe_app_models_process_globals;

    //process takes 1 input
    vcl_vector<vcl_string> input_types_(n_inputs_);
    input_types_[0] = "boxm2_scene_sptr";
    input_types_[1] = "boxm2_cache_sptr";
    input_types_[2] = "vpgl_camera_double_sptr";
    input_types_[3] = "unsigned";
    input_types_[4] = "unsigned";
    input_types_[5] = "vcl_string";// if identifier string is empty, then only one appearance model

    // process has 1 output:
    // output[0]: scene sptr
    vcl_vector<vcl_string>  output_types_(n_outputs_);
    output_types_[0] = "bbas_1d_array_float_sptr"; //seg_len
    output_types_[1] = "bbas_1d_array_float_sptr"; //alpha
    output_types_[2] = "bbas_1d_array_float_sptr"; //alpha
    output_types_[3] = "bbas_1d_array_float_sptr"; //alpha
    output_types_[4] = "bbas_1d_array_float_sptr"; //alpha

    bool good = pro.set_input_types(input_types_) &&
        pro.set_output_types(output_types_);
    // in case the 6th input is not set
    brdb_value_sptr idx = new brdb_value_t<vcl_string>("");
    pro.set_input(5, idx);
    return good;
}

bool boxm2_cpp_ray_probe_app_models_process(bprb_func_process& pro)
{
    using namespace boxm2_cpp_ray_probe_app_models_process_globals;

    if ( pro.n_inputs() < n_inputs_ ) {
        vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
        return false;
    }
    //get the inputs
    unsigned i = 0;
    boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
    vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
    unsigned pi=pro.get_input<unsigned>(i++);
    unsigned pj=pro.get_input<unsigned>(i++);



    vcl_vector<boxm2_block_id> vis_order=scene->get_vis_blocks((vpgl_generic_camera<double>*)(cam.ptr()));
    vcl_vector<boxm2_block_id>::iterator id;

    vcl_vector<float> seg_lengths;
    vcl_vector<float> alphas;
    vcl_vector<float> abs_depth;
    vcl_vector<float> phongs_variances;
    vcl_vector<float> entropy_air;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        vcl_cout<<"Block Id "<<(*id)<<vcl_endl;
        boxm2_block *     blk  =  cache->get_block(*id);
        boxm2_data_base *  alph = cache->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
        boxm2_data_base *  phongs  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_FLOAT8>::prefix("phongs_model"));
        boxm2_data_base *  histo_air  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix("histo_air"));
        boxm2_data_base *  entropy_histo_air  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_AUX0>::prefix("entropy_histo_air"));

        vcl_vector<boxm2_data_base*> datas;
        datas.push_back(alph);
        datas.push_back(phongs);
        datas.push_back(histo_air);
        datas.push_back(entropy_histo_air);

        boxm2_ray_probe_brdfs_functor ray_probe_functor;
        ray_probe_functor.init_data(datas,seg_lengths,abs_depth,alphas,phongs_variances,entropy_air);

        boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
        scene_info_wrapper->info=scene->get_blk_metadata(*id);

        cast_ray_per_block<boxm2_ray_probe_brdfs_functor>(ray_probe_functor,scene_info_wrapper->info,blk,cam,pi+1,pj+1,pi,pj);
    }

    bbas_1d_array_float_sptr seg_array             =new bbas_1d_array_float(seg_lengths.size());
    bbas_1d_array_float_sptr abs_depth_array       =new bbas_1d_array_float(abs_depth.size());
    bbas_1d_array_float_sptr vis_array           =new bbas_1d_array_float(abs_depth.size());
    bbas_1d_array_float_sptr phongs_variances_array=new bbas_1d_array_float(abs_depth.size());
    bbas_1d_array_float_sptr entropy_histo_array   =new bbas_1d_array_float(abs_depth.size());

    float vis=1.0f;
    for (unsigned i=0;i<seg_lengths.size();i++)
    {
        seg_array->data_array[i]=seg_lengths[i];
        abs_depth_array->data_array[i]=abs_depth[i];
        //alpha_array->data_array[i]=alphas[i];
        vis*=vcl_exp(-seg_lengths[i]*alphas[i]);
        vis_array->data_array[i]=vis;
        phongs_variances_array->data_array[i]=phongs_variances[i];
        entropy_histo_array->data_array[i] = entropy_air[i];
    }

    // store scene smaprt pointer
    pro.set_output_val<bbas_1d_array_float_sptr>(0, seg_array);
    pro.set_output_val<bbas_1d_array_float_sptr>(1, abs_depth_array);
    pro.set_output_val<bbas_1d_array_float_sptr>(2, vis_array);
    pro.set_output_val<bbas_1d_array_float_sptr>(3, phongs_variances_array);
    pro.set_output_val<bbas_1d_array_float_sptr>(4, entropy_histo_array);
    return true;
}
