/*
  fsm
*/
#include <vpl/vpl.h>
#include <vul/vul_temp_filename.h>

#include <vil1/vil1_16bit.h>
#include <vil1/vil1_stream_fstream.h>

#include <testlib/testlib_test.h>

static void test_write_endian()
{
  vcl_string tmp_nam = vul_temp_filename() + ".bin";
  char const *file = tmp_nam!="" ? tmp_nam.c_str() : "smoo.bin";
  vil1_stream *s = 0;

  // write bytes
  s = new vil1_stream_fstream(file, "w");
  s->ref();
#ifdef LEAVE_IMAGES_BEHIND
  vpl_chmod(file, 0666); // -rw-rw-rw-
#endif

  // the bytes written should be 0x02 0x01 0x03 0x04, in that
  // order, on all architectures.
  s->seek(0);
  vil1_16bit_write_little_endian(s, 0x0102);
  vil1_16bit_write_big_endian   (s, 0x0304);

  s->unref();

  // read them again.
  s = new vil1_stream_fstream(file, "r");
  s->ref();

  s->seek(0);
  unsigned char bytes[4];
  s->read(bytes, 4);

  s->unref();

  // clean up.
#ifndef LEAVE_IMAGES_BEHIND
  vpl_unlink(file);
#endif

  TEST ("byte values",
        bytes[0] == 0x02 &&
        bytes[1] == 0x01 &&
        bytes[2] == 0x03 &&
        bytes[3] == 0x04,  true);
}

TESTMAIN(test_write_endian);
