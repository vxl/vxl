#include "boxm2_block.h"
//:
// \file
#include <boxm2/boxm2_util.h>

boxm2_block::boxm2_block(boxm2_block_id id, char* buff)
{
  block_id_ = id;
  buffer_ = buff;
  this->b_read(buff);
}

boxm2_block::boxm2_block(boxm2_block_metadata data)
{
  block_id_ = data.id_;
  this->init_empty_block(data);
}

bool boxm2_block::b_read(char* buff)
{
    long bytes_read = 0;

    //0. first 8 bytes denote size
    vcl_memcpy(&byte_count_, buff, sizeof(byte_count_));
    bytes_read += sizeof(byte_count_);

    //1. read init level, max level, max mb
    vcl_memcpy(&init_level_, buff+bytes_read, sizeof(init_level_));
    bytes_read += sizeof(init_level_);
    vcl_memcpy(&max_level_, buff+bytes_read, sizeof(max_level_));
    bytes_read += sizeof(max_level_);
    vcl_memcpy(&max_mb_, buff+bytes_read, sizeof(max_mb_));
    bytes_read += sizeof(max_mb_);

    //2. read in sub block dimension, sub block num
    double dims[4];
    vcl_memcpy(&dims, buff+bytes_read, sizeof(dims));
    bytes_read += sizeof(dims);
    int    nums[4];
    vcl_memcpy(&nums, buff+bytes_read, sizeof(nums));
    bytes_read += sizeof(nums);
    sub_block_dim_ = vgl_vector_3d<double>(dims[0], dims[1], dims[2]);
    sub_block_num_ = vgl_vector_3d<unsigned>(nums[0], nums[1], nums[2]);

    //4. setup big arrays (3d block of trees)
    uchar16* treesBuff = (uchar16*) (buff+bytes_read);
    trees_     = boxm2_array_3d<uchar16>( sub_block_num_.x(),
                                          sub_block_num_.y(),
                                          sub_block_num_.z(),
                                          treesBuff);
    bytes_read += sizeof(uchar16)*sub_block_num_.x()*sub_block_num_.y()*sub_block_num_.z();
    return true;
}

//:
//  This type of writing is sort of counter intuitive, as the buffer
//  just needs to be returned and written to disk. The first few calls
//  ensure the meta data is lined up correctly.  To use this, just pass in
//  the boxm2_block buffer.
bool boxm2_block::b_write(char* buff)
{
    long bytes_written = 0;

    //0. writing total size
    vcl_memcpy(buff, &byte_count_, sizeof(byte_count_));
    bytes_written += sizeof(byte_count_);

    //1. write init level, max level, max mb
    vcl_memcpy(buff+bytes_written, &init_level_, sizeof(init_level_));
    bytes_written += sizeof(init_level_);
    vcl_memcpy(buff+bytes_written, &max_level_, sizeof(max_level_));
    bytes_written += sizeof(max_level_);
    vcl_memcpy(buff+bytes_written, &max_mb_, sizeof(max_mb_));
    bytes_written += sizeof(max_mb_);

    //2. Write sub block dimension, sub block num
    double dims[4] = {sub_block_dim_.x(), sub_block_dim_.y(), sub_block_dim_.z(), 0.0};
    vcl_memcpy(buff+bytes_written, dims, 4 * sizeof(double));
    bytes_written += 4 * sizeof(double);

    int nums[4] = {sub_block_num_.x(), sub_block_num_.y(), sub_block_num_.z(), 0 };
    vcl_memcpy(buff+bytes_written, nums, 4 * sizeof(int));
    bytes_written += 4 * sizeof(int);

    //the arrays themselves should be already in the char buffer, so no need to copy
    return true;
}


