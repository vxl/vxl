// This is mul/mil/mil_image_2d_of.h
#ifndef mil_image_2d_of_h_
#define mil_image_2d_of_h_
//:
// \file
// \brief Represent images of one or more planes of type T.
// \author Tim Cootes

#include <mil/mil_image_data.h>
#include <mil/mil_image_2d.h>
#include <mil/mil_transform_2d.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_iosfwd.h>

//: Represent images of one or more planes of type T.
//  Each plane is nx() x ny() Ts, with the (x,y) element
//  of the i'th plane accessible using im.plane(i)[x*im.xstep() + y*im.ystep()]
//  The actual image data is either allocated by the class
//  (using resize), in which case it is deleted by the
//  destructor, or is allocated outside (and is not deleted on
//  destruction).  This allows external images to be accessed
//  without a deep copy.
//
//  Note that copying one mil_image_of<T> to another takes a shallow
//  copy by default.  Use the explicit deepCopy() call to take a deep copy.
//
//  format() returns a vcl_string describing the format.
//  Formats currently include:
//  - "GreyByte" : an nx() x ny() greyscale image of Ts plane(0)[x+y*ystep()]
//  - "RGBPlaneByte" : an nx() x ny() colour image with three separate
//                     planes of data,
//                     plane(i)[x*im.xstep() + y*imystep()] i=0,1,2 for r,g,b
//
//  It is safest to examine the number of planes, xstep() and ystep() to
//  determine the format.

template<class T>
class mil_image_2d_of : public mil_image_2d
{
  //: Pointer to image data
  vbl_smart_ptr<mil_image_data<T> > data_;

  vcl_vector<T*> planes_;
  int nx_,ny_;
  int xstep_,ystep_;
  vcl_string format_;
  mil_transform_2d world2im_;

  void release_data();

  //: Resize current planes to [0..nx-1][0..ny-1]
  virtual void resize2(int nx, int ny);

  //: Resize to n_planes of [0..nx-1][0..ny-1]
  virtual void resize3(int nx, int ny, int n_planes);

 public:
  typedef T pixel_type;

  //: Dflt ctor
  //  Creates an empty one-plane image.
  mil_image_2d_of();

  //: Create an n_plane plane image of nx x ny pixels
  mil_image_2d_of(int nx, int ny, int n_planes=1);

  //: Destructor
  virtual ~mil_image_2d_of();

  //: Transformation from world to image co-ordinates
  virtual const mil_transform_2d& world2im() const { return world2im_; }

  //: Set the transformation from world to image co-ordinates
  virtual void setWorld2im(const mil_transform_2d& w2i) { world2im_ = w2i; }

  //: Define number of planes
  //  Each plane will be resized to (0,0)
  //  Default number of planes is 1
  void set_n_planes(int n);

  //: Define parameters.
  //  planes[i] is pointer to i'th plane of nx x ny image data
  //  i should be valid in range [0,n_planes-1]
  //  ystep gives data row length
  //  Copies of pointers recorded (i.e. a shallow copy)
  //  (x,y) point in plane i given by planes[i][x*xstep + y*ystep]
  void set(vcl_vector<T*>& planes, int nx, int ny,
           int xstep, int ystep,
           const char* format);

  //: Define parameters.
  //  planes[i] is pointer to i'th plane of nx x ny image data
  //  i should be valid in range [0,n_planes-1]
  //  Copies of pointers recorded (i.e. a shallow copy)
  void set(T** planes, int n_planes,
           int nx, int ny, int xstep, int ystep,
           const char* format);

  //: Define parameters for grey scale images (single plane).
  //  ystep gives data row length
  void setGrey(T* grey, int nx, int ny,int ystep);

  //: Define parameters for 3 plane (RGB) T images.
  //  Sets up a 3 plane image with plane(0) = r, plane(1) = g etc.
  //  ystep gives data row length.
  void setRGB(T* r, T* g, T* b,
              int nx, int ny, int ystep);

  //: Define parameters for 3 plane (RGB) T images.
  //  Sets up a 3 plane image with plane(0) = r, plane(1) = g etc.
  //  ystep gives data row length.
  //  xstep gives the length we have to jump to find the next color value.
  void setRGB(T* r, T* g, T* b,
              int nx, int ny, int xstep, int ystep);

