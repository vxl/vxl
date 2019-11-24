#include <iostream>
#include <iomanip>
#include <vector>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vgl/vgl_point_2d.h"
#include <bpgl/algo/bpgl_gridding.h>
#include "vnl/vnl_math.h"
#include "vnl/vnl_random.h"

void test_simple()
{
  // test a simple surface
  typedef double T;

  // sample a simple surface z=x
  std::vector<vgl_point_2d<T> > sample_locs;
  sample_locs.emplace_back(2.0f, 2.0f);
  sample_locs.emplace_back(1.5f, -1.2f);
  sample_locs.emplace_back(4.5f, 0.5f);
  sample_locs.emplace_back(4.5f, 2.5f);
  sample_locs.emplace_back(3.0f, 3.0f);

  vnl_random randgen(1234);  // fixed seed for repeatability
  std::vector<float> sample_vals;
  for (auto loc : sample_locs) {
    double noise = randgen.normal()*0.01;
    // f(x,y) = x
    sample_vals.push_back(loc.x() + noise);
  }

  vgl_point_2d<T> upper_left(0.0f, 0.0f);
  size_t ni = 8, nj = 8;
  T step_size = 1.0;
  unsigned min_neighbors = 3;
  unsigned max_neighbors = 5;
  T max_dist = 15.0f;

  bpgl_gridding::linear_interp<T, float> interp_fun;

  vil_image_view<float> gridded =
    bpgl_gridding::grid_data_2d(interp_fun, sample_locs, sample_vals,
                                upper_left, ni, nj, step_size,
                                min_neighbors, max_neighbors, max_dist);

  bool all_good = true;
  for (int j=0; j<nj; ++j) {
    for (int i=0; i<ni; ++i) {
      // "truth" is f(x,y) = x
      std::cout << "i,j: " << i << "," << j << "  "
                << "expected: " << i << "  "
                << "f(i,j): " << gridded(i,j) << "\n";
      double err = gridded(i,j) - i;
      all_good &= std::fabs(err) < 0.25;
    }
  }
  TEST("gridded values correct", all_good, true);
}

void test_degenerate()
{
  // test a degenerate (linear) set of control points
  typedef double T;

  // sample a simple surface z=x
  std::vector<vgl_point_2d<T> > sample_locs;
  sample_locs.emplace_back(0.0f, 0.0f);
  sample_locs.emplace_back(1.0f, 0.0f);
  sample_locs.emplace_back(2.0f, 0.0f);

  vnl_random randgen(1234);  // fixed seed for repeatability
  std::vector<float> sample_vals;
  for (auto loc : sample_locs) {
    double noise = randgen.normal()*0.01;
    // f(x,y) = x
    sample_vals.push_back(loc.x() + noise);
  }

  vgl_point_2d<T> upper_left(0.0f, 0.0f);
  size_t ni = 6, nj = 6;
  T step_size = 1.0;
  unsigned min_neighbors = 3;
  unsigned max_neighbors = 5;
  T max_dist = 15.0f;

  bpgl_gridding::linear_interp<T, float> interp_fun;

  vil_image_view<float> gridded =
    bpgl_gridding::grid_data_2d(interp_fun, sample_locs, sample_vals,
                                upper_left, ni, nj, step_size,
                                min_neighbors, max_neighbors, max_dist);

  bool all_good = true;
  for (int j=0; j<nj; ++j) {
    for (int i=0; i<ni; ++i) {
      // "truth" is f(x,y) = x
      std::cout << "i,j: " << i << "," << j << "  "
                << "expected: " << i << "  "
                << "f(i,j): " << gridded(i,j) << "\n";
      double err = gridded(i,j) - i;
      all_good &= std::fabs(err) < 0.25;
    }
  }
  TEST("gridded values correct", all_good, true);
}


void test_interp_real()
{
  // A test case derived from a real example giving unexpected results
  std::vector<vgl_point_2d<double>> ctrl_pts;
  ctrl_pts.emplace_back(9.35039, 151.517);
  ctrl_pts.emplace_back(8.93042, 151.390);
  ctrl_pts.emplace_back(8.57767, 151.285);

  std::vector<double> values = { 47.7940, 46.3976, 47.7940 };

  bpgl_gridding::linear_interp<double, double> interp_fun;

  vgl_point_2d<double> test_point(9, 151.999);
  double value = interp_fun(test_point, ctrl_pts, values);
  TEST_NEAR("interpolated value in correct range", value, 47.0, 1.0);
}


void test_interp_real_origin()
{
  // A test case derived from a real example giving unexpected results
  // interpolation points centered near origin
  std::vector<vgl_point_2d<double>> ctrl_pts;
  ctrl_pts.emplace_back(1.35039, 0.517);
  ctrl_pts.emplace_back(0.93042, 0.390);
  ctrl_pts.emplace_back(0.57767, 0.285);

  std::vector<double> values = { 47.7940, 46.3976, 47.7940 };

  bpgl_gridding::linear_interp<double, double> interp_fun;

  vgl_point_2d<double> test_point(1, 0.999);
  double value = interp_fun(test_point, ctrl_pts, values);
  TEST_NEAR("interpolated value in correct range", value, 47.0, 1.0);
}


#ifndef BPGL_TIMING
#define BPGL_TIMING 0
#endif

#if BPGL_TIMING
#include "vul/vul_timer.h"

void test_interp_timing(unsigned long num_iter)
{
  double value = 0.0;
  vul_timer timer;

  std::vector<vgl_point_2d<double>> ctrl_pts;
  ctrl_pts.emplace_back(9.35039, 151.517);
  ctrl_pts.emplace_back(8.93042, 151.390);
  ctrl_pts.emplace_back(8.57767, 151.285);

  std::vector<double> values = { 47.7940, 46.3976, 47.7940 };

  bpgl_gridding::linear_interp<double, double> interp_fun;

  vgl_point_2d<double> test_point(9, 151.999);

  timer.mark();
  for (unsigned i = 0; i < num_iter; i++)
    value = interp_fun(test_point, ctrl_pts, values);
  double operator_sec = timer.real() / 1000.0;

  std::cout << "---Timing report---\n"
            << "Interpolate " << ctrl_pts.size() << " points for " << num_iter << " iterations\n"
            << "operator()\n"
            << "  total time = " << operator_sec << " sec.\n"
            << "  per-operation time = " << operator_sec / double(num_iter) << " sec.\n"
            ;
}
#endif


static void test_gridding()
{
  test_simple();
  test_degenerate();
  test_interp_real();
  test_interp_real_origin();

  // timing report
#if BPGL_TIMING
  test_interp_timing(1000000);
#endif
}

TESTMAIN(test_gridding);
