//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_memory_image_of_h_
#define vil_memory_image_of_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: awf@robots.ox.ac.uk
// Created: 16 Feb 00

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

template <class T>
class vil_memory_image_of : public vil_memory_image {
public:
  // -- The pixel type of this image
  typedef T pixel_type;


//: Copy given image into a memory buffer.
  vil_memory_image_of(vil_image const& image);

//: Construct a w x h image, pixel format is determined from T
  vil_memory_image_of(int sizex, int sizey);

//: Make memory imagebuffer, and fill with "value"
  vil_memory_image_of(int sizex, int sizey, const T& value);

//: Clearly, this will deallocate the memory buffer
  ~vil_memory_image_of() {}

//: Load image.
  void set(vil_image const& image);
  
  // Data Access---------------------------------------------------------------

  //: Return read/write reference to pixel at (x,y)
  T&           operator () (int x, int y) { return ((T**)rows0_)[y][x]; }
  T const&     operator () (int x, int y) const { return ((T const* const*)rows0_)[y][x]; }
  
  //: Return pointer to raster y. Caveat lector : image(i,j) is image[j][i].
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

//: Fill with all-bits-zero
  // void clear() { _buffer->Clear(); }

public:
  vil_memory_image_of(const vil_memory_image_of& that);
  vil_memory_image_of& operator=(const vil_memory_image_of& that);
private:
  // deprecated -- This was used to copy the ROI, which is no longer on image
  vil_memory_image_of(vil_image const&, bool) {}
};

//: This is a templated declaration of save_jpeg.  No body is provided.
template <class T>
void save_jpeg(vil_memory_image_of<T>& image, const char *filename);

// short forms. keep consistent with vil_fwd.h
typedef vil_memory_image_of<bool>     vil_bool_buffer;
typedef vil_memory_image_of<vil_byte> vil_byte_buffer;
typedef vil_memory_image_of<int>      vil_int_buffer;
typedef vil_memory_image_of<float>    vil_float_buffer;

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_memory_image_of.
