#include "bvxm_gen_synthetic_world_process.h"
#include <bvxm/bvxm_util.h>

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vul/vul_file.h>
#include <vgl/vgl_box_2d.h>

//hard-coded values ... should be removed and added as a parameter class
const int IMAGE_U = 200;
const int IMAGE_V = 200;
const double x_scale = 900;
const double y_scale = 900;
const double focal_length = 1.;
const double camera_dist= 200;

//size of the world
//unsigned nx=100, ny=100, nz=50;
const float vox_length = 1.0f;
vcl_vector<vgl_box_3d<double> > boxes;


bvxm_gen_synthetic_world_process::bvxm_gen_synthetic_world_process()
{
  //process takes 0 inputs
  input_data_.resize(0,brdb_value_sptr(0));
  input_types_.resize(0);

  //process has 1 output
  //output[0] : The voxel_world

  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "bvxm_voxel_world_sptr";

  parameters()->add("size world x", "nx", (unsigned)100);
  parameters()->add("size world y", "ny", (unsigned)100);
  parameters()->add("size world z", "nz", (unsigned)50);

  parameters()->add("box min x", "minx", (unsigned)10);
  parameters()->add("box min y", "miny", (unsigned)10);
  parameters()->add("box min z", "minz", (unsigned)10);

  parameters()->add("box dim x", "dimx", (unsigned)89);
  parameters()->add("box dim y", "dimy", (unsigned)89);
  parameters()->add("box dim z", "dimz", (unsigned)33);

  parameters()->add("generate 2 boxes", "gen2", true);
  parameters()->add("generate images", "genImages", true);

  parameters()->add("world_dir", "worlddir", vcl_string("./test_gen_synthetic_world"));

  parameters()->add("random texture on box1", "rand1", true);
  parameters()->add("random texture on box2", "rand2", true);
  parameters()->add("fixed appearance val", "appval", 0.3f);

}

