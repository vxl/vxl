// This is core/vil1/vil1_stream.h
#ifndef vil1_stream_h_
#define vil1_stream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Stream interface for VIL image loaders
// \author  awf@robots.ox.ac.uk
// \date 16 Feb 00

typedef long int vil1_streampos; // should be identical to vcl_istream::streampos

//: Stream interface for VIL image loaders
// This allows the loaders to be used with any type of stream.
class vil1_stream
{
 public:
  //: Return false if the stream is broken.
  virtual bool ok() const = 0;

  //: Write n bytes from buf. Returns number of bytes written.
  //  The return value is less than n only in case of device failure.
  virtual vil1_streampos write(void const* buf, vil1_streampos n) = 0;

  //: Read n bytes into buf. Returns number of bytes read.
  //  The return value is less than n only at eof.
  virtual vil1_streampos read(void* buf, vil1_streampos n) = 0;

  //: Return file pointer
  virtual vil1_streampos tell() const = 0;

  //: Goto file pointer
  virtual void seek(vil1_streampos position) = 0;

  //: up/down the reference count
  void ref() { ++refcount_; }
  void unref();

 protected:
  vil1_stream();
  virtual ~vil1_stream();

 private: // use the methods, Luke!
  int refcount_;
};

#endif // vil1_stream_h_
