//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_memory_image_of_h_
#define vil_memory_image_of_h_
#ifdef __GNUC__
#pragma interface
#endif
// .SECTION Author
//    awf@robots.ox.ac.uk
// Created: 16 Feb 00
//
// .SECTION Modifications
//     960916 AWF Added save_pgm and more comments.
//     961209 Peter Vanroose added operator=() and copy constructor
//     980710 FSM Changed constructor to take a const Image *, not Image *
//     981105 AWF Made bilinear/bicubic return double.
//     990211 Peter Vanroose moved save_pgm() to Templates/ (EGCS complained)
//     990421 FSM Added constructor from a const fast_array<T> &

#include <vil/vil_byte.h>
#include <vil/vil_image.h>
#include <vil/vil_memory_image.h>

//: Image stored entirely in RAM
//
//    vil_memory_image_of<Type> provides a templated interface to a
//    vil_memory_image.  It is assumed that the user has queried the pixel size
//    of the image and is instantiating an ImageBuffer of the appropriate type.
//    
//    This allows C-efficiency access with C++ notational convenience after the
//    type has been ascertained.  Note that this should not be used for images
//    too large to fit in memory.
//
//  CAVEAT PROGRAMMER:
//    Each raster (row) is stored in a contiguous chunk of memory and the
//    operator [] method gives a pointer to the beginning of a raster.
//    Thus image[i][j] is the element in the ith row and jth column.
//    However, image(x, y) is the element in the xth column and yth row.

template <class T>
class vil_memory_image_of : public vil_memory_image {
public:
  // -- The pixel type of this image
  typedef T pixel_type;

  //: Empty image.
  vil_memory_image_of();

  //: 
  vil_memory_image_of(vil_memory_image_of const &);

  //: Copy given image into a memory buffer.
  // If it's already a memory image, do as the 
  // copy constructor (above) does.
  vil_memory_image_of(vil_image const& image);

  // Deprecated -- This was used to copy the ROI, which is no longer on image
  //vil_memory_image_of(vil_image const&, bool) {}

  //: Construct a w x h image, pixel format is determined from T
  vil_memory_image_of(int sizex, int sizey);
  
  //  //: Make memory imagebuffer, and fill with "value"
  //  vil_memory_image_of(int sizex, int sizey, T const& value);
  
  //: Clearly, this will deallocate the memory buffer
  ~vil_memory_image_of() {}
  
  //: This method hides the operator= in the base class.
  vil_memory_image_of& operator=(vil_memory_image_of const &);
  
  //: Load image.
  void set(vil_image const& image);

  //: These override the methods in the base class.
  void resize(int width, int height);
private:
  // don't try to use this.
  void resize(int planes, int width, int height);
public:
  
  // Data Access---------------------------------------------------------------

  //: Return read/write reference to pixel at (x,y)
  T&           operator () (int x, int y) { return ((T**)rows0_)[y][x]; }
  T const&     operator () (int x, int y) const { return ((T const* const*)rows0_)[y][x]; }
  
  //: Return pointer to raster y.
  T*           operator [] (int y) { return ((T**)rows0_)[y]; }
  T const*     operator [] (int y) const { return ((T const* const*)rows0_)[y]; }

  //: Return pointer to array of rasters. aka known as data_array() for matrices.
  T*        const* row_array() { return (T**)rows0_; }
  T const*  const* row_array() const { return (T**)rows0_; }

  //: Return pointer to the memory buffer.
  T*       get_buffer() { return (T*)rows0_[0]; }
  T const* get_buffer() const { return (T*)rows0_[0]; }
 
  //: Return true if (x,y) is a valid index into this buffer
  bool in_range(int x, int y) const { return (0 <= x) && (0 <= y) && (x < width_) && (y < height_); }
  
  //: Return true if (x+/-w,y+/-h) are valid indices into this buffer
  bool in_range_window(int x, int y, int w) const {
    return (w <= x) && (w <= y) && (x + w < width_) && (y + w < height_);
  }

  //: Return true if the region of size w,h starting at x,y is valid in this buffer.
  bool in_range(int x, int y, unsigned w, unsigned h) const {
    return (0<=x && x+int(w)<=width_) && (0<=y && y+int(h)<=height_);
  }

  //: Fill with given value
  void fill(T const& );
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_memory_image_of.
