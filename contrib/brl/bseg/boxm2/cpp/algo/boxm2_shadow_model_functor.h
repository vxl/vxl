#ifndef boxm2_shadow_model_functor_h_
#define boxm2_shadow_model_functor_h_
//:
// \file

#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <boxm2/io/boxm2_stream_cache.h>
#include <bsta/algo/bsta_sigma_normalizer.h>

class boxm2_compute_ambient_functor
{
public:
    typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
    typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
    typedef boxm2_data_traits<BOXM2_AUX2>::datatype aux2_datatype;

    //: "default" constructor
    boxm2_compute_ambient_functor() = default;

    bool init_data(boxm2_data_base *sunvis,
        boxm2_stream_cache_sptr str_cache,
        float & sunvis_weights,
        float & weighted_sunvis_intensities,
        float block_len, int max_levels)
    {
        sunvis_data_=new boxm2_data<BOXM2_AUX0>(sunvis->data_buffer(),sunvis->buffer_length(),sunvis->block_id());
        str_cache_ = str_cache;
        id_ = sunvis_data_->block_id();
        sunvis_weights_=&sunvis_weights;
        weighted_sunvis_intensities_=&weighted_sunvis_intensities;

        return true;
    }

    inline bool process_cell(int index)
    {
        boxm2_data<BOXM2_ALPHA>::datatype & sunvis=sunvis_data_->data()[index];

        std::vector<aux0_datatype>  out0   = str_cache_->get_next<BOXM2_AUX0>(id_, index);
        std::vector<aux1_datatype>  out1   = str_cache_->get_next<BOXM2_AUX1>(id_, index);
        std::vector<aux1_datatype>  out2   = str_cache_->get_next<BOXM2_AUX2>(id_, index);

        std::vector<aux0_datatype> obs; std::vector<float> vis;

        float sumobs=0.0f;
        float weight=0.0f;
        unsigned nimgs = (unsigned)out0.size();
        for (unsigned m = 0; m < nimgs; m++) {
            if(out0[m]>1e-10f)
            {
                float mean_obs = out1[m]/out0[m];
                float vis_i    = out2[m]/out0[m]; // mean vis
                sumobs+=vis_i*mean_obs;
                weight+=vis_i;
            }
        }
        if(weight>1e-10f)
        {
            if(sunvis<0.05)
            {
                (*sunvis_weights_)+=(1-sunvis);
                (*weighted_sunvis_intensities_)+=(1-sunvis)*(sumobs/weight);
            }
        }
        return true;
    }

private:
    boxm2_data<BOXM2_AUX0>* sunvis_data_;
    boxm2_stream_cache_sptr str_cache_;
    boxm2_block_id id_;
    float * sunvis_weights_;
    float * weighted_sunvis_intensities_;
};



#endif // boxm2_shadow_model_functor_h_
