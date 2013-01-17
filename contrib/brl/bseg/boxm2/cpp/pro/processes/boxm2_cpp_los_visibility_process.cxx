// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_los_visibility_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for computing visibility of p1 from p0 ( Line of Sight Computation).
//
// \author Vishal Jain
// \date  3, 2012

#include <vcl_fstream.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_vis_probe_functor.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_los_visibility_process_globals
{
    const unsigned n_inputs_  = 9;
    const unsigned n_outputs_ = 1;
}

bool boxm2_cpp_los_visibility_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_cpp_los_visibility_process_globals;

    //process takes 8 inputs:
    vcl_vector<vcl_string> input_types_(n_inputs_);
    input_types_[0] = "boxm2_scene_sptr";
    input_types_[1] = "boxm2_cache_sptr";
    input_types_[2] = "float";  //x0
    input_types_[3] = "float";  //y0
    input_types_[4] = "float";  //z0
    input_types_[5] = "float";  //x1
    input_types_[6] = "float";  //y1
    input_types_[7] = "float";  //z1
    input_types_[8] = "float";  //paramter to move away from the points.
    // process has 1 output:
    vcl_vector<vcl_string>  output_types_(n_outputs_);
    output_types_[0] = "float"; //seg_len
    brdb_value_sptr param  = new brdb_value_t<float>(5.0);
    pro.set_input(8, param);
    return pro.set_input_types(input_types_) &&
           pro.set_output_types(output_types_);
}

bool boxm2_cpp_los_visibility_process(bprb_func_process& pro)
{
    using namespace boxm2_cpp_los_visibility_process_globals;

    if ( pro.n_inputs() < n_inputs_ ) {
        vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
        return false;
    }
    //get the inputs
    unsigned k = 0;
    boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(k++);
    boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(k++);

    float x0 = pro.get_input<float>(k++);
    float y0 = pro.get_input<float>(k++);
    float z0 = pro.get_input<float>(k++);

    float x1 = pro.get_input<float>(k++);
    float y1 = pro.get_input<float>(k++);
    float z1 = pro.get_input<float>(k++);


    float t  = pro.get_input<float>(k++);
    
    vgl_point_3d<double> p0(x0,y0,z0);
    vgl_point_3d<double> p1(x1,y1,z1);
    vgl_vector_3d<double > dir = p1-p0;
    float length = dir.length();

    dir = normalize(dir);


    vgl_point_3d<double> p0_adjusted( p0+vcl_min(t,length/2)*dir);
    vgl_point_3d<double> p1_adjusted( p1-vcl_min(t,length/2)*dir);
    vgl_ray_3d<double> ray_01(p0_adjusted,p1_adjusted);

    vcl_vector<boxm2_block_id> vis_order=boxm2_util::blocks_along_a_ray(scene,p0_adjusted,p1_adjusted);
    vcl_vector<boxm2_block_id>::iterator id;
    float vis = 1.0f;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        vcl_cout<<(*id)<<vcl_endl;
        boxm2_block *      blk  =  cache->get_block(*id);
        boxm2_data_base *  alph = cache->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
        vcl_vector<boxm2_data_base*> datas;
        datas.push_back(alph);
        boxm2_vis_probe_functor vis_probe_functor;
        vis_probe_functor.init_data(datas, &vis);

        boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
        scene_info_wrapper->info=scene->get_blk_metadata(*id);
        boxm2_cast_ray_function<boxm2_vis_probe_functor>(ray_01,
                                                         scene_info_wrapper->info,
                                                         blk,0,0,vis_probe_functor,length-vcl_min(2*t,length));
    }

    vcl_cout<<"Visibilty is "<<vis<<vcl_endl;
    // store scene smaprt pointer
    pro.set_output_val<float>(0, vis);
    return true;
}
