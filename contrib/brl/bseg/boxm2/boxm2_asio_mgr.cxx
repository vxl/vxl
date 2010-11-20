#include "boxm2_asio_mgr.h"
#include "boxm2_block_id.h"
#include <sys/stat.h>

// creates a BAIO object that loads/saves block data from disk
void boxm2_asio_mgr::load_block(vcl_string dir, boxm2_block_id block_id)
{
  vcl_string filepath = dir + block_id.to_string() + ".bin";
  vcl_cout<<"boxm2_asio_mgr:: load requested from file:"<<filepath<<vcl_endl;

  //get file size
  long numBytes;
  struct stat results;
  if (stat(filepath.c_str(), &results) == 0)
    numBytes = results.st_size;
  else
    vcl_cerr<<"boxm2_asio_mgr:: cannot get file size for "<<dir<<vcl_endl;

  //read bytes asynchronously, store aio object in aio list
  char * bytes = new char[numBytes];
  baio aio; aio.read(filepath, bytes, numBytes);
  load_list_[block_id] = aio;
}

// method of saving block
void boxm2_asio_mgr::save_block(vcl_string dir, boxm2_block* block)
{
}

// returns loaded blocks
vcl_map<boxm2_block_id, boxm2_block*> get_loaded_blocks();

