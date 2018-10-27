#include <iostream>
#include <algorithm>
#include <limits>
#include <set>
#include "boxm2_vecf_mandible_scene.h"
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_refine_block_multi_data.h>
#include <boxm2/cpp/algo/boxm2_surface_distance_refine.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_quaternion.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/algo/vgl_orient_box_3d.h>
#include <vgl/vgl_intersection.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_closest_point.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <vnl/vnl_random.h>
#include <vul/vul_timer.h>
typedef boxm2_data_traits<BOXM2_PIXEL>::datatype pixtype;

void boxm2_vecf_mandible_scene::extract_block_data(){
  boxm2_vecf_articulated_scene::extract_source_block_data();
  mandible_base_  = boxm2_cache::instance()->get_data_base(base_model_,blk_id_,boxm2_data_traits<BOXM2_PIXEL>::prefix("mandible"));
  mandible_base_->enable_write();
  mandible_data_ = reinterpret_cast<boxm2_data_traits<BOXM2_PIXEL>::datatype*>(mandible_base_->data_buffer());
  mandible_size_ = static_cast<int>(mandible_base_->buffer_length());
}

// after loading the block initialize all the cell indices from the block labels, e.g., cell == LEFT_RAMUS, cell == LEFT_ANGLE, etc.
void boxm2_vecf_mandible_scene::cache_cell_centers_from_anatomy_labels(){
  std::vector<cell_info> source_cell_centers = blk_->cells_in_box(source_bb_);
  for(auto & source_cell_center : source_cell_centers){
    unsigned dindx = source_cell_center.data_index_;
    auto alpha = static_cast<float>(alpha_data_[dindx]);
    bool mandible = mandible_data_[dindx]   > pixtype(0);
    if(mandible||alpha>alpha_init_){
      auto mandible_index  = static_cast<unsigned>(mandible_cell_centers_.size());
      mandible_cell_centers_.push_back(source_cell_center.cell_center_);
      mandible_cell_data_index_.push_back(dindx);
      data_index_to_cell_index_[dindx] = mandible_index;
      // new cell that doesn't have appearance or anatomy data
      // this condition can happen if an unrefined cell center is
      // outside the distance tolerance to the geo surface, but when
      // refined, a leaf cell is within tolerance
      if(!mandible){
        params_.app_[0]=params_.mandible_intensity_;
        app_data_[dindx]=params_.app_;
        mandible_data_[dindx] = static_cast<pixtype>(true);
      }
    }
#if 0
        unsigned left_ramus_index  = static_cast<unsigned>(left_ramus_cell_centers_.size());
        unsigned left_angle_index  = static_cast<unsigned>(left_angle_cell_centers_.size());
        unsigned body_index  = static_cast<unsigned>(body_cell_centers_.size());
        unsigned right_angle_index  = static_cast<unsigned>(right_angle_cell_centers_.size());
        unsigned right_ramus_index  = static_cast<unsigned>(right_angle_cell_centers_.size());
                                                                    // no warnings
        bool left_ramus       = static_cast<unsigned char>(left_ramus_->data()[indx])   > 0;
        bool left_angle       = static_cast<unsigned char>(left_angle_->data()[indx])   > 0;
        bool body             = static_cast<unsigned char>(body_->data()[indx])         > 0;
        bool right_angle      = static_cast<unsigned char>(right_angle_->data()[indx])  > 0;
        bool right_ramus      = static_cast<unsigned char>(right_ramus_->data()[indx])  > 0;


        if(left_ramus){
          left_ramus_cell_centers_.push_back(p);
          left_ramus_cell_data_index_.push_back(indx);
          left_ramus_data_index_to_cell_index_[indx] = left_ramus_index;
        }

        if(left_angle){
          left_angle_cell_centers_.push_back(p);
          left_angle_cell_data_index_.push_back(indx);
          left_angle_data_index_to_cell_index_[indx] = left_angle_index;
        }

        if(body){
          body_cell_centers_.push_back(p);
          body_cell_data_index_.push_back(indx);
          body_data_index_to_cell_index_[indx] = body_index;
        }

        if(right_angle){
          right_angle_cell_centers_.push_back(p);
          right_angle_cell_data_index_.push_back(indx);
          right_angle_data_index_to_cell_index_[indx] = right_angle_index;
        }

        if(right_ramus){
          right_ramus_cell_centers_.push_back(p);
          right_ramus_cell_data_index_.push_back(indx);
          right_ramus_data_index_to_cell_index_[indx] = right_ramus_index;
        }
#endif
  }
}
// main constructors
boxm2_vecf_mandible_scene::boxm2_vecf_mandible_scene(std::string const& scene_file):
  boxm2_vecf_articulated_scene(scene_file),mandible_base_(nullptr), left_ramus_(nullptr), left_angle_(nullptr), body_(nullptr),
  right_angle_(nullptr), right_ramus_(nullptr), intrinsic_change_(false){
  boxm2_lru_cache::create(base_model_);
  this->extract_block_data();
  this->cache_cell_centers_from_anatomy_labels();
  mandible_geo_.set_params(params_);
}
boxm2_vecf_mandible_scene::boxm2_vecf_mandible_scene(std::string const& scene_file, std::string const& geometry_file):
  boxm2_vecf_articulated_scene(scene_file),mandible_base_(nullptr),
  left_ramus_(nullptr), left_angle_(nullptr), body_(nullptr), right_angle_(nullptr), right_ramus_(nullptr), intrinsic_change_(false)
{
  mandible_geo_ = boxm2_vecf_mandible(geometry_file);
  mandible_geo_.set_params(params_);
  this->extrinsic_only_ = true;
  target_blk_ = nullptr;
  target_data_extracted_ = false;
  boxm2_lru_cache::create(base_model_);
  this->extract_block_data();
  this->has_background_ = false;
  this->build_mandible();
  this->paint_mandible();
  std::vector<std::string> prefixes;
  prefixes.emplace_back("alpha");
  prefixes.emplace_back("boxm2_mog3_grey");
  prefixes.emplace_back("boxm2_num_obs");
  prefixes.emplace_back("boxm2_pixel_mandible");
  boxm2_surface_distance_refine<boxm2_vecf_mandible>(mandible_geo_, base_model_, prefixes, params_.neighbor_radius());
  boxm2_surface_distance_refine<boxm2_vecf_mandible>(mandible_geo_, base_model_, prefixes, params_.neighbor_radius());
  //boxm2_surface_distance_refine<boxm2_vecf_mandible>(mandible_geo_, base_model_, prefixes);
  this->rebuild();
 }

