// This is core/vil/vil_image_view.h
#ifndef vil_image_view_h_
#define vil_image_view_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A base class reference-counting view of some image data.
// \author Ian Scott - Manchester
// \verbatim
//  Modifications
//   Peter Vanroose - 21 Aug.2003 - support added for interleaved planes
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_cstddef.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_memory_chunk.h>
#include <vil/vil_pixel_format.h>


//: Concrete view of image data of type T held in memory
//  Views nplanes() planes of data each of size ni() x nj().
//  The (i,j) element of the p'th plane is given by
//  im.top_left_ptr()[i*im.istep() + j*im.jstep() + p*im.planestep]
//  The actual image data is either allocated by the class
//  (using set_size), in which case it is deleted
//  only when it has no views observing it, or is allocated outside (and is not
//  deleted on destruction).  This allows external images to be accessed
//  without a deep copy.
//
//  Note that copying one vil_image_view<T> to another takes a shallow
//  copy by default - it copies the view, not the raw image data.
//  Use the explicit deep_copy() call to take a deep copy.

template <class T>
class vil_image_view : public vil_image_view_base
{
 private:
  //: Helper types for safe boolean conversion.
  struct safe_bool_dummy { void dummy() {} };
  typedef void (safe_bool_dummy::* safe_bool)();
 protected:
  //: Pointer to pixel at origin.
  T * top_left_;
  //: Add this to a pixel pointer to move one column left.
  vcl_ptrdiff_t istep_;
  //: Add this to a pixel pointer to move one row down.
  vcl_ptrdiff_t jstep_;
  //: Add this to a pixel pointer to move one plane back.
  vcl_ptrdiff_t planestep_;

  //: Reference to actual image data.
  vil_memory_chunk_sptr ptr_;

  //: Disconnect this view from the underlying data,
  void release_memory() { ptr_ = 0; }

 public:

  //: Dflt ctor
  //  Creates an empty one-plane image.
   vil_image_view(): top_left_(0),istep_(0),jstep_(0),planestep_(0) {}

  //: Create an image of ni x nj pixels in (n_planes * n_interleaved_planes) planes
  //  If n_interleaved_planes > 1, the planes are interleaved.
  //  If n_planes > 1, each plane of pixels is stored contiguously.
  //  n_planes and n_components should not be both different from 1.
  //  n_planes * n_interleaved_planes should be 1 unless T is scalar.
  vil_image_view(unsigned ni, unsigned nj, unsigned n_planes=1, unsigned n_interleaved_planes=1);

  //: Set this view to look at someone else's memory data.
  //  If the data goes out of scope then this view could be invalid, and
  //  there's no way of knowing until its too late - so take care!
  vil_image_view(const T* top_left, unsigned ni, unsigned nj, unsigned nplanes,
                 vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step, vcl_ptrdiff_t plane_step);

  //: Set this view to look at another view's data
  //  Typically used by functions which generate a manipulated view of
  //  another's image data.
  //  Need to pass the memory chunk to set up the internal smart ptr appropriately
  vil_image_view(const vil_memory_chunk_sptr& mem_chunk,
                 const T* top_left, unsigned ni, unsigned nj, unsigned nplanes,
                 vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step, vcl_ptrdiff_t plane_step);

  //: Copy construct.
  // The new object will point to the same underlying image as the rhs.
  vil_image_view(const vil_image_view<T>& rhs);

  //: Construct from various vil_image_view types.
  // The new object will point to the same underlying image as the rhs
  // You can assign a vil_image_view<compound_type<T>> to a vil_image_view<T>
  // in all reasonable cases - the lhs will have as many planes as the rhs has
  // components. You can assign a vil_image_view<T> to a vil_image_view<compound_type<T>>
  // when the underlying data is formatted appropriately and the lhs has
  // as many components as the rhs has planes. O(1).
  // If the view types are not compatible this object will be set to empty.
  vil_image_view(const vil_image_view_base& rhs);

  //: Construct from various vil_image_view types.
  // The new object will point to the same underlying image as the rhs.
  //
  // You can assign a vil_image_view<compound_type<T>> to a vil_image_view<T>
  // in all reasonable cases - the lhs will have as many planes as the rhs has
  // components. You can assign a vil_image_view<T> to a vil_image_view<compound_type<T>>
  // when the underlying data is formatted appropriately and the lhs has
  // as many components as the rhs has planes. O(1).
  // If the view types are not compatible this object will be set to empty.
  vil_image_view(const vil_image_view_base_sptr& rhs);

