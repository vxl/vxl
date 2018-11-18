#include <iostream>
#include <sstream>
#include "bvxm_synth_world_generator.h"
//
#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

// Default constructor
bvxm_synth_world_generator::bvxm_synth_world_generator()
{
  //initialize class variables
  nx_=100;
  ny_=100;
  nz_=50;
  minx_=10;
  miny_=10;
  minz_=10;
  dimx_=50;//89;
  dimy_=50;//89;
  dimz_=20;//23;
  gen_images_=true;
  gen2_box_=false;
  // texture is a checkerboard pattern if true, else it is plain white
  texture_map_=true;
  world_dir_ = "./synth_world";
  num_train_images_ = 30;
  rand1_=true;
  rand2_=true;
  app_val_=0.3f;
  vox_length = 1.0f;

  //could be as a parameter class
  IMAGE_U = 250;
  IMAGE_V = 250;
  x_scale = 900;
  y_scale = 900;
  focal_length = 1.;
  camera_dist= 200;

  verbose = false;
}

// Returns a face number if a point is on the surface of a box [0,1,2,3,4,5],
// -1 otherwise (not on the surface case)
int bvxm_synth_world_generator::on_box_surface(vgl_box_3d<double> box, vgl_point_3d<double> v)
{
  // create a box a size smaller
  vgl_box_3d<double> in_box;
  constexpr int thickness = 1;
  in_box.set_min_point(vgl_point_3d<double>(box.min_x()+thickness, box.min_y()+thickness, box.min_z()+thickness));
  in_box.set_max_point(vgl_point_3d<double>(box.max_x()-thickness, box.max_y()-thickness, box.max_z()-thickness));

  if (box.contains(v) && !in_box.contains(v))
  {
    // find the face index
    if ((box.min_z() <= v.z()) && (v.z() <= box.min_z()+thickness))
      return 0;

    if ((box.max_z()-thickness <= v.z()) && (v.z() <= box.max_z()))
      return 0;//5;

    if ((box.min_y() <= v.y()) && (v.y() <= box.min_y()+thickness))
      return 1;

    if ((box.max_y()-thickness <= v.y()) && (v.y() <= box.max_y()))
      return 1;//3;

    if ((box.min_x() <= v.x()) && (v.x() <= box.min_x()+thickness))
      return 2;//4;

    if ((box.max_x()-thickness <= v.x()) && (v.x() <= box.max_x()))
      return 2;
  }
  return -1;
}

int bvxm_synth_world_generator::in_box(vgl_box_3d<double> box, vgl_point_3d<double> v)
{
  if (box.contains(v))
    return 1;
  else
    return -1;
}

void bvxm_synth_world_generator::generate_persp_camera(
                           double focal_length,
                           vgl_point_2d<double>& pp,  //principal point
                           double x_scale, double y_scale,
                           vgl_point_3d<double>& camera_center,
                           vpgl_perspective_camera<double>& cam)
{
  vpgl_calibration_matrix<double> K(focal_length,pp, x_scale, y_scale);
  cam.set_calibration(K);
  cam.set_camera_center(camera_center);
}

vpgl_rational_camera<double>
bvxm_synth_world_generator::perspective_to_rational(vpgl_perspective_camera<double>& cam_pers)
{
  vnl_matrix_fixed<double,3,4> cam_pers_matrix = cam_pers.get_matrix();
  std::vector<double> neu_u,den_u,neu_v,den_v;
  double x_scale = 1.0, x_off = 0.0,
         y_scale = 1.0, y_off = 0.0,
         z_scale = 1.0, z_off = 0.0,
         u_scale = 1.0, u_off = 0.0,
         v_scale = 1.0, v_off = 0.0;

  for (int i=0; i<20; i++) {
    neu_u.push_back(0.0);
    neu_v.push_back(0.0);
    den_u.push_back(0.0);
    den_v.push_back(0.0);
  }

  int vector_map[] = {9,15,18,19};

  for (int i=0; i<4; i++) {
    neu_u[vector_map[i]] = cam_pers_matrix(0,i);
    neu_v[vector_map[i]] = cam_pers_matrix(1,i);
    den_u[vector_map[i]] = cam_pers_matrix(2,i);
    den_v[vector_map[i]] = cam_pers_matrix(2,i);
  }

  vpgl_rational_camera<double> cam_rat(neu_u,den_u,neu_v,den_v,
                                       x_scale,x_off,y_scale,y_off,z_scale,z_off,
                                       u_scale,u_off,v_scale,v_off);
  return cam_rat;
}


