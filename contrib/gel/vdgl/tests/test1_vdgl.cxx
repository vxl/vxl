#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_fit_line.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vdgl/vdgl_edgel.h>
#include <testlib/testlib_test.h>

static void test1_vdgl()
{
  // Create an edgel chain...
  vdgl_edgel_chain e;

  // ...with some edgels (x,y,gamma,theta)
  vdgl_edgel e1( 1,2,3,4);
  vdgl_edgel e2( 2,6,7,8);
  vdgl_edgel e3( 6,9,3,2);
  vdgl_edgel e4( 4,5,3,4);

  e.add_edgel( e1);
  e.add_edgel( e2);
  e.add_edgel( e3);
  e.add_edgel( e4);

  vsol_line_2d_sptr myline;
  myline = vdgl_fit_line(e);

  //Here check for the length of the line

  TEST_NEAR("Length", myline->length(), 3.482142857142857, 1e-12);
  //--------------------------------------------------
  // test the generation of a straight edgel_chain defined by two points
  //
  double x0=0, y0=0, x1=10, y1=10;
  vdgl_edgel_chain_sptr ec = new vdgl_edgel_chain(x0, y0, x1, y1);
  unsigned int N = ec->size();
  TEST("Chain should have 11 edgels", N, 11);
  for (unsigned int i = 0; i<N; i++)
    vcl_cout << "edgel[" << i<<"] = (" << (*ec)[i] << ")\n";

  //--------------------------------------------------
  // test the angle computation for the linear interpolator
  //
  vsol_point_2d_sptr p0 = new vsol_point_2d(0,0);
  vsol_point_2d_sptr p1 = new vsol_point_2d(5,5);
  vdgl_digital_curve_sptr dc = new vdgl_digital_curve(p0, p1);
  vdgl_interpolator_sptr intrp = dc->get_interpolator();
  int Nc = intrp->get_edgel_chain()->size();
  for (double t = 0.1; t+1<Nc; t+=1.0)
    { TEST_NEAR("Slope in intermediate point", intrp->get_tangent_angle(t), 45.0, 1e-8); }
}

TESTMAIN(test1_vdgl);
