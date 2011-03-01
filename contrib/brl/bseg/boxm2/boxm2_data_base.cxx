#include "boxm2_data_base.h"
//:
// \file

//allocate an empty data diddy
boxm2_data_base::boxm2_data_base(boxm2_block_metadata data, const vcl_string data_type)
{
  id_ = data.id_;
  
#if 0 // variables not used
  const int MAX_BYTES    = data.max_mb_*1024*1024;
  const int BUFF_LENGTH  = 1<<16; // = 65536
#endif

  //determine number of cells to allocate - if random, then use the max_mb, otherwise use tree size
  // MAY want to create an initialize data that caters to size of trees, and doesn't assume only root
  long num_cells; 
  if(data.random_) {
    vcl_cout<<"DATA OF TYPE: "<<data_type<<" is initialized randomly!!!!!!"<<vcl_endl;
    num_cells = data.num_data_cells();
  }
  else {
    vcl_cout<<"DATA OF TYPE: "<<data_type<<" is initialized DETERMINISTACALLY!!!!!!"<<vcl_endl;
    num_cells = data.sub_block_num_.x() * data.sub_block_num_.y() * data.sub_block_num_.z(); 
    vcl_cout<<"NUmber of data cells: "<<num_cells<<vcl_endl;
  }
  
  vcl_size_t cell_size = boxm2_data_info::datasize(data_type);
  buffer_length_ = num_cells * cell_size;
  vcl_cout<<"Data size: "<<num_cells<<", bytes:"<<buffer_length_<<vcl_endl;

  //now construct a byte stream, and read in with b_read
  data_buffer_ = new char[buffer_length_];

  //initialize the data to the correct value
  if (data_type == boxm2_data_traits<BOXM2_ALPHA>::prefix()) {
    const float ALPHA_INIT = -vcl_log(1.0f - data.p_init_) / (data.sub_block_dim_.x());
    float* alphas = (float*) data_buffer_;
    int buffer_length = buffer_length_/sizeof(float);
    for (int i=0; i<buffer_length; ++i) alphas[i] = ALPHA_INIT;
  }
  else if (data_type == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() ||
           data_type == boxm2_data_traits<BOXM2_AUX>::prefix() ) {
    vcl_memset(data_buffer_, 0, buffer_length_);
  }
  else {
    vcl_memset(data_buffer_, 0, buffer_length_);
  }
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
