#include <iostream>
#include <algorithm>
#include <limits>
#include "boxm2_vecf_eye_scene.h"
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
static double gauss(double d, double sigma){
  return std::exp((-0.5*d*d)/(sigma*sigma));
}
typedef boxm2_data_traits<BOXM2_PIXEL>::datatype pixtype;
// fill the background alpha and intensity values to be slightly dark
void boxm2_vecf_eye_scene::fill_block(){
  vgl_point_3d<double> orig = blk_->local_origin();
  vgl_vector_3d<double> dims = blk_->sub_block_dim();
  vgl_vector_3d<unsigned int> nums = blk_->sub_block_num();
  for(unsigned iz = 0; iz<nums.z(); ++iz){
    double z = orig.z() + iz*dims.z();
    for(unsigned iy = 0; iy<nums.y(); ++iy){
      double y = orig.y() + iy*dims.y();
      for(unsigned ix = 0; ix<nums.x(); ++ix){
        double x = orig.x() + ix*dims.x();
        vgl_point_3d<double> p(x, y, z);
        unsigned indx;
        if(!blk_->data_index(p, indx))
          continue;
        sphere_->data()[indx] = static_cast<pixtype>(false);
        sphere_dist_->data()[indx]=std::numeric_limits<float>::max();
        iris_->data()[indx] = static_cast<pixtype>(false);
        pupil_->data()[indx] = static_cast<pixtype>(false);
      }
    }
  }
}
// currently unused
void boxm2_vecf_eye_scene::fill_target_block(){
  params_.app_[0]=static_cast<unsigned char>(10);
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  vgl_point_3d<double> orig = target_blk_->local_origin();
  vgl_vector_3d<double> dims = target_blk_->sub_block_dim();
  vgl_vector_3d<unsigned int> nums = target_blk_->sub_block_num();
  for(unsigned iz = 0; iz<nums.z(); ++iz){
    double z = orig.z() + iz*dims.z();
    for(unsigned iy = 0; iy<nums.y(); ++iy){
      double y = orig.y() + iy*dims.y();
      for(unsigned ix = 0; ix<nums.x(); ++ix){
        double x = orig.x() + ix*dims.x();
        vgl_point_3d<double> p(x, y, z);
        unsigned indx;
        if(!blk_->data_index(p, indx))
          continue;
        target_alpha_data_->data()[indx]=0.0075f;//temporary to be able to see white
        target_app_data_->data()[indx] = params_.app_;
        target_nobs_data_->data()[indx] = nobs;
      }
    }
  }
}
void boxm2_vecf_eye_scene::extract_block_data(){
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

  boxm2_data_base *  sphere_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_PIXEL>::prefix("sphere"));
  sphere_base->enable_write();
  sphere_=new boxm2_data<BOXM2_PIXEL>(sphere_base->data_buffer(),sphere_base->buffer_length(),sphere_base->block_id());

  boxm2_data_base *  sphere_dist_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_FLOAT>::prefix("sphere_dist"));
  sphere_dist_base->enable_write();
  sphere_dist_=new boxm2_data<BOXM2_FLOAT>(sphere_dist_base->data_buffer(),sphere_dist_base->buffer_length(),sphere_dist_base->block_id());

  boxm2_data_base *  iris_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_PIXEL>::prefix("iris"));
  iris_base->enable_write();
  iris_=new boxm2_data<BOXM2_PIXEL>(iris_base->data_buffer(),iris_base->buffer_length(),iris_base->block_id());

  boxm2_data_base *  pupil_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_PIXEL>::prefix("pupil"));
  pupil_base->enable_write();
  pupil_=new boxm2_data<BOXM2_PIXEL>(pupil_base->data_buffer(),pupil_base->buffer_length(),pupil_base->block_id());

}
void boxm2_vecf_eye_scene::extract_target_block_data(boxm2_scene_sptr target_scene, std::string const& app_id){

  std::vector<boxm2_block_id> blocks = target_scene->get_block_ids();
  auto iter_blk = blocks.begin();
  target_blk_ = boxm2_cache::instance()->get_block(target_scene, *iter_blk);

  boxm2_data_base *  alpha_base  = boxm2_cache::instance()->get_data_base(target_scene,*iter_blk,boxm2_data_traits<BOXM2_ALPHA>::prefix());
  alpha_base->enable_write();
  target_alpha_data_=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());

  boxm2_data_base *  app_base  = boxm2_cache::instance()->get_data_base(target_scene,*iter_blk,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(app_id));
  app_base->enable_write();
  target_app_data_=new boxm2_data<BOXM2_MOG3_GREY>(app_base->data_buffer(),app_base->buffer_length(),app_base->block_id());

  boxm2_data_base *  nobs_base  = boxm2_cache::instance()->get_data_base(target_scene,*iter_blk,boxm2_data_traits<BOXM2_NUM_OBS>::prefix(app_id));
  nobs_base->enable_write();
  target_nobs_data_=new boxm2_data<BOXM2_NUM_OBS>(nobs_base->data_buffer(),nobs_base->buffer_length(),nobs_base->block_id());
}

