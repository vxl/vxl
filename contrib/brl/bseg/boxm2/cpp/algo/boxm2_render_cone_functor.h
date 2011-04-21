#ifndef boxm2_render_cone_functor_h
#define boxm2_render_cone_functor_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_cone_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <vil/vil_image_view.h>
#if 0
#include <vcl_iostream.h>
#endif

class boxm2_render_cone_functor
{
 public:
  //: "default" constructor
  boxm2_render_cone_functor() {}

  bool init_data(vcl_vector<boxm2_data_base*> & datas, vil_image_view<float> * expected, vil_image_view<float>* vis_img)
  {
    alpha_data_ = new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    mog3_data_  = new boxm2_data<BOXM2_MOG3_GREY>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    expected_img_=expected;
    vis_img_     =vis_img;
    return true;
  }

  inline bool step_cell(float volume, int index, unsigned i, unsigned j,
                        float block_len, float& intensity_norm, float& weighted_int, 
                        float& prob_surface)
  {
    //grab voxel alpha and intensity
    boxm2_data<BOXM2_ALPHA>::datatype alpha = alpha_data_->data()[index];
    float voxel_int = boxm2_data_traits<BOXM2_MOG3_GREY>::processor::expected_color(mog3_data_->data()[index]);

    //probability that this voxel is occupied by surface
    float cell_occupancy_prob = (1.0-vcl_exp(-alpha*block_len));
    prob_surface += cell_occupancy_prob * volume; 

    //weighted intensity for this voxel
    weighted_int += cell_occupancy_prob * volume * voxel_int;
    intensity_norm += cell_occupancy_prob * volume;

    //probability that current cell is visible
    return true;
  }

  inline bool update_vis(float sphere_occ_prob, unsigned i, unsigned j) {
    float vis = (*vis_img_)(i,j);
    vis *= (1.0-sphere_occ_prob);
    (*vis_img_)(i,j) = vis;
    return true;
  }

  inline bool update_expected_int(float expected_int, float sphere_occ_prob, unsigned i, unsigned j) {
    float ei = (*expected_img_)(i,j);
    float vis = (*vis_img_)(i,j);

    //expected intensity is Visibility * Weighted Intensity * Occupancy 
    ei += vis * expected_int * sphere_occ_prob;
    (*expected_img_)(i,j) = ei;
    return true;
  }
  
  inline float vis(unsigned i, unsigned j) {
    return (*vis_img_)(i,j); 
  }

 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  vil_image_view<float> *expected_img_;
  vil_image_view<float> *vis_img_;
};

#if 0
//: Functor class to normalize expected image
class normalize_intensity
{
 public:
  normalize_intensity() {}

  void operator()(float mask, float &pix) const
  {
    pix+=mask*0.5f;
  }
};
#endif // 0

#endif
