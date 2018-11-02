#ifndef boxm2_normals_to_id_functor_h_
#define boxm2_normals_to_id_functor_h_
//:
// \file

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class boxm2_normals_to_id_functor
{
 public:
  typedef boxm2_data_traits<BOXM2_POINT>::datatype point_datatype;
  typedef boxm2_data_traits<BOXM2_NORMAL>::datatype normal_datatype;
  typedef boxm2_data_traits<BOXM2_VIS_SCORE>::datatype vis_score_datatype;
  typedef boxm2_data_traits<BOXM2_LABEL_SHORT>::datatype label_short_datatype;
  typedef boxm2_data_traits<BOXM2_ALPHA>::datatype alpha_datatype;

  //: "default" constructor
  boxm2_normals_to_id_functor() = default;

  bool init_data(std::vector<boxm2_data_base *> & datas, float nmag_t, float vis_t)
  {
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    point_data_=new boxm2_data<BOXM2_POINT>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    normal_data_=new boxm2_data<BOXM2_NORMAL>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    vis_score_data_=new boxm2_data<BOXM2_VIS_SCORE>(datas[3]->data_buffer(),datas[3]->buffer_length(),datas[3]->block_id());
    label_short_data_=new boxm2_data<BOXM2_LABEL_SHORT>(datas[4]->data_buffer(),datas[4]->buffer_length(),datas[4]->block_id());

    nmag_t_ = nmag_t;
    vis_t_  = vis_t;

    id_ = alpha_data_->block_id();
    return true;
  }

  inline bool process_cell(int index)
  {
    boxm2_data<BOXM2_ALPHA>::datatype & alpha=alpha_data_->data()[index]; // TODO - unused!
    boxm2_data<BOXM2_POINT>::datatype & point=point_data_->data()[index];
    boxm2_data<BOXM2_NORMAL>::datatype & normal=normal_data_->data()[index];
    boxm2_data<BOXM2_VIS_SCORE>::datatype & vis_score=vis_score_data_->data()[index]; // TODO - unused!
    boxm2_data<BOXM2_LABEL_SHORT>::datatype & label_short=label_short_data_->data()[index];

    if (normal[3] >= nmag_t_ && /*vis_score >= vis_t_ && */ point[3] != -1)
    {
        if ( std::fabs(normal[2]) > std::cos(vnl_math::pi/4)) // horizontal surface
        {
            label_short = 1;
        }
        else if (  std::fabs(normal[2]) <  std::cos(vnl_math::pi/4)) // vertical surface
        {
            int thetaindex = (int)std::floor((std::atan2(normal[1],normal[0]) + vnl_math::pi)/(vnl_math::pi/4));
            label_short = 2+(short)thetaindex;
        }
        else        // neither
        {
            label_short = 100;
        }
    }
    else {
        label_short = 100;
    }
    return true;
  }

 private:
  boxm2_data<BOXM2_ALPHA>* alpha_data_;
  boxm2_data<BOXM2_POINT>* point_data_;
  boxm2_data<BOXM2_VIS_SCORE>* vis_score_data_;
  boxm2_data<BOXM2_NORMAL>* normal_data_;
  boxm2_data<BOXM2_LABEL_SHORT>* label_short_data_;
  float nmag_t_ ;
  float vis_t_;
  boxm2_block_id id_;
};

#endif
