// This is mul/vimt3d/vimt3d_image_3d_of.h
#ifndef vimt3d_image_3d_of_h_
#define vimt3d_image_3d_of_h_
//:
// \file
// \brief Container for vil3d_image_view<T> + transform
// \author Tim Cootes

#include <vimt3d/vimt3d_image_3d.h>
#include <vil3d/vil3d_image_view.h>
#include <vcl_iosfwd.h>

//: Represent 2D image of type T together with a transform.
//  Each plane is ni() x nj() Ts, with the (x,y) element
//  of the i'th plane accessible using im.plane(i)[x*im.istep() + y*im.jstep()]
template<class T>
class vimt3d_image_3d_of : public vimt3d_image_3d
{
  vil3d_image_view<T> image_;

  //: Shallow equality tester.
  //  The parameter must be identical type to this.
  bool equals(const vimt_image &) const;
 public:
  //: Dflt ctor
  //  Creates an empty one-plane image.
  vimt3d_image_3d_of();

  //: Create an n_plane plane image of ni x nj pixels
  vimt3d_image_3d_of(unsigned ni, unsigned nj, unsigned nk, unsigned n_planes=1);

  //: Construct from a view and a world-to-image transform (takes copies of both)
  vimt3d_image_3d_of(const vil3d_image_view<T>& view, const vimt3d_transform_3d& w2i)
    : vimt3d_image_3d(w2i),image_(view) {}

  //: Destructor
  virtual ~vimt3d_image_3d_of();

  //: Baseclass view of image
  virtual const vil3d_image_view_base& image_base() const { return image_; }

  //: Image view
  vil3d_image_view<T>& image() { return image_; }

  //: Image view
  const vil3d_image_view<T>& image() const { return image_; }

  //: True if transforms are equal, and they share same image data.
  //  This does not do a deep equality on image data. If the images point
  //  to different image data objects that contain identical images, then
  //  the result will still be false.
  bool operator==(const vimt3d_image_3d_of<T> &) const;

  //: Define valid data region (including transform).
  //  Resizes and sets the transformation so that
  //  world2im(x,y) is valid for all points in range
  //  Specifically, set_valid_region(i0,ni,j0,nj,k0,nk);
  //  world2im() translates by (-i0,-j0,-k0)
  void set_valid_region(int i0, unsigned ni, int j0, unsigned nj,
                        int k0, unsigned nk);

  //: Take a deep copy of image (copy data, not just pointers)
  void deep_copy(const vimt3d_image_3d_of& image);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual vimt_image* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: print all data to os (rounds output to int)
  virtual void print_all(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // vimt3d_image_3d_of_h_
