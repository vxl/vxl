#ifndef droid_h_
#define droid_h_

// .NAME droid
// .INCLUDE osl/internals/droid.h
// .FILE internals/droid.cxx

#include <osl/osl_roi_window.h>
#include <vil1/vil1_memory_image_of.h>
#include <vxl_config.h>

/*namespace*/class droid
{
 public:
  static
  void compute_gradx_grady (osl_roi_window                 *window_str,
                            vil1_memory_image_of<vxl_byte> * image_ptr,
                            vil1_memory_image_of<int> *image_gradx_ptr,
                            vil1_memory_image_of<int> *image_grady_ptr);
  static
  void compute_fxx_fxy_fyy (osl_roi_window              *window_str,
                            vil1_memory_image_of<int>   *image_gradx_ptr,
                            vil1_memory_image_of<int>   *image_grady_ptr,
                            vil1_memory_image_of<float> *image_fxx_ptr,
                            vil1_memory_image_of<float> *image_fxy_ptr,
                            vil1_memory_image_of<float> *image_fyy_ptr);
  static
  float compute_cornerness (osl_roi_window              *window_str,
                            vil1_memory_image_of<float> *image_fxx_ptr,
                            vil1_memory_image_of<float> *image_fxy_ptr,
                            vil1_memory_image_of<float> *image_fyy_ptr,
                            float scale,
                            vil1_memory_image_of<float> *pixel_cornerness);
  static
  int find_corner_maxima (float corner_min,
                          osl_roi_window              *window_str,
                          vil1_memory_image_of<float> *pixel_cornerness,
                          vil1_memory_image_of<bool>  *image_corner_max_ptr);
  static
  int find_local_maxima(float min,int winsize,
                        int x1,int y1,
                        int x2,int y2,
                        vil1_memory_image_of<float>  *bitmap,
                        vil1_memory_image_of<bool>   *max_p);
  static
  float compute_corner_min (float /*corner_min*/,
                            float corner_max,
                            int corner_count_max,
                            osl_roi_window              *window_str,
                            vil1_memory_image_of<float> *pixel_cornerness,
                            vil1_memory_image_of<bool>  *image_corner_max_ptr);

  static
  bool compute_subpixel_max (vil1_memory_image_of<float> *pixel_cornerness,
                             int row,
                             int col,
                             double &x,double &y,
                             bool pab_emulate);
};

#endif // droid_h_
