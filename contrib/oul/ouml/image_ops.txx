//-*-c++-*--------------------------------------------------------------
#ifndef ouml_image_ops_txx_
#define ouml_image_ops_txx_
/**
 * \file
 *
 * The whole shebang. Hopefully this will find and track eyes
 * predominantly using eyespiders.
 * \author Brendan McCane
 * Copyright (c) 2000 Brendan McCane
 * University of Otago, Dunedin, New Zealand
 * Reproduction rights limited as described in the COPYRIGHT file.
 */
//----------------------------------------------------------------------
#include "image_ops.h"
#include <vcl_cassert.h>

//----------------------------------------------------------------------
/** -= operator for images
 * 
 * An in place -= operator for images. Saves from having to copy any
 * data anywhere.
 *
 * \param im1: vil_memory_image_of<T> &, the image to be modified
 * \param im2: vil_memory_image_of<T> &, the image to subtract from
 * \return vil_memory_image_of<T> &: a reference to im1
 *
 * \author Brendan McCane
 */
//----------------------------------------------------------------------

template <class T>
vil_memory_image_of<T> &operator -=
(
  vil_memory_image_of<T> &im1,
  vil_memory_image_of<T> &im2
)
{
  assert(im1.width()==im2.width());
  assert(im1.height()==im2.height());

  // for efficiency considerations, instead of doing double
  // indirection each time a pixel needs to be accessed, access the
  // row of pixels for each y-value and then dereference and
  // increment these pointers as needed to access each pixel. I
  // realise this isn't exactly the best OO style, but efficiency is
  // probably very important here.
  T *im1_row, *im2_row; 
  int width = im1.width(), height = im1.height();
  for (int y=0; y<height; y++)
  {
    im1_row = im1[y];
    im2_row = im2[y];
    for (int x=0; x<width; x++)
    {
      (*im1_row) -= (*im2_row);
      im1_row++; im2_row++;
    }
  }

  return im1;
}

//----------------------------------------------------------------------
/** += operator for images for adding a constant factor
 * 
 * An in place += operator for images. Saves from having to copy any
 * data anywhere.
 *
 * \param im1: vil_memory_image_of<T> &, the image to be modified
 * \param constant_add: T, the amount to add to each pixel in the image
 * \return vil_memory_image_of<T> &: a reference to im1
 *
 * \author Brendan McCane
 */
//----------------------------------------------------------------------
template <class T>
vil_memory_image_of<T> &operator +=
(
  vil_memory_image_of<T> &im1,
  T constant_add
)
{
  // for efficiency considerations, instead of doing double
  // indirection each time a pixel needs to be accessed, access the
  // row of pixels for each y-value and then dereference and
  // increment these pointers as needed to access each pixel. I
  // realise this isn't exactly the best OO style, but efficiency is
  // probably very important here.
  T *im1_row;
  int width = im1.width(), height = im1.height();
  for (int y=0; y<height; y++)
  {
    im1_row = im1[y];
    for (int x=0; x<width; x++)
    {
      (*im1_row) += constant_add;
      im1_row++; 
    }
  }

  return im1;
}


//----------------------------------------------------------------------
/** threshold_abs
 * 
 * Threshold the passed in image but in an abs manner. That is, a
 * pixel is set if abs(x-y)>threshold. Pixels which satisfy the
 * condition are set to their corresponding counterparts in the
 * source_vals image, and pixels which fail the condition are set to
 * zero_val.
 *
 * \param image: vil_memory_image_of<T> &, the image to be modified
 * \param source_vals: vil_memory_image_of<T> &, the image to be modified
 * \param threshold: T, the threshold to apply
 * \param zero_val: T, the value to set the thresholded pixels to
 *
 * \author Brendan McCane */
//----------------------------------------------------------------------

template <class T> void threshold_abs
(
  vil_memory_image_of<T> &image, 
  vil_memory_image_of<T> &source_vals,
  T threshold, T zero_val
)
{
  assert(image.width()==source_vals.width());
  assert(image.height()==source_vals.height());
  T *image_row, *src_row;
  int width = image.width(), height = image.height();
  for (int y=0; y<height; y++)
  {
    image_row = image[y];
    src_row = source_vals[y];
    for (int x=0; x<width; x++)
    {
      if (((*image_row)<threshold)&&((*image_row)>-threshold)) 
        *image_row = zero_val;
      else *image_row = *src_row;
      image_row++; src_row++;
    }
  }
}

#endif // ouml_image_ops_txx_
