// This is mul/mil3d/mil3d_image_3d_of.h
#ifndef mil3d_image_3d_of_h_
#define mil3d_image_3d_of_h_
//:
// \file
// \brief Represent 3D images of one or more planes of type T.
// \author Graham Vincent (following design of mil_image_2d_of by Tim Cootes)

#include <mil/mil_image_2d_of.h>
#include <mil/mil_image_data.h>
#include <mil3d/mil3d_image_3d.h>
#include <mil3d/mil3d_transform_3d.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_iosfwd.h>

//: Represent images of one or more planes of type T.
//  Each plane is nx() x ny() x nz() Ts, with the (x,y) element
//  of the i'th plane accessible using im.plane(i)[x*im.xstep() + y*im.ystep() +z*im.zstep()]
//  The actual image data is either allocated by the class
//  (using resize), in which case it is deleted by the
//  destructor, or is allocated outside (and is not deleted on
//  destruction).  This allows external images to be accessed
//  without a deep copy.
//
//  Note that copying one mil3d_image_of<T> to another takes a shallow
//  copy by default.  Use the explicit deepCopy() call to take a deep copy.
//
//  format() returns a vcl_string describing the format.
//  Formats currently include:
//  - "GreyByte" : an nx() x ny() x nz() greyscale image of Ts plane(0)[x+y*ystep()+z*zstep()]
//  - "RGBPlaneByte" : an nx() x ny() x nz()  colour image with three separate
//                     planes of data,
//                     plane(i)[x*im.xstep() + y*imystep() + z*im.zstep()] i=0,1,2 for r,g,b
//
//  It is safest to examine the number of planes, xstep(), ystep() and zstep() to
//  determine the format.

template<class T>
class mil3d_image_3d_of : public mil3d_image_3d
{
  //: Pointer to image data
  vbl_smart_ptr<mil_image_data<T> > data_;

  vcl_vector<T*> planes_;
  int nx_,ny_,nz_;
  int xstep_,ystep_, zstep_;
  vcl_string format_;
  mil3d_transform_3d world2im_;

  void release_data();

  //: Resize current planes to [0..nx-1][0..ny-1][0..nz-1]
  virtual void resize2(int nx, int ny, int nz);

  //: Resize to n_planes of [0..nx-1][0..ny-1][0..nz-1]
  virtual void resize3(int nx, int ny, int nz, int n_planes);

 public:

  //: Dflt ctor
  //  Creates an empty one-plane image.
  mil3d_image_3d_of();

  //: Create an n_plane plane image of nx x ny x nz pixels
  mil3d_image_3d_of(int nx, int ny, int nz, int n_planes=1);

  //: Destructor
  virtual ~mil3d_image_3d_of();

  //: Transformation from world to image co-ordinates
  virtual const mil3d_transform_3d& world2im() const { return world2im_; }

  //: Set the transformation from world to image co-ordinates
  virtual void setWorld2im(const mil3d_transform_3d& w2i) { world2im_ = w2i; }

  //: Define number of planes
  //  Each plane will be resized to (0,0)
  //  Default number of planes is 1
  void set_n_planes(int n);

  //: Define parameters.
  //  planes[i] is pointer to i'th plane of nx x ny x nz image data
  //  i should be valid in range [0,n_planes-1]
  //  xstep is step in memory between im(x,y,z) and im(x+1,y,z)
  //  ystep is step in memory between im(x,y,z) and im(x,y+1,z)
  //  zstep is step in memory between im(x,y,z) and im(x,y,z+1)
  //  Copies of pointers recorded (i.e. a shallow copy)
  //  (x,y,z) point in plane i given by planes[i][x*xstep + y*ystep+ z*zstep]
  void set(vcl_vector<T*>& planes, int nx, int ny, int nz,
           int xstep, int ystep, int zstep,
           const char* format);

  //: Arrange that this is window on given image.
  //  I.e. plane(i) points to im.plane(i) + offset
  //  The world2im transform is set to match
  //  so this appears identical to im when addressed
  //  in world coordinates.
  void setToWindow(const mil3d_image_3d_of& im,
                   int xlo, int xhi, int ylo, int yhi, int zlo, int zhi);

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

  //: Depth of image (Valid y range: [0,nz()-1]
  virtual int nz() const { return nz_; }

