// This is contrib/mul/vil2/vil2_stream_fstream.h
#ifndef vil2_stream_fstream_h_
#define vil2_stream_fstream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A vil2_stream implementation using vcl_fstream
// \author    awf@robots.ox.ac.uk
// \date 16 Feb 00

#include <vcl_fstream.h>
#include <vil2/vil2_stream.h>

//: A vil2_stream implementation using vcl_fstream
class vil2_stream_fstream : public vil2_stream
{
 public:
  vil2_stream_fstream(char const* filename, char const* mode);

  // implement virtual vil2_stream interface:
  bool ok() const { return f_.good(); }
  vil2_streampos write(void const* buf, vil2_streampos n);
  vil2_streampos read(void* buf, vil2_streampos n);
  vil2_streampos tell();
  void seek(vil2_streampos position);

 protected:
  ~vil2_stream_fstream();

 private:
  vcl_ios_openmode flags_;
  vcl_fstream f_;
  int id_;
};

#endif // vil2_stream_fstream_h_
