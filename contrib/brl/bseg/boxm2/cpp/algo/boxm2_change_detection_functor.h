#ifndef boxm2_change_detection_functor_h
#define boxm2_change_detection_functor_h
//:
// \file

#include <iostream>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <vil/algo/vil_gauss_filter.h>
#ifdef DEBUG
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

class boxm2_change_detection_functor
{
 public:
  //: "default" constructor
  boxm2_change_detection_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas,vil_image_view<float> * in_img, vil_image_view<float> * expected, vil_image_view<float>* vis_img)
  {
      alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
      mog3_data_=new boxm2_data<BOXM2_MOG3_GREY>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
      expected_img_=expected;
      vis_img_     =vis_img;
      in_img_     =in_img;
      return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j, float abs_depth=0.0)
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float vis=(*vis_img_)(i,j);
    float exp_int=(*expected_img_)(i,j);
    float intensity=(*in_img_)(i,j);
    float curr_p=(1-std::exp(-alpha*seg_len))*vis;
    exp_int+=curr_p*boxm2_processor_type<BOXM2_MOG3_GREY>::type::prob_density(mog3_data_->data()[index],intensity);
    (*expected_img_)(i,j)=exp_int;
    vis*=std::exp(-alpha*seg_len);
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
  normalize_foreground_probability_density() = default;

  float operator()(float &pix) const
  {
    return 1.f/(1.f+pix)-0.5f*std::min(pix,1.f/pix);
  }
};

class boxm2_change_detection_with_uncertainity_functor
{
 public:

  //: "default" constructor
  boxm2_change_detection_with_uncertainity_functor(unsigned ni, unsigned nj)
  {
    ni_=ni;
    nj_=nj;

    //: used only inside the functor.
    vis_img_       =new vil_image_view<float>(ni_,nj_);
    expected_img_  =new vil_image_view<float>(ni_,nj_);
    dist_image_    =new vil_image_view<double>(ni_,nj_);
    running_weight_=new vil_image_view<float>(ni_,nj_);
    vis_img_->fill(1.0f);
    expected_img_->fill(0.0f);
    running_weight_->fill(0.0f);
    dist_image_->fill(0.0f);
  }

  bool set_data(std::vector<boxm2_data_base*> & datas,
                vil_image_view<float> * in_img,
                vil_image_view<float> * change_image)
  {
    alpha_data_    =new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    mog3_data_     =new boxm2_data<BOXM2_MOG3_GREY>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());


    in_img_=in_img;
    change_image_=change_image;

    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j, float abs_depth)
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    boxm2_data<BOXM2_MOG3_GREY>::datatype mog3=mog3_data_->data()[index];

    float vis=(*vis_img_)(i,j);
    float curr_p=(1-std::exp(-alpha*seg_len))*vis;

    vnl_vector_fixed<unsigned char,8> updated_exp_distribution((unsigned char)0);

    (*expected_img_)(i,j)=(*expected_img_)(i,j)+boxm2_processor_type<BOXM2_MOG3_GREY>::type::expected_color(mog3)*curr_p;
    (*change_image_)(i,j)=(*change_image_)(i,j)+boxm2_processor_type<BOXM2_MOG3_GREY>::type::prob_density(mog3,(*in_img_)(i,j))*curr_p;
    //: representing 8 bytes of data aby double
    unsigned img_index=j*ni_+i;
    unsigned char * exp_distribution_array=reinterpret_cast<unsigned char *>(dist_image_->top_left_ptr()+img_index);
    vnl_vector_fixed<unsigned char,8> exp_distribution(exp_distribution_array);
    float w2=(*running_weight_)(i,j);

    //: merge mixtures only if the weight is non zero.
    if (w2<=0.0f)
      updated_exp_distribution=mog3_data_->data()[index];
    else
      boxm2_processor_type<BOXM2_MOG3_GREY>::type::merge_mixtures(mog3,curr_p,exp_distribution,w2,updated_exp_distribution);
#ifdef DEBUG
    std::cout<<'[';
    for (unsigned k=0;k<8;k++)
      std::cout<<(int)exp_distribution[k]<<',';
    std::cout<<"] *"<<w2<<" +\n[";
    for (unsigned k=0;k<8;k++)
      std::cout<<(int)mog3_data_->data()[index][k]<<',';
    std::cout<<"]* "<<curr_p<<" =\n [";
    for (unsigned k=0;k<8;k++)
      std::cout<<(int)updated_exp_distribution[k]<<',';
    std::cout<<"]\n\n";
#endif
    (*running_weight_)(i,j)=curr_p+w2;

    //: representing 8 bytes of data aby double
    double * final=reinterpret_cast<double *>(updated_exp_distribution.data_block());
    (*dist_image_)(i,j)=(*final);

    vis*=std::exp(-alpha*seg_len);
    (*vis_img_)(i,j)=vis;
    return true;
  }

  void finish()
  {
      int count=-1;
      float sigma =2.0f;
      for (unsigned j=0;j<nj_;j++)
          for (unsigned i=0;i<ni_;i++)
              (*expected_img_)(i,j)+=(*vis_img_)(i,j)*0.5f;

      vil_image_view<float> expblur(expected_img_->ni(), expected_img_->nj());
      vil_gauss_filter_2d((*expected_img_), expblur, sigma, (unsigned)(3*sigma+0.01f));

      for (unsigned j=0;j<nj_;j++)
          for (unsigned i=0;i<ni_;i++)
          {
              float pb=(*change_image_)(i,j);
              pb+=(*vis_img_)(i,j)*1.0f;
              float bf=1.f/(1.f+pb)-0.5f*std::min(pb,1.f/pb);

              ++count;
              unsigned char * exp_distribution_array=reinterpret_cast<unsigned char *>(dist_image_->top_left_ptr()+count);
              vnl_vector_fixed<unsigned char,8> exp_distribution(exp_distribution_array);
              float pr=boxm2_processor_type<BOXM2_MOG3_GREY>::type::prob_density(exp_distribution,expblur(i,j));
              float br=pr/(1.f+pr)-0.5f*std::min(pr,1.f/pr);

              (*change_image_)(i,j)=bf*br;
          }
  }
 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  vil_image_view<float> *in_img_;
  vil_image_view<float> *change_image_;

  vil_image_view<float> *expected_img_;
  vil_image_view<float> *vis_img_;
  vil_image_view<double>* dist_image_;
  vil_image_view<float> * running_weight_;
  unsigned ni_;
  unsigned nj_;
};


#endif
