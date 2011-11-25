#include <testlib/testlib_test.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_compute_similarity_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/algo/vpgl_bundle_adjust.h>
#include <vnl/vnl_crs_index.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_double_3.h>
#include <vcl_algorithm.h>


// create a test scene with world points, cameras, and ideal projections
// of the points into the images
void setup_scene(const vpgl_calibration_matrix<double>& K,
                 vcl_vector<vgl_point_3d<double> >& world,
                 vcl_vector<vpgl_perspective_camera<double> >& cameras,
                 vcl_vector<vgl_point_2d<double> >& image_points)
{
  world.clear();
  // The world points are the 8 corners of a unit cube
  world.push_back(vgl_point_3d<double>(0.0, 0.0, 0.0));
  world.push_back(vgl_point_3d<double>(0.0, 0.0, 1.0));
  world.push_back(vgl_point_3d<double>(0.0, 1.0, 0.0));
  world.push_back(vgl_point_3d<double>(0.0, 1.0, 1.0));
  world.push_back(vgl_point_3d<double>(1.0, 0.0, 0.0));
  world.push_back(vgl_point_3d<double>(1.0, 0.0, 1.0));
  world.push_back(vgl_point_3d<double>(1.0, 1.0, 0.0));
  world.push_back(vgl_point_3d<double>(1.0, 1.0, 1.0));

  vgl_rotation_3d<double> I; // no rotation initially

  cameras.clear();
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(8.0, 0.0, 8.0),I));
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(10.0, 10.0, 0.0),I));
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(7.0, 7.0, 7.0),I));
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(0.0, -15.0, -2.0),I));
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(5.0, 0.0, 0.0),I));

  // point all cameras to look at the origin
  for (unsigned int i=0; i<cameras.size(); ++i)
    cameras[i].look_at(vgl_homg_point_3d<double>(0.0, 0.0, 0.0));

  // project all points in all images
  image_points.clear();
  for (unsigned int i=0; i<cameras.size(); ++i) {
    for (unsigned int j=0; j<world.size(); ++j) {
      image_points.push_back(cameras[i](vgl_homg_point_3d<double>(world[j])));
    }
  }
}


// add uniform noise to the measurements with a maximum range size of max_p_err
vcl_vector<vgl_point_2d<double> >
make_noisy_measurements(const vcl_vector<vgl_point_2d<double> >& image_points,
                        double max_p_err)
{
  vnl_random rnd;
  // project each point adding uniform noise in a [-max_p_err/2, max_p_err/2] pixel window
  vcl_vector<vgl_point_2d<double> > noisy_image_points(image_points);
  for (unsigned int i=0; i<noisy_image_points.size(); ++i) {
    vgl_vector_2d<double> noise(rnd.drand32()-0.5, rnd.drand32()-0.5);
    noisy_image_points[i] += max_p_err * noise;
  }
  return noisy_image_points;
}


// add uniform noise to 3-d point locations
void
perturb_points(vcl_vector<vgl_point_3d<double> >& points,
               double max_p_err)
{
  vnl_random rnd;
  // add uniform noise in a [-max_p_err/2, max_p_err/2] window
  for (unsigned int j=0; j<points.size(); ++j) {
    vgl_vector_3d<double> noise(rnd.drand32()-0.5,
                                rnd.drand32()-0.5,
                                rnd.drand32()-0.5);
    points[j] += max_p_err * noise;
  }
}

// add uniform noise to camera locations and orientation
void
perturb_cameras(vcl_vector<vpgl_perspective_camera<double> >& cameras,
                double max_t_err, double max_r_error)
{
  vnl_random rnd;
  // add uniform noise in a [-max_p_err/2, max_p_err/2] window
  for (unsigned int i=0; i<cameras.size(); ++i) {
    vgl_vector_3d<double> tnoise(rnd.drand32()-0.5,
                                 rnd.drand32()-0.5,
                                 rnd.drand32()-0.5);
    cameras[i].set_camera_center(cameras[i].get_camera_center()+max_t_err * tnoise);
    vnl_double_3 rnoise(rnd.drand32()-0.5,
                        rnd.drand32()-0.5,
                        rnd.drand32()-0.5);
    rnoise.normalize();
    rnoise *= rnd.drand32() * max_r_error;
    cameras[i].set_rotation(cameras[i].get_rotation() *
                            vgl_rotation_3d<double>(rnoise));
  }
}


