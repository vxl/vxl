#ifndef boxm2_batch_opt2_phongs_functors_h_
#define boxm2_batch_opt2_phongs_functors_h_
//:
// \file

#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_phongs_model_processor.h>
#include <boxm2/io/boxm2_stream_cache.h>

//: compute pre_inf, vis_inf
class boxm2_batch_update_phongs_pass1_functor
{
  public:
    //: "default" constructor
    boxm2_batch_update_phongs_pass1_functor() = default;

    bool init_data(std::vector<boxm2_data_base*> & datas,
                   vil_image_view<float>* pre_img,
                   vil_image_view<float>* vis_img,
                   float sun_elev,
                   float sun_azim)
    {
        aux0_data_       =new boxm2_data<BOXM2_AUX0>(datas[0]->data_buffer(),  datas[0]->buffer_length(),datas[0]->block_id());
        aux1_data_       =new boxm2_data<BOXM2_AUX1>(datas[1]->data_buffer(),  datas[1]->buffer_length(),datas[1]->block_id());
        aux0_view_data_  =new boxm2_data<BOXM2_AUX0>(datas[2]->data_buffer(),  datas[2]->buffer_length(),datas[2]->block_id());
        aux1_view_data_  =new boxm2_data<BOXM2_AUX1>(datas[3]->data_buffer(),  datas[3]->buffer_length(),datas[3]->block_id());
        aux2_view_data_  =new boxm2_data<BOXM2_AUX2>(datas[4]->data_buffer(),  datas[4]->buffer_length(),datas[4]->block_id());
        aux3_view_data_  =new boxm2_data<BOXM2_AUX3>(datas[5]->data_buffer(),  datas[5]->buffer_length(),datas[5]->block_id());
        alpha_data_      =new boxm2_data<BOXM2_ALPHA> (datas[6]->data_buffer(),datas[6]->buffer_length(),datas[6]->block_id());
        phongs_data_     =new boxm2_data<BOXM2_FLOAT8>(datas[7]->data_buffer(),datas[7]->buffer_length(),datas[7]->block_id());

        pre_img_ = pre_img;
        vis_img_ = vis_img;

        sun_elev_ = sun_elev;
        sun_azim_ = sun_azim;
        return true;
    }

    inline bool step_cell(float seg_len,int index,unsigned i,unsigned j, float abs_depth = 0.0)
    {
        boxm2_data<BOXM2_AUX0>::datatype   & aux0 = aux0_data_->data()[index];
        boxm2_data<BOXM2_AUX1>::datatype   & aux1 = aux1_data_->data()[index];
        boxm2_data<BOXM2_AUX0>::datatype   & aux0_view = aux0_view_data_->data()[index];
        boxm2_data<BOXM2_AUX1>::datatype   & aux1_view = aux1_view_data_->data()[index];
        boxm2_data<BOXM2_AUX2>::datatype   & aux2_view = aux2_view_data_->data()[index];
        boxm2_data<BOXM2_AUX3>::datatype   & aux3_view = aux3_view_data_->data()[index];

        boxm2_data<BOXM2_FLOAT8>::datatype & phongs_model_data = phongs_data_->data()[index];

        brad_phongs_model model(phongs_model_data[0],
                                phongs_model_data[1],
                                phongs_model_data[2],
                                phongs_model_data[3],
                                phongs_model_data[4]);
        // compute average intensity for the cell
        float mean_obs =aux0/aux1;
        vnl_double_3 view_dir(aux1_view/aux0_view,aux2_view/aux0_view,aux3_view/aux0_view);

        float expI=boxm2_phongs_model_processor::expected_color(model,view_dir,sun_elev_,sun_azim_);
        // compute appearance probability of observation
        float PI=boxm2_phongs_model_processor::prob_density(mean_obs,expI,phongs_model_data[5]);
        // was: =boxm2_processor_type<BOXM2_MOG3_GREY>::type::prob_density(mog3_data_->data()[index], mean_obs);

        float vis=(*vis_img_)(i,j);
        float pre=(*pre_img_)(i,j);

        boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];

        float temp = std::exp(-alpha * seg_len);
        pre += vis*(1-temp)*PI;
        vis *= temp;

        // and update pre
        (*pre_img_)(i,j)= pre;
        // update visibility probability
        (*vis_img_)(i,j)= vis;

