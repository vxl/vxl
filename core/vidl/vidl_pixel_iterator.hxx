// This is core/vidl/vidl_pixel_iterator.hxx
#ifndef vidl_pixel_iterator_hxx_
#define vidl_pixel_iterator_hxx_
//:
// \file
// \brief Templated definitions for pixel iterators
//
// \author Matt Leotta
// \date 3 Mar 2006
//
// This file contains the template definitions for pixel iterators


#include "vidl_pixel_iterator.h"
#include "vidl_color.h"
#include <vxl_config.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-----------------------------------------------------------------------------


template <vidl_pixel_arrangement AR>
struct vidl_pixel_iterator_arrange_valid
{
  enum { value = (AR == VIDL_PIXEL_ARRANGE_SINGLE) };
};


//: The default pixel iterator for single arranged formats
template <vidl_pixel_arrangement AR, vidl_pixel_format FMT>
class vidl_pixel_iterator_arranged
{
  typedef typename vidl_pixel_traits_of<FMT>::type cmp_type;
  cmp_type * ptr_;
 public:
  //: Constructor
  vidl_pixel_iterator_arranged(const vidl_frame& frame)
    : ptr_((cmp_type*)frame.data())
  {
    assert(frame.pixel_format() == FMT);
    // The following should be a static asserts
    assert(vidl_pixel_traits_of<FMT>::bits_per_pixel%8 == 0);
    assert(vidl_pixel_traits_of<FMT>::arrangement() == VIDL_PIXEL_ARRANGE_SINGLE);
  }

  //: Destructor
  ~vidl_pixel_iterator_arranged() = default;

  //: Step to the next pixel
  vidl_pixel_iterator_arranged<AR,FMT>& next()
  {
    ptr_ += vidl_pixel_traits_of<FMT>::bits_per_pixel/(sizeof(cmp_type)*8);
    return *this;
  }

  //: Access the data
  cmp_type operator () (unsigned int i) const
  {
    assert(i<vidl_pixel_traits_of<FMT>::num_channels);
    return vidl_color_component<FMT>::get(ptr_,i);
  }

  //: Access the entire pixel at once
  void get(cmp_type* data) const
  {
    vidl_color_component<FMT>::get_all(ptr_,data);
  }

  //: write the entire pixel at once
  void set(const cmp_type* data)
  {
    vidl_color_component<FMT>::set_all(ptr_,data);
  }
};

template <>
struct vidl_pixel_iterator_arrange_valid<VIDL_PIXEL_ARRANGE_PLANAR>
{
  enum { value = true };
};


//: The default pixel iterator for planar arranged formats
template <vidl_pixel_format FMT>
class vidl_pixel_iterator_arranged<VIDL_PIXEL_ARRANGE_PLANAR,FMT>
{
  typedef typename vidl_pixel_traits_of<FMT>::type cmp_type;
  enum { csx = vidl_pixel_traits_of<FMT>::chroma_shift_x };
  enum { csy = vidl_pixel_traits_of<FMT>::chroma_shift_y };
  enum { x_mask = (1<<(csx+1))-1 }; // last csx+1 bits are 1
  enum { y_mask = (1<<(csy+1))-1 }; // last csy+1 bits are 1
  unsigned int line_size_;
  unsigned int line_cnt_;
  cmp_type * ptr_[vidl_pixel_traits_of<FMT>::num_channels];
  //: these act as fractional pixel counters
  vxl_byte step_x_, step_y_;
 public:
  //: Constructor
  vidl_pixel_iterator_arranged(const vidl_frame& frame)
    : line_size_(frame.ni()), line_cnt_(0), step_x_(1), step_y_(1)
  {
    assert(frame.pixel_format() == FMT);
    // The following should be a static asserts
    assert(vidl_pixel_traits_of<FMT>::arrangement() == VIDL_PIXEL_ARRANGE_PLANAR);

    const unsigned size = frame.ni()*frame.nj();

    ptr_[0] = (cmp_type*)frame.data();
    for (unsigned int i=1; i<vidl_pixel_traits_of<FMT>::num_channels; ++i) {
      if (i==1)
        ptr_[i] = ptr_[i-1] + size;
      else
        ptr_[i] = ptr_[i-1] + ((size>>csx)>>csy);
    }
  }

