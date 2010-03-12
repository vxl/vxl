// This is brl/bseg/bvxm/pro/processes/bvxm_estimate_camera_process.cxx
#include "bvxm_estimate_camera_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>

#include <brip/brip_vil_float_ops.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <bvxm/bvxm_edge_ray_processor.h>

#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vgl/algo/vgl_homg_operators_3d.h>

#include <vcl_cstdio.h>

#include <vul/vul_timer.h>
#include <vnl/vnl_inverse.h>
#include <vil/vil_flip.h>

#include <vgl/vgl_distance.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/algo/vnl_amoeba.h>

// global variables
int nx = 0;
int ny = 0;
int nz = 0;
double sx = 0.0;
double sy = 0.0;
double sz = 0.0;
double dx = 0.0;
double dy = 0.0;
double dz = 0.0;

// global constants
const double deg2rad = vnl_math::pi/180.0;
const double rad2deg = 1.0/deg2rad;
const double theta_range = 12.0*deg2rad;
const double theta_step = 0.05*deg2rad;
const double phi_range = 12.0*deg2rad;
const double phi_step = 0.05*deg2rad;
const double rot_range = 10.0*deg2rad;
const double rot_step = 1.0*deg2rad;
const int max_iter_rot_angle = 5;
const int max_iter_cam_center = 50;

template <class T>
vil_image_view<T> convert_to_spherical_coordinates(
  const vil_image_view<T> &img,
  const vpgl_perspective_camera<double> &cam,
  const double rotate)
{
  int theta_size = 2*vnl_math_ceil(theta_range/theta_step);
  int phi_size = 2*vnl_math_ceil(phi_range/phi_step);

  vil_image_view<T> imgs(theta_size,phi_size,1);
  imgs.fill((T)0);

  vnl_matrix<double> K = cam.get_calibration().get_matrix();
  vnl_matrix<double> K_inv = vnl_inverse<double>(K);

  vnl_matrix<double> R(3,3,0.0);
  R(0,0) = 1.0;
  R(1,1) = vcl_cos(rotate);
  R(1,2) = vcl_sin(rotate);
  R(2,1) = -vcl_sin(rotate);
  R(2,2) = vcl_cos(rotate);

  for(unsigned i=0; i<imgs.ni(); i++){
    for(unsigned j=0; j<imgs.nj(); j++){
      double curr_theta = (0.5*vnl_math::pi) - theta_range + (theta_step*(double)i);
      double curr_phi = -phi_range + (phi_step*(double)j);

      double x = vcl_sin(curr_theta)*vcl_cos(curr_phi);
      double y = vcl_sin(curr_theta)*vcl_sin(curr_phi);
      double z = vcl_cos(curr_theta);

      vnl_vector<double> curr_vector(3);
      curr_vector[0] = x;
      curr_vector[1] = y;
      curr_vector[2] = z;

      curr_vector = R*curr_vector;

      x = curr_vector[0];
      y = curr_vector[1];
      z = curr_vector[2];

      curr_vector[0] = -z;
      curr_vector[1] = y;
      curr_vector[2] = x;

      vnl_vector<double> curr_pixel = K*curr_vector;

      int u = vnl_math_rnd(curr_pixel[0]/curr_pixel[2]);
      int v = vnl_math_rnd(curr_pixel[1]/curr_pixel[2]);

      if(img.in_range(u,v)){
        imgs(i,j) = img(u,v);
      }
    }
  }

  return imgs;
}

vcl_vector<vgl_point_3d<double> > convert_3d_box_to_3d_points(const vgl_box_3d<double> box_3d)
{
  double box_x[2],box_y[2],box_z[2];
  box_x[0] = box_3d.min_x();
  box_y[0] = box_3d.min_y();
  box_z[0] = box_3d.min_z();
  box_x[1] = box_3d.max_x();
  box_y[1] = box_3d.max_y();
  box_z[1] = box_3d.max_z();

  vcl_vector<vgl_point_3d<double> > box_pts_3d;
  for (int i=0; i<2; i++) {
    for (int j=0; j<2; j++) {
      for (int k=0; k<2; k++) {
        box_pts_3d.push_back(vgl_point_3d<double>(box_x[i],box_y[j],box_z[k]));
      }
    }
  }

  return box_pts_3d;
}

