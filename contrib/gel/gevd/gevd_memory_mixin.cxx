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

//:
// \file

#include <vcl_cstring.h>   // memset() and memcpy() live here
//#include <vcl_memory.h>
#include <vcl_algorithm.h>

#include "gevd_memory_mixin.h"


//=========================================================================
//=========================================================================

//====================================================================
// Constructor.  Modified @ 6/11/91 gbs
//====================================================================
gevd_memory_mixin::gevd_memory_mixin(int s, void* ib, unsigned int type)
{
  size      = s;
  curr_into = 0;
  offset    = 0;
  SetStatus((type&MM_CREATION_FLAGS));

  if ((ib == 0) && (s>0))
  {
    touched = 0;
    buffer  = new unsigned char[size];

    // If desired, zero out the buffer and set the touched
    // flag to indicate that all data is valid.
    //
    if (GetStatusCode() & MM_CLEAR)
    {
      vcl_memset(buffer, 0, size); // corrected by PVr.
      touched = size;
    }

    // If we allocate a buffer and MM_PROTECTED is set,
    // we must clear it so that it can be deallocated.
    //
    ClearStatus(MM_PROTECTED);

    if (!buffer)
    {
      SetStatus(MM_MEMORY_ERROR);
      size = 0;
    }
  }
  else
  {
    touched = size;
    buffer  = (unsigned char*)ib;
    if (!buffer)
      SetStatus(MM_NIL_BUFFER|MM_WARN);
    else
      SetStatus(MM_PROTECTED|MM_FOREIGN_BLOCK|MM_DIRTY);
  }
  current = buffer;
}

gevd_memory_mixin::gevd_memory_mixin(gevd_memory_mixin const& m)
 : gevd_status_mixin(), size(m.GetSize()), touched(size), curr_into(0), offset(0)
{
  buffer  = new unsigned char[size];
  vcl_memcpy(buffer, m.GetBufferPtr(), size);
  current = buffer;
  SetStatus((m.Stat()&MM_CREATION_FLAGS));
  ClearStatus(MM_PROTECTED);
  if (!buffer)
  {
    SetStatus(MM_MEMORY_ERROR);
    size = 0;
  }
}

gevd_memory_mixin::~gevd_memory_mixin()
{
  if (!(GetStatusCode()&MM_PROTECTED))
    delete [] buffer;
}

int
gevd_memory_mixin::ReadBytes(void* ib, int b)
{
  if ((ib == 0) || !(GetStatusCode()&MM_READ))
    return 0;
  int num_b;
  if ((num_b=vcl_min(b, touched-curr_into)) < 0) num_b = 0;
  if (num_b<b)
    SetStatus(MM_DATA_OVERFLOW);
  else
    ClearStatus(MM_ERROR|MM_WARN);
  vcl_memcpy(ib, current,num_b);
  current   += num_b;
  curr_into += num_b;
  return num_b;
}

//====================================================================
//: Read b bytes from location loc to buffer ib.  New @ 6/11/91 gbs
//====================================================================
int
gevd_memory_mixin::ReadBytes(void* ib, int b, int loc)
{
  if ((ib == 0) || !(GetStatusCode()&MM_READ))
    return 0;

  //
  // loc is always relative to offset, so modify...
  //
  loc += offset;
  int num_b;
  if (loc<offset)
  {
    SetStatus(MM_UNDERFLOW);
    num_b = 0;
  }
  else
  {
    num_b = vcl_min(b+loc, touched) - loc;
    if (num_b<b)
      SetStatus(MM_OVERFLOW);
    else
      ClearStatus(MM_ERROR|MM_WARN);
  }
  vcl_memcpy(ib, buffer+loc,num_b);
  current = buffer + (curr_into += num_b);
  return num_b;
}

int
gevd_memory_mixin::ReadBytes(void* ib, int b, int* mapping)
{
  if ((ib == 0) || !(GetStatusCode()&MM_READ))
    return 0;
  int num_b;
  if ((num_b=vcl_min(b, touched-curr_into)) < 0) num_b = 0;;
  if (num_b<b)
    SetStatus(MM_OVERFLOW);
  else
    ClearStatus(MM_ERROR|MM_WARN);
  char* ibt = (char*)ib;
  for (int i = 0; i<num_b; i++)
    *(ibt++) = mapping[*(current++)];
  curr_into += num_b;
  return num_b;
}

