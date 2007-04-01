// This is core/vil1/vil1_memory_image_of.h
#ifndef vil1_memory_image_of_h_
#define vil1_memory_image_of_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author awf@robots.ox.ac.uk
// \date 16 Feb 00
//
//\verbatim
// Modifications
//     960916 AWF Added save_pgm and more comments.
//     961209 Peter Vanroose added operator=() and copy constructor
//     980710 FSM Changed constructor to take a const Image *, not Image *
//     981105 AWF Made bilinear/bicubic return double.
//     990211 Peter Vanroose moved save_pgm() to Templates/ (EGCS complained)
//     990421 FSM Added constructor from a const fast_array<T> &
//     010126 BJM (mccane@cs.otago.ac.nz) added constructor from
//            previously allocated memory. This memory is not deallocated on
//            destruction.
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
//\endverbatim

#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image.h>

//: Image stored entirely in RAM
//
//    vil1_memory_image_of<Type> provides a templated interface to a
//    vil1_memory_image.  It is assumed that the user has queried the pixel size
//    of the image and is instantiating an ImageBuffer of the appropriate type.
//
//    This allows C-efficiency access with C++ notational convenience after the
//    type has been ascertained.  Note that this should not be used for images
//    too large to fit in memory.
//
//  CAVEAT PROGRAMMER:
//    Each raster (row) is stored in a contiguous chunk of memory and the
//    operator [] method gives a pointer to the beginning of a raster.
//    Thus image[i][j] is the element in the i-th row and j-th column.
//    However, image(x, y) is the element in the x-th column and y-th row.

template <class T>
class vil1_memory_image_of : public vil1_memory_image
{
 public:
  // The pixel type of this image
  typedef T pixel_type;

  // iterators
  typedef T *iterator;
  inline iterator begin() { return get_buffer(); }
  inline iterator end  () { return get_buffer() + rows()*cols(); }

  typedef T const *const_iterator;
  inline const_iterator begin() const { return get_buffer(); }
  inline const_iterator end  () const { return get_buffer() + rows()*cols(); }

  inline unsigned size() const { return rows() * cols(); }

  //: Empty image.
  vil1_memory_image_of();

  //: This is a copy constructor, but it doesn't make a new buffer.
  vil1_memory_image_of(vil1_memory_image_of<T> const &);

  //: Copy given image into a memory buffer.
  // If it's already a memory image, do as the copy constructor (above) does.
  explicit
  vil1_memory_image_of(vil1_image const& image);

  //: Construct a w x h image, pixel format is determined from T
  vil1_memory_image_of(int sizex, int sizey);

  //: Construct a w x h image, pixel format is determined from T from memory previously created and pointed to by buf
  vil1_memory_image_of(T *buf, int sizex, int sizey);
#if 0
  //: Make memory imagebuffer, and fill with "value"
  vil1_memory_image_of(int sizex, int sizey, T const& value);
#endif
  //: Clearly, this will deallocate the memory buffer
  inline ~vil1_memory_image_of() {}

  //: This method hides the operator= in the base class.
  vil1_memory_image_of<T>& operator=(vil1_memory_image_of<T> const &);

  //: Copy a vil1_image, only if it's in an appropriate format.
  // This routine does not try to guess how to convert images which are
  // not compatible with T.
  vil1_memory_image_of<T>& operator=(vil1_image const &);

  //: Load image.
  void set(vil1_image const& image);

  //: These override the methods in the base class.
  void resize(int width, int height);
 private:
  // don't try to use this.
  void resize(int planes, int width, int height);
 public:

  // Data Access---------------------------------------------------------------

  //: Return read/write reference to pixel at (x,y)
  inline T&       operator () (int x, int y) { return ((T**)rows0_)[y][x]; }
  inline T const& operator () (int x, int y) const { return ((T const* const*)rows0_)[y][x]; }

  //: Return pointer to raster y.
  inline T*       operator [] (int y) { return ((T**)rows0_)[y]; }
  inline T const* operator [] (int y) const { return ((T const* const*)rows0_)[y]; }

  //: Return pointer to array of rasters. aka known as data_array() for matrices.
  inline T*        const* row_array() { return (T**)rows0_; }
  inline T const*  const* row_array() const { return (T**)rows0_; }

  //: Return pointer to the memory buffer.
  inline T*       get_buffer() { return (T*)rows0_[0]; }
  inline T const* get_buffer() const { return (T*)rows0_[0]; }

  //: Return true if (x,y) is a valid index into this buffer
  inline bool in_range(int x, int y) const { return (0 <= x) && (0 <= y) && (x < width_) && (y < height_); }

  //: Return true if (x+/-w,y+/-h) are valid indices into this buffer
  inline bool in_range_window(int x, int y, int w) const {
    return (w <= x) && (w <= y) && (x + w < width_) && (y + w < height_);
  }

  //: Return true if the region of size w,h starting at x,y is valid in this buffer.
  inline bool in_range(int x, int y, unsigned w, unsigned h) const {
    return (0<=x && x+int(w)<=width_) && (0<=y && y+int(h)<=height_);
  }

  //: Fill with given value
  void fill(T const& );
};

#endif // vil1_memory_image_of_h_