vgl_polygon<double> convert_3d_box_to_2d_polygon(const vgl_box_3d<double> box_3d, 
                                                 const vpgl_perspective_camera<double> *cam){
  vcl_vector<vgl_point_3d<double> > box_pts_3d = convert_3d_box_to_3d_points(box_3d);

  vcl_vector<vgl_point_2d<double> > box_pts_2d;
  for (unsigned i=0; i<box_pts_3d.size(); i++) {
    double u,v;    
    cam->project(box_pts_3d[i].x(),box_pts_3d[i].y(),box_pts_3d[i].z(),u,v);
    vgl_point_2d<double> curr_pt_2d(u,v);
    box_pts_2d.push_back(curr_pt_2d);
  }

  vgl_convex_hull_2d<double> ch(box_pts_2d);
  vgl_polygon<double> polygon_2d = ch.hull();

  return polygon_2d;
}

void get_expected_edge_image(const bvxm_voxel_slab<float> &data, 
                             const vpgl_perspective_camera<double> *cam, 
                             vil_image_view<float> *img_eei)
{
  img_eei->fill(0.0f);

  int ni = img_eei->ni();
  int nj = img_eei->nj();
  
  // Layer version of expected edge image
  vgl_box_3d<double> box_3d(sx,sy,(sz+((double)nz)*dz),(sx+((double)nx)*dx),(sy+((double)ny)*dy),sz);
  vgl_polygon<double> poly_2d = convert_3d_box_to_2d_polygon(box_3d,cam);

  vgl_homg_plane_3d<double> plane_0(0.0,0.0,1.0,-sz);
  vgl_homg_plane_3d<double> plane_1(0.0,0.0,1.0,-sz-dz);

  for(int i=0; i<ni; i++){
    for(int j=0; j<nj; j++){
      if(poly_2d.contains(double(i),(double)j)){
        // following line 640 milliseconds
        vgl_line_3d_2_points<double> bp = cam->backproject(vgl_point_2d<double>((double)i,(double)j));
        vgl_homg_line_3d_2_points<double> bp_h(vgl_homg_point_3d<double>(bp.point1()),vgl_homg_point_3d<double>(bp.point2()));
        vgl_point_3d<double> pt_0(vgl_homg_operators_3d<double>::intersect_line_and_plane(bp_h,plane_0));
        vgl_point_3d<double> pt_1(vgl_homg_operators_3d<double>::intersect_line_and_plane(bp_h,plane_1));

        vgl_vector_3d<double> diff = pt_1 - pt_0;

        double start_x = pt_0.x();
        double start_y = pt_0.y();
        double start_z = pt_0.z();

        double diff_x = diff.x();
        double diff_y = diff.y();
        double diff_z = diff.z();

        for(int vz=0; vz<nz; vz++){
          int vx = (int)((((start_x + (diff_x*vz)) - sx)/dx) + 0.5f);
          if(vx<0 || vx>=nx){
            continue;
          }

          int vy = (int)((((start_y + (diff_y*vz)) - sy)/dy) + 0.5f);
          if(vy<0 || vy>=ny){
            continue;
          }

          // this line takes 2200 milliseconds
          // with float ***data it takes 1100 ms
          (*img_eei)(i,j) = vnl_math_max((*img_eei)(i,j),data(vx,vy,vz));
        }
      }
    }
  }

  // todo : convert it chi-sq probability (not necessary for improving accuracy
}

template <class T_from,class T_to>
void normalize_to_interval(const vil_image_view<T_from>& img_inp, 
                           vil_image_view<T_to>& img_out, 
                           float min, 
                           float max)
{
  assert(min<max);
  vil_image_view<float> img_temp;
  vil_convert_cast<T_from,float>(img_inp,img_temp);
  float min_img,max_img;
  vil_math_value_range<float>(img_temp,min_img,max_img);
  assert(min_img<max_img);

  float scale = (max-min)/(max_img-min_img);
  float offset = (min*max_img - min_img*max)/(max_img-min_img);

  vil_math_scale_and_offset_values<float,float>(img_temp,scale,offset);

  vil_convert_cast<float,T_to>(img_temp,img_out);
}