// returns a face number if a point is on the surface of a box [0,1,2,3,4,5],
// -1 otherwise (not on the surface case)
int
bvxm_gen_synthetic_world_process::on_box_surface(vgl_box_3d<double> box, vgl_point_3d<double> v)
{
  // create a box a size smaller
  vgl_box_3d<double> in_box;
  const int thickness = 1;
  in_box.set_min_point(vgl_point_3d<double>(box.min_x()+thickness, box.min_y()+thickness, box.min_z()+thickness));
  in_box.set_max_point(vgl_point_3d<double>(box.max_x()-thickness, box.max_y()-thickness, box.max_z()-thickness));

  bool on = false;
  if (box.contains(v) && !in_box.contains(v))
  {
    on = true;

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

int
bvxm_gen_synthetic_world_process::in_box(vgl_box_3d<double> box, vgl_point_3d<double> v)
{
  if (box.contains(v))
    return 1;
  else
    return -1;
}

void
bvxm_gen_synthetic_world_process::generate_persp_camera(double focal_length,
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
bvxm_gen_synthetic_world_process::perspective_to_rational(vpgl_perspective_camera<double>& cam_pers)
{
  vnl_matrix_fixed<double,3,4> cam_pers_matrix = cam_pers.get_matrix();
  vcl_vector<double> neu_u,den_u,neu_v,den_v;
  double x_scale = 1.0,
         x_off = 0.0,
         y_scale = 1.0,
         y_off = 0.0,
         z_scale = 1.0,
         z_off = 0.0,
         u_scale = 1.0,
         u_off = 0.0,
         v_scale = 1.0,
         v_off = 0.0;

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


#if 0
vcl_vector<vpgl_camera_double_sptr >
generate_cameras_z(vgl_box_3d<double>& world)
{
  vgl_point_2d<double> principal_point(IMAGE_U/2., IMAGE_V/2.);

  vgl_point_3d<double> centroid = world.centroid();
  double x,z;
  double alpha = (vnl_math::pi/8.) * 3;
  double delta_alpha = vnl_math::pi/40.;
  vcl_vector<vgl_point_3d<double> > centers;
  for (unsigned i=0; i<11; i++) {
    x = camera_dist*vcl_cos(alpha);
    z = camera_dist*vcl_sin(alpha);
    centers.push_back(vgl_point_3d<double> (centroid.x()+x, centroid.y(), centroid.z()+z));
    vcl_cout << centers[i] << vcl_endl;
    alpha += delta_alpha;
  }

  vgl_box_2d<double> bb;
  vcl_vector<vpgl_camera_double_sptr> rat_cameras;
  for (unsigned i=0; i<centers.size(); i++)
  {
    vgl_point_3d<double> camera_center  = centers[i];
    vpgl_perspective_camera<double> persp_cam;
    generate_persp_camera(focal_length,principal_point, x_scale, y_scale, camera_center, persp_cam);
    persp_cam.look_at(vgl_homg_point_3d<double>(centroid));
    vpgl_rational_camera<double>* rat_cam = new vpgl_rational_camera<double>(perspective_to_rational(persp_cam));
    rat_cameras.push_back(rat_cam);

    vcl_vector<vgl_point_3d<double> > corners = corners_of_box_3d(world);
    for (unsigned i=0; i<corners.size(); i++) {
      vgl_point_3d<double> c = corners[i];
      double u,v, u2, v2;
      persp_cam.project(c.x(), c.y() ,c.z(), u, v);
      rat_cam->project(c.x(), c.y() ,c.z(), u2, v2);
      bb.add(vgl_point_2d<double> (u,v));
#ifdef DEBUG
      vcl_cout << "Perspective [" << u << ',' << v << "]\n"
               << "Rational [" << u2 << ',' << v2 << "]\n" << vcl_endl;
#endif
    }
    vcl_cout << bb << vcl_endl;
  }
  return rat_cameras;
}
#endif

vcl_vector<vpgl_camera_double_sptr >
bvxm_gen_synthetic_world_process::generate_cameras_yz(vgl_box_3d<double>& world)
{
  vgl_point_2d<double> principal_point(IMAGE_U/2., IMAGE_V/2.);

  vgl_point_3d<double> centroid = world.centroid();
  double x,y;
  double alpha = (vnl_math::pi/8.) * 3;
  double delta_alpha = vnl_math::pi/40.;
  vcl_vector<vgl_point_3d<double> > centers;
  for (unsigned i=0; i<11; i++) {
    x = camera_dist*vcl_cos(alpha);
    y = camera_dist*vcl_sin(alpha);
    centers.push_back(vgl_point_3d<double> (x+centroid.x(), y+centroid.y(), 450+centroid.z()));
#ifdef DEBUG
    vcl_cout << centers[i] << vcl_endl;
#endif
    alpha += delta_alpha;
  }

  vgl_box_2d<double> bb;
  vcl_vector<vpgl_camera_double_sptr> rat_cameras;
  for (unsigned i=0; i<centers.size(); i++)
  {
    vgl_point_3d<double> camera_center  = centers[i];
    vpgl_perspective_camera<double> persp_cam;
    generate_persp_camera(focal_length,principal_point, x_scale, y_scale, camera_center, persp_cam);
    persp_cam.look_at(vgl_homg_point_3d<double>(centroid));
    vpgl_rational_camera<double>* rat_cam = new vpgl_rational_camera<double>(perspective_to_rational(persp_cam));
    rat_cameras.push_back(rat_cam);

    vcl_vector<vgl_point_3d<double> > corners = bvxm_util::corners_of_box_3d<double>(world);
    for (unsigned i=0; i<corners.size(); i++) {
      vgl_point_3d<double> c = corners[i];
      double u,v, u2, v2;
      persp_cam.project(c.x(), c.y() ,c.z(), u, v);
      rat_cam->project(c.x(), c.y() ,c.z(), u2, v2);
      bb.add(vgl_point_2d<double> (u,v));
#ifdef DEBUG
      vcl_cout << "Perspective [" << u << ',' << v << "]\n"
               << "Rational [" << u2 << ',' << v2 << "]\n" << vcl_endl;
#endif
    }
#ifdef DEBUG
    vcl_cout << bb << vcl_endl;
#endif
  }
  return rat_cameras;
}

bool
bvxm_gen_synthetic_world_process::gen_images(vgl_vector_3d<unsigned> grid_size,
                                             bvxm_voxel_world_sptr world,
                                             bvxm_voxel_grid<float>* intensity_grid,
                                             bvxm_voxel_grid<float>* ocp_grid,
                                             bvxm_voxel_grid<apm_datatype>* apm_grid,
                                             vcl_vector<vpgl_camera_double_sptr>& cameras,
                                             vcl_vector <vil_image_view_base_sptr>& image_set,
                                             unsigned int bin_num)
{
  apm_datatype sample;
  apm_grid->initialize_data(sample);

  bvxm_voxel_grid<apm_datatype>::iterator apm_slab_it;
  bvxm_voxel_grid<float>::iterator obs_it = intensity_grid->begin();
  bool update_status;
  //slab thickness is 1
  bvxm_voxel_slab<float>* weight = new bvxm_voxel_slab<float>(grid_size.x(),grid_size.y(),1);
  weight->fill(1);

  for (apm_slab_it = apm_grid->begin(); apm_slab_it != apm_grid->end(); ++apm_slab_it,++obs_it) {
    bvxm_mog_grey_processor processor;
    update_status = processor.update(*apm_slab_it, *obs_it, *weight);
  }

  //vcl_string path = "./test_gen_synthetic_world/test_img";
  vcl_string dir; parameters()->get_value("worlddir", dir);
  vcl_string path = dir + "test_img";

  for (unsigned i=0; i<cameras.size(); i++) {
    vil_image_view_base_sptr img_arg;
    vil_image_view<float>* mask = new vil_image_view<float>(IMAGE_U, IMAGE_V);
    vil_image_view_base_sptr expected = new vil_image_view<unsigned char>(IMAGE_U, IMAGE_V);
    bvxm_image_metadata camera(img_arg, cameras[i]);

    world->expected_image<APM_MOG_GREY>(camera, expected, *mask, bin_num);
    vcl_stringstream s;
    s << path << i << '_' << bin_num << ".tif";
    vil_save(*expected, s.str().c_str());
    image_set.push_back(expected);
  }
  return true;
}

bool
bvxm_gen_synthetic_world_process::reconstruct_world( bvxm_voxel_world_sptr recon_world,
                                                     vcl_vector<vpgl_camera_double_sptr>& cameras,
                                                     vcl_vector <vil_image_view_base_sptr>& image_set,
                                                     unsigned int bin_num)
{
  vcl_string recon_path = "recon_world/test_img";
  vcl_string camera_path = "test_gen_cameras/camera";

  for (unsigned i = 0;i<cameras.size();i++)
  {
    bvxm_image_metadata observation(image_set[i],cameras[i]);
    recon_world->update<APM_MOG_GREY>(observation,bin_num);
  }

  for (unsigned i=0; i<cameras.size(); i++) {
    vil_image_view_base_sptr img_arg;
    vil_image_view<float> mask(IMAGE_U, IMAGE_V);
    vil_image_view_base_sptr expected = new vil_image_view<unsigned char>(IMAGE_U, IMAGE_V);
    bvxm_image_metadata camera(img_arg, cameras[i]);

    recon_world->expected_image<APM_MOG_GREY>(camera, expected, mask);
    vcl_stringstream s;
    s << recon_path << i << '_' << bin_num << ".tif";
    vcl_stringstream c;
    c << camera_path << i << ".rpc";
    vil_save(*expected, s.str().c_str());
    vpgl_rational_camera<double>* cam = static_cast<vpgl_rational_camera<double>*> (cameras[i].as_pointer());
    cam->save(c.str().c_str());
    image_set.push_back(expected);
  }
  return true;
}

void
bvxm_gen_synthetic_world_process::gen_texture_map(vgl_box_3d<double> box,
                                                  vcl_vector<vcl_vector<float> >& intens_map_bt,
                                                  vcl_vector<vcl_vector<float> >& intens_map_side1,
                                                  vcl_vector<vcl_vector<float> >& intens_map_side2, bool gen_rand, float app_val)
{
  // generate intensity maps
  unsigned upw = (unsigned)vcl_ceil(box.width()/8)+1;
  unsigned uph = (unsigned)vcl_ceil(box.height()/8)+1;
  unsigned upd = (unsigned)vcl_ceil(box.depth()/8)+1;

  intens_map_bt.resize(upw);
  intens_map_side1.resize(upw);
  intens_map_side2.resize(upw);

#ifdef DEBUG
  vcl_cout << box.width() << ' ' << box.depth() << ' ' << box.height() << vcl_endl;
#endif

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
  } else {
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

void
bvxm_gen_synthetic_world_process::gen_voxel_world_2box(vgl_vector_3d<unsigned> grid_size,
                                                       vgl_box_3d<double> voxel_world,
                                                       bvxm_voxel_grid<float>* ocp_grid,
                                                       bvxm_voxel_grid<float>* intensity_grid,
                                                       unsigned minx, unsigned miny, unsigned minz, unsigned dimx, unsigned dimy, unsigned dimz, 
                                                       unsigned nx, unsigned ny, unsigned nz,
                                                       bool gen_2box)
{
  boxes.clear();

  // fill with test data
  float init_val = 0.00;//0.01;
  ocp_grid->initialize_data(init_val);
  intensity_grid->initialize_data(init_val);

  bvxm_voxel_grid<float>::iterator ocp_slab_it;
  bvxm_voxel_grid<float>::iterator intensity_slab_it = intensity_grid->begin();

#if 0
  //object (essentially a box) placed in the voxel world
  vgl_box_3d<double> box(vgl_point_3d<double> (10,10,10),
                         vgl_point_3d<double> (89,89,33));
  boxes.push_back(box);

  vgl_box_3d<double> top_box;
  create_top_box(box, top_box, 29,29,15);
  boxes.push_back(top_box);
#endif // 0

  //object (essentially two boxes) placed in the voxel world
  //bvxm_util::generate_test_boxes<double>(10,10,10,89,89,33,nx,ny,nz,boxes);
  bvxm_util::generate_test_boxes<double>(minx,miny,minz,dimx,dimy,dimz,nx,ny,nz,boxes,gen_2box);

  //vcl_ofstream is("test_gen_synthetic_world/intensity_grid.txt");
  vcl_string dir; parameters()->get_value("worlddir", dir);
  vcl_ofstream is((dir + "intensity_grid.txt").c_str());

  assert(boxes.size() == 2);

  vgl_box_3d<double> box=boxes[0], top_box=boxes[1];

  bool rand1, rand2;  float app_val;
  parameters()->get_value("rand1", rand1);
  parameters()->get_value("rand2", rand2);
  parameters()->get_value("appval", app_val);

  // generate intensity maps
  vcl_vector<vcl_vector<float> > intens_map_bt;
  vcl_vector<vcl_vector<float> > intens_map_side1;
  vcl_vector<vcl_vector<float> > intens_map_side2;

  gen_texture_map(box, intens_map_bt, intens_map_side1, intens_map_side2, rand1, app_val);

  vcl_vector<vcl_vector<float> > top_intens_map_bt;
  vcl_vector<vcl_vector<float> > top_intens_map_side1;
  vcl_vector<vcl_vector<float> > top_intens_map_side2;
  gen_texture_map(top_box, top_intens_map_bt, top_intens_map_side1, top_intens_map_side2, rand2, app_val);

  unsigned z=nz;
  for (ocp_slab_it = ocp_grid->begin();ocp_slab_it != ocp_grid->end();++ocp_slab_it,++intensity_slab_it)
  {
    --z;
    is << z << "--->" << vcl_endl;

    for (unsigned i=0; i<nx; i++)
    {
      is << vcl_endl;
      for (unsigned j=0; j<ny; j++)
      {
        int face1 = on_box_surface(box, vgl_point_3d<double>(i,j,z));
        int face2 = on_box_surface(top_box, vgl_point_3d<double>(i,j,z));
        // create a checkerboard intensity
        if ((face1 != -1) || (face2 != -1))
        {
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
            //(*intensity_slab_it)(i,j,0) = face_intens[5-face2];
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
#if 0
          // generate a random intensity
          float r = (rand() % 256)/255.0;    // float face_intens[6] = {0.3, 0.45, 0.6, 0.75, 0.85, 1.0};
          if (face1 != -1)
            (*intensity_slab_it)(i,j,0) = r; // face_intens[face1];
          else
            (*intensity_slab_it)(i,j,0) = r; // face_intens[5-face2];
#endif
          (*ocp_slab_it)(i,j,0) = 1.0f;
          is << " x" ;
        }
        else
          is << " 0";
      }
    }
  }
  vcl_cout << "grid done." << vcl_endl;
}

void bvxm_gen_synthetic_world_process::gen_voxel_world_plane(vgl_vector_3d<unsigned> grid_size,
                                                             vgl_box_3d<double> voxel_world,
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

  //vcl_ofstream is("test_gen_synthetic_world/intensity_grid.txt");
  vcl_string dir; parameters()->get_value("worlddir", dir);
  vcl_ofstream is((dir + "intensity_grid.txt").c_str());

  unsigned z=nz;
  for (ocp_slab_it = ocp_grid->begin();
       ocp_slab_it != ocp_grid->end();
       ++ocp_slab_it,++intensity_slab_it)
  {
    z--;
    is << z << "--->" << vcl_endl;

    for (unsigned i=0; i<nx; i++) {
      is << vcl_endl;
      for (unsigned j=0; j<ny; j++) {
        int face1 = on_box_surface(plane_box, vgl_point_3d<double>(i,j,z));
        if (face1 != -1) {
          (*intensity_slab_it)(i,j,0) = 0.8f;
          (*ocp_slab_it)(i,j,0) = 1.0f;
          is << " x" ;
        } else
          is << " 0";
      }
    }
  }
  vcl_cout << "grid done." << vcl_endl;
}

bool
bvxm_gen_synthetic_world_process::test_reconstructed_ocp(bvxm_voxel_world_sptr recon_world)
{
  bvxm_voxel_grid<float>* ocp_grid =
    dynamic_cast<bvxm_voxel_grid<float>*>(recon_world->get_grid<OCCUPANCY>(0).ptr());

  vxl_uint_32 nx = ocp_grid->grid_size().x();
  vxl_uint_32 ny = ocp_grid->grid_size().y();
  vxl_uint_32 nz = ocp_grid->grid_size().z();

  // iterate through slabs
  unsigned i = 60, j=60, k=nz;
  bvxm_voxel_grid<float>::iterator ocp_slab_it = ocp_grid->begin();
  for (ocp_slab_it = ocp_grid->begin(); ocp_slab_it != ocp_grid->end(); ++ocp_slab_it ) {
    k--;
    float ocp = (*ocp_slab_it)(i,j,0);
    vcl_cout << "z=" << k << "  " << ocp << vcl_endl;
    for (unsigned b=0; b<boxes.size(); b++) {
      if (on_box_surface(boxes[b], vgl_point_3d<double>(i,j,k)) != -1)
        vcl_cout << "ON " << b << vcl_endl;
    }
  }
  return true;
}

bool bvxm_gen_synthetic_world_process::execute()
{
  // create the directory under build to put the intermediate files and the generated images
  //vcl_string model_dir("./test_gen_synthetic_world");
  vcl_string model_dir; parameters()->get_value("worlddir", model_dir);
  vul_file::make_directory(model_dir);

  vcl_string recon_model_dir("./recon_world");
  vul_file::make_directory(recon_model_dir);

  vul_file::make_directory("./test_gen_cameras");

  unsigned nx, ny, nz;
  parameters()->get_value("nx", nx);
  parameters()->get_value("ny", ny);
  parameters()->get_value("nz", nz);

  unsigned minx, miny, minz, dimx, dimy, dimz;
  parameters()->get_value("minx", minx);
  parameters()->get_value("miny", miny);
  parameters()->get_value("minz", minz);

  parameters()->get_value("dimx", dimx);
  parameters()->get_value("dimy", dimy);
  parameters()->get_value("dimz", dimz);

  bool gen_image_outs;
  parameters()->get_value("genImages", gen_image_outs);

  bool gen2_box;
  parameters()->get_value("gen2", gen2_box);

  vgl_vector_3d<unsigned> grid_size(nx,ny,nz);
  vgl_box_3d<double> voxel_world(vgl_point_3d<double> (0,0,0),
                                 vgl_point_3d<double> (nx, ny, nz));

  bvxm_world_params_sptr world_params = new bvxm_world_params();
  world_params->set_params(model_dir,
                           vgl_point_3d<float> (0,0,0),
                           vgl_vector_3d<unsigned int>(nx, ny, nz),
                           vox_length);
  bvxm_voxel_world_sptr world = new bvxm_voxel_world();
  world->set_params(world_params);
  world->clean_grids();

  unsigned int bin_num_1 = 0,bin_num_2 = 10;

  //create an mog grid for appearance model and use appearence model processor update to properly initialize it
  bvxm_voxel_grid<float>* ocp_grid = static_cast<bvxm_voxel_grid<float>* >
    (world->get_grid<OCCUPANCY>(0).as_pointer());

  bvxm_voxel_grid<apm_datatype>* apm_grid_1 = static_cast<bvxm_voxel_grid<apm_datatype>* >
    (world->get_grid<APM_MOG_GREY>(bin_num_1).as_pointer());

    bvxm_voxel_grid<apm_datatype>* apm_grid_2 = static_cast<bvxm_voxel_grid<apm_datatype>* >
    (world->get_grid<APM_MOG_GREY>(bin_num_2).as_pointer());

  bvxm_voxel_grid<float>* intensity_grid = new bvxm_voxel_grid<float>
    (model_dir + "intensity.vox",grid_size);

  gen_voxel_world_2box(grid_size, voxel_world, ocp_grid, intensity_grid, minx, miny, minz, dimx, dimy, dimz, nx, ny, nz, gen2_box);
  vcl_vector<vpgl_camera_double_sptr> cameras = generate_cameras_yz(voxel_world);

  vcl_vector <vil_image_view_base_sptr> image_set_1,image_set_2;

  if (gen_image_outs) {
    // generate images from synthetic world
    gen_images(grid_size, world, intensity_grid, ocp_grid, apm_grid_1,
             cameras, image_set_1, bin_num_1);
  }

  //world->save_occupancy_raw("./test_gen_synthetic_world/ocp.raw");
  vcl_string dir; parameters()->get_value("worlddir", dir);
  world->save_occupancy_raw(dir + "ocp.raw");

  //store output
  output_data_[0] = new brdb_value_t<bvxm_voxel_world_sptr>(world);

  return true;
}

static
bool gen_lidar_2box(vgl_vector_3d<unsigned> grid_size
                    , bvxm_voxel_world_sptr world
#if 0
                    , vcl_vector<vpgl_camera_double_sptr>& cameras
                    , vcl_vector <vil_image_view_base_sptr>& image_set
                    , unsigned int bin_num
#endif
                   )
{
  vil_image_view<unsigned char> lidar(grid_size.x(), grid_size.y());
  lidar.fill((unsigned char)0);

  // Generate the bottom one
  for (unsigned int b=0; b<boxes.size(); b++) {
    vgl_box_3d<double> box = boxes[b];
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

  vcl_string path = "./test_gen_synthetic_world/test_lidar";
  vcl_stringstream s;
  s << path << ".tif";
  vil_save(lidar, s.str().c_str());

  return true;
}

bool bvxm_gen_synthetic_world_process::test()
{
  execute();

  unsigned nx, ny, nz;
  parameters()->get_value("nx", nx);
  parameters()->get_value("ny", ny);
  parameters()->get_value("nz", nz);
  vcl_string model_dir; parameters()->get_value("worlddir", model_dir);

  bvxm_world_params_sptr world_params = new bvxm_world_params();
  world_params->set_params(model_dir,
                           vgl_point_3d<float> (0,0,0),
                           vgl_vector_3d<unsigned int>(nx, ny, nz),
                           vox_length);
  bvxm_voxel_world_sptr world = new bvxm_voxel_world();
  world->set_params(world_params);
  world->clean_grids();

  gen_lidar_2box(vgl_vector_3d<unsigned>(nx,ny,nz), world);

  bvxm_voxel_world_sptr recon_world = new bvxm_voxel_world();

  bvxm_world_params_sptr recon_world_params = new bvxm_world_params();
  recon_world_params->set_params("./recon_world",vgl_point_3d<float> (0,0,0),
                                 vgl_vector_3d<unsigned int>(nx, ny, nz), vox_length);

  recon_world->set_params(recon_world_params);
  recon_world->clean_grids();

  unsigned int bin_num_1 = 0, bin_num_2 = 10;

  recon_world->get_grid<APM_MOG_GREY>(bin_num_1);
  recon_world->get_grid<APM_MOG_GREY>(bin_num_2);
  recon_world->get_grid<OCCUPANCY>(0);

  //reconstruct the world from synthetic images and the apm grid stored in bin bin_num_1
  vcl_vector <vil_image_view_base_sptr> image_set_1;
  vgl_box_3d<double> voxel_world(vgl_point_3d<double> (0,0,0),
                                 vgl_point_3d<double> (nx, ny, nz));
  vcl_vector<vpgl_camera_double_sptr> cameras = generate_cameras_yz(voxel_world);
  reconstruct_world(recon_world,cameras, image_set_1,bin_num_1);
  recon_world->save_occupancy_raw("./recon_world/ocp1.raw");

  return test_reconstructed_ocp(recon_world);
}
