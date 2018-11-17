#include <iostream>
#include <testlib/testlib_test.h>
#include <vgl/algo/vgl_compute_rigid_3d.h>
#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>


namespace vgl_test_compute_rigid_3d {

std::vector<vgl_point_3d<double> >
transform_points(const std::vector<vgl_point_3d<double> >& points,
                 const vgl_rotation_3d<double>& R,
                 vgl_vector_3d<double> t)
{
  std::vector<vgl_point_3d<double> > t_pts;
  for (const auto & point : points)
  {
    vgl_point_3d<double> p = R*point + t;
    t_pts.push_back(p);
  }
  return t_pts;
}

void add_noise(std::vector<vgl_point_3d<double> >& points, double sigma)
{
  vnl_random r;
  for (auto & point : points)
  {
    vgl_vector_3d<double> e(r.normal(),r.normal(),r.normal());
    point += e*sigma;
  }
}

// compute the RMS error between two sets of points
double alignment_error(const std::vector<vgl_point_3d<double> >& points1,
                       const std::vector<vgl_point_3d<double> >& points2)
{
  double error = 0.0;
  for ( unsigned i=0; i<points1.size(); ++i )
  {
    error += (points1[i] - points2[i]).sqr_length();
  }
  return std::sqrt(error/points1.size());
}
}

static void test_compute_rigid_3d()
{
  std::vector<vgl_point_3d<double> > points1;
  points1.emplace_back(10.5, 200.0, -340.5);
  points1.emplace_back(23.0, 250.0, -310.2);
  points1.emplace_back(15.0, 260.0, -315.7);
  points1.emplace_back(50.0, 230.0, -332.1);
  points1.emplace_back(42.3, 205.0, -325.0);
  points1.emplace_back(-5.0, 265.0, -305.0);

  vgl_vector_3d<double> t(100, -200, 200);
  vgl_rotation_3d<double> R(3.0, -1.0, 0.5);

  std::vector<vgl_point_3d<double> > points2 =
    vgl_test_compute_rigid_3d::transform_points(points1,R,t);

  vgl_compute_rigid_3d<double> est_sim(points1, points2);
  est_sim.estimate();

  TEST_NEAR("translation estimate",(est_sim.translation()-t).length(),0.0,1e-8);
  TEST_NEAR("rotation estimate",
            (est_sim.rotation().as_matrix()-R.as_matrix()).array_inf_norm(),0.0,1e-8);
  TEST_NEAR("RMS alignment error",
            vgl_test_compute_rigid_3d::
            alignment_error( vgl_test_compute_rigid_3d::
                             transform_points(points1,
                                              est_sim.rotation(),
                                              est_sim.translation()), points2),
            0.0, 1e-8);

  // add noise and try again
  double sigma = 1e-2;
  vgl_test_compute_rigid_3d::add_noise(points1, sigma);
  vgl_test_compute_rigid_3d::add_noise(points2, sigma);

  vgl_compute_rigid_3d<double> est_sim2(points1, points2);
  est_sim2.estimate();

  TEST_NEAR("noisy translation estimate",(est_sim2.translation()-t).length(),0.0,150*sigma);
  TEST_NEAR("noisy rotation estimate",
            (est_sim2.rotation().as_matrix()-R.as_matrix()).array_inf_norm(),0.0,sigma);
  TEST_NEAR("RMS alignment error",
            vgl_test_compute_rigid_3d::
            alignment_error(vgl_test_compute_rigid_3d::
                            transform_points(points1,
                                             est_sim2.rotation(),
                                             est_sim2.translation()), points2),
            0.0, 10*sigma);

}

TESTMAIN(test_compute_rigid_3d);
