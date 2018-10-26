#ifndef boxm2_vis_probe_functor_h
#define boxm2_vis_probe_functor_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>

class boxm2_vis_probe_functor
{
 public:
  //: "default" constructor
  boxm2_vis_probe_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas, float * vis)
  {
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    vis_ = vis;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j, float abs_depth = 0.0)
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    (*vis_)*=std::exp(-alpha*seg_len);
    return true;
  }
 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  float * vis_;
};


#endif
