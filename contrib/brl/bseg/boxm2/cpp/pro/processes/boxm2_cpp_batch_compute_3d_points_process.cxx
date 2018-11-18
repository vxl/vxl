// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_batch_compute_3d_points_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process to compute a point hypothesis and a covariance for each voxel
//         First the process boxm2_cpp_cast_3d_point_hypothesis_process is run for each image
//
// \author Ozge C. Ozcanli
// \date April 04, 2012

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/boxm2_util.h>

#include <boxm2/cpp/algo/boxm2_3d_point_estimator_batch_functor.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>

namespace boxm2_cpp_batch_compute_3d_points_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_batch_compute_3d_points_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_compute_3d_points_process_globals;

  //process takes 3 inputs
  // 0) scene
  // 2) cache
  // 3) stream cache
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "boxm2_stream_cache_sptr";
  // process has 0 output:
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_batch_compute_3d_points_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_compute_3d_points_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  boxm2_stream_cache_sptr str_cache= pro.get_input<boxm2_stream_cache_sptr>(i++);

  boxm2_block_id bid;
  int data_index = 0;

  // vgl_point_3d<double> point(309.583,251.252,258.228);
  // boxm2_block_id bid; int data_index; float side_len;
  // if (!boxm2_util::get_point_index(scene,cache,point,bid,data_index,side_len)) {
  //   std::cout << "Cannot locate the point: " << point << " in the scene!\n";
  //   return false;
  // } else
  //   std::cout << "point: " << point << " is in block: " << bid << " index: " << data_index << std::endl;

  // assumes that the intensities of each image have been cast into data models of type ALPHA previously
  int ptsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
  int covTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_COVARIANCE>::prefix());
  int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
  // iterate the scene block by block and write to output
  std::vector<boxm2_block_id> blk_ids = scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator id;
  id = blk_ids.begin();
  for (id = blk_ids.begin(); id != blk_ids.end(); ++id) {
    // we're assuming that we have enough RAM to store the whole output blocks

    if(str_cache->exists<BOXM2_POINT>(*id)==0)
      continue;
    if(str_cache->exists<BOXM2_COVARIANCE>(*id)==0)
      continue;

    //: alpha is only retrieved to get buf len, there is a problem in get_data_base_new: TODO: fix this, there should be no need to retrieve alpha
    boxm2_data_base *  alph = cache->get_data_base(scene, *id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
    std::size_t buf_len = alph->buffer_length();
    std::cout << "\nin blk: " << *id << " data buf len: " << buf_len/alphaTypeSize << "\n";

    boxm2_data_base * output_pts = cache->get_data_base_new(scene, *id,boxm2_data_traits<BOXM2_POINT>::prefix(),buf_len/alphaTypeSize*ptsTypeSize,false);
    boxm2_data_base * output_covs = cache->get_data_base_new(scene, *id,boxm2_data_traits<BOXM2_COVARIANCE>::prefix(),buf_len/alphaTypeSize*covTypeSize,false);
    boxm2_3d_point_estimator_batch_functor data_functor;
    data_functor.init_data(output_pts, output_covs, str_cache, bid, data_index);

    int data_buf_len = output_pts->buffer_length()/ptsTypeSize;
    std::cout << "in blk: " << *id << " data buf len: " << data_buf_len << "\n";
    boxm2_data_serial_iterator<boxm2_3d_point_estimator_batch_functor>(data_buf_len,data_functor);
    cache->remove_data_base(scene, *id,boxm2_data_traits<BOXM2_POINT>::prefix());  // cache needs to be read-write cache for output blocks to be written before being discarded
    cache->remove_data_base(scene, *id,boxm2_data_traits<BOXM2_COVARIANCE>::prefix());
    std::cout << "\n";
  }

  return true;
}
