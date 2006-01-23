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
#include "vidl2_color.h"
#include "vidl2_frame.h"
#include <vxl_config.h>
#include <vcl_cassert.h>


//-----------------------------------------------------------------------------


//: The default pixel iterator
// used for non-planar non-packed formats 
template <vidl2_pixel_format FMT> 
class vidl2_pixel_iterator
{
  typedef typename vidl2_pixel_traits_of<FMT>::type cmp_type;
  public:
    //: Constructor
    vidl2_pixel_iterator(const vidl2_frame_sptr& frame):
      frame_(frame), ptr_((cmp_type*)frame->data())
    {
      assert(frame->pixel_format() == FMT);
      // The following should be a static asserts
      assert(vidl2_pixel_traits_of<FMT>::bits_per_pixel%8 == 0);
      assert(vidl2_pixel_traits_of<FMT>::arrangement() == VIDL2_PIXEL_ARRANGE_SINGLE);
    }

    //: Pre-increment
    vidl2_pixel_iterator<FMT> & operator++ ()
    {
      ptr_ += vidl2_pixel_traits_of<FMT>::bits_per_pixel/(sizeof(cmp_type)*8);
      return *this;
    }

    //: Access the data
    cmp_type operator () (unsigned int i) const
    {
      assert(i<vidl2_pixel_traits_of<FMT>::num_channels);
      return vidl2_color_component<FMT>::get(ptr_,i);
    }

  private:
    vidl2_frame_sptr frame_;
    const cmp_type * ptr_;
};


//: Iterator for YUV 4:2:2 packed images
VCL_DEFINE_SPECIALIZATION 
class vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_UYVY_422>
{
  public:
    //: Constructor
    vidl2_pixel_iterator(const vidl2_frame_sptr& frame):
      frame_(frame), mode_(true), ptr_((vxl_byte*)frame->data())
    {
      assert(frame->pixel_format() == VIDL2_PIXEL_FORMAT_UYVY_422);
    }

    //: Pre-increment
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_UYVY_422> & operator++ ()
    {
      mode_ = !mode_;
      if(mode_)
        ptr_ += 4;
      return *this;
    }

    //: Access the data
    vxl_byte operator () (unsigned int i) const
    {
      assert(i<3);
      if(!mode_ && !i)
        return ptr_[3];
      return ptr_[i^((i>>1)^1)];
    }

  private:
    vidl2_frame_sptr frame_;
    bool mode_;
    const vxl_byte * ptr_;
};


//: Iterator for YUV 4:1:1 packed images
VCL_DEFINE_SPECIALIZATION 
class vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_UYVY_411>
{
  public:
    //: Constructor
    vidl2_pixel_iterator(const vidl2_frame_sptr& frame):
      frame_(frame), mode_(0), ptr_((vxl_byte*)frame->data())
      {
        assert(frame->pixel_format() == VIDL2_PIXEL_FORMAT_UYVY_411);
        offset_[0][0] = 1; offset_[0][1] = 0; offset_[0][2] = 3;
        offset_[1][0] = 2; offset_[1][1] = 0; offset_[1][2] = 3;
        offset_[2][0] = 4; offset_[2][1] = 0; offset_[2][2] = 3;
        offset_[3][0] = 5; offset_[3][1] = 0; offset_[3][2] = 3;
      }

    //: Pre-increment
      vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_UYVY_411> & operator++ ()
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


//: Iterator for YUV 420 planar images
VCL_DEFINE_SPECIALIZATION 
class vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_YUV_420P>
{
  public:
    //: Constructor
    vidl2_pixel_iterator(const vidl2_frame_sptr& frame):
      frame_(frame), line_size_(frame_->ni()), line_cnt_(0),
      mode_x_(false), mode_y_(false)
    {
      assert(frame->pixel_format() == VIDL2_PIXEL_FORMAT_YUV_420P);
      assert(frame->ni()%2 == 0);
      assert(frame->nj()%2 == 0);

      unsigned size = frame->ni()*frame->nj();
      ptr_[0] = (vxl_byte*)frame->data();
      ptr_[1] = ptr_[0] + size;
      ptr_[2] = ptr_[1] + (size >> 2);
    }

    //: Pre-increment
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_YUV_420P> & operator++ ()
    {
      ++ptr_[0];
      if(mode_x_){
        ++ptr_[1];
        ++ptr_[2];
      }
      mode_x_ = !mode_x_;
      if(++line_cnt_ == line_size_){
        line_cnt_ = 0;
        mode_y_ = !mode_y_;
        mode_x_=false;
        if(mode_y_){
          ptr_[1] -= line_size_>>1;
          ptr_[2] -= line_size_>>1;
        }
      }
      return *this;
    }

    //: Access the data
    vxl_byte operator () (unsigned int i) const
    {
      assert(i<3);
      return *ptr_[i];
    }

  private:
    vidl2_frame_sptr frame_;
    unsigned int line_size_;
    unsigned int line_cnt_;
    bool mode_x_;
    bool mode_y_;
    const vxl_byte * ptr_[3];
};


//: Iterator for YUV 422 planar images
VCL_DEFINE_SPECIALIZATION 
class vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_YUV_422P>
{
  public:
    //: Constructor
    vidl2_pixel_iterator(const vidl2_frame_sptr& frame):
      frame_(frame), line_size_(frame_->ni()), line_cnt_(0), mode_(false)
    {
      assert(frame->pixel_format() == VIDL2_PIXEL_FORMAT_YUV_422P);
      assert(frame->ni()%2 == 0);

      unsigned size = frame->ni()*frame->nj();
      ptr_[0] = (vxl_byte*)frame->data();
      ptr_[1] = ptr_[0] + size;
      ptr_[2] = ptr_[1] + (size >> 1);
    }

    //: Pre-increment
    vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_YUV_422P> & operator++ ()
    {
      ++ptr_[0];
      if(mode_){
        ++ptr_[1];
        ++ptr_[2];
      }
      mode_ = !mode_;
      if(++line_cnt_ == line_size_){
        line_cnt_ = 0;
        mode_=false;
      }
      return *this;
    }

    //: Access the data
    vxl_byte operator () (unsigned int i) const
    {
      assert(i<3);
      return *ptr_[i];
    }

  private:
    vidl2_frame_sptr frame_;
    unsigned int line_size_;
    unsigned int line_cnt_;
    bool mode_;
    const vxl_byte * ptr_[3];
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
      return bool(ptr_[0] & bit_mask_);
    }

  private:
    vidl2_frame_sptr frame_;
    vxl_byte bit_mask_;
    const vxl_byte * ptr_;
};



#endif // vidl2_pixel_iterator_h_

