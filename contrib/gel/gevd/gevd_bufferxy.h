#ifndef gevd_bufferxy_h_
#define gevd_bufferxy_h_
//:
// \file
//
//=======================================================================
// This object is a block of memory which may be accessed by an
// (x,y) coordinate.  Addressing is performed using a pair of arrays
// which store pointers to the x columns and y rows.
//
// \author   Brian DeCleene
// \date     Nov. 16, 1990.
//
// \verbatim
//  Modifications:
//   Peter Vanroose - Nov. 4 1996.
//     - Copy constructor added: fixes bug (xra & yra were not copied)
//   Peter Vanroose - June 30 1997.
//     - Constructor added that allows to supply allocated memory.
//   Peter Vanroose - Aug 17 1999.
//     - File dump I/O added.
//   J.L. Mundy - Dec 27 2004.
//     - added support for vil
// \endverbatim
//=======================================================================

#include "gevd_memory_mixin.h"
#include <vcl_iostream.h>
#include <vil1/vil1_image.h>
#include <vil/vil_image_resource.h>

class gevd_bufferxy : public gevd_memory_mixin
{
  // Parameters which define the size of the buffer.
  int B;    // Bits per pixel
  int X;    // X dimension in pixels.
  int Y;    // Y dimension in pixels.

  // Array for accessing the memory.
  unsigned char** yra;
  unsigned int*   xra;

 protected:
  void Init(int X, int Y, int B);
  gevd_bufferxy(); // the default constructor should not be used.

  // SET ROUTINES FOR BUFFER SIZE
  inline void SetBitsPixel(int d)         { B = d; }
  inline void SetBytesPixel(int d)        { B = 8*d; }
  inline void SetSizeX(int x)             { X = x; }
  inline void SetSizeY(int y)             { Y = y; }

 public:

  gevd_bufferxy(int X, int Y, int B);
  gevd_bufferxy(int X, int Y, int B, void* memptr);
  gevd_bufferxy(vil1_image const &img);
  gevd_bufferxy(vil_image_resource_sptr const& img);
  ~gevd_bufferxy();
  gevd_bufferxy(gevd_bufferxy const&);

  // ACCESS ROUTINES FOR DIMENSIONS
  inline int GetBitsPixel()       const { return B; }
  inline int GetBytesPixel()      const { return (B+7)/8; }
  inline int GetSizeX()           const { return X; }
  inline int GetSizeY()           const { return Y; }
  inline int GetArea()            const { return GetSizeX()*GetSizeY(); }
  inline int GetSize()            const { return GetArea()*GetBytesPixel(); }

  // ELEMENT ADDRESSING METHOD
  inline void* GetBuffer()             { return GetBufferPtr(); }
  inline const void* GetBuffer() const { return GetBufferPtr(); }
  inline void* GetElementAddr(int x, int y)          { return xra[x]+yra[y]; }
  inline const void* GetElementAddr(int x,int y)const{ return xra[x]+yra[y]; }

  // FILE I/O
  void dump(const char* filename); // write to file
  gevd_bufferxy(const char* filename);  // read from file

  friend vcl_ostream& operator<<(vcl_ostream& os, gevd_bufferxy const& b) {
    return os << "gevd_bufferxy(width=" << b.GetSizeX() << ",height="
              << b.GetSizeY() << ",bits_per_pixel=" << b.GetBitsPixel() << ")";
  }
};

#endif // gevd_bufferxy_h_