std::vector<vpgl_camera_double_sptr >
bvxm_synth_world_generator::generate_cameras_z(vgl_box_3d<double>& world)
{
  vgl_point_2d<double> principal_point(IMAGE_U/2., IMAGE_V/2.);

  vgl_point_3d<double> centroid = world.centroid();
  double x,z;
  double alpha = (vnl_math::pi/8.) * 3;
  double delta_alpha = vnl_math::pi/40.;
  std::vector<vgl_point_3d<double> > centers;
  for (unsigned i=0; i<11; i++) {
    x = camera_dist*std::cos(alpha);
    z = camera_dist*std::sin(alpha);
    centers.emplace_back(centroid.x()+x, centroid.y(), centroid.z()+z);
    std::cout << centers[i] << std::endl;
    alpha += delta_alpha;
  }

  vgl_box_2d<double> bb;
  std::vector<vpgl_camera_double_sptr> rat_cameras;
  for (auto camera_center : centers)
  {
    vpgl_perspective_camera<double> persp_cam;
    generate_persp_camera(focal_length,principal_point, x_scale, y_scale, camera_center, persp_cam);
    persp_cam.look_at(vgl_homg_point_3d<double>(centroid));
    vpgl_rational_camera<double>* rat_cam = new vpgl_rational_camera<double>(perspective_to_rational(persp_cam));
    rat_cameras.emplace_back(rat_cam);

    std::vector<vgl_point_3d<double> > corners = bvxm_util::corners_of_box_3d(world);
    for (auto c : corners) {
      double u,v, u2, v2;
      persp_cam.project(c.x(), c.y() ,c.z(), u, v);
      rat_cam->project(c.x(), c.y() ,c.z(), u2, v2);
      bb.add(vgl_point_2d<double> (u,v));

      if (verbose)
      {
        std::cout << "Perspective [" << u << ',' << v << "]\n"
                 << "Rational [" << u2 << ',' << v2 << "]\n\n";
      }
    }
    std::cout << bb << std::endl;
  }
  return rat_cameras;
}

std::vector<vpgl_camera_double_sptr >
bvxm_synth_world_generator::generate_cameras_yz(vgl_box_3d<double>& world)
{
  vgl_point_2d<double> principal_point(IMAGE_U/2., IMAGE_V/2.);

  vgl_point_3d<double> centroid = world.centroid();
  std::cout << "centroid: " << centroid << std::endl;
  double x,y;
  double alpha = (vnl_math::pi/8.) * 3;
  double delta_alpha = vnl_math::pi/40.;
  std::vector<vgl_point_3d<double> > centers;

  for (unsigned i=0; i<num_train_images_; i++) {
    x = camera_dist*std::cos(alpha);
    y = camera_dist*std::sin(alpha);
    centers.emplace_back(x+centroid.x(), y+centroid.y(), 450+centroid.z());
  if (verbose)
    std::cout << centers[i] << std::endl;

    alpha += delta_alpha;
  }

  vgl_box_2d<double> bb;
  std::vector<vpgl_camera_double_sptr> persp_cameras;
  for (unsigned i=0; i<centers.size(); i++)
  {
    vgl_point_3d<double> camera_center  = centers[i];
    vpgl_perspective_camera<double> persp_cam;
    generate_persp_camera(focal_length,principal_point, x_scale, y_scale, camera_center, persp_cam);
    persp_cam.look_at(vgl_homg_point_3d<double>(centroid));
    persp_cameras.push_back(new vpgl_perspective_camera<double>(persp_cam));

    //save the camera to file
    std::stringstream cam_file;
    vul_file::make_directory("./cams");
    cam_file <<world_dir_ << "/cam_" << i << ".txt";
    std::ofstream cam_out(cam_file.str().c_str());
    cam_out << persp_cam;

    if (verbose) {
      std::vector<vgl_point_3d<double> > corners = bvxm_util::corners_of_box_3d<double>(world);
      for (auto c : corners) {
        double u,v;
        persp_cam.project(c.x(), c.y() ,c.z(), u, v);
        bb.add(vgl_point_2d<double> (u,v));
        std::cout << "Perspective [" << u << ',' << v << "]\n"<< std::endl;
      }
      std::cout << bb << std::endl;
    }
  }
  return persp_cameras;
}

