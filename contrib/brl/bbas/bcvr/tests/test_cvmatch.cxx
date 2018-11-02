#include <iostream>
#include <testlib/testlib_test.h>
#include <bcvr/bcvr_cvmatch.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

MAIN_ARGS(test_cvmatch)
{
  std::string dir_base;

  if ( argc >= 2 ) {
    dir_base = argv[1];
    dir_base += "/";
  }
  else
    dir_base = "";

  testlib_test_start("testing curve matching ");

  std::string data_file = "line1.con";
  // Test generic file loads
  bsol_intrinsic_curve_2d_sptr curve_2d1 = new bsol_intrinsic_curve_2d;
  curve_2d1->readCONFromFile( (dir_base+data_file).c_str() );
  TEST("load line1.con ", curve_2d1->size(), 10);

  bsol_intrinsic_curve_2d_sptr curve_2d2 = new bsol_intrinsic_curve_2d(*curve_2d1);
  // Actually testing copy constructor of bsol_intrinsic_curve_2d
  TEST("create curve2 from curve1 ", curve_2d2->size(), 10);

  auto* curveMatch = new bcvr_cvmatch();
  curveMatch->setCurve1 (curve_2d1);
  curveMatch->setCurve2 (curve_2d2);
  curveMatch->Match ();
  std::cout << "Final cost is: " << curveMatch->finalCost() << std::endl;
  TEST("matching curve1 with itself ", curveMatch->finalCost(), 0.0);

  data_file = "line2.con";
  // Test generic file loads
  bsol_intrinsic_curve_2d_sptr curve_2d2_n = new bsol_intrinsic_curve_2d;
  curve_2d2_n->readCONFromFile( (dir_base+data_file).c_str() );
  TEST("load line2.con ", curve_2d2_n->size(), 12);

  auto* curveMatch2 = new bcvr_cvmatch();
  curveMatch2->setCurve1 (curve_2d1);
  curveMatch2->setCurve2 (curve_2d2_n);
  curveMatch2->Match ();
  std::cout << "Final cost is: " << curveMatch2->finalCost() << std::endl;
  TEST_NEAR("matching curve1 with itself ", curveMatch2->finalCost(), 3.28, 0.01);

  return testlib_test_summary();
}
