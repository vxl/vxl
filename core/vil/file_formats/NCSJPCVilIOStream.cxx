#include "NCSJPCVilIOStream.h"

#include <vil/vil_stream.h>
#include <vcl_limits.h>

#undef max
#undef min
  //vil_streams can only hand 32 bit offsets
  static const vil_streampos maxVilStreamPos = vcl_numeric_limits< vil_streampos >::max();
  static const vil_streampos minVilStreamPos = vcl_numeric_limits< vil_streampos >::min();

CNCSJPCVilIOStream::CNCSJPCVilIOStream()
  : mVilStream( 0 ),
    mHomePos( -1 )
{ }

CNCSError CNCSJPCVilIOStream::Open( vil_stream* stream )
{
  mVilStream = stream;
  mVilStream->ref();
  mHomePos = stream->tell();

  *(CNCSError*)this = CNCSJPCIOStream::Open("VIL", false);

  return *(CNCSError*)this;
}

CNCSError CNCSJPCVilIOStream::Close()
{
  if ( mVilStream ){
    mVilStream->unref();
    mVilStream = 0;
    mHomePos = -1;
  }

  *(CNCSError*)this = NCS_SUCCESS;

  return *(CNCSError*)this;
}

bool CNCSJPCVilIOStream::Seek()
{
  return true; //TODO: is this correct?
}

bool CNCSJPCVilIOStream::Seek(INT64 offset, Origin origin )
{
#undef max
#undef min
  //static const INT64 maxInt64 = vcl_numeric_limits< INT64 >::max();
//NOT USED  static const vil_streampos maxVilStreamPos = vcl_numeric_limits< vil_streampos >::max();
//NOT USED  static const vil_streampos minVilStreamPos = vcl_numeric_limits< vil_streampos >::min();

  INT64 absoluteOffset = mHomePos;
  switch ( origin )
  {
    case START:
      absoluteOffset += offset;
      break;
    case END:
      absoluteOffset += Size() - 1 - offset;
      break;
    case CURRENT:
      absoluteOffset += Tell() + offset;
      break;
    default:
      *(CNCSError*)this = NCS_FILE_IO_ERROR;
      return false;
  }

  //make sure the offset specifies a valid location in the stream
  if ( ! ( absoluteOffset >= 0 && absoluteOffset <= mVilStream->file_size() ) ) {
    *(CNCSError*)this = NCS_FILE_SEEK_ERROR;
  } else {
    //this cast should be safe because we tested to make sure that
    //absoluteOffset is < mVilStream->file_size()... if that is true
    //then absoluteOffset is < 2^31 (max int size)
    mVilStream->seek( absoluteOffset );
    *(CNCSError*)this = NCS_SUCCESS;
  }
  return *(CNCSError*)this == NCS_SUCCESS;
}

INT64 NCS_FASTCALL CNCSJPCVilIOStream::Tell()
{
  return (INT64) (mVilStream->tell() - mHomePos);
}

INT64 NCS_FASTCALL CNCSJPCVilIOStream::Size()
{
  return (INT64) (mVilStream->file_size() - mHomePos);
}

bool NCS_FASTCALL CNCSJPCVilIOStream::Read(void* buffer, UINT32 count)
{
  vil_streampos bytesRead = mVilStream->read( buffer, count );
  if ( bytesRead != count ){
    *(CNCSError*)this = NCS_INVALID_PARAMETER;
    return false;
  }
  return true;
}

bool NCS_FASTCALL CNCSJPCVilIOStream::Write(void* buffer, UINT32 count)
{
  vil_streampos bytesWritten = mVilStream->write( buffer, count );
  if ( bytesWritten != count ){
    *(CNCSError*)this = NCS_INVALID_PARAMETER;
    return false;
  }
  return true;
}
