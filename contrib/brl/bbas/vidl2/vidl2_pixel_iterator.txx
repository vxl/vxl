// This is brl/bbas/vidl2/vidl2_pixel_iterator.txx
#ifndef vidl2_pixel_iterator_txx_
#define vidl2_pixel_iterator_txx_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Templated definitions for pixel iterators
//
// \author Matt Leotta 
// \date 3 Mar 2006
//
// This file contains the template definitions for pixel iterators


#include "vidl2_pixel_iterator.h"
#include "vidl2_color.h"
#include <vxl_config.h>
#include <vcl_cassert.h>

//-----------------------------------------------------------------------------


template <vidl2_pixel_arrangement AR>
struct vidl2_pixel_iterator_arrange_valid
{
  enum { value = (AR == VIDL2_PIXEL_ARRANGE_SINGLE) };
};


//: The default pixel iterator for single arranged formats
template <vidl2_pixel_arrangement AR, vidl2_pixel_format FMT>
class vidl2_pixel_iterator_arranged : public vidl2_pixel_iterator
{
  typedef typename vidl2_pixel_traits_of<FMT>::type cmp_type;
  public:
    //: Constructor
    vidl2_pixel_iterator_arranged(const vidl2_frame& frame):
      ptr_((cmp_type*)frame.data())
    {
      assert(frame.pixel_format() == FMT);
    // The following should be a static asserts
      assert(vidl2_pixel_traits_of<FMT>::bits_per_pixel%8 == 0);
      assert(vidl2_pixel_traits_of<FMT>::arrangement() == VIDL2_PIXEL_ARRANGE_SINGLE);
    }

    //: Destructor
    virtual ~vidl2_pixel_iterator_arranged<AR,FMT>(){}

    //: Return the pixel format
    virtual vidl2_pixel_format pixel_format() const
    { return FMT; }

    //: Step to the next pixel
    vidl2_pixel_iterator_arranged<AR,FMT>& next()
    {
      ptr_ += vidl2_pixel_traits_of<FMT>::bits_per_pixel/(sizeof(cmp_type)*8);
      return *this;
    }

    //: Pre-increment: step to the next pixel
    virtual vidl2_pixel_iterator& operator++ ()
    {
      return this->next();
    }

    //: Access the data
    cmp_type operator () (unsigned int i) const
    {
      assert(i<vidl2_pixel_traits_of<FMT>::num_channels);
      return vidl2_color_component<FMT>::get(ptr_,i);
    }

    //: Access the entire pixel at once
    void get(cmp_type* data) const
    {
      vidl2_color_component<FMT>::get_all(ptr_,data);
    }

    //: write the entire pixel at once
    void set(const cmp_type* data)
    {
      vidl2_color_component<FMT>::set_all(ptr_,data);
    }

    //: Copy the pixel data into a byte array
    virtual void get_data(vxl_byte* data) const
    {
      this->get(reinterpret_cast<cmp_type*>(data));
    }

    //: Set the pixel data from a byte array
    virtual void set_data(const vxl_byte* data)
    {
      this->set(reinterpret_cast<const cmp_type*>(data));
    }

  private:
    cmp_type * ptr_;
};


#if VCL_CAN_DO_PARTIAL_SPECIALIZATION

template <>
struct vidl2_pixel_iterator_arrange_valid<VIDL2_PIXEL_ARRANGE_PLANAR>
{
  enum { value = true };
};