        return true;
    }
  private:
    boxm2_data<BOXM2_AUX0> * aux0_data_;
    boxm2_data<BOXM2_AUX1> * aux1_data_;
    boxm2_data<BOXM2_ALPHA> * alpha_data_;
    boxm2_data<BOXM2_FLOAT8> * phongs_data_; // was: boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;

    boxm2_data<BOXM2_AUX0> * aux0_view_data_;
    boxm2_data<BOXM2_AUX1> * aux1_view_data_;
    boxm2_data<BOXM2_AUX2> * aux2_view_data_;
    boxm2_data<BOXM2_AUX3> * aux3_view_data_;

    float sun_elev_;
    float sun_azim_;

    vil_image_view<float> * pre_img_;
    vil_image_view<float> * vis_img_;
};


//: compute average pre_i, vis_i and post_i for each cell, save the values in aux
class boxm2_batch_update_opt2_phongs_pass2_functor
{
  public:
    //: "default" constructor
    boxm2_batch_update_opt2_phongs_pass2_functor() = default;

    bool init_data(std::vector<boxm2_data_base*> & datas,
                   vil_image_view<float>* pre_img,
                   vil_image_view<float>* vis_img,
                   float sun_elev,
                   float sun_azim,
                   vil_image_view<float> * beta_denom)
    {
        aux0_data_       =new boxm2_data<BOXM2_AUX0>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
        aux1_data_       =new boxm2_data<BOXM2_AUX1>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
        aux0_view_data_  =new boxm2_data<BOXM2_AUX0>(datas[2]->data_buffer(),  datas[2]->buffer_length(),datas[2]->block_id());
        aux1_view_data_  =new boxm2_data<BOXM2_AUX1>(datas[3]->data_buffer(),  datas[3]->buffer_length(),datas[3]->block_id());
        aux2_view_data_  =new boxm2_data<BOXM2_AUX2>(datas[4]->data_buffer(),  datas[4]->buffer_length(),datas[4]->block_id());
        aux3_view_data_  =new boxm2_data<BOXM2_AUX3>(datas[5]->data_buffer(),  datas[5]->buffer_length(),datas[5]->block_id());
        alpha_data_      =new boxm2_data<BOXM2_ALPHA>(datas[6]->data_buffer(),datas[6]->buffer_length(),datas[6]->block_id());
        phongs_data_     =new boxm2_data<BOXM2_FLOAT8>(datas[7]->data_buffer(),datas[7]->buffer_length(),datas[7]->block_id());
        aux_data_        =new boxm2_data<BOXM2_AUX>(datas[8]->data_buffer(),datas[8]->buffer_length(),datas[8]->block_id());

        pre_img_ = pre_img;
        vis_img_ = vis_img;

        sun_elev_ = sun_elev;
        sun_azim_ = sun_azim;

        beta_denom_ = beta_denom;

        return true;
    }

    inline bool step_cell(float seg_len,int index,unsigned i,unsigned j, float tabs =0.0)
    {
        boxm2_data<BOXM2_AUX0>::datatype   & aux0 = aux0_data_->data()[index];
        boxm2_data<BOXM2_AUX1>::datatype   & aux1 = aux1_data_->data()[index];
        boxm2_data<BOXM2_AUX0>::datatype   & aux0_view = aux0_view_data_->data()[index];
        boxm2_data<BOXM2_AUX1>::datatype   & aux1_view = aux1_view_data_->data()[index];
        boxm2_data<BOXM2_AUX2>::datatype   & aux2_view = aux2_view_data_->data()[index];
        boxm2_data<BOXM2_AUX3>::datatype   & aux3_view = aux3_view_data_->data()[index];

        boxm2_data<BOXM2_FLOAT8>::datatype & phongs_model_data = phongs_data_->data()[index];

        // compute average intensity for the cell
        if (aux0 <1e-10f) return true;
        float mean_obs =aux0/aux1;
        vnl_double_3 view_dir(aux1_view/aux0_view,aux2_view/aux0_view,aux3_view/aux0_view);
        float PI =1.0;
        if (phongs_model_data[5] >0.0)
        {
            brad_phongs_model model(phongs_model_data[0],
                                    phongs_model_data[1],
                                    phongs_model_data[2],
                                    phongs_model_data[3],
                                    phongs_model_data[4]);
            float expI=boxm2_phongs_model_processor::expected_color(model,view_dir,sun_elev_,sun_azim_);
            // compute appearance probability of observation
            PI=boxm2_phongs_model_processor::prob_density(mean_obs,expI,phongs_model_data[5]);
            // was: =boxm2_processor_type<BOXM2_MOG3_GREY>::type::prob_density(mog3_data_->data()[index], mean_obs);
        }
        float vis=(*vis_img_)(i,j);
        float pre=(*pre_img_)(i,j);

        boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];

        boxm2_data<BOXM2_AUX>::datatype & aux = aux_data_->data()[index];

        // accumulate aux sample values
        //aux[0] += (pre  + (*alt_prob_img_)(i,j)) * seg_len;
        aux[1] += vis  * seg_len;

