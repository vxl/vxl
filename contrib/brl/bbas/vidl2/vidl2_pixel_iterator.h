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
#include "vidl2_frame.h"
#include <vxl_config.h>
#include <vcl_cassert.h>

//-----------------------------------------------------------------------------

//: Component color encoding
// These functions indicate how to extract a byte for each
// color channel given a pointer to the pixel memory
// This is only meant for non-planar non-packed formats
template <vidl2_pixel_format FMT> 
struct vidl2_component_enc
{
  static vxl_byte channel(const vxl_byte * ptr, unsigned int i)
  {
    return ptr[i];
  }
};


VCL_DEFINE_SPECIALIZATION 
struct vidl2_component_enc<VIDL2_PIXEL_FORMAT_BGR_24>
{
  static vxl_byte channel(const vxl_byte * ptr, unsigned int i)
  {
    return ptr[2-i];
  }
};


VCL_DEFINE_SPECIALIZATION 
struct vidl2_component_enc<VIDL2_PIXEL_FORMAT_RGB_555>
{
  static vxl_byte channel(const vxl_byte * ptr, unsigned int i)
  {
    const vxl_uint_16* p = reinterpret_cast<const vxl_uint_16*>(ptr);
    return static_cast<vxl_byte>((*p >> (2-i)*5) & 31)<<3;
  }
};


VCL_DEFINE_SPECIALIZATION 
struct vidl2_component_enc<VIDL2_PIXEL_FORMAT_RGB_565>
{
  static vxl_byte channel(const vxl_byte * ptr, unsigned int i)
  {
    const vxl_uint_16* p = reinterpret_cast<const vxl_uint_16*>(ptr);
    switch(i){
      case 0: return vxl_byte((*p & 0xF800) >> 8); // R
      case 1: return vxl_byte((*p & 0x07E0) >> 3); // G
      case 2: return vxl_byte((*p & 0x001F) << 3); // B
    }
    return 0;
  }
};

//-----------------------------------------------------------------------------


//: The default pixel iterator
// used for non-planar non-packed formats 
template <vidl2_pixel_format FMT> 
class vidl2_pixel_iterator
{
  public:
    //: Constructor
    vidl2_pixel_iterator(const vidl2_frame_sptr& frame):
      frame_(frame), ptr_((vxl_byte*)frame->data())
    {
      assert(frame->pixel_format() == FMT);
      // The following should be a static asserts
      assert(vidl2_pixel_traits_of<FMT>::bits_per_pixel()%8 == 0);
      assert(!vidl2_pixel_traits_of<FMT>::planar());
      assert(!vidl2_pixel_traits_of<FMT>::packed());
    }

    //: Pre-increment
    vidl2_pixel_iterator<FMT> & operator++ ()
    {
      ptr_ += vidl2_pixel_traits_of<FMT>::bits_per_pixel()/8;
      return *this;
    }

    //: Access the data
    vxl_byte operator () (unsigned int i) const
    {
      assert(i<vidl2_pixel_traits_of<FMT>::num_channels());
      return vidl2_component_enc<FMT>::channel(ptr_,i);
    }

  private:
    vidl2_frame_sptr frame_;
    const vxl_byte * ptr_;
};


//: Iterator for YUV 4:2:2 packed images
VCL_DEFINE_SPECIALIZATION 
class vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_YUV_422>
{
  public:
    //: Constructor
    vidl2_pixel_iterator(const vidl2_frame_sptr& frame):
      frame_(frame), mode_(0), ptr_((vxl_byte*)frame->data())
    {
      assert(frame->pixel_format() == VIDL2_PIXEL_FORMAT_YUV_422);
      offset_[0][0] = 1; offset_[0][1] = 0; offset_[0][2] = 2; 
      offset_[1][0] = 1; offset_[1][1] = -2; offset_[1][2] = 0;
    }

    //: Pre-increment
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_YUV_422> & operator++ ()
    {
      mode_ = (mode_+1)%2;
      ptr_ += 2;
      return *this;
    }

    //: Access the data
    vxl_byte operator () (unsigned int i) const
    {
      assert(i<3);
      return ptr_[offset_[mode_][i]];
    }

  private:
    vidl2_frame_sptr frame_;
    vxl_byte mode_;
    const vxl_byte * ptr_;
    int offset_[2][3];
};


//: Iterator for YUV 4:1:1 packed images
VCL_DEFINE_SPECIALIZATION 
    class vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_YUV_411>
{
  public:
    //: Constructor
    vidl2_pixel_iterator(const vidl2_frame_sptr& frame):
      frame_(frame), mode_(0), ptr_((vxl_byte*)frame->data())
      {
        assert(frame->pixel_format() == VIDL2_PIXEL_FORMAT_YUV_411);
        offset_[0][0] = 1; offset_[0][1] = 0; offset_[0][2] = 3;
        offset_[1][0] = 2; offset_[1][1] = 0; offset_[1][2] = 3;
        offset_[2][0] = 4; offset_[2][1] = 0; offset_[2][2] = 3;
        offset_[3][0] = 5; offset_[3][1] = 0; offset_[3][2] = 3;
      }

    //: Pre-increment
      vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_YUV_411> & operator++ ()
      {
        mode_ = (mode_+1)%4;
        if(mode_==0)
          ptr_ += 6;
        return *this;
      }

    //: Access the data
      vxl_byte operator () (unsigned int i) const
      {
        assert(i<3);
        return ptr_[offset_[mode_][i]];
      }

  private:
    vidl2_frame_sptr frame_;
    vxl_byte mode_;
    const vxl_byte * ptr_;
    int offset_[4][3];
};


//: Iterator for monochrome 16-bit images
VCL_DEFINE_SPECIALIZATION 
class vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_MONO_16>
{
  public:
    //: Constructor
    vidl2_pixel_iterator(const vidl2_frame_sptr& frame):
      frame_(frame), ptr_((vxl_uint_16*)frame->data())
    {
      assert(frame->pixel_format() == VIDL2_PIXEL_FORMAT_MONO_16);
    }

    //: Pre-increment
      vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_MONO_16> & operator++ ()
    {
      ++ptr_;
      return *this;
    }

    //: Access the data
    vxl_uint_16 operator () (unsigned int i) const
    {
      assert(i==0);
      return *ptr_;
    }

  private:
    vidl2_frame_sptr frame_;
    const vxl_uint_16 * ptr_;
};


//: Iterator for monochrome boolean images
VCL_DEFINE_SPECIALIZATION 
class vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_MONO_1>
{
  public:
    //: Constructor
    vidl2_pixel_iterator(const vidl2_frame_sptr& frame):
      frame_(frame), bit_mask_(128), ptr_((vxl_byte*)frame->data())
    {
      assert(frame->pixel_format() == VIDL2_PIXEL_FORMAT_MONO_1);
    }

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
    vidl2_frame_sptr frame_;
    vxl_byte bit_mask_;
    const vxl_byte * ptr_;
};



#endif // vidl2_pixel_iterator_h_