  //: Step between im(x,y,z) and im(x+1,y,z).
  //  im(x,y,z) given by plane(i)[x*xstep()+y*ystep()+z*zstep()]
  int xstep() const { return xstep_; }

  //: Data square length or step between im(x,y,z) and im(x,y+1,z).
  //  im(x,y,z) given by plane(i)[x*xstep()+y*ystep()+z*zstep()]
  int ystep() const { return ystep_; }

  //: Data row length or step between im(x,y,z) and im(x,y,z+1).
  //  im(x,y,z) given by plane(i)[x*xstep()+y*ystep()+z*zstep()]
  int zstep() const { return zstep_; }

  //: Number of planes available
  int n_planes() const { return planes_.size(); }

  //: const pointer to i'th plane of data.
  //  plane(i)[x*xstep()+y*ystep()+z*zstep()] is value of (x,y,z) pixel in plane i
  //  Valid range for i: [0,n_planes()-1]
  const T* plane(int i) const { return planes_[i]; }

  //: (non-const) pointer to i'th plane of data.
  //  plane(i)[x*xstep()+y*ystep()+z*zstep()] is value of (x,y,z) pixel in plane i
  //  Valid range for i: [0,n_planes()-1]
  T* plane(int i) { return planes_[i]; }

  //: Access to pointer to T planes.
  //  planes()[i] is pointer to i'th image
  T** planes() { return &planes_[0]; }

  //: Access to (x,y,z) pixel in plane i.
  T& operator()(int x, int y, int z, int i)
  {
    assert(x>=0); assert(x<nx_);
    assert(y>=0); assert(y<ny_);
    assert(z>=0); assert(z<nz_);
    assert(i>=0); assert((unsigned int)i<planes_.size());
    return planes_[i][ystep_*y+x*xstep_+z*zstep_];
  }

  //: Access to (x,y,z) pixel in plane i.
  const T& operator()(int x, int y, int z, int i) const
  {
    assert(x>=0); assert(x<nx_);
    assert(y>=0); assert(y<ny_);
    assert(z>=0); assert(z<nz_);
    return planes_[i][ystep_*y+x*xstep_+z*zstep_];
  }

  //: Access to (x,y,z) pixel in plane 0.
  T& operator()(int x, int y, int z)
  {
    assert(x>=0); assert(x<nx_);
    assert(y>=0); assert(y<ny_);
    assert(z>=0); assert(z<nz_);
    return planes_[0][ystep_*y+x*xstep_+z*zstep_];
  }

  //: Access to (x,y,z) pixel in plane 0.
  const T& operator()(int x, int y, int z) const
  {
    assert(x>=0); assert(x<nx_);
    assert(y>=0); assert(y<ny_);
    assert(z>=0); assert(z<nz_);
    return planes_[0][ystep_*y+x*xstep_+z*zstep_];
  }

  //: Creates copy of a 2d slice from the 3d image.
  // The appropriate 2d transform is set up.
  // \param axis  axis through which slice is taken
  // \param slice_number slice number on axis
  // \param image_slice 2d image where the output slice is written into
  // \return true if slice_number is valid and slice is taken and
  // image2d is ZoomOnly or Identity, false otherwise
  bool deepSlice(Axis axis,int slice_number, mil_image_2d_of<T> &image_slice) const;

  //: True if transforms are equal, and they share same image data.
  //  This does not do a deep equality on image data. If the images point
  //  to different image data objects that contain identical images, then
  //  the result will still be false.
  bool operator==(const mil3d_image_3d_of<T> &) const;

  //: Define valid data region (including transform).
  //  Resizes and sets the transformation so that
  //  worldToIm(x,y,z) is valid for all points in range
  //  Specifically, resize(1+xhi-xlo,1+yhi-ylo,1+zhi-zlo);
  //  worldToIm() translates by (-xlo,-ylo,-zlo)
  void setValidRegion(int xlo, int xhi, int ylo, int yhi,int zlo, int zhi);

  //: Take a deep copy of image (copy data, not just pointers)
  void deepCopy(const mil3d_image_3d_of& image);

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

  //: print all data to os
  virtual void print_all(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#define MIL3D_IMAGE_3D_OF_INSTANTIATE(T) \
extern "please #include mil3d/mil3d_image_3d_of.txx instead"

#endif // mil3d_image_3d_of_h_
