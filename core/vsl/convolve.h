#ifndef _convolve_h_
#define _convolve_h_
//

#include "harris_internals.h"
#include <vil/vil_memory_image_of.h>

struct GL_WINDOW_STR;
typedef vil_memory_image_of<float> GL_FLOAT_ARRAY_STR;

// holds a convolution kernel ?
#define GL_STATIC_DOUBLE_TABLE_SIZE 40
struct GL_STATIC_DOUBLE_TABLE_STR {
  int count;
  double array [GL_STATIC_DOUBLE_TABLE_SIZE];
};

class convolve {
public:
  // convolutions
  static float compute_gauss_weight (float gauss_sigma, int mask_index);

  static void float_mask  (GL_WINDOW_STR *window_ptr,
			   GL_WINDOW_STR *new_window_ptr,
			   GL_STATIC_DOUBLE_TABLE_STR *mask_ptr,
			   GL_FLOAT_ARRAY_STR *image_ptr, 
			   GL_FLOAT_ARRAY_STR *image_scratch_ptr);

  static void float_mask2 (GL_WINDOW_STR *window_ptr, 
			   GL_STATIC_DOUBLE_TABLE_STR *mask_ptr,
			   GL_FLOAT_ARRAY_STR *image_ptr, 
			   GL_FLOAT_ARRAY_STR *image_scratch_ptr);

  static void float_mask3 (GL_WINDOW_STR *window_ptr, 
			   GL_STATIC_DOUBLE_TABLE_STR *mask_ptr,
			   GL_FLOAT_ARRAY_STR *image_ptr, 
			   GL_FLOAT_ARRAY_STR *image_scratch_ptr);

  static void float_mask4 (GL_WINDOW_STR *window_ptr,
			   GL_STATIC_DOUBLE_TABLE_STR *mask_ptr,
			   GL_FLOAT_ARRAY_STR *image_ptr,
			   GL_FLOAT_ARRAY_STR *image_scratch_ptr);

  static void float_mask5 (GL_WINDOW_STR *window_ptr, 
			   GL_STATIC_DOUBLE_TABLE_STR *mask_ptr,
			   GL_FLOAT_ARRAY_STR *image_ptr, 
			   GL_FLOAT_ARRAY_STR *image_scratch_ptr);

  static void float_mask6 (GL_WINDOW_STR *window_ptr, 
			   GL_STATIC_DOUBLE_TABLE_STR *mask_ptr,
			   GL_FLOAT_ARRAY_STR *image_ptr, 
			   GL_FLOAT_ARRAY_STR *image_scratch_ptr);

  static void create_gaussian (double gauss_sigma,
			       GL_STATIC_DOUBLE_TABLE_STR *mask_ptr);
  
  static void average_mask(int width, GL_WINDOW_STR *window_ptr, 
			   GL_STATIC_DOUBLE_TABLE_STR *mask_ptr,
			   GL_FLOAT_ARRAY_STR *image_ptr, 
			   GL_FLOAT_ARRAY_STR *image_scratch_ptr);
};

#endif