//: The default pixel iterator for planar arranged formats
template <vidl2_pixel_format FMT>
class vidl2_pixel_iterator_arranged<VIDL2_PIXEL_ARRANGE_PLANAR,FMT>
  : public vidl2_pixel_iterator
{
  typedef typename vidl2_pixel_traits_of<FMT>::type cmp_type;
  enum { csx = vidl2_pixel_traits_of<FMT>::chroma_shift_x };\
  enum { csy = vidl2_pixel_traits_of<FMT>::chroma_shift_y };\
  public:
    //: Constructor
    vidl2_pixel_iterator_arranged(const vidl2_frame& frame)
      : line_size_(frame.ni()), line_cnt_(0), step_x_(1), step_y_(1)
    {
      assert(frame.pixel_format() == FMT);
      // The following should be a static asserts
      assert(vidl2_pixel_traits_of<FMT>::arrangement() == VIDL2_PIXEL_ARRANGE_PLANAR);

      const unsigned size = frame.ni()*frame.nj()*sizeof(typename vidl2_pixel_traits_of<FMT>::type);

      ptr_[0] = (cmp_type*)frame.data();
      for(unsigned int i=1; i<vidl2_pixel_traits_of<FMT>::num_channels; ++i){
        if(i==1)
          ptr_[i] = ptr_[i-1] + size;
        else
          ptr_[i] = ptr_[i-1] + ((size>>csx)>>csy);
      }
    }

    //: Destructor
    virtual ~vidl2_pixel_iterator_arranged(){}

    //: Return the pixel format
    virtual vidl2_pixel_format pixel_format() const
    { return FMT; }

    //: Step to the next pixel
    vidl2_pixel_iterator_arranged<VIDL2_PIXEL_ARRANGE_PLANAR,FMT>& next()
    {
      ++ptr_[0];
      if(vidl2_pixel_traits_of<FMT>::num_channels > 1){
        int chroma_step = (step_x_>>csx)&0x01;
        if(++line_cnt_ < line_size_){
          step_x_ += 2;
        }
        else
        {
          line_cnt_ = 0;
          step_x_=1;
          chroma_step = 1;
          if(!((step_y_>>csy)&0x01))
            chroma_step -= (line_size_>>csy);
          step_y_ += 2;
        }
        for(unsigned int i=1; i<vidl2_pixel_traits_of<FMT>::num_channels; ++i){
          ptr_[i] += chroma_step;
        }
      }
      return *this;
    }

    //: Pre-increment: step to the next pixel
    virtual vidl2_pixel_iterator& operator++ ()
    {
      return this->next();
    }

    //: Access the data
    cmp_type operator () (unsigned int i) const
    {
      return *ptr_[i];
    }

    //: Access the entire pixel at once
    void get(cmp_type* data) const
    {
      for(unsigned int i=0; i<vidl2_pixel_traits_of<FMT>::num_channels; ++i)
        data[i] = *ptr_[i];
    }

    //: write the entire pixel at once
    void set(const cmp_type* data)
    {
      for(unsigned int i=0; i<vidl2_pixel_traits_of<FMT>::num_channels; ++i)
        *ptr_[i] = data[i];
    }

    //: Copy the pixel data into a byte array
    virtual void get_data(vxl_byte* data) const
    {
      this->get(reinterpret_cast<cmp_type*>(data));
    }

    //: Set the pixel data from a byte array
    virtual void set_data(const vxl_byte* data)
    {
      this->set(reinterpret_cast<const cmp_type*>(data));
    }

  private:
    unsigned int line_size_;
    unsigned int line_cnt_;
    cmp_type * ptr_[vidl2_pixel_traits_of<FMT>::num_channels];
    vxl_byte step_x_, step_y_;
};


#endif


template <vidl2_pixel_format FMT>
struct vidl2_pixel_iterator_valid
{
  enum { value = vidl2_pixel_iterator_arrange_valid<
    vidl2_pixel_arrangement(
        vidl2_pixel_traits_of<FMT>::arrangement_idx) >::value };
};


//: The default pixel iterator
// used for non-planar non-packed formats
template <vidl2_pixel_format FMT>
class vidl2_pixel_iterator_of
  : public vidl2_pixel_iterator_arranged<(vidl2_pixel_arrangement)vidl2_pixel_traits_of<FMT>::arrangement_idx,FMT>
{
  typedef vidl2_pixel_iterator_arranged<
      vidl2_pixel_arrangement( vidl2_pixel_traits_of<FMT>::arrangement_idx ),FMT> baseclass;
  typedef typename vidl2_pixel_traits_of<FMT>::type cmp_type;
  public:
    //: Constructor
    vidl2_pixel_iterator_of(const vidl2_frame& frame) : baseclass(frame){}

    //: Destructor
    virtual ~vidl2_pixel_iterator_of<FMT>(){}
};


