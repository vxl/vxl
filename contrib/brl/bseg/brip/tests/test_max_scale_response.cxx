#include <testlib/testlib_test.h>
#include <brip/brip_max_scale_response.h>
#include <vil/vil_image_view.h>
#include <vil/vil_resample_bilin.h>
#include <vcl_vector.h>

#ifdef DEBUG // I/O only for debugging output
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#endif

#ifdef DEBUG // functions only used in debugging output
static void print_pyramid(vcl_vector<vil_image_view<float> > const& py)
{
  unsigned levels = py.size();
  for (unsigned level = 0; level<levels; ++level) {
    const vil_image_view<float>& lv = py[level];
      unsigned ni = lv.ni(), nj = lv.nj();
      vcl_cout << "\n in print pyramid:[ " << level << "]\n";
      for (unsigned j = 0; j<nj; ++j){
        for (unsigned i = 0; i<ni; ++i)
          vcl_cout << vcl_setprecision(1) << vcl_fixed << lv(i,j) << ' ';
        vcl_cout <<'\n';
      }
  }
}

static void print_pyramid(vcl_vector<vil_image_view<vxl_byte> > const& mask)
{
  unsigned levels = mask.size();
  for (unsigned level = 0; level<levels; ++level) {
    const vil_image_view<vxl_byte>& m = mask[level];
      vcl_cout << "Mask at level " << level << '\n';
      for (unsigned j = 0; j<m.nj(); ++j){
        for (unsigned i = 0; i<m.nj(); ++i)
          vcl_cout << static_cast<unsigned>(m(i,j)) << ' ';
        vcl_cout <<'\n';
      }
  }
}
#endif // DEBUG

static void test_max_scale_response()
{
  START("max_scale_response");
  vil_image_view<float> level0(32,32);
  level0.fill(0.0f);
  // fine scale
  // coarser scale blob
  for (unsigned j = 5; j<25; ++j)
    for (unsigned i = 5; i<25; ++i)
      level0(i,j) = 1.0f;
  // construct several levels
  vil_image_view<float> level1, level2;
  vil_resample_bilin(level0, level1, 16, 16);
  vil_resample_bilin(level1, level2, 8, 8);
  vcl_vector<vil_image_view<float> > pyramid;
  pyramid.push_back(level0);
  pyramid.push_back(level1);
  pyramid.push_back(level2);
  brip_max_scale_response<float> msr(pyramid);
#ifdef DEBUG
  vcl_cout << "trace pyramid large blob\n";
  vcl_vector<vil_image_view<float> > tr = msr.trace_pyramid();
  print_pyramid(tr);

  vcl_cout << "Scale pyramid large blob\n";
  vcl_vector<vil_image_view<float> > spyr = msr.scale_pyramid();
  print_pyramid(spyr);

  vcl_cout << "Mask pyramid large blob\n";
  vcl_vector<vil_image_view<vxl_byte> > mask = msr.mask_pyramid();
  print_pyramid(mask);
#endif // DEBUG

  level0.fill(0.0f);
  for (unsigned j = 14; j<18; ++j)
    for (unsigned i = 14; i<18; ++i)
      level0(i,j) = 1.0f;
  vil_resample_bilin(level0, level1, 16, 16);
  vil_resample_bilin(level1, level2, 8, 8);
  vcl_vector<vil_image_view<float> > pyramid1;
  pyramid1.push_back(level0);
  pyramid1.push_back(level1);
  pyramid1.push_back(level2);
  brip_max_scale_response<float> msr1(pyramid1);

  vcl_vector<vil_image_view<float> > spyr1 = msr1.scale_pyramid();
  vcl_vector<vil_image_view<vxl_byte> > mask1 = msr1.mask_pyramid();
#ifdef DEBUG
  vcl_cout << "Scale pyramid small blob\n";
  print_pyramid(spyr1);

  vcl_cout << "Mask pyramid small blob\n";
  print_pyramid(mask1);
#endif // DEBUG
  bool good = mask1[0](15,15)&&!mask1[1](7,7)&&!mask1[2](3,3);
  TEST("Test mask pyramid", good, true);
  level0.fill(1.0f);
                     level0(14,12)=0.5f;level0(15,13)=0.0f;level0(16,12)=0.5f;
  level0(13,13)=0.5f;level0(14,13)=0.0f;level0(15,13)=0.0f;level0(15,13)=0.5f;
  level0(13,14)=0.5f;level0(14,14)=0.0f;level0(15,14)=0.0f;level0(15,14)=0.5f;
  level0(13,15)=0.5f;level0(14,15)=0.0f;level0(15,15)=0.0f;level0(15,15)=0.5f;
  level0(13,16)=0.5f;level0(14,16)=0.0f;level0(15,16)=0.0f;level0(15,16)=0.5f;
  level0(13,17)=0.5f;level0(14,17)=0.0f;level0(15,17)=0.0f;level0(15,17)=0.5f;
                     level0(14,18)=0.5f;level0(15,18)=0.0f;level0(16,18)=0.5f;
  vil_resample_bilin(level0, level1, 16, 16);
  vil_resample_bilin(level1, level2, 8, 8);
  vcl_vector<vil_image_view<float> > pyramid2;
  pyramid2.push_back(level0);
  pyramid2.push_back(level1);
  pyramid2.push_back(level2);
  brip_max_scale_response<float> msr2(pyramid2);

#ifdef DEBUG
  vcl_vector<vil_image_view<float> > spyr2 = msr2.scale_pyramid();
  vcl_cout << "Scale pyramid human\n";
  print_pyramid(spyr2);
  vcl_vector<vil_image_view<vxl_byte> > mask2 = msr2.mask_pyramid();
  vcl_cout << "Mask pyramid human\n";
  print_pyramid(mask2);
#endif // DEBUG
}

TESTMAIN(test_max_scale_response);
