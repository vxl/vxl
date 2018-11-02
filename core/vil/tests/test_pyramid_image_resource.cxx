// This is core/vil/tests/test_pyramid_image_resource.cxx
// Do not remove the following notice
// Modifications approved for public release, distribution unlimited
// DISTAR Case 14074
//
#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_config.h>
#include <vil/vil_open.h>
#include <vil/vil_save.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_load.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vil/vil_pyramid_image_resource.h>
#include <vil/vil_blocked_image_facade.h>
#include <vil/file_formats/vil_pyramid_image_list.h>
#include <vil/file_formats/vil_tiff.h>
#include <vil/vil_image_list.h>
#include <vul/vul_file.h>
#if HAS_J2K
#include <vil/file_formats/vil_j2k_nitf2_pyramid_image_resource.h>
#include <vil/file_formats/vil_j2k_image.h>
#endif
#if HAS_OPENJPEG2
#include <vil/file_formats/vil_openjpeg_pyramid_image_resource.h>
#include <vil/file_formats/vil_openjpeg.h>
#endif
#define DEBUG

static void test_pyramid_image_resource( int argc, char* argv[] )
{
  std::string image_base; // = "core/vil/tests/file_read_data/";
  // If CMake doesn't pass data path to argv[1]
  if ( argc >= 2 ) {
    image_base = argv[1];
  }else{
    std::string root = testlib_root_dir();
    image_base = root + "/core/vil/tests/file_read_data";
  }
  image_base += "/";
  std::cout << "************************************\n"
           << " Testing vil_pyramid_image_resource\n"
           << "************************************\n";
  //Test Resource
  const unsigned int ni = 73, nj = 43;

  vil_image_view<unsigned short> image;
  image.set_size(ni,nj);
  for (unsigned i = 0; i<ni; ++i)
    for (unsigned j = 0; j<nj; ++j)
      image(i,j) = static_cast<unsigned short>(i + ni*j);
  vil_image_resource_sptr ir = vil_new_image_resource_of_view(image);

  const unsigned ni2 = 36, nj2 = 21;
  vil_image_view<unsigned short> image2;
  image2.set_size(ni2,nj2);
  for (unsigned i = 0; i<ni2; ++i)
    for (unsigned j = 0; j<nj2; ++j)
      image2(i,j) = static_cast<unsigned short>(i + ni2*j);
  vil_image_resource_sptr ir2 = vil_new_image_resource_of_view(image2);

  const unsigned ni3 = 18, nj3 = 10;
  vil_image_view<unsigned short> image3;
  image3.set_size(ni3,nj3);
  for (unsigned i = 0; i<ni3; ++i)
    for (unsigned j = 0; j<nj3; ++j)
      image3(i,j) = static_cast<unsigned short>(i + ni3*j);
  vil_image_resource_sptr ir3 = vil_new_image_resource_of_view(image3);
  std::string d = "pyramid_dir";
  std::cout << "Made pyramid directory "<< d << '\n';
  vul_file::make_directory(d.c_str());
  //Test pyramid_image_list::put_resource(..)
  auto* pir = new vil_pyramid_image_list(d.c_str());
  if (!pir)
    TEST("Make a directory-based pyramid resource", true, false);
  else
  {
    TEST("Make a directory-based pyramid resource", true, true);
    pir->put_resource(ir3);
    for (unsigned i = 0; i<pir->nlevels(); ++i)
      pir->print(i);
    pir->put_resource(ir2);
    std::cout << '\n';
    for (unsigned i = 0; i<pir->nlevels(); ++i)
      pir->print(i);
    pir->put_resource(ir);
    std::cout << '\n';
    for (unsigned i = 0; i<pir->nlevels(); ++i)
      pir->print(i);
    float actual_scale = 1.0f;
    vil_image_view<unsigned short> tview =
      pir->get_copy_view(15,10, 10, 20, 0.6f, actual_scale);
    std::cout << "tview dimensions (" << tview.ni() << ' ' << tview.nj()
             << ") actual scale = "<< actual_scale << '\n';
    TEST_NEAR("actual scale in pyramid image", actual_scale, 0.493, 0.01);
    delete pir;
  }
  //Clean up directory
  vil_image_list vl(d.c_str());
  std::cout << "Starting to clean directory\n";
  vl.clean_directory();
  std::cout << "Cleaned up the pyramid directory\n";
  //Test image decimation. Designed to work with blocked images
  vil_image_resource_sptr bir = new vil_blocked_image_facade(ir, 16, 16);
  //test decimation for generating a pyramid level
  std::string dec_file = "decimated_resource";
  {
    vil_image_resource_sptr dec_resc =
      vil_pyramid_image_resource::decimate(bir, dec_file.c_str());
    if (dec_resc)
      std::cout << "Made a successful decimated resource\n";
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

      std::cout << "base view\n";
      for (unsigned j = 0; j<4; ++j)
      {
        for (unsigned i = 0; i<4; ++i)
          std::cout << image(i,j) << ' ';

        std::cout << '\n';
      }
      std::cout << "decimated view\n";
      for (unsigned j = 0; j<2; ++j)
      {
        for (unsigned i = 0; i<2; ++i)
          std::cout << dec_view(i,j) << ' ';

        std::cout << '\n';
      }
      TEST("decimated image read", dec_view(0,0), 37);
    }
  }//close open resource files
  vpl_unlink(dec_file.c_str());
  std::cout << "Clean up decimated resource file\n";
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
      std::cout << "ni25 = " << ni25 << ",  ni50 = " << ni50 << '\n';
      bool good = (ni25 == 19 && ni50 == 37) || (ni25 == 18 && ni50 == 36);
      TEST("Pyramid read", good, true);
    }
    else
    { TEST("Pyramid create", false, true); }
  }//close open resource files

  //clean directory
  vl.clean_directory();
  std::cout << "Cleaning bpyr directory\n";
  //Test pyramid_image_list::put_resource
  bool good = true;
  {//scope for dpyr
    vil_pyramid_image_resource_sptr dpyr =
      vil_new_pyramid_image_resource(d.c_str(), "pyil");
    if (dpyr)
    {
      good = dpyr->put_resource(ir)
          && dpyr->put_resource(ir2)
          && dpyr->put_resource(ir3);
      std::cout << "Nlevels = " << dpyr->nlevels() << '\n';
    }
    else
      good = false;
  }//close resource dpyr
  TEST("test pyramid_image_list::put_resource", good, true);
  { //open input pyramid rdpyr
    vil_pyramid_image_resource_sptr rdpyr = vil_load_pyramid_resource(d.c_str());
    TEST("load of pyramid resource rdpyr", !rdpyr, false);
    if (!rdpyr)
      return;
    unsigned nlevels = rdpyr->nlevels();
    std::cout << "Read nlevels = " << nlevels << '\n';
    for (unsigned L = 0; L<nlevels; ++L)
      rdpyr->print(L);
    vil_image_view<unsigned short> rv = rdpyr->get_copy_view(2);
    std::cout << "single file pyramid level 2\n";
    for (unsigned j = 0; j<2; ++j)
    {
      for (unsigned i = 0; i<2; ++i)
        std::cout << rv(i,j) << ' ';

      std::cout << '\n';
    }
    TEST("pyramid_image_list--reopen put resources", rv(1,1)==19, true);
  }//close dpyr resources
  //Cleanup pyramid directory
  vl.clean_directory();

  ///Test the tiff pyramid resource with multiple levels in a single file
  std::string file = "tiff_pyramid.tif"; //  create_multi_file_resource(file);
  {//scope for pi
    vil_pyramid_image_resource_sptr pi =
      vil_new_pyramid_image_resource(file.c_str(), "tiff");
    if (pi)
    {
      good = pi->put_resource(ir)
          && pi->put_resource(ir2)
          && pi->put_resource(ir3);
      std::cout << "Nlevels = " << pi->nlevels() << '\n';
    }
    else
      good = false;
  }//close resource pi
  TEST("multiimage tiff pyramid write", good, true);
  { //open input pyramid rpi
    std::cout << "Load multi-image pyramid\n";
    vil_pyramid_image_resource_sptr rpi = vil_load_pyramid_resource(file.c_str());
    std::cout << "loaded multi-image pyramid " << rpi  << '\n';
    unsigned nlevels = rpi->nlevels();
    std::cout << "Read nlevels = " << nlevels << '\n';
    for (unsigned L = 0; L<nlevels; ++L)
      rpi->print(L);
    vil_image_view<unsigned short> rv0 = rpi->get_copy_view(0);
    std::cout << "single file pyramid level 0\n";
    for (unsigned j = 0; j<2; ++j)
    {
      for (unsigned i = 0; i<2; ++i)
        std::cout << rv0(i,j) << ' ';

      std::cout << '\n';
    }
    vil_image_view<unsigned short> rv1 = rpi->get_copy_view(1);
    std::cout << "single file pyramid level 1\n";
    for (unsigned j = 0; j<2; ++j)
    {
      for (unsigned i = 0; i<2; ++i)
        std::cout << rv1(i,j) << ' ';

      std::cout << '\n';
    }
    vil_image_view<unsigned short> rv = rpi->get_copy_view(2);
    std::cout << "single file pyramid level 2\n";
    for (unsigned j = 0; j<2; ++j)
    {
      for (unsigned i = 0; i<2; ++i)
        std::cout << rv(i,j) << ' ';

      std::cout << '\n';
    }
    TEST("multiimage tiff pyramid read", rv(1,1)==19, true);
    vil_image_resource_sptr rlev_0= rpi->get_resource(0);
    auto* tlev_0 = (vil_tiff_image*)rlev_0.ptr();
    unsigned nimgs = tlev_0->nimages();
    vil_image_view<unsigned short> vl0 = rlev_0->get_view(1,1,1,1);
    std::cout << "view0(0,0) " << vl0(0,0)<<'\n';
    vil_image_resource_sptr rlev_1=rpi->get_resource(1);
    vil_image_view<unsigned short> vl1 = rlev_1->get_view(1,1,1,1);
    std::cout << "view1(0,0) " << vl1(0,0)<<'\n';
    vil_image_view<unsigned short> vl0a = rlev_0->get_view(1,1,1,1);
    std::cout << "view0a(0,0) " << vl0a(0,0)<<'\n';
    TEST("get multi-image tiff resource", nimgs==3&&vl0(0,0)==74&&vl1(0,0)==37&&vl0a(0,0)==74, true);
  }//close input pyramid rpi
  vpl_unlink(file.c_str());
  std::string fb = "tiff_pyramid_from_base.tif";
  {//scope for pyfb
    vil_pyramid_image_resource_sptr pyfb =
      vil_new_pyramid_image_from_base(fb.c_str(), bir, 3, "tiff", d.c_str());
    unsigned nlevels = pyfb->nlevels();
    std::cout << "Read nlevels = " << nlevels << '\n';
    for (unsigned L = 0; L<nlevels; ++L)
      pyfb->print(L);
    vil_image_view<unsigned short> rv0 = pyfb->get_copy_view(0);
    std::cout << "Level 0 View : ni " << rv0.ni() << '\n';
    vil_image_view<unsigned short> rv1 = pyfb->get_copy_view(1);
    std::cout << "Level 1 View : ni " << rv1.ni() << '\n';
    vil_image_view<unsigned short> rv2 = pyfb->get_copy_view(2);
    std::cout << "Level 2 View : ni " << rv2.ni() << '\n'
             << "Level 0\n";
    for (unsigned j = 0; j<8; ++j)
    {
      for (unsigned i = 0; i<8; ++i)
        std::cout << rv0(i,j) << ' ';

      std::cout << '\n';
    }
    std::cout << "Level 1\n";
    for (unsigned j = 0; j<8; ++j)
    {
      for (unsigned i = 0; i<8; ++i)
        std::cout << rv1(i,j) << ' ';

      std::cout << '\n';
    }

    std::cout << "Level 2\n";
    for (unsigned j = 0; j<8; ++j)
    {
      for (unsigned i = 0; i<8; ++i)
        std::cout << rv2(i,j) << ' ';

      std::cout << '\n';
    }
    good = rv0(1,1)==74 && rv1(1,1)==185&&rv2(1,1)==407;
    TEST("multi-image tiff pyramid from base", good, true);
  }//end scope for pyfb
  //remove the base file
  vpl_unlink(fb.c_str());
  //remove the pyramid directory
  vpl_rmdir(d.c_str());
  //
  //------- Test J2K NITF image pyramid resource ------------------//
  //
