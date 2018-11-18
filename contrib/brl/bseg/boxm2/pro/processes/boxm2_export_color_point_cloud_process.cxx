// This is brl/bseg/boxm2/pro/processes/boxm2_export_color_point_cloud_process.cxx
#include <iostream>
#include <fstream>
#include <sstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting the scene as a point cloud in XYZ or PLY format along with color or grayscale information. The process expects datatypes BOXM2_POINT and BOXM2_NORMAL.
//         The process can take as input a bounding box, specified as two points in a ply file.
//
// \author Vishal Jain
// \date Aug 15, 2014

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_data_traits.h>
#include "boxm2/cpp/algo/boxm2_export_oriented_point_cloud_function.h"
#include <vgl/vgl_intersection.h>


namespace boxm2_export_color_point_cloud_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_export_color_point_cloud_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_export_color_point_cloud_process_globals;
  //process takes 8 inputs (3 required ones), no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vcl_string"; //filename
  input_types_[3] = "float";      //prob. threshold
  input_types_[4] = "vcl_string"; //identifier

  brdb_value_sptr prob_t = new brdb_value_t<float>(0.3);
  pro.set_input(3, prob_t);

  brdb_value_sptr identifier = new brdb_value_t<std::string>("");
  pro.set_input(4, identifier);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_export_color_point_cloud_process(bprb_func_process& pro)
{
  using namespace boxm2_export_color_point_cloud_process_globals;
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
  std::string identifier = pro.get_input<std::string>(i++);

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
  unsigned num_vertices = 0;
  std::ofstream myfile;
  myfile.open(output_filename.c_str());
  //zip through each block
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
      boxm2_block_id id = blk_iter->first;
      boxm2_block_metadata blk_info= blk_iter->second;
      vgl_box_3d<double> original_bb = blk_info.bbox();
      double finest_cell_length = (1.0 / (double) ( 1<<3))* blk_info.sub_block_dim_.x();
      vgl_box_3d<double> bb_expanded = original_bb;
      if (vgl_intersection(bb_expanded, blk_info.bbox()).is_empty())
          continue;
      std::cout << "Processing Block: "<<id<< " with prob t: " << prob_t << "Color Model "  << " finest cell length: " << finest_cell_length << std::endl;
      boxm2_block *     blk = cache->get_block(scene,id);
      //get data sizes
      std::size_t alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      std::size_t pointTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
      int mogSize = (int) boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
      std::size_t expTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_EXPECTATION>::prefix());
      boxm2_data_base * alpha =        cache->get_data_base(scene, id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
      int data_buff_length = (int) (alpha->buffer_length()/alphaTypeSize);
      //specify size to make sure data is right size.
      boxm2_data_base * points = cache->get_data_base(scene, id,boxm2_data_traits<BOXM2_POINT>::prefix(), data_buff_length * pointTypeSize);
      boxm2_data_base * mog;
      if( data_type == "boxm2_gauss_rgb")
         mog = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(identifier), data_buff_length * mogSize);
      else
          mog= cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(identifier), data_buff_length * mogSize);
      boxm2_block_metadata data = blk_iter->second;
      if (output_filename.substr(output_filename.find_last_of('.') + 1) == "xyz")
          boxm2_export_oriented_point_cloud_function::exportColorPointCloudPLY(scene, data, blk, mog, alpha,data_type,  points, myfile,  prob_t,  bb_expanded, num_vertices);
      else if (output_filename.substr(output_filename.find_last_of('.') + 1) == "ply")
          boxm2_export_oriented_point_cloud_function::exportColorPointCloudPLY(scene, data, blk, mog, alpha,data_type,  points, myfile,  prob_t,  bb_expanded, num_vertices);
      else
          std::cout << "UNKNOWN FILE FORMAT..." << std::endl;
  }
  myfile.flush();
  myfile.close();
  //if ply, have to write annoying header at the beginning
  if (output_filename.substr(output_filename.find_last_of('.') + 1) == "ply") {
    std::ifstream myfile_input;
    myfile_input.open(output_filename.c_str());
    std::stringstream ss;
    ss << myfile_input.rdbuf();
    myfile_input.close();
    myfile.open(output_filename.c_str());
    myfile << "ply\nformat ascii 1.0\nelement vertex " << num_vertices
        << "\nproperty float x\nproperty float y\nproperty float z\nproperty uchar red\nproperty uchar green\nproperty uchar blue\nproperty float prob\n"
        << "end_header\n"
        << ss.str();
    myfile.flush();
    myfile.close();
  }
  return true;
}
