#include "bstm_time_block.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//:
// \file

bstm_time_block::bstm_time_block(const bstm_block_id& id, bstm_block_metadata data, char* buffer, std::size_t length)
{
  //data has already been read, save length
  byte_count_ = length;
  buffer_ = buffer;

  time_trees_ = boxm2_array_1d<uchar8>( length / sizeof(uchar8), (uchar8*) (buffer_));

  //this info is not so relevant (now) but save it anyway
  init_level_t_ = data.init_level_t_;
  max_level_t_  = data.max_level_t_;
  sub_block_num_t_ = data.sub_block_num_t_;
  block_id_ = id;

  read_only_ = false; // make sure that it is written back to disc
}

bstm_time_block::bstm_time_block(const bstm_block_id& id, bstm_block_metadata data, unsigned num_el)
{
  //this info is not so relevant (now) but save it anyway
  init_level_t_ = data.init_level_t_;
  max_level_t_  = data.max_level_t_;

  sub_block_num_t_ = data.sub_block_num_t_;
  block_id_ = id;

  //create data based on num_el
  byte_count_ = num_el * sizeof(uchar8) * sub_block_num_t_;
  buffer_ = new char[num_el * sizeof(uchar8) * sub_block_num_t_ ];

  time_trees_ = boxm2_array_1d<uchar8>( byte_count_ / sizeof(uchar8), (uchar8*) (buffer_));
  init_empty_block();

  read_only_ = false; // make sure that it is written back to disc
}


bstm_time_block::bstm_time_block(bstm_block_metadata data)
{
  init_level_t_ = data.init_level_t_;
  max_level_t_  = data.max_level_t_;
  sub_block_num_t_ = data.sub_block_num_t_;

  block_id_ = data.id_;

  byte_count_ = calc_byte_count(data);

  buffer_ = new char[byte_count_];
  //setup big arrays (1d block of trees)
  time_trees_     = boxm2_array_1d<uchar8>(byte_count_ / sizeof(uchar8) , (uchar8*) (buffer_));

  this->init_empty_block();
  read_only_ = false;  // make sure that it is written back to disc
}


//: initializes empty scene
bool bstm_time_block::init_empty_block()
{
  //--- Now initialize blocks and their pointers --------- ---------------------
  //6. initialize blocks in order
  int tree_index = 0;
  boxm2_array_1d<uchar8>::iterator iter;
  for (iter = time_trees_.begin(); iter != time_trees_.end(); ++iter)
  {
    //initialize empty tree
    uchar8 treeBlk( (unsigned char) 0 );

    //store root data index in bits [4,5,6,7];
    treeBlk[4] = (tree_index) & 0xff;
    treeBlk[5] = (tree_index>>8)  & 0xff;
    treeBlk[6] = (tree_index>>16) & 0xff;
    treeBlk[7] = (tree_index>>24) & 0xff;

    //Set Init_Level, 1=just root, 2=2 generations, 3=3 generations...etc.
    if (init_level_t_== 1) {
      treeBlk[0] = 0;
      ++tree_index;
    }
    else if (init_level_t_== 6) {
      for (int i=0; i<4; ++i)
        treeBlk[i] = 0xff;
      tree_index += 32;
    }
    else
      std::cerr << "Init lvl " << init_level_t_ << " not supported. Only init level 1 or 6 is supported for time trees currently...\n";

    //store this tree in the buffer
    for (int i=0; i<TT_NUM_BYTES; i++)
      (*iter)[i] = treeBlk[i];
  }
  return true;
}

//: given a data offset from bstm_block and a time instance, return the corresponding time tree
vnl_vector_fixed<unsigned char, 8>&  bstm_time_block::get_cell_tt(int cell_data_offset, double local_time)
{
  unsigned index = tree_index(local_time);
  return time_trees_[cell_data_offset*sub_block_num_t_ + index];
}

void  bstm_time_block::set_cell_tt(int cell_data_offset, const uchar8& input, double local_t)
{
  unsigned index = tree_index(local_t);
  time_trees_[cell_data_offset*sub_block_num_t_ + index] = input;
}

boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> >  bstm_time_block::get_cell_all_tt(int cell_data_offset)
{
  return boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> > (sub_block_num_t_, &(time_trees_[cell_data_offset*sub_block_num_t_ ]));
}

void bstm_time_block::set_cell_all_tt(int cell_data_offset, const boxm2_array_1d<uchar8>& input)
{
  assert(input.size() == sub_block_num_t_);
  for (unsigned i = 0; i < sub_block_num_t_; i++) //copy each tt
    time_trees_[cell_data_offset*sub_block_num_t_ + i] = input[i];
}

unsigned bstm_time_block::tree_index(double local_time)
{
  //compute the index of the time tree that contains local_time
  return (unsigned)std::floor(local_time);
}

void bstm_time_block::octree_num_cell_calc(bstm_block_metadata& data, long& num_cells)
{
  //determine number of cells to allocate - based on init_level
  long init_cells_per_tree;

  if ( data.init_level_ == 1) {
    init_cells_per_tree = 1;
  }
  else if ( data.init_level_ == 2) {
    init_cells_per_tree = 1 + 8;
  }
  else if ( data.init_level_ == 3) {
    init_cells_per_tree = 1 + 8 + 64;
  }
  else if ( data.init_level_ == 4) {
    init_cells_per_tree = 1 + 8 + 64 + 512;
  }
  else
    init_cells_per_tree = 0; // dummy setting, to avoid compiler warning

  //total number of cells = numTrees * init_cells_per_tree
  num_cells = data.sub_block_num_.x() *
              data.sub_block_num_.y() *
              data.sub_block_num_.z() * init_cells_per_tree;
}


// \return size of byte stream
long bstm_time_block::calc_byte_count(bstm_block_metadata& data)
{
  //get the number of octree cells expected in block
  long num_octree_cells;
  octree_num_cell_calc(data, num_octree_cells);

  return num_octree_cells * sub_block_num_t_ * sizeof(uchar8);
}


//------------ I/O -------------------------------------------------------------
std::ostream& operator <<(std::ostream &s, bstm_time_block& block)
{
  return
  s << "Block ID=" << block.block_id() << '\n'
    << "Byte Count=" << block.byte_count() << '\n'
    << "Init level=" << block.init_level() << '\n'
    << "Max level=" << block.max_level() << '\n'
    << "Read only=" << block.read_only() << '\n'
    << "Sub Block Num=" << block.sub_block_num()<< std::endl;
}

//: Binary write bstm_time_block to stream.
// DUMMY IMPLEMENTATION: does nothing!
void vsl_b_write(vsl_b_ostream&, bstm_time_block_sptr const&) {}

//: Binary load bstm_time_block from stream.
// DUMMY IMPLEMENTATION: does nothing!
void vsl_b_read(vsl_b_istream&, bstm_time_block_sptr&) {}
//: Binary load bstm_time_block from stream.
// DUMMY IMPLEMENTATION: does nothing!
void vsl_b_read(vsl_b_istream&, bstm_time_block_sptr const&) {}
