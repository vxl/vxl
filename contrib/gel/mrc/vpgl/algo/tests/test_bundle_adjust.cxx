
#include <testlib/testlib_test.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/algo/vpgl_bundle_adjust.h>
#include <vnl/vnl_crs_index.h>
#include <vnl/vnl_random.h>
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
  for (unsigned int i=0; i<cameras.size(); ++i){
    for (unsigned int j=0; j<world.size(); ++j){
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
  for (unsigned int i=0; i<noisy_image_points.size(); ++i){
    vgl_vector_2d<double> noise(rnd.drand32()-0.5, rnd.drand32()-0.5);
    noisy_image_points[i] += max_p_err * noise;
  }
  return noisy_image_points;
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
  for (int i=0; i<crs.num_rows(); ++i){
    for (int j=0; j<crs.num_cols(); ++j){
      int k = crs(i,j);
      if (k >= 0)
        subset_image_points[k] = image_points[i*crs.num_cols() + j];
    }
  }
  return subset_image_points;
}


static void test_bundle_adjust()
{
  const double max_p_err = 1; // maximum image error to introduce (pixels)

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


  // remove some measurments (occlusion)
  vcl_vector<vcl_vector<bool> > mask(cameras.size(), vcl_vector<bool>(world.size(),true) );
  vcl_vector<vgl_point_2d<double> > subset_image_points =
      make_occlusions(noisy_image_points, mask);

  // make some correspondences incorrect
  //subset_image_points[crs(0,3)] = subset_image_points[crs(0,4)];

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
    double rms_error = ba.end_error();
    TEST("Solution Correct (fixed K)", rms_error<0.5,true);
    vnl_vector<double> a1 = ba.cam_params();
    vnl_vector<double> b1 = ba.point_params();


    // make default cameras
    unknown_cameras = vcl_vector<vpgl_perspective_camera<double> >(cameras.size(),init_cam);
    // make the unknown world points
    unknown_world = vcl_vector<vgl_point_3d<double> >(world.size(),vgl_point_3d<double>(0.0, 0.0, 0.0));
    ba.set_use_gradient(false);
    converge = ba.optimize(unknown_cameras, unknown_world, subset_image_points, mask);
    TEST("Converged (without gradient, fixed K)",converge,true);
    rms_error = ba.end_error();
    TEST("Solution Correct (without gradient, fixed K)", rms_error<0.5,true);
    vnl_vector<double> a2 = ba.cam_params();
    vnl_vector<double> b2 = ba.point_params();
    double diff = vcl_max((a1-a2).inf_norm(), (b1-b2).inf_norm());
    TEST_NEAR("Solutions are the same (fixed K)",diff,0.0,1e-4);

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
    double rms_error = ba.end_error();
    TEST("Solution Correct (est focal len)", rms_error<0.5,true);
    vnl_vector<double> a1 = ba.cam_params();
    vnl_vector<double> b1 = ba.point_params();


    // make default cameras
    unknown_cameras = vcl_vector<vpgl_perspective_camera<double> >(cameras.size(),init_cam);
    // make the unknown world points
    unknown_world = vcl_vector<vgl_point_3d<double> >(world.size(),vgl_point_3d<double>(0.0, 0.0, 0.0));
    ba.set_use_gradient(false);
    converge = ba.optimize(unknown_cameras, unknown_world, subset_image_points, mask);
    TEST("Converged (without gradient, est focal len)",converge,true);
    rms_error = ba.end_error();
    TEST("Solution Correct (without gradient, est focal len)", rms_error<0.5,true);
    vnl_vector<double> a2 = ba.cam_params();
    vnl_vector<double> b2 = ba.point_params();
    double diff = vcl_max((a1-a2).inf_norm(), (b1-b2).inf_norm());
    TEST_NEAR("Solutions are the same (est focal len)",diff,0.0,1e-4);

    vpgl_bundle_adjust::write_vrml("test_bundle_est_f.wrl",unknown_cameras,unknown_world);
  }
}

TESTMAIN(test_bundle_adjust);