  //: Destructor
  ~vidl_pixel_iterator_arranged() = default;

  //: Step to the next pixel
  vidl_pixel_iterator_arranged<VIDL_PIXEL_ARRANGE_PLANAR,FMT>& next()
  {
    ++ptr_[0];
    if (vidl_pixel_traits_of<FMT>::num_channels > 1) {
      // step only if the last csx+1 bits of step_x_ are set
      int chroma_step = ((step_x_&x_mask) == x_mask)?1:0;
      if (++line_cnt_ < line_size_) {
        step_x_ += 2;
      }
      else
      {
        line_cnt_ = 0;
        step_x_=1;
        chroma_step = 1;
        // step back to start of row unless the last csy+1 bits of step_y_ are set
        if (!((step_y_&y_mask)==y_mask))
          chroma_step -= (line_size_>>csx);
        step_y_ += 2;
      }
      for (unsigned int i=1; i<vidl_pixel_traits_of<FMT>::num_channels; ++i) {
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
    for (unsigned int i=0; i<vidl_pixel_traits_of<FMT>::num_channels; ++i)
      data[i] = *ptr_[i];
  }

  //: write the entire pixel at once
  void set(const cmp_type* data)
  {
    for (unsigned int i=0; i<vidl_pixel_traits_of<FMT>::num_channels; ++i)
      *ptr_[i] = data[i];
  }
};


template <>
struct vidl_pixel_iterator_arrange_valid<VIDL_PIXEL_ARRANGE_PACKED>
{
  enum { value = true };
};

//: The default pixel iterator for packed arranged formats
template <vidl_pixel_format FMT>
class vidl_pixel_iterator_arranged<VIDL_PIXEL_ARRANGE_PACKED,FMT>
{
  typedef typename vidl_pixel_traits_of<FMT>::type cmp_type;
  cmp_type * ptr_;
  enum { macro_pix_size = 1<<vidl_pixel_traits_of<FMT>::chroma_shift_x };
  enum { pix_step_size = (vidl_pixel_traits_of<FMT>::bits_per_pixel
                          <<vidl_pixel_traits_of<FMT>::chroma_shift_x)>>3 };
  vxl_byte mode_;
 public:

  //: Constructor
  vidl_pixel_iterator_arranged(const vidl_frame& frame)
    : ptr_((vxl_byte*)frame.data()), mode_(0)
  {
  }

  //: Destructor
  ~vidl_pixel_iterator_arranged() = default;

  //: Step to the next pixel
  vidl_pixel_iterator_arranged<VIDL_PIXEL_ARRANGE_PACKED,FMT>& next()
  {
    mode_ = vxl_byte((mode_+1)%macro_pix_size);
    if (mode_==0)
      ptr_ += pix_step_size;
    return *this;
  }

  //: Access the data
  cmp_type operator () (unsigned int i) const
  {
    assert(i<vidl_pixel_traits_of<FMT>::num_channels);
    return ptr_[vidl_pixel_pack_of<FMT>::offset[mode_][i]];
  }

  //: Access the entire pixel at once
  void get(cmp_type* data) const
  {
    for (unsigned int i=0; i<vidl_pixel_traits_of<FMT>::num_channels; ++i)
      data[i] = ptr_[vidl_pixel_pack_of<FMT>::offset[mode_][i]];
  }

  //: write the entire pixel at once
  void set(const cmp_type* data)
  {
    for (unsigned int i=0; i<vidl_pixel_traits_of<FMT>::num_channels; ++i)
      ptr_[vidl_pixel_pack_of<FMT>::offset[mode_][i]] = data[i];
  }
};


template <vidl_pixel_format FMT>
struct vidl_pixel_iterator_valid
{
  enum { value = vidl_pixel_iterator_arrange_valid<
    vidl_pixel_arrangement(vidl_pixel_traits_of<FMT>::arrangement_idx) >::value };
};


//: The default pixel iterator
// used for non-planar non-packed formats
template <vidl_pixel_format FMT>
class vidl_pixel_iterator_of : public vidl_pixel_iterator
{
  enum { arrangement = vidl_pixel_traits_of<FMT>::arrangement_idx };
  typedef vidl_pixel_iterator_arranged<vidl_pixel_arrangement(arrangement),FMT> arranged_itr;
  arranged_itr itr_;
  typedef typename vidl_pixel_traits_of<FMT>::type cmp_type;
 public:
  //: Constructor
  vidl_pixel_iterator_of(const vidl_frame& frame) : itr_(frame) {}

  //: Destructor
  ~vidl_pixel_iterator_of<FMT>() override = default;

  //: Return the pixel format
  vidl_pixel_format pixel_format() const override
  { return FMT; }

  //: Pre-increment: step to the next pixel
  vidl_pixel_iterator& operator++ () override
  { itr_.next(); return *this; }

  //: Access the data
  cmp_type operator () (unsigned int i) const
  { return itr_(i); }

  //: Copy the pixel data into a byte array
  void get_data(vxl_byte* data) const override
  { itr_.get(reinterpret_cast<cmp_type*>(data)); }

  //: Set the pixel data from a byte array
  void set_data(const vxl_byte* data) override
  { itr_.set(reinterpret_cast<const cmp_type*>(data)); }
};


//-----------------------------------------------------------------------------
// Custom Pixel Iterators
//-----------------------------------------------------------------------------


//: Iterator for monochrome boolean images
template <>
struct vidl_pixel_iterator_valid<VIDL_PIXEL_FORMAT_MONO_1>
{ enum { value = true }; };

template <>
class vidl_pixel_iterator_of<VIDL_PIXEL_FORMAT_MONO_1>
  : public vidl_pixel_iterator
{
  vxl_byte bit_mask_;
  vxl_byte * ptr_;
 public:
  //: Constructor
  vidl_pixel_iterator_of(const vidl_frame& frame)
    : bit_mask_(128), ptr_((vxl_byte*)frame.data())
  {
    assert(frame.pixel_format() == VIDL_PIXEL_FORMAT_MONO_1);
  }

  //: Destructor
  ~vidl_pixel_iterator_of<VIDL_PIXEL_FORMAT_MONO_1>() override = default;

  //: Return the pixel format
  vidl_pixel_format pixel_format() const override
  { return VIDL_PIXEL_FORMAT_MONO_1; }

  //: Step to the next pixel
  vidl_pixel_iterator_of<VIDL_PIXEL_FORMAT_MONO_1>& next()
  {
    bit_mask_ >>= 1;
    if (!bit_mask_) {
      bit_mask_ = 128;
      ++ptr_;
    }

    return *this;
  }

  //: Pre-increment: step to the next pixel
  vidl_pixel_iterator& operator++ () override
  {
    return this->next();
  }

  //: Access the data
  bool operator () (unsigned int i) const
  {
    assert(i==0);
    return (ptr_[0] & bit_mask_) != 0;
  }

  //: Access the entire pixel at once
  void get(bool* data) const
  {
    data[0] = (ptr_[0] & bit_mask_) != 0;
  }

  //: write the entire pixel at once
  void set(const bool* data)
  {
    ptr_[0] &= ~bit_mask_ & (data[0]?bit_mask_:0);
  }

  //: Copy the pixel data into a byte array
  void get_data(vxl_byte* data) const override
  {
    this->get(reinterpret_cast<bool*>(data));
  }

  //: Set the pixel data from a byte array
  void set_data(const vxl_byte* data) override
  {
    this->set(reinterpret_cast<const bool*>(data));
  }
};


#endif // vidl_pixel_iterator_hxx_
