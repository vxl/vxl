#include <vul/vul_arg.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/algo/vpgl_bundle_adjust.h>
#include <vnl/vnl_crs_index.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_double_3.h>


// The Main Function
int main(int argc, char** argv)
{
  vul_arg<int>    a_num_cameras("-ncam", "number of cameras", 30);
  vul_arg<int>    a_num_points("-npt", "number of points", 30);
  vul_arg<double> a_frac_miss("-miss", "fraction of missing correspondences", 0.1);
  vul_arg<bool>   a_no_gradient("-no_grad", "use numeric differencing instead of gradients", false);
  vul_arg_parse(argc, argv);

  constexpr double max_p_err = 1.0; // maximum image error to introduce (pixels)

  // deterministic random number generator -- random but repeatable
  long int seed = 0;
  vnl_random rnd(seed);

  std::vector<vgl_point_3d<double> > world;
  world.reserve(a_num_cameras());
for (int i=0; i<a_num_cameras(); ++i)
    world.emplace_back(rnd.drand32(-1,1), rnd.drand32(-1,1), rnd.drand32(-1,1));

  // our known internal calibration
  vpgl_calibration_matrix<double> K(2000.0,vgl_homg_point_2d<double>(500,500));
  vgl_rotation_3d<double> I; // no rotation initially

  std::vector<vpgl_perspective_camera<double> > cameras;
  for (int i=0; i<a_num_points(); ++i)
  {
    vnl_double_3 p;
    do {
      p = vnl_double_3(rnd.drand32(-1,1),rnd.drand32(-1,1),rnd.drand32(-1,1));
    } while ( p.magnitude() <= 1.0 );
    p.normalize();
    p *= 10.0;
    vgl_homg_point_3d<double> c(p[0], p[1], p[2]);
    cameras.emplace_back(K,c,I);
    cameras.back().look_at(vgl_homg_point_3d<double>(0.0, 0.0, 0.0));
  }


  // project all points in all images
  std::vector<vgl_point_2d<double> > image_points;
  for (auto & camera : cameras) {
    for (const auto & j : world) {
      image_points.emplace_back(camera(vgl_homg_point_3d<double>(j)));
    }
  }


  // project each point adding uniform noise in a [-max_p_err/2, max_p_err/2] pixel window
  std::vector<vgl_point_2d<double> > noisy_image_points(image_points);
  for (auto & noisy_image_point : noisy_image_points) {
    vgl_vector_2d<double> noise(rnd.drand32()-0.5, rnd.drand32()-0.5);
    noisy_image_point += max_p_err * noise;
  }


  // make default cameras
  vpgl_perspective_camera<double> init_cam(K,vgl_homg_point_3d<double>(0.0, 0.0, -10.0),I);
  init_cam.look_at(vgl_homg_point_3d<double>(0.0, 0.0, 0.0));
  std::vector<vpgl_perspective_camera<double> >
    unknown_cameras(cameras.size(),init_cam);

  // make the unknown world points
  std::vector<vgl_point_3d<double> > unknown_world(world.size(),vgl_point_3d<double>(0.0, 0.0, 0.0));


  // make the mask (using all the points)
  std::vector<std::vector<bool> > mask(cameras.size(), std::vector<bool>(world.size(),true) );

  size_t num_missing = (unsigned int)std::floor(a_frac_miss()*cameras.size()*world.size());
  if (a_frac_miss() >= 1.0 )
    num_missing = cameras.size()*world.size();

  std::cout << "removing "<<num_missing<<" random correspondences"<<std::endl;
  for (unsigned int i=0; i<num_missing; /* */)
  {
    const size_t c = rnd.lrand32(static_cast<int>(cameras.size()-1));
    const size_t w = rnd.lrand32(static_cast<int>(world.size()-1));
    if (mask[c][w]) {
      mask[c][w] = false;
      ++i;
    }
  }

  // create a subset of projections based on the mask
  vnl_crs_index crs(mask);
  std::vector<vgl_point_2d<double> > subset_image_points(crs.num_non_zero());
  for (int i=0; i<crs.num_rows(); ++i) {
    for (int j=0; j<crs.num_cols(); ++j) {
      int k = crs(i,j);
      if (k >= 0)
        subset_image_points[k] = noisy_image_points[i*crs.num_cols() + j];
    }
  }


  vpgl_bundle_adjust ba;
  ba.set_use_gradient(!a_no_gradient());
  ba.optimize(unknown_cameras, unknown_world, subset_image_points, mask);

  vpgl_bundle_adjust::write_vrml("results.wrl",unknown_cameras,unknown_world);
}