boxm2_vecf_mandible_scene::boxm2_vecf_mandible_scene(std::string const& scene_file, std::string const&  /*geometry_file*/, std::string const& params_file_name):
  boxm2_vecf_articulated_scene(scene_file),left_ramus_(nullptr), left_angle_(nullptr), body_(nullptr), right_angle_(nullptr), right_ramus_(nullptr){

  std::ifstream params_file(params_file_name.c_str());
  if (!params_file){
    std::cout<<" could not open params file construction fails!: "<<params_file_name<<std::endl;
    return;
  }
  params_file >> this->params_;
  mandible_geo_.set_params(params_);
  this->extrinsic_only_ = true;
  target_blk_ = nullptr;
  target_data_extracted_ = false;
  boxm2_lru_cache::create(base_model_);
  this->extract_block_data();
  this->has_background_ = false;
  this->build_mandible();
  this->paint_mandible();
  std::vector<std::string> prefixes;
  prefixes.emplace_back("alpha");
  prefixes.emplace_back("boxm2_mog3_grey");
  prefixes.emplace_back("boxm2_num_obs");
  prefixes.emplace_back("boxm2_pixel_mandible");
  boxm2_surface_distance_refine<boxm2_vecf_mandible>(mandible_geo_, base_model_, prefixes);
  boxm2_surface_distance_refine<boxm2_vecf_mandible>(mandible_geo_, base_model_, prefixes);
  boxm2_surface_distance_refine<boxm2_vecf_mandible>(mandible_geo_, base_model_, prefixes);
  this->rebuild();
}
void boxm2_vecf_mandible_scene::rebuild(){
#if 0
  if(this->extrinsic_only_){
    std::cout<<" warning! rebuild called but scene accepts only extrinsic articulations!"<<std::endl;
    return;
  }
#endif
    this->extract_block_data();
    this->cache_cell_centers_from_anatomy_labels();
    this->cache_neighbors();
}
void boxm2_vecf_mandible_scene::cache_neighbors(){
  this->find_cell_neigborhoods();
#if 0
  this->find_left_ramus_cell_neigborhoods();
  this->find_left_angle_cell_neigborhoods();
  this->find_body_cell_neigborhoods();
  this->find_right_angle_cell_neigborhoods();
  this->find_right_ramus_cell_neigborhoods();
#endif
}

