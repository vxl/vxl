// This is mul/vil2/vil2_image_view.h
#ifndef vil2_image_view_h_
#define vil2_image_view_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A base class reference-counting view of some image data.
// \author Ian Scott - Manchester

#include <vil2/vil2_image_view_base.h>
#include <vil2/vil2_smart_ptr.h>
#include <vcl_iosfwd.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vil2/vil2_memory_chunk.h>
#include <vil/vil_byte.h>
#include <vil/vil_rgb.h>

//: Concrete view of image data held in memory
template <class T>
class vil2_image_view : public vil2_image_view_base
{
protected:
  T * top_left_;
  int xstep_;
  int ystep_;
  int planestep_;

  vil2_smart_ptr<vil2_memory_chunk> ptr_;
public:

    //: Dflt ctor
    //  Creates an empty one plane image.
  vil2_image_view();

    //: Create a n_plane plane image of nx x ny pixels
  vil2_image_view(unsigned nx, unsigned ny, unsigned n_planes=1);

    //: Set this view to look at someone else's memory data.
    //  If the data goes out of scope then this view could be invalid, and
    //  there's no way of knowing until its too late - so take care!
  vil2_image_view(const T* top_left, unsigned nx, unsigned ny, unsigned nplanes,
                  int xstep, int ystep, int planestep);

    //: Set this view to look at another view's data
    //  Typically used by functions which generate a manipulated view of
    //  another's image data.
    //  Need to pass the memory chunk to set up the internal smart ptr appropriately
  vil2_image_view(const vil2_smart_ptr<vil2_memory_chunk>& mem_chunk,
                  const T* top_left, unsigned nx, unsigned ny, unsigned nplanes,
                  int xstep, int ystep, int planestep);

    //: Copy constructor
    // If this view cannot set itself to view the other data (e.g. because the
    // types are incompatible) it will set itself to empty.
  vil2_image_view(const vil2_image_view_base& );

    //  Destructor
  virtual ~vil2_image_view();

  // Standard container stuff
  // This assumes that the data is arranged contiguously.
  // Is this assumption good?

  //: The pixel type of this image
  typedef T pixel_type;

  //: True if data all in one unbroken block
  bool is_contiguous() const;

  // iterators
  typedef T *iterator;
  inline iterator begin() { assert(is_contiguous()); return top_left_; }
  inline iterator end  () { assert(is_contiguous()); return top_left_ + size(); }

  typedef T const *const_iterator;
  inline const_iterator begin() const { assert(is_contiguous()); return top_left_; }
  inline const_iterator end  () const { assert(is_contiguous()); return top_left_ + size(); }

  // aritmetic indexing stuff

  //: Pointer to the first (top left in plane 0) pixel;
  T * top_left_ptr() { return top_left_; }  // Make origin explicit
  //: Pointer to the first (top left in plane 0) pixel;
  const T * top_left_ptr() const { return top_left_; }

  //: Add this to your pixel pointer to get next x pixel
  int xstep() const { return xstep_; }
  //: Add this to your pixel pointer to get next y pixel
  int ystep() const { return ystep_; }
  //: Add this to your pixel pointer to get pixel on next plane
  int planestep() const { return planestep_; }

  //: Cast to bool is true if pointing at some data.
  operator bool () const { return top_left_ != (T*)0; }


  //: The number of pixels.
  inline unsigned size() const { return nx() * ny() * nplanes(); }

  //: The number of bytes in the data
  inline unsigned size_bytes() const { return size() * sizeof(T); }

    //: Smart pointer to the object holding the data for this view
    // Will be a null pointer if this view looks at `third-party' data,
    // eg using set_to_memory.
    //
    // Typically used when creating new views of the data
  const vil2_smart_ptr<vil2_memory_chunk>& memory_chunk() const { return ptr_; }

