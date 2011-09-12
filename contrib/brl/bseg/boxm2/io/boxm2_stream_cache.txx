#ifndef boxm2_stream_cache_txx_
#define boxm2_stream_cache_txx_

#include "boxm2_stream_cache.h"
//:
// \file

template <boxm2_data_type T>
bool boxm2_stream_cache::open_streams(boxm2_stream_cache_datatype_helper_sptr h)
{
  vcl_string data_type = boxm2_data_traits<T>::prefix();
  vcl_vector<boxm2_stream_cache_helper_sptr>& strs = data_streams_[data_type];
  if (strs.size() != identifier_list_.size()) return false;
  for (unsigned i = 0; i < identifier_list_.size(); i++) {
    vcl_string key = boxm2_data_traits<T>::prefix(identifier_list_[i]);
    vcl_string filename = scene_->data_path() + key + "_" + h->current_block_.to_string() + ".bin";
    unsigned long numBytes = vul_file::size(filename);

    int cnt = int(numBytes/(float)h->cell_size_);
    if (h->cell_cnt_ < 0) h->cell_cnt_ = cnt;
    else if (h->cell_cnt_ != cnt)
      h->cell_cnt_ = cnt;  // to make it work after a possible refinement of blocks
    if (!strs[i]->open_file(filename.c_str())) {
      vcl_cerr<<"boxm2_stream_cache::get_next cannot open file "<<filename<<'\n';
      throw 0;
    }
  }

  return true;
}

//: returns the data points pointed by the current_index_ and then advances the current_index_ by 1
template <boxm2_data_type T>
vcl_vector<typename boxm2_data_traits<T>::datatype> boxm2_stream_cache::get_next(boxm2_block_id id, int index)
{
  //: get the data of this data type
  vcl_string data_type = boxm2_data_traits<T>::prefix();
  boxm2_stream_cache_datatype_helper_sptr h = get_helper(data_type);
  vcl_vector<boxm2_stream_cache_helper_sptr>& streams = data_streams_[data_type];

  if (h->current_index_ < 0) {  // this is the first time we're doing anything about this data type
    h->current_block_ = id;
    h->current_index_ = 0;
    h->cell_cnt_=-1;

    //: open up all the streams
    if (!open_streams<T>( h)) { vcl_cout <<data_type<< " 1 Error opening streams!\n"; throw 0; }
  }
  else if (h->current_block_ != id) {  // opening a new block, clear the current streams and open new ones
    h->current_block_ = id;
    h->current_index_ = 0;
    for (unsigned i = 0; i < streams.size(); i++) {
      streams[i]->close_file();
    }
    //: open up all the streams
    if (!open_streams<T>( h)) { vcl_cout << "2 Error opening streams!\n"; throw 0; }
  }

  // now return the data elements at the current index, if buf indices are zero it means the buf was never read
  vcl_vector<typename boxm2_data_traits<T>::datatype> output;
  if (!streams.size()) return output;  // return an empty list
  if (h->current_index_ >= h->cell_cnt_) { // we've reached end of file
    for (unsigned i =0; i < streams.size(); i++) {
      streams[i]->close_file();
    }
    h->current_index_ = -1;
    return output;
  }

  if (index >=0 && h->current_index_ != index) {
    vcl_cerr << "Indices are not in sync!\n";
    throw 0;
  }

  if (streams[0]->index_ < 0) {  // read the first chunks into the bufs
    for (unsigned i = 0; i < streams.size(); i++) {
      streams[i]->read(h->buf_size_, h->current_block_);
      streams[i]->index_ = 0;
    }
  }

  //: read the next cell
  for (unsigned i = 0; i < streams.size(); i++) {
    char * cell = streams[i]->get_cell(h->current_index_, h->cell_size_, h->current_block_);
    if (!cell) {  // need to read next chunk
      streams[i]->index_ = streams[i]->index_ + streams[i]->num_cells(h->cell_size_);
      streams[i]->read(h->buf_size_, h->current_block_);
      //: now it should be alright
      cell = streams[i]->get_cell(h->current_index_, h->cell_size_, h->current_block_);
      if (!cell) { vcl_cerr << "problem in reading from files!\n"; throw 0; }
    }
    typename boxm2_data_traits<T>::datatype val = reinterpret_cast<typename boxm2_data_traits<T>::datatype *>(cell)[0]; 
    output.push_back(val);
    delete [] cell;

  }
  h->current_index_++;

  //: check again as there may not be another call
  if (h->current_index_ >= h->cell_cnt_) { // we've reached end of file
    for (unsigned i =0; i < streams.size(); i++) {
      streams[i]->close_file();
    }
    h->current_index_ = -1;
  }

  return output;
}

//: returns the data points pointed by the current_index_ and then advances the current_index_ by 1
template <boxm2_data_type T>
vcl_vector<typename boxm2_data_traits<T>::datatype> boxm2_stream_cache::get_random_i(boxm2_block_id id, unsigned int index)
{
  //: get the data of this data type
  vcl_string data_type = boxm2_data_traits<T>::prefix();
  boxm2_stream_cache_datatype_helper_sptr h = get_helper(data_type);
  vcl_vector<boxm2_stream_cache_helper_sptr>& streams = data_streams_[data_type];
  h->current_block_ = id;
  h->current_index_ = 0;
  for (unsigned i = 0; i < streams.size(); i++) {
      streams[i]->close_file();
  }
  //: open up all the streams
  if (!open_streams<T>(h)) { vcl_cout << "Error opening streams!\n"; throw 0; }
    //vcl_cout<<"No of streams  "<<h->cell_size_<<" "<<h->buf_size_<<vcl_endl;

  vcl_vector<typename boxm2_data_traits<T>::datatype> output;
  if (index > h->buf_size_)
    return output;

  //: read the next cell
  for (unsigned i = 0; i < streams.size(); i++) {
    streams[i]->ifs_.seekg(index*h->cell_size_);
    streams[i]->read(h->buf_size_, h->current_block_);
    //: now it should be alright
    char * cell = streams[i]->get_cell(h->current_index_, h->cell_size_, h->current_block_);
    if (!cell) { vcl_cerr << "problem in reading from files!\n"; throw 0; }
    output.push_back(reinterpret_cast<typename boxm2_data_traits<T>::datatype*>(cell)[0]);

    delete [] cell;
  }

  //: check again as there may not be another call
  for (unsigned i =0; i < streams.size(); i++) {
    streams[i]->close_file();
  }
  h->current_index_ = -1;
  return output;
}

#undef BOXM2_STREAM_CACHE_INSTANTIATE
#define BOXM2_STREAM_CACHE_INSTANTIATE(T) \
template vcl_vector<boxm2_data_traits<T >::datatype> boxm2_stream_cache::get_next<T >(boxm2_block_id id, int index);\
template bool boxm2_stream_cache::open_streams<T >( boxm2_stream_cache_datatype_helper_sptr h);\
template vcl_vector<boxm2_data_traits<T >::datatype> boxm2_stream_cache::get_random_i<T >(boxm2_block_id id, unsigned int index)

#endif // boxm2_stream_cache_txx_
