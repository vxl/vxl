#ifndef boxm2_mean_intensities_batch_functor_h_
#define boxm2_mean_intensities_batch_functor_h_
//:
// \file

#include <boxm2/io/boxm2_stream_cache.h>
#include <vcl_vector.h>
#include <vcl_limits.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>

class boxm2_mean_intensities_batch_functor
{
 public:
  typedef boxm2_data_traits<BOXM2_AUX0>::datatype datatype;

  //: "default" constructor
  boxm2_mean_intensities_batch_functor() {}

  bool init_data(boxm2_data_base *output_alph, boxm2_stream_cache_sptr str_cache)
  {
    alpha_data_=new boxm2_data<BOXM2_AUX0>(output_alph->data_buffer(),output_alph->buffer_length(),output_alph->block_id());
    str_cache_ = str_cache;
    data_type_ = boxm2_data_traits<BOXM2_AUX0>::prefix();
    id_ = output_alph->block_id();
    return true;
  }

  inline bool process_cell(int index)
  {
    datatype & alpha=alpha_data_->data()[index];
    vcl_vector<datatype> out = str_cache_->get_next<BOXM2_AUX0>(id_, index);

    alpha = datatype(0.0);
    for (unsigned j = 0; j < out.size(); j++) {
      alpha += out[j];
    }
    alpha /= datatype(out.size());
    return true;
  }

 private:
  boxm2_data<BOXM2_AUX0>     * alpha_data_;
  boxm2_stream_cache_sptr str_cache_;
  vcl_string data_type_;
  boxm2_block_id id_;
};

class boxm2_mean_intensities_print_functor
{
 public:
  typedef boxm2_data_traits<BOXM2_AUX0>::datatype datatype;

  //: "default" constructor
  boxm2_mean_intensities_print_functor() {}

  bool init_data(boxm2_data_base *output_alph, boxm2_stream_cache_sptr str_cache)
  {
    alpha_data_=new boxm2_data<BOXM2_AUX0>(output_alph->data_buffer(),output_alph->buffer_length(),output_alph->block_id());
    str_cache_ = str_cache;
    id_ = output_alph->block_id();
    return true;
  }

  inline bool process_cell(int index)
  {
    datatype & alpha=alpha_data_->data()[index];
    vcl_vector<datatype> out = str_cache_->get_next<BOXM2_AUX0>(id_, index);

    if (index%1000000 == 0) {
      vcl_cout << alpha;
      for (unsigned j = 0; j < out.size(); ++j) {
        vcl_cout << ' ' << out[j];
      }
      vcl_cout << '\n';
    }
    return true;
  }

 private:
  boxm2_data<BOXM2_AUX0>     * alpha_data_;
  boxm2_stream_cache_sptr str_cache_;
  boxm2_block_id id_;
};

#endif // boxm2_mean_intensities_batch_functor_h_
