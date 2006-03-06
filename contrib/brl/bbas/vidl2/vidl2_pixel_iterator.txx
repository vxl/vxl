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
class vidl2_pixel_iterator_arranged
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
    ~vidl2_pixel_iterator_arranged<AR,FMT>(){}

    //: Step to the next pixel
    vidl2_pixel_iterator_arranged<AR,FMT>& next()
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
{
  typedef typename vidl2_pixel_traits_of<FMT>::type cmp_type;
  enum { csx = vidl2_pixel_traits_of<FMT>::chroma_shift_x };
  enum { csy = vidl2_pixel_traits_of<FMT>::chroma_shift_y };
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
    ~vidl2_pixel_iterator_arranged(){}

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

  private:
    unsigned int line_size_;
    unsigned int line_cnt_;
    cmp_type * ptr_[vidl2_pixel_traits_of<FMT>::num_channels];
    vxl_byte step_x_, step_y_;
};


template <>
struct vidl2_pixel_iterator_arrange_valid<VIDL2_PIXEL_ARRANGE_PACKED>
{
  enum { value = true };
};

//: The default pixel iterator for packed arranged formats
template <vidl2_pixel_format FMT>
class vidl2_pixel_iterator_arranged<VIDL2_PIXEL_ARRANGE_PACKED,FMT>
{
  typedef typename vidl2_pixel_traits_of<FMT>::type cmp_type;
  enum { macro_pix_size = 1<<vidl2_pixel_traits_of<FMT>::chroma_shift_x };
  enum { pix_step_size = (vidl2_pixel_traits_of<FMT>::bits_per_pixel
                          <<vidl2_pixel_traits_of<FMT>::chroma_shift_x)>>3 };
  public:

    //: Constructor
    vidl2_pixel_iterator_arranged(const vidl2_frame& frame)
      : mode_(0), ptr_((vxl_byte*)frame.data())
    {
    }

    //: Destructor
    ~vidl2_pixel_iterator_arranged(){}

    //: Step to the next pixel
    vidl2_pixel_iterator_arranged<VIDL2_PIXEL_ARRANGE_PACKED,FMT>& next()
    {
      mode_ = (mode_+1)%macro_pix_size;
      if(mode_==0)
        ptr_ += pix_step_size;
      return *this;
    }

    //: Access the data
    cmp_type operator () (unsigned int i) const
    {
      assert(i<vidl2_pixel_traits_of<FMT>::num_channels);
      return ptr_[vidl2_pixel_pack_of<FMT>::offset[mode_][i]];
    }

    //: Access the entire pixel at once
    void get(cmp_type* data) const
    {
      for(unsigned int i=0; i<vidl2_pixel_traits_of<FMT>::num_channels; ++i)
        data[i] = ptr_[vidl2_pixel_pack_of<FMT>::offset[mode_][i]];
    }

    //: write the entire pixel at once
    void set(const cmp_type* data)
    {
      for(unsigned int i=0; i<vidl2_pixel_traits_of<FMT>::num_channels; ++i)
        ptr_[vidl2_pixel_pack_of<FMT>::offset[mode_][i]] = data[i];
    }


  private:
    vxl_byte mode_;
    cmp_type * ptr_;
};


#endif


template <vidl2_pixel_format FMT>
struct vidl2_pixel_iterator_valid
{
  enum { value = vidl2_pixel_iterator_arrange_valid<
    vidl2_pixel_arrangement(vidl2_pixel_traits_of<FMT>::arrangement_idx) >::value };
};


//: The default pixel iterator
// used for non-planar non-packed formats
template <vidl2_pixel_format FMT>
class vidl2_pixel_iterator_of : public vidl2_pixel_iterator
{
  enum { arrangement = vidl2_pixel_traits_of<FMT>::arrangement_idx };
  typedef vidl2_pixel_iterator_arranged<vidl2_pixel_arrangement(arrangement),FMT> arranged_itr;
  typedef typename vidl2_pixel_traits_of<FMT>::type cmp_type;
  public:
    //: Constructor
    vidl2_pixel_iterator_of(const vidl2_frame& frame) : itr_(frame){}

    //: Destructor
    virtual ~vidl2_pixel_iterator_of<FMT>(){}

    //: Return the pixel format
    virtual vidl2_pixel_format pixel_format() const
    { return FMT; }

    //: Pre-increment: step to the next pixel
    virtual vidl2_pixel_iterator& operator++ ()
    { itr_.next(); return *this; }

    //: Access the data
    cmp_type operator () (unsigned int i) const
    { return itr_(i); }

    //: Copy the pixel data into a byte array
    virtual void get_data(vxl_byte* data) const
    { itr_.get(reinterpret_cast<cmp_type*>(data)); }

    //: Set the pixel data from a byte array
    virtual void set_data(const vxl_byte* data)
    { itr_.set(reinterpret_cast<const cmp_type*>(data)); }

  private:
    arranged_itr itr_;
};


//-----------------------------------------------------------------------------
// Custom Pixel Iterators
//-----------------------------------------------------------------------------



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

