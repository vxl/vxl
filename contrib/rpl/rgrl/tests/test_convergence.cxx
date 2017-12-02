#include <testlib/testlib_test.h>

void test_convergence()
{
  // there should be a test to make sure that all convergence objects converge
  // when we are registering identical data sets
  //
  // currently, there is a chunk of code in each of
  //   rgrl_convergence_on_median_error
  // and
  //    rgrl_convergence_on_weighted_error {.cxx}
  // that looks like:
  // \code
  //   bool converged = false;
  //   if (prev_status && current_view.regions_converged(prev_view) ) {
  //     ...
  //     converged = std::abs( (new_error-old_error) / new_error ) < 1e-3;
  //     ... }
  // \endcode
  // one problem that I see is: converged will always be false after the first iteration
  // is this really a warranted assumption?
  // although the error would most likely never be == to 0.0f on a real data set,
  // i could imagine a threshold incorporated such as:
  // \code
  //   bool converged = false;
  //   if (new_error < tolerance_/200) converged = true;
  //   else if (prev_status && current_view.regions_converged(prev_view) ) {
  //     ...
  //     converged = std::abs( (new_error-old_error) / new_error ) < 1e-3;
  //     ... }
  // \endcode
  // however; i'm not sure that tolerance_/200 is a good measure of convergence
}

TESTMAIN(test_convergence);
