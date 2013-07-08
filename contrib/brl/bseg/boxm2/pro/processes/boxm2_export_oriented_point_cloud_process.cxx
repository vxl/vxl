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
#include <vcl_sstream.h>
#include <vgl/vgl_intersection.h>


namespace boxm2_export_oriented_point_cloud_process_globals
{
  const unsigned n_inputs_ = 9;
  const unsigned n_outputs_ = 0;
}

bool boxm2_export_oriented_point_cloud_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_export_oriented_point_cloud_process_globals;

  //process takes 8 inputs (3 required ones), no outputs
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vcl_string"; //filename
  input_types_[3] = "bool"; //output additional info (prob, vis score, normal magnitude)
  input_types_[4] = "float"; //visibility score threshold
  input_types_[5] = "float"; //normal magnitude threshold
  input_types_[6] = "float"; //prob. threshold
  input_types_[7] = "float"; //exp. threshold
  input_types_[8] = "vcl_string"; //bounding box filename

  brdb_value_sptr output_prob = new brdb_value_t<bool>(false);
  pro.set_input(3, output_prob);

  brdb_value_sptr vis_t = new brdb_value_t<float>(0);
  pro.set_input(4, vis_t);

  brdb_value_sptr nmag_t = new brdb_value_t<float>(0);
  pro.set_input(5, nmag_t);

  brdb_value_sptr prob_t = new brdb_value_t<float>(0.0);
  pro.set_input(6, prob_t);

  brdb_value_sptr exp_t = new brdb_value_t<float>(0.0);
  pro.set_input(7, exp_t);

  brdb_value_sptr bb_filename = new brdb_value_t<vcl_string>("");
  pro.set_input(8, bb_filename);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool boxm2_export_oriented_point_cloud_process (bprb_func_process& pro)
{
  using namespace boxm2_export_oriented_point_cloud_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
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
  float exp_t = pro.get_input<float>(i++);
  vcl_string bb_filename = pro.get_input<vcl_string>(i++);

  unsigned num_vertices = 0;

  vcl_ofstream myfile;
  myfile.open(output_filename.c_str());

  //read bb from ply, if any
  vgl_box_3d<double> original_bb;
  if (!bb_filename.empty()) {
    boxm2_export_oriented_point_cloud_function::readBBFromPLY(bb_filename, original_bb);
    vcl_cout << "Read bb from PLY: " << original_bb << vcl_endl;
  }
  
  
  //zip through each block
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    boxm2_block_metadata blk_info= blk_iter->second;
    
    if (bb_filename.empty()){
      original_bb = blk_info.bbox();
    }
    
    
    double finest_cell_length = (1.0 / (double) ( 1<<3))* blk_info.sub_block_dim_.x();
    vgl_box_3d<double> bb_expanded = original_bb;
    
    bb_expanded.set_min_z(original_bb.min_z() - 10.0 * finest_cell_length);
    bb_expanded.set_max_z(original_bb.max_z() + 40.0 * finest_cell_length);

   // bb_expanded.expand_about_centroid(finest_cell_length*10.0);
    
   
    if (vgl_intersection(bb_expanded, blk_info.bbox()).is_empty())
      continue;
    
    vcl_cout << "Processing Block: "<<id<< " with prob t: " << prob_t << ", vis t: " << vis_t << " and nmag_t: " << nmag_t << " finest cell length: " << finest_cell_length << vcl_endl;
    boxm2_block *     blk     = cache->get_block(id);

    //get data sizes
    vcl_size_t alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    vcl_size_t pointTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
    vcl_size_t normalTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL>::prefix());
    vcl_size_t visTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_VIS_SCORE>::prefix());
    int mogSize = (int) boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    vcl_size_t expTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_EXPECTATION>::prefix());
    vcl_size_t nobsTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::prefix());
    vcl_size_t raydirTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_RAY_DIR>::prefix());

    boxm2_data_base * alpha =        cache->get_data_base(id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    int data_buff_length    = (int) (alpha->buffer_length()/alphaTypeSize);
    
    //specify size to make sure data is right size.
    boxm2_data_base * points = cache->get_data_base(id,boxm2_data_traits<BOXM2_POINT>::prefix(), data_buff_length * pointTypeSize);
    boxm2_data_base * normals = cache->get_data_base(id,boxm2_data_traits<BOXM2_NORMAL>::prefix(), data_buff_length * normalTypeSize);
    boxm2_data_base * vis = cache->get_data_base(id,boxm2_data_traits<BOXM2_VIS_SCORE>::prefix(), data_buff_length * visTypeSize);
    boxm2_data_base * vis_sum = cache->get_data_base(id,boxm2_data_traits<BOXM2_VIS_SCORE>::prefix("sum"), data_buff_length * visTypeSize);
    boxm2_data_base * mog = cache->get_data_base(id,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(), data_buff_length * mogSize);
    boxm2_data_base * exp = cache->get_data_base(id,boxm2_data_traits<BOXM2_EXPECTATION>::prefix(), data_buff_length * expTypeSize);
    boxm2_data_base * nobs = cache->get_data_base(id,boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::prefix(), data_buff_length * nobsTypeSize);
    boxm2_data_base * ray_dir_sum = cache->get_data_base(id,boxm2_data_traits<BOXM2_RAY_DIR>::prefix(), data_buff_length * raydirTypeSize);
    boxm2_data_base * ray_dir_weighted_sum = cache->get_data_base(id,boxm2_data_traits<BOXM2_RAY_DIR>::prefix(), data_buff_length * raydirTypeSize);

    
    boxm2_block_metadata data = blk_iter->second;
    if (output_filename.substr(output_filename.find_last_of(".") + 1) == "xyz")
      boxm2_export_oriented_point_cloud_function::exportPointCloudXYZ(scene, data, blk, alpha, vis, vis_sum, exp, nobs, points,normals, ray_dir_sum, myfile, output_aux, vis_t, nmag_t, prob_t, exp_t, bb_expanded);
 /*   else if (output_filename.substr(output_filename.find_last_of(".") + 1) == "ply")
      boxm2_export_oriented_point_cloud_function::exportPointCloudPLY(scene, data, blk, alpha, mog, vis,  exp, nobs, points,normals, ray_dir_sum , ray_dir_weighted_sum, myfile, output_aux, vis_t, nmag_t, prob_t, exp_t, bb_expanded, num_vertices);*/
    else if (output_filename.substr(output_filename.find_last_of(".") + 1) == "ply")
      boxm2_export_oriented_point_cloud_function::exportPointCloudPLY(scene, data, blk, alpha, vis, points,normals, myfile, output_aux, vis_t, nmag_t, prob_t,  bb_expanded, num_vertices);

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

