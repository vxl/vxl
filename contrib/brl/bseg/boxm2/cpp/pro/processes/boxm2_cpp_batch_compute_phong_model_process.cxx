// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_batch_compute_phong_model_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  Processes to update the scene using a set of data blocks in a batch mode
//
// \author Ozge C. Ozcanli
// \date May 12, 2011

#include <vcl_fstream.h>
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data_base.h>

//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/boxm2_util.h>

#include <boxm2/cpp/algo/boxm2_compute_phongs_model_functor.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>

//: run batch update
namespace boxm2_cpp_batch_compute_phong_model_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 0;
}

bool boxm2_cpp_batch_compute_phong_model_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_compute_phong_model_process_globals;

  //process takes 5 inputs
  // 0) scene
  // 1) cache
  // 2) stream cache
  // 3) the sun elevation
  // 4) the sun azimuth
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "boxm2_stream_cache_sptr";
  input_types_[3] = "float"; // sun_elev
  input_types_[4] = "float"; // sun_azim
  // process has 0 outputs:
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_batch_compute_phong_model_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_compute_phong_model_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
      vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
      return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  boxm2_stream_cache_sptr str_cache = pro.get_input<boxm2_stream_cache_sptr>(i++);
  float  sun_elev = pro.get_input<float>(i++);
  float  sun_azim = pro.get_input<float>(i++);

  // iterate the scene block by block and write to output
  vcl_vector<boxm2_block_id> blk_ids = scene->get_block_ids();
  vcl_vector<boxm2_block_id>::iterator id;
  id = blk_ids.begin();
  for (id = blk_ids.begin(); id != blk_ids.end(); id++) {
    boxm2_data_base *  alpha  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,true);

    boxm2_data_base *  phongs_model_data  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_FLOAT8>::prefix("phongs_model"),alpha->buffer_length()* 8 ,false);
    boxm2_compute_phongs_model_functor data_functor;
    data_functor.init_data(sun_elev,
                           sun_azim,
                           str_cache,
                           phongs_model_data);
    int phongs_model_TypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_FLOAT8>::prefix());
    int data_buff_length = (int) (phongs_model_data->buffer_length()/phongs_model_TypeSize);

    boxm2_data_serial_iterator<boxm2_compute_phongs_model_functor>(data_buff_length,data_functor);

    cache->remove_data_base( *id, boxm2_data_traits<BOXM2_FLOAT8>::prefix("phongs_model") );
  }
  return true;
}