// offset values needs to be used as following:
//   img1(i,j) = img2(i+offset_x,j+offset_y)
template <class T1,class T2,class TR>
static vil_image_view<TR> estimate_offsets_fd(const vil_image_view<T1> &img1,
                                              const vil_image_view<T2> &img2,
                                              int &offset_x,
                                              int &offset_y,
                                              float &score)
{
  vil_image_view<float> img_1;
  normalize_to_interval<T1,float>(img1,img_1,0.0f,1.0f);
  vil_image_view<float> img_2;
  normalize_to_interval<T2,float>(img2,img_2,0.0f,1.0f);

  vil_image_view<float> img_1_p;
  brip_vil_float_ops::resize_to_power_of_two(img_1, img_1_p);

  vil_image_view<float> img_2_p;
  brip_vil_float_ops::resize_to_power_of_two(img_2, img_2_p);

  vil_image_view<float> img_2_h = vil_flip_lr(img_2_p);
  vil_image_view<float> img_2_hv = vil_flip_ud(img_2_h);

  vil_image_view<float> img_1_n(img_1_p);
  vil_image_view<float> img_2_n(img_2_hv);

  vil_math_normalise<float>(img_1_n);
  vil_math_normalise<float>(img_2_n);

  vil_image_view<float> img_1_mag;
  vil_image_view<float> img_1_phase;
  brip_vil_float_ops::fourier_transform(img_1_n,img_1_mag,img_1_phase);

  vil_image_view<float> img_2_mag;
  vil_image_view<float> img_2_phase;
  brip_vil_float_ops::fourier_transform(img_2_n,img_2_mag,img_2_phase);

  vil_image_view<float> img_cc;
  vil_image_view<float> img_cc_mag(img_1_n.ni(),img_1_n.nj(),1);
  vil_image_view<float> img_cc_phase(img_1_n.ni(),img_1_n.nj(),1);

  for(unsigned i=0; i<img_cc_mag.ni(); i++){
    for(unsigned j=0; j<img_cc_mag.nj(); j++){
      float a = img_1_mag(i,j)*vcl_cos(img_1_phase(i,j));
      float b = img_1_mag(i,j)*vcl_sin(img_1_phase(i,j));
      float c = img_2_mag(i,j)*vcl_cos(img_2_phase(i,j));
      float d = img_2_mag(i,j)*vcl_sin(img_2_phase(i,j));
      float res_real = (a*c)-(b*d);
      float res_img = (b*c)+(a*d);
      float res_mag = vcl_sqrt((res_real*res_real)+(res_img*res_img));
      float res_phase = vcl_atan2(res_img, res_real);         

      img_cc_mag(i,j) = res_mag;
      img_cc_phase(i,j) = res_phase;
    }
  }

  brip_vil_float_ops::inverse_fourier_transform(img_cc_mag,img_cc_phase,img_cc);

  score = vcl_numeric_limits<float>::min();

  for(unsigned i=0; i<img_cc.ni(); i++){
    for(unsigned j=0; j<img_cc.nj(); j++){
      if(img_cc(i,j)>score){
        score = img_cc(i,j);
        offset_x = i;
        offset_y = j;
      }
    }
  }
  if(offset_x > (int)img_1_mag.ni()/2){
    offset_x = offset_x - (int)img_1_mag.ni();
  }
  if(offset_y > (int)img_1_mag.nj()/2){
    offset_y = offset_y - (int)img_1_mag.nj();
  }

  vil_image_view<TR> img_2_to_1(img_2.ni(),img_2.nj(),1);
  img_2_to_1.fill((TR)0);

  for(unsigned i=0; i<img_2_to_1.ni(); i++){
    for(unsigned j=0; j<img_2_to_1.nj(); j++){
      int curr_i = (int)i + offset_x;
      int curr_j = (int)j + offset_y;
      if(img_2_to_1.in_range(curr_i,curr_j)){
        img_2_to_1(curr_i,curr_j) = img_2(i,j);
      }
    }
  }

  return img_2_to_1;
}

void convert_angles_to_vector(const double theta,
                              const double phi,
                              double &vx,
                              double &vy,
                              double &vz)
{
  vx = vcl_sin(theta)*vcl_cos(phi);
  vy = vcl_sin(theta)*vcl_sin(phi);
  vz = vcl_cos(theta);
}

