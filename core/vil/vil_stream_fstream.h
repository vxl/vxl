// This is core/vil/vil_stream_fstream.h
#ifndef vil_stream_fstream_h_
#define vil_stream_fstream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A vil_stream implementation using vcl_fstream
// \author    awf@robots.ox.ac.uk
// \date 16 Feb 00

#include <vcl_fstream.h>
#include <vil/vil_stream.h>

//: A vil_stream implementation using vcl_fstream
class vil_stream_fstream : public vil_stream
{
 public:
  vil_stream_fstream(char const* filename, char const* mode);

  // implement virtual vil_stream interface:
  bool ok() const { return f_.good(); }
  vil_streampos write(void const* buf, vil_streampos n);
  vil_streampos read(void* buf, vil_streampos n);
  vil_streampos tell() const;
  void seek(vil_streampos position);

  vil_streampos file_size() const;

 protected:
  ~vil_stream_fstream();

 private:
  vcl_ios_openmode flags_;
  mutable vcl_fstream f_;
  int id_;
  mutable vil_streampos end_;
};

#endif // vil_stream_fstream_h_
