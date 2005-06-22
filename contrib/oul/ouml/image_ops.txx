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
 * \param im1: vil1_memory_image_of<T> &, the image to be modified
 * \param im2: vil1_memory_image_of<T> &, the image to subtract from
 * \return vil1_memory_image_of<T> &: a reference to im1
 *
 * \author Brendan McCane
 */
//----------------------------------------------------------------------

template <class T>
vil1_memory_image_of<T> &operator -=
(
  vil1_memory_image_of<T> &im1,
  vil1_memory_image_of<T> &im2
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
 * \param im1: vil1_memory_image_of<T> &, the image to be modified
 * \param constant_add: T, the amount to add to each pixel in the image
 * \return vil1_memory_image_of<T> &: a reference to im1
 *
 * \author Brendan McCane
 */
//----------------------------------------------------------------------
template <class T>
vil1_memory_image_of<T> &operator +=
(
  vil1_memory_image_of<T> &im1,
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
 * \param image: vil1_memory_image_of<T> &, the image to be modified
 * \param source_vals: vil1_memory_image_of<T> &, the image to be modified
 * \param threshold: T, the threshold to apply
 * \param zero_val: T, the value to set the thresholded pixels to
 *
 * \author Brendan McCane */
//----------------------------------------------------------------------

template <class T> void threshold_abs
(
  vil1_memory_image_of<T> &image, 
  vil1_memory_image_of<T> &source_vals,
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

/** max_val
 *
 * Return the maximum value for an image. Assumes that a < operator is
 * defined on the template parameter.
 *
 * \param vil1_memory_image_of<T> &image: the source image.
 *
 * \param T min: the minimum value to use to initialise the search. 
 * For unsigned chars this will probably be 0, but for signed ints it
 * might be -MAXINT, and for RGB could be rgb<0,0,0> etc.
 *
 * \return T: the maximum value found.
 */
template <class T>
T max_val(const vil1_memory_image_of<T> &image, T min)
{
	T max = min;
	for (int x=0; x<image.width(); x++)
		for (int y=0; y<image.height(); y++)
			if (image(x,y)>max)
				max = image(x,y);
	return max;
}

/** min_val
 *
 * Return the minimum value for an image. Assumes that a < operator is
 * defined on the template parameter.
 *
 * \param vil1_memory_image_of<T> &image: the source image.
 *
 * \param T max: the maximum value to use to initialise the search. 
 * For unsigned chars this will probably be 255, but for signed ints it
 * might be MAXINT, and for RGB could be rgb<255,255,255> etc.
 *
 * \return T: the minimum value found.
 */
template <class T>
T min_val(const vil1_memory_image_of<T> &image, T max)
{
	T min = max;
	for (int x=0; x<image.width(); x++)
		for (int y=0; y<image.height(); y++)
			if (image(x,y)<min)
				min = image(x,y);
	return min;
}

/** normalise_image
 *
 * Normalise an image so that the pixel values fit within the range
 * specified. This is useful for normalising prior to saving
 * int/double images. Can normalise to 0 to 255 and then save as a pgm
 * etc.
 *
 * \param vil1_memory_image_of<T> &src: the source image
 * \param T low: the minimum value to normalise to
 * \param T high: the maximum value to normalise to
 * \param T min: the smallest possible value of a T
 * \param T max: the largest possible value of a T
 * \param T epsilon: a small T. Needed for normalising floats etc as
 * numerical inaccuracy can cause the result to be slightly greater
 * than high.
 * \return vil1_memory_image_of<T> *: a new normalised image
 */

template <class T> vil1_memory_image_of<T> *normalise_image
(
	const vil1_memory_image_of<T> &src, 
	T low, T high,
	T min, T max,
    T epsilon
)
{
	assert(high>low);
	assert(max>min);
	vil1_memory_image_of<T> *ret = 
		new vil1_memory_image_of<T>(src.width(), src.height());
	T small = min_val(src, max);
	T large = max_val(src, min);
	double d1 = (double)(high-low);
	assert(d1>0);
	double d2 = (double)(large-small);
	assert(d2>=0);
	if (d2>0)
	{
		for (int x=0; x<src.width(); x++)
			for (int y=0; y<src.height(); y++)
			{
				(*ret)(x,y) = (T)((src(x,y)-small)*d1/d2+low);
				assert(((*ret)(x,y)>=low-epsilon)&&((*ret)(x,y)<=high+epsilon));
			}
	}
	else 
	{
		for (int x=0; x<src.width(); x++)
			for (int y=0; y<src.height(); y++)
				(*ret)(x,y) = (T)src(x,y);
	}
	return(ret);
}

#endif // ouml_image_ops_txx_