static double edge_prob_cross_correlation(const vil_image_view<float> &img1, const vil_image_view<float> &img2){
  vil_image_view<float> img1n; img1n.deep_copy(img1);
  vil_image_view<float> img2n; img2n.deep_copy(img2);

  vil_math_normalise<float>(img1n);
  vil_math_normalise<float>(img2n);

  vil_image_view<float> img_product;
  vil_math_image_product(img1n,img2n,img_product);

  double img_product_sum;
  vil_math_sum<float,double>(img_product_sum,img_product,0);

  return img_product_sum;
}

static double estimate_rotation_angle(const vil_image_view<float> &img1c,
                                      const vil_image_view<float> &img2c)
{
  assert(img1c.ni()==img2c.ni());
  assert(img1c.nj()==img2c.nj());

  double center_x = 0.5*(double)img1c.ni();
  double center_y = 0.5*(double)img1c.nj();

  vil_image_view<float> img1;
  normalize_to_interval(img1c,img1,0.0f,1.0f);
  vil_image_view<float> img2;
  normalize_to_interval(img2c,img2,0.0f,1.0f);

  int rot_size = vnl_math_ceil(rot_range/rot_step);

  double best_score = 0.0;
  double best_rot = 0.0;

  for(int r=-rot_size; r<rot_size+1; r++){
    double curr_rot = rot_step*(double)r;

    vgl_h_matrix_2d<double> H;
    vgl_h_matrix_2d<double> H_temp;

    H.set_identity();
    
    H_temp.set_identity();
    H_temp.set_translation(-center_x,-center_y);
    H = H_temp*H;

    H_temp.set_identity();
    H_temp.set_rotation(curr_rot);
    H = H_temp*H;

    H_temp.set_identity();
    H_temp.set_translation(center_x,center_y);
    H = H_temp*H;

    vil_image_view<float> img2_rot(img2.ni(),img2.nj());

    brip_vil_float_ops::homography(img2, H, img2_rot,true,0.0f);

    double curr_score = edge_prob_cross_correlation(img1,img2_rot);

    if(curr_score > best_score){
      best_score = curr_score;
      best_rot = curr_rot;
    }
  }

  return best_rot;
}

void estimate_rotation_iterative(const bvxm_voxel_slab<float> &data,
                                 const vil_image_view<float>& img_e,
                                 vpgl_perspective_camera<double> *cam)
{
  int ni = img_e.ni();
  int nj = img_e.nj();

  for(int iter=0; iter<max_iter_rot_angle; iter++){
    vcl_cout << ".";
    vil_image_view<float> img_eei(ni,nj,1);
    vil_image_view<vxl_byte> img_temp(ni,nj,1);

    get_expected_edge_image(data,cam,&img_eei);

    // todo : use minimal size image here
    vil_image_view<float> img_eei_s = convert_to_spherical_coordinates(img_eei,*cam,0.0);
    vil_image_view<float> img_e_s = convert_to_spherical_coordinates(img_e,*cam,0.0);

    int offset_x, offset_y;
    float offset_score;
    estimate_offsets_fd<float,float,float>(img_e_s,img_eei_s,offset_x,offset_y,offset_score);

    if(offset_x==0 && offset_y==0){
      break;
    }

    double theta = (0.5*vnl_math::pi) + (theta_step*(double)offset_x);
    double phi = (phi_step*(double)offset_y);

    double vx,vy,vz;
    convert_angles_to_vector(theta,phi,vx,vy,vz);

    vgl_vector_3d<double> vec_from(0.0,0.0,1.0);
    vgl_vector_3d<double> vec_to(-vz,vy,vx);

    vgl_rotation_3d<double> rot_offsets(vec_from,vec_to);

    cam->set_rotation(rot_offsets*cam->get_rotation());



    // Estimating the rotation angle around the axis

    get_expected_edge_image(data,cam,&img_eei);

    // todo : use optimal size images here
    double rot_ang = estimate_rotation_angle(img_e,img_eei);

    if(rot_ang==0.0){
      break;
    }

    vnl_vector<double> rot_rot_vec(3,0.0);
    rot_rot_vec[2] = rot_ang;
    vgl_rotation_3d<double> rot_rot(rot_rot_vec);

    cam->set_rotation(rot_rot*cam->get_rotation());
  }
}

