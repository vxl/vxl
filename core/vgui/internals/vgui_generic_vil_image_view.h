// This is core/vgui/internals/vgui_generic_vil_image_view.h
#ifndef vgui_generic_vil_image_view_h_
#define vgui_generic_vil_image_view_h_

//:
// \file
// \author Amitha Perera
// \date   Aug 2003

#include <vcl_cstddef.h> // for ptrdiff_t

#include <vil/vil_image_view_base.h>
#include <vil/vil_memory_chunk.h>
#include <vil/vil_pixel_format.h>

//: Stores enough information to re-create a vil_image_view
//
// This class is used in vgui_vil_image_renderer to store the details
// of the image view that it is asked to render. We can't store a
// vil_image_view_base, since it is abstract. We cannot directly store
// a vil_image_view<T> since then we would need a different renderer
// for each type T. (This was the old solution, and the result was a
// mess.) Besides, the rendering process involves converting the input
// data pixels into GL pixels at run-time; knowing the input pixel
// type at compile time adds very little value.
//
// Using a vgui_generic_vil_image_view, the renderer can store enough
// information to reconstuct the view as necessary. The pixel type is
// stored in a variable and used at run-time, so the same renderer
// code can be used for all image pixel types. This allows for a
// single vgui_image_tableau. (Instead of the old solution that had a
// templated vgui_image_tableau.)
//
// If we had member templates, most of this would be so easy...
//
class vgui_generic_vil_image_view
{
protected:
  //: Number of columns.
  unsigned ni_;
  //: Number of rasters.
  unsigned nj_;
  //: Number of planes.
  unsigned nplanes_;

  //: Pointer to pixel at origin.
  void const * top_left_;
  //: Add this to a pixel pointer to move one column left.
  vcl_ptrdiff_t istep_;
  //: Add this to a pixel pointer to move one row down.
  vcl_ptrdiff_t jstep_;
  //: Add this to a pixel pointer to move one plane back.
  vcl_ptrdiff_t planestep_;

  //: Reference to actual image data.
  vil_memory_chunk_sptr ptr_;

  //: Pixel type of this data
  vil_pixel_format pixel_format_;

public:
  vgui_generic_vil_image_view( );

  vgui_generic_vil_image_view( vil_image_view_base const& other );

  void operator=( vil_image_view_base const& other );

  //: Data start
  inline const void * top_left_ptr() const { return top_left_; }

  //: Width
  unsigned ni()  const {return ni_;}
  //: Height
  unsigned nj()  const {return nj_;}
  //: Number of planes
  unsigned nplanes() const {return nplanes_;}

  //: Add this to your pixel pointer to get next i pixel.
  inline vcl_ptrdiff_t istep() const { return istep_; }

  //: Add this to your pixel pointer to get next j pixel.
  inline vcl_ptrdiff_t jstep() const { return jstep_; }

  //: Add this to your pixel pointer to get pixel on next plane.
  inline vcl_ptrdiff_t planestep() const { return planestep_; }

  //: Smart pointer to the object holding the data for this view
  //
  // Will be a null pointer if this view looks at `third-party' data,
  // e.g. using set_to_memory.
  //
  inline const vil_memory_chunk_sptr& memory_chunk() const { return ptr_; }

  //: Cast to bool is true if pointing at some data.
  operator bool () const { return top_left_ != (void const*)0; }

  //: Return a base pointer to a new vil_image_view<T>
  //
  // This will construct a new vil_image_view<T> with an appropriate
  // T, and return a base class smart pointer.
  //
  vil_image_view_base_sptr make_view() const;

  //: Return a description of the concrete data pixel type.
  //
  // For example if the value is VIL_PIXEL_FORMAT_BYTE, you can safely
  // cast or assign the result if make_view() to a
  // vil_image_view<vxl_byte>.
  //
  enum vil_pixel_format pixel_format() const;

};

#endif // vgui_generic_vil_image_view_h_
