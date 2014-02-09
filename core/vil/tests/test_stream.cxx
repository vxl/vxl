// This is core/vil/tests/test_stream.cxx
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <vcl_string.h>
#include <vul/vul_file.h>

#include <vil/vil_stream_fstream.h>
#include <vxl_config.h>

#if defined(VCL_WIN32) && VXL_USE_WIN_WCHAR_T
# include <windows.h>
#endif

static void
test_stream( int argc, char* argv[] )
{
  vcl_string dir;
  // Test if path not passed in to argv by CMake
  if ( argc >= 2 ) {
    dir = argv[1];
  }else{
    vcl_string root = testlib_root_dir();
    dir = root + "/core/vil/tests/file_read_data";
  }
  bool exists = vul_file::is_directory(dir);
  if(!exists){
    TEST("Path not defined", false, true);
    return;
  }

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

#if defined(VCL_WIN32) && VXL_USE_WIN_WCHAR_T
  const unsigned int size = 4096;  // should be enough
  std::wstring wdir;
  wdir.resize(size);
  const int ret = MultiByteToWideChar(CP_ACP, 0, dir.c_str(), int(dir.size()), &wdir[0], size);
  TEST( "dir name converts to wchart_t type", ret>0, true );
  wdir.resize(ret);

  {
    vil_stream* fs = new vil_stream_fstream( (wdir+L"/ff_grey8bit_compressed.jpg").c_str(), "r" );
    fs->ref();
    TEST( "[wchar_t] Open file 1", fs->ok(), true );
    TEST( "[wchar_t] Size file 1", fs->file_size(), 421 );
    fs->unref();
  }

  {
    vil_stream* fs = new vil_stream_fstream( (wdir+L"/ff_rgb8bit_littleendian.viff").c_str(), "r" );
    fs->ref();
    TEST( "[wchar_t] Open file 2", fs->ok(), true );
    TEST( "[wchar_t] Size file 2", fs->file_size(), 1069 );
    fs->unref();
  }
#endif //defined(VCL_WIN32) && VXL_USE_WIN_WCHAR_T
}

TESTMAIN_ARGS(test_stream);
