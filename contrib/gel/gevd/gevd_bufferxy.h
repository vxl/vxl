// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>

#ifndef _gevd_bufferxy_h
#define _gevd_bufferxy_h

//=========================== Basics ====================================
//
// .NAME    gevd_bufferxy
// .HEADER  gel Package
// .LIBRARY gevd
// .INCLUDE gevd/gevd_bufferxy.h
// .FILE    gevd_bufferxy.C
//
// .SECTION Description
//      This object is a block of memory which may be accessed by an
// (x,y) coordinate.  Addressing is performed using a pair of arrays
// which store pointers to the x columns and y rows.
//
// Written by:       Brian DeCleene
// Date:             Nov. 16, 1990.
//
// Modified by:      Peter Vanroose
// Date:             Nov. 4 1996.
//                   Copy constructor added: fixes bug (xra & yra were not copied)
//
// Modified by:      Peter Vanroose
// Date:             June 30 1997.
//                   Constructor added that allows to supply allocated memory.
//
// Modified by:      Peter Vanroose
// Date:             Aug 17 1999.
//                   File dump I/O added.
//
//=======================================================================

#include "gevd_memory_mixin.h"
#include <vcl_iostream.h>
#include <vil/vil_image.h>

class gevd_bufferxy : public gevd_memory_mixin
{
    private:

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
      gevd_bufferxy(vil_image &img);
      ~gevd_bufferxy();
      gevd_bufferxy(gevd_bufferxy const&);

   // ACCESS ROUTINES FOR DIMENSIONS
      inline int GetBitsPixel()       const { return(B); }
      inline int GetBytesPixel()      const { return((B+7)/8); }
      inline int GetSizeX()           const { return(X); }
      inline int GetSizeY()           const { return(Y); }
      inline int GetArea()            const { return(GetSizeX()*GetSizeY()); }
      inline int GetSize()            const { return(GetArea()*GetBytesPixel()); }

    // ELEMENT ADDRESSING METHOD

      inline void* GetBuffer()        { return(GetBufferPtr()); }
      inline const void* GetBuffer() const { return(GetBufferPtr()); }
      inline void* GetElementAddr(int x, int y)
          { return(xra[x] + yra[y]); }
      inline const void* GetElementAddr(int x, int y) const
          { return(xra[x] + yra[y]); }

    // FILE I/O

      void dump(const char* filename); // write to file
      gevd_bufferxy(const char* filename);  // read from file

      friend vcl_ostream& operator<<(vcl_ostream& os, gevd_bufferxy const& b) {
        return os << "gevd_bufferxy(width=" << b.GetSizeX() << ",height="
               << b.GetSizeY() << ",bits_per_pixel=" << b.GetBitsPixel() << ")";
      }
};

#endif // _gevd_bufferxy_h
