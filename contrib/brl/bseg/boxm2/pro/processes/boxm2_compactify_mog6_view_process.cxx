// This is brl/bseg/boxm2/pro/processes/boxm2_compactify_mog6_view_process.cxx
//:
// \file

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/boxm2_data_traits.h>

namespace boxm2_compactify_mog6_view_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_compactify_mog6_view_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_compactify_mog6_view_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";

  // process has 2 outputs:
  std::vector<std::string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_compactify_mog6_view_process(bprb_func_process& pro)
{
  using namespace boxm2_compactify_mog6_view_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  if (!scene){
    std::cout << " null scene in boxm2_compactify_mog6_view_process\n";
    return false;
  }
  boxm2_cache_sptr  cache = pro.get_input<boxm2_cache_sptr>(1);


  //verifies that a scene has a valid appearance, spits out data type and appearance type size
  std::vector<std::string> valid_types;
  valid_types.push_back(boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix());
  valid_types.push_back(boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW>::prefix());
  std::string data_type;
  int appTypeSize;
  boxm2_util::verify_appearance(*scene, valid_types, data_type, appTypeSize);
  std::cout<<"DATA_TYPE:"<<data_type<<std::endl;

  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::const_iterator iter;
  for(iter = blocks.begin(); iter != blocks.end(); iter++)
  {
    if(data_type == boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix())
    {
      boxm2_data_base* app = cache->get_data_base(scene, iter->first, boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix(),0);
      auto* app_buf = ( vnl_vector_fixed<float, 16>*)app->data_buffer();
      unsigned num_el = app->buffer_length() / boxm2_data_traits<BOXM2_MOG6_VIEW>::datasize();
      boxm2_data_base* app_compact = cache->get_data_base(scene,iter->first, boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::prefix(), num_el * boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::datasize());
      auto* app_compact_buf = ( vnl_vector_fixed<unsigned char, 16>*)app_compact->data_buffer();
      for(unsigned  i = 0; i < num_el;i++)
      {
        vnl_vector_fixed<float, 16> tmp = app_buf[i];
        vnl_vector_fixed<unsigned char, 16> tmp_compact;
        for(unsigned  j = 0;j<16;j++)
          tmp_compact[j] = (unsigned char)(tmp[j]* 255);
        app_compact_buf[i] = tmp_compact;
      }
    }
    else if(data_type == boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW>::prefix())
    {
      boxm2_data_base* app = cache->get_data_base(scene,iter->first, boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW>::prefix(),0);
      auto* app_buf = ( vnl_vector_fixed<int, 16>*)app->data_buffer();
      unsigned num_el = app->buffer_length() / boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW>::datasize();
      boxm2_data_base* app_compact = cache->get_data_base(scene,iter->first, boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>::prefix(), num_el * boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>::datasize());
      auto* app_compact_buf = ( vnl_vector_fixed<int, 8>*)app_compact->data_buffer();
      for(unsigned  i = 0; i < num_el;i++)
      {
        vnl_vector_fixed<int, 16> tmp = app_buf[i];
        vnl_vector_fixed<int, 8> tmp_compact;
        for(unsigned  j = 0;j<8;j++)
          tmp_compact[j] = tmp[j*2];
        app_compact_buf[i] = tmp_compact;
      }
    }


  }

  return true;
}
