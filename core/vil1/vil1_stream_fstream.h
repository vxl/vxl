#ifndef vil_stream_fstream_h_
#define vil_stream_fstream_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vil/vil_stream_fstream.h

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

  bool ok();

  int write(void const* buf, int n);
  int read(void* buf, int n);
  int tell();
  void seek(int position);

protected:
  ~vil_stream_fstream();

private:
  vcl_ios_openmode flags_;
  vcl_fstream f_;
  int id_;
};

#endif // vil_stream_fstream_h_
