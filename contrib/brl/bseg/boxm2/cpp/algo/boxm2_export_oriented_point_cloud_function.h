#ifndef boxm2_export_oriented_point_cloud_function_h
#define boxm2_export_oriented_point_cloud_function_h
//:
// \file

#include <sstream>
#include <iostream>
#include <iomanip>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>
#include <boct/boct_bit_tree.h>

#include "boxm2_mog3_grey_processor.h"
#include "boxm2_gauss_rgb_processor.h"

#include "boxm2/boxm2_util.h"

class boxm2_export_oriented_point_cloud_function
{
 public:
  static void exportPointCloudXYZ(const boxm2_scene_sptr& scene, const boxm2_block_metadata& data, boxm2_block* blk,
                                      boxm2_data_base* alpha, boxm2_data_base* vis, boxm2_data_base* vis_sum, boxm2_data_base* exp,boxm2_data_base* nobs,
                                      boxm2_data_base* points, boxm2_data_base* normals,
                                      boxm2_data_base* ray_dir_sum, std::ofstream& file,
                                      bool output_aux, float vis_t, float nmag_t, float prob_t, float exp_t, vgl_box_3d<double> bb);
  static void exportPointCloudPLY(const boxm2_scene_sptr& scene, const boxm2_block_metadata& data, boxm2_block* blk,
                                      boxm2_data_base* alpha, boxm2_data_base* vis,
                                      boxm2_data_base* points, boxm2_data_base* normals,
                                      std::ofstream& file,
                                      bool output_aux, float vis_t, float nmag_t, float prob_t, vgl_box_3d<double> bb, unsigned& num_vertices);
  static void exportColorPointCloudPLY(const boxm2_scene_sptr& scene, const boxm2_block_metadata& data, boxm2_block* blk,
                                       boxm2_data_base* mog, boxm2_data_base* alpha,const std::string& datatype,
                                       boxm2_data_base* points,std::ofstream& file,float prob_t,vgl_box_3d<double> bb, unsigned& num_vertices);

  static bool calculateProbOfPoint(const boxm2_scene_sptr& scene, boxm2_block * blk,
                                   const vnl_vector_fixed<float, 4>& point,
                                   const float& alpha, float& prob);

  static void writePLYHeader(std::ofstream& file, unsigned num_vertices, std::stringstream& ss, bool output_aux);
  static void writePLYHeaderOnlyPoints(std::ofstream& file, unsigned num_vertices, std::stringstream& ss);

  //:Reads a bounding box from a .PLY. the box is created such that all points in the .PLY file are contained in the box
  static void readBBFromPLY(const std::string& filename, vgl_box_3d<double>& box);

  //: Use covariance estimate to export a point cloud
  //  return points for which prob is greater than prob_t and LE is less than LE_t and CE is less than LE_t
  static void exportPointCloudPLY(const boxm2_scene_sptr& scene, const boxm2_block_metadata& data, boxm2_block* blk,
                                  boxm2_data_base* mog, boxm2_data_base* alpha, boxm2_data_base* points, boxm2_data_base* covariances, std::ofstream& file,
                                  float prob_t, float LE_t, float CE_t, vgl_box_3d<double> bb, unsigned& num_vertices, const std::string& datatype);

  static bool calculateProbOfPoint(const boxm2_scene_sptr& scene, boxm2_block * blk,
                                   const vnl_vector_fixed<float, 4>& point,
                                   const vnl_vector_fixed<float, 9>& cov,
                                   const float& alpha,
                                   float& prob, double& color,
                                   vnl_vector_fixed<double, 3>& axes, double& LE, double& CE);
  static bool calculateLECEofPoint(const vnl_vector_fixed<float, 9>& cov,vnl_vector_fixed<double, 3>&axes, double& LE, double& CE);

};



#endif
