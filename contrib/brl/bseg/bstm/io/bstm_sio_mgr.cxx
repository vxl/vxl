#include "bstm_sio_mgr.h"
//
#include <vcl_fstream.h>
#include <sys/stat.h>  //for getting file sizes
#include <vul/vul_file.h>

bstm_block* bstm_sio_mgr::load_block(vcl_string dir, bstm_block_id block_id, bstm_block_metadata data )
{
  vcl_string filepath = dir + block_id.to_string() + ".bin";

  //get file size
  unsigned long numBytes = vul_file::size(filepath);

  //Read bytes into stream
  char * bytes = new char[numBytes];
  vcl_ifstream myFile (filepath.c_str(), vcl_ios::in | vcl_ios::binary);
  myFile.read(bytes, numBytes);
  if (!myFile) {
    //vcl_cerr<<"bstm_sio_mgr::load_block cannot read file "<<filepath<<vcl_endl;
    return NULL;
  }

  //instantiate new block
  return new bstm_block(block_id,data, bytes);
}

bstm_time_block* bstm_sio_mgr::load_time_block(vcl_string dir, bstm_block_id block_id, bstm_block_metadata data )
{
  vcl_string filepath = dir + "tt_" + block_id.to_string() + ".bin";

  //get file size
  unsigned long numBytes = vul_file::size(filepath);

  //Read bytes into stream
  char * bytes = new char[numBytes];
  vcl_ifstream myFile (filepath.c_str(), vcl_ios::in | vcl_ios::binary);
  myFile.read(bytes, numBytes);
  if (!myFile) {
    //vcl_cerr<<"bstm_sio_mgr::load_block cannot read file "<<filepath<<vcl_endl;
    return NULL;
  }

  //instantiate new block
  return new bstm_time_block(block_id,data, bytes, numBytes);
}

// loads a generic bstm_data_base* from disk (given data_type string prefix)
bstm_data_base* bstm_sio_mgr::load_block_data_generic(vcl_string dir, bstm_block_id id, vcl_string data_type)
{
  // file name
  vcl_string filename = dir + data_type + "_" + id.to_string() + ".bin";

  //get file size
  unsigned long numBytes=vul_file::size(filename);

  //Read bytes into stream
  char * bytes = new char[numBytes];
  vcl_ifstream myFile (filename.c_str(), vcl_ios::in | vcl_ios::binary);
  myFile.read(bytes, numBytes);
  if (!myFile) {
      //vcl_cerr<<"bstm_sio_mgr::load_data cannot read file "<<filename<<vcl_endl;
      return NULL;
  }

  //instantiate new block
  return new bstm_data_base(bytes,numBytes,id);
}


void bstm_sio_mgr::save_block(vcl_string dir, bstm_block* block)
{
  vcl_string filepath = dir + block->block_id().to_string() + ".bin";
  //vcl_cout<<"bstm_sio_mgr::write save to file: "<<filepath<<vcl_endl;
  char * bytes = block->buffer();
  block->b_write(bytes);

  // synchronously write to disk
  vcl_ofstream myFile (filepath.c_str(), vcl_ios::out | vcl_ios::binary);
  myFile.write(bytes, block->byte_count());
  myFile.close();
}

void bstm_sio_mgr::save_time_block(vcl_string dir, bstm_time_block* block)
{
  vcl_string filepath = dir +  "tt_" + block->block_id().to_string() + ".bin";
  //vcl_cout<<"bstm_sio_mgr::write save to file: "<<filepath<<vcl_endl;
  char * bytes = block->buffer();

  // synchronously write to disk
  vcl_ofstream myFile (filepath.c_str(), vcl_ios::out | vcl_ios::binary);
  myFile.write(bytes, block->byte_count());
  myFile.close();
}

// generically saves data_base * to disk (given prefix)
void bstm_sio_mgr::save_block_data_base(vcl_string dir, bstm_block_id block_id, bstm_data_base* data, vcl_string prefix)
{
  vcl_string filename = dir + prefix + "_" + block_id.to_string() + ".bin";

  char * bytes = data->data_buffer();
  vcl_ofstream myFile (filename.c_str(), vcl_ios::out | vcl_ios::binary);
  myFile.write(bytes, data->buffer_length());
  myFile.close();
  return;
}