bool bvxm_synth_world_generator::update(vgl_vector_3d<unsigned> grid_size,
                                        const bvxm_voxel_world_sptr& world,
                                        bvxm_voxel_grid<float>* intensity_grid,
                                        bvxm_voxel_grid<float>*  /*ocp_grid*/, // FIXME - unused parameter
                                        bvxm_voxel_grid<apm_datatype>* apm_grid,
                                        std::vector<vpgl_camera_double_sptr>& cameras,
                                        std::vector <vil_image_view_base_sptr>& image_set,
                                        unsigned int bin_num)
{
  apm_datatype sample;
  apm_grid->initialize_data(sample);

  bvxm_voxel_grid<apm_datatype>::iterator apm_slab_it;
  bvxm_voxel_grid<float>::iterator obs_it = intensity_grid->begin();
  //slab thickness is 1
  auto* weight = new bvxm_voxel_slab<float>(grid_size.x(),grid_size.y(),1);
  weight->fill(1);

  for (apm_slab_it = apm_grid->begin(); apm_slab_it != apm_grid->end(); ++apm_slab_it,++obs_it) {
    bvxm_mog_grey_processor processor;
    /* bool update_status = */ processor.update(*apm_slab_it, *obs_it, *weight);
  }

  std::string path = world_dir_ + "/test_img";

  if (gen_images_) {
    for (unsigned i=0; i<cameras.size(); i++) {
      vil_image_view_base_sptr img_arg;
      auto* mask = new vil_image_view<float>(IMAGE_U, IMAGE_V);
      vil_image_view_base_sptr expected = new vil_image_view<unsigned char>(IMAGE_U, IMAGE_V);
      bvxm_image_metadata camera(img_arg, cameras[i]);

      world->expected_image<APM_MOG_GREY>(camera, expected, *mask, bin_num);
      std::stringstream s;
      s << path << i << ".tif";
      vil_save(*expected, s.str().c_str());
      image_set.push_back(expected);
    }
  }
  return true;
}


void bvxm_synth_world_generator::gen_texture_map(vgl_box_3d<double> box,
                                                 std::vector<std::vector<float> >& intens_map_bt,
                                                 std::vector<std::vector<float> >& intens_map_side1,
                                                 std::vector<std::vector<float> >& intens_map_side2,
                                                 bool gen_rand, float app_val)
{
  // generate intensity maps
  unsigned upw = (unsigned)std::ceil(box.width()/8)+1;
  unsigned uph = (unsigned)std::ceil(box.height()/8)+1;
  unsigned upd = (unsigned)std::ceil(box.depth()/8)+1;

  intens_map_bt.resize(upw);
  intens_map_side1.resize(upw);
  intens_map_side2.resize(upw);

  if (verbose)
    std::cout << box.width() << ' ' << box.depth() << ' ' << box.height() << std::endl;

  if (gen_rand) {
    for (unsigned i=0; i<upw;i++) {
      intens_map_bt[i].resize(uph);
      for (unsigned j=0; j<uph;j++) {
        intens_map_bt[i][j] = (float)((rand() % 85)/255.0);
      }
    }

    for (unsigned i=0; i<upw;i++) {
      intens_map_side1[i].resize(upd);
      for (unsigned j=0; j<upd;j++) {
        intens_map_side1[i][j] = (float)((rand() % 85)/255.0 + 0.4);
      }
    }

    for (unsigned i=0; i<uph;i++) {
      intens_map_side2[i].resize(upd);
      for (unsigned j=0; j<upd;j++) {
        intens_map_side2[i][j] = (float)((rand() % 85)/255.0 + 0.7);
        if (intens_map_side2[i][j] > 1.0f)
          intens_map_side2[i][j] = 0.99f;
      }
    }
  }
  else {
    for (unsigned i=0; i<upw;i++) {
      intens_map_bt[i].resize(uph);
      for (unsigned j=0; j<uph;j++) {
        intens_map_bt[i][j] = app_val;
      }
    }

    for (unsigned i=0; i<upw;i++) {
      intens_map_side1[i].resize(upd);
      for (unsigned j=0; j<upd;j++) {
        intens_map_side1[i][j] = app_val;
      }
    }

    for (unsigned i=0; i<uph;i++) {
      intens_map_side2[i].resize(upd);
      for (unsigned j=0; j<upd;j++) {
        intens_map_side2[i][j] = app_val;
        if (intens_map_side2[i][j] > 1.0f)
          intens_map_side2[i][j] = 0.99f;
      }
    }
  }
}

