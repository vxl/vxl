#ifndef boxm2_change_detection_functor_h
#define boxm2_change_detection_functor_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>

class boxm2_change_detection_functor
{
 public:
  //: "default" constructor
  boxm2_change_detection_functor() {}

  bool init_data(vcl_vector<boxm2_data_base*> & datas,vil_image_view<float> * in_img, vil_image_view<float> * expected, vil_image_view<float>* vis_img)
  {
      alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
      mog3_data_=new boxm2_data<BOXM2_MOG3_GREY>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
      expected_img_=expected;
      vis_img_     =vis_img;
      in_img_     =in_img;
      return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j)
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float vis=(*vis_img_)(i,j);
    float exp_int=(*expected_img_)(i,j);
    float intensity=(*in_img_)(i,j);
    float curr_p=(1-vcl_exp(-alpha*seg_len))*vis;
    exp_int+=curr_p*boxm2_data_traits<BOXM2_MOG3_GREY>::processor::prob_density(mog3_data_->data()[index],intensity);
    (*expected_img_)(i,j)=exp_int;
    vis*=vcl_exp(-alpha*seg_len);
    (*vis_img_)(i,j)=vis;
    return true;
  }
 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  vil_image_view<float> *expected_img_;
  vil_image_view<float> *vis_img_;
  vil_image_view<float> *in_img_;
};
//: Functor class to normalize expected image
class normalize_foreground_probability_density
{
 public:
  normalize_foreground_probability_density() {}

  float operator()(float &pix) const
  {
    return 1/(1+pix);
  }


};

void boxm2_change_detection(boxm2_scene_info * linfo,
                            boxm2_block * blk_sptr,
                            vcl_vector<boxm2_data_base*> & datas,
                            vpgl_camera_double_sptr cam ,
                            vil_image_view<float> *input_img,
                            vil_image_view<float> *expected,
                            vil_image_view<float> * vis,
                            unsigned int roi_ni,
                            unsigned int roi_nj,
                            unsigned int roi_ni0=0,
                            unsigned int roi_nj0=0)
{
    boxm2_change_detection_functor cd_functor;
    cd_functor.init_data(datas,input_img,expected,vis);
    cast_ray_per_block<boxm2_change_detection_functor>(cd_functor,linfo,blk_sptr,cam,roi_ni,roi_nj,roi_ni0,roi_nj0);
}

#endif
