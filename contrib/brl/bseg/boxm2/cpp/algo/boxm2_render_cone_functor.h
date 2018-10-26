#ifndef boxm2_render_cone_functor_h
#define boxm2_render_cone_functor_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_cone_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <vil/vil_image_view.h>
class boxm2_render_cone_functor
{
 public:
  //: "default" constructor
  boxm2_render_cone_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas, vil_image_view<float> * expected, vil_image_view<float>* vis_img)
  {
    alpha_data_   = new boxm2_data<BOXM2_GAMMA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    mog3_data_    = new boxm2_data<BOXM2_MOG3_GREY>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    expected_img_ = expected;
    vis_img_      = vis_img;

    vis_cum=1.0f;
    exp_cum=0.0f;
    vol_cum=0.0f;

    return true;
  }

  inline bool step_cell(float volume, int index, unsigned i, unsigned j)
  {
    //grab voxel alpha and intensity
    boxm2_data<BOXM2_GAMMA>::datatype alpha = alpha_data_->data()[index];
    float voxel_int = boxm2_processor_type<BOXM2_MOG3_GREY>::type::expected_color(mog3_data_->data()[index]);
    //probability that this voxel is occupied by surface
    float temp =std::exp(-alpha*volume);

    //weighted intensity for this voxel
    exp_cum += voxel_int*volume;
    vis_cum *= temp;
    vol_cum += volume;
    //probability that current cell is visible
    return true;
  }

  inline bool compute_ball_properties(unsigned i, unsigned j)
  {
      float ei  = (*expected_img_)(i,j);
      float vis = (*vis_img_)(i,j);

      //expected intensity is Visibility * Weighted Intensity * Occupancy
      if(vol_cum>1e-12f) ei += vis * (1-vis_cum)* exp_cum/vol_cum ;


      (*expected_img_)(i,j) = ei;
      (*vis_img_)(i,j)=(vis*vis_cum);

      vis_cum=1.0f;
      exp_cum=0.0f;
      vol_cum=0.0f;


      return true;
  }
  inline bool redistribute(float vol, int index){return true;}

 private:
     float vis_cum;
     float exp_cum;
     float vol_cum;
     boxm2_data<BOXM2_GAMMA> * alpha_data_;
     boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
     vil_image_view<float> *expected_img_;
     vil_image_view<float> *vis_img_;
};


#endif
