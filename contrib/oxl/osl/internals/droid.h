#ifndef _droid_h_
#define _droid_h_

// .NAME droid
// .INCLUDE osl/internals/droid.h
// .FILE internals/droid.cxx

#include <osl/osl_roi_window.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_byte.h>

/*namespace*/class droid  {
public:
  static
  void compute_gradx_grady (osl_roi_window      *_window_str,
                            vil_memory_image_of<vil_byte>    *image_ptr,
                            vil_memory_image_of<int>     *image_gradx_ptr,
                            vil_memory_image_of<int>     *image_grady_ptr);
  static
  void compute_fxx_fxy_fyy (osl_roi_window      *_window_str,
                            vil_memory_image_of<int>     *image_gradx_ptr,
                            vil_memory_image_of<int>     *image_grady_ptr,
                            vil_memory_image_of<float> *image_fxx_ptr,
                            vil_memory_image_of<float> *image_fxy_ptr,
                            vil_memory_image_of<float> *image_fyy_ptr);
  static
  float compute_cornerness (osl_roi_window      *_window_str,
                            vil_memory_image_of<float> *image_fxx_ptr,
                            vil_memory_image_of<float> *image_fxy_ptr,
                            vil_memory_image_of<float> *image_fyy_ptr,
                            float scale,
                            vil_memory_image_of<float> *pixel_cornerness);
  static
  int find_corner_maxima (float corner_min,
                          osl_roi_window      *_window_str,
                          vil_memory_image_of<float> *pixel_cornerness,
                          vil_memory_image_of<bool>  *image_corner_max_ptr);
  static
  int find_local_maxima(float min,int winsize,
                        int x1,int y1,
                        int x2,int y2,
                        vil_memory_image_of<float>  *bitmap,
                        vil_memory_image_of<bool>  *max_p);
  static
  float compute_corner_min (float /*corner_min*/,
                            float corner_max,
                            int corner_count_max,
                            osl_roi_window      *_window_str,
                            vil_memory_image_of<float> *pixel_cornerness,
                            vil_memory_image_of<bool>  *image_corner_max_ptr);

  static
  bool compute_subpixel_max (vil_memory_image_of<float> *pixel_cornerness,
                             int row,
                             int col,
                             double &x,double &y,
                             bool pab_emulate);
};

#endif // _droid_h_
