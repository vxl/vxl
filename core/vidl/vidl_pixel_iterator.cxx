// This is core/vidl/vidl_pixel_iterator.cxx
#include "vidl_pixel_iterator.h"
#include "vidl_pixel_iterator.hxx"
//:
// \file
// \author Matt Leotta
//

namespace {

//: Recursive template metaprogram to populate the "vidl_has_iterator" array
template <vidl_pixel_format pix_type>
struct populate_has_iterator
{
  static inline void apply(bool* element)
  {
    *element = vidl_pixel_iterator_valid<pix_type>::value;
    populate_has_iterator<vidl_pixel_format(pix_type-1)>::apply(--element);
  }
};

//: The base case: unknown pixel type
template <>
struct populate_has_iterator<VIDL_PIXEL_FORMAT_UNKNOWN>
{
  static inline void apply(bool* /*element*/)
  {
    return;
  }
};


//: The array indicating which formats have valid iterators
struct vidl_has_iterator
{
  public:
    vidl_has_iterator()
    {
      populate_has_iterator<vidl_pixel_format(VIDL_PIXEL_FORMAT_ENUM_END-1)>
        ::apply(&array[VIDL_PIXEL_FORMAT_ENUM_END-1]);
    }

    bool operator [] (unsigned int i)
    { return array[i]; }

  private:
    bool array[VIDL_PIXEL_FORMAT_ENUM_END];
};

//: The single static instance of vidl_has_iterator
vidl_has_iterator has_iterator;


//: Recursive template metaprogram to make a pixel_iterator
template <vidl_pixel_format pix_type>
struct make_pixel_iterator
{
  static inline vidl_pixel_iterator* apply(const vidl_frame& frame)
  {
    if (frame.pixel_format() == pix_type){
      if (vidl_pixel_iterator_valid<pix_type>::value)
        return new vidl_pixel_iterator_of<pix_type>(frame);
      return (vidl_pixel_iterator*)nullptr;
    }
    return make_pixel_iterator<vidl_pixel_format(pix_type-1)>::apply(frame);
  }
};

//: The base case: unknown pixel type
template <>
struct make_pixel_iterator<VIDL_PIXEL_FORMAT_UNKNOWN>
{
  static inline vidl_pixel_iterator* apply(vidl_frame const& /*frame*/)
  {
    return (vidl_pixel_iterator*)nullptr;
  }
};

//=============================================================================

}; // anonymous namespace


//: Pixel iterator factory
// Creates a new pixel iterator on the heap
// The iterator is initialized to the first pixel in the frame
// \note The user is responsible for deleting the iterator
vidl_pixel_iterator*
vidl_make_pixel_iterator(const vidl_frame& frame)
{
  return make_pixel_iterator<vidl_pixel_format(VIDL_PIXEL_FORMAT_ENUM_END-1)>::apply(frame);
}


//: Return true if the pixel format has a valid pixel iterator implementation
bool vidl_has_pixel_iterator(vidl_pixel_format fmt)
{
  return has_iterator[fmt];
}
