// This is core/vil/tests/test_pyramid_image_resource.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_new.h>
#include <vil/vil_load.h>
#include <vil/vil_property.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vil/vil_image_view.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/vil_blocked_image_facade.h>
#include <vil/file_formats/vil_pyramid_image_list.h>
#include <vil/file_formats/vil_tiff.h>
#include <vil/vil_image_list.h>
#include <vul/vul_file.h>
#include <vil/file_formats/vil_tiff_header.h>
#define DEBUG

static void test_pyramid_image_resource()
{
  vcl_cout << "************************************\n"
           << " Testing vil_pyramid_image_resource\n"
           << "************************************\n";
  //Test Resource
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
  vil_image_resource_sptr ir3 = vil_new_image_resource_of_view(image3);
  vcl_string d = "pyramid_dir";
  vcl_cout << "Made pyramid directory "<< d << '\n';
  vul_file::make_directory(d.c_str());
  //Test pyramid_image_list::put_resource(..)
  vil_pyramid_image_list* pir = new vil_pyramid_image_list(d.c_str());
  if (!pir)
    TEST("Make a directory-based pyramid resource", true, false);
  else
  {
    TEST("Make a directory-based pyramid resource", true, true);
    pir->put_resource(ir3);
    for (unsigned i = 0; i<pir->nlevels(); ++i)
      pir->print(i);
    pir->put_resource(ir2);
    vcl_cout << '\n';
    for (unsigned i = 0; i<pir->nlevels(); ++i)
      pir->print(i);
    pir->put_resource(ir);
    vcl_cout << '\n';
    for (unsigned i = 0; i<pir->nlevels(); ++i)
      pir->print(i);
    float actual_scale = 1.0f;
    vil_image_view<unsigned short> tview =
      pir->get_copy_view(15,10, 10, 20, 0.6f, actual_scale);
    vcl_cout << "tview dimensions (" << tview.ni() << ' ' << tview.nj()
             << ") actual scale = "<< actual_scale << '\n';
    TEST_NEAR("actual scale in pyramid image", actual_scale, 0.493, 0.01);
    delete pir;
  }
  //Clean up directory
  vil_image_list vl(d.c_str());
  vcl_cout << "Starting to clean directory\n";
  vl.clean_directory();
  vcl_cout << "Cleaned up the pyramid directory\n";
  //Test image decimation. Designed to work with blocked images
  vil_image_resource_sptr bir = new vil_blocked_image_facade(ir, 16, 16);
  //test decimation for generating a pyramid level
  vcl_string dec_file = "decimated_resource";
  {
    vil_image_resource_sptr dec_resc =
      vil_pyramid_image_resource::decimate(bir, dec_file.c_str());
    if (dec_resc)
      vcl_cout << "Made a successful decimated resource\n";
  }//close open resource files
  {
    vil_image_resource_sptr reload_dec =
      vil_load_image_resource(dec_file.c_str());
    if (!reload_dec)
      TEST("reload decimated resource", true, false);
    else
    {
      TEST("reload decimated resource", true, true);
      vil_image_view<unsigned short> dec_view = reload_dec->get_view();

      vcl_cout << "base view\n";
      for (unsigned j = 0; j<4; ++j)
      {
        for (unsigned i = 0; i<4; ++i)
          vcl_cout << image(i,j) << ' ';

        vcl_cout << '\n';
      }
      vcl_cout << "decimated view\n";
      for (unsigned j = 0; j<2; ++j)
      {
        for (unsigned i = 0; i<2; ++i)
          vcl_cout << dec_view(i,j) << ' ';

        vcl_cout << '\n';
      }
      TEST("decimated image read", dec_view(0,0), 37);
    }
  }//close open resource files
  vpl_unlink(dec_file.c_str());
  vcl_cout << "Clean up decimated resource file\n";
  {
    vil_pyramid_image_resource_sptr bpyr =
      vil_new_pyramid_image_list_from_base(d.c_str(), ir, 3, true, "tiff", "R");
    if (bpyr)
    {
      for (unsigned l = 0; l<bpyr->nlevels(); ++l)
        bpyr->print(l);
      float ac = 1.0f;
      vil_image_view<unsigned short> v25 = bpyr->get_copy_view(0.25f , ac);
      unsigned ni25 = v25.ni();
      vil_image_view<unsigned short> v50 = bpyr->get_copy_view(0.5f , ac);
      unsigned ni50 = v50.ni();
      vcl_cout << "ni25 = " << ni25 << ",  ni50 = " << ni50 << '\n';
      bool good = (ni25 == 19 && ni50 == 37) || (ni25 == 18 && ni50 == 36);
      TEST("Pyramid read", good, true);
    }
    else
      TEST("Pyramid create", false, true);
  }//close open resource files

  //clean directory
  vl.clean_directory();
  vcl_cout << "Cleaning bpyr directory\n";
  //Test pyramid_image_list::put_resource
  bool good = true;
  {//scope for dpyr
    vil_pyramid_image_resource_sptr dpyr =
      vil_new_pyramid_image_resource(d.c_str(), "pyil");
    if (dpyr)
    {
      good = good && dpyr->put_resource(ir);
      good = good && dpyr->put_resource(ir2);
      good = good && dpyr->put_resource(ir3);
      vcl_cout << "Nlevels = " << dpyr->nlevels() << '\n';
    }
    else good = false;
  }//close resource dpyr
  TEST("test pyramid_image_list::put_resource", good, true);
  { //open input pyramid rdpyr
    vil_pyramid_image_resource_sptr rdpyr = vil_load_pyramid_resource(d.c_str());
    TEST("load of pyramid resource rdpyr", !rdpyr, false);
    if (!rdpyr)
      return;
    unsigned nlevels = rdpyr->nlevels();
    vcl_cout << "Read nlevels = " << nlevels << '\n';
    for (unsigned L = 0; L<nlevels; ++L)
      rdpyr->print(L);
    vil_image_view<unsigned short> rv = rdpyr->get_copy_view(2);
    vcl_cout << "single file pyramid level 2\n";
    for (unsigned j = 0; j<2; ++j)
    {
      for (unsigned i = 0; i<2; ++i)
        vcl_cout << rv(i,j) << ' ';

      vcl_cout << '\n';
    }
    TEST("pyramid_image_list--reopen put resources", rv(1,1)==19, true);
  }//close dpyr resources
  //Cleanup pyramid directory
  vl.clean_directory();

  ///Test the tiff pyramid resource with multiple levels in a single file
  vcl_string file = "tiff_pyramid.tif";
  //  create_multi_file_resource(file);
  good = true;
  {//scope for pi
    vil_pyramid_image_resource_sptr pi =
      vil_new_pyramid_image_resource(file.c_str(), "tiff");
    if (pi)
    {
      good = good && pi->put_resource(ir);
      good = good && pi->put_resource(ir2);
      good = good && pi->put_resource(ir3);
      vcl_cout << "Nlevels = " << pi->nlevels() << '\n';
    }
    else good = false;
  }//close resource pi
  TEST("multiimage tiff pyramid write", good, true);
  { //open input pyramid rpi
    vcl_cout << "Load multi-image pyramid\n";
    vil_pyramid_image_resource_sptr rpi = vil_load_pyramid_resource(file.c_str());
    vcl_cout << "loaded multi-image pyramid " << rpi  << '\n';
    unsigned nlevels = rpi->nlevels();
    vcl_cout << "Read nlevels = " << nlevels << '\n';
    for (unsigned L = 0; L<nlevels; ++L)
      rpi->print(L);
    vil_image_view<unsigned short> rv0 = rpi->get_copy_view(0);
    vcl_cout << "single file pyramid level 0\n";
    for (unsigned j = 0; j<2; ++j)
    {
      for (unsigned i = 0; i<2; ++i)
        vcl_cout << rv0(i,j) << ' ';

      vcl_cout << '\n';
    }
    vil_image_view<unsigned short> rv1 = rpi->get_copy_view(1);
    vcl_cout << "single file pyramid level 1\n";
    for (unsigned j = 0; j<2; ++j)
    {
      for (unsigned i = 0; i<2; ++i)
        vcl_cout << rv1(i,j) << ' ';

      vcl_cout << '\n';
    }
    vil_image_view<unsigned short> rv = rpi->get_copy_view(2);
    vcl_cout << "single file pyramid level 2\n";
    for (unsigned j = 0; j<2; ++j)
    {
      for (unsigned i = 0; i<2; ++i)
        vcl_cout << rv(i,j) << ' ';

      vcl_cout << '\n';
    }
    TEST("multiimage tiff pyramid read", rv(1,1)==19, true);
    vil_image_resource_sptr rlev_0= rpi->get_resource(0);
    vil_tiff_image* tlev_0 = (vil_tiff_image*)rlev_0.ptr();
    unsigned nimgs = tlev_0->nimages();
    vil_image_view<unsigned short> vl0 = rlev_0->get_view(1,1,1,1);
    vcl_cout << "view0(0,0) " << vl0(0,0)<<'\n';
    vil_image_resource_sptr rlev_1=rpi->get_resource(1);
    vil_image_view<unsigned short> vl1 = rlev_1->get_view(1,1,1,1);
    vcl_cout << "view1(0,0) " << vl1(0,0)<<'\n';
    vil_image_view<unsigned short> vl0a = rlev_0->get_view(1,1,1,1);
    vcl_cout << "view0a(0,0) " << vl0a(0,0)<<'\n';
    TEST("get multi-image tiff resource", nimgs==3&&vl0(0,0)==74&&vl1(0,0)==37&&vl0a(0,0)==74, true);
  }//close input pyramid rpi
  vpl_unlink(file.c_str());
  vcl_string fb = "tiff_pyramid_from_base.tif";
  {//scope for pyfb
    vil_pyramid_image_resource_sptr pyfb =
      vil_new_pyramid_image_from_base(fb.c_str(), bir, 3, "tiff", d.c_str());
    unsigned nlevels = pyfb->nlevels();
    vcl_cout << "Read nlevels = " << nlevels << '\n';
    for (unsigned L = 0; L<nlevels; ++L)
      pyfb->print(L);
    vil_image_view<unsigned short> rv0 = pyfb->get_copy_view(0);
    vcl_cout << "Level 0 View : ni " << rv0.ni() << '\n';
    vil_image_view<unsigned short> rv1 = pyfb->get_copy_view(1);
    vcl_cout << "Level 1 View : ni " << rv1.ni() << '\n';
    vil_image_view<unsigned short> rv2 = pyfb->get_copy_view(2);
    vcl_cout << "Level 2 View : ni " << rv2.ni() << '\n'
             << "Level 0\n";
    for (unsigned j = 0; j<8; ++j)
    {
      for (unsigned i = 0; i<8; ++i)
        vcl_cout << rv0(i,j) << ' ';

      vcl_cout << '\n';
    }
    vcl_cout << "Level 1\n";
    for (unsigned j = 0; j<8; ++j)
    {
      for (unsigned i = 0; i<8; ++i)
        vcl_cout << rv1(i,j) << ' ';

      vcl_cout << '\n';
    }

    vcl_cout << "Level 2\n";
    for (unsigned j = 0; j<8; ++j)
    {
      for (unsigned i = 0; i<8; ++i)
        vcl_cout << rv2(i,j) << ' ';

      vcl_cout << '\n';
    }
    good = rv0(1,1)==74 && rv1(1,1)==185&&rv2(1,1)==407;
    TEST("multi-image tiff pyramid from base", good, true);
  }//end scope for pyfb
  //remove the base file
  vpl_unlink(fb.c_str());
  //remove the pyramid directory
  vpl_rmdir(d.c_str());
}

TESTMAIN(test_pyramid_image_resource);
