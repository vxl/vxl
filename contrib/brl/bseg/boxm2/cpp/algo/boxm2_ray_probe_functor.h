#ifndef boxm2_ray_probe_functor_h
#define boxm2_ray_probe_functor_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>

class boxm2_ray_probe_functor
{
 public:
  //: "default" constructor
  boxm2_ray_probe_functor() {}

  bool init_data(vcl_vector<boxm2_data_base*> & datas,
                 vcl_vector<float> & seg_len,
                 vcl_vector<float> & abs_depth,
                 vcl_vector<float> & alpha,
                 vcl_vector<float> & sunvis)
  {
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    //aux_sun_data_=new boxm2_data<BOXM2_AUX0>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    seg_len_=&seg_len;
    alpha_  =&alpha;
    //sunvis_ = &sunvis;
    abs_depth_=&abs_depth;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j, float abs_depth)
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
   // boxm2_data<BOXM2_AUX0>::datatype aux_sun_vis=aux_sun_data_->data()[index];
    seg_len_->push_back(seg_len);
    alpha_->push_back(alpha);
    abs_depth_->push_back(abs_depth);
    //sunvis_->push_back(aux_sun_vis);
    return true;
  }

 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<BOXM2_AUX0> * aux_sun_data_;
  vcl_vector<float> * abs_depth_;
  vcl_vector<float> * seg_len_;
  vcl_vector<float> * alpha_;
  vcl_vector<float> * sunvis_;
};

class boxm2_ray_app_density_functor
{
 public:
  //: "default" constructor
  boxm2_ray_app_density_functor() {}

  bool init_data(vcl_vector<boxm2_data_base*> & datas,
                 vcl_vector<float> & app_density,
                 float intensity)
  {
    //alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    mog3_data_ =new boxm2_data<BOXM2_MOG3_GREY>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    app_density_=&app_density;
    intensity_  =intensity;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j,float t_abs)
  {
    app_density_->push_back(boxm2_data_traits<BOXM2_MOG3_GREY>::processor::prob_density(mog3_data_->data()[index], intensity_));
    return true;
  }
 private:
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  vcl_vector<float> * app_density_;
  float intensity_;
};

class boxm2_ray_probe_brdfs_functor
{
 public:
  //: "default" constructor
  boxm2_ray_probe_brdfs_functor() {}

  bool init_data(vcl_vector<boxm2_data_base*> & datas,
                 vcl_vector<float> & seg_len,
                 vcl_vector<float> & abs_depth,
                 vcl_vector<float> & alpha,
                 vcl_vector<float> & phongs_vars,
                 vcl_vector<float> & entropy_histo_air)
  {
    alpha_data_             =new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    phongs_models_data_     =new boxm2_data<BOXM2_FLOAT8>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    histo_air_data_         =new boxm2_data<BOXM2_MOG3_GREY>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    entropy_histo_air_data_ =new boxm2_data<BOXM2_AUX0>(datas[3]->data_buffer(),datas[3]->buffer_length(),datas[3]->block_id());
    seg_len_               = &seg_len;
    abs_depth_             = &abs_depth;
    alpha_                 = &alpha ;
    phongs_vars_           = &phongs_vars;
    entropy_histo_air_     = &entropy_histo_air;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j, float abs_depth)
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    boxm2_data<BOXM2_FLOAT8>::datatype phongs_model=phongs_models_data_->data()[index];
    boxm2_data<BOXM2_AUX0>::datatype entropy_air=entropy_histo_air_data_->data()[index];

    seg_len_->push_back(seg_len);
    abs_depth_->push_back(abs_depth);
    phongs_vars_->push_back(phongs_model[6]);
    alpha_->push_back(alpha);
#if 0 // unused ?!?
    float sigma = 0.75;
    float prob_density = 1/(vcl_sqrt(2*vnl_math::pi)* sigma)*vcl_exp(-(entropy_air-2.07)*(entropy_air-2.07)/(2*sigma*sigma));
#endif
    entropy_histo_air_->push_back(vcl_exp(entropy_air));
    return true;
  }

 private:
  boxm2_data<BOXM2_ALPHA>      * alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY>  * histo_air_data_;
  boxm2_data<BOXM2_AUX0>       * entropy_histo_air_data_;
  boxm2_data<BOXM2_FLOAT8>     * phongs_models_data_;
  vcl_vector<float> * abs_depth_;
  vcl_vector<float> * seg_len_;
  vcl_vector<float> * alpha_;
  vcl_vector<float> * phongs_vars_;
  vcl_vector<float> * entropy_histo_air_;
};

#endif
