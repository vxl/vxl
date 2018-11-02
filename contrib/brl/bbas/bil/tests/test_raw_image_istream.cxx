#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <bil/bil_raw_image_istream.h>



static void create_raw_image_stream()
{
    std::string root = testlib_root_dir();
    std::string base = root + "/contrib/brl/bbas/bil/tests/file_read_data/";
    std::string rawFile = base+"test_raw_image_stream.raw";
    std::ofstream ofile(rawFile.c_str(),std::ios::binary);
    int ni = 1;
    int nj = 1;
    int pixelsize = 8;
    int nframes = 10;
    ofile.write((char*)&ni,sizeof(int));
    ofile.write((char*)&nj,sizeof(int));
    ofile.write((char*)&pixelsize,sizeof(int));
    ofile.write((char*)&nframes,sizeof(int));
    ofile.close();
}
static void test_raw_image_istream()
{
   create_raw_image_stream();
    std::string root = testlib_root_dir();
    std::string base = root + "/contrib/brl/bbas/bil/tests/file_read_data/";
    std::string rawFile = base+"test_raw_image_stream.raw";
    // Constructor - from a file glob string
    bil_raw_image_istream  stream(rawFile);
    bool good = false;
    if(stream.is_open())
        good = stream.num_frames() == 10 && stream.format() == VIL_PIXEL_FORMAT_BYTE && stream.width()==1 && stream.height()==1;
    TEST("Correctly Read the header",good, true);

}

TESTMAIN(test_raw_image_istream);