//====================================================================
//: Read b bytes from loc to ib, thru mapping.  New @ 6/11/91 gbs
//====================================================================
int
gevd_memory_mixin::ReadBytes(void* ib, int b, int loc, int* mapping)
{
  if ((ib == 0) || !(GetStatusCode()&MM_READ))
    return 0;
  //
  // loc is always relative to offset, so modify...
  //
  loc += offset;
  int num_b;
  if (loc<offset)
  {
    SetStatus(MM_UNDERFLOW);
    num_b = 0;
  }
  else
  {
    num_b = vcl_min(b+loc, size) - loc;
  }
  if (num_b<b)
    SetStatus(MM_OVERFLOW);
  else
    ClearStatus(MM_ERROR|MM_WARN);
  if (loc<size && !(MM_UNDERFLOW & GetStatusCode()))
  {
    current   = buffer + loc;
    curr_into = vcl_min(loc+num_b,size);
  }
  char* ibt = (char*)ib;
  for (int i = 0; i<num_b; i++)
    *(ibt++) = mapping[*(current++)];
  return num_b;
}

void
gevd_memory_mixin::SetMemoryPtr(int s, void* ib)
{
  // If status is MM_FIXED, we are not allowed to replace
  // the buffer with a new one...
  //
  if (GetStatusCode()&MM_FIXED)
  {
    SetStatus(MM_ERROR);
    return;
  }

  if (!(GetStatusCode()&MM_PROTECTED)) delete buffer;
  size      = s;
  curr_into = 0;
  offset    = 0;
  ClearStatus();
  SetStatus(MM_READ|MM_WRITE);

  if ((ib == 0) && (s>0))
  {
    touched = 0;
    buffer  = new unsigned char[size];

    // If desired, zero out the buffer and set the touched
    // flag to indicate that all data is valid.
    if (GetStatusCode() & MM_CLEAR)
    {
      vcl_memset(buffer, 0, size); // corrected by PVr.
      touched = size;
    }

    // If we allocate a buffer and MM_PROTECTED is set,
    // we must clear it so that it can be deallocated.
    //
    ClearStatus(MM_PROTECTED);

    if (!buffer)
    {
      SetStatus(MM_MEMORY_ERROR);
      size = 0;
    }
  }
  else
  {
    touched = size;
    buffer  = (unsigned char*)ib;
    if (!buffer)
      SetStatus(MM_NIL_BUFFER|MM_WARN);
    else
      SetStatus(MM_PROTECTED|MM_FOREIGN_BLOCK|MM_DIRTY);
  }

  current = buffer;
}

int
gevd_memory_mixin::WriteBytes(const void* ib, int b)
{
  if (!(GetStatusCode()&MM_WRITE))
    return 0;
  int num_b = vcl_min(b, size-curr_into);
  vcl_memcpy(current, ib,num_b);
  if (num_b<b)
    SetStatus(MM_OVERFLOW);
  else
    ClearStatus(MM_ERROR|MM_WARN);
//      touched    = curr_into += num_b;  << Massive brain dammage!!!!!
  // What if the buffer already has valid data and we are
  // just writing over some of it, like we do in export!!!!


  curr_into += num_b;                  // fixed 11/1/91 ajh
  touched = vcl_max( touched, curr_into ); //

  current   += num_b;
  return num_b;
}

//====================================================================
//: Write b bytes to location loc from buffer ib.  New @ 6/11/91 gbs
//====================================================================
int
gevd_memory_mixin::WriteBytes(const void* ib, int b, int loc)
{
  //
  // loc is always relative to offset, so modify...
  //
  loc += offset;
  int num_b;
  if (loc<offset)
  {
    SetStatus(MM_UNDERFLOW);
    num_b = 0;
  }
  else
  {
    num_b = vcl_min(b+loc, size) - loc;
  }
  if (num_b<b) SetStatus(MM_OVERFLOW);
  if (loc<size && !(MM_UNDERFLOW & GetStatusCode()))
  {
    vcl_memcpy(buffer+loc,ib,num_b);
    curr_into = vcl_min(loc+num_b,size);
    current   = buffer + curr_into;
  }
  return num_b;
}

//====================================================================
//: Clear the memory and reset all of the appropriate variables.
//====================================================================
void
gevd_memory_mixin::Clear()
{
  touched = size;
  curr_into = 0;
  current = buffer;
  offset = 0;
  vcl_memset((char*)buffer,0,size);
}
