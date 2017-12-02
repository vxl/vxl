#ifndef bstm_majority_filter_h
#define bstm_majority_filter_h
//:
// \file

#include <bstm/bstm_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#include <bstm/io/bstm_cache.h>

class bstm_majority_filter
{
 public:
  typedef unsigned char uchar;
  typedef vnl_vector_fixed<uchar, 16> uchar16;

  //: "default" constructor
  bstm_majority_filter(bstm_block_metadata data, bstm_block* blk,bstm_time_block* blk_t, bstm_data_base* changes);

 private:
  //: returns a list of 3d points of neighboring blocks
  std::vector<vgl_point_3d<double> > neighbor_points( vgl_point_3d<double>& cellCenter, double side_len, boxm2_array_3d<uchar16>& trees );

};


#endif
