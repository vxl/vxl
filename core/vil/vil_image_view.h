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

#include <vcl_iosfwd.h>
#include <vil2/vil2_image_view_base.h>

//: An abstract base class of smart pointers to actual image data in memory.

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
  // Standard container stuff
  // This assumes that the data is arranged contiguously.
  // Is this assumption good?

  //: The pixel type of this image
  typedef T pixel_type;

  // iterators
  typedef T *iterator;
  inline iterator begin() { return get_buffer(); }
  inline iterator end  () { return get_buffer() + size(); }

  typedef T const *const_iterator;
  inline const_iterator begin() const { return get_buffer(); }
  inline const_iterator end  () const { return get_buffer() + size(); }

  // aritmetic indexing stuff

  //: Pointer to the first (top left in plane 0) pixel;
  T * top_left_ptr() { return top_left_; }  // Make origin explicit 
  //: Pointer to the first (top left in plane 0) pixel;
  const T * top_left_ptr const { return top_left_; }
  //: Add this to your pixel pointer to get 
  int xstep {return xstep_;}
  int ystep {return ystep_;}
  int planestep {return planestep_;}

  //: The number of pixels.
  inline unsigned size() const { return height() * width() * nplanes(); }

  //: The number of bytes in the data
  inline unsigned size_bytes() const { return size() * sizeof(T); }

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
  
  //: Make a compy of the underlying data.
  void deep_copy(const vil2_image_view& src)

  //: Disconnect this view from the underlying data.
  void release_data();

  //: Set this view to look at someone else's memory data.
  void set_to_memory(T* top_left, unsigned nx, unsigned ny, unsigned nplanes,
              unsigned xstep, unsigned ystep, unsigned planestep);

  //: Arrange that this is window on given image.
  void set_to_window(const vil2_image_view& im,
                     unsigned x0, unsigned nx, unsigned y0,
                     unsigned ny, unsigned p0=0, unsigned np=1)


  //: Print a 1-line summary of contents
  vcl_ostream& print(vcl_ostream&) const;

};

//: Print a 1-line summary of contents
inline
vcl_ostream& operator<<(vcl_ostream& s, vil2_image_view const& i) {
  return i.print(s);
}

#endif // vil_image_view_h_
