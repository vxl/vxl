/*
  fsm@robots.ox.ac.uk
*/
#include <vpl/vpl_unistd.h>

#include <vil/vil_16bit.h>
#include <vil/vil_stream_fstream.h>

int main(int, char **)
{
  char const *file = "/tmp/smoo";
  vil_stream *s = 0;

  // write bytes
  s = new vil_stream_fstream(file, "w");
  s->ref();
  
  // the bytes written should be 0x02 0x01 0x03 0x04, in that
  // order, on all architectures.
  s->seek(0);
  vil_16bit_write_little_endian(s, 0x0102);
  vil_16bit_write_big_endian   (s, 0x0304);

  s->unref();

  // read them again.
  s = new vil_stream_fstream(file, "r");
  s->ref();
  
  s->seek(0);
  unsigned char bytes[4];
  s->read(bytes, 4);

  s->unref();

  // clean up.
  vpl_unlink(file);

  // check
  if (bytes[0] == 0x02 &&
      bytes[1] == 0x01 &&
      bytes[2] == 0x03 &&
      bytes[3] == 0x04)
    return 0;
  else
    return 1;
}
