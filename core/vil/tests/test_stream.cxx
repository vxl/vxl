// This is core/vil/tests/test_stream.cxx
#include <testlib/testlib_test.h>

#include <vcl_string.h>

#include <vil/vil_stream_fstream.h>

int
test_stream_main( int argc, char* argv[] )
{
  if( argc < 2 ) {
    vcl_cout << "Supply file_read_data directory as the first argument"
             << vcl_endl;
    return 0;
  }

  testlib_test_start(" stream");
  vcl_string dir = argv[1];

  {
    vil_stream* fs = new vil_stream_fstream( (dir+"/ff_grey8bit_compressed.jpg").c_str(), "r" );
    fs->ref();
    TEST( "Open file 1", fs->ok(), true );
    TEST( "Size file 1", fs->file_size(), 421 );
    fs->unref();
  }

  {
    vil_stream* fs = new vil_stream_fstream( (dir+"/ff_rgb8bit_littleendian.viff").c_str(), "r" );
    fs->ref();
    TEST( "Open file 2", fs->ok(), true );
    TEST( "Size file 2", fs->file_size(), 1069 );
    fs->unref();
  }

  return testlib_test_summary();
}
