#include "convolve.h"
#include "harris_internals.h"
#include <vcl/vcl_cmath.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cassert.h>

#define LENGTH_OF_ARRAY(A) (sizeof(A) / sizeof (A[0]))

//-----------------------------------------------------------------------------

//: helper routine used by create_gaussian
float convolve::compute_gauss_weight (float sigma, int mask_index) {
  float sum = 0;
  
  for (int repeat = 0; repeat < 6; repeat++) {
    float x = mask_index-0.5 + 0.2*repeat;
    sum += exp( - (x*x) / (2 * sigma * sigma));
  }
  
  return sum / 6;
}

//: routine to generate a gaussian convolution mask.
void convolve::create_gaussian (double gauss_sigma, 
				GL_STATIC_DOUBLE_TABLE_STR *mask_ptr)
{
  // stop adding entries to the Gauss mask when the size of an entry is less than this : */
  float const CN_GAUSS_CUTOFF_VALUE = 0.02;
  
  int mask_index = 0;

  float gauss_weight = compute_gauss_weight (gauss_sigma, mask_index);
  while (gauss_weight > CN_GAUSS_CUTOFF_VALUE) {
    mask_ptr->array [mask_index] = gauss_weight;
    ++ mask_index;
    
    gauss_weight = compute_gauss_weight (gauss_sigma, mask_index);
    
    if (mask_index == LENGTH_OF_ARRAY (mask_ptr->array)) {
      cerr << "mask size equal to maximum-must recompile with new mask size";
      assert (0);	    
    }	
  }
  
  mask_ptr->count = mask_index;
  
  float total_mask_value = mask_ptr->array [0];
  for (mask_index = 1; mask_index < mask_ptr->count; mask_index++) {
    total_mask_value += 2 * mask_ptr->array [mask_index];
  }
  
  for (mask_index = 0; mask_index < mask_ptr->count; mask_index++) {
    mask_ptr->array [mask_index] /= total_mask_value;
    
    if (mask_ptr->array [mask_index] < CN_GAUSS_CUTOFF_VALUE)
      mask_ptr->count = mask_index;
  }
}

//----------------------------------------------------------------------------

//: A convolution routine for masks of any size.
// If the mask size is 2,3,4,5 or 6, this routine will call one of the
// float_mask? routines.
void convolve::float_mask (GL_WINDOW_STR *window_ptr, 
			   GL_WINDOW_STR *new_window_ptr,
			   GL_STATIC_DOUBLE_TABLE_STR *mask_ptr,
			   GL_FLOAT_ARRAY_STR *image_ptr,
			   GL_FLOAT_ARRAY_STR *image_scratch_ptr)
{
  switch (mask_ptr->count) {
  case 2:
    float_mask2 (window_ptr, mask_ptr, image_ptr, image_scratch_ptr);
    break;
  case 3:
    float_mask3 (window_ptr, mask_ptr, image_ptr, image_scratch_ptr);
    break;
  case 4:
    float_mask4 (window_ptr, mask_ptr, image_ptr, image_scratch_ptr);
    break;
  case 5:
    float_mask5 (window_ptr, mask_ptr, image_ptr, image_scratch_ptr);
    break;
  case 6:
    float_mask6 (window_ptr, mask_ptr, image_ptr, image_scratch_ptr);
    break;
  default:
    {    
      int row_min = window_ptr->row_start_index;
      int col_min = window_ptr->col_start_index+mask_ptr->count-1;
      int row_max = window_ptr->row_end_index;
      int col_max = window_ptr->col_end_index-(mask_ptr->count-1);
      
      /* horizontal smoothing. */
      
      for (int row = row_min; row < row_max; row++) {
	for (int col = col_min; col < col_max; col++) {
	  (*image_scratch_ptr) [row][col] = (*image_ptr) [row][col] * mask_ptr->array [0];
	  
	  for (int mask_index = 1; mask_index < mask_ptr->count; mask_index++)
	    (*image_scratch_ptr) [row][col]
	      += ((*image_ptr) [row][col-mask_index]+(*image_ptr) [row][col+mask_index]) 
	      *   mask_ptr->array [mask_index];
	}
      }
      
      /* vertical smoothing on top of the horizontal smoothing. */
      
      row_min += mask_ptr->count-1;
      row_max -= mask_ptr->count-1;
      
      for (int row = row_min; row < row_max; row++) {
	for (int col = col_min; col < col_max; col++) {
	  (*image_ptr) [row][col] = (*image_scratch_ptr) [row][col] * mask_ptr->array [0];
	  
	  for (int mask_index = 1; mask_index < mask_ptr->count; mask_index++)
	    (*image_ptr) [row][col]
	      += ((*image_scratch_ptr) [row-mask_index][col]+
		  (*image_scratch_ptr) [row+mask_index][col])
	      * mask_ptr->array [mask_index];
	}
      }
    }
  }
  
  /* do this at the end because window might be the same as new window. */
  
  new_window_ptr->row_start_index = window_ptr->row_start_index+(mask_ptr->count-1);
  new_window_ptr->row_end_index = window_ptr->row_end_index-(mask_ptr->count-1);
  new_window_ptr->col_start_index = window_ptr->col_start_index+(mask_ptr->count-1);
  new_window_ptr->col_end_index = window_ptr->col_end_index-(mask_ptr->count-1);
}



