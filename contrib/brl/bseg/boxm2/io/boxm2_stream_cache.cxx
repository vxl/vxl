#include "boxm2_stream_cache.h"
//:
// \file

boxm2_stream_cache_helper::~boxm2_stream_cache_helper()
{
  if (buf_) delete buf_;
  if (ifs_) ifs_.close();
}

bool boxm2_stream_cache_helper::open_file(vcl_string filename)
{
  ifs_.clear();
  ifs_.open(filename.c_str(), vcl_ios::in | vcl_ios::binary);
  if (!ifs_) return false;
  if (buf_) { delete buf_; buf_ = 0; }
  return true;
}

void boxm2_stream_cache_helper::read(unsigned long size, boxm2_block_id id)
{
  char * bytes = new char[size];
  ifs_.read(bytes, size);
  int cnt = (int)ifs_.gcount();
  if (buf_)
    delete buf_;
  buf_ = new boxm2_data_base(bytes,cnt,id);
}

void boxm2_stream_cache_helper::close_file()
{
  ifs_.close();
  if (buf_) { delete buf_; buf_ = 0; }
  index_ = -1;
}

//: return the byte buffer that contains ith cell, i is with respect to the global file
char *boxm2_stream_cache_helper::get_cell(int i, vcl_size_t cell_size, boxm2_block_id id)
{
  if (i < index_) { vcl_cerr << "cannot backtrack in the file!\n"; throw 0; }
  int dif = i-index_;
  if (dif >= num_cells(cell_size)) return 0; // means new buf needs to be loaded
  int byte_ind = (int)(dif*cell_size);
  return buf_->cell_buffer(byte_ind, cell_size);
}

//: return num cells on the buf
int boxm2_stream_cache_helper::num_cells(vcl_size_t cell_size)
{
  return (int)(buf_->buffer_length()/cell_size);
}


boxm2_stream_cache::boxm2_stream_cache(boxm2_scene_sptr scene,
                                       const vcl_vector<vcl_string>& data_types,
                                       const vcl_vector<vcl_string>& identifier_list,
                                       float num_giga)
 : scene_(scene), identifier_list_(identifier_list)
{
  mem_size_ = (unsigned long)(num_giga*vcl_pow(2.0, 30.0));
  vcl_cout << "Stream cache will take up " << num_giga << " GB of mem (" << mem_size_ << " bytes)." << vcl_endl;

  //: populate the data type map with sizes
  for (unsigned i = 0; i < data_types.size(); i++) {
    boxm2_stream_cache_datatype_helper_sptr h = new boxm2_stream_cache_datatype_helper;
    h->cell_size_ = boxm2_data_info::datasize(data_types[i]);
    data_types_[data_types[i]] = h;

    vcl_vector<boxm2_stream_cache_helper_sptr> tmp;
    for (unsigned j = 0; j < identifier_list.size(); j++) {
      boxm2_stream_cache_helper_sptr hh = new boxm2_stream_cache_helper;  // initializes file stream
      tmp.push_back(hh);
    }
    data_streams_[data_types[i]] = tmp;
  }
  unsigned long tot_size = 0;
  for (vcl_map<vcl_string, boxm2_stream_cache_datatype_helper_sptr>::iterator it = data_types_.begin(); it != data_types_.end(); it++) {
    tot_size += (unsigned long)it->second->cell_size_;
  }
  unsigned long k = (unsigned long)vcl_floor(float(mem_size_)/(identifier_list_.size()*tot_size));

  //: set buffer size in bytes for each data type
  vcl_map<vcl_string, boxm2_stream_cache_datatype_helper_sptr>::iterator it;
  for ( it = data_types_.begin(); it != data_types_.end(); it++) {
    it->second->buf_size_  = (unsigned long)it->second->cell_size_*k;
#ifdef DEBUG
    vcl_cout << it->first << " will have " << it->second->buf_size_/vcl_pow(2.0, 20.0) << " MB buffers per identifier.\n";
#endif
  }
}

//: hidden destructor (private so it cannot be called -- forces the class to be singleton)
boxm2_stream_cache::~boxm2_stream_cache()
{
  data_types_.clear();
  for (vcl_map<vcl_string, vcl_vector<boxm2_stream_cache_helper_sptr> >::iterator it = data_streams_.begin();
       it != data_streams_.end(); it++) {
    it->second.clear(); // calls the destructor for each member which closes the streams
  }
  data_streams_.clear();
}

//: in iterative mode, the files need to be closed and re-opened
void boxm2_stream_cache::close_streams()
{
  for (vcl_map<vcl_string, vcl_vector<boxm2_stream_cache_helper_sptr> >::iterator it = data_streams_.begin();
       it != data_streams_.end(); it++) {
    vcl_vector<boxm2_stream_cache_helper_sptr>& strs = it->second;
    for (unsigned i = 0; i < strs.size(); i++) {
      strs[i]->close_file();
    }
  }
}


boxm2_stream_cache_datatype_helper_sptr boxm2_stream_cache::get_helper(vcl_string& data_type)
{
  vcl_map<vcl_string, boxm2_stream_cache_datatype_helper_sptr >::iterator it = data_types_.find(data_type);
  if (it == data_types_.end()) {
    vcl_cerr << "boxm2_stream_cache::get_next cannot locate datatype: "<<data_type<<'\n';
    throw 0;
  }
  return it->second;
}

//: Binary write boxm2_cache  to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_cache const& scene) {}
void vsl_b_write(vsl_b_ostream& os, const boxm2_stream_cache* &p) {}
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_cache_sptr& sptr) {}
void vsl_b_write(vsl_b_ostream& os, boxm2_stream_cache_sptr const& sptr) {}

//: Binary load boxm2_cache  from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_stream_cache &scene) {}
void vsl_b_read(vsl_b_istream& is, boxm2_stream_cache* p) {}
void vsl_b_read(vsl_b_istream& is, boxm2_stream_cache_sptr& sptr) {}
void vsl_b_read(vsl_b_istream& is, boxm2_stream_cache_sptr const& sptr) {}

