#include "boxm2_data_base.h"
//:
// \file

//: allocate an empty data diddy
boxm2_data_base::boxm2_data_base(boxm2_block_metadata data, const vcl_string data_type, bool read_only)
{
  read_only_ = read_only;
  id_ = data.id_;

  //determine number of cells to allocate - if random, then use the max_mb, otherwise use tree size
  // MAY want to create an initialize data that caters to size of trees, and doesn't assume only root
  long num_cells = data.sub_block_num_.x() *
                   data.sub_block_num_.y() *
                   data.sub_block_num_.z();
  vcl_size_t cell_size = boxm2_data_info::datasize(data_type);
  buffer_length_ = num_cells * cell_size;
  vcl_cout<<"boxm2_data_base::empty "<<data_type<<" num cells: "
          <<num_cells<<'\n'
          <<"  number of bytes: "<<buffer_length_<<vcl_endl;

  //now construct a byte stream, and read in with b_read
  data_buffer_ = new char[buffer_length_];

  //initialize the data to the correct value
  if (data_type == boxm2_data_traits<BOXM2_ALPHA>::prefix()) {
    const float ALPHA_INIT = float(-vcl_log(1.0f - data.p_init_) / (data.sub_block_dim_.x()));
    float* alphas = (float*) data_buffer_;
    int buffer_length = (int)(buffer_length_/sizeof(float));
    for (int i=0; i<buffer_length; ++i) alphas[i] = ALPHA_INIT;
  }
  else   if (data_type == boxm2_data_traits<BOXM2_GAMMA>::prefix()) {
    const float GAMMA_INIT = float(-vcl_log(1.0f - data.p_init_) / (data.sub_block_dim_.x()*data.sub_block_dim_.x()*data.sub_block_dim_.x()));
    float* alphas = (float*) data_buffer_;
    int buffer_length = (int)(buffer_length_/sizeof(float));
    for (int i=0; i<buffer_length; ++i) alphas[i] = GAMMA_INIT;
  }
  else if (data_type == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() ||
           data_type == boxm2_data_traits<BOXM2_AUX>::prefix() ) {
    vcl_memset(data_buffer_, 0, buffer_length_);
  }
  else if (data_type == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) {
    vcl_memset(data_buffer_, (vxl_byte) 128, buffer_length_);
  }
  else {
    vcl_memset(data_buffer_, 0, buffer_length_);
  }
}
//: accessor to a portion of the byte buffer
void boxm2_data_base::set_default_value(vcl_string data_type)
{
  if (data_type == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() ||
           data_type == boxm2_data_traits<BOXM2_AUX>::prefix() ) {
    vcl_memset(data_buffer_, 0, buffer_length_);
  }
  else if (data_type == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) {
    vcl_memset(data_buffer_, (vxl_byte) 128, buffer_length_);
  }
  //default GAMMA value (not influenced by scene size or XML pinit)
  if (data_type == boxm2_data_traits<BOXM2_GAMMA>::prefix()) {
    float p_init = .01f; 
    vcl_cout<<"initializing data to "<<p_init<<vcl_endl;
    const float GAMMA_INIT = float(-vcl_log(1.0f - p_init) / (.025*.025*.025) );
    float* alphas = (float*) data_buffer_;
    int buffer_length = (int)(buffer_length_/sizeof(float));
    for (int i=0; i<buffer_length; ++i) 
      alphas[i] = GAMMA_INIT;
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
  } else return 0;
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
