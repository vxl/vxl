// This is brl/bbas/vidl2/vidl2_pixel_iterator.h
#ifndef vidl2_pixel_iterator_h_
#define vidl2_pixel_iterator_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Iterators for pixels with unusual pixel formats
//
// \author Matt Leotta 
// \date 16 Jan 2006
//
// This file contains pixel iterators for various pixel formats.
// These iterators provide a standard interface for iterating
// over the pixels in an image and extracting the color components.
// Each iterator defines the pre-increment <tt> operator++ </tt>
// to step to the next pixel in raster scan order.  Each iterator
// also defines <tt>operator () (unsigned int)</tt> to give read
// access to each of the pixels color components.


#include "vidl2_pixel_format.h"
#include <vxl_config.h>
#include <vcl_cassert.h>

template <vidl2_pixel_format FMT> 
class vidl2_pixel_iterator;

//: Iterator for YUV 4:2:2 packed images
VCL_DEFINE_SPECIALIZATION 
class vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_YUV_422>
{
  public:
    //: Constructor
    vidl2_pixel_iterator(const vxl_byte * buffer):
      half_step_(false), ptr_(buffer)
    {
      p_[1] = ptr_[0]; // U
      p_[0] = ptr_[1]; // Y0
      p_[2] = ptr_[2]; // V
    }

    //: Pre-increment
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_YUV_422> & operator++ ()
    {
      if(!half_step_){
        p_[0] = ptr_[3]; // Y1
        half_step_ = true;
      }
      else{
        ptr_ += 4;
        p_[1] = ptr_[0]; // U
        p_[0] = ptr_[1]; // Y0
        p_[2] = ptr_[2]; // V
        half_step_ = false;
      }
      return *this;
    }

    //: Access the data
    vxl_byte operator () (unsigned int i) const
    {
      assert(i<3);
      return p_[i];
    }

  private:
    bool half_step_;
    const vxl_byte * ptr_;
    vxl_byte p_[3];
};


//: Iterator for monochrome boolean images
VCL_DEFINE_SPECIALIZATION 
class vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_MONO_1>
{
  public:
    //: Constructor
    vidl2_pixel_iterator(const vxl_byte * buffer):
      bit_mask_(128), ptr_(buffer)
    {}

    //: Pre-increment
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_MONO_1> & operator++ ()
    {
      bit_mask_ >>= 1;
      if(!bit_mask_){
        bit_mask_ = 128;
        ++ptr_;
      }

      return *this;
    }

    //: Access the data
    bool operator () (unsigned int i) const
    {
      assert(i==0);
      return (ptr_[0] & bit_mask_);
    }

  private:
    vxl_byte bit_mask_;
    const vxl_byte * ptr_;
};



#endif // vidl2_pixel_iterator_h_

