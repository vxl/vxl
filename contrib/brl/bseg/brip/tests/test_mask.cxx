#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <brip/brip_rect_mask.h>

static void test_mask()
{
  brip_rect_mask m10(brip_rect_mask::r10);
  brip_rect_mask m30(brip_rect_mask::r30);
  brip_rect_mask m31(brip_rect_mask::r31);
  brip_rect_mask m32(brip_rect_mask::r32);
  brip_rect_mask m51(brip_rect_mask::r51);
  brip_rect_mask m52(brip_rect_mask::r52);
  brip_rect_mask c4_90_0(brip_rect_mask::c4_90_0);
  brip_rect_mask c4_45_0(brip_rect_mask::c4_45_0);
  brip_rect_mask c4_45_45(brip_rect_mask::c4_45_45);
  brip_rect_mask e5(brip_rect_mask::e5);
  std::cout << "m10\n" << m10 << '\n'
           << "m30\n" << m30 << '\n'
           << "m31\n" << m31 << '\n'
           << "m32\n" << m32 << '\n'
           << "m51\n" << m51 << '\n'
           << "m52\n" << m52 << '\n'
           << "c4_90_0\n" << c4_90_0 << '\n'
           << "c4_45_0\n" << c4_45_0 << '\n'
           << "c4_45_45\n" << c4_45_45 << '\n'
           << "e5\n" << e5 << '\n';
  int m10_c = m10(0,0), m30_c = m30(0,0);
  int m31_c = m31(0,0), m32_c = m32(0,0);
  int m51_c = m51(0,0), m52_c = m52(0,0);
  int c4_90_0_c = c4_90_0(0,0), c4_45_0_c = c4_45_0(0,0);
  int c4_45_45_c = c4_45_45(0,0), e5_c = e5(0,0);
  std::cout << m10_c << ' '<<  m30_c << ' ' << m31_c << ' ' << m32_c << ' '
           << m51_c << ' ' << m52_c << ' ' <<  c4_90_0_c << ' '
           << c4_45_0_c << ' ' << c4_45_45_c << ' ' << e5_c << '\n';
  int prod = m10_c * m30_c * m31_c * m32_c * m51_c * m52_c * c4_90_0_c *
    c4_45_0_c * c4_45_45_c * e5_c;
  TEST("brip_rect_mask (center product) ", prod, 1);
}

TESTMAIN(test_mask);
