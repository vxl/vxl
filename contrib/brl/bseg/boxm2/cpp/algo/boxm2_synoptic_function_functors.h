#ifndef boxm2_synoptic_function_functors_h_
#define boxm2_synoptic_function_functors_h_
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
#include <brad/brad_synoptic_function_1d.h>

#include <boxm2/cpp/algo/boxm2_phongs_model_processor.h>
#include <bsta/bsta_histogram.h>
#include <bsta/algo/bsta_sigma_normalizer.h>

#include <boxm2/cpp/algo/boxm2_compute_phongs_model_functor.h>

class boxm2_entropy_air
{
  public:
    inline static float compute_entropy_air(std::vector<float> & Iobs, std::vector<float> & vis)
    {
        //: temporary histogram to store gradient frequncies.
        std::vector<float> temp_histogram(8,0.125f);
        float sum = 1.0;
        for (unsigned i=0;i<Iobs.size();i++)
        {
            unsigned index = i + 1;
            if (i == Iobs.size()-1)
                index =0;
            float gradI=std::fabs(Iobs[i]-Iobs[index]);

            int bin_index  = (int) std::floor(gradI*8);
            bin_index = bin_index>7 ? 7:bin_index;
            temp_histogram[bin_index] += (vis[i]+vis[index])/2;
            sum += (vis[i]+vis[index])/2;
        }
        for (unsigned i =0; i < 8;i++) temp_histogram[i] /= sum;

        float entropy_histo  =0.0;
        for (unsigned int i = 0; i<8; ++i)
        {
            double pi = temp_histogram[i];
            if (pi>0)
                entropy_histo += float(pi*std::log(pi));
        }
        entropy_histo = std::exp(-entropy_histo/float(vnl_math::log2e));

        return entropy_histo;
    }
};


class boxm2_compute_empty_model_gradient_functor
{
  public:
    typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
    typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
    typedef boxm2_data_traits<BOXM2_AUX2>::datatype aux2_datatype;

    //: "default" constructor
    boxm2_compute_empty_model_gradient_functor() = default;

    bool init_data(boxm2_data_base * entropy_histo_data,
                   boxm2_stream_cache_sptr str_cache)
    {
        entropy_histo_data_=new boxm2_data<BOXM2_AUX0>(entropy_histo_data->data_buffer(),
                                                       entropy_histo_data->buffer_length(),
                                                       entropy_histo_data->block_id());
        str_cache_ = str_cache;
        id_ = entropy_histo_data_->block_id();
        return true;
    }

    inline bool process_cell(int index, bool is_leaf = false, float side_len = 0.0)
    {
        boxm2_data<BOXM2_AUX0>::datatype            & entropy_histo=entropy_histo_data_->data()[index];

        std::vector<aux0_datatype>  out0   = str_cache_->get_next<BOXM2_AUX0>(id_, index);
        std::vector<aux1_datatype>  out1   = str_cache_->get_next<BOXM2_AUX1>(id_, index);
        std::vector<aux2_datatype>  out2   = str_cache_->get_next<BOXM2_AUX2>(id_, index);
        unsigned nobs = (unsigned)out0.size();

        std::vector<float> temp_histogram(8,0.125f);

        std::vector<float> Iobs;
        std::vector<float> vis;
        for (unsigned i = 0; i < nobs; i++)
        {
            if (out0[i]>1e-10f)
            {
                float mean_obs = out1[i]/out0[i];
                float vis_i    = out2[i]/out0[i]; // mean vis
                Iobs.push_back(mean_obs);
                vis.push_back(vis_i);
            }
        }

        float sum = 1.0;
        for (unsigned i=0;i<Iobs.size();i++)
        {
            unsigned index = i + 1;
            if (i == Iobs.size()-1)
                index =0;
            float gradI=std::fabs(Iobs[i]-Iobs[index]);

            int bin_index  = (int) std::floor(gradI*8);
            bin_index = bin_index>7 ? 7:bin_index;
            temp_histogram[bin_index] += (vis[i]+vis[index])/2;
            sum += (vis[i]+vis[index])/2;
        }
        for (unsigned i =0; i < 8;i++) temp_histogram[i] /= sum;

        entropy_histo  =0.0;
        for (unsigned int i = 0; i<8; ++i)
        {
            double pi = temp_histogram[i];
            if (pi>0)
                entropy_histo += float(pi*std::log(pi));
        }
        entropy_histo /= float(vnl_math::log2e);
        entropy_histo = std::exp(-entropy_histo);

        return true;
    }

