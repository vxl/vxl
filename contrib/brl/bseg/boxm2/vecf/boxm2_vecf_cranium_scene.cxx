#include "boxm2_vecf_cranium_scene.h"
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_box_3d.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_closest_point.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <vcl_algorithm.h>
#include <vcl_limits.h>
#include <vcl_set.h>
#include <vul/vul_timer.h>

typedef boxm2_data_traits<BOXM2_PIXEL>::datatype pixtype;
// fill the background alpha and intensity values to be slightly dark
void boxm2_vecf_cranium_scene::fill_block(){
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
        cranium_->data()[indx]  = static_cast<pixtype>(false);
      }
    }
  }
}
// currently unused, except for display purposes
void boxm2_vecf_cranium_scene::fill_target_block(){
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
        if(!target_blk_->data_index(p, indx))
          continue;
        target_alpha_data_->data()[indx]=0.005f;//to see contrast against white
        target_app_data_->data()[indx] = params_.app_;
        target_nobs_data_->data()[indx] = nobs;
      }
    }
  }
}


void boxm2_vecf_cranium_scene::extract_block_data(){

  vcl_vector<boxm2_block_id> blocks = base_model_->get_block_ids();

  vcl_vector<boxm2_block_id>::iterator iter_blk = blocks.begin();
  blk_ = boxm2_cache::instance()->get_block(base_model_, *iter_blk);
  sigma_ = static_cast<float>(blk_->sub_block_dim().x());

  boxm2_data_base *  alpha_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_ALPHA>::prefix());
  alpha_base->enable_write();
  alpha_data_=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());

  boxm2_data_base *  app_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  app_base->enable_write();
  app_data_=new boxm2_data<BOXM2_MOG3_GREY>(app_base->data_buffer(),app_base->buffer_length(),app_base->block_id());

  boxm2_data_base *  nobs_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
  nobs_base->enable_write();
  nobs_data_=new boxm2_data<BOXM2_NUM_OBS>(nobs_base->data_buffer(),nobs_base->buffer_length(),nobs_base->block_id());

  boxm2_data_base *  cranium_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_PIXEL>::prefix("cranium"));
  cranium_base->enable_write();
  cranium_=new boxm2_data<BOXM2_PIXEL>(cranium_base->data_buffer(),cranium_base->buffer_length(),cranium_base->block_id());
}
void boxm2_vecf_cranium_scene::extract_target_block_data(boxm2_scene_sptr target_scene){

  vcl_vector<boxm2_block_id> blocks = target_scene->get_block_ids();
  vcl_vector<boxm2_block_id>::iterator iter_blk = blocks.begin();
  target_blk_ = boxm2_cache::instance()->get_block(target_scene, *iter_blk);

  boxm2_data_base *  alpha_base  = boxm2_cache::instance()->get_data_base(target_scene,*iter_blk,boxm2_data_traits<BOXM2_ALPHA>::prefix());
  alpha_base->enable_write();
  target_alpha_data_=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());

  boxm2_data_base *  app_base  = boxm2_cache::instance()->get_data_base(target_scene,*iter_blk,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  app_base->enable_write();
  target_app_data_=new boxm2_data<BOXM2_MOG3_GREY>(app_base->data_buffer(),app_base->buffer_length(),app_base->block_id());

  boxm2_data_base *  nobs_base  = boxm2_cache::instance()->get_data_base(target_scene,*iter_blk,boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
  nobs_base->enable_write();
  target_nobs_data_=new boxm2_data<BOXM2_NUM_OBS>(nobs_base->data_buffer(),nobs_base->buffer_length(),nobs_base->block_id());

  if(has_background_){
    vcl_cout<< " Darkening background "<<vcl_endl;
    this->fill_target_block();
  }
  this->determine_target_box_cell_centers(); // get cell centers in target corresponding to the source block (blk_)
}

// after loading the block initialize all the cell indices from the block labels, e.g., cell == LEFT_RAMUS, cell == LEFT_ANGLE, etc.
void boxm2_vecf_cranium_scene::cache_cell_centers_from_anatomy_labels(){
  vul_timer t;
  vgl_point_3d<double> orig = blk_->local_origin();
  vgl_vector_3d<double> dims = blk_->sub_block_dim();
  vgl_vector_3d<unsigned int> nums = blk_->sub_block_num();
  vgl_point_3d<double> p;
  for(unsigned iz = 0; iz<nums.z(); ++iz){
    double z = orig.z() + iz*dims.z();
    for(unsigned iy = 0; iy<nums.y(); ++iy){
      double y = orig.y() + iy*dims.y();
      for(unsigned ix = 0; ix<nums.x(); ++ix){
        double x = orig.x() + ix*dims.x();
        p.set(x, y, z);
        unsigned indx;
        if(!blk_->data_index(p, indx))
          continue;
        unsigned cranium_index  = static_cast<unsigned>(cranium_cell_centers_.size());
        cranium_cell_centers_.push_back(p);
        cranium_cell_data_index_.push_back(indx);
        data_index_to_cell_index_[indx] = cranium_index;
      }
    }
  }
  vcl_cout << "Reset indices " << static_cast<double>(t.real())/1000.0 << " sec.\n";
}
// constructors
boxm2_vecf_cranium_scene::boxm2_vecf_cranium_scene(vcl_string const& scene_file): boxm2_vecf_articulated_scene(scene_file), source_model_exists_(true), alpha_data_(0), app_data_(0), nobs_data_(0), cranium_(0), intrinsic_change_(false){
  this->extrinsic_only_ = false;
  target_blk_ = 0;
  target_data_extracted_ = false;
  this->has_background_ = false;
  boxm2_lru_cache::create(base_model_);
  vul_timer t;
  this->extract_block_data();
  this->recreate_cranium();
  vcl_cout << "Create cranium scene in " << t.real()/1000.0 << "seconds\n";
}  

boxm2_vecf_cranium_scene::boxm2_vecf_cranium_scene(vcl_string const& scene_file, vcl_string const& geometry_file):
  boxm2_vecf_articulated_scene(scene_file), source_model_exists_(false), alpha_data_(0), app_data_(0), nobs_data_(0), cranium_(0), intrinsic_change_(false)
{
  cranium_geo_ = boxm2_vecf_cranium(geometry_file);
  this->extrinsic_only_ = false;
  target_blk_ = 0;
  target_data_extracted_ = false;
  boxm2_lru_cache::create(base_model_);
  this->extract_block_data();
  this->has_background_ = false;
  this->rebuild();
 }

boxm2_vecf_cranium_scene::boxm2_vecf_cranium_scene(vcl_string const& scene_file, vcl_string const& geometry_file, vcl_string const& params_file_name):
  boxm2_vecf_articulated_scene(scene_file),source_model_exists_(false),alpha_data_(0), app_data_(0), nobs_data_(0),cranium_(0)
  {
    vcl_ifstream params_file(params_file_name.c_str());
    if (!params_file){
      vcl_cout<<" could not open params file : "<<params_file_name<<vcl_endl;
    }else{
    params_file >> this->params_;

    cranium_geo_ = boxm2_vecf_cranium(geometry_file);
    boxm2_lru_cache::create(base_model_);
    this->extract_block_data();
    this->cache_cell_centers_from_anatomy_labels();
    this->cache_neighbors();
    this->extrinsic_only_ = true;
  }
}
void boxm2_vecf_cranium_scene::rebuild(){
  if(this->extrinsic_only_){
    vcl_cout<<" warning! rebuild called but scene accepts only extrinsic articulations!"<<vcl_endl;
    return;
  }
    this->reset_buffers();
    this->create_cranium();
    this->cache_neighbors();
    this->determine_target_box_cell_centers();
}
void boxm2_vecf_cranium_scene::cache_neighbors(){
  this->find_cell_neigborhoods();
}

void boxm2_vecf_cranium_scene::build_cranium(){
  double len = 3 * blk_->sub_block_dim().x();
  double d_thresh = 0.8660*len;//sqrt(3)/2 x len, diagonal distance
  vgl_box_3d<double> bb = cranium_geo_.bounding_box();
   // cell in a box centers are in global coordinates
  vcl_vector<cell_info> ccs = blk_->cells_in_box(bb);
  for(vcl_vector<cell_info>::iterator cit = ccs.begin();
      cit != ccs.end(); ++cit){
    const vgl_point_3d<double>& cell_center = cit->cell_center_;
    unsigned indx = cit->data_index_;
    double d = cranium_geo_.surface_distance(cell_center);
    if(d < d_thresh){
      if(!is_type_global(cell_center, CRANIUM)){
        cranium_cell_centers_.push_back(cell_center);
        cranium_cell_data_index_.push_back(indx);
        data_index_to_cell_index_[indx]=static_cast<unsigned>(cranium_cell_centers_.size())-1;
        float blending_factor = static_cast<float>(gauss(d,sigma_));
        alpha_data_->data()[indx]= - vcl_log(1.0f - ( 0.95f ))/ static_cast<float>(this->subblock_len()) * blending_factor;
        cranium_->data()[indx] = static_cast<pixtype>(true);
      }
    }
  }
}
void boxm2_vecf_cranium_scene::create_cranium(){
  this->build_cranium();
  this->paint_cranium();
 }


void boxm2_vecf_cranium_scene::find_cell_neigborhoods(){
  vul_timer t;
  double distance = params_.neighbor_radius()*subblock_len();
  for(unsigned i = 0; i<cranium_cell_centers_.size(); i++){
      vgl_point_3d<double>& p = cranium_cell_centers_[i];
      unsigned indx_i = cranium_cell_data_index_[i];
      vcl_vector<vgl_point_3d<double> > nbrs = blk_->sub_block_neighbors(p, distance);
      for(unsigned j =0; j<nbrs.size(); ++j){
        vgl_point_3d<double>& q = nbrs[j];
        unsigned indx_n;
        if(!blk_->data_index(q, indx_n))
          continue;
        vcl_map<unsigned, unsigned >::iterator iit= data_index_to_cell_index_.find(indx_n);
        if(iit == data_index_to_cell_index_.end())
          continue;
        if(iit->second==i)
                continue;
        cell_neighbor_cell_index_[i].push_back(iit->second);
        vcl_vector<unsigned>& indices = cell_neighbor_data_index_[indx_i];
        indices.push_back(indx_n);
      }
  }
  vcl_cout << "Find cranium cell neighborhoods in " << static_cast<double>(t.real())/1000.0 << " sec.\n";
}

 void boxm2_vecf_cranium_scene::recreate_cranium(){
   cranium_cell_centers_.clear();
   cranium_cell_data_index_.clear();
   cell_neighbor_cell_index_.clear();
   data_index_to_cell_index_.clear();
   cell_neighbor_data_index_.clear();
   cache_cell_centers_from_anatomy_labels();
   //this->find_cell_neigborhoods(); no vector field for now JLM
 }

void boxm2_vecf_cranium_scene::paint_cranium(){
  params_.app_[0]=params_.cranium_intensity_;
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  unsigned ns = static_cast<unsigned>(cranium_cell_centers_.size());
  for(unsigned i = 0; i<ns; ++i){
    unsigned indx = cranium_cell_data_index_[i];
    app_data_->data()[indx] = params_.app_;
    nobs_data_->data()[indx] = nobs;
  }
}

 bool boxm2_vecf_cranium_scene::is_type_data_index(unsigned data_index, boxm2_vecf_cranium_scene::anat_type type) const{

   if(type == CRANIUM){
     unsigned char cranium = static_cast<unsigned char>(cranium_->data()[data_index]);
     return cranium>0;
   }
   return false;
 }

bool boxm2_vecf_cranium_scene::is_type_global(vgl_point_3d<double> const& global_pt, boxm2_vecf_cranium_scene::anat_type  type) const{
  unsigned indx;
  bool success =  blk_->data_index(global_pt, indx);
  if (!success){
    //#if _DEBUG
    //    vcl_cout<<"point "<<global_pt<< " was out of eye scene bounding box "<<vcl_endl;
    //#endif
    return false;
}
  return this->is_type_data_index(indx, type);
}

 bool boxm2_vecf_cranium_scene::find_nearest_data_index(boxm2_vecf_cranium_scene::anat_type type, vgl_point_3d<double> const& probe, unsigned& data_indx) const{
   double r = this->subblock_len();
   double dr = 2*r;
   vgl_point_3d<double> org = blk_->local_origin();
   vgl_vector_3d<double> loc = (probe-org)/r;
   double x0 = vcl_floor(loc.x()), y0 = vcl_floor(loc.y()), z0 = vcl_floor(loc.z());
   x0 = x0*r + org.x();   y0 = y0*r + org.y();  z0 = z0*r + org.z();
   double dmin = vcl_numeric_limits<double>::max();
   data_indx = 0;
   unsigned data_index_min = 0;
   for(double x = (x0-dr); x<=(x0+dr); x+=r)
     for(double y = (y0-dr); y<=(y0+dr); y+=r)
       for(double z = (z0-dr); z<=(z0+dr); z+=r){
         vgl_point_3d<double> p(x, y, z);
         unsigned data_index;
         if(!blk_->data_index(p, data_index))
           continue;
         if(!is_type_data_index(data_index, type))
           continue;
         double d = vgl_distance(p, probe);
         if(d<dmin){
           dmin = d;
           data_index_min =data_index;
         }
       }
   if(dmin>params_.neighbor_radius()*r)
    return false;
   data_indx = data_index_min;
   return true;
 }


void  boxm2_vecf_cranium_scene::inverse_vector_field(vgl_rotation_3d<double> const& rot, vcl_vector<vgl_vector_3d<double> >& vf,
                                                      vcl_vector<bool>& valid) const{

  vul_timer t;
  vgl_box_3d<double> bb = cranium_geo_.bounding_box();
  if(source_model_exists_)
    bb = blk_->bounding_box_global();
  unsigned nt = static_cast<unsigned>(box_cell_centers_.size());
  vf.resize(nt);// initialized to 0
  valid.resize(nt, false);
  vgl_rotation_3d<double> inv_rot = rot.inverse();
  unsigned cnt = 0;
  for(unsigned i = 0; i<nt; ++i){
    vgl_point_3d<double> p = (box_cell_centers_[i].cell_center_)-params_.offset_;
    if(!bb.contains(p))
      continue;
    cnt++;
    // vf only defined for cells on the cranium
    if(!is_type_global(p, CRANIUM))
      continue;
    valid[i]=true;
    //is a sphere voxel cell so define the vector field
#if 0
    vgl_point_3d<double> rp = inv_rot * p;
    vf[i].set(rp.x() - p.x(), rp.y() - p.y(), rp.z() - p.z());
#endif
    vf[i].set(0.0, 0.0, 0.0);
  }
  // vcl_cout << "computed " << cnt << " pts out of "<< nt << " for eye vector field in " << t.real()/1000.0 << " sec.\n";
  // vcl_cout << ncont << " were inside smin\n";
}



//
// interpolate data around the inverted position of the target in the source reference frame. Interpolation weights are based
// on a Gaussian distribution with respect to distance from the source location
//
void boxm2_vecf_cranium_scene::interpolate_vector_field(vgl_point_3d<double> const& src, unsigned sindx, unsigned dindx, unsigned tindx,
                                                      vcl_vector<vgl_point_3d<double> > & cell_centers,
                                                      vcl_map<unsigned, vcl_vector<unsigned> >& cell_neighbor_cell_index,
                                                      vcl_map<unsigned, vcl_vector<unsigned> >&cell_neighbor_data_index){
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
  boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype color_app;
  typedef vnl_vector_fixed<double,8> double8;
  double8 curr_color;

  const vgl_point_3d<double>& scell = cell_centers[sindx];
  //appearance and alpha data at source cell
  app = app_data_->data()[dindx];
  boxm2_data_traits<BOXM2_ALPHA>::datatype alpha0 = alpha_data_->data()[dindx];
  double sig = params_.gauss_sigma()*subblock_len();
  // interpolate using Gaussian weights based on distance to the source point
  double dc = vgl_distance(scell, src);
  const vcl_vector<unsigned>& nbr_cells = cell_neighbor_cell_index[sindx];
  const vcl_vector<unsigned>& nbr_data = cell_neighbor_data_index[dindx];
  double sumw = gauss(dc, sig), sumint = app[0]*sumw, sumalpha = alpha0*sumw;
  double8 sumcolor= sumw * curr_color;
  for(unsigned k = 0; k<nbr_cells.size(); ++k){
    double d = vgl_distance(cell_centers[nbr_cells[k]],src);
    unsigned nidx = nbr_data[k];
    double w = gauss(d, sig);
    sumw += w;
    app = app_data_->data()[nidx];
    double alpha = alpha_data_->data()[nidx];
    sumint   += w * app[0];
    sumalpha += w * alpha;
  }
  sumint/=sumw;
  app[0] = static_cast<unsigned char>(sumint);
  sumalpha /= sumw;
  sumcolor/=sumw;
  color_app[0] = (unsigned char) (sumcolor[0] * 255); color_app[2] = (unsigned char)(sumcolor[2]*255); color_app[4]= (unsigned char) (sumcolor[4] * 255);
  boxm2_data_traits<BOXM2_ALPHA>::datatype alpha = static_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype>(sumalpha);
  target_app_data_->data()[tindx] = app;
  target_alpha_data_->data()[tindx] = alpha;
}

void boxm2_vecf_cranium_scene::apply_vector_field_to_target(vcl_vector<vgl_vector_3d<double> > const& vf,
                                                              vcl_vector<bool> const& valid){
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
    int n = static_cast<unsigned>(box_cell_centers_.size());

  if(n==0)
    return;//shouldn't happen
  vul_timer t;
  // iterate over the target cells and interpolate info from source
  for(int j = 0; j<n; ++j){
    if(!valid[j])
      continue;
    // target cell center translated back to source box, and should be a sphere point
    // but we don't need to check, since what is needed is the source cell for the rotation
    vgl_point_3d<double> p = box_cell_centers_[j].cell_center_-params_.offset_;

    vgl_point_3d<double> src = p + vf[j];//add inverse vector field

    // find closest sphere voxel cell
    unsigned sindx, dindx;
    if(!this->find_nearest_data_index(CRANIUM, src, dindx))
      continue;
    sindx = data_index_to_cell_index_[dindx];
    unsigned tindx = box_cell_centers_[j].data_index_;

    this->interpolate_vector_field(src, sindx, dindx, tindx,
                                   cranium_cell_centers_, cell_neighbor_cell_index_,
                                   cell_neighbor_data_index_);
  }
  //  vcl_cout << "Apply eye vector field in " << t.real()/1000.0 << " sec.\n";
}

void boxm2_vecf_cranium_scene::map_to_target(boxm2_scene_sptr target_scene){
  if(!target_data_extracted_){
    this->extract_target_block_data(target_scene);
    target_data_extracted_  = true;
  }

  if(intrinsic_change_ ){
    intrinsic_change_ = false;
    this->clear_target(target_scene);
  }
#if 1
  vgl_rotation_3d<double> rot;
  vcl_vector<vgl_vector_3d<double> > invf;
  vcl_vector<bool> valid;
  this->inverse_vector_field(rot, invf, valid);
  this->apply_vector_field_to_target(invf, valid);
#endif
}


bool boxm2_vecf_cranium_scene::set_params(boxm2_vecf_articulated_params const& params){
  try{
    boxm2_vecf_cranium_params const& params_ref = dynamic_cast<boxm2_vecf_cranium_params const &>(params);
    intrinsic_change_ = this->vfield_params_change_check(params_ref); 
    params_ = boxm2_vecf_cranium_params(params_ref);
#if _DEBUG
    vcl_cout<< "intrinsic change? "<<intrinsic_change_<<vcl_endl;
#endif
    if(intrinsic_change_){
      this->rebuild();
    }
    return true;
  }catch(std::exception e){
    vcl_cout<<" Can't downcast cranium parameters! PARAMETER ASSIGNMENT PHAILED!"<<vcl_endl;
    return false;
  }
}

bool boxm2_vecf_cranium_scene::vfield_params_change_check(const boxm2_vecf_cranium_params & params){
#if 0
  double tol = 0.001;
  bool intrinsic_change = false;
  //  intrinsic_change |= fabs(this->params_.crease_dphi_rad_ - params.crease_dphi_rad_)>tol;
  intrinsic_change |= fabs(this->params_.dphi_rad_ - params.dphi_rad_)              >tol;
  intrinsic_change |= fabs(this->params_.brow_angle_rad_  - params.brow_angle_rad_) > tol;
  intrinsic_change |= fabs(this->params_.eye_radius_  - params.eye_radius_) > tol;
  intrinsic_change |= fabs(this->params_.scale_x_coef_  - params.scale_x_coef_) > tol;
  intrinsic_change |= fabs(this->params_.scale_y_coef_  - params.scale_y_coef_) > tol;

  intrinsic_change |= fabs((float)this->params_.pupil_intensity_               - (float)params.pupil_intensity_) > tol;
  intrinsic_change |= fabs((float)this->params_.sclera_intensity_              - (float)params.sclera_intensity_) > tol;
  intrinsic_change |= fabs((float)this->params_.lower_eyelid_intensity_        - (float)params.lower_eyelid_intensity_) > tol;
  intrinsic_change |= fabs((float)this->params_.eyelid_intensity_              - (float)params.eyelid_intensity_) > tol;
  intrinsic_change |= fabs((float)this->params_.eyelid_crease_upper_intensity_ - (float)params.eyelid_crease_upper_intensity_) > tol;
  return intrinsic_change;
#endif
  return false;//temporary
}
void boxm2_vecf_cranium_scene::reset_buffers(){
  vcl_vector<boxm2_block_id> blocks = base_model_->get_block_ids();
  boxm2_block_metadata mdata = base_model_->get_block_metadata_const(blocks[0]);


  app_data_      ->set_default_value(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(), mdata);
  alpha_data_    ->set_default_value(boxm2_data_traits<BOXM2_ALPHA>::prefix(), mdata);
  cranium_      ->set_default_value(boxm2_data_traits<BOXM2_PIXEL>::prefix(), mdata);
  cranium_cell_centers_.clear();
  cranium_cell_data_index_.clear();

  cell_neighbor_cell_index_.clear();
  data_index_to_cell_index_.clear();
  cell_neighbor_data_index_.clear();

}
// find the orbit cell locations in the target volume
void boxm2_vecf_cranium_scene::determine_target_box_cell_centers(){
  vgl_box_3d<double> source_box = blk_->bounding_box_global();
  vgl_box_3d<double> offset_box(source_box.centroid() + params_.offset_ ,source_box.width(),source_box.height(),source_box.depth(),vgl_box_3d<double>::centre);
  if(target_blk_){
    box_cell_centers_ = target_blk_->cells_in_box(offset_box);
    vcl_cout << "N target cells = " << box_cell_centers_.size() << '\n';
  }
}