// remove some observations and mark them in the mask
vcl_vector<vgl_point_2d<double> >
make_occlusions(const vcl_vector<vgl_point_2d<double> >& image_points,
                vcl_vector<vcl_vector<bool> >& mask)
{
  // remove several correspondences
  mask[0][1] = false;
  mask[0][2] = false;

  mask[1][0] = false;
  mask[1][1] = false;

  mask[2][3] = false;
  mask[2][5] = false;
  mask[2][6] = false;

  mask[4][0] = false;
  mask[4][1] = false;
  mask[4][6] = false;

  // create a subset of projections based on the mask
  vnl_crs_index crs(mask);
  vcl_vector<vgl_point_2d<double> > subset_image_points(crs.num_non_zero());
  for (int i=0; i<crs.num_rows(); ++i) {
    for (int j=0; j<crs.num_cols(); ++j) {
      int k = crs(i,j);
      if (k >= 0)
        subset_image_points[k] = image_points[i*crs.num_cols() + j];
    }
  }
  return subset_image_points;
}


// apply a similarity transformation to map the points as close as possible to
// the the ground truth
void similarity_to_truth(const vcl_vector<vgl_point_3d<double> >& truth_pts,
                         vcl_vector<vgl_point_3d<double> >& est_pts,
                         vcl_vector<vpgl_perspective_camera<double> >& est_cameras)
{
  vgl_compute_similarity_3d<double> reg(est_pts,truth_pts);
  reg.estimate();
  const vgl_rotation_3d<double>& R = reg.rotation();
  double s = reg.scale();
  const vgl_vector_3d<double>& t = reg.translation();
  for (unsigned i=0; i<est_pts.size(); ++i)
  {
    vgl_point_3d<double>& p = est_pts[i];
    p = R*p;
    p.set(s*p.x(), s*p.y(), s*p.z());
    p += t;
  }
  for (unsigned i=0; i<est_cameras.size(); ++i)
  {
    vpgl_perspective_camera<double>& P = est_cameras[i];
    P.set_rotation(P.get_rotation()*R.inverse());
    vgl_point_3d<double> c = R*P.get_camera_center();
    c.set(s*c.x(), s*c.y(), s*c.z());
    P.set_camera_center(c + t);
  }
}