#if HAS_J2K
  good = true;
  std::string filepath = image_base+"p0_12a.ntf";
  vil_image_resource_sptr resc = vil_load_image_resource(filepath.c_str());
  if (resc) {
    vil_nitf2_image* nitf2 = static_cast<vil_nitf2_image*>(resc.ptr());
    good = nitf2&& nitf2->is_jpeg_2000_compressed();
    vil_j2k_nitf2_pyramid_image_resource* j2k_nitf =
      new vil_j2k_nitf2_pyramid_image_resource(resc);
    unsigned n_i = j2k_nitf->ni(), n_j = j2k_nitf->nj();
    unsigned nlevels = j2k_nitf->nlevels();
    good = good && n_i == 3 && n_j == 5 && nlevels == 1;
    vil_image_view_base_sptr view = j2k_nitf->get_copy_view(0);
    good = good && view && view->pixel_format()==VIL_PIXEL_FORMAT_BYTE;
    TEST("NITF J2K pyramid resource", good, true);
  }
  else
  { TEST("NITF J2K pyramid resource", false, true); }
  //test compression
  // make a test image
  unsigned nic=131, njc=151;//odd dimensions
  vil_image_view<unsigned short> comp_view(nic, njc);
  for (unsigned j =0; j<njc; ++j)
    for (unsigned i =0; i<nic; ++i)
      comp_view(i,j) = static_cast<unsigned short>(i+j);
  std::string comp_file = "compress.j2k";
  std::string uncomp_file = "orig.tiff";
  std::string long_comp_file = "long_comp.j2k";
  {
    vil_stream* vs = vil_open(comp_file.c_str(), "w");
    vs->ref();
    vil_j2k_file_format fmt;
    fmt.set_compression_ratio(1);
    vil_image_resource_sptr cres =
      fmt.make_output_image(vs, nic, njc, 1,
                            VIL_PIXEL_FORMAT_UINT_16);
    good = cres->put_view(comp_view);
    vs->unref();
    vil_image_resource_sptr reload_res = vil_load_image_resource(comp_file.c_str());
    vil_image_view<unsigned short> reload_view =
      *reinterpret_cast<vil_image_view<unsigned short>* > (reload_res->get_view().ptr());
    for (unsigned j =0; j<njc&&good; ++j)
      for (unsigned i =0; i<nic&&good; ++i)
        good = reload_view(i,j) == comp_view(i,j);
    TEST("J2K view compression", good, true);

    nic = 40; njc =1500;
    vil_image_view<unsigned short> long_view(nic, njc);
    for (unsigned j =0; j<njc; ++j)
      for (unsigned i =0; i<nic; ++i)
        long_view(i,j) = static_cast<unsigned short>(i+j);

    good = vil_save(long_view, uncomp_file.c_str())
        && vil_j2k_image::s_encode_jpeg2000(uncomp_file.c_str(),
                                            long_comp_file.c_str());

    vil_image_resource_sptr reload_long_res = vil_load_image_resource(long_comp_file.c_str());
    vil_image_view<unsigned short> reload_long_view =
    *reinterpret_cast<vil_image_view<unsigned short>* > (reload_long_res->get_view().ptr());
    for (unsigned j =0; j<njc&&good; ++j)
      for (unsigned i =0; i<nic&&good; ++i)
        good = reload_long_view(i,j) == long_view(i,j);
    TEST("J2K line block compression", good, true);
  }
  vpl_unlink(comp_file.c_str());
  vpl_unlink(uncomp_file.c_str());
  vpl_unlink(long_comp_file.c_str());
