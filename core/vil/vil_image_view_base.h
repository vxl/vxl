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
#include <vcl_cassert.h>
#include <vil2/vil2_pixel_format.h>
#include <vil2/vil2_smart_ptr.h>

//: An abstract base class of smart pointers to actual image data in memory.
// If you want an actual image, try instantiating vil2_image_view<T>.

class vil2_image_view_base
{
protected:
  unsigned ni_;
  unsigned nj_;
  unsigned nplanes_;

  vil2_image_view_base(unsigned ni, unsigned nj, unsigned nplanes):
  ni_(ni), nj_(nj), nplanes_(nplanes), reference_count_(0) {}

  //: Default is an empty one plane image
  //  Don't set nplanes_ to zero as it confuses resize(nx,ny) later
  vil2_image_view_base(): ni_(0), nj_(0), nplanes_(1), reference_count_(0) {}

public:
  // The destructor must be virtual so that the memory chunk is destroyed.
  virtual ~vil2_image_view_base() {};

  //: Width
  unsigned ni()  const {return ni_;}
  //: Height
  unsigned nj()  const {return nj_;}
  //: Number of planes
  unsigned nplanes() const {return nplanes_;}

  //: The number of pixels.
  unsigned size() const { return ni_ * nj_ * nplanes_; }

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
  // a vil2_image_view<vxl_byte>.
  virtual enum vil2_pixel_format pixel_format() const=0;

  //: True if this is (or is derived from) class s
  virtual bool is_class(vcl_string const& s) const;

 private:
  // You probably should not use a vil2_image_view in a vbl_smart_ptr, so the
  // ref functions are private
  friend class vil2_smart_ptr<vil2_image_view_base>;
  void ref() { ++reference_count_; }
  void unref() {
    assert(reference_count_>0);
    if (--reference_count_<=0) delete this;}
  int reference_count_;
};

typedef vil2_smart_ptr<vil2_image_view_base> vil2_image_view_base_sptr;

//: Print a 1-line summary of contents
inline
vcl_ostream& operator<<(vcl_ostream& s, vil2_image_view_base const& im) {
  im.print(s); return s;
}

#endif // vil2_image_view_base_h_
