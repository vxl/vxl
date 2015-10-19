// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_get_index_from_3d_point_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for converting a 3d point into an octree blockid/index
//
// \author Andy Neff
// \date Oct 15 2015

#include <vcl_fstream.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_util.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <bsta/bsta_joint_histogram.h>
//directory utility
#include <vcl_where_root_dir.h>
#include <boct/boct_bit_tree.h>
#include <brad/brad_phongs_model_est.h>


namespace boxm2_cpp_get_index_from_3d_point_process_globals
{
  const unsigned n_inputs_ =  5;
  const unsigned n_outputs_ = 4;
}

bool boxm2_cpp_get_index_from_3d_point_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_get_index_from_3d_point_process_globals;

  bool ok=false;
 
  //process takes 7 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "float"; //x
  input_types_[3] = "float"; //y
  input_types_[4] = "float"; //z
  ok = pro.set_input_types(input_types_);
  if (!ok) return ok;

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0]="int"; 
  output_types_[1]="int"; 
  output_types_[2]="int"; 
  output_types_[3]="int"; 
  ok = pro.set_output_types(output_types_);
  if (!ok) return ok;
  
  return true;
}

bool  boxm2_cpp_get_index_from_3d_point_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_get_index_from_3d_point_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  float x =pro.get_input<float>(i++);
  float y =pro.get_input<float>(i++);
  float z =pro.get_input<float>(i++);

  vgl_point_3d<double> local;
  boxm2_block_id id;
  if (!scene->contains(vgl_point_3d<double>(x, y, z), id, local))
    return false;

  int index_x=(int)vcl_floor(local.x()); //should this be +0.5?
  int index_y=(int)vcl_floor(local.y()); //should this be +0.5?
  int index_z=(int)vcl_floor(local.z()); //should this be +0.5?
  boxm2_block * blk=cache->get_block(scene, id);
  boxm2_block_metadata mdata = scene->get_block_metadata_const(id);
  vnl_vector_fixed<unsigned char,16> treebits=blk->trees()(index_x,index_y,index_z);
  boct_bit_tree tree(treebits.data_block(),mdata.max_level_);
  int bit_index=tree.traverse(local);
  int index=tree.get_data_index(bit_index,false);
  
  pro.set_output_val<int>(0, id.i());
  pro.set_output_val<int>(1, id.j());
  pro.set_output_val<int>(2, id.k());
  pro.set_output_val<int>(3, index);

  return true;
}
