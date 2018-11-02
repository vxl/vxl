// This is core/vil/tests/test_image_list.cxx
#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  std::cout << "************************\n"
           << " Testing vil_image_list\n"
           << "************************\n";
  // Test image list by saving three resource files and then
  // retrieving them using image_list.
  constexpr unsigned int ni = 73;
  constexpr unsigned int nj = 43;
  vil_image_view<unsigned short> image;
  image.set_size(ni,nj);
  for (unsigned i = 0; i<ni; ++i)
    for (unsigned j = 0; j<nj; ++j)
      image(i,j) = (unsigned short)(i + ni*j);
  vil_image_resource_sptr ir = vil_new_image_resource_of_view(image);

  const unsigned ni2 = 36, nj2 = 21;
  vil_image_view<unsigned short> image2;
  image2.set_size(ni2,nj2);
  for (unsigned i = 0; i<ni2; ++i)
    for (unsigned j = 0; j<nj2; ++j)
      image2(i,j) = (unsigned short)(i + ni2*j);
  vil_image_resource_sptr ir2 = vil_new_image_resource_of_view(image2);

  const unsigned ni3 = 18, nj3 = 10;
  vil_image_view<unsigned short> image3;
  image3.set_size(ni3,nj3);
  for (unsigned i = 0; i<ni3; ++i)
    for (unsigned j = 0; j<nj3; ++j)
      image3(i,j) = (unsigned short)(i + ni3*j);
  bool good;
  vil_image_resource_sptr ir3 = vil_new_image_resource_of_view(image3);
  std::string dir = "image_list_dir";
  {
    bool mkdir = vul_file::make_directory(dir.c_str());
    if (mkdir)
      std::cout << "vul make directory worked\n";
    else
      std::cout << "vul make directory failed\n";
    TEST("vil_is_directory",vil_image_list::vil_is_directory(dir.c_str()),true);
    int chd = vpl_chdir(dir.c_str());
    std::cout << "return code for chdir(" << dir << "): " << chd << std::endl;
    good = vil_save_image_resource(ir, "R0.tif", "tiff");
    TEST("vil_save_image_resource R0", good, true);
    std::cout << "Saved R0\n";
#if 0
    good = vil_save_image_resource(ir2, "R1.tif", "tiff");
    TEST("vil_save_image_resource R1", good, true);
    std::cout << "Saved R1\n";
#endif
    good = vil_save_image_resource(ir3, "R2.tif", "tiff");
    TEST("vil_save_image_resource R2", good, true);
    std::cout << "Saved R2\n";
    chd = vpl_chdir("..");
    std::cout << "return code for chdir(..): " << chd << std::endl;
    vil_image_list il(dir.c_str());
    std::vector<vil_image_resource_sptr> rescs = il.resources();
    std::cout << "Size = " << rescs.size() << std::endl;
    TEST("size()", rescs.size(), 2);
    if (rescs.size() == 2 && rescs[0] && rescs[1])
    {
      std::cout << "Successfully read the resource list\n"
               << "0->ni() = " << rescs[0]->ni() << '\n'
               << "1->ni() = " << rescs[1]->ni() << '\n';
      if (rescs[0]->ni() == 73) // no guarantee that files are returned in order
      {
        TEST("ni()", rescs[0]->ni(), 73);
        TEST("ni()", rescs[1]->ni(), 18);
      }
      else
      {
        TEST("ni()", rescs[0]->ni(), 18);
        TEST("ni()", rescs[1]->ni(), 73);
      }
    }
    else
      std::cout << "The resource list is corrupt\n";
  } // close open resource files
  // Cleanup resource files
  std::cout << "Cleaning up directory " << dir << '\n';;
  vpl_chdir(dir.c_str());
  std::string s =  "*.*";
  for (vul_file_iterator fit = s;fit; ++fit)
    vpl_unlink(fit());
  std::cout << "Removing the directory\n";
  vpl_chdir("..");
  vpl_rmdir(dir.c_str());
}

TESTMAIN(test_image_list);
