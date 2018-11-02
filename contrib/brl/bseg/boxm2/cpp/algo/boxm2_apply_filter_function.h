#ifndef boxm2_apply_filter_function_h
#define boxm2_apply_filter_function_h
//:
// \file

#include <iostream>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <vnl/vnl_operators.h>
#include <vgl/vgl_point_3d.h>

template <boxm2_data_type RESPONSE_DATA_TYPE>
class boxm2_apply_filter_function
{
 public:
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;

  //specify kernel base name and the id of kernel to run
  boxm2_apply_filter_function (std::string kernel_base_file_name, unsigned id_kernel);


  //apply all the filters to specified data and save the results in points and responses
  //additionally this function can interpolate responses to normals.
  void apply_filter(boxm2_block_metadata data, boxm2_block* blk, boxm2_data_base* alphas,
                    boxm2_data_base* responses, float prob_threshold, unsigned octree_lvl);

 private:

  //: returns the map between the kernel grid and the actual points in the global coordinates
  bool neighbor_points(const vgl_point_3d<double>& cellCenter, double side_len, const boxm2_array_3d<uchar16>& trees,
                       std::map<vnl_vector_fixed<int,3>, vgl_point_3d<double> >& neighborhood);

  //: evaluate given points in the data
  float eval_alpha(
            boxm2_block_metadata data, const boct_bit_tree& bit_tree,
            const vgl_point_3d<double> & point,  const boxm2_array_3d<uchar16>& trees,
            const boxm2_data_traits<BOXM2_ALPHA>::datatype* alpha_data, int curr_depth);

  //: evaluate a filter in a given neighborhood
  float eval_filter(std::map<vnl_vector_fixed<int,3>, vgl_point_3d<double> > neighbors,
                    boxm2_block_metadata data, const boct_bit_tree& bit_tree,  const boxm2_array_3d<uchar16>& trees,
                    const boxm2_data_traits<BOXM2_ALPHA>::datatype* alpha_data, int curr_depth);

  std::map< vnl_vector_fixed<int,3> , float> kernel_;
};

#endif //boxm2_apply_filter_function_h