void boxm2_vecf_eye_scene::reset_indices(){
  vgl_point_3d<double> orig = blk_->local_origin();
  vgl_vector_3d<double> dims = blk_->sub_block_dim();
  vgl_vector_3d<unsigned int> nums = blk_->sub_block_num();
  for(unsigned iz = 0; iz<nums.z(); ++iz){
    double z = orig.z() + iz*dims.z();
    for(unsigned iy = 0; iy<nums.y(); ++iy){
      double y = orig.y() + iy*dims.y();
      for(unsigned ix = 0; ix<nums.x(); ++ix){
        double x = orig.x() + ix*dims.x();
        vgl_point_3d<double> p(x, y, z);
        unsigned indx;
        if(!blk_->data_index(p, indx))
          continue;
        double side_length;
        vgl_point_3d<double> cell_center, local_tree_coords;
        if(!blk_->contains(p, local_tree_coords, cell_center, side_length))
          continue;
        pixtype val=sphere_->data()[indx];
        auto sphere = static_cast<unsigned char>(val);
        val=iris_->data()[indx];
        auto iris = static_cast<unsigned char>(val);
        val=pupil_->data()[indx];
        auto pupil = static_cast<unsigned char>(val);
        if(sphere){
          sphere_cell_centers_.push_back(cell_center);
          sphere_cell_data_index_.push_back(indx);
          double d = sphere_dist_->data()[indx];
          closest_sphere_distance_norm_.push_back(d);
          if(iris){
            iris_cell_centers_.push_back(cell_center);
            iris_cell_data_index_.push_back(indx);
          }
          if(pupil){
            pupil_cell_centers_.push_back(cell_center);
            pupil_cell_data_index_.push_back(indx);
          }
        }
      }
    }
  }
}

boxm2_vecf_eye_scene::boxm2_vecf_eye_scene(std::string const& scene_file, bool initialize):
  base_model_(new boxm2_scene(scene_file)), alpha_data_(nullptr), app_data_(nullptr), nobs_data_(nullptr), sphere_(nullptr), sphere_dist_(nullptr), iris_(nullptr), pupil_(nullptr)
{
  boxm2_lru_cache::create(base_model_);
  this->extract_block_data();
  if(initialize){
    this->fill_block();
    this->create_eye();
  }else this->recreate_eye();
}


