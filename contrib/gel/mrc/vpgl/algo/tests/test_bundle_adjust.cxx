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

static void test_bundle_adjust()
{
  const double max_p_err = 1.0; // maximum image error to introduce (pixels)

  vcl_vector<vgl_point_3d<double> > world;
  // The world points are the 8 corners of a unit cube
  world.push_back(vgl_point_3d<double>(0.0, 0.0, 0.0));
  world.push_back(vgl_point_3d<double>(0.0, 0.0, 1.0));
  world.push_back(vgl_point_3d<double>(0.0, 1.0, 0.0));
  world.push_back(vgl_point_3d<double>(0.0, 1.0, 1.0));
  world.push_back(vgl_point_3d<double>(1.0, 0.0, 0.0));
  world.push_back(vgl_point_3d<double>(1.0, 0.0, 1.0));
  world.push_back(vgl_point_3d<double>(1.0, 1.0, 0.0));
  world.push_back(vgl_point_3d<double>(1.0, 1.0, 1.0));

  // our known internal calibration
  //vpgl_calibration_matrix<double> K(2000.0,vgl_homg_point_2d<double>(512,384));
  vpgl_calibration_matrix<double> K(2000.0,vgl_homg_point_2d<double>(512,384),1,0.9,2);
  vgl_rotation_3d<double> I; // no rotation initially

  vcl_vector<vpgl_perspective_camera<double> > cameras;
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(8.0, 0.0, 8.0),I));
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(10.0, 10.0, 0.0),I));
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(7.0, 7.0, 7.0),I));
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(0.0, -15.0, -2.0),I));
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(5.0, 0.0, 0.0),I));

  // point all cameras to look at the origin
  for (unsigned int i=0; i<cameras.size(); ++i)
    cameras[i].look_at(vgl_homg_point_3d<double>(0.0, 0.0, 0.0));

  vpgl_bundle_adjust::write_vrml("test_bundle1.wrl",cameras,world);

  // project all points in all images
  vcl_vector<vgl_point_2d<double> > image_points;
  for (unsigned int i=0; i<cameras.size(); ++i){
    for (unsigned int j=0; j<world.size(); ++j){
      image_points.push_back(cameras[i](vgl_homg_point_3d<double>(world[j])));
    }
  }


  vnl_random rnd;
  // project each point adding uniform noise in a [-max_p_err/2, max_p_err/2] pixel window
  vcl_vector<vgl_point_2d<double> > noisy_image_points(image_points);
  for (unsigned int i=0; i<noisy_image_points.size(); ++i){
    vgl_vector_2d<double> noise(rnd.drand32()-0.5, rnd.drand32()-0.5);
    noisy_image_points[i] += max_p_err * noise;
  }


  // make default cameras
  vpgl_perspective_camera<double> init_cam(K,vgl_homg_point_3d<double>(0.0, 0.0, -10.0),I);
  init_cam.look_at(vgl_homg_point_3d<double>(0.0, 0.0, 0.0));
  vcl_vector<vpgl_perspective_camera<double> >
    unknown_cameras(cameras.size(),init_cam);

  // make the unknown world points
  vcl_vector<vgl_point_3d<double> > unknown_world(world.size(),vgl_point_3d<double>(0.0, 0.0, 0.0));


  // make the mask (using all the points)
  vcl_vector<vcl_vector<bool> > mask(cameras.size(), vcl_vector<bool>(world.size(),true) );


  // remove several correspondences
  mask[0][1] = false;
  mask[0][2] = false;
  mask[1][0] = false;
  mask[1][1] = false;
  mask[1][3] = false;
  mask[2][3] = false;
  mask[2][5] = false;
  mask[2][6] = false;
  mask[3][7] = false;
  mask[3][8] = false;
  mask[4][0] = false;
  mask[4][1] = false;
  mask[4][6] = false;
  mask[4][8] = false;


  // create a subset of projections based on the mask
  vnl_crs_index crs(mask);
  vcl_vector<vgl_point_2d<double> > subset_image_points(crs.num_non_zero());
  for (int i=0; i<crs.num_rows(); ++i){
    for (int j=0; j<crs.num_cols(); ++j){
      int k = crs(i,j);
      if (k >= 0)
        subset_image_points[k] = noisy_image_points[i*crs.num_cols() + j];
    }
  }

  // make some correspondences incorrect
  //subset_image_points[crs(0,3)] = subset_image_points[crs(0,4)];

  vpgl_bundle_adjust ba;
  bool converge = ba.optimize(unknown_cameras, unknown_world, subset_image_points, mask);
  TEST("Converged",converge,true);
  double rms_error = ba.end_error();
  TEST("Solution Correct", rms_error<0.5,true);
  vnl_vector<double> a1 = ba.cam_params();
  vnl_vector<double> b1 = ba.point_params();


  // make default cameras
  unknown_cameras = vcl_vector<vpgl_perspective_camera<double> >(cameras.size(),init_cam);
  // make the unknown world points
  unknown_world = vcl_vector<vgl_point_3d<double> >(world.size(),vgl_point_3d<double>(0.0, 0.0, 0.0));
  ba.set_use_gradient(false);
  converge = ba.optimize(unknown_cameras, unknown_world, subset_image_points, mask);
  TEST("Converged (without gradient)",converge,true);
  rms_error = ba.end_error();
  TEST("Solution Correct (without gradient)", rms_error<0.5,true);
  vnl_vector<double> a2 = ba.cam_params();
  vnl_vector<double> b2 = ba.point_params();
  double diff = vcl_max((a1-a2).inf_norm(), (b1-b2).inf_norm());
  TEST_NEAR("Solutions are the same",diff,0.0,1e-4);


  vpgl_bundle_adjust::write_vrml("test_bundle2.wrl",unknown_cameras,unknown_world);
}

TESTMAIN(test_bundle_adjust);
