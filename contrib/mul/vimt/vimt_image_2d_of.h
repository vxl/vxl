// This is mul/vimt/vimt_image_2d_of.h
#ifndef vimt_image_2d_of_h_
#define vimt_image_2d_of_h_
//:
//  \file
//  \brief Container for vil2_image_view<T> + transform
//  \author Tim Cootes

#include <vimt/vimt_image_2d.h>
#include <vil2/vil2_image_view.h>

//: Represent 2D image of type T together with a transform.
//  Each plane is ni() x nj() Ts, with the (x,y) element
//  of the i'th plane accessible using im.plane(i)[x*im.istep() + y*im.jstep()]
template<class T>
class vimt_image_2d_of : public vimt_image_2d
{
  vil2_image_view<T> image_;
 public:
  //: Dflt ctor
  //  Creates an empty one plane image.
  vimt_image_2d_of();

  //: Create a n_plane plane image of nx x ny pixels
  vimt_image_2d_of(unsigned nx, unsigned ny, unsigned n_planes=1);

  //: Construct from a view and a world-to-image transform (takes copies of both)
  vimt_image_2d_of(const vil2_image_view<T>& view, const vimt_transform_2d& w2i)
    : vimt_image_2d(w2i),image_(view) {}

  //: Destructor
  virtual ~vimt_image_2d_of();

  //: Baseclass view of image
  virtual const vil2_image_view_base& image_base() const { return image_; }

  //: Image view
  vil2_image_view<T>& image() { return image_; }

  //: Image view
  const vil2_image_view<T>& image() const { return image_; }

  //: Arrange that this is window on given image.
  //  The parameters should be in image co-ords.
  //  The world2im transform is set to match
  //  so this appears identical to im when addressed
  //  in world co-ords.
  void set_to_window(const vimt_image_2d_of& im,
                     unsigned x0, unsigned nx, unsigned y0, unsigned ny);

  //: Create windowed view of given image
  //  The parameters should be in image co-ords.
  //  The world2im transform is set to match
  //  so this appears identical to im when addressed
  //  in world co-ords.
  vimt_image_2d_of<T> window(unsigned x0, unsigned nx, unsigned y0, unsigned ny) const;

  //: True if transforms are equal, and they share same image data.
  //  This does not do a deep equality on image data. If the images point
  //  to different image data objects that contain identical images, then
  //  the result will still be false.
  bool operator==(const vimt_image_2d_of<T> &) const;

  //: Define valid data region (including transform).
  //  Resizes and sets the tranformation so that
  //  world2im(x,y) is valid for all points in range
  //  Specifically, resize(nx,ny);
  //  world2im() translates by (-xlo,-ylo)
  //
  //  WARNING - confusing choice of parameters? Not consistent with vil2_image_view
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

#endif // vimt_image_2d_of_h_
