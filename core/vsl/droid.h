#ifndef _droid_h_
#define _droid_h_

#include <vsl/vsl_roi_window.h>
#include <vil/vil_memory_image_of.h>

/*namespace*/class droid  {
public:
  static
  void compute_gradx_grady (vsl_roi_window      *_window_str,
			    vil_byte_buffer    *image_ptr,
			    vil_int_buffer     *image_gradx_ptr,
			    vil_int_buffer     *image_grady_ptr);
  static
  void compute_fxx_fxy_fyy (vsl_roi_window      *_window_str,
			    vil_int_buffer     *image_gradx_ptr,
			    vil_int_buffer     *image_grady_ptr,
			    vil_float_buffer *image_fxx_ptr,
			    vil_float_buffer *image_fxy_ptr,
			    vil_float_buffer *image_fyy_ptr);
  static
  float compute_cornerness (vsl_roi_window      *_window_str,
			    vil_float_buffer *image_fxx_ptr,
			    vil_float_buffer *image_fxy_ptr,
			    vil_float_buffer *image_fyy_ptr,
			    float scale,
			    vil_float_buffer *pixel_cornerness);
  static  
  int find_corner_maxima (float corner_min,
			  vsl_roi_window      *_window_str,
			  vil_float_buffer *pixel_cornerness,
			  vil_bool_buffer  *image_corner_max_ptr);
  static
  int find_local_maxima(float min,int winsize,
			int x1,int y1,
			int x2,int y2,
			vil_float_buffer  *bitmap,
			vil_bool_buffer  *max_p);
  static
  float compute_corner_min (float /*corner_min*/, 
			    float corner_max,
			    int corner_count_max,
			    vsl_roi_window      *_window_str,
			    vil_float_buffer *pixel_cornerness,
			    vil_bool_buffer  *image_corner_max_ptr);

  static
  bool compute_subpixel_max (vil_float_buffer *pixel_cornerness, 
			     int row,
			     int col,
			     double &x,double &y,
			     bool pab_emulate);
};
				  
#endif
