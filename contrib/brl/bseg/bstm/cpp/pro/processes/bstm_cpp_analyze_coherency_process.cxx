// This is brl/bseg/bstm/cpp/pro/processes/bstm_cpp_analyze_coherency_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for analyzing the temporal coherency of a region (bounding box) over a specified time interval.
//
// \author Ali Osman Ulusoy
// \date Mar 18, 2013

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/io/bstm_cache.h>
#include <bstm/io/bstm_lru_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_intersection.h>

#include <bstm/bstm_util.h>
#include <bstm/cpp/algo/bstm_analyze_coherency_function.h>

namespace bstm_cpp_analyze_coherency_process_globals
{
  constexpr unsigned n_inputs_ = 12;
  constexpr unsigned n_outputs_ = 0;
}

bool bstm_cpp_analyze_coherency_process_cons(bprb_func_process& pro)
{
  using namespace bstm_cpp_analyze_coherency_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);

  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "float"; //center x
  input_types_[3] = "float"; //center y
  input_types_[4] = "float"; //center z
  input_types_[5] = "float"; //len x
  input_types_[6] = "float"; //len y
  input_types_[7] = "float"; //len z
  input_types_[8] = "float"; //initial time
  input_types_[9] = "float"; //end time
  input_types_[10] = "float"; //p_threshold
  input_types_[11] = "vcl_string"; //output filename

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_cpp_analyze_coherency_process(bprb_func_process& pro)
{
  using namespace bstm_cpp_analyze_coherency_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache= pro.get_input<bstm_cache_sptr>(i++);
  auto center_x = pro.get_input<float>(i++);
  auto center_y = pro.get_input<float>(i++);
  auto center_z = pro.get_input<float>(i++);
  auto len_x = pro.get_input<float>(i++);
  auto len_y = pro.get_input<float>(i++);
  auto len_z = pro.get_input<float>(i++);
  auto init_time = pro.get_input<float>(i++);
  auto end_time = pro.get_input<float>(i++);
  auto p_threshold = pro.get_input<float>(i++);
  std::string output_filename = pro.get_input<std::string>(i++);

  //create output file
  std::ofstream output_file(output_filename.c_str());

  //create vgl box
  const vgl_point_3d<double> center(center_x,center_y,center_z);
  vgl_box_3d<double> box(center,len_x,len_y,len_z, vgl_box_3d<double>::centre);

  //iterate over each block/metadata to check if bbox intersects the input bbox
  std::map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  std::map<bstm_block_id, bstm_block_metadata> ::const_iterator bstm_iter = blocks.begin();
  for(; bstm_iter != blocks.end() ; bstm_iter++)
  {
    bstm_block_id bstm_id = bstm_iter->first;
    bstm_block_metadata bstm_metadata = bstm_iter->second;
    if(!vgl_intersection<double>( bstm_metadata.bbox(), box).is_empty() ) //if the two boxes intersect
    {
      double init_local_time, end_local_time;
      if(bstm_metadata.contains_t (init_time, init_local_time) && bstm_metadata.contains_t (end_time, end_local_time)) //if the block box contains the given times
      {
        std::cout << "Found intersecting bbox at block " << bstm_id << "..." << std::endl;

        bstm_block* blk = cache->get_block(bstm_metadata.id_);
        bstm_time_block* blk_t = cache->get_time_block(bstm_metadata.id_);
        bstm_data_base * alph = cache->get_data_base(bstm_metadata.id_, bstm_data_traits<BSTM_ALPHA>::prefix());
        bstm_data_base * app = cache->get_data_base(bstm_metadata.id_, bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix() );

        auto * alpha_data = (bstm_data_traits<BSTM_ALPHA>::datatype*) alph->data_buffer();
        auto * app_data = (bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype*) app->data_buffer();

        bstm_analyze_coherency_function coherency(blk, bstm_metadata, blk_t, app_data, alpha_data, init_local_time,end_local_time, box, p_threshold, output_file);
      }
    }
  }
  output_file.close();
  return true;

}