  private:
    boxm2_data<BOXM2_AUX0>* entropy_histo_data_;
    boxm2_stream_cache_sptr str_cache_;
    boxm2_block_id id_;
};

class boxm2_synoptic_fucntion_1d_functor
{
  public:
    typedef boxm2_data_traits<BOXM2_FLOAT8>::datatype float8_datatype;
    typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
    typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
    typedef boxm2_data_traits<BOXM2_AUX2>::datatype aux2_datatype;
    typedef boxm2_data_traits<BOXM2_AUX3>::datatype aux3_datatype;

    //: "default" constructor
    boxm2_synoptic_fucntion_1d_functor() = default;

    bool init_data(boxm2_stream_cache_sptr str_cache,
                   boxm2_data_base * alpha_model,
                   boxm2_data_base * cubic_model)
    {
        cubic_model_data_ = new boxm2_data<BOXM2_FLOAT8>(cubic_model->data_buffer(),
                                                         cubic_model->buffer_length(),
                                                         cubic_model->block_id());
        alpha_model_data_ = new boxm2_data<BOXM2_ALPHA>(alpha_model->data_buffer(),
                                                        alpha_model->buffer_length(),
                                                        alpha_model->block_id());
        str_cache_ = str_cache;
        id_ = cubic_model->block_id();
        return true;
    }

    inline bool process_cell(int index, bool isleaf = false, float side_len =0.0)
    {
        boxm2_data<BOXM2_FLOAT8>::datatype & cubic_model=cubic_model_data_->data()[index];
        boxm2_data<BOXM2_ALPHA>::datatype & alpha=alpha_model_data_->data()[index];
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
        if (!isleaf)
            return true;

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

        std::vector<double> thetas;
        std::vector<double> phis;
        std::vector<double> vis_double;
        std::vector<double> obs_double;

        for (unsigned i=0;i<Iobs.size();i++)
        {
            if (Iobs[i] < 0.0 || Iobs[i] > 1.0 )
                vis[i] = 0.0;

            vnl_double_3 vec(xdir[i],ydir[i],zdir[i]);
            vec = vec.normalize();
            vis_double.push_back(vis[i]);
            obs_double.push_back(Iobs[i]);
            phis.push_back(std::atan2(vec[1],vec[0]));
            thetas.push_back(std::acos(vec[2]));
        }

        brad_synoptic_function_1d f(thetas,phis,vis_double,obs_double);
        f.fit_intensity_cubic();
        cubic_model[5] = (float) f.cubic_fit_prob_density();

         cubic_model[6] = boxm2_entropy_air::compute_entropy_air(Iobs,vis);
#if 0
        std::vector<double> amps;
        f.compute_auto_correlation();

        f.auto_corr_freq_amplitudes(amps);

        if (cubic_model[6]> 0.0)
            alpha *= (cubic_model[5]/cubic_model[6]);

        cubic_model[6] = f.max_frequency_prob_density();
#endif // 0
        float ps = 1 - std::exp(-alpha*side_len);
        float p = cubic_model[5] * ps /(cubic_model[5] * ps +cubic_model[6] * (1-ps) ) ;

        if (p<1)
            alpha = -(std::log(1-p)) / side_len;
        else
            alpha =10000;
        return true;
    }

  private:
    boxm2_data<BOXM2_FLOAT8>* cubic_model_data_;
    boxm2_data<BOXM2_ALPHA>* alpha_model_data_;
    boxm2_stream_cache_sptr str_cache_;
    boxm2_block_id id_;
};

class boxm2_compute_phongs_and_empty_update_functor
{
  public:
    typedef boxm2_data_traits<BOXM2_FLOAT8>::datatype float8_datatype;
    typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
    typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
    typedef boxm2_data_traits<BOXM2_AUX2>::datatype aux2_datatype;
    typedef boxm2_data_traits<BOXM2_AUX3>::datatype aux3_datatype;

    //: "default" constructor
    boxm2_compute_phongs_and_empty_update_functor() = default;

