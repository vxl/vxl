#ifndef boxm2_export_vis_wrl_h
#define boxm2_export_vis_wrl_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iostream.h>
#include <boxm2/io/boxm2_cache.h>
#include <vgl/vgl_point_3d.h>


class boxm2_compute_export_vis_wrl_function
{
 public:
  //: "default" constructor
  static void exportVisToWRL(boxm2_block_metadata data, boxm2_block* blk,
                             boxm2_data_base* alphas, boxm2_data_base* vis,
                             boxm2_data_base* points, vcl_ofstream& file);
};
#endif
