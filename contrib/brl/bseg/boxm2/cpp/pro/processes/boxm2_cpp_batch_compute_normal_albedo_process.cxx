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

#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>

//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/boxm2_util.h>

//#include <boxm2/cpp/algo/boxm2_synoptic_function_functors.h>
#include <boxm2/cpp/algo/boxm2_compute_normal_albedo_functor_opt.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>

//: run batch update
namespace boxm2_cpp_batch_compute_normal_albedo_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 0;
}

bool boxm2_cpp_batch_compute_normal_albedo_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_compute_normal_albedo_process_globals;

  //process takes 6 inputs
  // 0) scene
  // 1) cache
  // 2) stream cache
  // 3) name of text file containing list of image_metadata files
  // 4) name of text file containing list of atmospheric_parameters files
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "boxm2_stream_cache_sptr";
  input_types_[3] = "vcl_string"; 
  input_types_[4] = "vcl_string"; 

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
  
  vcl_string md_list_fname = pro.get_input<vcl_string>(3);
  vcl_string atm_list_fname = pro.get_input<vcl_string>(4);

  // load metadata and atmopsheric_parameters
  vcl_vector<brad_image_metadata> metadata;
  vcl_vector<brad_atmospheric_parameters> atm_params;

  vcl_ifstream md_list_ifs(md_list_fname.c_str());
  if (!md_list_ifs.good()) {
     vcl_cerr << "ERROR reading: " << md_list_fname << vcl_endl;
     return false;
  }
  while (!md_list_ifs.eof()) {
     vcl_string filename;
     md_list_ifs >> filename;
     if (filename.length() == 0)
        continue;
     vcl_cout << "metadata filename = <" << filename << ">" <<  vcl_endl;
     brad_image_metadata md;
     vcl_ifstream md_ifs(filename.c_str());
     if (!md_ifs.good()) {
        vcl_cerr << "ERROR reading image_metadata file: " << filename << vcl_endl;
        return false;
     }
     md_ifs >> md;
     metadata.push_back(md);
  }

  vcl_ifstream atm_list_ifs(atm_list_fname.c_str());
  if (!atm_list_ifs.good()) {
     vcl_cerr << "ERROR reading: " << atm_list_fname << vcl_endl;
     return false;
  }
  while (!atm_list_ifs.eof()) {
     vcl_string filename;
     atm_list_ifs >> filename;
     if (filename.length() == 0)
        continue;
     vcl_cout << "atmospheric_params filename = <" << filename << ">" <<  vcl_endl;
     brad_atmospheric_parameters atm;
     vcl_ifstream atm_ifs(filename.c_str());
     if (!atm_ifs.good()) {
        vcl_cerr << "ERROR reading atmospheric_parameters file: " << filename << vcl_endl;
        return false;
     }
     atm_ifs >> atm;
     atm_params.push_back(atm);
  }
  // sanity check
  if (metadata.size() != atm_params.size()) {
     vcl_cerr << "ERROR: metadata and atmospheric parameter files are different length" << vcl_endl;
     return false;
  }

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
    boxm2_compute_normal_albedo_functor_opt data_functor(update_alpha);
    data_functor.init_data(metadata, atm_params, str_cache, alpha, na_model_data);

    int na_model_TypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix());
    int data_buff_length = (int) (na_model_data->buffer_length()/na_model_TypeSize);

    boxm2_data_leaves_serial_iterator<boxm2_compute_normal_albedo_functor_opt>(blk, data_buff_length,data_functor);
    cache->remove_data_base( *id, boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix() );
    cache->remove_data_base( *id, boxm2_data_traits<BOXM2_ALPHA>::prefix() );

  }
  return true;
}