  //  Destructor
  virtual ~vil_image_view() {}

  // Standard container stuff
  // This assumes that the data is arranged contiguously.
  // Is this assumption good?

  //: The pixel type of this image
  typedef T pixel_type;

  //: True if data all in one unbroken block and top_left_ptr() is lowest data address
  bool is_contiguous() const;

  // iterators
  typedef T *iterator;
  inline iterator begin() { assert(is_contiguous()); return top_left_; }
  inline iterator end  () { assert(is_contiguous()); return top_left_ + size(); }

  typedef T const *const_iterator;
  inline const_iterator begin() const { assert(is_contiguous()); return top_left_; }
  inline const_iterator end  () const { assert(is_contiguous()); return top_left_ + size(); }

  // arithmetic indexing stuff

  //: Pointer to the first (top left in plane 0) pixel.
  //  Note that this is not necessarily the lowest data memory address.
  inline T * top_left_ptr() { return top_left_; }  // Make origin explicit
  //: Pointer to the first (top left in plane 0) pixel.
  //  Note that this is not necessarily the lowest data memory address.
  inline const T * top_left_ptr() const { return top_left_; }

  //: Add this to your pixel pointer to get next i pixel.
  //  Note that istep() may well be negative; see e.g. vil_flip_lr
  inline vcl_ptrdiff_t istep() const { return istep_; }
  //: Add this to your pixel pointer to get next j pixel.
  //  Note that jstep() may well be negative; see e.g. vil_flip_ud
  inline vcl_ptrdiff_t jstep() const { return jstep_; }
  //: Add this to your pixel pointer to get pixel on next plane.
  //  Note that planestep() may well be negative, e.g. with BMP file images
  inline vcl_ptrdiff_t planestep() const { return planestep_; }

  //: Cast to bool is true if pointing at some data.
  operator safe_bool () const
    { return (top_left_ != (T*)0)? &safe_bool_dummy::dummy : 0; }

  //: The number of bytes in the data
  inline unsigned size_bytes() const { return size() * sizeof(T); }

  //: Smart pointer to the object holding the data for this view
  // Will be a null pointer if this view looks at `third-party' data,
  // e.g. using set_to_memory.
  //
  // Typically used when creating new views of the data
  inline const vil_memory_chunk_sptr& memory_chunk() const { return ptr_; }

  //: Smart pointer to the object holding the data for this view
  // Will be a null pointer if this view looks at `third-party' data,
  // e.g. using set_to_memory
  //
  // Typically used when creating new views of the data
  inline vil_memory_chunk_sptr& memory_chunk() { return ptr_; }

  // Ordinary image indexing stuff.

  //: Return read-only reference to pixel at (i,j) in plane 0.
  inline const T& operator()(unsigned i, unsigned j) const {
    assert(i<ni_); assert(j<nj_);
    return top_left_[jstep_*j+i*istep_]; }

  //: Return read/write reference to pixel at (i,j) in plane 0.
  inline T&       operator()(unsigned i, unsigned j) {
    assert(i<ni_); assert(j<nj_);
    return top_left_[istep_*i+j*jstep_]; }

  //: Return read-only reference to pixel at (i,j) in plane p.
  inline const T& operator()(unsigned i, unsigned j, unsigned p) const {
    assert(i<ni_); assert(j<nj_); assert(p<nplanes_);
    return top_left_[p*planestep_ + j*jstep_ + i*istep_]; }

  //: Return read-only reference to pixel at (i,j) in plane p.
  inline T&       operator()(unsigned i, unsigned j, unsigned p) {
    assert(i<ni_); assert(j<nj_); assert(p<nplanes_);
    return top_left_[p*planestep_ + j*jstep_ + i*istep_]; }


  // image stuff

  //: resize current planes to ni x nj
  // If already correct size, this function returns quickly
  virtual void set_size(unsigned ni, unsigned nj);

  //: resize to ni x nj x nplanes
  // If already correct size, this function returns quickly
  virtual void set_size(unsigned ni, unsigned nj, unsigned nplanes);