    //: Smart pointer to the object holding the data for this view
    // Will be a null pointer if this view looks at `third-party' data,
    // eg using set_to_memory
    //
    // Typically used when creating new views of the data
  vil2_smart_ptr<vil2_memory_chunk>& memory_chunk() { return ptr_; }

  // Ordinary image indexing stuff.

  //: Return read-only reference to pixel at (x,y) in plane 0.
  const T& operator()(unsigned x, unsigned y) const {
    assert(x<nx_); assert(y<ny_);
    return top_left_[ystep_*y+x*xstep_]; }

  //: Return read/write reference to pixel at (x,y) in plane 0.
  T&       operator()(unsigned x, unsigned y) {
    assert(x<nx_); assert(y<ny_);
    return top_left_[ystep_*y+x*xstep_]; }

  //: Return read-only reference to pixel at (x,y) in plane p.
  const T& operator()(unsigned x, unsigned y, unsigned p) const {
  assert(x<nx_); assert(y<ny_);
   return top_left_[planestep_*p + ystep_*y + x*xstep_]; }

  //: Return read-only reference to pixel at (x,y) in plane p.
  T&       operator()(unsigned x, unsigned y, unsigned p) {
  assert(x<nx_); assert(y<ny_);
   return top_left_[planestep_*p + ystep_*y + x*xstep_]; }


// image stuff

  //: resize current planes to width x height
  // If already correct size, this function returns quickly
  void resize(unsigned width, unsigned height);

  //: resize to width x height x nplanes
  // If already correct size, this function returns quickly
  void resize(unsigned width, unsigned height, unsigned nplanes);

  //: Make a copy of the data in src and set this to view it
  void deep_copy(const vil2_image_view<T>& src);

  //: Create a copy of the data viewed by this, and return a view of copy.
  vil2_image_view<T> deep_copy() const;

  //: Disconnect this view from the underlying data.
  void release_data();

  //: Set this view to look at someone else's memory data.
  //  If the data goes out of scope then this view could be invalid, and
  //  there's no way of knowing until its too late - so take care!
  //
  //  Note that though top_left is passed in as const, the data may be manipulated
  //  through the view.
  void set_to_memory(const T* top_left, unsigned nx, unsigned ny, unsigned nplanes,
              int xstep, int ystep, int planestep);

  //: Arrange that this is window on some planes of given image.
  //  i.e. plane(i) points to im.plane(i+p0) + offset
  void set_to_window(const vil2_image_view& im,
                     unsigned x0, unsigned nx, unsigned y0,
                     unsigned ny, unsigned p0, unsigned np);

  //: Arrange that this is window on all planes of given image.
  void set_to_window(const vil2_image_view& im,
                     unsigned x0, unsigned nx, unsigned y0, unsigned ny);

  //: Return an nx x ny window of this data with offset (x0,y0)
  vil2_image_view<T> window(unsigned x0, unsigned nx, unsigned y0, unsigned ny) const;

  //: Return a view of plane p
  vil2_image_view<T> plane(unsigned p) const;

  //: Fill view with given value
  void fill(T value);

  //: Print a 1-line summary of contents
  virtual void print(vcl_ostream&) const;

    //: print all image data to os in a grid (rounds output to int)
  virtual void print_all(vcl_ostream& os) const;

    //: Return class name
  virtual vcl_string is_a() const;

    //: True if this is (or is derived from) class s
  virtual bool is_class(vcl_string const& s) const;

    //: True if they share same view of same image data.
    //  This does not do a deep equality on image data. If the images point
    //  to different image data objects that contain identical images, then
    //  the result will still be false.
  bool operator==(const vil2_image_view<T> &other) const;

  const vil2_image_view_base & operator = (const vil2_image_view_base & rhs);
};

//: Print a 1-line summary of contents
template <class T>
inline
vcl_ostream& operator<<(vcl_ostream& s, vil2_image_view<T> const& i) {
  return i.print(s);
}

#endif // vil_image_view_h_
