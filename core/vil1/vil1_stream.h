#ifndef vil_stream_h_
#define vil_stream_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_stream - Stream interface for VIL image loaders
// .INCLUDE vil/vil_stream.h
// .FILE vil_stream.cxx
// .SECTION Author
//    awf@robots.ox.ac.uk
// Created: 16 Feb 00

//: Stream interface for VIL image loaders
// This allows the loaders to be used with any type of stream.
class vil_stream {
public:
  //: Write n bytes from buf. returns number of bytes written.
  //  the return value is less than n only in case of device failure.
  virtual int write(void const* buf, int n) = 0;

  //: Read n bytes into buf
  //  the return value is less than n only at eof.
  virtual int read(void* buf, int n) = 0;

  //: Return file pointer
  virtual int  tell() = 0;

  //: Goto file pointer
  virtual void seek(int position) = 0;

  //: up/down the reference count
  void ref();
  void unref();

protected:
  vil_stream();
  virtual ~vil_stream();

private: // use the methods, Luke!
  int refcount;
};

#endif // vil_stream_h_