//-----------------------------------------------------------------------------

//: A convolution routine for masks of size 2.
void convolve::float_mask2 (GL_WINDOW_STR *window_ptr,
			    GL_STATIC_DOUBLE_TABLE_STR *mask_ptr,
			    GL_FLOAT_ARRAY_STR *image_ptr,
			    GL_FLOAT_ARRAY_STR *image_scratch_ptr)
{
  int row_min = window_ptr->row_start_index;
  int col_min = window_ptr->col_start_index+mask_ptr->count-1;
  int row_max = window_ptr->row_end_index;
  int col_max = window_ptr->col_end_index-(mask_ptr->count-1);
    
  float mask0 = mask_ptr->array [0];
  float mask1 = mask_ptr->array [1];
    
  //	horizontal smoothing.
    
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image_scratch_ptr) [row][col] 
	= (*image_ptr) [row][col] * mask0
	+((*image_ptr) [row][col-1]+(*image_ptr) [row][col+1]) * mask1;
    }
  }
    
  //	vertical smoothing on top of the horizontal smoothing.
    
  row_min += mask_ptr->count-1;
  row_max -= mask_ptr->count-1;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image_ptr) [row][col] 
	= (*image_scratch_ptr) [row][col] * mask0
	+((*image_scratch_ptr) [row-1][col]+(*image_scratch_ptr) [row+1][col]) * mask1;
    }
  }
}



//-----------------------------------------------------------------------------

//: A convolution routine for masks of size 3.
void convolve::float_mask3 (GL_WINDOW_STR *window_ptr,
			    GL_STATIC_DOUBLE_TABLE_STR *mask_ptr,
			    GL_FLOAT_ARRAY_STR *image_ptr,
			    GL_FLOAT_ARRAY_STR *image_scratch_ptr)
{
  int row_min = window_ptr->row_start_index;
  int col_min = window_ptr->col_start_index+mask_ptr->count-1;
  int row_max = window_ptr->row_end_index;
  int col_max = window_ptr->col_end_index-(mask_ptr->count-1);
    
  float mask0 = mask_ptr->array [0];
  float mask1 = mask_ptr->array [1];
  float mask2 = mask_ptr->array [2];
    
  //	horizontal smoothing.
    
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image_scratch_ptr) [row][col] 
	=  (*image_ptr) [row][col] * mask0
	+ ((*image_ptr) [row][col-1]+(*image_ptr) [row][col+1]) * mask1
	+ ((*image_ptr) [row][col-2]+(*image_ptr) [row][col+2]) * mask2;
    }
  }
    
  //	vertical smoothing on top of the horizontal smoothing.
    
  row_min += mask_ptr->count-1;
  row_max -= mask_ptr->count-1;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image_ptr) [row][col] 
	=  (*image_scratch_ptr) [row][col] * mask0
	+ ((*image_scratch_ptr) [row-1][col]+(*image_scratch_ptr) [row+1][col]) * mask1
	+ ((*image_scratch_ptr) [row-2][col]+(*image_scratch_ptr) [row+2][col]) * mask2;
    }
  }
}



//-----------------------------------------------------------------------------

//: -- A convolution routine for masks of size 4.
void convolve::float_mask4 (GL_WINDOW_STR *window_ptr,
			    GL_STATIC_DOUBLE_TABLE_STR *mask_ptr,
			    GL_FLOAT_ARRAY_STR *image_ptr,
			    GL_FLOAT_ARRAY_STR *image_scratch_ptr)
{
  int row_min = window_ptr->row_start_index;
  int col_min = window_ptr->col_start_index+mask_ptr->count-1;
  int row_max = window_ptr->row_end_index;
  int col_max = window_ptr->col_end_index-(mask_ptr->count-1);
    
  float mask0 = mask_ptr->array [0];
  float mask1 = mask_ptr->array [1];
  float mask2 = mask_ptr->array [2];
  float mask3 = mask_ptr->array [3];
    
  //	horizontal smoothing.
    
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image_scratch_ptr) [row][col] 
	= (*image_ptr) [row][col] * mask0
	+((*image_ptr) [row][col-1]+(*image_ptr) [row][col+1]) * mask1
	+ ((*image_ptr) [row][col-2]+(*image_ptr) [row][col+2]) * mask2
	+((*image_ptr) [row][col-3]+(*image_ptr) [row][col+3]) * mask3;
    }
  }
    
  //	vertical smoothing on top of the horizontal smoothing.
    
  row_min += mask_ptr->count-1;
  row_max -= mask_ptr->count-1;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image_ptr) [row][col] 
	= (*image_scratch_ptr) [row][col] * mask0
	+((*image_scratch_ptr) [row-1][col]+(*image_scratch_ptr) [row+1][col]) * mask1
	+ ((*image_scratch_ptr) [row-2][col]+(*image_scratch_ptr) [row+2][col]) * mask2
	+((*image_scratch_ptr) [row-3][col]+(*image_scratch_ptr) [row+3][col]) * mask3;
    }
  }
}



