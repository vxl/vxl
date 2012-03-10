#ifndef boxm2_export_point_cloud_xyz_h
#define boxm2_export_point_cloud_xyz_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>

class boxm2_export_point_cloud_xyz_function
{
 public:
  static void exportPointCloudXYZ(boxm2_block_metadata data, boxm2_block* blk,
                                  boxm2_data_base* alphas,
                                  boxm2_data_base* points, boxm2_data_base* normals, vcl_ofstream& file,
                                  bool output_aux, float vis_t, float nmag_t);
  static void exportPointCloudPLY(boxm2_block_metadata data, boxm2_block* blk,
                                  boxm2_data_base* alphas,
                                  boxm2_data_base* points, boxm2_data_base* normals, vcl_ofstream& file,
                                  bool output_aux, float vis_t, float nmag_t, unsigned& num_vertices);
  static void writePLYHeader(vcl_ofstream& file, unsigned num_vertices, vcl_stringstream& ss, bool output_aux);
};

#endif
