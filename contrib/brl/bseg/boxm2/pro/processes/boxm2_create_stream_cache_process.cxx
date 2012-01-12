// This is brl/bseg/boxm2/pro/processes/boxm2_create_stream_cache_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for creating stream cache.
//
// \author Ozge C. Ozcanli
// \date May 10, 2011

#include <vcl_fstream.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_stream_cache.h>

namespace boxm2_create_stream_cache_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}

bool boxm2_create_stream_cache_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_create_stream_cache_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "vcl_string";  // data type
  input_types_[2] = "vcl_string";  // name of the identifier list file
  input_types_[3] = "float"; // number of gigabytes available for stream cache, the buffer lengths will be based on this number

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_stream_cache_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_create_stream_cache_process(bprb_func_process& pro)
{
  using namespace boxm2_create_stream_cache_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene= pro.get_input<boxm2_scene_sptr>(i++);
  vcl_string data_type_fname = pro.get_input<vcl_string>(i++);   // open data streams of this type with each identifier
  vcl_string identifier_fname= pro.get_input<vcl_string>(i++);
  float num_giga = pro.get_input<float>(i++);

  // extract list of image_ids from file
  vcl_ifstream ifs(identifier_fname.c_str());
  if (!ifs.good()) {
    vcl_cerr << "error opening file " <<identifier_fname << '\n';
    return false;
  }
  vcl_vector<vcl_string> image_ids;
  unsigned int n_images = 0;
  ifs >> n_images;
  for (unsigned int i=0; i<n_images; ++i) {
    vcl_string img_id;
    ifs >> img_id;
    image_ids.push_back(img_id);
    //vcl_cout << "adding: " << img_id << vcl_endl;
  }
  ifs.close();

  // extract list of type names from file
  vcl_ifstream ifst(data_type_fname.c_str());
  if (!ifst.good()) {
    vcl_cerr << "error opening file " <<data_type_fname << '\n';
    return false;
  }
  vcl_vector<vcl_string> type_names;
  unsigned int n_types = 0;
  ifst >> n_types;
  for (unsigned int i=0; i<n_types; ++i) {
    vcl_string type_name;
    ifst >> type_name;
    type_names.push_back(type_name);
    //vcl_cout << "adding type: " << type_name << vcl_endl;
  }
  ifst.close();

  boxm2_stream_cache * str_cache = new boxm2_stream_cache(scene, type_names, image_ids, num_giga);

  // store cache pointer
  pro.set_output_val<boxm2_stream_cache_sptr>(0, str_cache);
  return true;
}


namespace boxm2_stream_cache_close_files_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 0;
}

bool boxm2_stream_cache_close_files_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_stream_cache_close_files_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_stream_cache_sptr";
  
  // process has 0 output:
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_stream_cache_close_files_process(bprb_func_process& pro)
{
  using namespace boxm2_stream_cache_close_files_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_stream_cache_sptr cache = pro.get_input<boxm2_stream_cache_sptr>(i++);
  cache->close_streams();
  return true;
}