//scan a dense set of points on the sphere and find
//voxels that contain the points. the sphere voxels are
//represented by the cell center position. Also update various
//containers to support further operations such as
//access to the database indices for each sphere voxel
void boxm2_vecf_eye_scene::build_sphere(){
  double pi = vnl_math::pi;
  double two_pi = 2.0*pi;
  for(double r = params_.eye_radius_-1.0; r<=params_.eye_radius_; r+=1.0){
  vgl_sphere_3d<double> sph(0.0, 0.0, 0.0, r);
    for(double el = 0.0; el<=pi; el+=params_.angle_inc())
      for(double az = 0.0; az<=two_pi; az+=params_.angle_inc()){
        vgl_point_3d<double> p;
        sph.spherical_to_cartesian(el, az, p);
        unsigned indx;
        if(!blk_->data_index(p, indx))
          continue;
        double side_len;
        vgl_point_3d<double> local_tree_coords;
        vgl_point_3d<double> cell_center;
        if(!blk_->contains(p, local_tree_coords, cell_center, side_len))
          continue;
        side_len *= 0.86602540; // 1/2*sqrt(3) the cell diagonal
        //if new sphere or shell point add to list
        std::vector<vgl_point_3d<double> >::iterator iit;
        iit = std::find(sphere_cell_centers_.begin(), sphere_cell_centers_.end(), cell_center);
        if(iit==sphere_cell_centers_.end()){
          sphere_cell_centers_.push_back(cell_center);
          sphere_cell_data_index_.push_back(indx);
          sphere_->data()[indx] = static_cast<pixtype>(true);
          double d = vgl_distance(cell_center, local_tree_coords)/side_len;
          closest_sphere_distance_norm_.push_back(d);
          sphere_dist_->data()[indx]=static_cast<float>(d);
        }else{ // update distance in case a new sphere point is closer
          unsigned i = static_cast<unsigned>(iit-sphere_cell_centers_.begin());
          double d = vgl_distance(cell_center, local_tree_coords)/side_len;
          if(closest_sphere_distance_norm_[i]>d){
            closest_sphere_distance_norm_[i]=d;
            unsigned idx = sphere_cell_data_index_[i];
            sphere_dist_->data()[indx]=static_cast<float>(d);
          }
        }
      }
  }
}
//find the voxels that belong to the iris, chosen from those
//that lie on the sphere. The base set of iris cell centers
//are not affected by rotation, i.e, rotation takes the base
//set to the rotated version, iris_cell_centers.
void boxm2_vecf_eye_scene::build_iris(){
  iris_cell_centers_.clear();
  iris_cell_data_index_.clear();
  double pi = vnl_math::pi;
  double two_pi = 2.0*pi;
  for(double r = params_.eye_radius_-1.0; r<=params_.eye_radius_; r+=1.0){
  vgl_sphere_3d<double> sph(0.0, 0.0, 0.0, r);
  double iris_half_ang = std::atan(params_.iris_radius_/params_.eye_radius_);
  double el_st = 0.0, el_nd = iris_half_ang;
  double az_st = 0 , az_nd = two_pi; // all azimuth range
  for(double el = el_st; el<=el_nd; el+=params_.angle_inc())
    for(double az = az_st; az<=az_nd; az+=params_.angle_inc()){
      vgl_point_3d<double> p;
      sph.spherical_to_cartesian(el, az, p);
      double side_len;
      vgl_point_3d<double> local_tree_coords;
      vgl_point_3d<double> cell_center;
      if(!blk_->contains(p, local_tree_coords, cell_center, side_len))
        continue;
      std::vector<vgl_point_3d<double> >::iterator iit;
      // if a new iris point
      iit = std::find(sphere_cell_centers_.begin(), sphere_cell_centers_.end(), cell_center);
      if(iit==sphere_cell_centers_.end())
        continue;
      unsigned sp_i = static_cast<unsigned>(iit-sphere_cell_centers_.begin());
      // add it to the base set
      iit = std::find(iris_cell_centers_.begin(), iris_cell_centers_.end(), cell_center);
      if(iit==iris_cell_centers_.end()){
        iris_cell_centers_.push_back(cell_center);
        unsigned indx = sphere_cell_data_index_[sp_i];
        iris_->data()[indx] = static_cast<pixtype>(true);
        iris_cell_data_index_.push_back(indx);
      }
    }
  }
}
void boxm2_vecf_eye_scene::build_pupil(){
  pupil_cell_centers_.clear();
  pupil_cell_data_index_.clear();
  double pi = vnl_math::pi;
  double two_pi = 2.0*pi;
  for(double r = params_.eye_radius_-1.0; r<=params_.eye_radius_; r+=1.0){
  vgl_sphere_3d<double> sph(0.0, 0.0, 0.0, r);
  double pupil_half_ang = std::atan(params_.pupil_radius_/params_.eye_radius_);
  double el_st = 0.0, el_nd = pupil_half_ang;
  double az_st = 0 , az_nd = two_pi; // all azimuth range
  for(double el = el_st; el<=el_nd; el+=params_.angle_inc())
    for(double az = az_st; az<=az_nd; az+=params_.angle_inc()){
      vgl_point_3d<double> p;
      sph.spherical_to_cartesian(el, az, p);
      double side_len;
      vgl_point_3d<double> local_tree_coords;
      vgl_point_3d<double> cell_center;
      if(!blk_->contains(p, local_tree_coords, cell_center, side_len))
        continue;
      //check if the point is in the iris set
      std::vector<vgl_point_3d<double> >::iterator iit;
      iit = std::find(iris_cell_centers_.begin(), iris_cell_centers_.end(), cell_center);
      if(iit==iris_cell_centers_.end())
        continue;
      //if so, see if the point is allready in the base pupil set
      iit = std::find(pupil_cell_centers_.begin(), pupil_cell_centers_.end(), cell_center);
      if(iit==pupil_cell_centers_.end()){
        std::vector<vgl_point_3d<double> >::iterator jit;
        jit = std::find(sphere_cell_centers_.begin(), sphere_cell_centers_.end(), cell_center);
        unsigned sp_i = static_cast<unsigned>(jit-sphere_cell_centers_.begin());
        unsigned indx = sphere_cell_data_index_[sp_i];
        pupil_->data()[indx] = static_cast<pixtype>(true);
        pupil_cell_centers_.push_back(cell_center);
        pupil_cell_data_index_.push_back(indx);
      }
    }
  }
}
//establish neighboring cells to each sphere cell, based on distance
// add to various convenient indices
void boxm2_vecf_eye_scene::find_cell_neigborhoods(){
  std::vector<vgl_point_3d<double> > temp = sphere_cell_centers_;
  for(unsigned i = 0; i<sphere_cell_centers_.size(); i++){
      vgl_point_3d<double>& p = sphere_cell_centers_[i];
      unsigned indx_i = sphere_cell_data_index_[i];
      data_index_to_cell_index_[indx_i]=i;
      for(unsigned j =0; j<temp.size(); ++j){
      if(j==i)
        continue;
      vgl_point_3d<double>& q = temp[j];
      double d = std::sqrt((p.x()-q.x())*(p.x()-q.x()) + (p.y()-q.y())*(p.y()-q.y()) + (p.z()-q.z())*(p.z()-q.z()));
      if(d<=params_.sphere_neighbor_radius()){
        cell_neighbor_cell_index_[i].push_back(j);
        unsigned indx_j = sphere_cell_data_index_[j];
        std::vector<unsigned>& indices = cell_neighbor_data_index_[indx_i];
        auto iit = std::find(indices.begin(), indices.end(), indx_j);
        if(iit == indices.end()){
          indices.push_back(indx_j);
          cell_neighbor_distance_[indx_i].push_back(d);
        }
      }
      }
  }
}

