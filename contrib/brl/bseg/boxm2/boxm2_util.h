#ifndef boxm2_util_h
#define boxm2_util_h
//:
// \file
#include <vcl_iostream.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>

//: Utility class with static methods
class boxm2_util
{
  public:
    
    //: given a buffer and a length, randomly permutes the members of buffer
    static void random_permutation(int* buffer, int size); 

    static float clamp(float x, float a, float b);
    
    static vcl_vector<vpgl_perspective_camera<double>* > cameras_from_directory(vcl_string dir); 
    static vcl_vector<vcl_string> images_from_directory(vcl_string dir);
    static vcl_vector<vcl_string> camfiles_from_directory(vcl_string dir);

    static vbl_array_2d<vil_image_view<vxl_byte>* >  render_upper_hemisphere(int num_az, 
                                                                             int num_incline, 
                                                                             double radius,
    
                                                                             unsigned ni,
                                                                             unsigned nj,
                                                                             vgl_box_3d<double> bb);

    static vil_image_view_base_sptr prepare_input_image(vil_image_view_base_sptr loaded_image);
    static vil_image_view_base_sptr prepare_input_image(vcl_string filename) { 
      vil_image_view_base_sptr loaded_image = vil_load(filename.c_str());
      return boxm2_util::prepare_input_image(loaded_image);
    }
    
    static vil_rgba<vxl_byte> mean_pixel(vil_image_view<vil_rgba<vxl_byte> >& img); 

    static vpgl_perspective_camera<double>* construct_camera(double elevation, 
                                                              double azimuth, 
                                                              double radius, 
                                                              unsigned ni,
                                                              unsigned nj,
                                                              vgl_box_3d<double> bb,
                                                              bool fit_bb=true);
                                                              
    static int find_nearest_cam(vgl_vector_3d<double>& normal, 
                                vcl_vector<vpgl_perspective_camera<double>* >& cams); 
                                                              
    static bool copy_file(vcl_string file, vcl_string dest); 
    
    //Will be used to specify rows and cols for JavaScript App
    static bool generate_html(int width, int height, int nrows, int ncols, vcl_string dest); 
    static bool generate_jsfunc(vbl_array_2d<vcl_string> img_files, vcl_string dest); 

};

#endif // boxm2_util_h
