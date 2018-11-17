#ifndef boxm2_filter_block_function_h
#define boxm2_filter_block_function_h
//:
// \file

#include <iostream>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <vgl/vgl_point_3d.h>

class boxm2_filter_block_function
{
 public:
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;

  //: "default" constructor
  boxm2_filter_block_function(const boxm2_scene_sptr& scene, boxm2_block_metadata data, boxm2_block* blk, boxm2_data_base* alphas);

 private:

  //: returns a list of 3d points (int locations) of neighboring blocks
  std::vector<vgl_point_3d<int> > neighbors( vgl_point_3d<int>& center, boxm2_array_3d<uchar16>& trees );
  //: returns a list of 3d points of neighboring blocks
  std::vector<vgl_point_3d<double> > neighbor_points( vgl_point_3d<double>& cellCenter, double side_len,const boxm2_array_3d<uchar16>& trees );

  boxm2_block* blk_;
  uchar16*     trees_;
  float*       alpha_;
  boxm2_scene_sptr scene_;
};


#endif