  //: Make a copy of the data in src and set this to view it
  void deep_copy(const vil_image_view<T>& src);

  //: Make empty.
  // Disconnects view from underlying data.
  inline void clear() { release_memory(); ni_=nj_=nplanes_=0; top_left_=0; }

  //: Set this view to look at someone else's memory data.
  //  If the data goes out of scope then this view could be invalid, and
  //  there's no way of knowing until it's too late -- so take care!
  //
  //  Note that though top_left is passed in as const, the data may be manipulated
  //  through the view.
  void set_to_memory(const T* top_left, unsigned ni, unsigned nj, unsigned nplanes,
                     vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step, vcl_ptrdiff_t plane_step);

  //: Fill view with given value
  void fill(T value);

  //: Print a 1-line summary of contents
  virtual void print(vcl_ostream&) const;

  //: Return class name
  virtual vcl_string is_a() const;

  //: True if this is (or is derived from) class s
  virtual bool is_class(vcl_string const& s) const;

  //: Return a description of the concrete data pixel type.
  // The value corresponds directly to pixel_type.
  inline vil_pixel_format pixel_format() const { return vil_pixel_format_of(T()); }

  //: True if they share same view of same image data.
  //  This does not do a deep equality on image data. If the images point
  //  to different image data objects that contain identical images, then
  //  the result will still be false.
  bool operator==(const vil_image_view_base& other) const;

  //: True if they do not share same view of same image data.
  //  This does not do a deep inequality on image data. If the images point
  //  to different image data objects that contain identical images, then
  //  the result will still be true.
  inline bool operator!=(const vil_image_view_base& rhs) const { return !operator==(rhs); }

  //: Provides an ordering.
  //  Useful for ordered containers.
  //  There is no guaranteed meaning to the less than operator, except that
  //  (a<b && b<a)  is false and  !(a<b) && !(b<a)  is equivalent to  a==b
  bool operator<(const vil_image_view_base& rhs) const;

  //: Provides an ordering.
  inline bool operator>=(const vil_image_view_base& rhs) const { return !operator<(rhs); }

  //: Provides an ordering.
  bool operator>(const vil_image_view_base& rhs) const;

  //: Provides an ordering.
  inline bool operator<=(const vil_image_view_base & rhs) const { return !operator>(rhs); }

  //: Copy a view. The rhs and lhs will point to the same image data.
  const vil_image_view<T>& operator=(const vil_image_view<T>& rhs);

  //: Copy a view. The rhs and lhs will point to the same image data.
  // You can assign a vil_image_view<compound_type<T>> to a vil_image_view<T>
  // in all reasonable cases - the lhs will have as many planes as the rhs has
  // components. You can assign a vil_image_view<T> to a vil_image_view<compound_type<T>>
  // when the underlying data is formatted appropriately and the lhs has
  // as many components as the rhs has planes. O(1).
  // If the view types are not compatible this object will be set to empty.
  const vil_image_view<T>& operator=(const vil_image_view_base & rhs);

  //: Copy a view. The rhs and lhs will point to the same image data.
  // You can assign a vil_image_view<compound_type<T>> to a vil_image_view<T>
  // in all reasonable cases - the lhs will have as many planes as the rhs has
  // components. You can assign a vil_image_view<T> to a vil_image_view<compound_type<T>>
  // when the underlying data is formatted appropriately and the lhs has
  // as many components as the rhs has planes. O(1).
  // If the view types are not compatible this object will be set to empty.
  // If the pointer is null, this object will be set to empty.
  inline const vil_image_view<T>& operator=(const vil_image_view_base_sptr& rhs)
  {
    if (!rhs) clear();
    else *this = *rhs;
    return *this;
  }
};

//: Print a 1-line summary of contents
template <class T>
inline
vcl_ostream& operator<<(vcl_ostream& s, vil_image_view<T> const& im)
{
  im.print(s); return s;
}


//: True if the actual images are identical.
// $\bigwedge_{i,j,p} {\textstyle src}(i,j,p) == {\textstyle dest}(i,j,p)$
// The data may be formatted differently in each memory chunk.
//  O(size).
// \relates vil_image_view
template<class T>
bool vil_image_view_deep_equality(const vil_image_view<T> &lhs, const vil_image_view<T> &rhs);

#endif // vil_image_view_h_
