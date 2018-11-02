// This is brl/bseg/boxm2/cpp/algo/boxm2_mog6_view_processor.h
#ifndef boxm2_mog6_view_processor_h_
#define boxm2_mog6_view_processor_h_


#include <iostream>
#include <vector>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class  boxm2_mog6_view_processor
{
 public:
     static float expected_color( vnl_vector_fixed<float, 16> app_model)
       { return (app_model[0] + app_model[2] + app_model[4] + app_model[6] + app_model[8] + app_model[10])/6; }
};


class  boxm2_mog6_view_compact_processor
{
 public:
     static float expected_color( vnl_vector_fixed<unsigned char, 16> app_model)
       { return ((float)app_model[0] + (float)app_model[2] + (float)app_model[4] + (float)app_model[6] +
                   (float)app_model[8] + (float)app_model[10]) / (6 * 255.0f) ; }
};

#endif // boxm2_mog6_view_processor_h_