//run through all the sphere points (sclera) and paint them white
void boxm2_vecf_eye_scene::paint_sclera(){

  params_.app_[0]=params_.sclera_intensity_;
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  auto ns = static_cast<unsigned>(sphere_cell_centers_.size());
  for(unsigned i = 0; i<ns; ++i){
    unsigned indx = sphere_cell_data_index_[i];
    auto d = static_cast<float>(closest_sphere_distance_norm_[i]);
    if(d >0.0f)
      alpha_data_->data()[indx]= -5.0f*std::log(d);//factor of 5 to increase occlusion
    else
      alpha_data_->data()[indx]=200.0f;
    app_data_->data()[indx] = params_.app_;
    nobs_data_->data()[indx] = nobs;
  }
}
//run through all the iris points and paint them
//with the specified intensity
void boxm2_vecf_eye_scene::paint_iris(){
  // set iris intensity
  params_.app_[0]=params_.iris_intensity_;
  auto ni = static_cast<unsigned>(iris_cell_data_index_.size());
  for(unsigned i = 0; i<ni; ++i){
    unsigned indx = iris_cell_data_index_[i];
    app_data_->data()[indx] = params_.app_;
  }
}
//run through all the pupil points and paint them
//with the specified intensity
void boxm2_vecf_eye_scene::paint_pupil(){
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
  params_.app_[0]=params_.pupil_intensity_;
  auto np = static_cast<unsigned>(pupil_cell_data_index_.size());
  for(unsigned i = 0; i<np; ++i){
    unsigned indx = pupil_cell_data_index_[i];
    app_data_->data()[indx] = params_.app_;
  }
}
// define eye voxel positions, neighborhoods and data indices
// somewhat costly since the sphere is sampled very finely so
// as to not miss any sphere voxels
void boxm2_vecf_eye_scene::create_eye(){
  this->build_sphere();
  this->build_iris();
  this->build_pupil();
  this->find_cell_neigborhoods();
  // paint the appearance in base gaze direction, +z
  this->paint_sclera();
  this->paint_iris();
  this->paint_pupil();
}

 void boxm2_vecf_eye_scene::recreate_eye(){
   this->reset_indices();
   this->find_cell_neigborhoods();
   // paint the appearance in base gaze direction, +z
  this->paint_sclera();
  this->paint_iris();
  this->paint_pupil();
 }


