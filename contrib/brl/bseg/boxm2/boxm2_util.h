#ifndef boxm2_util_h
#define boxm2_util_h
//:
// \file
#include <vcl_iostream.h>
#include <vpgl/vpgl_proj_camera.h>

//: Utility class with static methods
class boxm2_util
{
  public:
    
    //: given a buffer and a length, randomly permutes the members of buffer
    static void random_permutation(int* buffer, int size); 

    static float clamp(float x, float a, float b);

};

#endif // boxm2_util_h