//-----------------------------------------------------------------------------
// Custom Pixel Iterators
//-----------------------------------------------------------------------------


//: Iterator for YUV 4:2:2 packed images
VCL_DEFINE_SPECIALIZATION
struct vidl2_pixel_iterator_valid<VIDL2_PIXEL_FORMAT_UYVY_422>
{ enum { value = true }; };

VCL_DEFINE_SPECIALIZATION
class vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_UYVY_422>
  : public vidl2_pixel_iterator
{
  public:
    //: Constructor
    vidl2_pixel_iterator_of(const vidl2_frame& frame):
    mode_(true), ptr_((vxl_byte*)frame.data())
    {
      assert(frame.pixel_format() == VIDL2_PIXEL_FORMAT_UYVY_422);
    }

    //: Destructor
    virtual ~vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_UYVY_422>(){}

    //: Return the pixel format
    virtual vidl2_pixel_format pixel_format() const
    { return VIDL2_PIXEL_FORMAT_UYVY_422; }

    //: Step to the next pixel
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_UYVY_422>& next()
    {
      mode_ = !mode_;
      if(mode_)
        ptr_ += 4;
      return *this;
    }

    //: Pre-increment: step to the next pixel
    virtual vidl2_pixel_iterator& operator++ ()
    {
      return this->next();
    }

    //: Access the data
    vxl_byte operator () (unsigned int i) const
    {
      assert(i<3);
      if(!mode_ && !i)
        return ptr_[3];
      return ptr_[i^((i>>1)^1)];
    }

    //: Access the entire pixel at once
    void get(vxl_byte* data) const
    {
      data[0] = ptr_[mode_?1:3];
      data[1] = ptr_[0];
      data[2] = ptr_[2];
    }

    //: write the entire pixel at once
    void set(const vxl_byte* data)
    {
      if(mode_){
        ptr_[1] = data[0];
        ptr_[0] = data[1];
        ptr_[2] = data[2];
      }else{
        ptr_[3] = data[0];
      // average with previous values
        ptr_[0] = static_cast<vxl_byte>(((int)ptr_[0]+data[1])>>1);
        ptr_[2] = static_cast<vxl_byte>(((int)ptr_[2]+data[2])>>1);
      }
    }

    //: Copy the pixel data into a byte array
    virtual void get_data(vxl_byte* data) const
    {
      this->get(data);
    }

    //: Set the pixel data from a byte array
    virtual void set_data(const vxl_byte* data)
    {
      this->set(data);
    }

  private:
    bool mode_;
    vxl_byte * ptr_;
};


//: Iterator for UYVY 4:1:1 packed images
VCL_DEFINE_SPECIALIZATION
struct vidl2_pixel_iterator_valid<VIDL2_PIXEL_FORMAT_UYVY_411>
{ enum { value = true }; };

