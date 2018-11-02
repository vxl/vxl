#include <limits>
#include <vil/vil_config.h>
#if HAS_DCMTK

#include "vil_dicom_stream.h"
#include <vil/vil_stream.h>
#include <dcerror.h>

#include <cassert>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// ===========================================================================
//                                                             stream producer

vil_dicom_stream_producer::
vil_dicom_stream_producer( vil_stream* in_vs )
  : vs_( in_vs )
{
  vs_->ref();
}


vil_dicom_stream_producer::
~vil_dicom_stream_producer()
{
  vs_->unref();
}


OFBool
vil_dicom_stream_producer::
good() const
{
  return vs_->ok();
}


OFCondition
vil_dicom_stream_producer::
status() const
{
  return good() ? EC_Normal : EC_InvalidStream;
}


OFBool
vil_dicom_stream_producer::
eos() const
{
  return vs_->tell() >= vs_->file_size();
}


Uint32
vil_dicom_stream_producer::
avail() const
{
  vil_streampos n = vs_->file_size() - vs_->tell();
  assert( n >= 0 );
  //assert ensures that the cast will succeed.
  //apparently dicom streams only support 32 bit positions
  //whereas vil_streams now support 64 bit positions (when
  //available)
  assert( n <= (vil_streampos)std::numeric_limits<Uint32>::max() );
  return (Uint32)n;
}


Uint32
vil_dicom_stream_producer::
read( void *buf, Uint32 buflen )
{
  vil_streampos n = vs_->read( buf, buflen );
  assert( n >= 0 );
  //assert ensures that the cast will succeed.
  //apparently dicom streams only support 32 bit positions
  //whereas vil_streams now support 64 bit positions (when
  //available)
  assert( n <= (vil_streampos)std::numeric_limits<Uint32>::max() );
  return (Uint32)n;
}


Uint32
vil_dicom_stream_producer::
skip(Uint32 skiplen)
{
  vs_->seek( vs_->tell() + skiplen );
  return skiplen;
}


void
vil_dicom_stream_producer::
putback(Uint32 num)
{
  vs_->seek( vs_->tell() - (long int)num );
}


// ===========================================================================
//                                                              stream factory

vil_dicom_stream_factory::
vil_dicom_stream_factory( vil_stream* in_vs )
  : vs_( in_vs )
{
  vs_->ref();
}

vil_dicom_stream_factory::
~vil_dicom_stream_factory()
{
  vs_->unref();
}


DcmInputStream*
vil_dicom_stream_factory::
create() const
{
  return new vil_dicom_stream_input( vs_ );
}


// ===========================================================================
//                                                                stream input

vil_dicom_stream_input::
vil_dicom_stream_input( vil_stream* vs )
  : DcmInputStream( new vil_dicom_stream_producer( vs ) )
{
}


vil_dicom_stream_input::
~vil_dicom_stream_input()
{
  delete currentProducer();
}


DcmInputStreamFactory*
vil_dicom_stream_input::
newFactory() const
{
  return 0;
}

#endif // HAS_DCMTK
