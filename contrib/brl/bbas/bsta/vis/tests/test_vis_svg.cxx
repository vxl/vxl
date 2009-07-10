// This is contrib/brl/bbas/bsta/vis/tests/test_vis_svg.cxx
//:
// \file
// \brief Various tests for visualization of bsta histograms using SVG library
// \author Ozge C. Ozcanli (Brown)
// \date   April 22, 2009
//
#include <testlib/testlib_test.h>
#include <bsta/vis/bsta_svg_tools.h>

static void test_vis_svg()
{
  double range = 128.0;
  int bins = 16;
  double delta = range/bins;
  bsta_histogram<double> h(range, bins);
  double v = 0.0;
  for (int b =0; b<bins; b++, v+=delta)
    h.upcount(v, 1.0);

  write_svg<double>(h, "test.svg");
}

TESTMAIN( test_vis_svg );
