
#include <boxm2/boxm2_block.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>
#include <bbas/baio/baio.h>
#include <vgl/vgl_point_3d.h>

//: boxm2_disk_io_mgr: disk level storage class
//  handles all of the asynchronous IO read and write requests
class boxm2_disk_io_mgr
{
  
  public: 
    
    //: creates a BAIO object that loads/saves block data from disk
    void load_block(vcl_string dir, vgl_point_3d<int> block_id); 
    void save_block(vcl_string dir, boxm2_block* block );
    
    //: creates a BAIO object that laods/saves data from disk
    void load_data(vcl_string dir, vgl_point_3d<int> block_id, int data_type); 
    void save_data(vcl_string dir, vgl_point_3d<int> block_id, int data_type);
  
    //: returns load and save lists (for updating cache)
    vcl_vector<baio> load_list() { return load_list_; }
    vcl_vector<baio> save_list() { return save_list_; }
  
  private: 
    
    //: list of asynchronous io loads
    vcl_vector<baio> load_list_;
    
    //: list of asynchronous io saves
    vcl_vector<baio> save_list_; 
  
};
