#include <iostream>
#include <iomanip>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <bpgl/algo/bpgl_gridding.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>

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
  unsigned num_neighbors = 4;
  T maxdist = 15.0f;

  bpgl_gridding::linear_interp<T, float> interp_fun(maxdist, NAN);

  vil_image_view<float> gridded =
    bpgl_gridding::grid_data_2d(sample_locs, sample_vals,
                                upper_left, ni, nj, step_size,
                                interp_fun,
                                num_neighbors);
  bool print_grid = false;
  if (print_grid) {
    for (int j=0; j<nj; ++j) {
      for (int i=0; i<ni; ++i) {
        std::cout << std::fixed << std::setprecision(3) << gridded(i,j) << " ";
      }
      std::cout << std::endl;
    }
  }

  bool all_good = true;
  for (int j=0; j<nj; ++j) {
    for (int i=0; i<ni; ++i) {
      // "truth" is f(x,y) = x
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
  unsigned num_neighbors = 3;
  T maxdist = 15.0f;

  bpgl_gridding::linear_interp<T, float> interp_fun(maxdist, NAN);

  vil_image_view<float> gridded =
    bpgl_gridding::grid_data_2d(sample_locs, sample_vals,
                                upper_left, ni, nj, step_size,
                                interp_fun,
                                num_neighbors);
  bool print_grid = false;
  if (print_grid) {
    for (int j=0; j<nj; ++j) {
      for (int i=0; i<ni; ++i) {
        std::cout << std::fixed << std::setprecision(3) << gridded(i,j) << " ";
      }
      std::cout << std::endl;
    }
  }

  bool all_good = true;
  for (int j=0; j<nj; ++j) {
    for (int i=0; i<ni; ++i) {
      // "truth" is f(x,y) = x
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

 bpgl_gridding::linear_interp<double, double> interp;

  vgl_point_2d<double> test_point(9, 151.999);
  double value = interp(test_point, ctrl_pts, values);

  TEST_NEAR("interpolated value in correct range", value, 47.0, 1.0);
}

static void test_gridding()
{
  test_simple();
  test_degenerate();
  test_interp_real();
}

TESTMAIN(test_gridding);
