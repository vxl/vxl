//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_memory_image_of_h_
#define vil_memory_image_of_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: awf@robots.ox.ac.uk
// Created: 16 Feb 00

#include <vcl/vcl_iostream.h>  
#include <vcl/vcl_iterator.h>

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
  vil_memory_image_of(vil_generic_image const* image);

//: Construct a w x h image, pixel format is determined from T
  vil_memory_image_of(int sizex, int sizey);

//: Make memory imagebuffer, and fill with "value"
  vil_memory_image_of(int sizex, int sizey, const T& value);

//: Clearly, this will deallocate the memory buffer
  ~vil_memory_image_of() {}

//: Load image.
  void set(vil_generic_image const* image);
  
  // Data Access---------------------------------------------------------------

  //: Return read/write reference to pixel at (x,y)
  T&           operator () (int x, int y) { return ((T***)rows_)[0][y][x]; }
  T const&     operator () (int x, int y) const { return ((T const* const* const*)rows_)[0][y][x]; }

  //: Return pointer to raster y. Caveat lector : image(i,j) is image[j][i].
  T*           operator [] (int y) { return ((T***)rows_)[0][y]; }
  T const*     operator [] (int y) const { return ((T const* const* const*)rows_)[0][y]; }

//: Return true if (x,y) is a valid index into this buffer
  bool in_range(int x, int y) const { return (0 <= x) && (0 <= y) && (x < width_) && (y < height_); }
  
//: Return true if (x,y) is a valid index into this buffer
  bool in_range_window(int x, int y, int w) const {
    return (w <= x) && (w <= y) && (x + w < width_) && (y + w < height_);
  }

//: Fill with all-bits-zero
  // void clear() { _buffer->Clear(); }

//: Save as PNM file if possible to ostream
  //bool save_pnm(ostream&); 

//: Save as PNM file if possible to filename.  Compresses if filename ends in "gz"
  // TODO
  //bool save_pnm(char const* filename); 

  // constructors
public:
  vil_memory_image_of(const vil_memory_image_of& that);
  vil_memory_image_of& operator=(const vil_memory_image_of& that);
};

//: This is a templated declaration of save_jpeg.  No body is provided.
template <class T>
void save_jpeg(vil_memory_image_of<T>& image, const char *filename);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_memory_image_of.