        const float beta_num = pre + vis*PI;
        const float beta_denom = (*beta_denom_)(i,j);

        float beta = 1.0f;
        if (beta_denom > 1e-5f) {
            beta =  beta_num / beta_denom;
        }
        const float old_PQ = (float)(1.0 - std::exp(-alpha*seg_len));
        const float new_PQ = old_PQ * beta;
        const float pass_prob_old = 1.0f - old_PQ;
        float pass_prob = 1.0f - new_PQ;

        // compute expected information gained from update
        const float weight = new_PQ * std::log(new_PQ / old_PQ) + pass_prob * std::log(pass_prob / pass_prob_old);
        // ensure log doesn't go to infinity
        if (pass_prob < 1e-5f) {
            pass_prob = 1e-5f;
        }
        aux[2] += std::log(pass_prob) * weight/seg_len;
        aux[3] += weight;

        float temp = std::exp(-alpha * seg_len);
        pre += vis*(1-temp)*PI;
        vis *= temp;

        // and update pre
        (*pre_img_)(i,j)= pre;
        // update visibility probability
        (*vis_img_)(i,j)= vis;

        return true;
    }
  private:
    boxm2_data<BOXM2_AUX0>  * aux0_data_;
    boxm2_data<BOXM2_AUX1>  * aux1_data_;
    boxm2_data<BOXM2_ALPHA> * alpha_data_;
    boxm2_data<BOXM2_FLOAT8>* phongs_data_;
    boxm2_data<BOXM2_AUX>   * aux_data_;
    boxm2_data<BOXM2_AUX0> * aux0_view_data_;
    boxm2_data<BOXM2_AUX1> * aux1_view_data_;
    boxm2_data<BOXM2_AUX2> * aux2_view_data_;
    boxm2_data<BOXM2_AUX3> * aux3_view_data_;

    float sun_elev_;
    float sun_azim_;

    vil_image_view<float> * pre_img_;
    vil_image_view<float> * vis_img_;
    vil_image_view<float> * beta_denom_;
};


class boxm2_batch_update_opt2_phongs_functor
{
  public:
    typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
    typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
    typedef boxm2_data_traits<BOXM2_AUX>::datatype aux_datatype;

    //: "default" constructor
    boxm2_batch_update_opt2_phongs_functor() = default;

    bool init_data(boxm2_data_base *alph,
                   boxm2_data_base *phongs,
                   boxm2_stream_cache_sptr str_cache1,
                   boxm2_stream_cache_sptr str_cache2,
                   float sun_elev,
                   float sun_azim)
    {
        alpha_data_  =new boxm2_data<BOXM2_ALPHA>(alph->data_buffer(),alph->buffer_length(),alph->block_id());
        phongs_data_ =new boxm2_data<BOXM2_FLOAT8>(phongs->data_buffer(), phongs->buffer_length(), phongs->block_id());
        str_cache1_   =str_cache1;
        str_cache2_   =str_cache2;
        id_ = alph->block_id();
        sun_elev_ = sun_elev;
        sun_azim_ = sun_azim;
        return true;
    }

