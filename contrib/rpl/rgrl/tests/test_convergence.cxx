#include <testlib/testlib_test.h>

MAIN( test_convergence )
{
  START( "various convergence objects" );

  // there should be a test to make sure that all convergence objects converge when we are registering
  // identical data sets

  // currently, there is a chunk of code in each of rgrl_convergence_on_median_error and rgrl_convergence_on_weighted_error {.cxx}
  // that looks like:
  // bool converged = false;
  //  if (prev_status && current_view.regions_converged(prev_view) ) {
  //  ...
  // converged = vcl_abs( (new_error-old_error) / new_error ) < 1e-3;
  // ... }

  // one problem that I see is: converged will always be false after the first iteration
  // is this really a warranted assumption?
  // although the error would most likely never be == to 0.0f on a real data set,
  // i could imagine a threshold incorporated such as:
  // bool converged = false;
  //  if (new_error < tolerance_/200) converged = true;
  //  else if (prev_status && current_view.regions_converged(prev_view) ) {
  //  ...
  // converged = vcl_abs( (new_error-old_error) / new_error ) < 1e-3;
  // ... }
  // however; i'm not sur ethat tolerance_/200 is a good measure of convergence

  SUMMARY();
}
