// This is brl/bbas/bdgl/tests/test_region_algs.cxx
#include <vcl_iostream.h>
#include <vdgl/vdgl_digital_region.h>
#include <bdgl/bdgl_region_algs.h>
#include <testlib/testlib_test.h>

static void test_region_algs()
{
  // we want to test the methods on bdgl_region_algs
  vcl_cout << "Testing region merge\n";
  const unsigned int n1 = 10, n2 = 20;
  float* X1 = new float[n1];
  float* Y1 = new float[n1];
  unsigned short * I1 = new unsigned short[n1];
  for (unsigned int i = 0; i<n1; i++)
  {
    X1[i] = 1.0f*i; Y1[i] = 2.0f*i; I1[i]= 10*i;
  }

  vdgl_digital_region_sptr r1 = new vdgl_digital_region(n1, X1, Y1, I1);
  delete[] X1;
  delete[] Y1;
  delete[] I1;

  float* X2 = new float[n2];
  float* Y2 = new float[n2];
  unsigned short * I2 = new unsigned short[n2];
  for (unsigned int i = 0; i<n2; i++)
  {
    X2[i] = 3.0f*i; Y2[i] = -6.0f*i; I2[i]= 100*i;
  }
  vdgl_digital_region_sptr r2 = new vdgl_digital_region(n2, X2, Y2, I2);
  delete[] X2;
  delete[] Y2;
  delete[] I2;

  vdgl_digital_region_sptr rm;
  bdgl_region_algs::merge(r1, r2, rm);

  unsigned int n = n1 + n2;
  TEST("bdgl_region_algs::merge(): num points", rm->Npix(), n);

  for (rm->reset(); rm->next();)
    vcl_cout << "P(" << rm->X() << ' ' << rm->Y() << ' ' << rm->I() << ")\n";

  rm->reset(); rm->next();
  for (r1->reset(); r1->next(); rm->next())
  {
    TEST("bdgl_region_algs::merge(): X value", rm->X(), r1->X());
    TEST("bdgl_region_algs::merge(): Y value", rm->Y(), r1->Y());
    TEST("bdgl_region_algs::merge(): I value", rm->I(), r1->I());
  }
  for (r2->reset(); r2->next(); rm->next())
  {
    TEST("bdgl_region_algs::merge(): X value", rm->X(), r2->X());
    TEST("bdgl_region_algs::merge(): Y value", rm->Y(), r2->Y());
    TEST("bdgl_region_algs::merge(): I value", rm->I(), r2->I());
  }
  //TEST("Num Intersection points = 1", pts.size(), 1);
}

TESTMAIN(test_region_algs);
