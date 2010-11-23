#include "boxm2_asio_mgr.h"
//:
// \file

//: creates a BAIO object that loads/saves block data from disk
void boxm2_asio_mgr::load_block(vcl_string dir, boxm2_block_id block_id)
{
  vcl_string filepath = dir + block_id.to_string() + ".bin";
  vcl_cout<<"boxm2_asio_mgr:: load requested from file:"<<filepath<<vcl_endl;

  //get file size
  unsigned long numBytes = vul_file::size(filepath);

  //read bytes asynchronously, store aio object in aio list
  char * bytes = new char[numBytes];
  baio* aio = new baio();
  aio->read(filepath, bytes, numBytes);
  load_list_[block_id] = aio;
}

//: method of saving block
void boxm2_asio_mgr::save_block(vcl_string dir, boxm2_block* block)
{
  vcl_string filepath = dir + block->block_id().to_string() + ".bin";
  vcl_cout<<"boxm2_asio_mgr::write save to file: "<<filepath<<vcl_endl;

  // get block id
  boxm2_block_id id = block->block_id();

  // make sure bytes are written to the buffer
  char * bytes = block->buffer();
  block->b_write(bytes);

  // async write to disk
  baio* aio = new baio();
  aio->write(filepath, bytes, block->byte_count());
  save_list_[id] = aio;

  // TODO go through save list and find completed requests
  //    1. close file
  //    2. delete aio objects
}


//: returns a map of pointers to block pointers
vcl_map<boxm2_block_id, boxm2_block*> boxm2_asio_mgr::get_loaded_blocks()
{
  vcl_map<boxm2_block_id, boxm2_block*> toReturn;
  vcl_map<boxm2_block_id, baio*>::iterator iter;
  for (iter=load_list_.begin(); iter!=load_list_.end(); ++iter)
  {
    // get baio object and block id
    baio*           aio = (*iter).second;
    boxm2_block_id  id  = (*iter).first;

    if ( aio->status() == BAIO_FINISHED )
    {
      // close baio file
      aio->close_file();

      // instantiate new block
      boxm2_block*  blk = new boxm2_block(id, aio->buffer());
      toReturn[id] = blk;

      // remove iter from the load list/delete aio
      load_list_.erase(iter);
      delete aio;
    }
  }
  return toReturn;
}