void boxm2_vecf_mandible_scene::build_mandible(){
  double len = params_.neighbor_radius()*dims_.x();
  double d_thresh = len;
  vgl_box_3d<double> bb = mandible_geo_.bounding_box();
   // cell in a box centers are in global coordinates
  std::vector<cell_info> ccs = blk_->cells_in_box(bb);
  for(auto & cc : ccs){
    const vgl_point_3d<double>& cell_center = cc.cell_center_;
    unsigned indx = cc.data_index_;
    double d = mandible_geo_(cell_center);
    if(d < d_thresh){
      if(!is_type_global(cell_center, MANDIBLE)){
        mandible_cell_centers_.push_back(cell_center);
        mandible_cell_data_index_.push_back(indx);
        data_index_to_cell_index_[indx]=static_cast<unsigned>(mandible_cell_centers_.size())-1;
        //float blending_factor = static_cast<float>(gauss(d,sigma_));
        alpha_data_[indx]= - std::log(1.0f - ( 0.99f ))/ static_cast<float>(this->subblock_len());
        mandible_data_[indx] = static_cast<pixtype>(true);
      }
    }
  }
}
void boxm2_vecf_mandible_scene::create_mandible(){
  this->build_mandible();
  this->find_cell_neigborhoods();
  this->paint_mandible();
}


void boxm2_vecf_mandible_scene::find_cell_neigborhoods(){
  vul_timer t;
  double distance = params_.neighbor_radius()*dims_.x();
  for(unsigned i = 0; i<mandible_cell_centers_.size(); i++){
      vgl_point_3d<double>& p = mandible_cell_centers_[i];
      unsigned indx_i = mandible_cell_data_index_[i];
      std::vector<vgl_point_3d<double> > nbrs = blk_->sub_block_neighbors(p, distance);
      for(auto & q : nbrs){
        unsigned indx_n;
        if(!blk_->data_index(q, indx_n))
          continue;
        auto iit= data_index_to_cell_index_.find(indx_n);
        if(iit == data_index_to_cell_index_.end())
          continue;
        if(iit->second==i)
                continue;
        cell_neighbor_cell_index_[i].push_back(iit->second);
        std::vector<unsigned>& indices = cell_neighbor_data_index_[indx_i];
        indices.push_back(indx_n);
      }
  }
  std::cout << "Find mandible cell neighborhoods in " << static_cast<double>(t.real())/1000.0 << " sec.\n";
}

 void boxm2_vecf_mandible_scene::recreate_mandible(){
   this->rebuild();
 }

void boxm2_vecf_mandible_scene::paint_mandible(){
  params_.app_[0]=params_.mandible_intensity_;
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  auto ns = static_cast<unsigned>(mandible_cell_centers_.size());
  for(unsigned i = 0; i<ns; ++i){
    unsigned indx = mandible_cell_data_index_[i];
    app_data_[indx] = params_.app_;
    nobs_data_[indx] = nobs;
  }
}

 bool boxm2_vecf_mandible_scene::is_type_data_index(unsigned data_index, boxm2_vecf_mandible_scene::anat_type type) const{
   if(type == MANDIBLE){
     auto mandible = static_cast<unsigned char>(mandible_data_[data_index]);
     return mandible>0;
   }
#if 0
   if(type == LEFT_RAMUS){
     unsigned char left_ramus = static_cast<unsigned char>(left_ramus_->data()[data_index]);
     return left_ramus>0;
   }
   if(type == LEFT_ANGLE){
     unsigned char left_angle = static_cast<unsigned char>(left_angle_->data()[data_index]);
     return left_angle>0;
   }
   if(type == BODY){
     unsigned char body = static_cast<unsigned char>(body_->data()[data_index]);
     return body>0;
   }
   if(type == RIGHT_ANGLE){
     unsigned char right_angle = static_cast<unsigned char>(right_angle_->data()[data_index]);
     return right_angle>0;
   }
   if(type == RIGHT_RAMUS){
     unsigned char right_ramus = static_cast<unsigned char>(right_ramus_->data()[data_index]);
     return right_ramus>0;
   }
#endif
   return false;
 }

