#include "boxm2_sio_mgr.h"
#include <vcl_fstream.h>
#include <sys/stat.h>  //for getting file sizes

#if defined(HAS_HDFS) && HAS_HDFS
#include <bhdfs/bhdfs_manager.h>
#include <bhdfs/bhdfs_fstream.h>
#endif

boxm2_block* boxm2_sio_mgr::load_block(vcl_string dir, boxm2_block_id block_id, BOXM2_IO_FS_TYPE fs_type)
{
  vcl_string filepath = dir + block_id.to_string() + ".bin";
  unsigned long numBytes = 0;
  char* bytes=0;

  if (fs_type == LOCAL) {
    //get file size
    numBytes = vul_file::size(filepath);

    //Read bytes into stream
    bytes = new char[numBytes];
    vcl_ifstream myFile (filepath.c_str(), vcl_ios::in | vcl_ios::binary);
    myFile.read(bytes, numBytes);
    if (!myFile) {
    //vcl_cerr<<"boxm2_sio_mgr::load_block cannot read file "<<filepath<<vcl_endl;
    return NULL;
    }
  }
  else if (fs_type == HDFS) {
    // lib bhdfs is needed for this case
#if defined(HAS_HDFS) && HAS_HDFS
    bytes = load_from_hdfs(filepath, numBytes);
    if (bytes == 0) {
      vcl_cerr << "boxm2_sio_mgr:: There is an error reading from HDFS!\n";
    return NULL;
    }
#else
    vcl_cerr << "boxm2_sio_mgr:: bhdfs is needed for HDFS file system!\n";
    return NULL;
#endif
  }
  else {
    vcl_cerr << "boxm2_sio_mgr:: FileSystem -" << fs_type << " is not implemented, yet!\n";
    return NULL;
  }
  //instantiate new block
  return new boxm2_block(block_id, bytes);
}

boxm2_block* boxm2_sio_mgr::load_block(vcl_string dir, boxm2_block_id block_id,boxm2_block_metadata data, BOXM2_IO_FS_TYPE fs_type)
{
  vcl_string filepath = dir + block_id.to_string() + ".bin";
  unsigned long numBytes = 0;
  char* bytes=0;

  if (fs_type == LOCAL) {
    //get file size
    numBytes = vul_file::size(filepath);

    //Read bytes into stream
    bytes = new char[numBytes];
    vcl_ifstream myFile (filepath.c_str(), vcl_ios::in | vcl_ios::binary);
    myFile.read(bytes, numBytes);
    if (!myFile) {
      //vcl_cerr<<"boxm2_sio_mgr::load_block cannot read file "<<filepath<<vcl_endl;
      return NULL;
    }
  }
  else if (fs_type == HDFS) {
    // lib bhdfs is needed for this case
#if defined(HAS_HDFS) && HAS_HDFS
    bytes = load_from_hdfs(filepath, numBytes);
    if (bytes == 0) {
      vcl_cerr << "boxm2_sio_mgr:: There is an error reading from HDFS!\n";
    return NULL;
    }
#else
    vcl_cerr << "boxm2_sio_mgr:: bhdfs is needed for HDFS file system!\n";
    return NULL;
#endif
  }
  else {
    vcl_cerr << "boxm2_sio_mgr:: FileSystem -" << fs_type << " is not implemented, yet!\n";
    return NULL;
  }
  //instantiate new block
  return new boxm2_block(block_id,data, bytes);
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

// loads a generic boxm2_data_base* from disk (given data_type string prefix)
boxm2_data_base* boxm2_sio_mgr::load_block_data_generic(vcl_string dir, boxm2_block_id id, vcl_string data_type, BOXM2_IO_FS_TYPE fs_type)
{
  // file name
  vcl_string filename = dir + data_type + "_" + id.to_string() + ".bin";
  unsigned long numBytes = 0;
  char* bytes=0;
  if (fs_type == LOCAL) {
    //get file size
    numBytes=vul_file::size(filename);

    //Read bytes into stream
    bytes = new char[numBytes];
    vcl_ifstream myFile (filename.c_str(), vcl_ios::in | vcl_ios::binary);
    myFile.read(bytes, numBytes);
    if (!myFile) {
        //vcl_cerr<<"boxm2_sio_mgr::load_data cannot read file "<<filename<<vcl_endl;
        return NULL;
    }
  }
  else if (fs_type == HDFS) {
    // lib bhdfs is needed for this case
#if defined(HAS_HDFS) && HAS_HDFS
    bytes = load_from_hdfs(filename, numBytes);
    if (bytes == 0) {
      vcl_cerr << "boxm2_sio_mgr:: There is an error reading from HDFS!\n";
    return NULL;
    }
#else
    vcl_cerr << "boxm2_sio_mgr:: bhdfs is needed for HDFS file system!\n";
    return NULL;
#endif
  }
  else {
    vcl_cerr << "boxm2_sio_mgr:: FileSystem -" << fs_type << " is not implemented, yet!\n";
    return NULL;
  }
  //instantiate new block
  return new boxm2_data_base(bytes,numBytes,id);
}

// generically saves data_base * to disk (given prefix)
void boxm2_sio_mgr::save_block_data_base(vcl_string dir, boxm2_block_id block_id, boxm2_data_base* data, vcl_string prefix)
{
  vcl_string filename = dir + prefix + "_" + block_id.to_string() + ".bin";

  char * bytes = data->data_buffer();
  vcl_ofstream myFile (filename.c_str(), vcl_ios::out | vcl_ios::binary);
  myFile.write(bytes, data->buffer_length());
  myFile.close();
  return;
}

char* boxm2_sio_mgr::load_from_hdfs(vcl_string filepath, unsigned long &numBytes)
{
#if defined(HAS_HDFS) && HAS_HDFS
  if (!bhdfs_manager::exists())
    bhdfs_manager::create(vcl_string("default"),0);
  bhdfs_manager_sptr mgr = bhdfs_manager::instance();
  vcl_string cur_dir = mgr->get_working_dir();
  vcl_cout << "Working directory =" << cur_dir << vcl_endl;
  bhdfs_fstream_sptr fs = new bhdfs_fstream(filepath, "r");
  numBytes = fs->file_size();
  //vcl_cerr << "BInary file size=" << numBytes << vcl_endl;
  //vcl_cout << "number of bytes=" << numBytes;
  char* bytes =  new char[numBytes];
  fs->read(bytes, numBytes);
  return bytes;
#else
  return NULL;
#endif
}
