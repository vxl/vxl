
//: boxm2_cache: top level storage (abstract) class - handles all block
//  io, from both the cache/marshaller and the disk
class boxm2_cache
{
  public:
    
    //: 
    virtual boxm2_block* get_block( 
  
  protected: 
    
    //: boxm2_disk_io_mgr (handles both sync and asynch io requests)
    boxm2_disk_io_mgr io_mgr; 
  
    //: b
}