bool boxm2_vecf_mandible_scene::is_type_global(vgl_point_3d<double> const& global_pt, boxm2_vecf_mandible_scene::anat_type  type) const{
  unsigned indx;
  bool success =  blk_->data_index(global_pt, indx);
  if (!success){
    //#if _DEBUG
    //    std::cout<<"point "<<global_pt<< " was out of eye scene bounding box "<<std::endl;
    //#endif
    return false;
}
  return this->is_type_data_index(indx, type);
}

// found depth is for debug purposes
bool boxm2_vecf_mandible_scene::find_nearest_data_index(boxm2_vecf_mandible_scene::anat_type type, vgl_point_3d<double> const& probe, double cell_len, unsigned& data_indx, int& found_depth) const{
  unsigned depth;
  bool found_probe = blk_->data_index(probe, data_indx, depth, cell_len);
  if(!found_probe)
    return false;
  if(!is_type_data_index(data_indx, type))
    return false;
  found_depth = static_cast<int>(depth);
  return true;
}

bool boxm2_vecf_mandible_scene::inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const{
  if(!mandible_geo_.inverse_vector_field(target_pt, inv_vf))
    return false;
  vgl_point_3d<double> rp = target_pt + inv_vf;
  if(!source_bb_.contains(rp))
    return false;
  unsigned dindx = 0;
  if(!blk_->data_index(rp, dindx))
    return false;
  if(!is_type_data_index(dindx, MANDIBLE))
    return false;
  return true;
}

bool boxm2_vecf_mandible_scene::coupled_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const{
  if(!mandible_geo_.inverse_vector_field(target_pt, inv_vf))
    return false;
  return true;// for now the coupled vector field extends through all of target space
}

void  boxm2_vecf_mandible_scene::inverse_vector_field(std::vector<vgl_vector_3d<double> >& vf, std::vector<bool>& valid) const{

  vul_timer t;
  //the target cell centers. the vector field could potentially be defined at all target points
  auto nt = static_cast<unsigned>(box_cell_centers_.size());
  vf.resize(nt);// initialized to 0
  valid.resize(nt, false);
  unsigned box_cnt = 0;
  for(unsigned i = 0; i<nt; ++i){
    vgl_vector_3d<double> inv_vf;
    if(!inverse_vector_field(box_cell_centers_[i].cell_center_,inv_vf))
      continue;
    box_cnt++;
    valid[i]=true;
    vf[i].set(inv_vf.x(), inv_vf.y(), inv_vf.z());
  }
  std::cout << "computed " << box_cnt << " pts "<< nt << " for mandible vector field in " << t.real()/1000.0 << " sec.\n";
}

////////// this stuff will be used later =====>
void boxm2_vecf_mandible_scene::create_left_ramus(){
   this->build_left_ramus();
   this->paint_left_ramus();
}


void boxm2_vecf_mandible_scene::find_left_ramus_cell_neigborhoods(){
  vul_timer t;
  double distance = params_.neighbor_radius()*subblock_len();
  for(unsigned i = 0; i<left_ramus_cell_centers_.size(); i++){
      vgl_point_3d<double>& p = left_ramus_cell_centers_[i];
      unsigned indx_i = left_ramus_cell_data_index_[i];
      std::vector<vgl_point_3d<double> > nbrs = blk_->sub_block_neighbors(p, distance);
      for(auto & q : nbrs){
        unsigned indx_n;
        if(!blk_->data_index(q, indx_n))
          continue;
        auto iit= left_ramus_data_index_to_cell_index_.find(indx_n);
        if(iit == left_ramus_data_index_to_cell_index_.end())
          continue;
        if(iit->second==i)
                continue;
        left_ramus_cell_neighbor_cell_index_[i].push_back(iit->second);
        std::vector<unsigned>& indices = left_ramus_cell_neighbor_data_index_[indx_i];
        indices.push_back(indx_n);
      }
  }
  std::cout << "Find left_ramus cell neighborhoods in " << static_cast<double>(t.real())/1000.0 << " sec.\n";
}

