#include "boxm2_sio_mgr.h"
#include <vcl_fstream.h>
#include <sys/stat.h>  //for getting file sizes

boxm2_block* boxm2_sio_mgr::load_block(vcl_string dir, boxm2_block_id block_id)
{
  vcl_string filepath = dir + block_id.to_string() + ".bin";
  vcl_cout<<"boxm2_sio_mgr:: load from file:"<<filepath<<vcl_endl;

  //get file size
  unsigned long numBytes = vul_file::size(filepath);

  //Read bytes into stream
  char * bytes = new char[numBytes];
  vcl_ifstream myFile (filepath.c_str(), vcl_ios::in | vcl_ios::binary);
  myFile.read(bytes, numBytes);
  if (!myFile) {
    vcl_cerr<<"boxm2_sio_mgr:: cannot read file "<<filepath<<vcl_endl;
    return NULL; 
  }

  //instantiate new block
  return new boxm2_block(block_id, bytes);
}


void boxm2_sio_mgr::save_block(vcl_string dir, boxm2_block* block)
{
  vcl_string filepath = dir + block->block_id().to_string() + ".bin";
  //vcl_cout<<"boxm2_sio_mgr::write save to file: "<<filepath<<vcl_endl;
  char * bytes = block->buffer();
  block->b_write(bytes);

  // synchronously write to disk
  vcl_ofstream myFile (filepath.c_str(), vcl_ios::out | vcl_ios::binary);
  myFile.write(bytes, block->byte_count());
  myFile.close();
}


boxm2_data_base* boxm2_sio_mgr::load_block_data_generic(vcl_string dir, boxm2_block_id id, vcl_string data_type)
{
  // file name
  vcl_string filename = dir + data_type + "_" + id.to_string() + ".bin";

  //get file size
  unsigned long numBytes=vul_file::size(filename);

  //Read bytes into stream
  char * bytes = new char[numBytes]; 
  vcl_ifstream myFile (filename.c_str(), vcl_ios::in | vcl_ios::binary);
  myFile.read(bytes, numBytes);
  if (!myFile)
      vcl_cerr<<"boxm2_sio_mgr:: cannot read file "<<dir<<vcl_endl;

  //instantiate new block
  return new boxm2_data_base(bytes,numBytes,id); 
}