VCL_DEFINE_SPECIALIZATION
class vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_UYVY_411>
  : public vidl2_pixel_iterator
{
  public:
    //: Constructor
    vidl2_pixel_iterator_of(const vidl2_frame& frame):
      mode_(0), ptr_((vxl_byte*)frame.data())
    {
      assert(frame.pixel_format() == VIDL2_PIXEL_FORMAT_UYVY_411);
      offset_[0][0] = 1; offset_[0][1] = 0; offset_[0][2] = 3;
      offset_[1][0] = 2; offset_[1][1] = 0; offset_[1][2] = 3;
      offset_[2][0] = 4; offset_[2][1] = 0; offset_[2][2] = 3;
      offset_[3][0] = 5; offset_[3][1] = 0; offset_[3][2] = 3;
    }

    //: Destructor
    virtual ~vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_UYVY_411>(){}

    //: Return the pixel format
    virtual vidl2_pixel_format pixel_format() const
    { return VIDL2_PIXEL_FORMAT_UYVY_411; }


    //: Step to the next pixel
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_UYVY_411>& next()
    {
      mode_ = (mode_+1)%4;
      if(mode_==0)
        ptr_ += 6;
      return *this;
    }

    //: Pre-increment: step to the next pixel
    virtual vidl2_pixel_iterator& operator++ ()
    {
      return this->next();
    }

    //: Access the data
    vxl_byte operator () (unsigned int i) const
    {
      assert(i<3);
      return ptr_[offset_[mode_][i]];
    }

    //: Access the entire pixel at once
    void get(vxl_byte* data) const
    {
      data[0] = ptr_[offset_[mode_][0]];
      data[1] = ptr_[offset_[mode_][1]];
      data[2] = ptr_[offset_[mode_][2]];
    }

    //: write the entire pixel at once
    void set(const vxl_byte* data)
    {
      ptr_[offset_[mode_][0]] = data[0];
    // add a quarter of the color from each pixel
    // note - U,Y channels are not valid until we
    // have iterated through the entire macro pixel
      if(mode_==0){
        ptr_[offset_[0][1]] = data[1]>>2;
        ptr_[offset_[0][2]] = data[2]>>2;
      }else{
        ptr_[offset_[mode_][1]] += data[1]>>2;
        ptr_[offset_[mode_][2]] += data[2]>>2;
      }
    }

    //: Copy the pixel data into a byte array
    virtual void get_data(vxl_byte* data) const
    {
      this->get(data);
    }

    //: Set the pixel data from a byte array
    virtual void set_data(const vxl_byte* data)
    {
      this->set(data);
    }

  private:
    vxl_byte mode_;
    vxl_byte * ptr_;
    int offset_[4][3];
};


//: Iterator for monochrome boolean images
VCL_DEFINE_SPECIALIZATION
struct vidl2_pixel_iterator_valid<VIDL2_PIXEL_FORMAT_MONO_1>
{ enum { value = true }; };

VCL_DEFINE_SPECIALIZATION
class vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_MONO_1>
  : public vidl2_pixel_iterator
{
  public:
    //: Constructor
    vidl2_pixel_iterator_of(const vidl2_frame& frame):
      bit_mask_(128), ptr_((vxl_byte*)frame.data())
    {
      assert(frame.pixel_format() == VIDL2_PIXEL_FORMAT_MONO_1);
    }

    //: Destructor
    virtual ~vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_MONO_1>(){}

    //: Return the pixel format
    virtual vidl2_pixel_format pixel_format() const
    { return VIDL2_PIXEL_FORMAT_MONO_1; }


    //: Step to the next pixel
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_MONO_1>& next()
    {
      bit_mask_ >>= 1;
      if(!bit_mask_){
        bit_mask_ = 128;
        ++ptr_;
      }

      return *this;
    }

    //: Pre-increment: step to the next pixel
    virtual vidl2_pixel_iterator& operator++ ()
    {
      return this->next();
    }

    //: Access the data
    bool operator () (unsigned int i) const
    {
      assert(i==0);
      return bool(ptr_[0] & bit_mask_);
    }

    //: Access the entire pixel at once
    void get(bool* data) const
    {
      data[0] = bool(ptr_[0] & bit_mask_);
    }

    //: write the entire pixel at once
    void set(const bool* data)
    {
      ptr_[0] &= ~bit_mask_ & data[0]?bit_mask_:0;
    }

    //: Copy the pixel data into a byte array
    virtual void get_data(vxl_byte* data) const
    {
      this->get(reinterpret_cast<bool*>(data));
    }

    //: Set the pixel data from a byte array
    virtual void set_data(const vxl_byte* data)
    {
      this->set(reinterpret_cast<const bool*>(data));
    }

  private:
    vxl_byte bit_mask_;
    vxl_byte * ptr_;
};


#endif // vidl2_pixel_iterator_txx_

