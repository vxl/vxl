// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_batch_probe_intensities_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for finding mean intensities at each voxel using a set of data blocks in a batch mode
//
// \author Ozge C. Ozcanli
// \date May 12, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/boxm2_util.h>

#include <boxm2/cpp/algo/boxm2_mean_intensities_batch_functor.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>
#include <boxm2/cpp/algo/boxm2_cast_intensities_functor.h>
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>

namespace boxm2_cpp_batch_probe_intensities_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 3;
}

bool boxm2_cpp_batch_probe_intensities_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_probe_intensities_process_globals;

  //process takes 3 inputs
  // 0) scene
  // 2) cache
  // 3) stream cache
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "boxm2_stream_cache_sptr";
  input_types_[3] = "float";
  input_types_[4] = "float";
  input_types_[5] = "float";
  // process has 0 output:
  std::vector<std::string>  output_types_(n_outputs_);

  output_types_[0]="bbas_1d_array_float_sptr";
  output_types_[1]="bbas_1d_array_float_sptr";
  output_types_[2]="bbas_1d_array_float_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_batch_probe_intensities_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_probe_intensities_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache =pro.get_input<boxm2_cache_sptr>(i++);
  boxm2_stream_cache_sptr str_cache= pro.get_input<boxm2_stream_cache_sptr>(i++);
  auto x= pro.get_input<float>(i++);
  auto y= pro.get_input<float>(i++);
  auto z= pro.get_input<float>(i++);

  vgl_point_3d<double> local;
  boxm2_block_id id;
  if (!scene->contains(vgl_point_3d<double>(x, y, z), id, local))
    return false;

  int index_x=(int)std::floor(local.x());
  int index_y=(int)std::floor(local.y());
  int index_z=(int)std::floor(local.z());

  boxm2_block * blk=cache->get_block(scene,id);
  boxm2_block_metadata mdata = scene->get_block_metadata_const(id);
  vnl_vector_fixed<unsigned char,16> treebits=blk->trees()(index_x,index_y,index_z);
  boct_bit_tree tree(treebits.data_block(),mdata.max_level_);
  int bit_index=tree.traverse(local);
  int data_index=tree.get_data_index(bit_index,false);
  std::cout<<"Index is "<<data_index<<std::endl;

   std::vector<float> cum_len;
   std::vector<float> cum_int;
   std::vector<float> cum_vis;
   std::vector<float> cum_pre;
  for (int k=0; k<=data_index; ++k)
  {
    cum_len=str_cache->get_next<BOXM2_AUX0>(id,k);
    cum_int=str_cache->get_next<BOXM2_AUX1>(id,k);
    cum_vis=str_cache->get_next<BOXM2_AUX2>(id,k);
    cum_pre=str_cache->get_next<BOXM2_AUX3>(id,k);
  }

  std::cout<<"=================="<<std::endl;
  bbas_1d_array_float_sptr intensities  =new bbas_1d_array_float(cum_int.size());
  bbas_1d_array_float_sptr visibilites  =new bbas_1d_array_float(cum_int.size());
  bbas_1d_array_float_sptr pres  =new bbas_1d_array_float(cum_int.size());

  for (unsigned i=0;i<cum_int.size();i++)
  {
    if (cum_len[i]>1e-10f)
    {
      intensities->data_array[i]=cum_int[i]/cum_len[i];
      visibilites->data_array[i]=cum_vis[i]/cum_len[i];
      pres->data_array[i] = cum_pre[i]/ cum_len[i];
    }
    else
    {
      intensities->data_array[i]=-1.0f;
      visibilites->data_array[i]= 0.0f;
      pres->data_array[i]=0.0f;
    }
  }// store scene smaprt pointer
  pro.set_output_val<bbas_1d_array_float_sptr>(0, intensities );
  pro.set_output_val<bbas_1d_array_float_sptr>(1, visibilites );
  pro.set_output_val<bbas_1d_array_float_sptr>(2, pres );

  return true;
}
