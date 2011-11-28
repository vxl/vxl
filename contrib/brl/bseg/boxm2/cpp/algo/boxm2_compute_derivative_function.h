#ifndef boxm2_compute_derivative_function_h
#define boxm2_compute_derivative_function_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iostream.h>
#include <boxm2/io/boxm2_cache.h>
#include <vgl/vgl_point_3d.h>

class boxm2_compute_derivative_function
{
 public:
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;

  //: "default" constructor
  boxm2_compute_derivative_function(boxm2_block_metadata data, boxm2_block* blk, boxm2_data_base* alphas,
                                    boxm2_data_base* normals,  boxm2_data_base* points, float prob_threshold,
                                    float normal_threshold, vcl_string  kernel_x_file_name,
                                    vcl_string kernel_y_file_name, vcl_string kernel_z_file_name);

 private:
  //: returns a list of 3d points of neighboring cells in 3d
  vcl_vector<vcl_pair<vgl_point_3d<int>, vgl_point_3d<double> > >  neighbor_points(const  vgl_point_3d<double>& cellCenter, double side_len, const boxm2_array_3d<uchar16>& trees, int nhood_size );
  //: evaluate given points in the data
  vcl_vector<vcl_pair<vgl_point_3d<int>, float> > eval_neighbors(
            boxm2_block_metadata data, const boct_bit_tree& bit_tree,
            const vcl_vector<vcl_pair<vgl_point_3d<int>, vgl_point_3d<double> > >& neighbors,  const boxm2_array_3d<uchar16>& trees,
            const boxm2_data_traits<BOXM2_ALPHA>::datatype* alpha_data, int curr_depth);
  //: load kernel from file
  vcl_vector<vcl_pair<vgl_point_3d<int>, float> > load_kernel(vcl_string filename);
  //: apply a filter to given neighborhood
  float apply_filter(vcl_vector<vcl_pair<vgl_point_3d<int>, float> > neighbors, vcl_vector<vcl_pair<vgl_point_3d<int>, float> > filter);

  vcl_vector<vcl_pair<vgl_point_3d<int>, float> > kernel_x_;
  vcl_vector<vcl_pair<vgl_point_3d<int>, float> > kernel_y_;
  vcl_vector<vcl_pair<vgl_point_3d<int>, float> > kernel_z_;
};


#endif
