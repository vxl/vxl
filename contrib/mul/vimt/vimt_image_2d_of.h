// This is mul/vimt/vimt_image_2d_of.h
#ifndef vimt_image_2d_of_h_
#define vimt_image_2d_of_h_
//:
// \file
// \brief Container for vil_image_view<T> + transform
// \author Tim Cootes

#include <vimt/vimt_image_2d.h>
#include <vil/vil_image_view.h>
#include <vcl_iosfwd.h>

//: Represent 2D image of type T together with a transform.
//  Each plane is ni() x nj() Ts, with the (x,y) element
//  of the i'th plane accessible using im.plane(i)[x*im.istep() + y*im.jstep()]
template<class T>
class vimt_image_2d_of : public vimt_image_2d
{
  vil_image_view<T> image_;

  //: Shallow equality tester.
  //  The parameter must be identical type to this.
  bool equals(const vimt_image &) const;

public:

  //: Default constructor
  //  Creates an empty one-plane image.
  vimt_image_2d_of() {}


  //: Construct an image of size (ni, nj, np) with optional world_to_image transform w2i.
  vimt_image_2d_of(unsigned ni, unsigned nj, unsigned np=1,
                   const vimt_transform_2d& w2i=vimt_transform_2d())
    : vimt_image_2d(w2i), image_(ni, nj, np) {}


  //: Construct from a view and a world-to-image transform (takes copies of both)
  vimt_image_2d_of(const vil_image_view<T>& view, const vimt_transform_2d& w2i)
    : vimt_image_2d(w2i), image_(view) {}

  
  //: Destructor
  virtual ~vimt_image_2d_of();

  //: Baseclass view of image
  virtual const vil_image_view_base& image_base() const { return image_; }

  //: Image view
  vil_image_view<T>& image() { return image_; }

  //: Image view
  const vil_image_view<T>& image() const { return image_; }

  //: True if transforms are equal, and they share same image data.
  //  This does not do a deep equality on image data. If the images point
  //  to different image data objects that contain identical images, then
  //  the result will still be false.
  bool operator==(const vimt_image_2d_of<T> &) const;

  //: Define valid data region (including transform).
  //  Resizes and sets the transformation so that
  //  world2im(x,y) is valid for all points in range
  //  Specifically, set_valid_region(x0,nx,y0,ny);
  //  world2im() translates by (-x0,-y0)
  void set_valid_region(int x0, unsigned nx, int y0, unsigned ny);

  //: Take a deep copy of image (copy data, not just pointers)
  void deep_copy(const vimt_image_2d_of& image);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

    //: Create a copy on the heap and return base class pointer
    //  Note that this will make a shallow copy of any contained images
  virtual vimt_image* clone() const;

    //: Create a deep copy on the heap and return base class pointer
    //  This will make a deep copy of any contained images
  virtual vimt_image* deep_clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: print all data to os (rounds output to int)
  virtual void print_all(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // vimt_image_2d_of_h_
