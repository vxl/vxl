// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_normals_to_id_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process to convert normals to preidentified ids
// 0- for horizontal surface or (0,0,1)
// 1- for vertical surface
//
//10 - for unknown normal
// \author Vishal Jain
// \date Jan 17, 2013

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_normals_to_id_functor.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>
//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_normals_to_id_process_globals
{
    constexpr unsigned n_inputs_ = 2;
    constexpr unsigned n_outputs_ = 0;
    std::size_t lthreads[2]={8,8};
}

bool boxm2_cpp_normals_to_id_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_cpp_normals_to_id_process_globals;

    //process takes 1 input
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "boxm2_scene_sptr";
    input_types_[1] = "boxm2_cache_sptr";

    // process has 1 output:
    // output[0]: scene sptr
    std::vector<std::string>  output_types_(n_outputs_);

    bool good = pro.set_input_types(input_types_) &&
        pro.set_output_types(output_types_);

    return good;
}

bool boxm2_cpp_normals_to_id_process(bprb_func_process& pro)
{
    using namespace boxm2_cpp_normals_to_id_process_globals;

    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }
    //get the inputs
    unsigned i = 0;
    boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
    std::vector<boxm2_block_id> blocks=scene->get_block_ids();
    boxm2_normals_to_id_functor functor;
    for (auto & block : blocks)
    {
        std::cout<<"Block Id "<<block<<std::endl;
        std::size_t alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        std::size_t pointTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
        std::size_t normalTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL>::prefix());
        std::size_t visTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_VIS_SCORE>::prefix());
        std::size_t labelshortSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix());


        boxm2_data_base * alpha =        cache->get_data_base(scene,block,boxm2_data_traits<BOXM2_ALPHA>::prefix());
        int data_buff_length = (int) (alpha->buffer_length()/alphaTypeSize);


        boxm2_data_base * points = cache->get_data_base(scene,block,boxm2_data_traits<BOXM2_POINT>::prefix(), data_buff_length * pointTypeSize);
        boxm2_data_base * normals = cache->get_data_base(scene,block,boxm2_data_traits<BOXM2_NORMAL>::prefix(), data_buff_length * normalTypeSize);
        boxm2_data_base * vis = cache->get_data_base(scene,block,boxm2_data_traits<BOXM2_VIS_SCORE>::prefix(), data_buff_length * visTypeSize);
        boxm2_data_base * normalids = cache->get_data_base(scene,block,boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix("orientation"), data_buff_length * labelshortSize,false);

        std::vector<boxm2_data_base*> datas;
        datas.push_back(alpha);
        datas.push_back(points);
        datas.push_back(normals);
        datas.push_back(vis);
        datas.push_back(normalids);


        functor.init_data(datas,0.1,0.1);//alpha,cubic_model_data);
        boxm2_data_serial_iterator<boxm2_normals_to_id_functor>(data_buff_length,functor);
    }
    cache->write_to_disk();

    return true;
}
