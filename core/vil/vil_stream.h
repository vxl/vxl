// This is core/vil2/vil2_stream.h
#ifndef vil2_stream_h_
#define vil2_stream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Stream interface for VIL image loaders
// \author  awf@robots.ox.ac.uk
// \date 16 Feb 00

typedef long int vil2_streampos; // should be identical to vcl_istream::streampos

//: Stream interface for VIL image loaders
// This allows the loaders to be used with any type of stream.
class vil2_stream
{
 public:
  //: Return false if the stream is broken.
  virtual bool ok() const = 0;

  //: Write n bytes from buf. Returns number of bytes written.
  //  The return value is less than n only in case of device failure.
  virtual vil2_streampos write(void const* buf, vil2_streampos n) = 0;

  //: Read n bytes into buf. Returns number of bytes read.
  //  The return value is less than n only at eof.
  virtual vil2_streampos read(void* buf, vil2_streampos n) = 0;

  //: Return file pointer
  virtual vil2_streampos tell() = 0;

  //: Goto file pointer
  virtual void seek(vil2_streampos position) = 0;

  //: up/down the reference count
  void ref();
  void unref();

 protected:
  vil2_stream();
  virtual ~vil2_stream();

 private: // use the methods, Luke!
  int refcount_;
};

#endif // vil2_stream_h_
