// This is brl/bseg/bstm/pro/processes/bstm_boxm2_scene_to_bstm_process.cxx
#include <fstream>
#include <iostream>
#include <cstdio>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for creating a scene from a bundler file
//
// \author Andy Miller
// \date Sep 16, 2011
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/bstm_scene.h>
#include <boxm2/boxm2_scene.h>
#include <bstm/util/bstm_cams_and_box_to_scene.h>
#include <vgl/vgl_box_3d.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

namespace bstm_boxm2_scene_to_bstm_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}

bool bstm_boxm2_scene_to_bstm_process_cons(bprb_func_process& pro)
{
  using namespace bstm_boxm2_scene_to_bstm_process_globals;

  //process takes 5 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // boxm2 filename
  input_types_[1] = "vcl_string"; // bstm data pathc
  input_types_[2] = "unsigned";   // number of time steps

  // process has 2 outputs
  std::vector<std::string>  output_types_(n_outputs_);

  //set input and output types
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_boxm2_scene_to_bstm_process(bprb_func_process& pro)
{
  using namespace bstm_boxm2_scene_to_bstm_process_globals;
  typedef vpgl_perspective_camera<double> CamType;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }

  //----------------------------------------------------------------------------
  //get the inputs
  //----------------------------------------------------------------------------
  unsigned i = 0;
  std::string boxm2_filename = pro.get_input<std::string>(i++);
  std::string bstm_scene_dir = pro.get_input<std::string>(i++);
  auto time_steps = pro.get_input<unsigned>(i++);


  if (!vul_file::make_directory_path( bstm_scene_dir.c_str()))
    return false;

  boxm2_scene_sptr boxm2Scene = new boxm2_scene(boxm2_filename);
  bstm_scene_sptr scene = new bstm_scene(bstm_scene_dir, vgl_point_3d<double>(0.0,0.0,0.0));
  scene->set_local_origin(vgl_point_3d<double>(0.0,0.0,0.0));

  //setup appearances
  std::vector<std::string> boxm2_apps = boxm2Scene->appearances();
  std::vector<std::string> apps;
  for(auto boxm2_app : boxm2_apps)
  {
    boxm2_app.replace(boxm2_app.begin(), boxm2_app.begin() + 5, "bstm");
    apps.push_back(boxm2_app);
  }
  scene->set_appearances(apps);

  //setup blocks

  //time related stuff
  double scene_origin_t = 0;
  double subBlockDim_t = 32; //fixed for now
  unsigned num_blocks_t = (double)(time_steps) / subBlockDim_t;
  unsigned numSubBlocks_t = 1;
  std::cout << "Num t blocks :" << num_blocks_t << std::endl;

  std::vector<boxm2_block_id> boxm2_blocks = boxm2Scene->get_block_ids();
  for(std::vector<boxm2_block_id>::const_iterator iter = boxm2_blocks.begin(); iter != boxm2_blocks.end(); iter++)
  {
    boxm2_block_metadata boxm2_mdata = boxm2Scene->get_block_metadata_const(*iter);

    for(unsigned int t = 0; t < num_blocks_t; ++t)
    {
        //get block map
        bstm_block_id id(iter->i_,iter->j_,iter->k_,t);

        std::map<bstm_block_id, bstm_block_metadata> blks = scene->blocks();
        if (blks.find(id)!=blks.end()) {
            std::cout<<"block already exists: "<<id<<std::endl;
            continue;
        }

        //block origin
        double bt = scene_origin_t + t*subBlockDim_t;

        ////get the inputs
        unsigned max_num_lvls = boxm2_mdata.max_level_;
        float    max_data_size = boxm2_mdata.max_mb_;
        float    p_init = boxm2_mdata.p_init_;
        bstm_block_metadata mdata(id,
                                    boxm2_mdata.local_origin_,
                                   bt,
                                   boxm2_mdata.sub_block_dim_, subBlockDim_t,
                                   boxm2_mdata.sub_block_num_, numSubBlocks_t,
                                   1,max_num_lvls,max_data_size,p_init);
        blks[id]=mdata;
        scene->set_blocks(blks);
    }


  }


  scene->save_scene();
  std::cout << "Done." << std::endl;
  return true;
}
