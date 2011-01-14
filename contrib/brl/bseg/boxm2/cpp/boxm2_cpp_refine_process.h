#ifndef boxm2_cpp_refine_process_h
#define boxm2_cpp_refine_process_h
//:
// \file
#include "boxm2_cpp_process_base.h"
#include <vil/vil_image_view.h>

class boxm2_cpp_refine_process : public boxm2_cpp_process_base
{
  public:
    boxm2_cpp_refine_process() {}

    //: process init and execute
    bool init() { return true; }
    bool execute(vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output);
    bool clean();

  private:

};

#endif