void bvxm_synth_world_generator::gen_voxel_world_2box(vgl_vector_3d<unsigned> /*grid_size*/,
                                                      vgl_box_3d<double> /*voxel_world*/,
                                                      bvxm_voxel_grid<float>* ocp_grid,
                                                      bvxm_voxel_grid<float>* intensity_grid,
                                                      unsigned minx, unsigned miny, unsigned minz,
                                                      unsigned dimx, unsigned dimy, unsigned dimz,
                                                      unsigned nx, unsigned ny, unsigned nz,
                                                      bool gen_2box)
{
  boxes_vector.clear();

  // fill with test data
  float init_val = 0.00;//0.01;
  ocp_grid->initialize_data(init_val);
  intensity_grid->initialize_data(init_val);

  bvxm_voxel_grid<float>::iterator ocp_slab_it;
  bvxm_voxel_grid<float>::iterator intensity_slab_it = intensity_grid->begin();

  //object (essentially two boxes) placed in the voxel world
  bvxm_util::generate_test_boxes<double>(minx,miny,minz,dimx,dimy,dimz,nx,ny,nz,boxes_vector,gen_2box);

  std::ofstream is((world_dir_ + "/intensity_grid.txt").c_str());

  assert(boxes_vector.size() == 2);

  vgl_box_3d<double> box=boxes_vector[0], top_box=boxes_vector[1];

  bool rand1=rand1_, rand2=rand2_; float app_val=app_val_;

  // generate intensity maps
  std::vector<std::vector<float> > intens_map_bt;
  std::vector<std::vector<float> > intens_map_side1;
  std::vector<std::vector<float> > intens_map_side2;

  gen_texture_map(box, intens_map_bt, intens_map_side1, intens_map_side2, rand1, app_val);

  std::vector<std::vector<float> > top_intens_map_bt;
  std::vector<std::vector<float> > top_intens_map_side1;
  std::vector<std::vector<float> > top_intens_map_side2;
  gen_texture_map(top_box, top_intens_map_bt, top_intens_map_side1, top_intens_map_side2, rand2, app_val);

  unsigned z=nz;
  for (ocp_slab_it = ocp_grid->begin();ocp_slab_it != ocp_grid->end();++ocp_slab_it,++intensity_slab_it)
  {
    --z;
    is << z << "--->" << std::endl;

    for (unsigned i=0; i<nx; i++)
    {
      is << std::endl;
      for (unsigned j=0; j<ny; j++)
      {
        int face1 = on_box_surface(box, vgl_point_3d<double>(i,j,z));
        int face2 = on_box_surface(top_box, vgl_point_3d<double>(i,j,z));

        if ((face1 != -1) || (face2 != -1))
        {
          if (!texture_map_) {
            (*intensity_slab_it)(i,j,0) = 1.0f;
          }
          else // texture map set to true
          {
            // create a checkerboard intensity
            if (face1 != -1) {
              if (face1 == 0) {
                int a = int(i-box.min_x())/8;
                int b = int(j-box.min_y())/8;
                (*intensity_slab_it)(i,j,0) = intens_map_bt[a][b];
              }
              else if (face1 == 1) {
                int a = int(i-box.min_x())/8;
                int b = int(z-box.min_z())/8;
                (*intensity_slab_it)(i,j,0) = intens_map_side1[a][b];
              }
              else {
                int a = int(j-box.min_y())/8;
                int b = int(z-box.min_z())/8;
                (*intensity_slab_it)(i,j,0) = intens_map_side2[a][b];
              }
            }
            else {
              if (face2 == 0) {
                int a = int(i-top_box.min_x())/8;
                int b = int(j-top_box.min_y())/8;
                (*intensity_slab_it)(i,j,0) = top_intens_map_bt[a][b];
              }
              else if (face2 == 1) {
                int a = int(i-top_box.min_x())/8;
                int b = int(z-top_box.min_z())/8;
                (*intensity_slab_it)(i,j,0) = top_intens_map_side1[a][b];
              }
              else {
                int a = int(j-top_box.min_y())/8;
                int b = int(z-top_box.min_z())/8;
                (*intensity_slab_it)(i,j,0) = top_intens_map_side2[a][b];
              }
            }
          }
          (*ocp_slab_it)(i,j,0) = 1.0f;
          is << " x" ;
        }
         else
           is << " 0";
      }
    }
  }
  std::cout << "grid done." << std::endl;
}

