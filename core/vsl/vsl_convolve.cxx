#include "vsl_convolve.h"
#include <vsl/vsl_roi_window.h>

//-----------------------------------------------------------------------------

// Depending on the mask size, this routine will call the 
// appropriate float_mask? routine.
void vsl_convolve (vsl_roi_window const *window, 
		   vsl_1d_half_kernel<double> const *mask,
		   vil_memory_image_of<float> *image,
		   vil_memory_image_of<float> *scratch)
{
  switch (mask->count) {
  case 2:
    vsl_convolve2 (window, mask, image, scratch);
    break;
  case 3:
    vsl_convolve3 (window, mask, image, scratch);
    break;
  case 4:
    vsl_convolve4 (window, mask, image, scratch);
    break;
  case 5:
    vsl_convolve5 (window, mask, image, scratch);
    break;
  case 6:
    vsl_convolve6 (window, mask, image, scratch);
    break;
  default:
    vsl_convolven (window, mask, image, scratch);
    break;
  }
}

//-----------------------------------------------------------------------------

//: A convolution routine for masks of size 2.
void vsl_convolve2 (vsl_roi_window const *window,
		    vsl_1d_half_kernel<double> const *mask,
		    vil_memory_image_of<float> *image,
		    vil_memory_image_of<float> *scratch)
{
  int row_min = window->row_start_index;
  int col_min = window->col_start_index+mask->count-1;
  int row_max = window->row_end_index;
  int col_max = window->col_end_index-(mask->count-1);
    
  float mask0 = mask->array [0];
  float mask1 = mask->array [1];
    
  //	horizontal smoothing.
    
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*scratch) [row][col] 
	= (*image) [row][col] * mask0
	+((*image) [row][col-1]+(*image) [row][col+1]) * mask1;
    }
  }
    
  //	vertical smoothing on top of the horizontal smoothing.
    
  row_min += mask->count-1;
  row_max -= mask->count-1;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image) [row][col] 
	= (*scratch) [row][col] * mask0
	+((*scratch) [row-1][col]+(*scratch) [row+1][col]) * mask1;
    }
  }
}



//-----------------------------------------------------------------------------

//: A convolution routine for masks of size 3.
void vsl_convolve3 (vsl_roi_window const *window,
		    vsl_1d_half_kernel<double> const *mask,
		    vil_memory_image_of<float> *image,
		    vil_memory_image_of<float> *scratch)
{
  int row_min = window->row_start_index;
  int col_min = window->col_start_index+mask->count-1;
  int row_max = window->row_end_index;
  int col_max = window->col_end_index-(mask->count-1);
    
  float mask0 = mask->array [0];
  float mask1 = mask->array [1];
  float mask2 = mask->array [2];
    
  //	horizontal smoothing.
    
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*scratch) [row][col] 
	=  (*image) [row][col] * mask0
	+ ((*image) [row][col-1]+(*image) [row][col+1]) * mask1
	+ ((*image) [row][col-2]+(*image) [row][col+2]) * mask2;
    }
  }
    
  //	vertical smoothing on top of the horizontal smoothing.
    
  row_min += mask->count-1;
  row_max -= mask->count-1;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image) [row][col] 
	=  (*scratch) [row][col] * mask0
	+ ((*scratch) [row-1][col]+(*scratch) [row+1][col]) * mask1
	+ ((*scratch) [row-2][col]+(*scratch) [row+2][col]) * mask2;
    }
  }
}



//-----------------------------------------------------------------------------

//: -- A convolution routine for masks of size 4.
void vsl_convolve4 (vsl_roi_window const *window,
		    vsl_1d_half_kernel<double> const *mask,
		    vil_memory_image_of<float> *image,
		    vil_memory_image_of<float> *scratch)
{
  int row_min = window->row_start_index;
  int col_min = window->col_start_index+mask->count-1;
  int row_max = window->row_end_index;
  int col_max = window->col_end_index-(mask->count-1);
    
  float mask0 = mask->array [0];
  float mask1 = mask->array [1];
  float mask2 = mask->array [2];
  float mask3 = mask->array [3];
    
  //	horizontal smoothing.
    
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*scratch) [row][col] 
	= (*image) [row][col] * mask0
	+((*image) [row][col-1]+(*image) [row][col+1]) * mask1
	+((*image) [row][col-2]+(*image) [row][col+2]) * mask2
	+((*image) [row][col-3]+(*image) [row][col+3]) * mask3;
    }
  }
    
  //	vertical smoothing on top of the horizontal smoothing.
    
  row_min += mask->count-1;
  row_max -= mask->count-1;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image) [row][col] 
	= (*scratch) [row][col] * mask0
	+((*scratch) [row-1][col]+(*scratch) [row+1][col]) * mask1
	+((*scratch) [row-2][col]+(*scratch) [row+2][col]) * mask2
	+((*scratch) [row-3][col]+(*scratch) [row+3][col]) * mask3;
    }
  }
}



//-----------------------------------------------------------------------------

