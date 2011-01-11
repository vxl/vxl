#ifndef boxm2_cpp_update_process_h
#define boxm2_cpp_update_process_h
//:
// \file
#include "boxm2_cpp_process_base.h"
#include <vil/vil_image_view.h>

class boxm2_cpp_update_process : public boxm2_cpp_process_base
{
  public:
    boxm2_cpp_update_process() : image_(0), vis_img_(0) {}

    //: process init and execute
    bool init() { return true; }
    bool execute(vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);
    bool clean();
    //: update specific method
    bool set_image( vil_image_view<float>* img ) { image_ = img; return true; }
    //: get image
    vil_image_view<float>* image() { return image_; }

  private:
    //: image (so it isn't created over and over)
    vil_image_view<float> * image_;

    //: visibility image
    vil_image_view<float> * vis_img_;
};

#endif
