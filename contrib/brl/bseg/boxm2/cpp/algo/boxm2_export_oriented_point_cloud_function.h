#ifndef boxm2_export_oriented_point_cloud_function_h
#define boxm2_export_oriented_point_cloud_function_h
//:
// \file

#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>
#include <vcl_sstream.h>
#include <vcl_iomanip.h>
#include <boct/boct_bit_tree.h>

#include <rply.h>   //.ply parser

#include "boxm2_mog3_grey_processor.h"
#include "boxm2_gauss_rgb_processor.h"

#include "boxm2/boxm2_util.h"

class boxm2_export_oriented_point_cloud_function
{
 public:
  static void exportPointCloudXYZ(const boxm2_scene_sptr& scene, boxm2_block_metadata data, boxm2_block* blk,
                                  boxm2_data_base* alpha, boxm2_data_base* vis,
                                  boxm2_data_base* points, boxm2_data_base* normals, vcl_ofstream& file,
                                  bool output_aux, float vis_t, float nmag_t, float prob_t, vgl_box_3d<double> bb);
  static void exportPointCloudPLY(const boxm2_scene_sptr& scene, boxm2_block_metadata data, boxm2_block* blk,
                                  boxm2_data_base* alpha, boxm2_data_base* vis,
                                  boxm2_data_base* points, boxm2_data_base* normals, vcl_ofstream& file,
                                  bool output_aux, float vis_t, float nmag_t, float prob_t, vgl_box_3d<double> bb, unsigned& num_vertices);

  static bool calculateProbOfPoint(const boxm2_scene_sptr& scene, boxm2_block * blk, const vnl_vector_fixed<float, 4>& point, const float& alpha, float& prob);

  static void writePLYHeader(vcl_ofstream& file, unsigned num_vertices, vcl_stringstream& ss, bool output_aux);

  static void readBBFromPLY(const vcl_string& filename, vgl_box_3d<double>& box);
};



#endif