void bvxm_synth_world_generator::gen_voxel_world_plane(vgl_vector_3d<unsigned> /*grid_size*/,
                                                       vgl_box_3d<double> /*voxel_world*/,
                                                       bvxm_voxel_grid<float>* ocp_grid,
                                                       bvxm_voxel_grid<float>* intensity_grid,
                                                       unsigned nx, unsigned ny, unsigned nz)
{
  // fill with test data
  float init_val = 0.0;
  ocp_grid->initialize_data(init_val);
  intensity_grid->initialize_data(init_val);

  bvxm_voxel_grid<float>::iterator ocp_slab_it;
  bvxm_voxel_grid<float>::iterator intensity_slab_it = intensity_grid->begin();

  vgl_box_3d<double> plane_box(vgl_point_3d<double> (20,20,24),
                               vgl_point_3d<double> (80, 80, 25));

  std::ofstream is((world_dir_ + "/intensity_grid.txt").c_str());

  unsigned z=nz;
  for (ocp_slab_it = ocp_grid->begin();
       ocp_slab_it != ocp_grid->end();
       ++ocp_slab_it,++intensity_slab_it)
  {
    z--;
    is << z << "--->" << std::endl;

    for (unsigned i=0; i<nx; i++) {
      is << std::endl;
      for (unsigned j=0; j<ny; j++) {
        int face1 = on_box_surface(plane_box, vgl_point_3d<double>(i,j,z));
        if (face1 != -1) {
          (*intensity_slab_it)(i,j,0) = 0.8f;
          (*ocp_slab_it)(i,j,0) = 1.0f;
          is << " x" ;
        }
        else
          is << " 0";
      }
    }
  }
  std::cout << "grid done." << std::endl;
}

bool
bvxm_synth_world_generator::gen_lidar_2box(vgl_vector_3d<unsigned> grid_size,
                                           const bvxm_voxel_world_sptr& /*world*/)
{
  vil_image_view<unsigned char> lidar(grid_size.x(), grid_size.y());
  lidar.fill((unsigned char)0);

  // Generate the bottom one
  for (auto box : boxes_vector) {
    double z = box.max_z();

    for (unsigned i=0; i<grid_size.x(); i++)
      for (unsigned j=0; j<grid_size.y(); j++){
        vgl_point_3d<double> p(i,j,z);
        if (box.contains(p)) {
          // check if there is already a higher d there
          if (lidar(i,j) < z)
            lidar(i,j) = (unsigned char)z;
        }
      }
  }

  std::string path = world_dir_ + "/lidar";
  vul_file::make_directory(world_dir_);
  std::stringstream s;
  s << path << ".tif";
  vil_save(lidar, s.str().c_str());

  return true;
}

bvxm_voxel_world_sptr bvxm_synth_world_generator::generate_world()
{
  bvxm_voxel_world_sptr world = new bvxm_voxel_world();
  vul_file::make_directory(world_dir_);

  vgl_vector_3d<unsigned> grid_size(nx_,ny_,nz_);
  vgl_box_3d<double> voxel_world(vgl_point_3d<double> (0,0,0),
                                 vgl_point_3d<double> (nx_, ny_, nz_));
  bvxm_world_params_sptr world_params = new bvxm_world_params();
  world_params->set_params(world_dir_,
                           vgl_point_3d<float> (0,0,0),
                           vgl_vector_3d<unsigned int>(nx_, ny_, nz_),
                           vox_length);

  world->set_params(world_params);
  world->clean_grids();

  unsigned scale=0;
  unsigned int bin = 0;

  //create a mog grid for appearance model and use appearance model processor update to properly initialize it
  bvxm_voxel_grid<float>* ocp_grid = static_cast<bvxm_voxel_grid<float>* >
      (world->get_grid<OCCUPANCY>(0,scale).as_pointer());

  bvxm_voxel_grid_base_sptr apm_grid_base = world->get_grid<APM_MOG_GREY>(bin,scale);

  auto *apm_grid = static_cast<bvxm_voxel_grid<apm_datatype>*>(apm_grid_base.ptr());

  bvxm_voxel_grid<float>* intensity_grid = new bvxm_voxel_grid<float>
      (world_dir_ + "/intensity.vox",grid_size);

  gen_voxel_world_2box(grid_size, voxel_world, ocp_grid, intensity_grid, minx_, miny_, minz_, dimx_, dimy_, dimz_, nx_, ny_, nz_, gen2_box_);
  std::vector<vpgl_camera_double_sptr> cameras = generate_cameras_yz(voxel_world);

  std::vector <vil_image_view_base_sptr> image_set_1;

  update(grid_size, world, intensity_grid, ocp_grid, apm_grid,
         cameras, image_set_1, bin);

  //world->save_occupancy_raw(world_dir_ + "/ocp.raw");

  return world;
}
