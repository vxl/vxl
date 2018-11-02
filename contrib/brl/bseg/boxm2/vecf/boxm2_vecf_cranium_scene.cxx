#include <iostream>
#include <algorithm>
#include <limits>
#include <set>
#include "boxm2_vecf_cranium_scene.h"
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
#include <vul/vul_timer.h>
#include <boxm2/cpp/algo/boxm2_surface_distance_refine.h>
#include <boxm2/cpp/algo/boxm2_refine_block_multi_data.h>
typedef vnl_vector_fixed<unsigned char, 16> uchar16;
typedef boxm2_data_traits<BOXM2_PIXEL>::datatype pixtype;

void boxm2_vecf_cranium_scene::extract_block_data(){
  boxm2_vecf_articulated_scene::extract_source_block_data();
  cranium_base_  = boxm2_cache::instance()->get_data_base(base_model_,blk_id_,boxm2_data_traits<BOXM2_PIXEL>::prefix("cranium"));
  cranium_base_->enable_write();
  cranium_data_=reinterpret_cast<boxm2_data_traits<BOXM2_PIXEL>::datatype*>(cranium_base_->data_buffer());

}

// after loading the block initialize all the cell indices from the block labels, e.g., cell == LEFT_RAMUS, cell == LEFT_ANGLE, etc.
void boxm2_vecf_cranium_scene::cache_cell_centers_from_anatomy_labels(){
  std::vector<cell_info> source_cell_centers = blk_->cells_in_box(source_bb_);
  for(auto & source_cell_center : source_cell_centers){
    unsigned dindx = source_cell_center.data_index_;
    auto alpha = static_cast<float>(alpha_data_[dindx]);
    bool cranium = cranium_data_[dindx]   > pixtype(0);
    if(cranium||alpha>alpha_init_){
      auto cranium_index  = static_cast<unsigned>(cranium_cell_centers_.size());
      cranium_cell_centers_.push_back(source_cell_center.cell_center_);
      cranium_cell_data_index_.push_back(dindx);
      data_index_to_cell_index_[dindx] = cranium_index;
      // new cell that doesn't have appearance or anatomy data
      // this condition can happen if an unrefined cell center is
      // outside the distance tolerance to the geo surface, but when
      // refined, a leaf cell is within tolerance
      if(!cranium){
        params_.app_[0]=params_.cranium_intensity_;
        app_data_[dindx]=params_.app_;
        cranium_data_[dindx] = static_cast<pixtype>(true);
      }
    }else{
      params_.app_ = app_data_[dindx];
      auto alp = static_cast<float>(alpha_data_[dindx]);
    }
  }
}
// constructors
boxm2_vecf_cranium_scene::boxm2_vecf_cranium_scene(std::string const& scene_file): boxm2_vecf_articulated_scene(scene_file), cranium_data_(nullptr){
  boxm2_lru_cache::create(base_model_);
  cranium_geo_.set_params(params_);
  vul_timer t;
  this->rebuild();
  std::cout << "Create cranium scene in " << t.real()/1000.0 << "seconds\n";
}


boxm2_vecf_cranium_scene::boxm2_vecf_cranium_scene(std::string const& scene_file, std::string const& geometry_file):
  boxm2_vecf_articulated_scene(scene_file)
{
  cranium_geo_ = boxm2_vecf_cranium(geometry_file);
  cranium_geo_.set_params(params_);
  target_blk_ = nullptr;
  target_data_extracted_ = false;
  boxm2_lru_cache::create(base_model_);
  this->extract_block_data();
  this->has_background_ = false;
  this->build_cranium();
  this->paint_cranium();
  std::vector<std::string> prefixes;
  prefixes.emplace_back("alpha");
  prefixes.emplace_back("boxm2_mog3_grey");
  prefixes.emplace_back("boxm2_num_obs");
  prefixes.emplace_back("boxm2_pixel_cranium");
  boxm2_surface_distance_refine<boxm2_vecf_cranium>(cranium_geo_, base_model_, prefixes);
  boxm2_surface_distance_refine<boxm2_vecf_cranium>(cranium_geo_, base_model_, prefixes);
  //  boxm2_surface_distance_refine<boxm2_vecf_cranium>(cranium_geo_, base_model_, prefixes);
  this->rebuild();
  }


void boxm2_vecf_cranium_scene::rebuild(){
    this->extract_block_data();
    this->cache_cell_centers_from_anatomy_labels();
    //    this->cache_neighbors();
}

void boxm2_vecf_cranium_scene::cache_neighbors(){
  this->find_cell_neigborhoods();
}