#endif //HAS_J2K

  //
  //------- Test OpenJPEG image pyramid resource ------------------//
  //
#if HAS_OPENJPEG2
  std::string filepath_open_jpg2 = image_base+"jpeg2000/file1.jp2";
  vil_image_resource_sptr resc_open_jpeg2 = vil_load_image_resource(filepath_open_jpg2.c_str());
  if (resc_open_jpeg2)
  {
    auto* j2k = static_cast<vil_openjpeg_image*>(resc_open_jpeg2.ptr());
    good = j2k && j2k->is_valid();
    auto* j2k_pyr =
      new vil_openjpeg_pyramid_image_resource(resc_open_jpeg2);
    unsigned n_i = j2k_pyr->ni(), n_j = j2k_pyr->nj();
    unsigned nlevels = j2k_pyr->nlevels();
    good = good && n_i == 768 && n_j == 512 && nlevels == 6;
    vil_image_view_base_sptr view = j2k_pyr->get_copy_view(0);
    good = good && view && view->pixel_format()==VIL_PIXEL_FORMAT_BYTE;
    view = j2k_pyr->get_copy_view(1);
    good = good && view->ni() == 384 && view->nj() == 256;
    view = j2k_pyr->get_copy_view(2);
    good = good && view->ni() == 192 && view->nj() == 128;
    view = j2k_pyr->get_copy_view(3);
    good = good && view->ni() == 96 && view->nj() == 64;
    view = j2k_pyr->get_copy_view(4);
    good = good && view->ni() == 48 && view->nj() == 32;
    view = j2k_pyr->get_copy_view(5);
    good = good && view->ni() == 24 && view->nj() == 16;
    TEST("OpenJPEG pyramid resource", good, true);
  }
  else
  {
    TEST("OpenJPEG pyramid resource", false, true);
  }
#endif //HAS_OPENJPEG
}

TESTMAIN_ARGS(test_pyramid_image_resource);
