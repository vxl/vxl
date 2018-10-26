#ifndef gevd_memory_mixin_h_
#define gevd_memory_mixin_h_

//=======================================================================
//:
// \file
// \brief Captures all functional details required to access a real computer memory
//
// \author  Brian DeCleene
// \date    Nov. 16, 1990.
//
// \verbatim
//  Modifications:
//   Burt Smith - June 11, 1991
//    - Added status and handling of touched vs. untouched space.
//   Burt Smith - July  8, 1991
//    - Incorporated StatusMixin.
//   Burt Smith - July 17, 1991
//    - Changed ReadBytes(..., int loc) to seek from offset, not from start.
//   Peter Vanroose - Nov.  4, 1996
//    - added copy constructor; this fixes bug by copying the contents of 'buffer'.
//   Peter Vanroose, Jul 97
//    -  copy constructor re-added !?!
// \endverbatim
//=======================================================================

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
#define MM_ERROR                0x00000080 // error has occurred
#define MM_WARN                 0x00000100 // warning has occurred
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
  void                    SetMemoryPtr(int s, void* ib = nullptr);

  inline unsigned char*   GetBufferPtr()  { return buffer;    }
  const  unsigned char* GetBufferPtr() const { return buffer; }
  inline unsigned char*   GetCurrentPtr() { return current;   }
  inline int              GetSize() const { return size;      }
  inline void SetStatus(int x=0)  {gevd_status_mixin::SetStatus(x); }
  inline void ClearStatus(int x=0) {gevd_status_mixin::ClearStatus(x);}
 public:
  // Constructors and Destructors
  //
  gevd_memory_mixin(int s = 0, void* ib = nullptr,
                    unsigned int type = MM_READ|MM_WRITE);

  virtual ~gevd_memory_mixin();
  gevd_memory_mixin(gevd_memory_mixin const&);

  // Methods for moving about the file.
  //
  int  GetOffset() const { return offset;      }
  void SetOffset()       { offset = curr_into; }
#define min_(a,b) ((a)<(b)?a:b)
  void SkipBytes(int b)  { int skip = min_(b,size-curr_into);
                           current += skip; curr_into += skip;
                           if (skip<b) SetStatus(MM_OVERFLOW);
                           if (curr_into>touched)
                             SetStatus(MM_DATA_OVERFLOW | MM_WARN);     }
#undef min_
  void SkipToByte(int b) { if (b>size) SetStatus(MM_OVERFLOW);
                                 else {if (b>touched)
                                         SetStatus(MM_DATA_OVERFLOW|MM_WARN);
                                       current = buffer + b;
                                       curr_into = b;}                  }
  void SkipToStart()     { current = buffer;        curr_into = 0;      }
  void SkipToDataStart() { current = buffer+offset; curr_into = offset; }

  // Methods for reading and writing data.
  //
  int  ReadBytes(void* ib, int b);
  int  ReadBytes(void* ib, int b, int loc);
  int  ReadBytes(void* ib, int b, const int* mapping);
  int  ReadBytes(void* ib, int b, int loc, const int* mapping);
  int  WriteBytes(const void* ib, int b);
  int  WriteBytes(const void* ib, int b, int loc);
  void Clear();
};

//=======================================================================

#endif // gevd_memory_mixin_h_
