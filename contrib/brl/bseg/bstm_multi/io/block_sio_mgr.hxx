#include <iostream>
#include <fstream>
#include <ios>
#include <string>
#include "block_sio_mgr.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>

template <typename Block>
Block *block_sio_mgr<Block>::load_block(const std::string &dir,
                                        const block_id &block_id,
                                        const block_metadata &data) {
  std::string filepath = dir + block_id.to_string() + ".bin";

  // get file size
  unsigned long numBytes = vul_file::size(filepath);

  // Read bytes into stream
  char *bytes = new char[numBytes];
  std::ifstream myFile(filepath.c_str(), std::ios::in | std::ios::binary);
  myFile.read(bytes, numBytes);
  if (!myFile) {
    // std::cerr<<"block_sio_mgr::load_block cannot read file
    // "<<filepath<<std::endl;
    return nullptr;
  }

  // instantiate new block
  return new Block(block_id, data, bytes);
}

// loads a generic bstm_data_base* from disk (given data_type string prefix)
template <typename Block>
block_data_base *block_sio_mgr<Block>::load_block_data_generic(
    const std::string &dir, const block_id &id, const std::string &data_type) {
  // file name
  std::string filename = dir + data_type + "_" + id.to_string() + ".bin";

  // get file size
  unsigned long numBytes = vul_file::size(filename);

  // Read bytes into stream
  char *bytes = new char[numBytes];
  std::ifstream myFile(filename.c_str(), std::ios::in | std::ios::binary);
  myFile.read(bytes, numBytes);
  if (!myFile) {
    // std::cerr<<"block_sio_mgr::load_data cannot read file
    // "<<filename<<std::endl;
    return nullptr;
  }

  // instantiate new block
  return new block_data_base(bytes, numBytes);
}

template <typename Block>
void block_sio_mgr<Block>::save_block(const std::string &dir, Block *block) {
  std::string filepath = dir + block->block_id().to_string() + ".bin";
  // std::cout<<"block_sio_mgr::write save to file: "<<filepath<<std::endl;
  char *bytes = block->buffer();
  block->b_write(bytes);

  // synchronously write to disk
  std::ofstream myFile(filepath.c_str(), std::ios::out | std::ios::binary);
  myFile.write(bytes, block->byte_count());
  myFile.close();
}

// generically saves data_base * to disk (given prefix)
template <typename Block>
void block_sio_mgr<Block>::save_block_data_base(const std::string &dir,
                                                const block_id &block_id,
                                                block_data_base *data,
                                                const std::string &prefix) {
  std::string filename = dir + prefix + "_" + block_id.to_string() + ".bin";

  std::ofstream myFile(filename.c_str(), std::ios::out | std::ios::binary);
  // only write if data length is non-zero (and data buffer is non-null)
  if (data->buffer_length() > 0) {
    char *bytes = data->data_buffer();
    myFile.write(bytes, data->buffer_length());
  }
  myFile.close();
  return;
}
