#include <testlib/testlib_test.h>
#include <vil/vil_print.h>
#include <bil/algo/bil_finite_differences.h>


MAIN( test_bil_finite_differences )
{
  START ("2D Finite Differences");

  int dim = 6;
  vil_image_view<float> testim(dim,dim);
  vil_image_view<float> dxp,dxm,dxc,dyp,dym,dyc;

  testim.fill(1);
  bil_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc);

  float dxpsum=0;
  float dxmsum=0;
  float dxcsum=0;
  float dypsum=0;
  float dymsum=0;
  float dycsum=0;
  for (unsigned j = 0 ; j < testim.nj(); j++){
    for (unsigned i = 0 ; i < testim.ni(); i++){
      dxpsum += dxp(i,j);
      dxmsum += dxm(i,j);
      dxcsum += dxc(i,j);
      dypsum += dyp(i,j);
      dymsum += dym(i,j);
      dycsum += dyc(i,j);
    }
  }

  TEST_NEAR("D_x_plus", dxpsum,0, 0.001);
  TEST_NEAR("D_x_minus", dxmsum,0, 0.001);
  TEST_NEAR("D_x_center", dxcsum,0, 0.001);
  TEST_NEAR("D_y_plus", dypsum,0, 0.001);
  TEST_NEAR("D_y_minus", dymsum,0, 0.001);
  TEST_NEAR("D_y_center", dycsum,0, 0.001);

  for (unsigned j = 0 ; j < testim.nj(); j++){
    for (unsigned i = 0 ; i < testim.ni(); i++){
      if (i < static_cast<unsigned>(dim/2)){
        testim(i,j) = 1;
      }
      else{
        testim(i,j) = 0;
      }
    }
  }

  bil_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc);
#if 0
  vil_print_all(vcl_cout,dxp);
  vil_print_all(vcl_cout,dxm);
  vil_print_all(vcl_cout,dxc);
