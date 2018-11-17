#include <iostream>
#include <algorithm>
#include <limits>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vil/vil_save.h>
#include <boxm2/cpp/algo/boxm2_import_triangle_mesh.h>
#include <boxm2/cpp/algo/boxm2_render_functions.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include "boxm2_vecf_shuttle_scene.h"
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_box_3d.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_closest_point.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_lru_cache.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_fileio.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_matrix.h>
static double gauss(double d, double sigma){
  return std::exp((-0.5*d*d)/(sigma*sigma));
}
void boxm2_vecf_shuttle_scene::extract_block_data(){
  std::vector<boxm2_block_id> blocks = base_model_->get_block_ids();
  auto iter_blk = blocks.begin();
  blk_ = boxm2_cache::instance()->get_block(base_model_, *iter_blk);

  boxm2_data_base *  alpha_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_ALPHA>::prefix());
  alpha_base->enable_write();
  alpha_data_=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());

  boxm2_data_base *  app_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  app_base->enable_write();
  app_data_=new boxm2_data<BOXM2_MOG3_GREY>(app_base->data_buffer(),app_base->buffer_length(),app_base->block_id());

  boxm2_data_base *  nobs_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
  nobs_base->enable_write();
  nobs_data_=new boxm2_data<BOXM2_NUM_OBS>(nobs_base->data_buffer(),nobs_base->buffer_length(),nobs_base->block_id());
}
boxm2_vecf_shuttle_scene::boxm2_vecf_shuttle_scene(std::string const& scene_file):
  base_model_(new boxm2_scene(scene_file)), alpha_data_(nullptr), app_data_(nullptr), nobs_data_(nullptr)
{
  boxm2_lru_cache::create(base_model_);
  this->extract_block_data();
}
boxm2_vecf_shuttle_scene::boxm2_vecf_shuttle_scene(std::string const& scene_file, std::string const& imesh_file):
  base_model_(new boxm2_scene(scene_file)), alpha_data_(nullptr), app_data_(nullptr), nobs_data_(nullptr)
{
  //need to delete scene files
  boxm2_lru_cache::create(base_model_);
  imesh_mesh m;
  imesh_read(imesh_file, m);
  boxm2_import_triangle_mesh(base_model_, boxm2_cache::instance(), m);
  boxm2_cache::instance()->write_to_disk();
  this->extract_block_data();
}
vil_image_view<float> boxm2_vecf_shuttle_scene::silhouette(vpgl_camera_double_sptr const& cam, unsigned ni, unsigned nj) const{
  vil_image_view<float> ret;
  if(cam->type_name() != "vpgl_affine_camera"){
    std::cout << "currently only support affine cameras" << std::endl;
    return ret;
  }
  auto* acam = dynamic_cast<vpgl_affine_camera<double>* >(cam.ptr());
  if(!acam){
          std::cout << "currently only support affine cameras" << std::endl;
    return ret;
  }
  boxm2_scene_info* linfo = base_model_->get_blk_metadata(blk_->block_id());
  // set viewing distance for ray origins
  int dimz = (linfo->scene_dims)[2];
  double lenz = 2.0*dimz*(linfo->block_len);
  acam->set_viewing_distance(lenz);
  auto* gcam = new vpgl_generic_camera<double>;
  bool good = vpgl_generic_camera_convert::convert(*acam, ni, nj, *gcam);
  if(!good){
    std::cout << "failed to convert affine camera" << std::endl;
    return ret;
  }
  vpgl_camera_double_sptr camd = (vpgl_camera<double>*)gcam;
  ret.set_size(ni, nj);
  ret.fill(1.0f);
  auto* vis= new vil_image_view<float>(ni, nj);
  vis->fill(1.0f);
  boxm2_render_silhouette(linfo, blk_, alpha_data_, camd, &ret, vis, ni, nj);
  return ret;
}

