// This is core/vil/algo/tests/test_algo_convolve_1d.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_new.h>
#include <vil/vil_crop.h>
#include <vil/algo/vil_convolve_1d.h>

static void test_algo_convolve_1d_double()
{
  vcl_cout << "*************************\n"
           << " Testing vil_convolve_1d\n"
           << "*************************\n";

  const int n = 10;
  vcl_vector<double> src(n), dest(n+2), kernel(3);
  for (int i=0;i<n;++i) src[i]=i+1;
  for (int i=0;i<3;++i) kernel[i]=i+1;

  // Note: In the following dest[1]..dest[n] should be valid.
  // dest[0] and dest[n+1] should be untouched (set to 999).
  // They are included to test for over-runs.

  vil_convolve_1d(&src[0],n,1, &dest[1],1,
                  &kernel[1],-1,1,
                  double(), // indicates accumulator type
                  vil_convolve_no_extend,vil_convolve_no_extend);
  TEST_NEAR("First full value",dest[2],10.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-8.0,1e-6);

  vcl_cout<<"Testing vil_convolve_ignore_edge end type\n";
  for (int i=0;i<n+2;++i) dest[i]=999;

  vil_convolve_1d(&src[0],n,1, &dest[1],1,
                  &kernel[1],-1,1,
                  double(), // indicates accumulator type
                  vil_convolve_ignore_edge,vil_convolve_ignore_edge);

  TEST_NEAR("Start",dest[1],999,1e-6);
  TEST_NEAR("First full value",dest[2],10.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-8.0,1e-6);
  TEST_NEAR("End",dest[n],999,1e-6);
  TEST_NEAR("No overrun start",dest[0],999,1e-6);
  TEST_NEAR("No overrun end",dest[n+1],999,1e-6);

  vcl_cout<<"Testing vil_convolve_no_extend end type\n";
  for (int i=0;i<n+2;++i) dest[i]=999;

  vil_convolve_1d(&src[0],n,1, &dest[1],1,
                  &kernel[1],-1,1,
                  double(), // indicates accumulator type
                  vil_convolve_no_extend,vil_convolve_no_extend);

  TEST_NEAR("Start",dest[1],0,1e-6);
  TEST_NEAR("First full value",dest[2],10.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-8.0,1e-6);
  TEST_NEAR("End",dest[n],0,1e-6);
  TEST_NEAR("No overrun start",dest[0],999,1e-6);
  TEST_NEAR("No overrun end",dest[n+1],999,1e-6);


  vcl_cout<<"Testing vil_convolve_zero_extend end type\n";
  for (int i=0;i<n+2;++i) dest[i]=999;
  vil_convolve_1d(&src[0],n,1, &dest[1],1,
                  &kernel[1],-1,1,
                  double(), // indicates accumulator type
                  vil_convolve_zero_extend,vil_convolve_zero_extend);

  TEST_NEAR("Start",dest[1],4,1e-6);
  TEST_NEAR("First full value",dest[2],10.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-8.0,1e-6);
  TEST_NEAR("End",dest[n],5*n-3,1e-6);
  TEST_NEAR("No overrun start",dest[0],999,1e-6);
  TEST_NEAR("No overrun end",dest[n+1],999,1e-6);

  vcl_cout<<"Testing vil_convolve_constant_extend end type\n";
  for (int i=0;i<n+2;++i) dest[i]=999;
  vil_convolve_1d(&src[0],n,1, &dest[1],1,
                  &kernel[1],-1,1,
                  double(), // indicates accumulator type
                  vil_convolve_constant_extend,vil_convolve_constant_extend);

  TEST_NEAR("Start",dest[1],7.0,1e-6);
  TEST_NEAR("First full value",dest[2],10.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-8.0,1e-6);
  TEST_NEAR("End",dest[n],6.0*n-3.0,1e-6);
  TEST_NEAR("No overrun start",dest[0],999,1e-6);
  TEST_NEAR("No overrun end",dest[n+1],999,1e-6);

  vcl_cout<<"Testing vil_convolve_reflect_extend end type\n";
  for (int i=0;i<n+2;++i) dest[i]=999;
  vil_convolve_1d(&src[0],n,1, &dest[1],1,
                  &kernel[1],-1,1,
                  double(), // indicates accumulator type
                  vil_convolve_reflect_extend,vil_convolve_reflect_extend);

  TEST_NEAR("Start",dest[1],10.0,1e-6);
  TEST_NEAR("First full value",dest[2],10.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6.0*n-8.0,1e-6);
  TEST_NEAR("End",dest[n],6.0*n-4.0,1e-6);
  TEST_NEAR("No overrun start",dest[0],999,1e-6);
  TEST_NEAR("No overrun end",dest[n+1],999,1e-6);

  vcl_cout<<"Testing vil_convolve_periodic_extend end type\n";
  for (int i=0;i<n+2;++i) dest[i]=999;
  vil_convolve_1d(&src[0],n,1, &dest[1],1,
                  &kernel[1],-1,1,
                  double(), // indicates accumulator type
                  vil_convolve_periodic_extend,vil_convolve_periodic_extend);

  TEST_NEAR("Start",dest[1],3.0*n+4.0,1e-6);
  TEST_NEAR("First full value",dest[2],10.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-8.0,1e-6);
  TEST_NEAR("End",dest[n],5.0*n-2.0,1e-6);
  TEST_NEAR("No overrun start",dest[0],999,1e-6);
  TEST_NEAR("No overrun end",dest[n+1],999,1e-6);

  vcl_cout<<"Testing vil_convolve_trim end type\n";
  for (int i=0;i<n+2;++i) dest[i]=999;
  vil_convolve_1d(&src[0],n,1, &dest[1],1,
                  &kernel[1],-1,1,
                  double(), // indicates accumulator type
                  vil_convolve_trim,vil_convolve_trim);

  TEST_NEAR("Start",dest[1],8.0,1e-6);
  TEST_NEAR("First full value",dest[2],10.0,1e-6);
  TEST_NEAR("Last full value",dest[n-1],6*n-8.0,1e-6);
  TEST_NEAR("End",dest[n],6*n-18.0/5.0,1e-6);
  TEST_NEAR("No overrun start",dest[0],999,1e-6);
  TEST_NEAR("No overrun end",dest[n+1],999,1e-6);

  vcl_cout << "\n\nvil_convolve_1d(vil_image_resource_sptr&,...)\n";

  vil_image_resource_sptr mem = vil_new_image_resource(n,n,1,VIL_PIXEL_FORMAT_BYTE);
  vil_image_view<vxl_byte> v(n,n,1), v_out(n,n,1);
  for (int j=0; j<n; ++j)
    for (int i=0; i<n; ++i)
      v(i,j) = i+1;

  TEST ("memory image.put_view()", mem->put_view(v,0,0), true);

  // set up a convolved image_resource object
  vil_image_resource_sptr conv =
    vil_convolve_1d(mem, vxl_byte(), &kernel[1],-1,1, int(),
                    vil_convolve_constant_extend, vil_convolve_zero_extend);

  //set up a convolved view.
  vil_convolve_1d(v, v_out, &kernel[1], -1, 1, int(),
                  vil_convolve_constant_extend, vil_convolve_zero_extend);

  // check they are equal in various regions..
  TEST("convolved resource.get_view() == convolved view.window() top-left corner",
       vil_image_view_deep_equality(vil_crop(v_out,0,4,0,4),
                                    vil_image_view<vxl_byte>(conv->get_view(0,4,0,4))), true);
  TEST("convolved resource.get_view() == convolved view.window() centre",
       vil_image_view_deep_equality(vil_crop(v_out,3,4,3,4),
                                    vil_image_view<vxl_byte>(conv->get_view(3,4,3,4))), true);
  TEST("convolved resource.get_view() == convolved view.window() bottom-right corner",
       vil_image_view_deep_equality(vil_crop(v_out,n-4,4,n-4,4),
                                    vil_image_view<vxl_byte>(conv->get_view(n-4,4,n-4,4))), true);
}

static void test_algo_convolve_1d()
{
  test_algo_convolve_1d_double();
}

TESTMAIN(test_algo_convolve_1d);
