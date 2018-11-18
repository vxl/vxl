// This is core/vidl/vidl_pixel_iterator.h
#ifndef vidl_pixel_iterator_h_
#define vidl_pixel_iterator_h_
//:
// \file
// \brief Iterators over pixels in a frame
//
// \author Matt Leotta
// \date 16 Jan 2006
//
// This file contains the classes and functions to use pixel iterators
// in a polymorphic way.  Note that pixel iterators are not fast.
// They are intended to allow for the automatic generation of conversion
// routines between any pair of pixel formats.  Fast conversion functions
// for specific formats can be written to override the default ones when needed.


#include "vidl_pixel_format.h"
#include "vidl_frame.h"


//-----------------------------------------------------------------------------

//: Abstract pixel iterator base class
// A concrete pixel iterator for each pixel format is
// derived from this class (see vidl_pixel_format.hxx)
class vidl_pixel_iterator
{
  public:
    //: Destructor
    virtual ~vidl_pixel_iterator()= default;
    //: Return the pixel format
    virtual vidl_pixel_format pixel_format() const = 0;
    //: Pre-increment: step to the next pixel
    virtual vidl_pixel_iterator& operator++ () = 0;
    //: Copy the pixel data into a byte array
    virtual void get_data(vxl_byte* data) const = 0;
    //: Set the pixel data from a byte array
    virtual void set_data(const vxl_byte* data) = 0;
};


//: Pixel iterator factory
// Creates a new pixel iterator on the heap
// The iterator is initialized to the first pixel in the frame
// \note The user is responsible for deleting the iterator
vidl_pixel_iterator*
vidl_make_pixel_iterator(const vidl_frame& frame);


//: Return true if the pixel format has a valid pixel iterator implementation
bool vidl_has_pixel_iterator(vidl_pixel_format FMT);




#endif // vidl_pixel_iterator_h_
