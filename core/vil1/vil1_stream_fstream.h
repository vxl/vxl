// This is vxl/vil/vil_stream_fstream.h
#ifndef vil_stream_fstream_h_
#define vil_stream_fstream_h_
#ifdef __GNUC__
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
class vil_stream_fstream : public vil_stream {
public:
  vil_stream_fstream(char const* filename, char const* mode);

  // implement virtual vil_stream interface:
  bool ok() const { return f_.good(); }
  vil_streampos write(void const* buf, vil_streampos n);
  vil_streampos read(void* buf, vil_streampos n);
  vil_streampos tell();
  void seek(vil_streampos position);

protected:
  ~vil_stream_fstream();

private:
  vcl_ios_openmode flags_;
  vcl_fstream f_;
  int id_;
};

#endif // vil_stream_fstream_h_
