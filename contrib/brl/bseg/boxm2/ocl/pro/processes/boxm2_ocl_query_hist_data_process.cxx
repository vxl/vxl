// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_query_hist_data_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for querying  histogram  data given a 3-d point.
//
// \author Vishal Jain
// \date Mar 10, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

#include <boct/boct_bit_tree.h>
#define SEGLEN_FACTOR 93206.7555f   //Hack representation of int32.maxvalue/(ni*nj*block_length)

namespace boxm2_ocl_query_hist_data_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 7;
}

bool boxm2_ocl_query_hist_data_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_query_hist_data_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_cache_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "float"; //x
  input_types_[3] = "float"; //y
  input_types_[4] = "float"; //z


  std::vector<std::string>  output_types_(n_outputs_);
  for (unsigned i=0;i<7;i++) output_types_[i] = "float";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_query_hist_data_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_query_hist_data_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  auto x =pro.get_input<float>(i++);
  auto y =pro.get_input<float>(i++);
  auto z =pro.get_input<float>(i++);
  vnl_vector_fixed<int,8> * element_int = nullptr;
  // set arguments
  std::vector<boxm2_block_id> block_ids = scene->get_block_ids();
  for (auto & block_id : block_ids)
  {
    boxm2_block_metadata mdata=scene->get_block_metadata(block_id);
    vgl_vector_3d<double> dims(mdata.sub_block_dim_.x()*mdata.sub_block_num_.x(),
                               mdata.sub_block_dim_.y()*mdata.sub_block_num_.y(),
                               mdata.sub_block_dim_.z()*mdata.sub_block_num_.z());

    vgl_point_3d<double> lorigin=mdata.local_origin_;
    vgl_box_3d<double> bbox(lorigin,lorigin+dims);
    if (!bbox.contains(x,y,z)) continue;

    // get the data pointer of the cell containin the given point.
    double local_x=(x-mdata.local_origin_.x())/mdata.sub_block_dim_.x();
    double local_y=(y-mdata.local_origin_.y())/mdata.sub_block_dim_.y();
    double local_z=(z-mdata.local_origin_.z())/mdata.sub_block_dim_.z();

    int index_x=(int)std::floor(local_x);
    int index_y=(int)std::floor(local_y);
    int index_z=(int)std::floor(local_z);
    boxm2_block * blk=cache->get_block(scene, block_id);


    vnl_vector_fixed<unsigned char,16> treebits=blk->trees()(index_x,index_y,index_z);
    boct_bit_tree tree(treebits.data_block(),mdata.max_level_);
    int bit_index=tree.traverse(vgl_point_3d<double>(local_x,local_y,local_z));

    //int buff_index=(int)treebits[12]*256+(int)treebits[13];
    int data_offset=tree.get_data_index(bit_index,false);//buff_index*65536+tree.get_data_index(bit_index);

    boxm2_data_base *  hist_base = cache->get_data_base(scene, block_id,boxm2_data_traits<BOXM2_BATCH_HISTOGRAM>::prefix());
    boxm2_data<BOXM2_BATCH_HISTOGRAM> *hist_data=new boxm2_data<BOXM2_BATCH_HISTOGRAM>(hist_base->data_buffer(),hist_base->buffer_length(),hist_base->block_id());

    boxm2_array_1d<vnl_vector_fixed<float,8> > hist_data_array=hist_data->data();
    vnl_vector_fixed<float,8> element=hist_data_array[data_offset];

    element_int =reinterpret_cast<vnl_vector_fixed<int,8> * >( &element );

    break;
  }
  if (element_int)
    for (unsigned k=0;k<7;k++)
      pro.set_output_val<float>(k,(float)((*element_int )[k])/SEGLEN_FACTOR);
  return true;
}
