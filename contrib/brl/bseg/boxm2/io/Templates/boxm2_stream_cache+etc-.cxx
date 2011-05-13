#include <boxm2/io/boxm2_stream_cache.cxx>
#include <vcl_vector.h>

template vcl_vector<boxm2_data_traits<BOXM2_ALPHA>::datatype> boxm2_stream_cache::get_next<BOXM2_ALPHA,boxm2_data_traits<BOXM2_ALPHA>::datatype>(boxm2_block_id id, int index);

template vcl_vector<boxm2_data_traits<BOXM2_MOG2_RGB>::datatype> boxm2_stream_cache::get_next<BOXM2_MOG2_RGB,boxm2_data_traits<BOXM2_MOG2_RGB>::datatype>(boxm2_block_id id, int index);
template vcl_vector<boxm2_data_traits<BOXM2_MOG3_GREY_16>::datatype> boxm2_stream_cache::get_next<BOXM2_MOG3_GREY_16,boxm2_data_traits<BOXM2_MOG3_GREY_16>::datatype>(boxm2_block_id id, int index);
template vcl_vector<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype> boxm2_stream_cache::get_next<BOXM2_MOG3_GREY,boxm2_data_traits<BOXM2_MOG3_GREY>::datatype>(boxm2_block_id id, int index);

template bool boxm2_stream_cache::open_streams<BOXM2_ALPHA>(vcl_string data_type, boxm2_stream_cache_datatype_helper_sptr h);

template bool boxm2_stream_cache::open_streams<BOXM2_MOG3_GREY>(vcl_string data_type, boxm2_stream_cache_datatype_helper_sptr h);
template bool boxm2_stream_cache::open_streams<BOXM2_MOG3_GREY_16>(vcl_string data_type, boxm2_stream_cache_datatype_helper_sptr h);
template bool boxm2_stream_cache::open_streams<BOXM2_MOG2_RGB>(vcl_string data_type, boxm2_stream_cache_datatype_helper_sptr h);

template vcl_vector<boxm2_data_traits<BOXM2_AUX>::datatype> boxm2_stream_cache::get_next<BOXM2_AUX,boxm2_data_traits<BOXM2_AUX>::datatype>(boxm2_block_id id, int index);
template vcl_vector<boxm2_data_traits<BOXM2_AUX0>::datatype> boxm2_stream_cache::get_next<BOXM2_AUX0,boxm2_data_traits<BOXM2_AUX0>::datatype>(boxm2_block_id id, int index);
template vcl_vector<boxm2_data_traits<BOXM2_AUX1>::datatype> boxm2_stream_cache::get_next<BOXM2_AUX1,boxm2_data_traits<BOXM2_AUX1>::datatype>(boxm2_block_id id, int index);
template vcl_vector<boxm2_data_traits<BOXM2_AUX2>::datatype> boxm2_stream_cache::get_next<BOXM2_AUX2,boxm2_data_traits<BOXM2_AUX2>::datatype>(boxm2_block_id id, int index);
template vcl_vector<boxm2_data_traits<BOXM2_AUX3>::datatype> boxm2_stream_cache::get_next<BOXM2_AUX3,boxm2_data_traits<BOXM2_AUX3>::datatype>(boxm2_block_id id, int index);

template bool boxm2_stream_cache::open_streams<BOXM2_AUX>(vcl_string data_type, boxm2_stream_cache_datatype_helper_sptr h);
template bool boxm2_stream_cache::open_streams<BOXM2_AUX0>(vcl_string data_type, boxm2_stream_cache_datatype_helper_sptr h);
template bool boxm2_stream_cache::open_streams<BOXM2_AUX1>(vcl_string data_type, boxm2_stream_cache_datatype_helper_sptr h);
template bool boxm2_stream_cache::open_streams<BOXM2_AUX2>(vcl_string data_type, boxm2_stream_cache_datatype_helper_sptr h);
template bool boxm2_stream_cache::open_streams<BOXM2_AUX3>(vcl_string data_type, boxm2_stream_cache_datatype_helper_sptr h);





