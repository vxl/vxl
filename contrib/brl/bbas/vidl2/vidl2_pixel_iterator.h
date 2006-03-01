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
class vidl2_pixel_iterator
{
  public:
    //: Destructor
    virtual ~vidl2_pixel_iterator(){}
    //: Return the pixel format
    virtual vidl2_pixel_format pixel_format() const = 0;
    //: Pre-increment: step to the next pixel
    virtual vidl2_pixel_iterator& operator++ () = 0;
    //: Copy the pixel data into a byte array
    virtual void get_data(vxl_byte* data) const = 0;
    //: Set the pixel data from a byte array
    virtual void set_data(const vxl_byte* data) = 0;
};


//: Pixel iterator factory
// Creates a new pixel iterator on the heap
// The iterator is initialized to the first pixel in the frame
// \note The user is responsible for deleting the iterator
vidl2_pixel_iterator*
vidl2_make_pixel_iterator(const vidl2_frame& frame);


//: Return true if the pixel format has a valid pixel iterator implementation
bool vidl2_has_pixel_iterator(vidl2_pixel_format FMT);


template <vidl2_pixel_format FMT>
struct vidl2_pixel_iterator_valid
{
  enum { value = (vidl2_pixel_arrangement(vidl2_pixel_traits_of<FMT>::arrangement_idx) == VIDL2_PIXEL_ARRANGE_SINGLE) };
};

//: The default pixel iterator
// used for non-planar non-packed formats 
template <vidl2_pixel_format FMT> 
class vidl2_pixel_iterator_of : public vidl2_pixel_iterator
{
  typedef typename vidl2_pixel_traits_of<FMT>::type cmp_type;
  public:
    //: Constructor
    vidl2_pixel_iterator_of(const vidl2_frame& frame):
      ptr_((cmp_type*)frame.data())
    {
      assert(frame.pixel_format() == FMT);
      // The following should be a static asserts
      assert(vidl2_pixel_traits_of<FMT>::bits_per_pixel%8 == 0);
      assert(vidl2_pixel_traits_of<FMT>::arrangement() == VIDL2_PIXEL_ARRANGE_SINGLE);
    }

    //: Destructor
    virtual ~vidl2_pixel_iterator_of<FMT>(){}

    //: Return the pixel format
    virtual vidl2_pixel_format pixel_format() const
    { return FMT; }

    //: Step to the next pixel
    vidl2_pixel_iterator_of<FMT>& next()
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


//: Iterator for YUV 4:1:1 packed images
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


//: Iterator for YUV 420 planar images
VCL_DEFINE_SPECIALIZATION 
struct vidl2_pixel_iterator_valid<VIDL2_PIXEL_FORMAT_YUV_420P>
{ enum { value = true }; };

VCL_DEFINE_SPECIALIZATION 
class vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_YUV_420P>
  : public vidl2_pixel_iterator
{
  public:
    //: Constructor
    vidl2_pixel_iterator_of(const vidl2_frame& frame):
      line_size_(frame.ni()), line_cnt_(0),
      mode_x_(false), mode_y_(false)
    {
      assert(frame.pixel_format() == VIDL2_PIXEL_FORMAT_YUV_420P);
      assert(frame.ni()%2 == 0);
      assert(frame.nj()%2 == 0);

      unsigned size = frame.ni()*frame.nj();
      ptr_[0] = (vxl_byte*)frame.data();
      ptr_[1] = ptr_[0] + size;
      ptr_[2] = ptr_[1] + (size >> 2);
    }

    //: Destructor
    virtual ~vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_YUV_420P>(){}

    //: Return the pixel format
    virtual vidl2_pixel_format pixel_format() const
    { return VIDL2_PIXEL_FORMAT_YUV_420P; }


    //: Step to the next pixel
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_YUV_420P>& next()
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

    //: Pre-increment: step to the next pixel
    virtual vidl2_pixel_iterator& operator++ ()
    {
      return this->next();
    }

    //: Access the data
    vxl_byte operator () (unsigned int i) const
    {
      assert(i<3);
      return *ptr_[i];
    }

    //: Access the entire pixel at once
    void get(vxl_byte* data) const
    {
      data[0] = *ptr_[0];
      data[1] = *ptr_[1];
      data[2] = *ptr_[2];
    }

    //: write the entire pixel at once
    void set(const vxl_byte* data)
    {
      *ptr_[0] = data[0];
      *ptr_[1] = data[1];
      *ptr_[2] = data[2];
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
    unsigned int line_size_;
    unsigned int line_cnt_;
    bool mode_x_;
    bool mode_y_;
    vxl_byte * ptr_[3];
};


//: Iterator for YUV 422 planar images
VCL_DEFINE_SPECIALIZATION 
struct vidl2_pixel_iterator_valid<VIDL2_PIXEL_FORMAT_YUV_422P>
{ enum { value = true }; };

VCL_DEFINE_SPECIALIZATION 
class vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_YUV_422P>
  : public vidl2_pixel_iterator
{
  public:
    //: Constructor
    vidl2_pixel_iterator_of(const vidl2_frame& frame):
      line_size_(frame.ni()), line_cnt_(0), mode_(false)
    {
      assert(frame.pixel_format() == VIDL2_PIXEL_FORMAT_YUV_422P);
      assert(frame.ni()%2 == 0);

      unsigned size = frame.ni()*frame.nj();
      ptr_[0] = (vxl_byte*)frame.data();
      ptr_[1] = ptr_[0] + size;
      ptr_[2] = ptr_[1] + (size >> 1);
    }

    //: Destructor
    virtual ~vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_YUV_422P>(){}

    //: Return the pixel format
    virtual vidl2_pixel_format pixel_format() const
    { return VIDL2_PIXEL_FORMAT_YUV_422P; }


    //: Step to the next pixel
    vidl2_pixel_iterator_of<VIDL2_PIXEL_FORMAT_YUV_422P>& next()
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

    //: Pre-increment: step to the next pixel
    virtual vidl2_pixel_iterator& operator++ ()
    {
      return this->next();
    }

    //: Access the data
    vxl_byte operator () (unsigned int i) const
    {
      assert(i<3);
      return *ptr_[i];
    }

    //: Access the entire pixel at once
    void get(vxl_byte* data) const
    {
      data[0] = *ptr_[0];
      data[1] = *ptr_[1];
      data[2] = *ptr_[2];
    }

    //: write the entire pixel at once
    void set(const vxl_byte* data)
    {
      *ptr_[0] = data[0];
      *ptr_[1] = data[1];
      *ptr_[2] = data[2];
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
    unsigned int line_size_;
    unsigned int line_cnt_;
    bool mode_;
    vxl_byte * ptr_[3];
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



#endif // vidl2_pixel_iterator_h_