void boxm2_vecf_mandible_scene::build_left_ramus(){
  double len = 4 * dims_.x();
  double d_thresh = params_.neighbor_radius()*len;
  double margin = 1.0;//For now
  vgl_box_3d<double> bb;// = mandible_geo_.left_ramus_bounding_box(margin);
  // cells in  box centers are in global coordinates
  std::vector<cell_info> ccs = blk_->cells_in_box(bb);
  for(auto & cc : ccs){
    const vgl_point_3d<double>& cell_center = cc.cell_center_;
    unsigned indx = cc.data_index_;
    double d =0.0;// mandible_geo_.left_ramus_surface_distance(cell_center);

    if(d < d_thresh){
      left_ramus_cell_centers_.push_back(cell_center);
      left_ramus_cell_data_index_.push_back(indx);
      left_ramus_->data()[indx] = static_cast<pixtype>(true);
      left_ramus_data_index_to_cell_index_[indx]=static_cast<unsigned>(left_ramus_cell_centers_.size())-1;
      auto blending_factor = static_cast<float>(gauss(d,sigma_));
      alpha_data_[indx]= - std::log(1.0f - ( 0.95f ))/ static_cast<float>(this->subblock_len()) * blending_factor;
    }
  }
}

void boxm2_vecf_mandible_scene::paint_left_ramus(){
  params_.app_[0]=params_.left_ramus_intensity_;
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  auto ns = static_cast<unsigned>(left_ramus_cell_centers_.size());
  for(unsigned i = 0; i<ns; ++i){
    unsigned indx = left_ramus_cell_data_index_[i];
    app_data_[indx] = params_.app_;
    nobs_data_[indx] = nobs;
  }
}
///  <========  End of stuff to be used later===========
// == the full inverse vector field  p_source = p_target + vf ===
void boxm2_vecf_mandible_scene::inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const& unrefined_target_pts){
  auto n = static_cast<unsigned>(unrefined_target_pts.size());
  vfield_unrefined_.resize(n, vgl_vector_3d<double>(0.0, 0.0, 0.0));
  valid_unrefined_.resize(n, false);
  for(unsigned vf_index = 0; vf_index<n; ++vf_index){
    const vgl_point_3d<double>& p = unrefined_target_pts[vf_index];
    vgl_vector_3d<double> inv_vf;
    if(!mandible_geo_.inverse_vector_field(p, inv_vf))
      continue;
    vgl_point_3d<double> rot_p_in_source = p + inv_vf;
    if(!source_bb_.contains(rot_p_in_source))
      continue;
    valid_unrefined_[vf_index] = true;
    vfield_unrefined_[vf_index].set(inv_vf.x(), inv_vf.y(), inv_vf.z());
  }
}
// interpolate data around the inverted position of the target in the source reference frame. Interpolation weights are based
// on a Gaussian distribution with respect to distance from the source location
//
void boxm2_vecf_mandible_scene::interpolate_vector_field(vgl_point_3d<double> const& src, unsigned sindx, unsigned dindx, unsigned tindx,
                                                      std::vector<vgl_point_3d<double> > & cell_centers,
                                                      std::map<unsigned, std::vector<unsigned> >& cell_neighbor_cell_index,
                                                      std::map<unsigned, std::vector<unsigned> >&cell_neighbor_data_index){

  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
  boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype color_app;
  typedef vnl_vector_fixed<double,8> double8;
  double8 curr_color;

  const vgl_point_3d<double>& scell = cell_centers[sindx];
  //appearance and alpha data at source cell
  app = app_data_[dindx];
  boxm2_data_traits<BOXM2_ALPHA>::datatype alpha0 = alpha_data_[dindx];

  double sig = params_.gauss_sigma()*subblock_len();
  // interpolate using Gaussian weights based on distance to the source point
  double dc = vgl_distance(scell, src);
  const std::vector<unsigned>& nbr_cells = cell_neighbor_cell_index[sindx];
  const std::vector<unsigned>& nbr_data = cell_neighbor_data_index[dindx];
  double sumw = gauss(dc, sig), sumint = app[0]*sumw, sumalpha = alpha0*sumw;
  double8 sumcolor= sumw * curr_color;
  for(unsigned k = 0; k<nbr_cells.size(); ++k){
    double d = vgl_distance(cell_centers[nbr_cells[k]],src);
    unsigned nidx = nbr_data[k];
    double w = gauss(d, sig);
    sumw += w;
    app = app_data_[nidx];
    double alpha = alpha_data_[nidx];
    sumint   += w * app[0];
    sumalpha += w * alpha;
  }
  sumint/=sumw;
  app[0] = static_cast<unsigned char>(sumint);
  sumalpha /= sumw;
  sumcolor/=sumw;
  color_app[0] = (unsigned char) (sumcolor[0] * 255); color_app[2] = (unsigned char)(sumcolor[2]*255); color_app[4]= (unsigned char) (sumcolor[4] * 255);
  auto alpha = static_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype>(sumalpha);
  target_app_data_[tindx] = app;
  target_alpha_data_[tindx] = alpha;
}