//: A convolution routine for masks of size 5.
void vsl_convolve5 (vsl_roi_window const *window,
		    vsl_1d_half_kernel<double> const *mask,
		    vil_memory_image_of<float> *image,
		    vil_memory_image_of<float> *scratch)
{
  int row_min = window->row_start_index;
  int col_min = window->col_start_index+mask->count-1;
  int row_max = window->row_end_index;
  int col_max = window->col_end_index-(mask->count-1);
    
  float mask0 = mask->array [0];
  float mask1 = mask->array [1];
  float mask2 = mask->array [2];
  float mask3 = mask->array [3];
  float mask4 = mask->array [4];
    
  //	horizontal smoothing.
    
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*scratch) [row][col] 
	=  (*image) [row][col] * mask0
	+ ((*image) [row][col-1]+(*image) [row][col+1]) * mask1
	+ ((*image) [row][col-2]+(*image) [row][col+2]) * mask2
	+ ((*image) [row][col-3]+(*image) [row][col+3]) * mask3
	+ ((*image) [row][col-4]+(*image) [row][col+4]) * mask4;
    }
  }
    
  //	vertical smoothing on top of the horizontal smoothing.
    
  row_min += mask->count-1;
  row_max -= mask->count-1;

  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image) [row][col] 
	=  (*scratch) [row][col] * mask0
	+ ((*scratch) [row-1][col]+(*scratch) [row+1][col]) * mask1
	+ ((*scratch) [row-2][col]+(*scratch) [row+2][col]) * mask2
	+ ((*scratch) [row-3][col]+(*scratch) [row+3][col]) * mask3
	+ ((*scratch) [row-4][col]+(*scratch) [row+4][col]) * mask4;
    }
  }
}



//-----------------------------------------------------------------------------

//: A convolution routine for masks of size 7.
void vsl_convolve6 (vsl_roi_window const *window,
		    vsl_1d_half_kernel<double> const *mask,
		    vil_memory_image_of<float> *image,
		    vil_memory_image_of<float> *scratch)
{
  int row_min = window->row_start_index;
  int col_min = window->col_start_index+mask->count-1;
  int row_max = window->row_end_index;
  int col_max = window->col_end_index-(mask->count-1);
  
  float mask0 = mask->array [0];
  float mask1 = mask->array [1];
  float mask2 = mask->array [2];
  float mask3 = mask->array [3];
  float mask4 = mask->array [4];
  float mask5 = mask->array [5];
  
  //	horizontal smoothing.
  
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*scratch) [row][col] 
	=  (*image) [row][col] * mask0
	+ ((*image) [row][col-1]+(*image) [row][col+1]) * mask1
	+ ((*image) [row][col-2]+(*image) [row][col+2]) * mask2
	+ ((*image) [row][col-3]+(*image) [row][col+3]) * mask3
	+ ((*image) [row][col-4]+(*image) [row][col+4]) * mask4
	+ ((*image) [row][col-5]+(*image) [row][col+5]) * mask5;
    }
  }
    
  //	vertical smoothing on top of the horizontal smoothing.
  
  row_min += mask->count-1;
  row_max -= mask->count-1;
  
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image) [row][col] 
	=  (*scratch) [row][col] * mask0
	+ ((*scratch) [row-1][col]+(*scratch) [row+1][col]) * mask1
	+ ((*scratch) [row-2][col]+(*scratch) [row+2][col]) * mask2
	+ ((*scratch) [row-3][col]+(*scratch) [row+3][col]) * mask3
	+ ((*scratch) [row-4][col]+(*scratch) [row+4][col]) * mask4
	+ ((*scratch) [row-5][col]+(*scratch) [row+5][col]) * mask5;
    }
  }
}

//--------------------------------------------------------------------------------

//: A convolution routine for masks of any size.
void vsl_convolven (vsl_roi_window const *window,
		    vsl_1d_half_kernel<double> const *mask,
		    vil_memory_image_of<float> *image,
		    vil_memory_image_of<float> *scratch)
{    
  int row_min = window->row_start_index;
  int col_min = window->col_start_index+mask->count-1;
  int row_max = window->row_end_index;
  int col_max = window->col_end_index-(mask->count-1);
  
  /* horizontal smoothing. */
  
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*scratch) [row][col] = (*image) [row][col] * mask->array [0];
      
      for (unsigned int mask_index = 1; mask_index < mask->count; mask_index++)
	(*scratch) [row][col]
	  += ((*image) [row][col-mask_index]+(*image) [row][col+mask_index]) 
	  *   mask->array [mask_index];
    }
  }
  
  /* vertical smoothing on top of the horizontal smoothing. */
  
  row_min += mask->count-1;
  row_max -= mask->count-1;
  
  for (int row = row_min; row < row_max; row++) {
    for (int col = col_min; col < col_max; col++) {
      (*image) [row][col] = (*scratch) [row][col] * mask->array [0];
      
      for (unsigned int mask_index = 1; mask_index < mask->count; mask_index++)
	(*image) [row][col]
	  += ((*scratch) [row-mask_index][col]+
	      (*scratch) [row+mask_index][col])
	  * mask->array [mask_index];
    }
  }
}

//--------------------------------------------------------------------------------