vgl_point_3d<double> boxm2_vecf_eye_scene::closest_point_on_shell(vgl_point_3d<double> const& p) const{
  vgl_point_3d<double> min_pt;
  double min_dist = std::numeric_limits<double>::max();
  for(double r = params_.eye_radius_-1.0; r<=params_.eye_radius_; r+=1.0){
    vgl_sphere_3d<double> sph(0.0, 0.0, 0.0, r);
    vgl_point_3d<double> cp = vgl_closest_point<double>(sph, p);
    double d = vgl_distance<double>(cp, p);
    if(d<min_dist){
      min_dist = d;
      min_pt = cp;
    }
  }
  return min_pt;
}

std::vector<vgl_vector_3d<double> > boxm2_vecf_eye_scene::inverse_vector_field(vgl_rotation_3d<double> const& rot) const{
  unsigned k = 0;
  std::vector<vgl_vector_3d<double> > null;
  vgl_rotation_3d<double> inv_rot = rot.inverse();
  auto n = static_cast<unsigned>(sphere_cell_centers_.size());
  std::vector<vgl_vector_3d<double> > vf(n);// initialized to 0
  vgl_point_3d<double> center(0.0, 0.0, 0.0);
  vgl_point_3d<double> cell_center;
  vgl_point_3d<double> lc;
  double side_len;
  if(!blk_->contains(center, lc, cell_center, side_len))
    return null;//shouldn't happen
  for(unsigned j = 0; j<n; ++j){
    const vgl_point_3d<double>& p = sphere_cell_centers_[j];
    vgl_point_3d<double> pl(p.x()-lc.x(), p.y()-lc.y(), p.z()-lc.z());
    vgl_point_3d<double> cp = this->closest_point_on_shell(pl);
    vgl_point_3d<double> rp = inv_rot*cp;
    vgl_point_3d<double> local_tree_coords;
    if(!blk_->contains(rp, local_tree_coords, cell_center, side_len)){
      continue;
    }
    std::vector<vgl_point_3d<double> >::const_iterator iit;
    iit = std::find(sphere_cell_centers_.begin(), sphere_cell_centers_.end(), cell_center);
    if(iit==sphere_cell_centers_.end()){
      std::cout << "warning! shouldn't happen\n";
      continue;
    }
    vf[j].set(rp.x()-pl.x(), rp.y()-pl.y(), rp.z()-pl.z());
  }
  return vf;
}

