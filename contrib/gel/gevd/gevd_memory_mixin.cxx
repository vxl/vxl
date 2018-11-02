// This is gel/gevd/gevd_memory_mixin.cxx
#include <cstring>
#include <iostream>
#include <algorithm>
#include "gevd_memory_mixin.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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

  if ((ib == nullptr) && (s>0))
  {
    touched = 0;
    buffer  = new unsigned char[size];

    // If desired, zero out the buffer and set the touched
    // flag to indicate that all data is valid.
    //
    if (GetStatusCode() & MM_CLEAR)
    {
      std::memset(buffer, 0, size); // corrected by PVr.
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
  std::memcpy(buffer, m.GetBufferPtr(), size);
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
  if ((ib == nullptr) || !(GetStatusCode()&MM_READ))
    return 0;
  int num_b;
  if ((num_b=std::min(b, touched-curr_into)) < 0) num_b = 0;
  if (num_b<b)
    SetStatus(MM_DATA_OVERFLOW);
  else
    ClearStatus(MM_ERROR|MM_WARN);
  std::memcpy(ib, current,num_b);
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
  if ((ib == nullptr) || !(GetStatusCode()&MM_READ))
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
    num_b = std::min(b+loc, touched) - loc;
    if (num_b<b)
      SetStatus(MM_OVERFLOW);
    else
      ClearStatus(MM_ERROR|MM_WARN);
  }
  std::memcpy(ib, buffer+loc,num_b);
  current = buffer + (curr_into += num_b);
  return num_b;
}

int
gevd_memory_mixin::ReadBytes(void* ib, int b, const int* mapping)
{
  if ((ib == nullptr) || !(GetStatusCode()&MM_READ))
    return 0;
  int num_b;
  if ((num_b=std::min(b, touched-curr_into)) < 0) num_b = 0;;
  if (num_b<b)
    SetStatus(MM_OVERFLOW);
  else
    ClearStatus(MM_ERROR|MM_WARN);
  char* ibt = (char*)ib;
  for (int i = 0; i<num_b; i++)
    *(ibt++) = (char)mapping[*(current++)];
  curr_into += num_b;
  return num_b;
}

//====================================================================
//: Read b bytes from loc to ib, thru mapping.  New @ 6/11/91 gbs
//====================================================================
int
gevd_memory_mixin::ReadBytes(void* ib, int b, int loc, const int* mapping)
{
  if ((ib == nullptr) || !(GetStatusCode()&MM_READ))
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
    num_b = std::min(b+loc, size) - loc;
  }
  if (num_b<b)
    SetStatus(MM_OVERFLOW);
  else
    ClearStatus(MM_ERROR|MM_WARN);
  if (loc<size && !(MM_UNDERFLOW & GetStatusCode()))
  {
    current   = buffer + loc;
    curr_into = std::min(loc+num_b,size);
  }
  char* ibt = (char*)ib;
  for (int i = 0; i<num_b; i++)
    *(ibt++) = (char)mapping[*(current++)];
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

  if ((ib == nullptr) && (s>0))
  {
    touched = 0;
    buffer  = new unsigned char[size];

    // If desired, zero out the buffer and set the touched
    // flag to indicate that all data is valid.
    if (GetStatusCode() & MM_CLEAR)
    {
      std::memset(buffer, 0, size); // corrected by PVr.
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
  int num_b = std::min(b, size-curr_into);
  std::memcpy(current, ib,num_b);
  if (num_b<b)
    SetStatus(MM_OVERFLOW);
  else
    ClearStatus(MM_ERROR|MM_WARN);
  /* touched = curr_into += num_b; */ // << Massive brain damage!!!!!
  // What if the buffer already has valid data and we are
  // just writing over some of it, like we do in export!!!!

  curr_into += num_b;                  // fixed 11/1/91 ajh
  touched = std::max( touched, curr_into ); //

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
    num_b = std::min(b+loc, size) - loc;
  }
  if (num_b<b) SetStatus(MM_OVERFLOW);
  if (loc<size && !(MM_UNDERFLOW & GetStatusCode()))
  {
    std::memcpy(buffer+loc,ib,num_b);
    curr_into = std::min(loc+num_b,size);
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
  std::memset((char*)buffer,0,size);
}
