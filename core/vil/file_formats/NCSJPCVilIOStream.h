// vil_j2k: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef NCSJPCVILIOSTREAM_H
#define NCSJPCVILIOSTREAM_H

#include <NCSJPCIOStream.h>

#include <vil/vil_stream.h>

/**
  * Wrapper class that allows you to effectively "convert" a vil_stream
  * to a CNCSJPCIOStream.  This class is used by vil_j2k_image to read JPEG 2000
  * imagery.  A couple of notes:
  * 1) This stream's home position (ie. the place it goes when you call Seek(0,START),
  * is the current position of the stream that is passed to Open() at the time of that call.
  * \sa Open() for more info.
  *
  * Known Issues:
  * 1) As of 2/24/05, the writing capability of this class has not been tested.  In
  * theory it should work just fine though.
  */
class CNCSJPCVilIOStream : public CNCSJPCIOStream
{
public:
  CNCSJPCVilIOStream();

  /**
    * Pass me the stream you want me to wrap.  stream->tell() at the time
    * of this function call will become my home position.  see \sa mHomePos for
    * more details on what that means.
    */
  virtual CNCSError Open( vil_stream* stream );

  virtual CNCSError Close();

  virtual bool NCS_FASTCALL Seek ();

  virtual bool NCS_FASTCALL Seek (INT64 offset, Origin origin = CURRENT);

  virtual INT64 NCS_FASTCALL Tell(); 

  virtual INT64 NCS_FASTCALL Size();

  virtual bool NCS_FASTCALL Read (void* buffer, UINT32 count);

  virtual bool NCS_FASTCALL Write (void* buffer, UINT32 count);

protected:
  /**
    * The stream I get all my data from (and write too)
    */
  vil_stream* mVilStream;
  /**
    * This position is my home position (ie. mVilStream->tell() when it was
    * passed into me).  All Seek()'s with an origin of START will be relative 
    * to me.  Tell() is the position relative to this position etc.
    */
  vil_streampos mHomePos;
};

#endif //NCSJPCVILIOSTREAM_H