void boxm2_vecf_eye_scene::apply_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf){
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;

  // eye ball center in global source coordinates
  // used later
  vgl_point_3d<double> cell_center;
  vgl_point_3d<double> lcent;
  double side_len;
  vgl_point_3d<double> center(0.0, 0.0, 0.0);
  if(!blk_->contains(center, lcent, cell_center, side_len))
    return;//shouldn't happen

  // global bounding box of *this eye scene
  vgl_box_3d<double> bb = blk_->bounding_box_global();
  // translate box to target scene according to offset vector
  vgl_point_3d<double> min_pt = bb.min_point() + params_.offset_;
  vgl_point_3d<double> max_pt = bb.max_point() + params_.offset_;
  bb.set_min_point(min_pt);   bb.set_max_point(max_pt);
  // compute the cell info for all the target cells in the translated box
  std::vector<cell_info> tgt_centers = target_blk_->cells_in_box(bb);
  int n = static_cast<unsigned>(tgt_centers.size());
  if(n==0)
    return;//shouldn't happen

  // iterate over the target cells and interpolate info from source
  // temporary data storage maps
  std::map<unsigned, unsigned char> temp;
  std::map<unsigned, boxm2_data_traits<BOXM2_ALPHA>::datatype> temp_alpha;
  std::map<unsigned, bool> temp_is_sphere;
  vgl_point_3d<double> lc;//local center

  for(int j = 0; j<n; ++j){
    // target cell center translated back to source
    vgl_point_3d<double> p = tgt_centers[j].cell_center_-params_.offset_;
    // data index for source end of reverse translation vector
    unsigned dindx;
    if(!blk_->data_index(p, dindx))
      continue;
    // find the corresponding spherical cell center
    std::map<unsigned, unsigned>::iterator mit;
    mit = data_index_to_cell_index_.find(dindx);
    if(mit==data_index_to_cell_index_.end()){
      //not a sphere cell
      temp_is_sphere[j] = false;
      continue;
    }
    temp_is_sphere[j]=true;
    unsigned cell_indx = mit->second;

    //source cell after reverse rotation vector field
    vgl_point_3d<double> src = p + vf[cell_indx];//add vector field

    if(!blk_->contains(src, lc, cell_center, side_len))
      return;//shouldn't happen
    double dc = (lc -cell_center).length();

    //data index of rotated source cell
    if(!blk_->data_index(src, dindx))
      continue;
    mit = data_index_to_cell_index_.find(dindx);
    if(mit==data_index_to_cell_index_.end()){
      //not a sphere cell
      continue;
    }

    //appearance and alpha data at source cell
    app = app_data_->data()[dindx];
    boxm2_data_traits<BOXM2_ALPHA>::datatype alpha0 = alpha_data_->data()[dindx];

    // interpolate using Gaussian weights
    const std::vector<double>& dists = cell_neighbor_distance_[dindx];
    const std::vector<unsigned> nbr_data = cell_neighbor_data_index_[dindx];
    double sumw = gauss(dc,params_.gauss_sigma()), sumint = app[0]*sumw, sumalpha = alpha0*sumw;
    for(unsigned k = 0; k<dists.size(); ++k)
      if(dists[k]<=std::sqrt(2.0)){// add neighbors if they are planar diagonal or nearer
        unsigned nidx = nbr_data[k];
        double w = gauss(dists[k],params_.gauss_sigma());
        sumw += w;
        app = app_data_->data()[nidx];
        double alpha = alpha_data_->data()[nidx];
        sumint += w*app[0];
        sumalpha += w*alpha;
    }
    sumint/=sumw;
    auto ir_int = static_cast<unsigned char>(sumint);
    temp[j]=ir_int;
    sumalpha /= sumw;
    temp_alpha[j]=static_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype>(sumalpha);
  }
  // set data on target
  for(int j = 0; j<n; ++j){
    if(!temp_is_sphere[j])
      continue;
    unsigned tindx = tgt_centers[j].data_index_;
    app[0]=temp[j];
    target_app_data_->data()[tindx] = app;
    target_alpha_data_->data()[tindx]=temp_alpha[j];
  }
}

void boxm2_vecf_eye_scene::map_to_target(const boxm2_scene_sptr& target_scene, std::string const& app_id){
  this->extract_target_block_data(target_scene, app_id);
  vnl_vector_fixed<double, 3> Z(0.0, 0.0, 1.0);
  vnl_vector_fixed<double, 3> to_dir(params_.eye_pointing_dir_.x(),
                                     params_.eye_pointing_dir_.y(),
                                     params_.eye_pointing_dir_.z());
  vgl_rotation_3d<double> rot(Z, to_dir);
  std::vector<vgl_vector_3d<double> > invf = this->inverse_vector_field(rot);
  this->apply_vector_field_to_target(invf);
}
