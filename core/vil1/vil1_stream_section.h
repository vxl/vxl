#ifndef vil_stream_section_h_
#define vil_stream_section_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

// Adapt a given vil_stream by presenting a section
// of it to the client. Objects of this class WILL
// NOT delete the given vil_stream on destruction.
//
// To allow several sections to access the same
// underlying stream simultaneously, this stream
// will seek a lot on the underlying stream, so 
// make sure that it's not expensive to do that.
// For example, if the underlying stream is an
// in-core vil_stream, seeking is probably quite
// cheap.
//
// A length of -1 means that there is no upper limit
// on the length of the section presented, at least
// not one imposed by this class.

#include <vil/vil_stream.h>

class vil_stream_section : public vil_stream {
public:
  vil_stream_section(vil_stream *underlying, int pos, int n = -1);
  ~vil_stream_section();

  //: implement virtual interface
  int read (void       *buf, int n);
  int write(void const *buf, int n);
  int tell();
  void seek(int position);

private:
  int transfer(void *buf, int n, bool read);

  vil_stream *base;
  int start;
  int length;
  int curpos; // 0 <= curpos <= length if length>=0
};

#endif
