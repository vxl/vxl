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

#ifndef _gevd_memory_mixin_h
#define _gevd_memory_mixin_h

//=========================== Basics ====================================
//
// .NAME    gevd_memory_mixin - Captures all functional details required to access a real computer memory
// .HEADER  GeneralUtility Package
// .LIBRARY Basics
// .INCLUDE Basics/gevd_memory_mixin.h
// .FILE    gevd_memory_mixin.h
// .FILE    gevd_memory_mixin.C
//
// .SECTION Author:
// Written by:       Brian DeCleene
// Date:             Nov. 16, 1990.
//
// .SECTION Modifications:
// Modified by:      Burt Smith
// Date:             June 11, 1991 - Added status and handling of touched
//                                      vs. untouched space.
//                   July  8, 1991 - Incorporated StatusMixin.
//                   July 17, 1991 - Changed ReadBytes(..., int loc) to
//                                   seek from offset, not from start.
// Modified by:      Peter Vanroose
//                   Nov.  4, 1996 - added copy constructor; this fixes bug
//                                   by copying the contents of 'buffer'.
// Modified by:      Peter Vanroose, Jul 97: copy constructor re-added !?!
//

//#include "Basics/util.h"
#include "gevd_status_mixin.h"

//======================================================================//
/* Status Variables.  Certain events are always errors, hence the error */
/* bit is set for those events.  For events which might be errors but   */
/* are not always, the error bit is left clear; the proper choice will  */
/* be made at the time the event occurs.                                */
//======================================================================//
#define MM_PROTECTED_FLAGS      0x000001CE // bits that may not be altered
//                                         // except by the buffer.
// Error conditions
//
#define MM_MEMORY_ERROR         0x00000083 // unable to allocate buffer
#define MM_DATA_OVERFLOW        0x00000004 // past initialized data
#define MM_OVERFLOW             0x00000088 // past allocated space
#define MM_UNDERFLOW            0x000000C0 // read before allocated space
//
// Status conditions
//
#define MM_NIL_BUFFER           0x00000001 // buffer is nil buffer
#define MM_DIRTY                0x00000010 // buffer changed since alloced
#define MM_FOREIGN_BLOCK        0x00000020 // foreign memory used for buffer
#define MM_ERROR                0x00000080 // error has occured
#define MM_WARN                 0x00000100 // warning has occured
//
// Buffer types, usually indicated at creation.  Note that it is important
// NOT to use the MM_PROTECTED type unless absolutely necessary; this will
// guarantee deallocation of resources when the buffer is no longer used.
//
#define MM_READ                 0x00000200 // buffer can be read.
#define MM_WRITE                0x00000400 // buffer can be written to.
#define MM_PROTECTED            0x00000800 // buffer cannot be deleted.
#define MM_FIXED                0x00001000 // buffer cannot be replaced.
#define MM_CLEAR                0x00002000 // buffer cleared at creation.  A
                                           // supplied buffer is not cleared.

#define MM_CREATION_FLAGS       0x00003E00 // Flags used at creation.

//---------------------------------------------------------------------

class gevd_memory_mixin : public gevd_status_mixin
{
  private:
    int size;               // amount of allocated memory (bytes).
    int touched;            // amount of allocated space touched.
    unsigned char* buffer;  // pointer to allocated memory

    int curr_into;          // amount into the allocated memory (bytes)
    unsigned char* current; // pointer to current place in memory.

    int offset;             // marked position in buffer.

  protected:
    void                    SetMemoryPtr(int s, void* ib = 0);

    inline unsigned char*   GetBufferPtr()  { return(buffer);    }
    inline unsigned char*   GetCurrentPtr() { return(current);   }
    inline int              GetSize()       { return(size);      }
    inline int              GetSize()const  { return(size);      }
    inline void SetStatus(int x=0)  {gevd_status_mixin::SetStatus(x); }
    inline void ClearStatus(int x=0) {gevd_status_mixin::ClearStatus(x);}
  public:
    // Constructors and Destructors
    //
    gevd_memory_mixin(int s = 0, void* ib = 0,
                      unsigned int type = MM_READ|MM_WRITE);

  virtual ~gevd_memory_mixin();
  gevd_memory_mixin(gevd_memory_mixin const&);
    inline const unsigned char* GetBufferPtr() const { return(buffer); }

    // Methods for moving about the file.
    //
    int  GetOffset()       { return(offset);                              }
    void SetOffset()       { offset = curr_into;                          }
#define min_(a,b) ((a)<(b)?a:b)
    void SkipBytes(int b)  { int skip = min_(b,size-curr_into);
                             current += skip; curr_into += skip;
                             if(skip<b) SetStatus(MM_OVERFLOW);
                             if(curr_into>touched)
                               SetStatus(MM_DATA_OVERFLOW | MM_WARN);     }
#undef min_
    void SkipToByte(int b) { if(b>size) SetStatus(MM_OVERFLOW);
                                   else {if(b>touched)
                                           SetStatus(MM_DATA_OVERFLOW|MM_WARN);
                                         current = buffer + b;
                                         curr_into = b;}                  }
    void SkipToStart()     { current = buffer;        curr_into = 0;      }
    void SkipToDataStart() { current = buffer+offset; curr_into = offset; }

    // Methods for reading and writing data.
    //
    int  ReadBytes(void* ib, int b);
    int  ReadBytes(void* ib, int b, int loc);
    int  ReadBytes(void* ib, int b, int* mapping);
    int  ReadBytes(void* ib, int b, int loc, int* mapping);
    int  WriteBytes(const void* ib, int b);
    int  WriteBytes(const void* ib, int b, int loc);
    void Clear();
};

//=======================================================================

#endif // _gevd_memory_mixin_h
