#ifndef _droid_h_
#define _droid_h_

#include "harris_internals.h"
#include <vil/vil_memory_image_of.h>

/*namespace*/class droid  {
public:
  typedef unsigned char byte;
  typedef vil_memory_image_of<bool>  GL_BOOL_ARRAY_STR;
  typedef vil_memory_image_of<float> GL_FLOAT_ARRAY_STR;
  typedef vil_memory_image_of<byte>  GL_BYTE_ARRAY_STR;
  typedef vil_memory_image_of<int>   GL_INT_ARRAY_STR;
  
  static
  void compute_gradx_grady (GL_WINDOW_STR      *_window_str,
			    GL_BYTE_ARRAY_STR  *image_ptr,
			    GL_INT_ARRAY_STR   *image_gradx_ptr,
			    GL_INT_ARRAY_STR   *image_grady_ptr);
  static
  void compute_fxx_fxy_fyy (GL_WINDOW_STR      *_window_str,
			    GL_INT_ARRAY_STR   *image_gradx_ptr,
			    GL_INT_ARRAY_STR   *image_grady_ptr,
			    GL_FLOAT_ARRAY_STR *image_fxx_ptr,
			    GL_FLOAT_ARRAY_STR *image_fxy_ptr,
			    GL_FLOAT_ARRAY_STR *image_fyy_ptr);
  static
  float compute_cornerness (GL_WINDOW_STR      *_window_str,
			    GL_FLOAT_ARRAY_STR *image_fxx_ptr,
			    GL_FLOAT_ARRAY_STR *image_fxy_ptr,
			    GL_FLOAT_ARRAY_STR *image_fyy_ptr,
			    float scale,
			    GL_FLOAT_ARRAY_STR *pixel_cornerness);
  static  
  int find_corner_maxima (float corner_min,
			  GL_WINDOW_STR      *_window_str,
			  GL_FLOAT_ARRAY_STR *pixel_cornerness,
			  GL_BOOL_ARRAY_STR  *image_corner_max_ptr);
  static
  int find_local_maxima(float min,int winsize,
			int x1,int y1,
			int x2,int y2,
			GL_FLOAT_ARRAY_STR  *bitmap,
			GL_BOOL_ARRAY_STR  *max_p);
  static
  float compute_corner_min (float /*corner_min*/, 
			    float corner_max,
			    int corner_count_max,
			    GL_WINDOW_STR      *_window_str,
			    GL_FLOAT_ARRAY_STR *pixel_cornerness,
			    GL_BOOL_ARRAY_STR  *image_corner_max_ptr);

  static
  bool compute_subpixel_max (GL_FLOAT_ARRAY_STR *pixel_cornerness, 
			     int row,
			     int col,
			     double &x,double &y,
			     bool pab_emulate);
};
				  
#endif
