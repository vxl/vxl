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
                        float block_len, float& vol_alpha, float& intensity_norm, float& weighted_int)
  {
    //grab voxel alpha and intensity
    boxm2_data<BOXM2_ALPHA>::datatype alpha = alpha_data_->data()[index];
    float voxel_int = boxm2_data_traits<BOXM2_MOG3_GREY>::processor::expected_color(mog3_data_->data()[index]);

    //probability that this voxel is occupied by surface
    float cell_occupancy_prob = (1.0-vcl_exp(-alpha*volume*block_len));     
    
    //weighted intensity for this voxel
    weighted_int += cell_occupancy_prob * voxel_int; 
    intensity_norm += cell_occupancy_prob; 

    //probability that current cell is visible
    //float curr_p = cell_occupancy_prob*vis;
    vol_alpha += (-alpha*volume*block_len); 
    
    //current vis/expected intensity
    //float vis=(*vis_img_)(i,j);
    //float exp_int=(*expected_img_)(i,j);
/*
    if(i==7 && j==6) {
      vcl_cout<<"data index: "<<index<<" at ("<<i<<','<<j<<") : "<<vcl_endl;
      vcl_cout<<"  voxel contrib: "<<contr<<vcl_endl;
      vcl_cout<<"  cell occ prob: "<<cell_occupancy_prob<<vcl_endl;
      vcl_cout<<"  curr prob    : "<<curr_p<<vcl_endl;
      vcl_cout<<"  intersect vol: "<<volume<<vcl_endl;
      vcl_cout<<"  cell vis     : "<<vis<<vcl_endl;
    }  
*/
/*
    exp_int += contr; 
    (*expected_img_)(i,j) = exp_int;
*/
    return true;
  }
  
  inline bool update_vis(float sphere_occ_prob, unsigned i, unsigned j) {
    float vis = (*vis_img_)(i,j); 
    vis *= (1.0-sphere_occ_prob); 
    (*vis_img_)(i,j) = vis; 
	return true;
  }
  
  inline bool update_expected_int(float expected_int, float sphere_occ_prob, unsigned i, unsigned j) {
    int ei = (*expected_img_)(i,j); 
    float vis = (*vis_img_)(i,j); 

    ei += vis * expected_int * sphere_occ_prob; 
    (*expected_img_)(i,j) = ei;
	return true;
  }
  
 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  vil_image_view<float> *expected_img_;
  vil_image_view<float> *vis_img_;
};

//: Functor class to normalize expected image
/*
class normalize_intensity
{
 public:
  normalize_intensity() {}

  void operator()(float mask, float &pix) const
  {
    pix+=mask*0.5f;
  }
};
*/

#endif