    inline bool process_cell(int index)
    {
        boxm2_data<BOXM2_ALPHA>::datatype           & alpha=alpha_data_->data()[index];
        boxm2_data<BOXM2_FLOAT8>::datatype & phongs_model  =phongs_data_->data()[index];

        //: this has image data and view directions mixed.
        std::vector<aux0_datatype> aux0_raw = str_cache1_->get_next<BOXM2_AUX0>(id_, index);
        std::vector<aux1_datatype> aux1_raw = str_cache1_->get_next<BOXM2_AUX1>(id_, index);
        std::vector<aux0_datatype> aux2_raw = str_cache1_->get_next<BOXM2_AUX2>(id_, index);
        std::vector<aux1_datatype> aux3_raw = str_cache1_->get_next<BOXM2_AUX3>(id_, index);

        std::vector<aux_datatype>  aux  = str_cache2_->get_next<BOXM2_AUX> (id_, index);

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
        std::vector<aux0_datatype> cum_len;
        std::vector<aux0_datatype> Iobs;
        std::vector<aux0_datatype> vis;
        std::vector<aux0_datatype> xdir;
        std::vector<aux0_datatype> ydir;
        std::vector<aux0_datatype> zdir;
        cum_len.insert(cum_len.begin(), aux0_raw.begin(), aux0_raw.begin()+half_m);
        Iobs.insert(Iobs.begin(), aux1_raw.begin(), aux1_raw.begin()+half_m);
        vis.insert(vis.begin(), aux2_raw.begin(), aux2_raw.begin()+half_m);

        xdir.insert(xdir.begin(), aux1_raw.begin()+half_m, aux1_raw.end());
        ydir.insert(ydir.begin(), aux2_raw.begin()+half_m, aux2_raw.end());
        zdir.insert(zdir.begin(), aux3_raw.begin()+half_m, aux3_raw.end());
        std::vector<vnl_double_3>  viewing_dirs;
        for (unsigned i=0;i<Iobs.size();i++)
        {
            if (Iobs[i] < 0.0 || Iobs[i] > 1.0 ) vis[i] = 0.0;
            vnl_double_3 vec(xdir[i],ydir[i],zdir[i]);
            vec = vec.normalize();
            viewing_dirs.push_back(vec);
        }

       // UPDATE METHOD #1 : "OR" of occlusion probabilities
        double log_pass_prob_sum = 0.0;
        double weighted_seg_len_sum = 0.0;
        for (unsigned int s=0; s<aux.size(); ++s) {
            float obs_seg_len = cum_len[s]; // .seg_len_;
            float obs_weighted_seg_len = aux[s][3]; //.weighted_seg_len_sum_;

            if (obs_seg_len > 1e-6) {
                if (obs_weighted_seg_len > 1e-6) {
                    log_pass_prob_sum += aux[s][2]; //.log_pass_prob_sum_;
                    weighted_seg_len_sum += obs_weighted_seg_len;
                }
            }
        }

        // update the occlusion density
        if (weighted_seg_len_sum > 1e-6) {
            alpha = (float)(-log_pass_prob_sum / weighted_seg_len_sum);
        }

        // update brdf model.
        float var = 0.09f;
        brad_phongs_model pmodel = boxm2_phongs_model_processor::compute_phongs_model(var,sun_elev_,sun_azim_,
                                                                                      Iobs,viewing_dirs,vis);
        phongs_model[0] = pmodel.kd();
        phongs_model[1] = pmodel.ks();
        phongs_model[2] = pmodel.gamma();
        phongs_model[3] = pmodel.normal_elev();
        phongs_model[4] = pmodel.normal_azim();
        phongs_model[5] = var;

        return true;
    }

  private:
    boxm2_data<BOXM2_ALPHA> * alpha_data_;
    boxm2_data<BOXM2_FLOAT8>* phongs_data_;
    boxm2_stream_cache_sptr str_cache1_;
    boxm2_stream_cache_sptr str_cache2_;
    boxm2_block_id id_;

    float sun_elev_;
    float sun_azim_;
};


class boxm2_batch_update_nonray_phongs_functor
{
  public:
    typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;

    //: "default" constructor
    boxm2_batch_update_nonray_phongs_functor() = default;

    bool init_data(boxm2_data_base *alph,
                   boxm2_data_base *phongs,
                   boxm2_data_base *air,
                   boxm2_data_base *uncertain)
    {
        alpha_data_  =new boxm2_data<BOXM2_ALPHA>(alph->data_buffer(),alph->buffer_length(),alph->block_id());
        phongs_data_ =new boxm2_data<BOXM2_FLOAT8>(phongs->data_buffer(), phongs->buffer_length(), phongs->block_id());
        air_data_ =new boxm2_data<BOXM2_AUX0>(air->data_buffer(), air->buffer_length(), air->block_id());
        uncertain_data_ =new boxm2_data<BOXM2_AUX1>(uncertain->data_buffer(), uncertain->buffer_length(), uncertain->block_id());
        id_ = alph->block_id();
        return true;
    }

    inline bool process_cell(int index)
    {
        boxm2_data<BOXM2_ALPHA>::datatype  & alpha           = alpha_data_    ->data()[index];
        boxm2_data<BOXM2_FLOAT8>::datatype & phongs_model    = phongs_data_   ->data()[index];
        boxm2_data<BOXM2_AUX0>::datatype   & entropy_air     = air_data_      ->data()[index];
        boxm2_data<BOXM2_AUX1>::datatype   & uncertain_model = uncertain_data_->data()[index];

        float ratio = 0.0f;
        if (entropy_air> 0.0f)
          ratio = phongs_model[6]/entropy_air;
        else
          ratio = phongs_model[6]/1.0f;

        alpha *= ratio;
        uncertain_model = 1/std::max(ratio,1/ratio);

        return true;
    }

  private:
    boxm2_data<BOXM2_ALPHA> * alpha_data_;
    boxm2_data<BOXM2_FLOAT8>* phongs_data_;
    boxm2_data<BOXM2_AUX0>* air_data_;
    boxm2_data<BOXM2_AUX1>* uncertain_data_;

    boxm2_block_id id_;
};

#endif // boxm2_batch_opt2_phongs_functors_h_
