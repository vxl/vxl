#include "boxm2_export_oriented_point_cloud.h"

#include <boxm2/cpp/algo/boxm2_export_oriented_point_cloud_function.h>
#include <vgl/vgl_intersection.h>

bool boxm2_export_oriented_point_cloud::
export_oriented_point_cloud(boxm2_scene_sptr scene, const boxm2_cache_sptr& cache,
                            const std::string& output_filename, bool output_aux,
                            float vis_t, float nmag_t, float prob_t, float exp_t,
                            const std::string& bb_filename)
{
  unsigned num_vertices = 0;

  std::ofstream myfile;
  myfile.open(output_filename.c_str());

  //read bb from ply, if any
  vgl_box_3d<double> original_bb;
  if (!bb_filename.empty()) {
    boxm2_export_oriented_point_cloud_function::readBBFromPLY(bb_filename, original_bb);
    std::cout << "Read bb from PLY: " << original_bb << std::endl;
  }


  //zip through each block
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
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

    std::cout << "Processing Block: "<<id<< " with prob t: " << prob_t << ", vis t: " << vis_t << " and nmag_t: " << nmag_t << " finest cell length: " << finest_cell_length << std::endl;
    boxm2_block *     blk     = cache->get_block(scene,id);

    //get data sizes
    std::size_t alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    std::size_t pointTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
    std::size_t normalTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL>::prefix());
    std::size_t visTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_VIS_SCORE>::prefix());
    int mogSize = (int) boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    std::size_t expTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_EXPECTATION>::prefix());
    std::size_t nobsTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::prefix());
    std::size_t raydirTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_RAY_DIR>::prefix());

    // check for invalid parameters
    if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
    {
      std::cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
      return false;
    }

    boxm2_data_base * alpha =        cache->get_data_base(scene, id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    int data_buff_length    = (int) (alpha->buffer_length()/alphaTypeSize);

    //specify size to make sure data is right size.
    boxm2_data_base * points = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_POINT>::prefix(), data_buff_length * pointTypeSize);
    boxm2_data_base * normals = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_NORMAL>::prefix(), data_buff_length * normalTypeSize);
    boxm2_data_base * vis = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_VIS_SCORE>::prefix(), data_buff_length * visTypeSize);
    boxm2_data_base * vis_sum = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_VIS_SCORE>::prefix("sum"), data_buff_length * visTypeSize);
    // boxm2_data_base * mog = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(), data_buff_length * mogSize);
    boxm2_data_base * exp = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_EXPECTATION>::prefix(), data_buff_length * expTypeSize);
    boxm2_data_base * nobs = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::prefix(), data_buff_length * nobsTypeSize);
    boxm2_data_base * ray_dir_sum = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_RAY_DIR>::prefix(), data_buff_length * raydirTypeSize);
    //boxm2_data_base * ray_dir_weighted_sum = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_RAY_DIR>::prefix(), data_buff_length * raydirTypeSize);


    boxm2_block_metadata data = blk_iter->second;
    if (output_filename.substr(output_filename.find_last_of('.') + 1) == "xyz")
      boxm2_export_oriented_point_cloud_function::exportPointCloudXYZ(scene, data, blk, alpha, vis, vis_sum, exp, nobs, points,normals, ray_dir_sum, myfile, output_aux, vis_t, nmag_t, prob_t, exp_t, bb_expanded);
 /*   else if (output_filename.substr(output_filename.find_last_of(".") + 1) == "ply")
      boxm2_export_oriented_point_cloud_function::exportPointCloudPLY(scene, data, blk, alpha, mog, vis,  exp, nobs, points,normals, ray_dir_sum , ray_dir_weighted_sum, myfile, output_aux, vis_t, nmag_t, prob_t, exp_t, bb_expanded, num_vertices);*/
    else if (output_filename.substr(output_filename.find_last_of('.') + 1) == "ply")
      boxm2_export_oriented_point_cloud_function::exportPointCloudPLY(scene, data, blk, alpha, vis, points,normals, myfile, output_aux, vis_t, nmag_t, prob_t,  bb_expanded, num_vertices);

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
    boxm2_export_oriented_point_cloud_function::writePLYHeader(myfile,num_vertices,ss,output_aux);
    myfile.flush();
    myfile.close();
  }

  return true;
}
