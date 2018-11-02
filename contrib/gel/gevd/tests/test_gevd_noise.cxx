//*****************************************************************************
// File name: test_gevd_noise.cxx
// Description: Test gevd_noise class
//-----------------------------------------------------------------------------
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2003/02/02| Peter Vanroose           |Creation
//*****************************************************************************

#include <iostream>
#include <gevd/gevd_noise.h>
#include <testlib/testlib_test.h>
#include <vnl/vnl_sample.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void
test_gevd_noise()
{
  constexpr int size = 100000;
  vnl_sample_reseed();
  auto* data = new float[size];
  for (int i=0; i<size; ++i) data[i]=(float)vnl_sample_normal(.5,.1);
  gevd_noise noise_estim(data,size); // default # bins (200)
  delete[] data; // "data" should not be needed after constructor is called!
  float sensor_noise, texture_noise;
  bool st = noise_estim.EstimateSensorTexture(sensor_noise, texture_noise);
  TEST("gevd_noise::EstimateSensorTexture()", st, true);
  if (!st) return;
  std::cout << "sensor_noise = " << sensor_noise
           << ", texture_noise = " << texture_noise << std::endl;
  TEST("sensor  noise cannot be negative", sensor_noise >= 0, true);
  TEST("texture noise cannot be negative", texture_noise >= 0, true);
  TEST_NEAR("sensor  noise", sensor_noise,  0.4975, 0.04);
  TEST_NEAR("texture noise", texture_noise, 0.4975, 0.04);
}

TESTMAIN(test_gevd_noise);
