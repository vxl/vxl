#include "boxm2_data_base.h"
#include <vcl_algorithm.h>
//:
// \file

void helper(boxm2_block_metadata& data, long& num_cells, double& side_len)
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

  //total number of cells = numTrees * init_cells_per_tree
  num_cells = data.sub_block_num_.x() *
              data.sub_block_num_.y() *
              data.sub_block_num_.z() * init_cells_per_tree;
}

//: allocate an empty data diddy
boxm2_data_base::boxm2_data_base(boxm2_block_metadata data, const vcl_string data_type, bool read_only)
{
  read_only_ = read_only;
  id_ = data.id_;

  long num_cells;
  double side_len;
  helper(data, num_cells, side_len);

  vcl_size_t cell_size = boxm2_data_info::datasize(data_type);
  //total buffer length
  buffer_length_ = num_cells * cell_size;
#if 0
  vcl_cout<<"boxm2_data_base::empty "<<data_type<<" num cells: "
          <<num_cells<<'\n'
          <<"  number of bytes: "<<buffer_length_<<vcl_endl;
#endif

  //now construct a byte stream, and read in with b_read
  data_buffer_ = new char[buffer_length_];

  this->set_default_value(data_type, data);
}
//: accessor to a portion of the byte buffer
void boxm2_data_base::set_default_value(vcl_string data_type, boxm2_block_metadata data)
{
  long num_cells;
  double side_len;
  helper(data, num_cells, side_len);

  //initialize the data to the correct value
  if (data_type.find(boxm2_data_traits<BOXM2_ALPHA>::prefix()) != vcl_string::npos) {
    const float ALPHA_INIT = float(-vcl_log(1.0f - data.p_init_) / side_len);
    float* alphas = (float*) data_buffer_;
    vcl_fill(alphas, alphas+num_cells, ALPHA_INIT);
  }
  else if (data_type.find(boxm2_data_traits<BOXM2_GAMMA>::prefix()) != vcl_string::npos) {
    const float GAMMA_INIT = float(-vcl_log(1.0f - data.p_init_) / (side_len*side_len*side_len));
    float* alphas = (float*) data_buffer_;
    int buffer_length = (int)(buffer_length_/sizeof(float));
    for (int i=0; i<buffer_length; ++i) alphas[i] = GAMMA_INIT;
  }
  else if ( data_type.find(boxm2_data_traits<BOXM2_NUM_OBS>::prefix()) != vcl_string::npos ||
            data_type.find(boxm2_data_traits<BOXM2_AUX>::prefix()) != vcl_string::npos ) {
    vcl_memset(data_buffer_, 0, buffer_length_);
  }
  else if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) != vcl_string::npos) {
    vcl_memset(data_buffer_, (vxl_byte) 128, buffer_length_);
  }
  else if ( data_type.find(boxm2_data_traits<BOXM2_FLOAT8>::prefix()) != vcl_string::npos ) {
      float* floats = (float*) data_buffer_;
      int buffer_length = (int)(buffer_length_/sizeof(float));
      for (int i=0; i<buffer_length; ++i) floats[i] = 0.0;
  }
  else if ( data_type.find(boxm2_data_traits<BOXM2_FLOAT16>::prefix()) != vcl_string::npos ) {
      float* floats = (float*) data_buffer_;
      int buffer_length = (int)(buffer_length_/sizeof(float));
      for (int i=0; i<buffer_length; ++i) floats[i] = 0.0;
  }
  else if (data_type.find(boxm2_data_traits<BOXM2_VIS_SPHERE>::prefix()) != vcl_string::npos ) {
        float* floats = (float*) data_buffer_;
        int buffer_length = (int)(buffer_length_/sizeof(float));
        for (int i=0; i<buffer_length; ++i) floats[i] = 1.0f;
    }
  else {
    vcl_memset(data_buffer_, 0, buffer_length_);
  }
}

//: accessor to a portion of the byte buffer
char * boxm2_data_base::cell_buffer(int i, vcl_size_t cell_size)
{
  if ((i+cell_size-1) < buffer_length_) {
    char * out = new char[cell_size];
    for (int j = 0; j < (int)cell_size; j++) {
      out[j] = data_buffer_[i+j];
    }
    return out;
  }
  else return 0;
}


//: Binary write boxm2_data_base to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_data_base const& scene) {}
//: Binary write boxm2_data_base to stream
void vsl_b_write(vsl_b_ostream& os, const boxm2_data_base* &p) {}
//: Binary write boxm2_data_base_sptr to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_data_base_sptr& sptr) {}
//: Binary write boxm2_data_base_sptr to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_data_base_sptr const& sptr) {}

//: Binary load boxm2_data_base from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_data_base &scene) {}
//: Binary load boxm2_data_base from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_data_base* p) {}
//: Binary load boxm2_data_base_sptr from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_data_base_sptr& sptr) {}
//: Binary load boxm2_data_base_sptr from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_data_base_sptr const& sptr) {}
