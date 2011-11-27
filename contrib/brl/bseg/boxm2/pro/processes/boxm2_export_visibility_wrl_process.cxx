// This is brl/bseg/boxm2/pro/processes/boxm2_export_visibility_wrl_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting the visibilities computed at each cell as a wrl file
//
// \author Ali Osman Ulusoy
// \date Oct 17, 2011

#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_cache.h>


#include <bvrml/bvrml_write.h>
#include "boxm2/cpp/algo/boxm2_export_vis_wrl_function.h"

namespace boxm2_export_visibility_wrl_process_globals
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 0;
}

bool boxm2_export_visibility_wrl_process_cons(bprb_func_process& pro)
{

  using namespace boxm2_export_visibility_wrl_process_globals;

  //process takes 2 inputs
  int i=0;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vcl_string";

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  //output_types_[0] = "boxm2_scene_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

}

bool boxm2_export_visibility_wrl_process(bprb_func_process& pro)
{

  using namespace boxm2_export_visibility_wrl_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vcl_string output_filename = pro.get_input<vcl_string>(i++);

  vcl_cout << "STARTING EXPORT WRL MESH" << vcl_endl;

  vcl_ofstream myfile;
  myfile.open(output_filename.c_str());

  bvrml_write::write_vrml_header(myfile);

  //zip through each block
   vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
   vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
   for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
   {
     boxm2_block_id id = blk_iter->first;
     vcl_cout << "Filtering Block: "<<id<<vcl_endl;

     boxm2_block *     blk     = cache->get_block(id);
     boxm2_data_base * alph    = cache->get_data_base(id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
     boxm2_data_base * vis    = cache->get_data_base(id,boxm2_data_traits<BOXM2_AUX0>::prefix());
     boxm2_data_base * points    = cache->get_data_base(id,boxm2_data_traits<BOXM2_POINT>::prefix());

     //refine block and datas
     boxm2_block_metadata data = blk_iter->second;
     boxm2_compute_export_vis_wrl_function::exportVisToWRL(data,blk,alph,vis,points,myfile);
   }

  myfile.flush();
  myfile.close();

  return true;
}


