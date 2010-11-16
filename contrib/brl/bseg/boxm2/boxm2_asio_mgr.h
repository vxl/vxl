#ifndef boxm2_asio_mgr_h_
#define boxm2_asio_mgr_h_
//:
// \file
#include <boxm2/boxm2_block.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <bbas/baio/baio.h>
#include <vgl/vgl_point_3d.h>

//: disk level storage class.
//  handles all of the asynchronous IO read and write requests
class boxm2_asio_mgr
{
  public:

    //: creates a BAIO object that loads block data from disk
    void load_block(vcl_string dir, boxm2_block_id block_id);
    
    //: creates a BAIO object that saves block data to disk
    void save_block(vcl_string dir, boxm2_block* block );

    //: creates a BAIO object that loads data from disk
    void load_data(vcl_string dir, boxm2_block_id block_id, int data_type);
    
    //: creates a BAIO object that saves data to disk
    void save_data(vcl_string dir, boxm2_block_id block_id, int data_type);

    //: returns load list (for updating cache)
    vcl_map<boxm2_block_id, baio> load_list() { return load_list_; }
    
    //: returns save list (for updating cache)
    vcl_map<boxm2_block_id, baio> save_list() { return save_list_; }

  private:

    //: NEED TO KEEP TRACK OF DATA LOAD AND SAVES
    //: list of asynchronous io loads TODO: Make a map to data_types as well
    vcl_map<boxm2_block_id, baio> load_list_;

    //: list of asynchronous io saves
    vcl_map<boxm2_block_id, baio> save_list_;
};

#endif // boxm2_asio_mgr_h_
