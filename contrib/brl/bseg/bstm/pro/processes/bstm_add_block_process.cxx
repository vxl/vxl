// This is brl/bseg/bstm/pro/processes/bstm_add_block_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for creating a scene
//


#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/bstm_scene.h>

namespace bstm_add_block_process_globals
{
  constexpr unsigned n_inputs_ = 21;
  constexpr unsigned n_outputs_ = 0;
}

bool bstm_add_block_process_cons(bprb_func_process& pro)
{
  using namespace bstm_add_block_process_globals;

  //process takes 2 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bstm_scene_sptr";

  input_types_[1] = "int";
  input_types_[2] = "int";
  input_types_[3] = "int";
  input_types_[4] = "int";

  input_types_[5] = "unsigned";
  input_types_[6] = "unsigned";
  input_types_[7] = "unsigned";
  input_types_[8] = "unsigned";

  input_types_[9] = "unsigned"; // maximum tree depth
  input_types_[10] = "unsigned"; // maximum tree depth_t

  input_types_[11] = "float";    // local origin x
  input_types_[12] = "float";    // local origin y
  input_types_[13] = "float";    // local origin z
  input_types_[14] = "float";    // local origin t

  input_types_[15] = "float";    // block dims
  input_types_[16] = "float";    // block dims_t

  input_types_[17] = "float";    // max_mb
  input_types_[18] = "float";    // p_init
  input_types_[19] = "unsigned"; // init tree depth
  input_types_[20] = "unsigned"; // init time tree depth

  // process has 1 output
  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bstm_add_block_process(bprb_func_process& pro)
{
  using namespace bstm_add_block_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene    =pro.get_input<bstm_scene_sptr>(i++);
  int index_i          =pro.get_input<int>        (i++);
  int index_j          =pro.get_input<int>        (i++);
  int index_k          =pro.get_input<int>        (i++);
  int index_t          =pro.get_input<int>        (i++);
  auto num_x            =pro.get_input<unsigned>        (i++);
  auto num_y            =pro.get_input<unsigned>        (i++);
  auto num_z            =pro.get_input<unsigned>        (i++);
  auto num_t            =pro.get_input<unsigned>        (i++);
  auto max_num_lvls     =pro.get_input<unsigned>        (i++);
  auto max_num_lvls_t   =pro.get_input<unsigned>        (i++);

  auto    local_x          =pro.get_input<float>           (i++);
  auto    local_y          =pro.get_input<float>           (i++);
  auto    local_z          =pro.get_input<float>           (i++);
  auto    local_t          =pro.get_input<float>           (i++);

  auto    blk_dims         =pro.get_input<float>           (i++);
  auto    blk_dim_t        =pro.get_input<float>           (i++);

  auto    max_data_size    =pro.get_input<float>           (i++);
  auto    p_init           =pro.get_input<float>           (i++);

  auto init_level     =pro.get_input<unsigned>        (i++);
  auto init_level_t   =pro.get_input<unsigned>        (i++);

  bstm_block_id id(index_i,index_j,index_k, index_t);
  std::map<bstm_block_id, bstm_block_metadata> blks=scene->blocks();

  if (blks.find(id)!=blks.end())
  {
      std::cout<<"block already exists"<<std::endl;
      return false;
  }

  bstm_block_metadata mdata(id,
                             vgl_point_3d<double>(local_x,local_y,local_z), local_t,
                             vgl_vector_3d<double>(blk_dims,blk_dims,blk_dims), blk_dim_t,
                             vgl_vector_3d<unsigned>(num_x,num_y,num_z), num_t,
                             init_level,max_num_lvls, max_data_size,p_init, 2, init_level_t, max_num_lvls_t);



  blks[id]=mdata;
  scene->set_blocks(blks);

  return true;
}
