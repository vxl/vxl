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

//: An abstract base class of smart pointers to actual image data in memory.
// If you want an actual image, try instantiating vil_image_view<T>.

class vil2_image_view_base
{
protected:
  unsigned nx_;
  unsigned ny_;
  unsigned nplanes_;

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
  vcl_ostream& print(vcl_ostream&) const;

};

//: Print a 1-line summary of contents
inline
vcl_ostream& operator<<(vcl_ostream& s, vil2_image_view_base const& i) {
  return i.print(s);
}

#endif // vil2_image_view_base_h_