static void test_bundle_adjust()
{
  const double max_p_err = 0.25;//1; // maximum image error to introduce (pixels)

  vcl_vector<vgl_point_3d<double> > world;
  vcl_vector<vpgl_perspective_camera<double> > cameras;
  vcl_vector<vgl_point_2d<double> > image_points;
  // our known internal calibration
  //vpgl_calibration_matrix<double> K(2.0,vgl_homg_point_2d<double>(0,0));
  vpgl_calibration_matrix<double> K(2000.0,vgl_homg_point_2d<double>(512,384));
  //vpgl_calibration_matrix<double> K(2000.0,vgl_homg_point_2d<double>(512,384),1,0.9,2);
  setup_scene(K, world, cameras, image_points);

  // write a VRML version of the scene for debugging
  vpgl_bundle_adjust::write_vrml("test_bundle_truth.wrl",cameras,world);

  // create noisy measurements
  vcl_vector<vgl_point_2d<double> > noisy_image_points =
      make_noisy_measurements(image_points, max_p_err);

  // remove some measurements (occlusion)
  vcl_vector<vcl_vector<bool> > mask(cameras.size(), vcl_vector<bool>(world.size(),true) );
  vcl_vector<vgl_point_2d<double> > subset_image_points =
      make_occlusions(noisy_image_points, mask);

  // test optimization with fixed calibration
  {
    // make default cameras
    vgl_rotation_3d<double> I; // no rotation
    vpgl_perspective_camera<double> init_cam(K,vgl_homg_point_3d<double>(0.0, 0.0, -10.0),I);
    init_cam.look_at(vgl_homg_point_3d<double>(0.0, 0.0, 0.0),vgl_vector_3d<double>(0,-1,0));
    vcl_vector<vpgl_perspective_camera<double> >
      unknown_cameras(cameras.size(),init_cam);

    // make the unknown world points
    vcl_vector<vgl_point_3d<double> > unknown_world(world.size(),vgl_point_3d<double>(0.0, 0.0, 0.0));

    vpgl_bundle_adjust ba;
    bool converge = ba.optimize(unknown_cameras, unknown_world, subset_image_points, mask);
    TEST("Converged (fixed K)",converge,true);
    similarity_to_truth(world, unknown_world, unknown_cameras);
    double rms_3d_pts = 0.0;
    for ( unsigned i=0; i< unknown_world.size(); ++i)
      rms_3d_pts += (unknown_world[i] - world[i]).sqr_length();
    rms_3d_pts = vcl_sqrt(rms_3d_pts/world.size());
    TEST_NEAR("Solution Correct (fixed K)", rms_3d_pts, 0.0, 2.0e-3);

    // make default cameras
    unknown_cameras = vcl_vector<vpgl_perspective_camera<double> >(cameras.size(),init_cam);
    // make the unknown world points
    unknown_world = vcl_vector<vgl_point_3d<double> >(world.size(),vgl_point_3d<double>(0.0, 0.0, 0.0));
    ba.set_use_gradient(false);
    converge = ba.optimize(unknown_cameras, unknown_world, subset_image_points, mask);
    TEST("Converged (without gradient, fixed K)",converge,true);
    similarity_to_truth(world, unknown_world, unknown_cameras);
    rms_3d_pts = 0.0;
    for ( unsigned i=0; i< unknown_world.size(); ++i)
      rms_3d_pts += (unknown_world[i] - world[i]).sqr_length();
    rms_3d_pts = vcl_sqrt(rms_3d_pts/world.size());
    TEST_NEAR("Solution Correct (without gradient, fixed K)", rms_3d_pts, 0.0, 2.0e-3);

    vpgl_bundle_adjust::write_vrml("test_bundle_fixed_k.wrl",unknown_cameras,unknown_world);
  }

  // test optimization with shared calibration and unknown focal length
  {
    // make default cameras
    vgl_rotation_3d<double> I; // no rotation
    vpgl_calibration_matrix<double> K2 = K;
    K2.set_focal_length(1500);
    vpgl_perspective_camera<double> init_cam(K2,vgl_homg_point_3d<double>(0.0, 0.0, -10.0),I);
    init_cam.look_at(vgl_homg_point_3d<double>(0.0, 0.0, 0.0),vgl_vector_3d<double>(0,-1,0));
    vcl_vector<vpgl_perspective_camera<double> >
      unknown_cameras(cameras.size(),init_cam);

    // make the unknown world points
    vcl_vector<vgl_point_3d<double> > unknown_world(world.size(),vgl_point_3d<double>(0.0, 0.0, 0.0));

    vpgl_bundle_adjust ba;
    ba.set_self_calibrate(true);
    ba.set_max_iterations(10000);
    bool converge = ba.optimize(unknown_cameras, unknown_world, subset_image_points, mask);
    TEST("Converged (est focal len)",converge,true);
    similarity_to_truth(world, unknown_world, unknown_cameras);
    double rms_3d_pts = 0.0;
    for ( unsigned i=0; i< unknown_world.size(); ++i)
      rms_3d_pts += (unknown_world[i] - world[i]).sqr_length();
    rms_3d_pts = vcl_sqrt(rms_3d_pts/world.size());
    TEST_NEAR("Solution Correct (est focal len)", rms_3d_pts, 0.0, 2.0e-3);

    // make default cameras
    unknown_cameras = vcl_vector<vpgl_perspective_camera<double> >(cameras.size(),init_cam);
    // make the unknown world points
    unknown_world = vcl_vector<vgl_point_3d<double> >(world.size(),vgl_point_3d<double>(0.0, 0.0, 0.0));
    ba.set_use_gradient(false);
    converge = ba.optimize(unknown_cameras, unknown_world, subset_image_points, mask);
    TEST("Converged (without gradient, est focal len)",converge,true);
    similarity_to_truth(world, unknown_world, unknown_cameras);
    rms_3d_pts = 0.0;
    for ( unsigned i=0; i< unknown_world.size(); ++i)
      rms_3d_pts += (unknown_world[i] - world[i]).sqr_length();
    rms_3d_pts = vcl_sqrt(rms_3d_pts/world.size());
    TEST_NEAR("Solution Correct (without gradient, est focal len)", rms_3d_pts, 0.0, 2.0e-3);

    vpgl_bundle_adjust::write_vrml("test_bundle_est_f.wrl",unknown_cameras,unknown_world);
  }

  // test optimization with outliers
  {
    mask = vcl_vector<vcl_vector<bool> >(cameras.size(), vcl_vector<bool>(world.size(),true) );
    // make outliers
    noisy_image_points[6].x() += 20;
    noisy_image_points[6].y() -= 18;
    noisy_image_points[10] = noisy_image_points[0];

    vcl_vector<vgl_point_3d<double> > unknown_world(world);
    perturb_points(unknown_world, 0.1);
    vcl_vector<vgl_point_3d<double> > init_world(unknown_world);
    vcl_vector<vpgl_perspective_camera<double> > unknown_cameras(cameras);
    perturb_cameras(unknown_cameras,0.1, 0.0001);
    vcl_vector<vpgl_perspective_camera<double> > init_cameras(unknown_cameras);

    vpgl_bundle_adjust ba;
    ba.set_self_calibrate(true);

    ba.set_use_m_estimator(true);
    ba.set_max_iterations(10000);
    ba.set_m_estimator_scale(100.0);
    bool converge = ba.optimize(unknown_cameras, unknown_world, noisy_image_points, mask);
    ba.set_m_estimator_scale(50.0);
    converge = converge && ba.optimize(unknown_cameras, unknown_world, noisy_image_points, mask);
    ba.set_m_estimator_scale(20.0);
    converge = converge && ba.optimize(unknown_cameras, unknown_world, noisy_image_points, mask);
    ba.set_m_estimator_scale(4.0);
    converge = converge && ba.optimize(unknown_cameras, unknown_world, noisy_image_points, mask);
    TEST("Converged (outliers)",converge,true);
    double rms_error = ba.end_error();
    vcl_cout << "Final RMS reprojection error: "<<rms_error<<vcl_endl;
    vcl_vector<double> weights = ba.final_weights();
    bool outliers_downweighted = true;
    for (unsigned i=0; i<weights.size(); ++i)
    {
      if ( i == 6 || i == 10 ) // outliers
      {
        if (weights[i] > 0.5) // should have low weight
        {
          outliers_downweighted = false;
          vcl_cout << "outlier measurement "<<i<<" has high weight "<<weights[i]<<vcl_endl;
        }
      }
      else if (weights[i] < 0.9) // inliers should have high weight
      {
        outliers_downweighted = false;
        vcl_cout << "inlier measurement "<<i<<" has low weight "<<weights[i]<<vcl_endl;
      }
    }
    TEST("only outliers down-weighted",outliers_downweighted, true);

    similarity_to_truth(world, unknown_world, unknown_cameras);
    double rms_3d_pts = 0.0;
    for ( unsigned i=0; i< unknown_world.size(); ++i)
      rms_3d_pts += (unknown_world[i] - world[i]).sqr_length();
    rms_3d_pts = vcl_sqrt(rms_3d_pts/world.size());
    TEST_NEAR("Solution Correct (outliers)", rms_3d_pts, 0.0, 1.0e-3);

    unknown_cameras = init_cameras;
    unknown_world = init_world;

    ba.set_use_gradient(false);
    ba.set_m_estimator_scale(100.0);
    converge = ba.optimize(unknown_cameras, unknown_world, noisy_image_points, mask);
    ba.set_m_estimator_scale(50.0);
    converge = converge && ba.optimize(unknown_cameras, unknown_world, noisy_image_points, mask);
    ba.set_m_estimator_scale(20.0);
    converge = converge && ba.optimize(unknown_cameras, unknown_world, noisy_image_points, mask);
    ba.set_m_estimator_scale(4.0);
    converge = converge && ba.optimize(unknown_cameras, unknown_world, noisy_image_points, mask);
    TEST("Converged (without gradient, outliers)",converge,true);
    rms_error = ba.end_error();
    vcl_cout << "Final RMS reprojection error: "<<rms_error<<vcl_endl;

    weights = ba.final_weights();
    outliers_downweighted = true;
    for (unsigned i=0; i<weights.size(); ++i)
    {
      if ( i == 6 || i == 10 ) // outliers
      {
        if (weights[i] > 0.5) // should have low weight
        {
          outliers_downweighted = false;
          vcl_cout << "outlier measurement "<<i<<" has high weight "<<weights[i]<<vcl_endl;
        }
      }
      else if (weights[i] < 0.9) // inliers should have high weight
      {
        outliers_downweighted = false;
        vcl_cout << "inlier measurement "<<i<<" has low weight "<<weights[i]<<vcl_endl;
      }
    }
    TEST("only outliers down-weighted",outliers_downweighted, true);

    similarity_to_truth(world, unknown_world, unknown_cameras);
    rms_3d_pts = 0.0;
    for ( unsigned i=0; i< unknown_world.size(); ++i)
      rms_3d_pts += (unknown_world[i] - world[i]).sqr_length();
    rms_3d_pts = vcl_sqrt(rms_3d_pts/world.size());
    TEST_NEAR("Solution Correct (without gradient, outliers)", rms_3d_pts, 0.0, 1.0e-3);

    vpgl_bundle_adjust::write_vrml("test_bundle_est_f.wrl",unknown_cameras,unknown_world);
  }
}

TESTMAIN(test_bundle_adjust);
