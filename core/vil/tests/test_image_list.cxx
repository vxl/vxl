// This is core/vil/tests/test_image_list.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_image_list.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_save.h>
#define DEBUG

static void test_image_list()
{
  vcl_cout << "************************\n"
           << " Testing vil_image_list\n"
           << "************************\n";
  //Test image list by saving three resource files and then
  //retrieving them using image_list.
  const unsigned int ni = 73;
  const unsigned int nj = 43;
  vil_image_view<unsigned short> image;
  image.set_size(ni,nj);
  for (unsigned i = 0; i<ni; ++i)
    for (unsigned j = 0; j<nj; ++j)
      image(i,j) = i + ni*j;
  vil_image_resource_sptr ir = vil_new_image_resource_of_view(image);

  const unsigned ni2 = 36, nj2 = 21;
  vil_image_view<unsigned short> image2;
  image2.set_size(ni2,nj2);
  for (unsigned i = 0; i<ni2; ++i)
    for (unsigned j = 0; j<nj2; ++j)
      image2(i,j) = i + ni2*j;
  vil_image_resource_sptr ir2 = vil_new_image_resource_of_view(image2);

  const unsigned ni3 = 18, nj3 = 10;
  vil_image_view<unsigned short> image3;
  image3.set_size(ni3,nj3);
  for (unsigned i = 0; i<ni3; ++i)
    for (unsigned j = 0; j<nj3; ++j)
      image3(i,j) = i + ni3*j;
  bool good;
  vil_image_resource_sptr ir3 = vil_new_image_resource_of_view(image3);
  vcl_string dir = "image_list_dir";
  {
    bool mkdir = vul_file::make_directory(dir.c_str());
    if (mkdir)
      vcl_cout << "vul make directory worked\n";
    else
      vcl_cout << "vul make directory failed\n";
    TEST("vil_is_directory",vil_image_list::vil_is_directory(dir.c_str()),true);
    int chd = vpl_chdir(dir.c_str());
    vcl_cout << "return code for chdir(" << dir << "): " << chd << vcl_endl;
    good = vil_save_image_resource(ir, "R0.tif", "tiff");
    TEST("vil_save_image_resource R0", good, true);
    vcl_cout << "Saved R0\n";
#if 0
    good = vil_save_image_resource(ir2, "R1.tif", "tiff");
    TEST("vil_save_image_resource R1", good, true);
    vcl_cout << "Saved R1\n";
#endif
    good = vil_save_image_resource(ir3, "R2.tif", "tiff");
    TEST("vil_save_image_resource R2", good, true);
    vcl_cout << "Saved R2\n";
    chd = vpl_chdir("..");
    vcl_cout << "return code for chdir(..): " << chd << vcl_endl;
    vil_image_list il(dir.c_str());
    vcl_vector<vil_image_resource_sptr> rescs = il.resources();
    vcl_cout << "Size = " << rescs.size() << vcl_endl;
    TEST("size()", rescs.size(), 2);
    if (rescs.size() == 2 && rescs[0])
    {
      vcl_cout << "Succefully read the resource list\n";
      TEST("ni()", rescs[0]->ni(), 73);
    }
    else
      vcl_cout << "The resource list is corrupt\n";
  }//close open resource files
  //Cleanup resource files
  vcl_cout << "Cleaning up directory " << dir << '\n';;
  vpl_chdir(dir.c_str());
  vcl_string s =  "*.*";
  for (vul_file_iterator fit = s;fit; ++fit)
    vpl_unlink(fit());
  vcl_cout << "Removing the directory\n";
  vpl_chdir("..");
  vpl_rmdir(dir.c_str());
}

TESTMAIN(test_image_list);
