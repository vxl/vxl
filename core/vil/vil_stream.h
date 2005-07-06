// This is core/vil/vil_stream.h
#ifndef vil_stream_h_
#define vil_stream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Stream interface for VIL image loaders
// \author  awf@robots.ox.ac.uk
// \date 16 Feb 00

#include <vxl_config.h>
#if VXL_HAS_INT_64
typedef vxl_int_64 vil_streampos;
#else //VXL_HAS_INT_64
typedef vxl_int_32 vil_streampos;
#endif //VXL_HAS_INT_64

//: Stream interface for VIL image loaders
// This allows the loaders to be used with any type of stream.
class vil_stream
{
 public:
  //: Return false if the stream is broken.
  virtual bool ok() const = 0;

  //: Write n bytes from buf. Returns number of bytes written.
  //  The return value is less than n only in case of device failure.
  virtual vil_streampos write(void const* buf, vil_streampos n) = 0;

  //: Read n bytes into buf. Returns number of bytes read.
  //  The return value is less than n only at eof.
  virtual vil_streampos read(void* buf, vil_streampos n) = 0;

  //: Return file pointer
  virtual vil_streampos tell() const = 0;

  //: Goto file pointer
  virtual void seek(vil_streampos position) = 0;

  //: Amount of data in the stream
  virtual vil_streampos file_size() const = 0;

  //: up/down the reference count
  void ref() { ++refcount_; }

  void unref();

 protected:
  vil_stream();
  virtual ~vil_stream();

 private: // use the methods, Luke!
  int refcount_;
};

#endif // vil_stream_h_