//: initializes empty scene given
// This method uses the max_mb parameter to determine how many data cells to
// allocate.  MAX_MB is assumed to include blocks, alpha, mog3, num_obs and 16 byte aux data
bool boxm2_block::init_empty_block(boxm2_block_metadata data)
{
  //calc max number of bytes, data buffer length, and alpha init (consts)
  const int MAX_BYTES    = int(data.max_mb_)*1024*1024;
  const int BUFF_LENGTH  = 1L<<16; // 65536

  //total number of (sub) blocks in the scene
  int total_blocks =  data.sub_block_num_.x()
                    * data.sub_block_num_.y()
                    * data.sub_block_num_.z();

  //to initialize
  int num_buffers, blocks_per_buffer;

  //only 1 buffer, blocks per buffer is all blocks
  num_buffers = 1;
  blocks_per_buffer = total_blocks;
  vcl_cout<<"Num buffers: "<<num_buffers
          <<" .. num_trees: "<<blocks_per_buffer<<vcl_endl;

  //now construct a byte stream, and read in with b_read
  byte_count_ = calc_byte_count(num_buffers, blocks_per_buffer, total_blocks);
  init_level_ = data.init_level_;
  max_level_  = data.max_level_;
  max_mb_     = int(data.max_mb_);
  buffer_ = new char[byte_count_];

  //get member variable metadata straight, then write to the buffer
  long bytes_read = 0;
  bytes_read += sizeof(byte_count_);   //0. first 8 bytes denote size
  bytes_read += sizeof(init_level_);   //1. read init level, max level, max mb
  bytes_read += sizeof(max_level_);
  bytes_read += sizeof(max_mb_);
  double dims[4]; int nums[4];
  bytes_read += sizeof(dims);          //2. read in sub block dimension, sub block num
  bytes_read += sizeof(nums);
  sub_block_dim_ = data.sub_block_dim_;
  sub_block_num_ = data.sub_block_num_;

  //4. setup big arrays (3d block of trees)
  uchar16* treesBuff = (uchar16*) (buffer_+bytes_read);
  trees_     = boxm2_array_3d<uchar16>( sub_block_num_.x(),
                                        sub_block_num_.y(),
                                        sub_block_num_.z(),
                                        treesBuff);
  bytes_read += sizeof(uchar16)*sub_block_num_.x()*sub_block_num_.y()*sub_block_num_.z();

  //--- Now initialize blocks and their pointers --------- ---------------------
  //6. initialize blocks in order
  int tree_index = 0;
  boxm2_array_3d<uchar16>::iterator iter;
  for (iter = trees_.begin(); iter != trees_.end(); ++iter, ++tree_index)
  {
    //data index is tree index at this point
    //store data index in bits [10, 11, 12, 13] ;
    uchar16 treeBlk( (unsigned char) 0 );
    treeBlk[10] = (tree_index) & 0xff;
    treeBlk[11] = (tree_index>>8)  & 0xff;
    treeBlk[12] = (tree_index>>16) & 0xff;
    treeBlk[13] = (tree_index>>24) & 0xff;
    for (int i=0; i<16; i++)
      (*iter)[i] = treeBlk[i];
  }
  
  return true;
}


//: Given number of buffers, number of trees in each buffer, and number of total trees (x*y*z number)
// \return size of byte stream
long boxm2_block::calc_byte_count(int num_buffers, int trees_per_buffer, int num_trees)
{
  long toReturn = num_trees * sizeof(uchar16) +                     //3d block pointers
                  //num_buffers*trees_per_buffer * sizeof(int) +     //tree pointers
                  //num_buffers*(sizeof(ushort) + sizeof(ushort2)) +  //blocks in buffers and mem ptrs
                  sizeof(long) +                      //this number
                  3*sizeof(int) +                     //init level, max level, max_mb
                  4*sizeof(double) +                  //dims
                  4*sizeof(int); // +                     //nums 
                  //sizeof(int) + sizeof(int);          //+ numBuffers, treeLen
  return toReturn;
}


//------------ I/O -------------------------------------------------------------
vcl_ostream& operator <<(vcl_ostream &s, boxm2_block& block)
{
  s << "Block ID=" << block.block_id() << vcl_endl;
  s << "Byte Count=" << block.byte_count() << vcl_endl;
  s << "Init level=" << block.init_level() << vcl_endl;
  s << "Max level=" << block.max_level() << vcl_endl;
  s << "Max MB=" << block.max_mb() << vcl_endl;
  s << "Sub Block Dim=" << block.sub_block_dim() << vcl_endl;
  s << "Sub Block Num=" << block.sub_block_num() << vcl_endl;
  return s;
}

//: Binary write boxm2_block to stream.
// DUMMY IMPLEMENTATION: does nothing!
void vsl_b_write(vsl_b_ostream&, boxm2_block_sptr const&) {}

//: Binary load boxm2_block from stream.
// DUMMY IMPLEMENTATION: does nothing!
void vsl_b_read(vsl_b_istream&, boxm2_block_sptr&) {}
//: Binary load boxm2_block from stream.
// DUMMY IMPLEMENTATION: does nothing!
void vsl_b_read(vsl_b_istream&, boxm2_block_sptr const&) {}

