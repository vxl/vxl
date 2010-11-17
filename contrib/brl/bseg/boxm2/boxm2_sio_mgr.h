#ifndef boxm2_sio_mgr_h_
#define boxm2_sio_mgr_h_
//:
// \file
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_block_id.h>
#include <vcl_iostream.h>

//: disk level storage class.
//  handles all of the synchronous IO read and write requests
class boxm2_sio_mgr
{
  public:

    //: loads block from disk
    static boxm2_block* load_block(vcl_string dir, boxm2_block_id block_id); 
    
    //: saves block to disk
    static void save_block(vcl_string dir, boxm2_block* block );


    //TODO create data load and save functions

};

#endif // boxm2_sio_mgr_h_
