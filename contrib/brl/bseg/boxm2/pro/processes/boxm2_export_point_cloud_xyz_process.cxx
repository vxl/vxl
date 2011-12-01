// This is brl/bseg/boxm2/pro/processes/boxm2_export_point_cloud_xyz_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting point clouds in XYZ or PLY format.
//
// \author Ali Osman Ulusoy
// \date Oct 25, 2011

#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_cache.h>

#include "boxm2/cpp/algo/boxm2_export_point_cloud_xyz_function.h"

namespace boxm2_export_point_cloud_xyz_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 0;
}

bool boxm2_export_point_cloud_xyz_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_export_point_cloud_xyz_process_globals;

  //process takes 6 inputs, no outputs
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vcl_string";
  input_types_[3] = "bool"; //output prob
  input_types_[4] = "bool"; //output vis
  input_types_[5] = "float";

  brdb_value_sptr output_prob = new brdb_value_t<bool>(false);
  pro.set_input(3, output_prob);

  brdb_value_sptr output_vis = new brdb_value_t<bool>(false);
  pro.set_input(4, output_vis);

  brdb_value_sptr vis_t = new brdb_value_t<float>(0);
  pro.set_input(5, vis_t);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool boxm2_export_point_cloud_xyz_process (bprb_func_process& pro)
{
  using namespace boxm2_export_point_cloud_xyz_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vcl_string output_filename = pro.get_input<vcl_string>(i++);
  bool output_prob = pro.get_input<bool>(i++);
  bool output_vis = pro.get_input<bool>(i++);
  float vis_t = pro.get_input<float>(i++);

  unsigned num_vertices = 0;

  vcl_cout << "START EXPORTING POINT CLOUD WITH VISIBILITY THRESHOLD  " << vis_t << vcl_endl;

  vcl_ofstream myfile;
  myfile.open(output_filename.c_str());

  //zip through each block

  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    vcl_cout << "Processing Block: "<<id<<vcl_endl;

    boxm2_block *     blk     = cache->get_block(id);
    boxm2_data_base * vis    = cache->get_data_base(id,boxm2_data_traits<BOXM2_AUX0>::prefix());
    boxm2_data_base * points    = cache->get_data_base(id,boxm2_data_traits<BOXM2_POINT>::prefix());
    boxm2_data_base * normals    = cache->get_data_base(id,boxm2_data_traits<BOXM2_NORMAL>::prefix());

    //refine block and datas
    boxm2_block_metadata data = blk_iter->second;
    if (output_filename.substr(output_filename.find_last_of(".") + 1) == "xyz")
      boxm2_export_point_cloud_xyz_function::exportPointCloudXYZ(data,blk,vis,points,normals,myfile, output_prob, output_vis, vis_t);
    else if (output_filename.substr(output_filename.find_last_of(".") + 1) == "ply")
      boxm2_export_point_cloud_xyz_function::exportPointCloudPLY(data,blk,vis,points,normals,myfile, output_prob, output_vis, vis_t, num_vertices);
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
    boxm2_export_point_cloud_xyz_function::writePLYHeader(myfile,num_vertices,ss);
    myfile.flush();
    myfile.close();
  }

  return true;
}

