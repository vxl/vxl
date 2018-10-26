#include <iostream>
#include <algorithm>
#include "bstm_data_base.h"
#include <vcl_compiler.h>
//:
// \file

void helper(const bstm_block_metadata& data, long& num_cells, double& side_len)
{
  //determine number of cells to allocate - based on init_level
  long init_cells_per_tree;
  //double side_len = data.sub_block_dim_.x();      //side length for initialized cell
  side_len = data.sub_block_dim_.x();      //side length for initialized cell
  if ( data.init_level_ == 1) {
    init_cells_per_tree = 1;
  }
  else if ( data.init_level_ == 2) {
    init_cells_per_tree = 1 + 8;
    side_len /= 2.0;
  }
  else if ( data.init_level_ == 3) {
    init_cells_per_tree = 1 + 8 + 64;
    side_len /= 4.0;
  }
  else if ( data.init_level_ == 4) {
    init_cells_per_tree = 1 + 8 + 64 + 512;
    side_len /= 8.0;
  }
  else
    init_cells_per_tree = 0; // dummy setting, to avoid compiler warning

  if(data.init_level_t_ == 1)
    init_cells_per_tree *= 1;
  else if(data.init_level_t_ == 6)
    init_cells_per_tree *= 32;
  else {
    std::cerr << "Init lvl " << data.init_level_t_ << " not supported. Only init level 1 or 6 is supported for time trees currently...\n";
    init_cells_per_tree = 0;
  }
  //total number of cells = numTrees * init_cells_per_tree
  num_cells = data.sub_block_num_.x() *
              data.sub_block_num_.y() *
              data.sub_block_num_.z() *
              data.sub_block_num_t_   * init_cells_per_tree;
}

//: allocate an empty data diddy
bstm_data_base::bstm_data_base(bstm_block_metadata data, const std::string data_type, bool read_only)
{
  read_only_ = read_only;
  id_ = data.id_;

  long num_cells;
  double side_len;
  helper(data, num_cells, side_len);

  std::size_t cell_size = bstm_data_info::datasize(data_type);
  //total buffer length
  buffer_length_ = num_cells * cell_size;
#if 0
  std::cout<<"bstm_data_base::empty "<<data_type<<" num cells: "
          <<num_cells<<'\n'
          <<"  number of bytes: "<<buffer_length_<<std::endl;
#endif

  //now construct a byte stream, and read in with b_read
  data_buffer_ = new char[buffer_length_];

  this->set_default_value(data_type, data);
}
//: accessor to a portion of the byte buffer
void bstm_data_base::set_default_value(std::string data_type, bstm_block_metadata data)
{
  long num_cells;
  double side_len;
  helper(data, num_cells, side_len);

  //initialize the data to the correct value
  if (data_type.find(bstm_data_traits<BSTM_ALPHA>::prefix()) != std::string::npos) {
    const float ALPHA_INIT = float(-std::log(1.0f - data.p_init_) / side_len);
    float* alphas = (float*) data_buffer_;
    std::fill(alphas, alphas+num_cells, ALPHA_INIT);
  }
//  else if (data_type.find(bstm_data_traits<BSTM_GAUSS_RGB>::prefix()) != std::string::npos) {
//    std::memset(data_buffer_, (vxl_byte) 128, buffer_length_);
//  }
  else if (data_type.find(bstm_data_traits<BSTM_CHANGE>::prefix()) != std::string::npos) {
    const float CHANGE_P_INIT = data.p_init_;
    float* change_p = (float*) data_buffer_;

    int buffer_length = (int)(buffer_length_/ bstm_data_traits<BSTM_CHANGE>::datasize() );
    for (int i=0; i<buffer_length; ++i) change_p[i] = CHANGE_P_INIT;
  }
  else {
    std::memset(data_buffer_, 0, buffer_length_);
  }
}

//: accessor to a portion of the byte buffer
char * bstm_data_base::cell_buffer(int i, std::size_t cell_size)
{
  if ((i+cell_size-1) < buffer_length_) {
    char * out = new char[cell_size];
    for (int j = 0; j < (int)cell_size; j++) {
      out[j] = data_buffer_[i+j];
    }
    return out;
  }
  else return nullptr;
}


//: Binary write bstm_data_base to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, bstm_data_base const&  /*scene*/) {}
//: Binary write bstm_data_base to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, const bstm_data_base* & /*p*/) {}
//: Binary write bstm_data_base_sptr to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, bstm_data_base_sptr&  /*sptr*/) {}
//: Binary write bstm_data_base_sptr to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, bstm_data_base_sptr const&  /*sptr*/) {}

//: Binary load bstm_data_base from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, bstm_data_base & /*scene*/) {}
//: Binary load bstm_data_base from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, bstm_data_base*  /*p*/) {}
//: Binary load bstm_data_base_sptr from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, bstm_data_base_sptr&  /*sptr*/) {}
//: Binary load bstm_data_base_sptr from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, bstm_data_base_sptr const&  /*sptr*/) {}