class cam_est_amoeba : public vnl_cost_function
{
public:
  cam_est_amoeba(const bvxm_voxel_slab<float> &data, 
                 const vil_image_view<float> &img_e, 
                 const vpgl_perspective_camera<double> *cam)
                 : data_(data), cam_(cam), img_e_(img_e), vnl_cost_function(2) 
  {
    best_score = 0.0;
    best_camera.set_calibration(cam->get_calibration());
    best_camera.set_rotation(cam->get_rotation());
    best_camera.set_camera_center(cam->get_camera_center());
    
    vpgl_perspective_camera<double> cam_centered(cam->get_calibration(),vgl_point_3d<double>(0.0,0.0,0.0),cam->get_rotation());
    double im_center_x = 0.5*(double)img_e.ni();
    double im_center_y = 0.5*(double)img_e.nj();

    vgl_line_3d_2_points<double> ln_center = cam_centered.backproject(vgl_point_2d<double>(im_center_x,im_center_y));
    vgl_line_3d_2_points<double> ln_center_x = cam_centered.backproject(vgl_point_2d<double>(im_center_x+1.0,im_center_y));
    vgl_line_3d_2_points<double> ln_center_y = cam_centered.backproject(vgl_point_2d<double>(im_center_x,im_center_y-1.0));

    vec_x = (ln_center_x.point2() - ln_center.point2());
    vec_y = (ln_center_y.point2() - ln_center.point2());

    normalize(vec_x);
    normalize(vec_y);

    cam_center = cam->get_camera_center();
    cam_center = cam_center - 2.0*vec_x;
    cam_center = cam_center - 2.0*vec_y;
  }

  double get_result(vnl_vector<double>& x, vpgl_perspective_camera<double> *cam){
    cam->set_calibration(best_camera.get_calibration());
    cam->set_camera_center(best_camera.get_camera_center());
    cam->set_rotation(best_camera.get_rotation());
    return best_score;
  }

  double f(const vnl_vector<double>& x)
  {
    vgl_point_3d<double> curr_center = (cam_center + (x[0]*vec_x)) + (x[1]*vec_y);

    vpgl_perspective_camera<double> curr_cam(cam_->get_calibration(),curr_center,cam_->get_rotation());

    estimate_rotation_iterative(data_,img_e_,&curr_cam);
    vil_image_view<float> img_eei(img_e_.ni(),img_e_.nj());
    get_expected_edge_image(data_,&curr_cam,&img_eei);

    double curr_score = edge_prob_cross_correlation(img_e_,img_eei);

    if(curr_score > best_score){
      best_score = curr_score;
      best_camera.set_camera_center(curr_cam.get_camera_center());
      best_camera.set_rotation(curr_cam.get_rotation());
    }

    return -curr_score;
  }

  const bvxm_voxel_slab<float> &data_;
  const vil_image_view<float> &img_e_;
  const vpgl_perspective_camera<double> *cam_;
  double best_score;
  vpgl_perspective_camera<double> best_camera;
  vgl_vector_3d<double> vec_x;
  vgl_vector_3d<double> vec_y;
  vgl_point_3d<double> cam_center;
};

void estimate_camera_amoeba(const bvxm_voxel_slab<float> &data,
                            const vil_image_view<float>& img_e,
                            vpgl_perspective_camera<double> *cam)
{
  cam_est_amoeba cost_ftn(data,img_e,cam);
  vnl_vector<double> x(2,1.0);
  vnl_amoeba amoeba(cost_ftn);
  // todo : parameterized this
  amoeba.set_relative_diameter(3.0);
  // todo : make this a global variable
  amoeba.set_max_iterations(max_iter_cam_center);
  amoeba.minimize(x);

  cost_ftn.get_result(x,cam);
}

//: set input and output types
bool bvxm_estimate_camera_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_estimate_camera_process_globals;

  // process takes 4 inputs:
  //input[0]: The voxel world
  //input[1]: Initial camera
  //input[2]: Edge image
  //input[3]: Scale of the image

  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "bvxm_voxel_world_sptr";
  input_types_[i++] = "vpgl_camera_double_sptr";
  input_types_[i++] = "vil_image_view_base_sptr";
  input_types_[i++] = "unsigned";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 3 outputs:
  // output[0]: The optimized camera
  // output[1]: Expected edge image (after camera correction)
  // output[2]: Expected edge image (before camera correction)

  vcl_vector<vcl_string> output_types_(n_outputs_);
  unsigned j = 0;
  output_types_[j++] = "vpgl_camera_double_sptr";
  output_types_[j++] = "vil_image_view_base_sptr";
  output_types_[j++] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