//-----------------------------------------------------------------------------

//: A convolution routine for masks of size 5.
void convolve::float_mask5 (GL_WINDOW_STR *window_ptr,
			    GL_STATIC_DOUBLE_TABLE_STR *mask_ptr,
			    GL_FLOAT_ARRAY_STR *image_ptr,
			    GL_FLOAT_ARRAY_STR *image_scratch_ptr)
{
  int row_min = window_ptr->row_start_index;
  int col_min = window_ptr->col_start_index+mask_ptr->count-1;
  int row_max = window_ptr->row_end_index;
  int col_max = window_ptr->col_end_index-(mask_ptr->count-1);
    
  float mask0 = mask_ptr->array [0];
  float mask1 = mask_ptr->array [1];
  float mask2 = mask_ptr->array [2];
  float mask3 = mask_ptr->array [3];
  float mask4 = mask_ptr->array [4];
    
  //	horizontal smoothing.
    
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image_scratch_ptr) [row][col] 
	=  (*image_ptr) [row][col] * mask0
	+ ((*image_ptr) [row][col-1]+(*image_ptr) [row][col+1]) * mask1
	+ ((*image_ptr) [row][col-2]+(*image_ptr) [row][col+2]) * mask2
	+ ((*image_ptr) [row][col-3]+(*image_ptr) [row][col+3]) * mask3
	+ ((*image_ptr) [row][col-4]+(*image_ptr) [row][col+4]) * mask4;
    }
  }
    
  //	vertical smoothing on top of the horizontal smoothing.
    
  row_min += mask_ptr->count-1;
  row_max -= mask_ptr->count-1;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image_ptr) [row][col] 
	=  (*image_scratch_ptr) [row][col] * mask0
	+ ((*image_scratch_ptr) [row-1][col]+(*image_scratch_ptr) [row+1][col]) * mask1
	+ ((*image_scratch_ptr) [row-2][col]+(*image_scratch_ptr) [row+2][col]) * mask2
	+ ((*image_scratch_ptr) [row-3][col]+(*image_scratch_ptr) [row+3][col]) * mask3
	+ ((*image_scratch_ptr) [row-4][col]+(*image_scratch_ptr) [row+4][col]) * mask4;
    }
  }
}



//-----------------------------------------------------------------------------

//: A convolution routine for masks of size 7.
void convolve::float_mask6 (GL_WINDOW_STR *window_ptr,
			    GL_STATIC_DOUBLE_TABLE_STR *mask_ptr,
			    GL_FLOAT_ARRAY_STR *image_ptr,
			    GL_FLOAT_ARRAY_STR *image_scratch_ptr)
{
  int row_min = window_ptr->row_start_index;
  int col_min = window_ptr->col_start_index+mask_ptr->count-1;
  int row_max = window_ptr->row_end_index;
  int col_max = window_ptr->col_end_index-(mask_ptr->count-1);
  
  float mask0 = mask_ptr->array [0];
  float mask1 = mask_ptr->array [1];
  float mask2 = mask_ptr->array [2];
  float mask3 = mask_ptr->array [3];
  float mask4 = mask_ptr->array [4];
  float mask5 = mask_ptr->array [5];
  
  //	horizontal smoothing.
  
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image_scratch_ptr) [row][col] 
	=  (*image_ptr) [row][col] * mask0
	+ ((*image_ptr) [row][col-1]+(*image_ptr) [row][col+1]) * mask1
	+ ((*image_ptr) [row][col-2]+(*image_ptr) [row][col+2]) * mask2
	+ ((*image_ptr) [row][col-3]+(*image_ptr) [row][col+3]) * mask3
	+ ((*image_ptr) [row][col-4]+(*image_ptr) [row][col+4]) * mask4
	+ ((*image_ptr) [row][col-5]+(*image_ptr) [row][col+5]) * mask5;
    }
  }
    
  //	vertical smoothing on top of the horizontal smoothing.
  
  row_min += mask_ptr->count-1;
  row_max -= mask_ptr->count-1;
  
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image_ptr) [row][col] 
	=  (*image_scratch_ptr) [row][col] * mask0
	+ ((*image_scratch_ptr) [row-1][col]+(*image_scratch_ptr) [row+1][col]) * mask1
	+ ((*image_scratch_ptr) [row-2][col]+(*image_scratch_ptr) [row+2][col]) * mask2
	+ ((*image_scratch_ptr) [row-3][col]+(*image_scratch_ptr) [row+3][col]) * mask3
	+ ((*image_scratch_ptr) [row-4][col]+(*image_scratch_ptr) [row+4][col]) * mask4
	+ ((*image_scratch_ptr) [row-5][col]+(*image_scratch_ptr) [row+5][col]) * mask5;
    }
  }
}

//--------------------------------------------------------------------------------

