// This is mul/vil2/vil2_image_view_base.h
#ifndef vil2_image_view_base_h_
#define vil2_image_view_base_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A base class reference-counting view of some image data.
// \author Ian Scott - Manchester

#include <vcl_iosfwd.h>
#include <vcl_string.h>

//: An abstract base class of smart pointers to actual image data in memory.
// If you want an actual image, try instantiating vil_image_view<T>.

class vil2_image_view_base
{
protected:
  unsigned nx_;
  unsigned ny_;
  unsigned nplanes_;

  vil2_image_view_base(unsigned nx, unsigned ny, unsigned nplanes):
  nx_(nx), ny_(ny), nplanes_(nplanes) {}

  vil2_image_view_base(): nx_(0), ny_(0), nplanes_(0) {}

public:

  //: Width
  unsigned nx()  const {return nx_;}
  //: Height
  unsigned ny()  const {return ny_;}
  //: Number of planes
  unsigned nplanes() const {return nplanes_;}

  //: The number of pixels.
  unsigned size() const { return ny() * nx() * nplanes(); }

  //: resize current planes to width x height
  // If already correct size, this function returns quickly
  virtual void resize(unsigned width, unsigned height) =0;

  //: resize to width x height x nplanes
  // If already correct size, this function returns quickly
  virtual void resize(unsigned width, unsigned height, unsigned nplanes) =0;
  
  //: Print a 1-line summary of contents
  virtual void print(vcl_ostream&) const =0;

    //: Return class name
  virtual vcl_string is_a() const;

    //: Return a description of the concrete data pixel type.
    // For example if the value is VIL2_PIXEL_FORMAT_VIL_BYTE,
    // you can safely cast, or assign the base class reference to
    // a vil2_image_view<vil_byte>.
  virtual enum vil2_pixel_format pixel_format() const=0;

    //: True if this is (or is derived from) class s
  virtual bool is_class(vcl_string const& s) const;
};

//: Print a 1-line summary of contents
inline
vcl_ostream& operator<<(vcl_ostream& s, vil2_image_view_base const& i) {
  i.print(s); return s;
}

#endif // vil2_image_view_base_h_
