#ifndef space_time_sio_mgr_h_
#define space_time_sio_mgr_h_

//:
// \file space_time_sio_mgr.h
// \brief Loads blocks and data from ID's and data_types with blocking.
//  If file is not available, will return null.

//: disk level storage class.
//  handles all of the synchronous IO read and write requests

#include <vcl_string.h>

#include <bstm/basic/bstm_block_id.h>

template <typename Block> class space_time_sio_mgr {
public:
  typedef typename Block::metadata block_metadata;

  //: loads block from disk
  static Block *load_block(vcl_string dir,
                           const bstm_block_id &block_id,
                           const block_metadata &data);

  //: saves block to disk
  static void save_block(vcl_string dir, Block *block);

  //: load data generically
  // loads a generic data_base* from disk (given data_type string prefix)
  static space_time_data_base *load_block_data_generic(vcl_string dir,
                                                       const bstm_block_id &id,
                                                       vcl_string data_type);

  //: saves data generically
  // generically saves data_base * to disk (given prefix)
  static void save_block_data_base(vcl_string dir,
                                   const bstm_block_id &block_id,
                                   space_time_data_base *data,
                                   vcl_string prefix);
};

#endif // space_time_sio_mgr_h_
