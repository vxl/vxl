#include <iostream>
#include <cmath>
#include <string>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>
#include <bil/algo/bil_image_bounds_from_histogram.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>

static void test_image_bounds_from_histogram()
{
  unsigned ni = 2000, nj = 2000, np = 3;
  vil_image_view<vxl_byte> view(ni,nj);
  view.fill(static_cast<vxl_byte>(0));
  vnl_random rand;
  int low = 50, high = 150;
  for(unsigned j = 0; j<nj; ++j)
    for(unsigned i = 0; i<ni; ++i){
      int v = rand.lrand32(low, high);
      view(i, j) = static_cast<vxl_byte>(v);
    }
  vil_image_resource_sptr imgr = vil_new_image_resource_of_view(view);
  bil_image_bounds_from_histogram ibh(imgr);
  bool good = ibh.construct_histogram();
  double per_lim = 0.0002;
  double lb = ibh.lower_bound(0, per_lim);
  double ub = ibh.upper_bound(0, per_lim);
  std::cout << "bounds(" << lb << ' ' << ub <<")\n";
  TEST("test image bounds on uniform dist", (ub==high)&&(lb==low),true);
  vil_image_view<vxl_byte> view3(ni,nj,np);
  view3.fill(static_cast<vxl_byte>(0));
  std::vector<int> low3(3), high3(3);
  low3[0]=20; low3[1]=40; low3[2] = 50;
  high3[0]=120; high3[1]=140; high3[2] = 150;
  for(unsigned j = 0; j<nj; ++j)
    for(unsigned i = 0; i<ni; ++i)
        for(unsigned p = 0; p<np; ++p){
      int v = rand.lrand32(low3[p], high3[p]);
      view3(i, j,p) = static_cast<vxl_byte>(v);
    }
         vil_image_resource_sptr imgr3 = vil_new_image_resource_of_view(view3);
  bil_image_bounds_from_histogram ibh3(imgr3);
  good = ibh3.construct_histogram();
  double lbsum = 0.0, ubsum = 0.0;
   for(unsigned p = 0; p<np; ++p){
    double lb = ibh3.lower_bound(p, per_lim);
    double ub = ibh3.upper_bound(p, per_lim);
    std::cout << "bounds(" << lb << ' ' << ub <<")\n";
        lbsum += lb;  ubsum += ub;
   }
   TEST("test_image_bounds_color", (lbsum == 110)&&(ubsum == 410), true);

}

TESTMAIN(test_image_bounds_from_histogram);
