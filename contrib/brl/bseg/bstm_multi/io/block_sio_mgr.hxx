#include "block_sio_mgr.h"

#include <vcl_fstream.h>
#include <vcl_ios.h>
#include <vcl_string.h>
#include <vul/vul_file.h>

template <typename Block>
Block *block_sio_mgr<Block>::load_block(const vcl_string &dir,
                                        const block_id &block_id,
                                        const block_metadata &data) {
  vcl_string filepath = dir + block_id.to_string() + ".bin";

  // get file size
  unsigned long numBytes = vul_file::size(filepath);

  // Read bytes into stream
  char *bytes = new char[numBytes];
  vcl_ifstream myFile(filepath.c_str(), vcl_ios::in | vcl_ios::binary);
  myFile.read(bytes, numBytes);
  if (!myFile) {
    // vcl_cerr<<"block_sio_mgr::load_block cannot read file
    // "<<filepath<<vcl_endl;
    return VXL_NULLPTR;
  }

  // instantiate new block
  return new Block(block_id, data, bytes);
}

// loads a generic bstm_data_base* from disk (given data_type string prefix)
template <typename Block>
block_data_base *block_sio_mgr<Block>::load_block_data_generic(
    const vcl_string &dir, const block_id &id, const vcl_string &data_type) {
  // file name
  vcl_string filename = dir + data_type + "_" + id.to_string() + ".bin";

  // get file size
  unsigned long numBytes = vul_file::size(filename);

  // Read bytes into stream
  char *bytes = new char[numBytes];
  vcl_ifstream myFile(filename.c_str(), vcl_ios::in | vcl_ios::binary);
  myFile.read(bytes, numBytes);
  if (!myFile) {
    // vcl_cerr<<"block_sio_mgr::load_data cannot read file
    // "<<filename<<vcl_endl;
    return VXL_NULLPTR;
  }

  // instantiate new block
  return new block_data_base(bytes, numBytes);
}

template <typename Block>
void block_sio_mgr<Block>::save_block(const vcl_string &dir, Block *block) {
  vcl_string filepath = dir + block->block_id().to_string() + ".bin";
  // vcl_cout<<"block_sio_mgr::write save to file: "<<filepath<<vcl_endl;
  char *bytes = block->buffer();
  block->b_write(bytes);

  // synchronously write to disk
  vcl_ofstream myFile(filepath.c_str(), vcl_ios::out | vcl_ios::binary);
  myFile.write(bytes, block->byte_count());
  myFile.close();
}

// generically saves data_base * to disk (given prefix)
template <typename Block>
void block_sio_mgr<Block>::save_block_data_base(const vcl_string &dir,
                                                const block_id &block_id,
                                                block_data_base *data,
                                                const vcl_string &prefix) {
  vcl_string filename = dir + prefix + "_" + block_id.to_string() + ".bin";

  vcl_ofstream myFile(filename.c_str(), vcl_ios::out | vcl_ios::binary);
  // only write if data length is non-zero (and data buffer is non-null)
  if (data->buffer_length() > 0) {
    char *bytes = data->data_buffer();
    myFile.write(bytes, data->buffer_length());
  }
  myFile.close();
  return;
}