bool boxm2_vecf_particle::operator () (boxm2_vecf_particle const& p0, boxm2_vecf_particle const& p1){
  return (p0.p_ > p1.p_);
}
void boxm2_vecf_shuttle_scene::generate_particles(const vpgl_camera_double_sptr& cam, double txmin, double txmax, double dtx){
  if(cam->type_name() != "vpgl_affine_camera"){
    std::cout << "currently only support affine cameras" << std::endl;
    return;
  }
  particles_.clear();
  auto* acam = dynamic_cast<vpgl_affine_camera<double>*>(cam.ptr());
  vnl_matrix_fixed<double,3,4> cam_mat = (*acam).get_matrix();
  for(double tx = txmin; tx<=txmax; tx+=dtx){
    vnl_matrix_fixed<double,3,4> temp = cam_mat;
    temp[0][3] += tx;
    auto* tacm = new vpgl_affine_camera<double>(temp);
    vpgl_camera_double_sptr tdcam(tacm);
    boxm2_vecf_particle part(0.0f, tdcam);
    part.tx_ = tx;
    particles_.push_back(part);
  }
}
void boxm2_vecf_shuttle_scene::generate_particles_txry(const vpgl_camera_double_sptr& cam,
                                                      double txmin, double txmax, double dtx,
                                                      double rymin, double rymax, double dry){
if(cam->type_name() != "vpgl_affine_camera"){
    std::cout << "currently only support affine cameras" << std::endl;
    return;
  }
  particles_.clear();
  auto* acam = dynamic_cast<vpgl_affine_camera<double>*>(cam.ptr());
  vnl_matrix_fixed<double,3,4> cam_mat = (*acam).get_matrix();
  for(double tx = txmin; tx<=txmax; tx+=dtx){
    for(double ry = rymin; ry<=rymax; ry+=dry){
      vnl_matrix_fixed<double,3,4> temp = cam_mat;
      temp[0][3] += tx;
      temp[1][3] -= 5.0;
      vnl_matrix<double> R(3,3,0.0);
      R[0][0]=1.0;  R[0][1]=0.0; R[0][2]= -ry;
      R[1][0]=0.0;  R[1][1]=1.0; R[1][2]= 0.0;
      R[2][0]=ry;   R[2][1]=0.0; R[2][2]= 1.0;
#if 1
      vnl_matrix<double> m = temp.extract (3, 3);
      m*=R;
      temp.update(m);
#endif
      auto* tacm = new vpgl_affine_camera<double>(temp);
      vpgl_camera_double_sptr tdcam(tacm);
      boxm2_vecf_particle part(0.0f, tdcam);
      part.tx_ = tx;
      part.ry_ = ry;
      particles_.push_back(part);
    }
  }
}
void boxm2_vecf_shuttle_scene::generate_particles_txrz(const vpgl_camera_double_sptr& cam,
                                                       double txmin, double txmax, double dtx,
                                                       double rzmin, double rzmax, double drz){
if(cam->type_name() != "vpgl_affine_camera"){
    std::cout << "currently only support affine cameras" << std::endl;
    return;
  }
  particles_.clear();
  auto* acam = dynamic_cast<vpgl_affine_camera<double>*>(cam.ptr());
  vnl_matrix_fixed<double,3,4> cam_mat = (*acam).get_matrix();
  for(double tx = txmin; tx<=txmax; tx+=dtx){
    for(double rz = rzmin; rz<=rzmax; rz+=drz){
      vnl_matrix_fixed<double,3,4> temp = cam_mat;
      temp[0][3] += tx;
      temp[1][3] -= 5.0;
      vnl_matrix<double> R(3,3,0.0);
      R[0][0]=1.0;  R[0][1]=-rz; R[0][2]= 0.0;
      R[1][0]=rz;   R[1][1]=1.0; R[1][2]= 0.0;
      R[2][0]=0.0;  R[2][1]=0.0; R[2][2]= 1.0;
#if 1
      vnl_matrix<double> m = temp.extract (3, 3);
      m*=R;
      temp.update(m);
#endif
      auto* tacm = new vpgl_affine_camera<double>(temp);
      vpgl_camera_double_sptr tdcam(tacm);
      boxm2_vecf_particle part(0.0f, tdcam);
      part.tx_ = tx;
      part.rz_ = rz;
      particles_.push_back(part);
    }
  }
}
void boxm2_vecf_shuttle_scene::generate_particles_txy(const vpgl_camera_double_sptr& cam,
                                                  double txmin, double txmax, double dtx,
                                                  double tymin, double tymax, double dty){
  if(cam->type_name() != "vpgl_affine_camera"){
    std::cout << "currently only support affine cameras" << std::endl;
    return;
  }
  particles_.clear();
  auto* acam = dynamic_cast<vpgl_affine_camera<double>*>(cam.ptr());
  vnl_matrix_fixed<double,3,4> cam_mat = (*acam).get_matrix();
  for(double tx = txmin; tx<=txmax; tx+=dtx){
    for(double ty = tymin; ty<=tymax; ty+=dty){
      vnl_matrix_fixed<double,3,4> temp = cam_mat;
      temp[0][3] += tx;
      temp[1][3] += ty;
#if 0
      vnl_matrix<double> m = temp.extract (3, 3);
      m *= s;
      temp.update(m);
#endif
      auto* tacm = new vpgl_affine_camera<double>(temp);
      vpgl_camera_double_sptr tdcam(tacm);
      boxm2_vecf_particle part(0.0f, tdcam);
      part.tx_ = tx;
      part.ty_ = ty;
      particles_.push_back(part);
    }
  }
}
void boxm2_vecf_shuttle_scene::evaluate_particles(vil_image_view<vxl_byte> const& observed_img, vgl_box_2d<int> const& bkg_box){
  silhouettes_.clear();
  unsigned ni = observed_img.ni(), nj = observed_img.nj();
  int imin = bkg_box.min_x(), imax = bkg_box.max_x();
  int jmin = bkg_box.min_y(), jmax = bkg_box.max_y();
  double xsum = 0.0;
  double xsqsum = 0.0;
  double n = 0.0;
  for(int j = jmin; j<=jmax; ++j)
    for(int i = imin; i<=imax; ++i){
      auto x = static_cast<double>(observed_img(i,j));
      xsum += x;
      xsqsum += x*x;
      n += 1.0;
    }
  bk_mean_ = xsum/n;
  bk_var_ = xsqsum/n - (bk_mean_*bk_mean_);
  double low_lim = bk_mean_ + sqrt(bk_var_);
  low_lim = 95.0;
  std::cout << "bk_mean " << bk_mean_ << " bk_var " << bk_var_ << " low lim " << low_lim << std::endl;
  log_p_uniform_ = log(255.0-low_lim);
  double txsum = 0.0;
  double txsqsum = 0.0;
  n = 0.0;
  for(int j = 0; j<static_cast<int>(nj); ++j)
    for(int i = 0; i<static_cast<int>(ni); ++i){
      auto x = static_cast<double>(observed_img(i,j));
      txsum += x;
      txsqsum += x*x;
      n += 1.0;
    }
  total_mean_ = txsum/n;
  total_var_ = txsqsum/n - (total_mean_*total_mean_);
  for(auto & particle : particles_){
    double log_p = 0.0;
    vil_image_view<float> sil = this->silhouette(particle.cam_, ni, nj);
    silhouettes_.push_back(sil);
    for(unsigned j = 0; j<nj; ++j)
      for(unsigned i = 0; i<ni; ++i){
        double s = sil(i,j);
        double obs = observed_img(i,j);
        bool sil_inside = (s==0.0);
        bool obs_inside = obs>=low_lim;
        if(sil_inside != obs_inside)
          log_p -= 100.0;
      }
    particle.p_ = log_p/(ni*nj);
    particle.print_txrz();
  }
}

void boxm2_vecf_shuttle_scene::save_silhouettes(std::string const& base_dir){
  // assume dir has ending /
  std::string name = "silhouette";
  unsigned index = 0;
  for(auto sit = silhouettes_.begin();
      sit != silhouettes_.end(); ++sit, ++index){
    std::stringstream ss;
    ss << index;
    std::string path = base_dir + name + "_" + ss.str() + ".tif";
    vil_save(*sit, path.c_str());
}
}
void boxm2_vecf_shuttle_scene::save_cameras(std::string const& base_dir){
  std::string name = "affine_cam";
  unsigned index = 0;
  for(auto pit = particles_.begin();
      pit != particles_.end(); ++pit, ++index){
    std::stringstream ss;
    ss << index;
    std::string path = base_dir + name + "_" + ss.str() + ".txt";
    std::ofstream ostr(path.c_str());
    auto* afcam = dynamic_cast<vpgl_affine_camera<double>*>((pit->cam_).ptr());
    if(!afcam){
      std::cout << "FATAL - null affine camera for index " << index << std::endl;
      return;
    }
    ostr << *afcam;
    ostr.close();
  }
}