bool boxm2_vecf_mandible_scene::apply_vector_field(cell_info const& target_cell, vgl_vector_3d<double> const& inv_vf){
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
  boxm2_data_traits<BOXM2_ALPHA>::datatype alpha = 0.0f;
  vgl_point_3d<double> trg_cent_in_source = target_cell.cell_center_;
  double side_len = target_cell.side_length_;
  unsigned tindx = target_cell.data_index_;
  vgl_point_3d<double> src = trg_cent_in_source + inv_vf;//add inverse vector field
  int depth = target_cell.depth_;//for debug purposes
  int found_depth;//for debug purposes
  unsigned sindx, dindx;
  if(!this->find_nearest_data_index(MANDIBLE, src, side_len, dindx, found_depth))
    return false;
  sindx = data_index_to_cell_index_[dindx];
  app = app_data_[dindx];
  alpha = alpha_data_[dindx];
  // use nearest neighbor interpolation for now
  target_app_data_[tindx] = app;
  target_alpha_data_[tindx] = alpha;
  return true;
}

void boxm2_vecf_mandible_scene::apply_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf,
                                                              std::vector<bool> const& valid){
  auto n = static_cast<unsigned>(box_cell_centers_.size());
  int valid_count = 0;
  if(n==0)
    return;//shouldn't happen
  vul_timer t;
  // iterate over the target cells and interpolate info from source
  for(unsigned j = 0; j<n; ++j){

    // if vector field is not defined then clear the cell
    if(!valid[j]){
      target_alpha_data_[box_cell_centers_[j].data_index_] = 0.0f;
      continue;
    }
    // cells have vector field defined but not mandible cells
    if(!this->apply_vector_field(box_cell_centers_[j], vf[j])){
      target_alpha_data_[box_cell_centers_[j].data_index_] = 0.0f;
      continue;
    }
    valid_count++;
  }
  std::cout << "Apply mandible vector field to " << valid_count << " out of " << n << " cells in " << t.real()/1000.0 << " sec.\n";
}

