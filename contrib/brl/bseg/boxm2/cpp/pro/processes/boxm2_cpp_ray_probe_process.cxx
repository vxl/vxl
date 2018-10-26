// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_ray_probe_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for probing along a ray in the scene.
//
// \author Vishal Jain
// \date June 3, 2011

#include <vcl_compiler.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_ray_probe_functor.h>
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>

#include <vsph/vsph_camera_bounds.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_ray_probe_process_globals
{
    constexpr unsigned n_inputs_ = 7;
    constexpr unsigned n_outputs_ = 7;
}

bool boxm2_cpp_ray_probe_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_cpp_ray_probe_process_globals;

    //process takes 7 inputs:
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "boxm2_scene_sptr";
    input_types_[1] = "boxm2_cache_sptr";
    input_types_[2] = "vpgl_camera_double_sptr";
    input_types_[3] = "unsigned";
    input_types_[4] = "unsigned";
    input_types_[5] = "vcl_string";// prefix (optional)
    input_types_[6] = "vcl_string";// identifier (optional)

    // process has 6 outputs:
    std::vector<std::string>  output_types_(n_outputs_);
    output_types_[0] = "bbas_1d_array_float_sptr"; //seg_len
    output_types_[1] = "bbas_1d_array_float_sptr"; //alpha
    output_types_[2] = "bbas_1d_array_float_sptr"; //vis
    output_types_[3] = "bbas_1d_array_float_sptr"; //depth
    output_types_[4] = "bbas_1d_array_float_sptr"; //res
    output_types_[5] = "bbas_1d_array_float_sptr"; //data type asked for
    output_types_[6] = "int"; //n elems

    bool good = pro.set_input_types(input_types_) &&
                pro.set_output_types(output_types_);
    // in case the 6th or 7th input are not set
    brdb_value_sptr idx = new brdb_value_t<std::string>("");
    pro.set_input(5, idx);
    pro.set_input(6, idx);
    return good;
}

bool boxm2_cpp_ray_probe_process(bprb_func_process& pro)
{
    using namespace boxm2_cpp_ray_probe_process_globals;

    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }
    //get the inputs
    unsigned k = 0;
    boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(k++);
    boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(k++);
    vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(k++);
    unsigned pi=pro.get_input<unsigned>(k++);
    unsigned pj=pro.get_input<unsigned>(k++);
    std::string prefix = pro.get_input<std::string>(k++);
    std::string identifier = pro.get_input<std::string>(k++);

    std::string data_type;
    std::vector<std::string> apps = scene->appearances();
    for (unsigned int i=0; i<apps.size(); ++i) {
        if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
            data_type = apps[i];
        else if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
            data_type = apps[i];
    }

    if (identifier.size() > 0) {
        data_type += "_" + identifier;
    }

    std::vector<boxm2_block_id> vis_order=scene->get_vis_blocks((vpgl_perspective_camera<double>*)(cam.ptr()));
    double cone_half_angle, solid_angle;vgl_ray_3d<double> ray_ij;
    vsph_camera_bounds::pixel_solid_angle(*(vpgl_perspective_camera<double>*)(cam.ptr()), pi, pj, ray_ij, cone_half_angle, solid_angle);

    std::vector<boxm2_block_id>::iterator id;

    std::vector<float> seg_lengths;
    std::vector<float> alphas;
    std::vector<float> abs_depth;
    std::vector<float> data_to_return;
    int nelems = 0; // initialise here, in case the "for" loop is empty
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        boxm2_block *     blk = cache->get_block(scene,*id);
        boxm2_data_base *  alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());

        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        int data_buffer_length = (int) (alph->buffer_length()/alphaTypeSize);


        std::vector<boxm2_data_base*> datas;
        datas.push_back(alph);
        if (prefix!="")
        {
            std::string name = prefix;
            int dataTypeSize = (int)boxm2_data_info::datasize(name);
            if (identifier!="")
                name+= ("_"+identifier);
            boxm2_data_base *  data_of_interest = cache->get_data_base(scene,*id,name,data_buffer_length*dataTypeSize);

            datas.push_back(data_of_interest);
        }
        boxm2_ray_probe_functor ray_probe_functor;
        ray_probe_functor.init_data(datas,seg_lengths,abs_depth,alphas,data_to_return, prefix, nelems);
        boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
        scene_info_wrapper->info=scene->get_blk_metadata(*id);

        cast_ray_per_block<boxm2_ray_probe_functor>(ray_probe_functor,scene_info_wrapper->info,blk,cam,pi+1,pj+1,pi,pj);
    }

    bbas_1d_array_float_sptr seg_array  =new bbas_1d_array_float(seg_lengths.size());
    bbas_1d_array_float_sptr vis_array  =new bbas_1d_array_float(seg_lengths.size());
    bbas_1d_array_float_sptr alpha_array=new bbas_1d_array_float(alphas.size());
    bbas_1d_array_float_sptr abs_depth_array=new bbas_1d_array_float(abs_depth.size());
    bbas_1d_array_float_sptr res_depth_array=new bbas_1d_array_float(abs_depth.size());
    bbas_1d_array_float_sptr data_to_return_array=new bbas_1d_array_float(data_to_return.size());
    float vis=1.0f;
    for (unsigned i=0; i<seg_lengths.size(); ++i)
    {
        seg_array->data_array[i]=seg_lengths[i];
        abs_depth_array->data_array[i]=abs_depth[i];

        alpha_array->data_array[i]=alphas[i];

        vis_array->data_array[i]=vis;
        vis*=std::exp(-seg_lengths[i]*alphas[i]);

        res_depth_array->data_array[i] = abs_depth[i]*std::tan(cone_half_angle)*2;
        for (int j=0 ; j<nelems; ++j)
            data_to_return_array->data_array[i*nelems+j] = data_to_return[i*nelems+j];

    }

    // store scene smaprt pointer
    pro.set_output_val<bbas_1d_array_float_sptr>(0, seg_array);
    pro.set_output_val<bbas_1d_array_float_sptr>(1, alpha_array);
    pro.set_output_val<bbas_1d_array_float_sptr>(2, vis_array);
    pro.set_output_val<bbas_1d_array_float_sptr>(3, abs_depth_array);
    pro.set_output_val<bbas_1d_array_float_sptr>(4, res_depth_array);
    pro.set_output_val<bbas_1d_array_float_sptr>(5, data_to_return_array);
    pro.set_output_val<int>(6, nelems);
    return true;
}
