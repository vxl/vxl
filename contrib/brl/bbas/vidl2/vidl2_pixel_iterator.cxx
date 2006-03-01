// This is brl/bbas/vidl2/vidl2_pixel_iterator.cxx
#include "vidl2_pixel_iterator.h"
//:
// \file
// \author Matt Leotta
//

#include <vcl_iostream.h>

namespace {

//: Recursive template metaprogram to populate the "vidl2_has_iterator" array
template <vidl2_pixel_format pix_type>
struct populate_has_iterator
{
  static inline void apply(bool* element)
  {
    *element = vidl2_pixel_iterator_valid<pix_type>::value;
    populate_has_iterator<vidl2_pixel_format(pix_type-1)>::apply(--element);
  }
};

//: The base case: unknown pixel type
VCL_DEFINE_SPECIALIZATION
struct populate_has_iterator<VIDL2_PIXEL_FORMAT_UNKNOWN>
{
  static inline void apply(bool* element)
  {
    return;
  }
};


//: The array indicating which formats have valid iterators
struct vidl2_has_iterator
{
  public:
    vidl2_has_iterator()
    {
      populate_has_iterator<vidl2_pixel_format(VIDL2_PIXEL_FORMAT_ENUM_END-1)>
        ::apply(&array[VIDL2_PIXEL_FORMAT_ENUM_END-1]);
    }

    bool operator [] (unsigned int i)
    { return array[i]; }

  private:
    bool array[VIDL2_PIXEL_FORMAT_ENUM_END];
};

//: The single static instance of vidl2_has_iterator
vidl2_has_iterator has_iterator;



//: Recursive template metaprogram to make a pixel_iterator
template <vidl2_pixel_format pix_type>
struct make_pixel_iterator
{
  static inline vidl2_pixel_iterator* apply(const vidl2_frame& frame)
  {
    if(frame.pixel_format() == pix_type){
      if(vidl2_pixel_iterator_valid<pix_type>::value)
        return new vidl2_pixel_iterator_of<pix_type>(frame);
      return NULL;
    }
    return make_pixel_iterator<vidl2_pixel_format(pix_type-1)>::apply(frame);
  }
};

//: The base case: unknown pixel type
VCL_DEFINE_SPECIALIZATION
struct make_pixel_iterator<VIDL2_PIXEL_FORMAT_UNKNOWN>
{
  static inline vidl2_pixel_iterator* apply(const vidl2_frame& frame)
  {
    return NULL;
  }
};



//=============================================================================

};


//: Pixel iterator factory
// Creates a new pixel iterator on the heap
// The iterator is initialized to the first pixel in the frame
// \note The user is responsible for deleting the iterator
vidl2_pixel_iterator*
vidl2_make_pixel_iterator(const vidl2_frame& frame)
{
  return make_pixel_iterator<vidl2_pixel_format(VIDL2_PIXEL_FORMAT_ENUM_END-1)>::apply(frame);
}


//: Return true if the pixel format has a valid pixel iterator implementation
bool vidl2_has_pixel_iterator(vidl2_pixel_format fmt)
{
  return has_iterator[fmt];
}
