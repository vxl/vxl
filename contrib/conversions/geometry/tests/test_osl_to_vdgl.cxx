#include <testlib/testlib_test.h>

#if defined(HAS_OSL) && defined(HAS_VDGL)
#include <geometry/osl_to_vdgl.h>
#include <osl/osl_edgel_chain.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vsol/vsol_point_2d.h>

void test_osl_to_vdgl() {
  osl_edgel_chain dc(7);
  float x[] = { 1, 2, 3, 2, 1, 0, -1 };
  float y[] = { 0, 1, 3, 2, 1, 0, -1 };
  for (int i=0; i<7; ++i) {
    dc.SetX(x[i], i);
    dc.SetY(y[i], i);
  }
  vdgl_digital_curve vdc = osl_to_vdgl(dc);
  vsol_point_2d_sptr p0 = vdc.p0();
  TEST("p0()", p0->x()==1 && p0->y()==0, true);
  vsol_point_2d_sptr p1 = vdc.p1();
  TEST("p1()", p1->x()==-1 && p1->y()==-1, true);
  TEST("midpt", vdc.get_x(0.5)==2 && vdc.get_y(0.5)==2, true);
}
#else
void test_osl_to_vdgl() { TEST("osl or vdgl was not built",true,true); }
#endif

TESTMAIN(test_osl_to_vdgl);
