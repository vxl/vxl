// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_batch_compute_normal_albedo_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  Processes to update the scene using a set of data blocks in a batch mode
//
// \author Daniel Crispell
// \date Nov 23, 2011

#include <vcl_fstream.h>
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_normal_albedo_array.h>
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>

//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/boxm2_util.h>

//#include <boxm2/cpp/algo/boxm2_synoptic_function_functors.h>
#include <boxm2/cpp/algo/boxm2_compute_normal_albedo_functor.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>

//: run batch update
namespace boxm2_cpp_batch_compute_normal_albedo_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 0;
}

bool boxm2_cpp_batch_compute_normal_albedo_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_compute_normal_albedo_process_globals;

  //process takes 6 inputs
  // 0) scene
  // 1) cache
  // 2) stream cache
  // 3) the list of sun elevations
  // 4) the list of sun azimuths
  // 5) the list of image irradiances (compute with bbas_estimate_irradiance_process)
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "boxm2_stream_cache_sptr";
  input_types_[3] = "bbas_1d_array_float_sptr"; // sun_azimuths (degrees)
  input_types_[4] = "bbas_1d_array_float_sptr"; // sun_elivations (degrees)
  input_types_[5] = "bbas_1d_array_float_sptr"; // estimated irradiances (compute with bbas_estimate_irradiance_process)

  // process has 0 outputs:
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_batch_compute_normal_albedo_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_compute_normal_albedo_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
      vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
      return false;
  }

  //get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(1);
  boxm2_stream_cache_sptr str_cache = pro.get_input<boxm2_stream_cache_sptr>(2);

  bbas_1d_array_float_sptr sun_azims = pro.get_input<bbas_1d_array_float_sptr>(3);
  bbas_1d_array_float_sptr sun_elevs = pro.get_input<bbas_1d_array_float_sptr>(4);

  bbas_1d_array_float_sptr irrads = pro.get_input<bbas_1d_array_float_sptr>(5);

  // iterate the scene block by block and write to output
  vcl_vector<boxm2_block_id> blk_ids = scene->get_block_ids();
  vcl_vector<boxm2_block_id>::iterator id;
  id = blk_ids.begin();
  for (id = blk_ids.begin(); id != blk_ids.end(); id++) {
    boxm2_block *     blk     = cache->get_block(*id);
    boxm2_data_base *  alpha  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0, false);
    unsigned int n_cells = alpha->buffer_length() / boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    vcl_string naa_prefix = boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix();
    vcl_cout << "naa_prefix = " << naa_prefix << vcl_endl;
    unsigned int na_model_size = boxm2_data_info::datasize(naa_prefix);
    unsigned int na_data_size = n_cells * boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix());
    vcl_cout << "na_model_size = " << na_model_size << vcl_endl;
    vcl_cout << "na_model_size2 = " << sizeof(boxm2_normal_albedo_array) << vcl_endl;
    vcl_cout << "n_cells = " << n_cells << vcl_endl;
    vcl_cout << "na_data_size = " << na_data_size << vcl_endl;
    boxm2_data_base *  na_model_data  = cache->get_data_base(*id,boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix(), na_data_size, false);

    bool update_alpha = false;
    boxm2_compute_normal_albedo_functor data_functor(update_alpha);
    data_functor.init_data(sun_azims, sun_elevs, irrads, str_cache, alpha, na_model_data);

    int na_model_TypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix());
    int data_buff_length = (int) (na_model_data->buffer_length()/na_model_TypeSize);

    boxm2_data_leaves_serial_iterator<boxm2_compute_normal_albedo_functor>(blk, data_buff_length,data_functor);
    cache->remove_data_base( *id, boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix() );
    cache->remove_data_base( *id, boxm2_data_traits<BOXM2_ALPHA>::prefix() );


  }
  return true;
}

