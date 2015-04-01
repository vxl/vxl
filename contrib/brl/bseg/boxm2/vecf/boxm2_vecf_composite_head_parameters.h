#ifndef boxm2_vecf_composite_head_parameters_h_
#define boxm2_vecf_composite_head_parameters_h_

#include  <vgl/vgl_vector_3d.h>

class boxm2_vecf_composite_head_parameters
{
  public:
    //: default constructor
    boxm2_vecf_composite_head_parameters():
      head_scale_(1.0, 1.0, 1.0),
      look_dir_(0.0, 0.0, 1.0)
  {}

    //: full constructor
    boxm2_vecf_composite_head_parameters(vgl_vector_3d<double> head_scale,
                                         vgl_vector_3d<double> look_dir) :
      head_scale_(head_scale),
      look_dir_(look_dir)
  {}

    //: head scaling in x,y,z to allow for coarse shape fitting
    vgl_vector_3d<double> head_scale_;

    //: gaze direction of the eyes
    vgl_vector_3d<double> look_dir_;

};

#endif
