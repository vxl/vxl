#ifndef boxm2_export_point_cloud_xyz_h
#define boxm2_export_point_cloud_xyz_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iostream.h>
#include <boxm2/io/boxm2_cache.h>
#include <vgl/vgl_point_3d.h>

class boxm2_export_point_cloud_xyz_function
{
 public:
  //: "default" constructor
  static void exportPointCloudXYZ(boxm2_block_metadata data, boxm2_block* blk,
							 boxm2_data_base* alphas,
							 boxm2_data_base* points, boxm2_data_base* normals, vcl_ofstream& file,
							 bool output_prob, bool output_vis, float vis_t);
  static void exportPointCloudPLY(boxm2_block_metadata data, boxm2_block* blk,
							 boxm2_data_base* alphas,
							 boxm2_data_base* points, boxm2_data_base* normals, vcl_ofstream& file,
							 bool output_prob, bool output_vis, float vis_t, unsigned& num_vertices);
  static void writePLYHeader(vcl_ofstream& file, unsigned num_vertices, vcl_stringstream& ss);
};
#endif
