// This is contrib/brl/bbas/bil/bil_bounded_image_view.h
#ifndef bil_bounded_image_view_h_
#define bil_bounded_image_view_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A type of image view that masquerades as a larger image
// \author J.L. Mundy
// \verbatim
//  Modifications <none>
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_cstddef.h>
#include <vil/vil_image_view.h>
#include <vsl/vsl_binary_io.h>

//  The actual image data is contained in the parent of this class.
//  The parent has data (ni(), nj(), nplanes());
//  This view considers that data to be a region inside a larger image 
//  space with an offset, (ib0(), jb0()),  the size of this view is
//  (nib(), njb()) and the same number of planes.
// The purpose of the bounded view is support uniform processing
// on a specified image space, even though only a variable portion 
// of the image is valid. This situation arises, for example, in 
// projecting a given 3-d region into a set of camera views of 
// the region.
template <class T>
class bil_bounded_image_view : public vil_image_view<T>
{
 protected:
  //: zero value
  T zero_;
  //: column origin of the bounded data
  unsigned ib0_;
  //: row orgin of the bounded data
  unsigned jb0_;
  //: number of columns in the image space
  unsigned nib_;
  //: number of rows in the image space
  unsigned njb_;

 public:
   //: this constructor should not be used
  bil_bounded_image_view() {}

  //: the main constructor
  bil_bounded_image_view(const vil_image_view<T>& bounded_data,
                          unsigned ib0, unsigned jb0,//data origin
                          unsigned nib, unsigned njb //global image size
                          );

  //: Copy constructor
  //  For simplicity, the pixel format of the bounded view must be the same
  //  as the bounded data. If an attempt is to bind inconsistent views,
  //  the resulting view will be empty;
  bil_bounded_image_view(const bil_bounded_image_view<T>& rhs);

  //: Construct from abstract base
  bil_bounded_image_view(const vil_image_view_base& rhs);

  //: Construct from abstract base pointer
  bil_bounded_image_view(const vil_image_view_base_sptr& rhs)
    { operator=(rhs);}

  ~bil_bounded_image_view() {}
  //:accessors
  unsigned ib0() const {return ib0_;}
  unsigned jb0() const {return jb0_;}
  unsigned nib() const {return nib_;}
  unsigned njb() const {return njb_;}

  //:The global pixel access methods
  // Note! pixel access methods of the parent will return data according
  // to the bounded view. If (i,j) is outside the bounds then (T)0 is returned
  // If (i,j) is outside (nib-1, njb-1) an assertion is thrown
  // according to ordinary image indexing stuff.

  //: Return read-only reference to pixel at (i,j) in plane 0.
  const T& gpix(unsigned i, unsigned j) const;

  //: Return read/write reference to pixel at (i,j) in plane 0.
  T&  gpix(unsigned i, unsigned j);

  //: Return read-only reference to pixel at (i,j) in plane p.
  const T& gpix(unsigned i, unsigned j, unsigned p) const;

  //: Return read-only reference to pixel at (i,j) in plane p.
  T&       gpix(unsigned i, unsigned j, unsigned p);

  //: This operator de-references an image_view sptr, returning an empty view if the pointer is null.
  inline const bil_bounded_image_view<T>& operator=(const vil_image_view_base_sptr& rhs)
  {
    if (!rhs)
      *this = *(new bil_bounded_image_view<T>(vil_image_view<T>(), 0, 0, 0, 0));
    else
      *this = *rhs;
    return *this;
  }

  short version() const { return 1; } 

  void b_write(vsl_b_ostream &os) const;

  void b_read(vsl_b_istream &is);
};

#endif // bil_bounded_image_view_h_
