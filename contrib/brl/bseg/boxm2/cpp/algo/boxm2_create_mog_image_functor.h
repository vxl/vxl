#ifndef boxm2_create_mog_image_functor_h
#define boxm2_create_mog_image_functor_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <boxm2/cpp/algo/boxm2_gauss_grey_processor.h>
#include <vil/vil_image_view.h>

template <boxm2_data_type APM_TYPE>
class boxm2_create_mog_image_functor
{
 public:
  //: "default" constructor
  boxm2_create_mog_image_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas, vbl_array_2d<typename boxm2_data_traits<APM_TYPE>::datatype> * mog_img, vil_image_view<float> * vis_img)
  {
    alpha_data_ = new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    mog3_data_ = new boxm2_data<APM_TYPE>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    nobs_data_ = new boxm2_data<BOXM2_NUM_OBS>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());

    mog_img_ = mog_img;
    vis_img_ = vis_img;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j, float abs_depth = 0.0)
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float vis=(*vis_img_)(i,j);
    float curr_p=(1-std::exp(-alpha*seg_len))*vis;
    typename boxm2_data<APM_TYPE>::datatype & mog3_voxel =mog3_data_->data()[index];

    //float mean_obs = boxm2_processor_type<APM_TYPE>::type::most_probable_mode_color(mog3_voxel);
    float mean_obs = boxm2_processor_type<APM_TYPE>::type::expected_color(mog3_voxel);

    typename boxm2_data<APM_TYPE>::datatype & mog3=(*mog_img_)(i,j);
    typename boxm2_data<BOXM2_NUM_OBS>::datatype & nobs=nobs_data_->data()[index];
    vnl_vector_fixed<float,4> nobs_float;
    nobs_float[0]=(float)nobs[0]; nobs_float[1]=(float)nobs[1]; nobs_float[2]=(float)nobs[2];
    //: converting float to short
    nobs_float[3]=((float)nobs[3])/100.0f;
    boxm2_processor_type<APM_TYPE>::type::update_app_model(mog3,nobs_float, mean_obs,curr_p,0.09f, 0.03f);
    nobs[0]=(unsigned short)nobs_float[0]; nobs[1]=(unsigned short)nobs_float[1]; nobs[2]=(unsigned short)nobs_float[2];
    nobs[3]=(unsigned short)(nobs_float[3]*100.0f);

    vis*=std::exp(-alpha*seg_len);
    (*vis_img_)(i,j)=vis;

    return true;
  }
 private:
  boxm2_data<APM_TYPE> * mog3_data_;
  boxm2_data<BOXM2_ALPHA>     * alpha_data_;
  boxm2_data<BOXM2_NUM_OBS>   * nobs_data_;

  vbl_array_2d<typename boxm2_data_traits<APM_TYPE>::datatype> * mog_img_;
  vil_image_view<float> * vis_img_;

};



#endif
