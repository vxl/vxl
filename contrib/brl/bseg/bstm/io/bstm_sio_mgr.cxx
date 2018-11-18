#include <iostream>
#include <fstream>
#include "bstm_sio_mgr.h"
//
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <sys/stat.h>  //for getting file sizes
#include <vul/vul_file.h>

bstm_block* bstm_sio_mgr::load_block(const std::string& dir, const bstm_block_id& block_id, const bstm_block_metadata& data )
{
  std::string filepath = dir + block_id.to_string() + ".bin";

  //get file size
  unsigned long numBytes = vul_file::size(filepath);

  //Read bytes into stream
  char * bytes = new char[numBytes];
  std::ifstream myFile (filepath.c_str(), std::ios::in | std::ios::binary);
  myFile.read(bytes, numBytes);
  if (!myFile) {
    //std::cerr<<"bstm_sio_mgr::load_block cannot read file "<<filepath<<std::endl;
    return nullptr;
  }

  //instantiate new block
  return new bstm_block(block_id,data, bytes);
}

bstm_time_block* bstm_sio_mgr::load_time_block(const std::string& dir, const bstm_block_id& block_id, const bstm_block_metadata& data )
{
  std::string filepath = dir + "tt_" + block_id.to_string() + ".bin";

  //get file size
  unsigned long numBytes = vul_file::size(filepath);

  //Read bytes into stream
  char * bytes = new char[numBytes];
  std::ifstream myFile (filepath.c_str(), std::ios::in | std::ios::binary);
  myFile.read(bytes, numBytes);
  if (!myFile) {
    //std::cerr<<"bstm_sio_mgr::load_block cannot read file "<<filepath<<std::endl;
    return nullptr;
  }

  //instantiate new block
  return new bstm_time_block(block_id,data, bytes, numBytes);
}

// loads a generic bstm_data_base* from disk (given data_type string prefix)
bstm_data_base* bstm_sio_mgr::load_block_data_generic(const std::string& dir, const bstm_block_id& id, const std::string& data_type)
{
  // file name
  std::string filename = dir + data_type + "_" + id.to_string() + ".bin";

  //get file size
  unsigned long numBytes=vul_file::size(filename);

  //Read bytes into stream
  char * bytes = new char[numBytes];
  std::ifstream myFile (filename.c_str(), std::ios::in | std::ios::binary);
  myFile.read(bytes, numBytes);
  if (!myFile) {
      //std::cerr<<"bstm_sio_mgr::load_data cannot read file "<<filename<<std::endl;
      return nullptr;
  }

  //instantiate new block
  return new bstm_data_base(bytes,numBytes,id);
}


void bstm_sio_mgr::save_block(const std::string& dir, bstm_block* block)
{
  std::string filepath = dir + block->block_id().to_string() + ".bin";
  //std::cout<<"bstm_sio_mgr::write save to file: "<<filepath<<std::endl;
  char * bytes = block->buffer();
  block->b_write(bytes);

  // synchronously write to disk
  std::ofstream myFile (filepath.c_str(), std::ios::out | std::ios::binary);
  myFile.write(bytes, block->byte_count());
  myFile.close();
}

void bstm_sio_mgr::save_time_block(const std::string& dir, bstm_time_block* block)
{
  std::string filepath = dir +  "tt_" + block->block_id().to_string() + ".bin";
  //std::cout<<"bstm_sio_mgr::write save to file: "<<filepath<<std::endl;
  char * bytes = block->buffer();

  // synchronously write to disk
  std::ofstream myFile (filepath.c_str(), std::ios::out | std::ios::binary);
  myFile.write(bytes, block->byte_count());
  myFile.close();
}

// generically saves data_base * to disk (given prefix)
void bstm_sio_mgr::save_block_data_base(const std::string& dir, const bstm_block_id& block_id, bstm_data_base* data, const std::string& prefix)
{
  std::string filename = dir + prefix + "_" + block_id.to_string() + ".bin";

  char * bytes = data->data_buffer();
  std::ofstream myFile (filename.c_str(), std::ios::out | std::ios::binary);
  myFile.write(bytes, data->buffer_length());
  myFile.close();
  return;
}
