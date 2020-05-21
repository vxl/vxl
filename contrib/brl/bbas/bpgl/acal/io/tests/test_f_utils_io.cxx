// acal/io/tests/test_f_utils_io.cxx
#include "test_generic_io.h"
#include <acal/io/acal_io_f_utils.h>

static void
test_f_utils_io()
{
  // f_params io test
  f_params fp;
  fp.epi_dist_mul_ = 3.5;
  fp.max_epi_dist_ = 6.0;
  fp.F_similar_abcd_tol_ = 0.05;
  fp.F_similar_e_tol_ = 2.0;
  fp.ray_uncertainty_tol_ = 60.0;
  fp.min_num_matches_ = 10;

  test_generic_io(fp, "f_params");

}

TESTMAIN(test_f_utils_io);
