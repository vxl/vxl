// This is brl/bseg/bstm/pro/processes/bstm_cpp_export_point_cloud_process.cxx
#include <iostream>
#include <fstream>
#include <sstream>
#include <bprb/bprb_func_process.h>
//:
// \file


#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/bstm_scene.h>
#include <bstm/bstm_util.h>
#include <bstm/io/bstm_cache.h>
#include <bstm/bstm_data_traits.h>


namespace bstm_cpp_export_point_cloud_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

bool bstm_cpp_export_point_cloud_process_cons(bprb_func_process& pro)
{
  using namespace bstm_cpp_export_point_cloud_process_globals;

  //process takes 8 inputs (3 required ones), no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "vcl_string"; //filename
  input_types_[3] = "bool";  //output aux data?
  input_types_[4] = "float"; //time

  brdb_value_sptr output_prob = new brdb_value_t<bool>(false);
  pro.set_input(3, output_prob);


  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool bstm_cpp_export_point_cloud_process (bprb_func_process& pro)
{
  using namespace bstm_cpp_export_point_cloud_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene = pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache = pro.get_input<bstm_cache_sptr>(i++);
  std::string output_filename = pro.get_input<std::string>(i++);
  bool output_aux = pro.get_input<bool>(i++);
  auto time = pro.get_input<float>(i++);

  std::ofstream file;
  file.open(output_filename.c_str());


  //zip through each block
  std::map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  std::map<bstm_block_id, bstm_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    bstm_block_id id = blk_iter->first;
    bstm_block_metadata data = blk_iter->second;
    double local_time;
    if(!data.contains_t(time,local_time))
      continue;
    bstm_data_base * points=        cache->get_data_base(id,bstm_data_traits<BSTM_POINT>::prefix());
    int num_el = points->buffer_length() / bstm_data_traits<BSTM_POINT>::datasize();

    //bstm_data_base * labels=        cache->get_data_base(id,bstm_data_traits<BSTM_LABEL>::prefix());
    bstm_data_base * change_prob =  cache->get_data_base(id,bstm_data_traits<BSTM_CHANGE>::prefix(), num_el *  bstm_data_traits<BSTM_CHANGE>::datasize());

//    bstm_data_base * change_prob_pos =  cache->get_data_base(id,bstm_data_traits<BSTM_CHANGE>::prefix("pos"));
//    bstm_data_base * change_prob_neg =  cache->get_data_base(id,bstm_data_traits<BSTM_CHANGE>::prefix("neg"));

    auto *     points_data = (bstm_data_traits<BSTM_POINT>::datatype*) points->data_buffer();
    auto *    change_data = (bstm_data_traits<BSTM_CHANGE>::datatype*) change_prob->data_buffer();

//    bstm_data_traits<BSTM_CHANGE>::datatype *    change_data_prob = (bstm_data_traits<BSTM_CHANGE>::datatype*) change_prob_pos->data_buffer();
//    bstm_data_traits<BSTM_CHANGE>::datatype *    change_data_neg = (bstm_data_traits<BSTM_CHANGE>::datatype*) change_prob_neg->data_buffer();

    file << std::fixed;
    for (unsigned currIdx=0; currIdx < (points->buffer_length()/bstm_data_traits<BSTM_POINT>::datasize() ) ; currIdx++) {
      if (points_data[currIdx][3] != -1) {
        file <<  points_data[currIdx][0] << ' ' << points_data[currIdx][1] << ' ' << points_data[currIdx][2] << ' ';
        if(output_aux)
          file << change_data[currIdx]  << std::endl;
        else
          file << std::endl;
      }

    }

  }

  std::cout << "Done exporting." << std::endl;
  return true;
}
