#ifndef bstm_copy_data_to_future_function_h
#define bstm_copy_data_to_future_function_h
//:
// \file    This function copyies the data in the input time step into all future time nodes

#include <iostream>
#include <bstm/bstm_data_traits.h>
#include <bstm/bstm_time_tree.h>
#include <bstm/bstm_data_base.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/io/bstm_cache.h>

template <bstm_data_type APM_DATA_TYPE, bstm_data_type NOBS_DATA_TYPE >
class bstm_copy_data_to_future_function
{
 public:
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;

  //: "default" constructor
  bstm_copy_data_to_future_function(bstm_time_block* t_blk, bstm_block* blk,
                      std::vector<bstm_data_base*> & datas, float time);

  //: initialize generic data base pointers as their data type
  bool init_data(bstm_time_block* t_blk, bstm_block* blk, std::vector<bstm_data_base*> & datas, float local_time);

  bool copy();

 private:

  void move_data(bstm_time_tree& time_tree);

  bstm_time_block* blk_t_;
  bstm_block* blk_;

  //TODO: data types are hardcoded for now
  bstm_data_traits<BSTM_ALPHA>::datatype*       alpha_;
  typename bstm_data_traits<APM_DATA_TYPE>::datatype*    mog_;
  typename bstm_data_traits<NOBS_DATA_TYPE>::datatype*     num_obs_;
  bstm_data_traits<BSTM_CHANGE>::datatype*     change_;

  float local_time_;

  unsigned num_el_; //the number of elements in meta data buffers
};

#endif //bstm_copy_data_to_future_function_h
