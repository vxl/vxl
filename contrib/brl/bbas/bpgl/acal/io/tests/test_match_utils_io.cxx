// acal/io/tests/test_match_utils_io.cxx
#include "test_generic_io.h"
#include <acal/io/acal_io_match_utils.h>

static void
test_match_utils_io()
{
  // acal_corr io test
  acal_corr ac(818, vgl_point_2d<double>(188.987, 227.430));
  test_generic_io(ac, "acal_corr");

  // acal_match_pair io test
  acal_corr ac_a( 818, vgl_point_2d<double>(188.987, 227.430));
  acal_corr ac_b(1617, vgl_point_2d<double>(278.163, 315.765));
  acal_match_pair mp(ac_a, ac_b);
  test_generic_io(mp, "acal_match_pair");

}

TESTMAIN(test_match_utils_io);
