#include <iostream>
#include <fstream>
#include "boxm2_sio_mgr.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <sys/stat.h>  //for getting file sizes

#if defined(HAS_HDFS) && HAS_HDFS
#include <bhdfs/bhdfs_manager.h>
#include <bhdfs/bhdfs_fstream.h>
#endif

boxm2_block* boxm2_sio_mgr::load_block(const std::string& dir, const boxm2_block_id& block_id, BOXM2_IO_FS_TYPE fs_type)
{
  std::string filepath = dir + block_id.to_string() + ".bin";
  unsigned long numBytes = 0;
  char* bytes=nullptr;

  if (fs_type == LOCAL) {
    //get file size
    numBytes = vul_file::size(filepath);

    //Read bytes into stream
    bytes = new char[numBytes];
    std::ifstream myFile (filepath.c_str(), std::ios::in | std::ios::binary);
    if (!myFile) {
    //std::cerr<<"boxm2_sio_mgr::load_block cannot read file "<<filepath<<std::endl;
    return nullptr;
    }

    myFile.read(bytes, numBytes);
  }
  else if (fs_type == HDFS) {
    // lib bhdfs is needed for this case
#if defined(HAS_HDFS) && HAS_HDFS
    bytes = load_from_hdfs(filepath, numBytes);
    if (bytes == 0) {
      std::cerr << "boxm2_sio_mgr:: There is an error reading from HDFS!\n";
    return NULL;
    }
#else
    std::cerr << "boxm2_sio_mgr:: bhdfs is needed for HDFS file system!\n";
    return nullptr;
#endif
  }
  else {
    std::cerr << "boxm2_sio_mgr:: FileSystem -" << fs_type << " is not implemented, yet!\n";
    return nullptr;
  }
  //instantiate new block
  return new boxm2_block(block_id, bytes);
}

boxm2_block* boxm2_sio_mgr::load_block(const std::string& dir, const boxm2_block_id& block_id,const boxm2_block_metadata& data, BOXM2_IO_FS_TYPE fs_type)
{
  std::string filepath = dir + block_id.to_string() + ".bin";
  unsigned long numBytes = 0;
  char* bytes=nullptr;

  if (fs_type == LOCAL) {
    //get file size
    numBytes = vul_file::size(filepath);

    //Read bytes into stream
    bytes = new char[numBytes];
    std::ifstream myFile (filepath.c_str(), std::ios::in | std::ios::binary);
    if (!myFile) {
      //std::cerr<<"boxm2_sio_mgr::load_block cannot read file "<<filepath<<std::endl;
      return nullptr;
    }
    myFile.read(bytes, numBytes);
  }
  else if (fs_type == HDFS) {
    // lib bhdfs is needed for this case
#if defined(HAS_HDFS) && HAS_HDFS
    bytes = load_from_hdfs(filepath, numBytes);
    if (bytes == 0) {
      std::cerr << "boxm2_sio_mgr:: There is an error reading from HDFS!\n";
    return NULL;
    }
#else
    std::cerr << "boxm2_sio_mgr:: bhdfs is needed for HDFS file system!\n";
    return nullptr;
#endif
  }
  else {
    std::cerr << "boxm2_sio_mgr:: FileSystem -" << fs_type << " is not implemented, yet!\n";
    return nullptr;
  }
  //instantiate new block
  auto * returnboxm2_block = new boxm2_block(block_id,data, bytes);
  return returnboxm2_block;
}

void boxm2_sio_mgr::save_block(const std::string& dir, boxm2_block* block)
{
  std::string filepath = dir + block->block_id().to_string() + ".bin";
  //std::cout<<"boxm2_sio_mgr::write save to file: "<<filepath<<std::endl;
  char * bytes = block->buffer();
  block->b_write(bytes);

  // synchronously write to disk
  std::ofstream myFile (filepath.c_str(), std::ios::out | std::ios::binary);
  myFile.write(bytes, block->byte_count());
  myFile.close();
}

// loads a generic boxm2_data_base* from disk (given data_type string prefix)
boxm2_data_base* boxm2_sio_mgr::load_block_data_generic(const std::string& dir, const boxm2_block_id& id, const std::string& data_type, BOXM2_IO_FS_TYPE fs_type)
{
  // file name
  std::string filename = dir + data_type + "_" + id.to_string() + ".bin";
  unsigned long numBytes = 0;
  char* bytes=nullptr;
  if (fs_type == LOCAL) {
    //get file size
    numBytes=vul_file::size(filename);

    //Read bytes into stream
    bytes = new char[numBytes];
    std::ifstream myFile (filename.c_str(), std::ios::in | std::ios::binary);
    myFile.read(bytes, numBytes);
    if (!myFile) {
        //std::cerr<<"boxm2_sio_mgr::load_data cannot read file "<<filename<<std::endl;
        return nullptr;
    }
  }
  else if (fs_type == HDFS) {
    // lib bhdfs is needed for this case
#if defined(HAS_HDFS) && HAS_HDFS
    bytes = load_from_hdfs(filename, numBytes);
    if (bytes == 0) {
      std::cerr << "boxm2_sio_mgr:: There is an error reading from HDFS!\n";
    return NULL;
    }
#else
    std::cerr << "boxm2_sio_mgr:: bhdfs is needed for HDFS file system!\n";
    return nullptr;
#endif
  }
  else {
    std::cerr << "boxm2_sio_mgr:: FileSystem -" << fs_type << " is not implemented, yet!\n";
    return nullptr;
  }
  //instantiate new block
  return new boxm2_data_base(bytes,numBytes,id);
}

// generically saves data_base * to disk (given prefix)
void boxm2_sio_mgr::save_block_data_base(const std::string& dir, const boxm2_block_id& block_id, boxm2_data_base* data, const std::string& prefix)
{
  std::string filename = dir + prefix + "_" + block_id.to_string() + ".bin";

  char * bytes = data->data_buffer();
  std::ofstream myFile (filename.c_str(), std::ios::out | std::ios::binary);
  myFile.write(bytes, data->buffer_length());
  myFile.close();
  return;
}

char* boxm2_sio_mgr::load_from_hdfs(const std::string&  /*filepath*/, unsigned long & /*numBytes*/)
{
#if defined(HAS_HDFS) && HAS_HDFS
  if (!bhdfs_manager::exists())
    bhdfs_manager::create(std::string("default"),0);
  bhdfs_manager_sptr mgr = bhdfs_manager::instance();
  std::string cur_dir = mgr->get_working_dir();
  std::cout << "Working directory =" << cur_dir << std::endl;
  bhdfs_fstream_sptr fs = new bhdfs_fstream(filepath, "r");
  numBytes = fs->file_size();
  //std::cerr << "BInary file size=" << numBytes << std::endl;
  //std::cout << "number of bytes=" << numBytes;
  char* bytes =  new char[numBytes];
  fs->read(bytes, numBytes);
  return bytes;
#else
  return nullptr;
#endif
}
