#include "bstm_block.h"
//:
// \file

bstm_block::bstm_block(const bstm_block_id& id, bstm_block_metadata data, char* buffer)
{
  init_level_ = data.init_level_;
  max_level_  = data.max_level_;
  max_mb_     = int(data.max_mb_);
  sub_block_dim_ = data.sub_block_dim_;
  sub_block_num_ = data.sub_block_num_;

  block_id_ = id;
  buffer_ = buffer;
  this->b_read(buffer_);
  read_only_ = false; // make sure that it is written back to disc
}

bstm_block::bstm_block(bstm_block_metadata data)
{
  block_id_ = data.id_;
  this->init_empty_block(data);
  read_only_ = false;  // make sure that it is written back to disc
}

bool bstm_block::b_read(char* buff)
{
  auto* treesBuff = (uchar16*) (buff);
  byte_count_ = sizeof(uchar16)* sub_block_num_.x()*sub_block_num_.y()*sub_block_num_.z();
  trees_     = boxm2_array_3d<uchar16>( sub_block_num_.x(),
                                        sub_block_num_.y(),
                                        sub_block_num_.z(),
                                        treesBuff);
  return true;
}

//:
//  This type of writing is sort of counter intuitive, as the buffer
//  just needs to be returned and written to disk. The first few calls
//  ensure the meta data is lined up correctly.  To use this, just pass in
//  the bstm_block buffer.
bool bstm_block::b_write(char*  /*buff*/)
{
  return true;
}


//: initializes empty scene given
// This method uses the max_mb parameter to determine how many data cells to
// allocate.  MAX_MB is assumed to include blocks, alpha, mog3, num_obs and 16 byte aux data
bool bstm_block::init_empty_block(bstm_block_metadata data)
{
  //total number of (sub) blocks in the scene
  int total_blocks =  data.sub_block_num_.x()
                    * data.sub_block_num_.y()
                    * data.sub_block_num_.z();

  //to initialize
  int num_buffers, blocks_per_buffer;

  //only 1 buffer, blocks per buffer is all blocks
  num_buffers = 1;
  blocks_per_buffer = total_blocks;
  std::cout<<"Num buffers: "<<num_buffers
          <<" .. num_trees: "<<blocks_per_buffer<<std::endl;

  //now construct a byte stream, and read in with b_read
  byte_count_ = calc_byte_count(num_buffers, blocks_per_buffer, total_blocks);
  init_level_ = data.init_level_;
  max_level_  = data.max_level_;
  max_mb_     = int(data.max_mb_);
  buffer_ = new char[byte_count_];

  //get member variable metadata straight, then write to the buffer
  long bytes_read = 0;

  sub_block_dim_ = data.sub_block_dim_;
  sub_block_num_ = data.sub_block_num_;

  //4. setup big arrays (3d block of trees)
  auto* treesBuff = (uchar16*) (buffer_+bytes_read);
  trees_     = boxm2_array_3d<uchar16>( sub_block_num_.x(),
                                        sub_block_num_.y(),
                                        sub_block_num_.z(),
                                        treesBuff);

  //--- Now initialize blocks and their pointers --------- ---------------------
  //6. initialize blocks in order
  int tree_index = 0;
  boxm2_array_3d<uchar16>::iterator iter;
  for (iter = trees_.begin(); iter != trees_.end(); ++iter)
  {
    //initialize empty tree
    uchar16 treeBlk( (unsigned char) 0 );

    //the tree_index must be scaled with the number of time trees
    //int tree_index_scaled = tree_index * sub_block_num_t_;

    //store root data index in bits [10, 11, 12, 13] ;
    treeBlk[10] = (tree_index) & 0xff;
    treeBlk[11] = (tree_index>>8)  & 0xff;
    treeBlk[12] = (tree_index>>16) & 0xff;
    treeBlk[13] = (tree_index>>24) & 0xff;

    //Set Init_Level, 1=just root, 2=2 generations, 3=3 generations, 4=all four
    if (init_level_== 1) {
      treeBlk[0] = 0;
      tree_index += 1;
    }
    else if (init_level_ == 2){
      treeBlk[0] = 1;
      tree_index += 9;                //root + 1st
    }
    else if (init_level_ == 3) {
      treeBlk[0] = 1;
      treeBlk[1] = 0xff;
      tree_index += 1 + 8 + 64;       //root + 1st + 2nd
    }
    else if (init_level_ == 4) {
      treeBlk[0] = 1;
      for (int i=1; i<1+8; ++i)
        treeBlk[i] = 0xff;
      tree_index += 1 + 8 + 64 + 512; // root + 1st + 2nd + 3rd...
    }

    //store this tree in block bytes
    for (int i=0; i<16; i++)
      (*iter)[i] = treeBlk[i];
  }
  return true;
}


//: Given number of buffers, number of trees in each buffer, and number of total trees (x*y*z number)
// \return size of byte stream
long bstm_block::calc_byte_count(int  /*num_buffers*/, int  /*trees_per_buffer*/, int num_trees)
{
  long toReturn = num_trees * sizeof(uchar16) ;
  return toReturn;
}


//------------ I/O -------------------------------------------------------------
std::ostream& operator <<(std::ostream &s, bstm_block& block)
{
  return
  s << "Block ID=" << block.block_id() << '\n'
    << "Byte Count=" << block.byte_count() << '\n'
    << "Init level=" << block.init_level() << '\n'
    << "Max level=" << block.max_level() << '\n'
    << "Max MB=" << block.max_mb() << '\n'
    << "Read only=" << block.read_only() << '\n'
    << "Sub Block Dim=" << block.sub_block_dim() << '\n'
    << "Sub Block Num=" << block.sub_block_num() << std::endl;
}

//: Binary write bstm_block to stream.
// DUMMY IMPLEMENTATION: does nothing!
void vsl_b_write(vsl_b_ostream&, bstm_block_sptr const&) {}

//: Binary load bstm_block from stream.
// DUMMY IMPLEMENTATION: does nothing!
void vsl_b_read(vsl_b_istream&, bstm_block_sptr&) {}
//: Binary load bstm_block from stream.
// DUMMY IMPLEMENTATION: does nothing!
void vsl_b_read(vsl_b_istream&, bstm_block_sptr const&) {}