//:  optimizes camera parameters based on edges
bool bvxm_estimate_camera_process(bprb_func_process& pro)
{
  using namespace bvxm_estimate_camera_process_globals;

  //check number of inputs
  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  
  // voxel world
  bvxm_voxel_world_sptr vox_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  bvxm_edge_ray_processor edge_proc(vox_world);
  
  // camera
  vpgl_camera_double_sptr cam_inp = pro.get_input<vpgl_camera_double_sptr>(i++);
  vpgl_perspective_camera<double> *cam_init = dynamic_cast<vpgl_perspective_camera<double>*>(cam_inp.ptr());
  
  // image
  vil_image_view_base_sptr img_e_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> img_e_vb(img_e_sptr);
  
  vil_image_view<float> img_e;
  normalize_to_interval<vxl_byte,float>(img_e_vb,img_e,0.0f,1.0f);

  unsigned ni = img_e.ni();
  unsigned nj = img_e.nj();

  // scale of image
  unsigned scale = pro.get_input<unsigned>(i++);

  float n_normal = vox_world->get_params()->edges_n_normal();

  int num_obs = vox_world->num_observations<EDGES>(0,scale);
  vcl_cout << "Number of observations in the voxel world: " << num_obs << '\n';

  vil_image_view<float> *img_eei = new vil_image_view<float>(ni,nj,1);
  img_eei->fill(0.0f);

  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;
  bvxm_voxel_grid_base_sptr edges_grid_base = vox_world->get_grid<EDGES>(0,scale);
  bvxm_voxel_grid<edges_datatype> *edges_grid  = static_cast<bvxm_voxel_grid<edges_datatype>*>(edges_grid_base.ptr());

  nx = (int)edges_grid->grid_size().x();
  ny = (int)edges_grid->grid_size().y();
  nz = (int)edges_grid->grid_size().z();

  bvxm_voxel_slab<float> data(nx,ny,nz);

  vgl_point_3d<float> pt_0 = vox_world->voxel_index_to_xyz(0,0,0);
  vgl_point_3d<float> pt_1 = vox_world->voxel_index_to_xyz(1,1,1);

  sx = (double)pt_0.x();
  sy = (double)pt_0.y();
  sz = (double)pt_0.z();

  dx = (double)(pt_1.x() - pt_0.x());
  dy = (double)(pt_1.y() - pt_0.y());
  dz = (double)(pt_1.z() - pt_0.z());

  bvxm_voxel_grid<edges_datatype>::iterator edges_grid_it(edges_grid->begin());

  for (int z=0; z<nz; z++, ++edges_grid_it){
    for (int x=0; x<nx; x++){
      for (int y=0; y<ny; y++){
        data(x,y,z) = (*edges_grid_it)(x,y);
      }
    }
  }

  vpgl_perspective_camera<double> *cam_est = new vpgl_perspective_camera<double>(*cam_init);
  
  get_expected_edge_image(data,cam_est,img_eei);

  vil_image_view<vxl_byte> *img_eei_before_correction = new vil_image_view<vxl_byte>(ni,nj,1);  
  normalize_to_interval<float,vxl_byte>(*img_eei,*img_eei_before_correction,0.0f,255.0f);

  vcl_cout << "Estimating correct camera parameters\n";
  estimate_camera_amoeba(data,img_e,cam_est);
  vcl_cout << "\n";

  get_expected_edge_image(data,cam_est,img_eei);
  
  vil_image_view<vxl_byte> *img_eei_vb = new vil_image_view<vxl_byte>(ni,nj,1);  
  normalize_to_interval<float,vxl_byte>(*img_eei,*img_eei_vb,0.0f,255.0f);

  // output
  unsigned j = 0;
  // update the camera and store
  pro.set_output_val<vpgl_camera_double_sptr>(j++, cam_est);
  // update the edge image after update and store
  pro.set_output_val<vil_image_view_base_sptr>(j++, img_eei_vb);
  // update the edge image before update and store
  pro.set_output_val<vil_image_view_base_sptr>(j++, img_eei_before_correction);

  return true;
}
