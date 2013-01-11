#ifndef bstm_sio_mgr_h_
#define bstm_sio_mgr_h_
//:
// \file
// \brief Loads blocks and data from ID's and data_types with blocking.
//  If file is not available, will return null.
#include <bstm/bstm_block.h>
#include <bstm/bstm_time_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/basic/bstm_block_id.h>
#include <bstm/bstm_data_traits.h>
#include <vcl_iostream.h>

//: disk level storage class.
//  handles all of the synchronous IO read and write requests
class bstm_sio_mgr
{
  public:
    //: loads block from disk
    static bstm_block* load_block(vcl_string dir, bstm_block_id block_id,bstm_block_metadata data );

    //: saves block to disk
    static void           save_block(vcl_string dir, bstm_block* block);

    //: loads time block from disk
    static bstm_time_block* load_time_block(vcl_string dir, bstm_block_id block_id,bstm_block_metadata data );

    //: saves time block to disk
    static void           save_time_block(vcl_string dir, bstm_time_block* block);

    //: load data generically
    // loads a generic bstm_data_base* from disk (given data_type string prefix)
    static bstm_data_base*  load_block_data_generic(vcl_string dir, bstm_block_id id, vcl_string data_type);

    //: saves data generically
    // generically saves data_base * to disk (given prefix)
    static void save_block_data_base(vcl_string dir, bstm_block_id block_id, bstm_data_base* data, vcl_string prefix);
};


#endif // bstm_sio_mgr_h_