  //: Define parameters for packed RGB T images.
  //  Sets up a 3 plane image, assuming nx x ny image
  //  of xstep T pixels, i.e. red(x,y) = data[x*xstep+y*step],
  //  green(x,y) = data[1+x*xstep+y*step],
  //  blue(x,y) = data[2+x*xstep+y*step]
  void setRGB(T* data, int nx, int ny, int xstep, int ystep);

  //: Arrange that this is window on given image.
  //  I.e. plane(i) points to im.plane(i) + offset.
  //  The parameters should be in image coordinates.
  //  The world2im transform is set to match
  //  so this appears identical to im when addressed
  //  in world coordinates.
  void setToWindow(const mil_image_2d_of& im,
                   int xlo, int xhi, int ylo, int yhi);

  //: Fills all planes with b
  void fill(T b);

  //: Format of data
  //  Options include "GreyByte", "RGBPlaneByte", "RGBPackedByte"
  const vcl_string& format() const { return format_; }

  //: Set vcl_string defining format
  void setFormat(const char* f);

  //: Width of image (Valid x range: [0,nx()-1]
  virtual int nx() const { return nx_; }

  //: Height of image (Valid y range: [0,ny()-1]
  virtual int ny() const { return ny_; }

  //: Step between im(x,y) and im(x+1y).
  //  im(x,y) given by plane(i)[x*xstep()+y*ystep()]
  int xstep() const { return xstep_; }

  //: Data row length or step between im(x,y) and im(x,y+1).
  //  im(x,y) given by plane(i)[x*xstep()+y*ystep()]
  int ystep() const { return ystep_; }

  //: Number of planes available
  int n_planes() const { return planes_.size(); }

  //: const pointer to i'th plane of data.
  //  plane(i)[x*xstep()+y*ystep()] is value of (x,y) pixel in plane i
  //  Valid range for i: [0,n_planes()-1]
  const T* plane(int i) const { return planes_[i]; }

  //: (non-const) pointer to i'th plane of data.
  //  plane(i)[x*xstep()+y*nx()] is value of (x,y) pixel in plane i
  //  Valid range for i: [0,n_planes()-1]
  T* plane(int i) { return planes_[i]; }

  //: Access to pointer to T planes.
  //  planes()[i] is pointer to i'th image
  T** planes() { return &planes_[0]; }

  //: Access to (x,y) pixel in plane i.
  T& operator()(int x, int y, int i)
  {
    assert(x>=0); assert(x<nx_); assert(y>=0); assert(y<ny_);
    assert(i>=0); assert((unsigned int)i<planes_.size());
    return planes_[i][ystep_*y+x*xstep_];
  }

  //: Access to (x,y) pixel in plane i.
  const T& operator()(int x, int y, int i) const
  {
    assert(x>=0); assert(x<nx_); assert(y>=0); assert(y<ny_);
    return planes_[i][ystep_*y+x*xstep_];
  }

  //: Access to (x,y) pixel in plane 0.
  T& operator()(int x, int y)
  {
    assert(x>=0); assert(x<nx_); assert(y>=0); assert(y<ny_);
    return planes_[0][ystep_*y+x*xstep_];
  }

  //: Access to (x,y) pixel in plane 0.
  const T& operator()(int x, int y) const
  {
    assert(x>=0); assert(x<nx_); assert(y>=0); assert(y<ny_);
    return planes_[0][ystep_*y+x*xstep_];
  }

  //: True if transforms are equal, and they share same image data.
  //  This does not do a deep equality on image data. If the images point
  //  to different image data objects that contain identical images, then
  //  the result will still be false.
  bool operator==(const mil_image_2d_of<T> &) const;

  //: Define valid data region (including transform).
  //  Resizes and sets the transformation so that
  //  worldToIm(x,y) is valid for all points in range
  //  Specifically, resize(1+xhi-xlo,1+yhi-ylo);
  //  worldToIm() translates by (-xlo,-ylo)
  void setValidRegion(int xlo, int xhi, int ylo, int yhi);

  //: Take a deep copy of image (copy data, not just pointers)
  void deepCopy(const mil_image_2d_of& image);

  //: Get range of values in plane p
  void getRange(T& min_v, T& max_v, int p) const;

  //: Get range of values over all planes
  void getRange(T& min_v, T& max_v) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual mil_image* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: print all data to os (rounds output to int)
  virtual void print_all(vcl_ostream& os) const;

  //: print all messily to data to os (doesn't round to int)
  virtual void print_messy_all(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#define MIL_IMAGE_2D_OF_INSTANTIATE(T) \
extern "please #include mil/mil_image_2d_of.txx instead"

#endif // mil_image_2d_of_h_