int boxm2_vecf_mandible_scene::prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index){
  int max_level = blk_->max_level();
  if(!valid_unrefined_[pt_index])
    return -1;
  // map the target back to source
  vgl_vector_3d<double> vtr = vfield_unrefined_[pt_index];
  vgl_point_3d<double> rot_center_in_source = sub_block_pt +  vtr;

  // sub_block axis-aligned corners in source
  vgl_point_3d<double> sbc_min(rot_center_in_source.x()-0.5*targ_dims_.x(),
                               rot_center_in_source.y()-0.5*targ_dims_.y(),
                               rot_center_in_source.z()-0.5*targ_dims_.z());

  vgl_point_3d<double> sbc_max(rot_center_in_source.x()+0.5*targ_dims_.x(),
                               rot_center_in_source.y()+0.5*targ_dims_.y(),
                               rot_center_in_source.z()+0.5*targ_dims_.z());

  vgl_box_3d<double> target_box_in_source;
  target_box_in_source.add(sbc_min);
  target_box_in_source.add(sbc_max);

  // rotate the target box in source by the inverse rotation
  // the box is rotated about its centroid
  vgl_orient_box_3d<double> target_obox(target_box_in_source, mandible_geo_.inv_rot().as_quaternion());
  vgl_box_3d<double> rot_target_box_in_source = target_obox.enclosing_box();
  vgl_box_3d<double> int_box = vgl_intersection(source_bb_, rot_target_box_in_source);
  if(int_box.is_empty())
    return -1;
  // the source blocks intersecting the rotated target box
  std::vector<vgl_point_3d<int> > int_sblks = blk_->sub_blocks_intersect_box(rot_target_box_in_source);

  // iterate through each intersecting source tree and find the maximum tree depth
  int max_depth = 0;
  for(auto & int_sblk : int_sblks){
    const uchar16& tree_bits = trees_(int_sblk.x(), int_sblk.y(), int_sblk.z());
    //safely cast since bit_tree is just temporary
    auto& uctree_bits = const_cast<uchar16&>(tree_bits);
    boct_bit_tree bit_tree(uctree_bits.data_block(), max_level);
    int dpth = bit_tree.depth();
    if(dpth>max_depth){
      max_depth = dpth;
    }
  }
  return max_depth;
}

void boxm2_vecf_mandible_scene::map_to_target(boxm2_scene_sptr target_scene){
  vul_timer t;
  // initially extract unrefined target data
  if(!target_data_extracted_)
    this->extract_target_block_data(target_scene);
  this->extract_unrefined_cell_info();//on articulated_scene
  std::vector<vgl_point_3d<double> > tgt_pts;
  for(auto & cit : unrefined_cell_info_)
    tgt_pts.push_back(cit.pt_);

  // compute inverse vector field for prerefining the target
  this->inverse_vector_field_unrefined(tgt_pts);
  // refine the target to match the source tree refinement
   this->prerefine_target(target_scene);
  // have to extract target data again to refresh data bases and buffers after refinement
  this->extract_target_block_data(target_scene);
  this->determine_target_box_cell_centers();

  std::vector<vgl_vector_3d<double> > invf;
  std::vector<bool> valid;
  this->inverse_vector_field(invf, valid);
  this->apply_vector_field_to_target(invf, valid);
  target_data_extracted_  = true;
  std::cout << "Map to target in " << t.real()/1000.0 << " secs\n";
}


bool boxm2_vecf_mandible_scene::set_params(boxm2_vecf_articulated_params const& params){
  try{
    auto const& params_ref = dynamic_cast<boxm2_vecf_mandible_params const &>(params);
    intrinsic_change_ = this->vfield_params_change_check(params_ref);
    params_ = boxm2_vecf_mandible_params(params_ref);
    mandible_geo_.set_params(params_);
#if _DEBUG
    std::cout<< "intrinsic change? "<<intrinsic_change_<<std::endl;
#endif
    if(intrinsic_change_){
      this->rebuild();
    }
    return true;
  }catch(std::exception e){
    std::cout<<" Can't downcast orbit parameters! PARAMETER ASSIGNMENT PHAILED!"<<std::endl;
    return false;
  }
}

bool boxm2_vecf_mandible_scene::vfield_params_change_check(const boxm2_vecf_mandible_params &  /*params*/){
  return false;//temporary
}

void boxm2_vecf_mandible_scene::determine_target_box_cell_centers(){
  vgl_box_3d<double> offset_box(source_bb_.centroid() + params_.offset_ ,source_bb_.width(),source_bb_.height(),source_bb_.depth(),vgl_box_3d<double>::centre);
  if(target_blk_){
    box_cell_centers_ = target_blk_->cells_in_box(offset_box);
  }
}
