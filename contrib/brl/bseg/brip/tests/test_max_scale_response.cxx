#include <vector>
#include <iostream>
#include <iomanip>
#include <testlib/testlib_test.h>
#include <brip/brip_max_scale_response.h>
#include <vil/vil_image_view.h>
#include <vil/vil_resample_bilin.h>

#ifdef DEBUG // I/O only for debugging output
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

#ifdef DEBUG // functions only used in debugging output
static void print_pyramid(std::vector<vil_image_view<float> > const& py)
{
  unsigned levels = py.size();
  for (unsigned level = 0; level<levels; ++level) {
    const vil_image_view<float>& lv = py[level];
      unsigned ni = lv.ni(), nj = lv.nj();
      std::cout << "\n in print pyramid:[ " << level << "]\n";
      for (unsigned j = 0; j<nj; ++j){
        for (unsigned i = 0; i<ni; ++i)
          std::cout << std::setprecision(1) << std::fixed << lv(i,j) << ' ';
        std::cout <<'\n';
      }
  }
}

static void print_pyramid(std::vector<vil_image_view<vxl_byte> > const& mask)
{
  unsigned levels = mask.size();
  for (unsigned level = 0; level<levels; ++level) {
    const vil_image_view<vxl_byte>& m = mask[level];
      std::cout << "Mask at level " << level << '\n';
      for (unsigned j = 0; j<m.nj(); ++j){
        for (unsigned i = 0; i<m.nj(); ++i)
          std::cout << static_cast<unsigned>(m(i,j)) << ' ';
        std::cout <<'\n';
      }
  }
}
#endif // DEBUG

static void test_max_scale_response()
{
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
  std::vector<vil_image_view<float> > pyramid;
  pyramid.push_back(level0);
  pyramid.push_back(level1);
  pyramid.push_back(level2);
  brip_max_scale_response<float> msr(pyramid);
  std::vector<vil_image_view<float> > spyr = msr.scale_pyramid();
  std::vector<vil_image_view<vxl_byte> > mask = msr.mask_pyramid();
#ifdef DEBUG
  std::cout << "trace pyramid large blob\n";
  std::vector<vil_image_view<float> > tr = msr.trace_pyramid();
  print_pyramid(tr);

  std::cout << "Scale pyramid large blob\n";
  print_pyramid(spyr);

  std::cout << "Mask pyramid large blob\n";
  print_pyramid(mask);
#endif // DEBUG
  bool good = spyr[0](15,15)==4.0f;
  good = good && spyr[1](7,7)==4.0f;
  good = good && spyr[2](3,3)==4.0f;
  good = good && spyr[0](4,4)==2.0f;
  good = good && spyr[1](2,2)==2.0f;
  good = good && spyr[2](1,1)==2.0f;
  good = good && !mask[0](15,15)&&!mask[1](7,7)&&mask[2](3,3);
  good = good && !mask[0](4,4)&&mask[1](2,2)&&!mask[2](1,1);
  TEST("Test scale and mask pyramid - large blob", good, true);


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
  std::vector<vil_image_view<float> > pyramid2;
  pyramid2.push_back(level0);
  pyramid2.push_back(level1);
  pyramid2.push_back(level2);
  brip_max_scale_response<float> msr2(pyramid2);
  std::vector<vil_image_view<float> > spyr2 = msr2.scale_pyramid();
  std::vector<vil_image_view<vxl_byte> > mask2 = msr2.mask_pyramid();
  good = spyr2[0](15,15)==1.0;
  good = good&& spyr2[1](8,8)==1.0;
  good = good&& spyr2[2](4,4)==1.0;
  good = good&& mask2[0](15,15)&&!mask2[0](8,8)&&!mask2[0](4,4);
  TEST("Test scale and mask pyramid - human blob", good, true);
#ifdef DEBUG
  std::cout << "Scale pyramid human\n";
  print_pyramid(spyr2);
  std::cout << "Mask pyramid human\n";
  print_pyramid(mask2);
#endif // DEBUG
}

TESTMAIN(test_max_scale_response);
