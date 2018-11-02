#ifndef block_sio_mgr_h_
#define block_sio_mgr_h_

//:
// \file block_sio_mgr.h
// \brief Loads blocks and data from ID's and data_types with blocking.
//  If file is not available, will return null. Closely based off of
//  bstm/io/bstm_sio_manager.h
//
// \author Raphael Kargon
// \date Aug 02 2017

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bstm_multi/block_data_base.h>

//: disk level storage class.
//  handles all of the synchronous IO read and write requests/
//  Templated on block type.
template <typename Block> class block_sio_mgr {
public:
  typedef typename Block::id_t block_id;
  typedef typename Block::metadata_t block_metadata;

  //: loads block from disk
  static Block *load_block(const std::string &dir,
                           const block_id &block_id,
                           const block_metadata &data);

  //: saves block to disk
  static void save_block(const std::string &dir, Block *block);

  //: load data generically
  // loads a generic data_base* from disk (given data_type string prefix)
  static block_data_base *load_block_data_generic(const std::string &dir,
                                                  const block_id &id,
                                                  const std::string &data_type);

  //: saves data generically
  // generically saves data_base * to disk (given prefix)
  static void save_block_data_base(const std::string &dir,
                                   const block_id &block_id,
                                   block_data_base *data,
                                   const std::string &prefix);
};

#endif // block_sio_mgr_h_
