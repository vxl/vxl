#include <testlib/testlib_test.h>

#if !defined(HAS_VDGL)
void test_DigitalCurve_to_vdgl() { TEST("vdgl was not built",true,true); }
#elif !defined(HAS_NO_SPATIALOBJECTS)
#include <geometry/DigitalCurve_to_vdgl.h>
#include <DigitalGeometry/DigitalCurve.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vsol/vsol_point_2d.h>

void test_DigitalCurve_to_vdgl()
{
  DigitalCurve dc(7);
  float x[] = { 1, 2, 3, 2, 1, 0, -1 };
  float y[] = { 0, 1, 3, 2, 1, 0, -1 };
  dc.SetX(x);
  dc.SetY(y);
  vdgl_digital_curve vdc = DigitalCurve_to_vdgl(dc);
  vsol_point_2d_sptr p0 = vdc.p0();
  TEST("p0()", p0->x()==1 && p0->y()==0, true);
  vsol_point_2d_sptr p1 = vdc.p1();
  TEST("p1()", p1->x()==-1 && p1->y()==-1, true);
  TEST("midpt", vdc.get_x(0.5)==2 && vdc.get_y(0.5)==2, true);
}
#else
void test_DigitalCurve_to_vdgl()
{
  TEST("You do not have the TargetJr SpatialObjects package",true,true);
}
#endif

TESTMAIN(test_DigitalCurve_to_vdgl);
