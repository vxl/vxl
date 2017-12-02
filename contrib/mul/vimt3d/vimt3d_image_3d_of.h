// This is mul/vimt3d/vimt3d_image_3d_of.h

#ifndef vimt3d_image_3d_of_h_
#define vimt3d_image_3d_of_h_

//:
// \file
// \brief Container for vil3d_image_view<T> + transform
// \author Tim Cootes


#include <iostream>
#include <iosfwd>
#include <vimt3d/vimt3d_image_3d.h>
#include <vil3d/vil3d_image_view.h>
#include <vcl_compiler.h>


//: Represent 3D image of type T together with a transform.
//  Each plane is ni() x nj() Ts, with the (x,y) element
//  of the i'th plane accessible using im.plane(i)[x*im.istep() + y*im.jstep()]
template<class T>
class vimt3d_image_3d_of : public vimt3d_image_3d
{
private:

  vil3d_image_view<T> image_;

  //: Shallow equality tester.
  //  The parameter must be identical type to this.
  bool equals(const vimt_image &) const;


public:

  //: Construct an empty one-plane image.
  vimt3d_image_3d_of() {}


  //: Construct an image of size (ni, nj, nk, np) with optional world_to_image transform w2i.
  vimt3d_image_3d_of(unsigned ni, unsigned nj, unsigned nk, unsigned np=1,
                     const vimt3d_transform_3d& w2i=vimt3d_transform_3d())
    : vimt3d_image_3d(w2i), image_(ni, nj, nk, np) {}


  //: Construct from a view and a world-to-image transform.
  // The underlying pixel data is not duplicated.
  vimt3d_image_3d_of(const vil3d_image_view<T>& view,
                     const vimt3d_transform_3d& w2i)
    : vimt3d_image_3d(w2i), image_(view) {}


  //: Destructor
  virtual ~vimt3d_image_3d_of() {}

  //: Base class view of image
  virtual const vil3d_image_view_base& image_base() const { return image_; }

  //: Image view
  vil3d_image_view<T>& image() { return image_; }

  //: Image view
  const vil3d_image_view<T>& image() const { return image_; }

   //: Get the number of planes in the image.
   unsigned n_planes() const {return image_.nplanes();}


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
  virtual std::string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(std::string const& s) const;

    //: Create a copy on the heap and return base class pointer
    //  Note that this will make a shallow copy of any contained images
  virtual vimt_image* clone() const { return new vimt3d_image_3d_of(*this); }

    //: Create a deep copy on the heap and return base class pointer
    //  This will make a deep copy of any contained images
  virtual vimt_image* deep_clone() const;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const;

  //: print all data to os (rounds output to int)
  virtual void print_all(std::ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};


//=======================================================================
//: True if the transforms and the actual image data are identical.
// The image pointers need not be identical,
// provided that the underlying image data are the same.
// \relatesalso vimt3d_image_3d_of<T>
// \relatesalso vil3d_image_view
template<class T>
bool vimt3d_image_3d_deep_equality(const vimt3d_image_3d_of<T>& lhs,
                                   const vimt3d_image_3d_of<T>& rhs);


#endif // vimt3d_image_3d_of_h_