    bool init_data(float sun_elev,
                   float sun_azim,
                   boxm2_stream_cache_sptr str_cache,
                   boxm2_data_base * alpha,
                   boxm2_data_base * phongs_model,
                   boxm2_data_base * air_entropy_model,
                   bsta_sigma_normalizer_sptr n_table)
    {
        phongs_model_data_ = new boxm2_data<BOXM2_FLOAT8>(phongs_model->data_buffer(),
                                                          phongs_model->buffer_length(),
                                                          phongs_model->block_id());
        entropy_histo_data_=new boxm2_data<BOXM2_AUX0>(air_entropy_model->data_buffer(),
                                                       air_entropy_model->buffer_length(),
                                                       air_entropy_model->block_id());
        alpha_data_        =new boxm2_data<BOXM2_ALPHA>(alpha->data_buffer(),
                                                        alpha->buffer_length(),
                                                        alpha->block_id());

        str_cache_ = str_cache;
        id_ = phongs_model->block_id();
        sun_elev_ =sun_elev;
        sun_azim_ =sun_azim;
        n_table_ = n_table;
        return true;
    }

    inline bool process_cell(int index, bool is_leaf= false, float side_len= 0.0)
    {
        boxm2_data<BOXM2_FLOAT8>::datatype & phongs_model=phongs_model_data_->data()[index];
        boxm2_data<BOXM2_AUX0>::datatype   & entropy_histo=entropy_histo_data_->data()[index];
        std::vector<aux0_datatype>  aux0_raw   = str_cache_->get_next<BOXM2_AUX0>(id_, index);
        std::vector<aux1_datatype>  aux1_raw   = str_cache_->get_next<BOXM2_AUX1>(id_, index);
        std::vector<aux2_datatype>  aux2_raw   = str_cache_->get_next<BOXM2_AUX2>(id_, index);
        std::vector<aux3_datatype>  aux3_raw   = str_cache_->get_next<BOXM2_AUX3>(id_, index);

        if (!is_leaf)
            return true;

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
            if (Iobs[i] < 0.0 || Iobs[i] > 1.0 ) vis[i] = 0.0;
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

        entropy_histo = boxm2_entropy_air::compute_entropy_air(Iobs,vis);

        return true;
    }

  private:
    boxm2_data<BOXM2_FLOAT8>* phongs_model_data_;
    boxm2_data<BOXM2_AUX0>  * entropy_histo_data_;
    boxm2_data<BOXM2_ALPHA> * alpha_data_;
    boxm2_stream_cache_sptr str_cache_;
    boxm2_block_id id_;
    float sun_elev_;
    float sun_azim_;
    bsta_sigma_normalizer_sptr n_table_;
};

class boxm2_update_synoptic_probability
{
  public:
    boxm2_update_synoptic_probability() = default;

    bool init_data(boxm2_data_base * alpha_model,
                   boxm2_data_base * cubic_model)
    {
        cubic_model_data_ = new boxm2_data<BOXM2_FLOAT8>(cubic_model->data_buffer(),
                                                         cubic_model->buffer_length(),
                                                         cubic_model->block_id());
        alpha_model_data_ = new boxm2_data<BOXM2_ALPHA>(alpha_model->data_buffer(),
                                                        alpha_model->buffer_length(),
                                                        alpha_model->block_id());
        id_ = cubic_model->block_id();
        return true;
    }

    inline bool process_cell(int index, bool isleaf = false, float side_len =0.0)
    {
        boxm2_data<BOXM2_FLOAT8>::datatype & cubic_model=cubic_model_data_->data()[index];
        boxm2_data<BOXM2_ALPHA>::datatype & alpha=alpha_model_data_->data()[index];
        if (!isleaf)
            return true;

        float ps = 1 - std::exp(-alpha*side_len);
        float p = cubic_model[5] * ps /(cubic_model[5] * ps +cubic_model[6] * (1-ps) ) ;

        if (p<1)
            alpha = -(std::log(1-p)) / side_len;
        else
            alpha =10000;
        return true;
    }

  private:
    boxm2_data<BOXM2_FLOAT8>* cubic_model_data_;
    boxm2_data<BOXM2_ALPHA>* alpha_model_data_;
    boxm2_stream_cache_sptr str_cache_;
    boxm2_block_id id_;
};

#endif // boxm2_synoptic_function_functors_h_