#endif // 0
  for (unsigned j = 0 ; j < testim.nj(); j++){
    TEST_NEAR("D_x_plus"    ,     dxp(0,j)    ,    0    ,     0.001);
    TEST_NEAR("D_x_minus"   ,     dxm(0,j)    ,    0    ,     0.001);
    TEST_NEAR("D_x_center"  ,     dxc(0,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_plus"    ,     dyp(0,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_minus"   ,     dym(0,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_center"  ,     dyc(0,j)    ,    0    ,     0.001);

    TEST_NEAR("D_x_plus"    ,     dxp(dim/2 - 1,j)    ,   -1    ,     0.001);
    TEST_NEAR("D_x_minus"   ,     dxm(dim/2 - 1,j)    ,    0    ,     0.001);
    TEST_NEAR("D_x_center"  ,     dxc(dim/2 - 1,j)    ,   -0.5  ,     0.001);
    TEST_NEAR("D_y_plus"    ,     dyp(dim/2 - 1,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_minus"   ,     dym(dim/2 - 1,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_center"  ,     dyc(dim/2 - 1,j)    ,    0    ,     0.001);

    TEST_NEAR("D_x_plus"    ,     dxp(dim/2,j)    ,    0    ,     0.001);
    TEST_NEAR("D_x_minus"   ,     dxm(dim/2,j)    ,   -1    ,     0.001);
    TEST_NEAR("D_x_center"  ,     dxc(dim/2,j)    ,   -0.5  ,     0.001);
    TEST_NEAR("D_y_plus"    ,     dyp(dim/2,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_minus"   ,     dym(dim/2,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_center"  ,     dyc(dim/2,j)    ,    0    ,     0.001);
  }

  for (unsigned j = 0 ; j < testim.nj(); j++){
    for (unsigned i = 0 ; i < testim.ni(); i++){
      if (j < static_cast<unsigned>(dim/2)){
        testim(i,j) = 1;
      }
      else{
        testim(i,j) = 0;
      }
    }
  }

  bil_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc);

  /*
  vil_print_all(vcl_cout,dyp);
  vil_print_all(vcl_cout,dym);
  vil_print_all(vcl_cout,dyc);
  */
  for (unsigned i = 0 ; i < testim.ni(); i++){
    TEST_NEAR("D_x_plus"    ,     dxp(i,0)    ,    0    ,     0.001);
    TEST_NEAR("D_x_minus"   ,     dxm(i,0)    ,    0    ,     0.001);
    TEST_NEAR("D_x_center"  ,     dxc(i,0)    ,    0    ,     0.001);
    TEST_NEAR("D_y_plus"    ,     dyp(i,0)    ,    0    ,     0.001);
    TEST_NEAR("D_y_minus"   ,     dym(i,0)    ,    0    ,     0.001);
    TEST_NEAR("D_y_center"  ,     dyc(i,0)    ,    0    ,     0.001);

    TEST_NEAR("D_x_plus"    ,     dxp(i,dim/2 - 1)    ,    0    ,     0.001);
    TEST_NEAR("D_x_minus"   ,     dxm(i,dim/2 - 1)    ,    0    ,     0.001);
    TEST_NEAR("D_x_center"  ,     dxc(i,dim/2 - 1)    ,    0    ,     0.001);
    TEST_NEAR("D_y_plus"    ,     dyp(i,dim/2 - 1)    ,   -1    ,     0.001);
    TEST_NEAR("D_y_minus"   ,     dym(i,dim/2 - 1)    ,    0    ,     0.001);
    TEST_NEAR("D_y_center"  ,     dyc(i,dim/2 - 1)    ,   -0.5  ,     0.001);

    TEST_NEAR("D_x_plus"    ,     dxp(i,dim/2)    ,    0    ,     0.001);
    TEST_NEAR("D_x_minus"   ,     dxm(i,dim/2)    ,    0    ,     0.001);
    TEST_NEAR("D_x_center"  ,     dxc(i,dim/2)    ,    0    ,     0.001);
    TEST_NEAR("D_y_plus"    ,     dyp(i,dim/2)    ,    0    ,     0.001);
    TEST_NEAR("D_y_minus"   ,     dym(i,dim/2)    ,   -1    ,     0.001);
    TEST_NEAR("D_y_center"  ,     dyc(i,dim/2)    ,   -0.5  ,     0.001);
  }


  for (unsigned j = 0 ; j < testim.nj(); j++){
  for (unsigned i = 0 ; i < testim.ni(); i++){
    if (j == testim.nj()-1){
      testim(i,j) = 1;
    }
    else{
      testim(i,j) = 0;
    }
  }
  }

  bil_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc);

  /*
  vil_print_all(vcl_cout,dyp);
  vil_print_all(vcl_cout,dym);
  vil_print_all(vcl_cout,dyc);
  */
  for (unsigned i = 0 ; i < testim.ni(); i++){
    TEST_NEAR("D_x_plus"    ,     dxp(i,0)    ,    0    ,     0.001);
    TEST_NEAR("D_x_minus"   ,     dxm(i,0)    ,    0    ,     0.001);
    TEST_NEAR("D_x_center"  ,     dxc(i,0)    ,    0    ,     0.001);
    TEST_NEAR("D_y_plus"    ,     dyp(i,0)    ,    0    ,     0.001);
    TEST_NEAR("D_y_minus"   ,     dym(i,0)    ,    0    ,     0.001);
    TEST_NEAR("D_y_center"  ,     dyc(i,0)    ,    0    ,     0.001);

    TEST_NEAR("D_x_plus"    ,     dxp(i,testim.nj() - 2)    ,    0    ,     0.001);
    TEST_NEAR("D_x_minus"   ,     dxm(i,testim.nj() - 2)    ,    0    ,     0.001);
    TEST_NEAR("D_x_center"  ,     dxc(i,testim.nj() - 2)    ,    0    ,     0.001);
    TEST_NEAR("D_y_plus"    ,     dyp(i,testim.nj() - 2)    ,    1    ,     0.001);
    TEST_NEAR("D_y_minus"   ,     dym(i,testim.nj() - 2)    ,    0    ,     0.001);
    TEST_NEAR("D_y_center"  ,     dyc(i,testim.nj() - 2)    ,   0.5   ,     0.001);

    TEST_NEAR("D_x_plus"    ,     dxp(i,testim.nj() - 1)    ,    0    ,     0.001);
    TEST_NEAR("D_x_minus"   ,     dxm(i,testim.nj() - 1)    ,    0    ,     0.001);
    TEST_NEAR("D_x_center"  ,     dxc(i,testim.nj() - 1)    ,    0    ,     0.001);
    TEST_NEAR("D_y_plus"    ,     dyp(i,testim.nj() - 1)    ,    0    ,     0.001);
    TEST_NEAR("D_y_minus"   ,     dym(i,testim.nj() - 1)    ,    1    ,     0.001);
    TEST_NEAR("D_y_center"  ,     dyc(i,testim.nj() - 1)    ,   0.5   ,     0.001);
  }

  for (unsigned j = 0 ; j < testim.nj(); j++){
    for (unsigned i = 0 ; i < testim.ni(); i++){
      if (i == testim.ni()-1){
        testim(i,j) = 1;
      }
      else{
        testim(i,j) = 0;
      }
    }
  }

  bil_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc);

#if 0
  vil_print_all(vcl_cout,dxp);
  vil_print_all(vcl_cout,dxm);
  vil_print_all(vcl_cout,dxc);
#endif // 0
  for (unsigned j = 0 ; j < testim.nj(); j++){
    TEST_NEAR("D_x_plus"    ,     dxp(0,j)    ,    0    ,     0.001);
    TEST_NEAR("D_x_minus"   ,     dxm(0,j)    ,    0    ,     0.001);
    TEST_NEAR("D_x_center"  ,     dxc(0,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_plus"    ,     dyp(0,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_minus"   ,     dym(0,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_center"  ,     dyc(0,j)    ,    0    ,     0.001);

    TEST_NEAR("D_x_plus"    ,     dxp(testim.ni() - 2,j)    ,    1    ,     0.001);
    TEST_NEAR("D_x_minus"   ,     dxm(testim.ni() - 2,j)    ,    0    ,     0.001);
    TEST_NEAR("D_x_center"  ,     dxc(testim.ni() - 2,j)    ,   0.5   ,     0.001);
    TEST_NEAR("D_y_plus"    ,     dyp(testim.ni() - 2,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_minus"   ,     dym(testim.ni() - 2,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_center"  ,     dyc(testim.ni() - 2,j)    ,    0    ,     0.001);

    TEST_NEAR("D_x_plus"    ,     dxp(testim.ni() - 1,j)    ,    0    ,     0.001);
    TEST_NEAR("D_x_minus"   ,     dxm(testim.ni() - 1,j)    ,    1    ,     0.001);
    TEST_NEAR("D_x_center"  ,     dxc(testim.ni() - 1,j)    ,   0.5   ,     0.001);
    TEST_NEAR("D_y_plus"    ,     dyp(testim.ni() - 1,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_minus"   ,     dym(testim.ni() - 1,j)    ,    0    ,     0.001);
    TEST_NEAR("D_y_center"  ,     dyc(testim.ni() - 1,j)    ,    0    ,     0.001);
  }

  SUMMARY();
}
