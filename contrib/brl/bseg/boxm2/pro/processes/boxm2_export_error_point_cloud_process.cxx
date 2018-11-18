// This is brl/bseg/boxm2/pro/processes/boxm2_export_oriented_point_cloud_process.cxx
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting the point hypothesis at each voxel as a point cloud in PLY format.
//         The process expects datatypes BOXM2_POINT and BOXM2_COVARIANCE.
//         The process can take as input the scene file to get its bounding box,
//
// \author Ozge C. Ozcanli
// \date April 18, 2012

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_data_traits.h>
#include "boxm2/cpp/algo/boxm2_export_oriented_point_cloud_function.h"

namespace boxm2_export_error_point_cloud_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_export_error_point_cloud_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_export_error_point_cloud_process_globals;

  //process takes 6 inputs
  std::vector<std::string>  output_types_(n_outputs_);
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vcl_string"; //filename
  input_types_[3] = "float"; //prob. threshold
  input_types_[4] = "float"; //LE threshold  -- points with LE less than LE threshold will be returned, pass -1 if no LE thresholding will be done
  input_types_[5] = "float"; //CE threshold  -- points with CE less than CE threshold will be returned, pass -1 if no CE thresholding will be done

  brdb_value_sptr prob_t = new brdb_value_t<float>(0.0);
  pro.set_input(3, prob_t);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_export_error_point_cloud_process (bprb_func_process& pro)
{
  using namespace boxm2_export_error_point_cloud_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  std::string output_filename = pro.get_input<std::string>(i++);
  auto prob_t = pro.get_input<float>(i++);
  auto LE_t = pro.get_input<float>(i++);
  auto CE_t = pro.get_input<float>(i++);
  // if LE_t or CE_t are passed as -1.0 then they are invalid so replace them with max value so that any LE, CE is less than that
  if (LE_t == -1.0)
    LE_t = std::numeric_limits<float>::max();
  if (CE_t == -1.0)
    CE_t = std::numeric_limits<float>::max();

  unsigned num_vertices = 0;

  std::vector<std::string> apps = scene->appearances();
  std::string data_type = "";
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix() )
      data_type = app;
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
      data_type = app;
  }
  if(    data_type=="")
  {
      std::cout<<"COLOR Model not recognizable"<<std::endl;
      return false;
  }

  std::ofstream myfile;
  myfile.open(output_filename.c_str());

  vgl_box_3d<double> bb = scene->bounding_box();

  //get data sizes
  std::size_t pointTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
  std::size_t covTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_COVARIANCE>::prefix());
  std::size_t alphaTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
  std::size_t mogTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  std::size_t rgbTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix());

  //zip through each block
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    boxm2_block *     blk = cache->get_block(scene, id);

    //: the following code does not work
    boxm2_block_metadata data = blk_iter->second;

    boxm2_data_base * alph = cache->get_data_base(scene, id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
    int buffer_length = (int)(alph->buffer_length()/alphaTypeSize);
    std::cout << "\nin blk: " << id << " data buf len: " << buffer_length << std::endl;

    std::cout << "Processing Block: "<<id<< " with prob t: " << prob_t << std::endl;

    //specify size to make sure data is right size.
    boxm2_data_base * points = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_POINT>::prefix(), buffer_length*pointTypeSize,true);
    boxm2_data_base * covs = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_COVARIANCE>::prefix(), buffer_length*covTypeSize,true);
    boxm2_data_base * mog;

    if( data_type == "boxm2_gauss_rgb")
      mog = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(), buffer_length*rgbTypeSize,true);
    else
      mog = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(), buffer_length*mogTypeSize,true);


    boxm2_export_oriented_point_cloud_function::exportPointCloudPLY(scene, data, blk, mog, alph, points, covs, myfile, prob_t, LE_t, CE_t, bb, num_vertices, data_type);
  }
  myfile.flush();
  myfile.close();

  std::ifstream myfile_input;
  myfile_input.open(output_filename.c_str());
  std::stringstream ss;
  ss << myfile_input.rdbuf();
  myfile_input.close();
  myfile.open(output_filename.c_str());
  boxm2_export_oriented_point_cloud_function::writePLYHeaderOnlyPoints(myfile,num_vertices,ss);
  myfile.flush();
  myfile.close();

  return true;
}
