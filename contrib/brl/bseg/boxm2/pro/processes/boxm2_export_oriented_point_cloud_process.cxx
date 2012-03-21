// This is brl/bseg/boxm2/pro/processes/boxm2_export_oriented_point_cloud_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting the scene as a point cloud in XYZ or PLY format. The process expects datatypes BOXM2_POINT and BOXM2_NORMAL.
//         The process can take as input a bounding box, specified as two points in a ply file. In addition to points and normals, the process can
//         output quantities such as probabilities, visibility score(BOXM2_VIS_SCORE), as well as normal
//         magnitude(currently stored in BOXM2_NORMAL[4] for convenience).
//
// \author Ali Osman Ulusoy
// \date Oct 25, 2011

#include <vcl_fstream.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_data_traits.h>
#include "boxm2/cpp/algo/boxm2_export_oriented_point_cloud_function.h"


namespace boxm2_export_oriented_point_cloud_process_globals
{
  const unsigned n_inputs_ = 8;
  const unsigned n_outputs_ = 0;

}

bool boxm2_export_oriented_point_cloud_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_export_oriented_point_cloud_process_globals;

  //process takes 6 inputs, no outputs
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vcl_string"; //filename
  input_types_[3] = "bool"; //output additional info (prob, vis score, normal magnitude)
  input_types_[4] = "float"; //visibility score threshold
  input_types_[5] = "float"; //normal magnitude threshold
  input_types_[6] = "float"; //prob. threshold
  input_types_[7] = "vcl_string"; //bounding box filename

  brdb_value_sptr output_prob = new brdb_value_t<bool>(false);
  pro.set_input(3, output_prob);

  brdb_value_sptr vis_t = new brdb_value_t<float>(0);
  pro.set_input(4, vis_t);

  brdb_value_sptr nmag_t = new brdb_value_t<float>(0);
  pro.set_input(5, nmag_t);

  brdb_value_sptr prob_t = new brdb_value_t<float>(0.0);
  pro.set_input(6, prob_t);

  brdb_value_sptr bb_filename = new brdb_value_t<vcl_string>("");
  pro.set_input(7, bb_filename);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool boxm2_export_oriented_point_cloud_process (bprb_func_process& pro)
{
  using namespace boxm2_export_oriented_point_cloud_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vcl_string output_filename = pro.get_input<vcl_string>(i++);
  bool output_aux = pro.get_input<bool>(i++);
  float vis_t = pro.get_input<float>(i++);
  float nmag_t = pro.get_input<float>(i++);
  float prob_t = pro.get_input<float>(i++);
  vcl_string bb_filename = pro.get_input<vcl_string>(i++);

  unsigned num_vertices = 0;

  vcl_ofstream myfile;
  myfile.open(output_filename.c_str());

  //read bb from ply, if any
  vgl_box_3d<double> bb;
  if(!bb_filename.empty()) {
    boxm2_export_oriented_point_cloud_function::readBBFromPLY(bb_filename, bb);
  }

  //zip through each block
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  int dataSize = 0;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    vcl_cout << "Processing Block: "<<id<< " with prob t: " << prob_t << ", vis t: " << vis_t << " and nmag_t: " << nmag_t << vcl_endl;

    boxm2_block *     blk     = cache->get_block(id);

    //get data sizes
    vcl_size_t alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    vcl_size_t pointTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
    vcl_size_t normalTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL>::prefix());
    vcl_size_t visTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_VIS_SCORE>::prefix());

    boxm2_data_base * alpha =        cache->get_data_base(id,boxm2_data_traits<BOXM2_ALPHA>::prefix());

    //specify size to make sure data is right size.
    boxm2_data_base * points    = cache->get_data_base(id,boxm2_data_traits<BOXM2_POINT>::prefix(), (alpha->buffer_length() /alphaTypeSize) * pointTypeSize);
    boxm2_data_base * normals    = cache->get_data_base(id,boxm2_data_traits<BOXM2_NORMAL>::prefix(), (alpha->buffer_length() /alphaTypeSize) * normalTypeSize);
    boxm2_data_base * vis =        cache->get_data_base(id,boxm2_data_traits<BOXM2_VIS_SCORE>::prefix(), (alpha->buffer_length() /alphaTypeSize) * visTypeSize);

    boxm2_block_metadata data = blk_iter->second;
    if (output_filename.substr(output_filename.find_last_of(".") + 1) == "xyz")
      boxm2_export_oriented_point_cloud_function::exportPointCloudXYZ(scene, data, blk, alpha, vis, points,normals, myfile, output_aux, vis_t, nmag_t, prob_t, bb);
    else if (output_filename.substr(output_filename.find_last_of(".") + 1) == "ply")
      boxm2_export_oriented_point_cloud_function::exportPointCloudPLY(scene, data, blk, alpha, vis, points,normals, myfile, output_aux, vis_t, nmag_t, prob_t, bb, num_vertices);
    else
      vcl_cout << "UNKNOWN FILE FORMAT..." << vcl_endl;
  }
  myfile.flush();
  myfile.close();

  //if ply, have to write annoying header at the beginning
  if (output_filename.substr(output_filename.find_last_of(".") + 1) == "ply") {
    vcl_ifstream myfile_input;
    myfile_input.open(output_filename.c_str());
    vcl_stringstream ss;
    ss << myfile_input.rdbuf();
    myfile_input.close();
    myfile.open(output_filename.c_str());
    boxm2_export_oriented_point_cloud_function::writePLYHeader(myfile,num_vertices,ss,output_aux);
    myfile.flush();
    myfile.close();
  }

  return true;
}