void boxm2_vecf_cranium_scene::build_cranium(){
  double len_coef = params_.neighbor_radius();
  vgl_box_3d<double> bb = cranium_geo_.bounding_box();
   // cell in a box centers are in global coordinates
  std::vector<cell_info> ccs = blk_->cells_in_box(bb);
  for(auto & cc : ccs){
    const vgl_point_3d<double>& cell_center = cc.cell_center_;
    unsigned indx = cc.data_index_;
    double d = cranium_geo_.distance(cell_center);
    double d_thresh = len_coef*cc.side_length_;
    if(d < d_thresh){
      if(!is_type_global(cell_center, CRANIUM)){
        cranium_cell_centers_.push_back(cell_center);
        cranium_cell_data_index_.push_back(indx);
        data_index_to_cell_index_[indx]=static_cast<unsigned>(cranium_cell_centers_.size())-1;
        auto blending_factor = static_cast<float>(gauss(d,sigma_));
        alpha_data_[indx]= - std::log(1.0f - ( 0.95f ))/ static_cast<float>(this->subblock_len()) * blending_factor;
        cranium_data_[indx] = static_cast<pixtype>(true);
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
  std::cout << "Find cranium cell neighborhoods in " << static_cast<double>(t.real())/1000.0 << " sec.\n";
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
  auto ns = static_cast<unsigned>(cranium_cell_centers_.size());
  for(unsigned i = 0; i<ns; ++i){
    unsigned indx = cranium_cell_data_index_[i];
    app_data_[indx] = params_.app_;
    nobs_data_[indx] = nobs;
  }
}

 bool boxm2_vecf_cranium_scene::is_type_data_index(unsigned data_index, boxm2_vecf_cranium_scene::anat_type type) const{

   if(type == CRANIUM){
     auto cranium = static_cast<unsigned char>(cranium_data_[data_index]);
     return cranium>0;
   }
   return false;
 }

bool boxm2_vecf_cranium_scene::is_type_global(vgl_point_3d<double> const& global_pt, boxm2_vecf_cranium_scene::anat_type  type) const{
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

bool boxm2_vecf_cranium_scene::find_nearest_data_index(boxm2_vecf_cranium_scene::anat_type type, vgl_point_3d<double> const& probe, double cell_len, unsigned& data_indx, int& found_depth) const{
  unsigned depth;
  bool found_probe = blk_->data_index(probe, data_indx, depth, cell_len);
  if(!found_probe)
    return false;
  if(!is_type_data_index(data_indx, type))
    return false;
  found_depth = static_cast<int>(depth);
  return true;
}

int boxm2_vecf_cranium_scene::prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index){
  int max_level = blk_->max_level();
  if(!valid_unrefined_[pt_index])
    return -1;
  // map the target back to source
  vgl_point_3d<double> center_in_source = sub_block_pt +  vfield_unrefined_[pt_index];

  // sub_block axis-aligned corners in source
  vgl_point_3d<double> sbc_min(center_in_source.x()-0.5*targ_dims_.x(),
                               center_in_source.y()-0.5*targ_dims_.y(),
                               center_in_source.z()-0.5*targ_dims_.z());

  vgl_point_3d<double> sbc_max(center_in_source.x()+0.5*targ_dims_.x(),
                               center_in_source.y()+0.5*targ_dims_.y(),
                               center_in_source.z()+0.5*targ_dims_.z());

  vgl_box_3d<double> target_box_in_source;
  target_box_in_source.add(sbc_min);
  target_box_in_source.add(sbc_max);

  // the source blocks intersecting the rotated target box
  std::vector<vgl_point_3d<int> > int_sblks = blk_->sub_blocks_intersect_box(target_box_in_source);

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

// == the full inverse vector field  p_source = p_target + vf ===
void boxm2_vecf_cranium_scene::inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const& unrefined_target_pts){
  auto n = static_cast<unsigned>(unrefined_target_pts.size());
  vfield_unrefined_.resize(n, vgl_vector_3d<double>(0.0, 0.0, 0.0));
  valid_unrefined_.resize(n, false);
  for(unsigned vf_index = 0;vf_index<n; ++vf_index){
    const vgl_point_3d<double>& p = unrefined_target_pts[vf_index];
    vgl_vector_3d<double> inv_vf;
    cranium_geo_.inverse_vector_field(p, inv_vf);
    vgl_point_3d<double> p_in_source = p + inv_vf;
    if(!source_bb_.contains(p_in_source))
      continue;
    valid_unrefined_[vf_index] = true;
    vfield_unrefined_[vf_index].set(inv_vf.x(), inv_vf.y(), inv_vf.z());
  }
}

bool boxm2_vecf_cranium_scene::inverse_vector_field(vgl_point_3d<double> const&  target_pt, vgl_vector_3d<double>& inv_vf) const{
  if(!cranium_geo_.inverse_vector_field(target_pt, inv_vf))
    return false;
  vgl_point_3d<double> tp = target_pt + inv_vf;
  if(!source_bb_.contains(tp))
    return false;
  // vf only defined for cells on the cranium
  if(!is_type_global(tp, CRANIUM))
    return false;
  return true;

}


void  boxm2_vecf_cranium_scene::inverse_vector_field(std::vector<vgl_vector_3d<double> >& vf,
                                                      std::vector<bool>& valid) const{
  vul_timer t;
  auto nt = static_cast<unsigned>(box_cell_centers_.size());
  vf.resize(nt, vgl_vector_3d<double>(0.0, 0.0, 0.0));// initialized to 0
  valid.resize(nt, false);
  for(unsigned i = 0; i<nt; ++i){
    vgl_vector_3d<double> inv_vf;
    if(!inverse_vector_field(box_cell_centers_[i].cell_center_, inv_vf))
      continue;
    valid[i]=true;
    vf[i].set(inv_vf.x(), inv_vf.y(), inv_vf.z());
  }


  std::cout << "computed cranium vector field in " << t.real()/1000.0 << " sec.\n";
}

//
// interpolate data around the inverted position of the target in the source reference frame. Interpolation weights are based
// on a Gaussian distribution with respect to distance from the source location
//
void boxm2_vecf_cranium_scene::interpolate_vector_field(vgl_point_3d<double> const& src, unsigned sindx, unsigned dindx, unsigned tindx,
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

bool boxm2_vecf_cranium_scene::apply_vector_field(cell_info const& target_cell, vgl_vector_3d<double> const& inv_vf){
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
  boxm2_data_traits<BOXM2_ALPHA>::datatype alpha = 0.0f;
  vgl_point_3d<double> trg_cent_in_source = target_cell.cell_center_;
  double side_len = target_cell.side_length_;
  unsigned tindx = target_cell.data_index_;
  vgl_point_3d<double> src = trg_cent_in_source + inv_vf;//add inverse vector field
  int depth = target_cell.depth_;//for debug purposes
  int found_depth;//for debug purposes
  unsigned sindx, dindx;
  if(!this->find_nearest_data_index(CRANIUM, src, side_len, dindx, found_depth))
    return false;
  sindx = data_index_to_cell_index_[dindx];
  app = app_data_[dindx];
  alpha = alpha_data_[dindx];
  target_app_data_[tindx] = app;
  target_alpha_data_[tindx] = alpha;
  return true;
}

void boxm2_vecf_cranium_scene::apply_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf,
                                                              std::vector<bool> const& valid){
  auto n = static_cast<unsigned>(box_cell_centers_.size());
  if(n==0)
    return;//shouldn't happen
  vul_timer t;
  // iterate over the target cells and interpolate info from source
  for(unsigned j = 0; j<n; ++j){

    // if vector field is not defined then assign zero alpha
    if(!valid[j]){
     target_alpha_data_[box_cell_centers_[j].data_index_] = 0.0f;
      continue;
    }
    // cells have vector field defined but not mandible cells
    if(!this->apply_vector_field(box_cell_centers_[j], vf[j])){
      target_alpha_data_[box_cell_centers_[j].data_index_] = 0.0f;
      continue;
    }
  }
  std::cout << "Apply cranium vector field to " << n << " cells in " << t.real()/1000.0 << " sec.\n";
}

void boxm2_vecf_cranium_scene::map_to_target(boxm2_scene_sptr target_scene){
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


bool boxm2_vecf_cranium_scene::set_params(boxm2_vecf_articulated_params const& params){
  try{
    auto const& params_ref = dynamic_cast<boxm2_vecf_cranium_params const &>(params);
    bool change = this->vfield_params_change_check(params_ref);
    params_ = boxm2_vecf_cranium_params(params_ref);
    cranium_geo_.set_params(params_);
#if _DEBUG
    //std::cout<< "intrinsic change? "<<intrinsic_change_<<std::endl;
#endif
    if(change){
      this->rebuild();
    }
    return true;
  }catch(std::exception e){
    std::cout<<" Can't downcast cranium parameters! PARAMETER ASSIGNMENT PHAILED!"<<std::endl;
    return false;
  }
}

bool boxm2_vecf_cranium_scene::vfield_params_change_check(const boxm2_vecf_cranium_params &  /*params*/){
  // move to parames class
  return false;//temporary
}

// find the cranium cell locations in the target volume
void boxm2_vecf_cranium_scene::determine_target_box_cell_centers(){
  if(!blk_){
    std::cout << "Null source block -- FATAL!\n";
    return;
  }

  vgl_box_3d<double> offset_box(source_bb_.centroid() + params_.offset_ ,source_bb_.width(),source_bb_.height(),source_bb_.depth(),vgl_box_3d<double>::centre);
  if(target_blk_){
    box_cell_centers_ = target_blk_->cells_in_box(target_blk_->bounding_box_global());
  }else{
   std::cout << "Null target block -- FATAL!\n";
    return;
  }
}
