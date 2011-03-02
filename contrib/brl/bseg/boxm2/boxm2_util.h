#ifndef boxm2_util_h
#define boxm2_util_h
//:
// \file
#include <vcl_iostream.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_view.h>

//: Utility class with static methods
class boxm2_util
{
  public:
    
    //: given a buffer and a length, randomly permutes the members of buffer
    static void random_permutation(int* buffer, int size); 

    static float clamp(float x, float a, float b);
    
    static vbl_array_2d<vil_image_view<vxl_byte>* >  render_upper_hemisphere(int num_az, 
                                                                             int num_incline, 
                                                                             double radius,
                                                                             unsigned ni,
                                                                             unsigned nj,
                                                                             vgl_box_3d<double> bb);

    static vpgl_perspective_camera<double>* construct_camera(double elevation, 
                                                              double azimuth, 
                                                              double radius, 
                                                              unsigned ni,
                                                              unsigned nj,
                                                              vgl_box_3d<double> bb,
                                                              bool fit_bb=true);
                                                              
    static bool copy_file(vcl_string file, vcl_string dest); 
    
    //Will be used to specify rows and cols for JavaScript App
    static bool generate_html(int width, int height, int nrows, int ncols, vcl_string dest); 
    static bool generate_jsfunc(vbl_array_2d<vcl_string> img_files, vcl_string dest); 


};

#endif // boxm2_util_h
