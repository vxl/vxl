#ifndef boxm2_compute_phongs_model_functor_h_
#define boxm2_compute_phongs_model_functor_h_
//:
// \file

#include <vector>
#include <iostream>
#include <algorithm>
#include <boxm2/io/boxm2_stream_cache.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <brad/brad_phongs_model_est.h>
#include <boxm2/cpp/algo/boxm2_phongs_model_processor.h>
#include <bsta/bsta_histogram.h>
#include <bsta/algo/bsta_sigma_normalizer.h>

class boxm2_compute_phongs_model_functor
{
  public:
    typedef boxm2_data_traits<BOXM2_FLOAT8>::datatype float8_datatype;
    typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
    typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
    typedef boxm2_data_traits<BOXM2_AUX2>::datatype aux2_datatype;
    typedef boxm2_data_traits<BOXM2_AUX3>::datatype aux3_datatype;

    //: "default" constructor
    boxm2_compute_phongs_model_functor() = default;

    bool init_data(float sun_elev,
                   float sun_azim,
                   boxm2_stream_cache_sptr str_cache,
                   boxm2_data_base * phongs_model,
                   bsta_sigma_normalizer_sptr n_table)
    {
        phongs_model_data_ = new boxm2_data<BOXM2_FLOAT8>(phongs_model->data_buffer(),
                                                          phongs_model->buffer_length(),
                                                          phongs_model->block_id());
        str_cache_ = str_cache;
        id_ = phongs_model->block_id();
        sun_elev_ =sun_elev;
        sun_azim_ =sun_azim;
        n_table_ = n_table;
        return true;
    }

    inline bool process_cell(int index, bool is_leaf = false, float side_len = 0.0)
    {
        boxm2_data<BOXM2_FLOAT8>::datatype & phongs_model=phongs_model_data_->data()[index];
        std::vector<aux0_datatype>  aux0_raw   = str_cache_->get_next<BOXM2_AUX0>(id_, index);
        std::vector<aux1_datatype>  aux1_raw   = str_cache_->get_next<BOXM2_AUX1>(id_, index);
        std::vector<aux2_datatype>  aux2_raw   = str_cache_->get_next<BOXM2_AUX2>(id_, index);
        std::vector<aux3_datatype>  aux3_raw   = str_cache_->get_next<BOXM2_AUX3>(id_, index);
        for (unsigned m = 0; m < aux0_raw.size(); m++) {
            if (aux0_raw[m]>1e-10f)
            {
                aux1_raw[m] /=aux0_raw[m];
                aux2_raw[m] /=aux0_raw[m];
                aux3_raw[m] /=aux0_raw[m];
            }
            else
            {
                aux1_raw[m] = 0.0;
                aux2_raw[m] = 0.0;
            }
        }

        int half_m = aux0_raw.size()/2;

        std::vector<aux1_datatype> Iobs;
        std::vector<aux2_datatype> vis;
        std::vector<aux1_datatype> xdir;
        std::vector<aux2_datatype> ydir;
        std::vector<aux3_datatype> zdir;

        Iobs.insert(Iobs.begin(), aux1_raw.begin(), aux1_raw.begin()+half_m);
        vis.insert(vis.begin(), aux2_raw.begin(), aux2_raw.begin()+half_m);

        xdir.insert(xdir.begin(), aux1_raw.begin()+half_m, aux1_raw.end());
        ydir.insert(ydir.begin(), aux2_raw.begin()+half_m, aux2_raw.end());
        zdir.insert(zdir.begin(), aux3_raw.begin()+half_m, aux3_raw.end());
        std::vector<vnl_double_3>  viewing_dirs;
        float sum_weights = 0.0f ;
        for (unsigned i=0;i<Iobs.size();i++)
        {
            if (Iobs[i] < 0.0 || Iobs[i] > 1.0 )
                vis[i] = 0.0;
            vnl_double_3 vec(xdir[i],ydir[i],zdir[i]);
            vec = vec.normalize();
            viewing_dirs.push_back(vec);

            sum_weights+=vis[i];
        }

        float var = 0.09f;
        brad_phongs_model pmodel = boxm2_phongs_model_processor::compute_phongs_model(var,sun_elev_,sun_azim_,Iobs,viewing_dirs,vis);
        var = var * n_table_->normalization_factor(sum_weights)*n_table_->normalization_factor(sum_weights);

        float sum_prob_densities = 0.0f;
        sum_weights = 0.0f ;

        for (unsigned i = 0; i < Iobs.size(); i++)
        {
            float val = boxm2_phongs_model_processor::expected_color(pmodel,viewing_dirs[i],sun_elev_,sun_azim_);
            sum_prob_densities += boxm2_phongs_model_processor::prob_density(Iobs[i],val,var) * vis[i];
            sum_weights        += vis[i];
        }
        phongs_model[0] = pmodel.kd();
        phongs_model[1] = pmodel.ks();
        phongs_model[2] = pmodel.gamma();
        phongs_model[3] = pmodel.normal_elev();
        phongs_model[4] = pmodel.normal_azim();

        phongs_model[5] = var;
        if (sum_weights > 0.0f)
            phongs_model[6] = sum_prob_densities/sum_weights;
        else
            phongs_model[6] = 0.0f;
        return true;
    }

  private:
    boxm2_data<BOXM2_FLOAT8>* phongs_model_data_;
    boxm2_stream_cache_sptr str_cache_;
    boxm2_block_id id_;
    float sun_elev_;
    float sun_azim_;
    bsta_sigma_normalizer_sptr n_table_;
};

#endif // boxm2_compute_phongs_model_functor_h_
