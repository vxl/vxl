//*****************************************************************************
// File name: test_gevd_float_operators.cxx
// Description: Test gevd_float_operators class
//-----------------------------------------------------------------------------
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2003/02/02| Peter Vanroose           |Creation
//*****************************************************************************

#include <gevd/gevd_float_operators.h>
#include <gevd/gevd_bufferxy.h>
#include <vcl_cmath.h> // for sqrt(float)
#include <testlib/testlib_test.h>

void
test_gevd_float_operators()
{
  gevd_bufferxy buf_in(8,8,32), *buf_out=0, *buf_mag=0, *buf_dirx=0, *buf_diry=0;
  gevd_float_operators::Fill(buf_in, 5.0f); *(float*)buf_in.GetElementAddr(1,2) = 3.0f;
  int r = gevd_float_operators::Threshold(buf_in, 4.0f);
  TEST("gevd_float_operators::Threshold()", r, 1);
  gevd_float_operators::Gaussian(buf_in, buf_out);
  TEST_NEAR("gevd_float_operators::Gaussian()", *(float*)buf_out->GetElementAddr(7,7), 5.0, 1e-6);
  gevd_float_operators::Gradient(*buf_out, buf_mag, buf_dirx, buf_diry);
  TEST("gevd_float_operators::Gradient()", *(float*)buf_mag->GetElementAddr(1,3) > 0.7f, true);
  TEST("gevd_float_operators::Gradient()", *(float*)buf_mag->GetElementAddr(5,6) < 1e-5f, true);
  gevd_float_operators::Hessian(*buf_out, buf_mag, buf_dirx, buf_diry);
  TEST("gevd_float_operators::Hessian()", *(float*)buf_mag->GetElementAddr(1,2) > 1.0f, true);
  TEST("gevd_float_operators::Hessian()", *(float*)buf_mag->GetElementAddr(6,5) < 1e-5f, true);
  gevd_float_operators::Laplacian(*buf_out, buf_mag, buf_dirx, buf_diry);
  TEST("gevd_float_operators::Laplacian()", *(float*)buf_mag->GetElementAddr(1,2) > 6.0f, true);
  TEST("gevd_float_operators::Laplacian()", *(float*)buf_mag->GetElementAddr(7,7) < 1e-5f, true);
  float* kernel = 0; int radius;
  gevd_float_operators::Find1dGaussianKernel(7.0f, kernel, radius);
  gevd_bufferxy kernel_buf(2,2,32);
  *(float*)kernel_buf.GetElementAddr(0,0) = 1.0f;
  *(float*)kernel_buf.GetElementAddr(0,1) = -1.0f;
  *(float*)kernel_buf.GetElementAddr(1,0) = -2.0f;
  *(float*)kernel_buf.GetElementAddr(1,1) = 3.0f;
  gevd_float_operators::Convolve(buf_in, kernel_buf, buf_out);
#define TEST_(m,i,j,x) TEST_NEAR("gevd_float_operators::"#m, *(float*)buf_out->GetElementAddr(i,j), x, 1e-7f)
  for (int i=1; i<7; ++i) for (int j=1; j<7; ++j) { // avoid boundaries: undefined behaviour
    if      (i==2 && j==3) TEST_("Convolve", i,j, 0.0f);
    else if (i==2 && j==2) TEST_("Convolve", i,j, 10.0f);
    else if (i==1 && j==2) TEST_("Convolve", i,j, -10.0f);
    else if (i==1 && j==3) TEST_("Convolve", i,j, 15.0f);
    else                   TEST_("Convolve", i,j, 5.0f);
  }
  gevd_float_operators::Correlation(buf_in, kernel_buf, buf_out);
  for (int i=1; i<7; ++i) for (int j=1; j<7; ++j) {
    if      (i==2 && j==3) TEST_("Correlation", i,j, -3.f/vcl_sqrt(177.f));
    else if (i==2 && j==2) TEST_("Correlation", i,j, 5.f/vcl_sqrt(177.f));
    else if (i==1 && j==2) TEST_("Correlation", i,j, -11.f/vcl_sqrt(177.f));
    else if (i==1 && j==3) TEST_("Correlation", i,j, 9.f/vcl_sqrt(177.f));
    else                   TEST_("Correlation", i,j, 0.f/vcl_sqrt(177.f));
  }
  gevd_float_operators::CorrelationAlongAxis(buf_in, kernel_buf, buf_out);
  for (int i=1; i<7; ++i) for (int j=1; j<7; ++j) {
    TEST_("CorrelationAlongAxis", i,j, 0.f);
  }
}

TESTMAIN(test_gevd_float_operators);
