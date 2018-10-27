#include <algorithm>
#include <limits>
#include "boxm2_vecf_middle_fat_pocket_scene.h"
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_box_3d.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_closest_point.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <vul/vul_timer.h>
#include <boxm2/cpp/algo/boxm2_surface_distance_refine.h>
#include <boxm2/cpp/algo/boxm2_refine_block_multi_data.h>
typedef vnl_vector_fixed<unsigned char, 16> uchar16;
typedef boxm2_data_traits<BOXM2_PIXEL>::datatype pixtype;

void boxm2_vecf_middle_fat_pocket_scene::extract_block_data(){
  boxm2_vecf_articulated_scene::extract_source_block_data();
  middle_fat_pocket_base_  = boxm2_cache::instance()->get_data_base(base_model_,blk_id_,boxm2_data_traits<BOXM2_PIXEL>::prefix("middle_fat_pocket"));
  middle_fat_pocket_base_->enable_write();
  middle_fat_pocket_data_=reinterpret_cast<boxm2_data_traits<BOXM2_PIXEL>::datatype*>(middle_fat_pocket_base_->data_buffer());
}

void boxm2_vecf_middle_fat_pocket_scene::create_anatomy_labels(){
  if(!blk_) return;
  vgl_box_3d<double> source_box = blk_->bounding_box_global();
  std::vector<cell_info> source_cell_centers = blk_->cells_in_box(source_box);
  for(auto & source_cell_center : source_cell_centers){
    int depth = source_cell_center.depth_;
    unsigned data_index = source_cell_center.data_index_;
    auto alpha = static_cast<float>(alpha_data_[data_index]);
    if(alpha>alpha_init_){
      middle_fat_pocket_data_[data_index] = static_cast<boxm2_data_traits<BOXM2_PIXEL>::datatype>(true);
    }
    else{
      middle_fat_pocket_data_[data_index]= static_cast<boxm2_data_traits<BOXM2_PIXEL>::datatype>(false);
    }
  }
}


//retrieve cells with middle_fat_pocket anatomy label
void boxm2_vecf_middle_fat_pocket_scene::cache_cell_centers_from_anatomy_labels(){
  std::vector<cell_info> source_cell_centers = blk_->cells_in_box(source_bb_);
  for(auto & source_cell_center : source_cell_centers){
    unsigned dindx = source_cell_center.data_index_;
    float alpha = alpha_data_[dindx];
    bool middle_fat_pocket = middle_fat_pocket_data_[dindx]   > pixtype(0);
    if(middle_fat_pocket||alpha>alpha_init_){
      auto middle_fat_pocket_index  = static_cast<unsigned>(middle_fat_pocket_cell_centers_.size());
      const vgl_point_3d<double>& p = source_cell_center.cell_center_;
      middle_fat_pocket_cell_centers_.push_back(p);
      middle_fat_pocket_cell_data_index_.push_back(dindx);
      data_index_to_cell_index_[dindx] = middle_fat_pocket_index;
      // new cell that doesn't have appearance or anatomy data
      // this condition can happen if an unrefined cell center is
      // outside the distance tolerance to the geo surface, but when
      // refined, a leaf cell is within tolerance
      if(!middle_fat_pocket){
        params_.app_[0]=params_.middle_fat_pocket_intensity_;
        app_data_[dindx]=params_.app_;
        middle_fat_pocket_data_[dindx] = static_cast<pixtype>(true);
      }
    }
  }
}

boxm2_vecf_middle_fat_pocket_scene::boxm2_vecf_middle_fat_pocket_scene(std::string const& scene_file, std::string const& geometry_file, bool initialize):
  boxm2_vecf_articulated_scene(scene_file){
  source_model_exists_=!initialize;//on articulated scene
  middle_fat_pocket_geo_ = boxm2_vecf_middle_fat_pocket(geometry_file);
  boxm2_lru_cache::create(base_model_);
  this->extract_block_data();
  if(initialize){
    this->build_middle_fat_pocket();
    std::vector<std::string> prefixes;
    prefixes.emplace_back("alpha");
    prefixes.emplace_back("boxm2_mog3_grey");
    prefixes.emplace_back("boxm2_num_obs");
    prefixes.emplace_back("boxm2_pixel_middle_fat_pocket");
    double nrad = params_.neighbor_radius();
    boxm2_surface_distance_refine<boxm2_vecf_middle_fat_pocket>(middle_fat_pocket_geo_, base_model_, prefixes, nrad);
    boxm2_surface_distance_refine<boxm2_vecf_middle_fat_pocket>(middle_fat_pocket_geo_, base_model_, prefixes, nrad);
    boxm2_surface_distance_refine<boxm2_vecf_middle_fat_pocket>(middle_fat_pocket_geo_, base_model_, prefixes, nrad);
  }
  this->rebuild();
}

void boxm2_vecf_middle_fat_pocket_scene::rebuild(){
  this->extract_block_data();
  this->cache_cell_centers_from_anatomy_labels();
  this->paint_middle_fat_pocket(); // no appearance for unrefined scene
//this->cache_neighbors();
}
void boxm2_vecf_middle_fat_pocket_scene::cache_neighbors(){
  this->find_cell_neigborhoods();
}


void boxm2_vecf_middle_fat_pocket_scene::build_middle_fat_pocket(){
  double len_coef = 0.5*params_.neighbor_radius(); //JLM experiment
  vgl_box_3d<double> bb = middle_fat_pocket_geo_.bounding_box();
   // cell in a box centers are in global coordinates
  std::vector<cell_info> ccs = blk_->cells_in_box(bb);
  for(auto & cc : ccs){
    const vgl_point_3d<double>& cell_center = cc.cell_center_;
    unsigned indx = cc.data_index_;
   double d = middle_fat_pocket_geo_.distance(cell_center);
   double d_thresh = len_coef*cc.side_length_;
    if(d < d_thresh){
      if(!is_type_global(cell_center, MIDDLE_FAT_POCKET)){
        middle_fat_pocket_cell_centers_.push_back(cell_center);
        middle_fat_pocket_cell_data_index_.push_back(indx);
        data_index_to_cell_index_[indx]=static_cast<unsigned>(middle_fat_pocket_cell_centers_.size())-1;
        //float blending_factor = static_cast<float>(gauss(d,sigma_));
        alpha_data_[indx]= - std::log(1.0f - ( 0.95f ))/ static_cast<float>(this->subblock_len());// * blending_factor;
        middle_fat_pocket_data_[indx] = static_cast<pixtype>(true);
      }
    }else{
      alpha_data_[indx]=0.0f;
      middle_fat_pocket_data_[indx] = static_cast<pixtype>(false);
    }
  }
}

void boxm2_vecf_middle_fat_pocket_scene::find_cell_neigborhoods(){
  vul_timer t;
  double distance = params_.neighbor_radius()*subblock_len();
  for(unsigned i = 0; i<middle_fat_pocket_cell_centers_.size(); i++){
      vgl_point_3d<double>& p = middle_fat_pocket_cell_centers_[i];
      unsigned indx_i = middle_fat_pocket_cell_data_index_[i];
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
  std::cout << "Find middle_fat_pocket cell neighborhoods in " << static_cast<double>(t.real())/1000.0 << " sec.\n";
}

void boxm2_vecf_middle_fat_pocket_scene::paint_middle_fat_pocket(){
  params_.app_[0]=params_.middle_fat_pocket_intensity_;
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  auto ns = static_cast<unsigned>(middle_fat_pocket_cell_centers_.size());
  for(unsigned i = 0; i<ns; ++i){
    unsigned indx = middle_fat_pocket_cell_data_index_[i];
    if(indx%3 == 0)
      params_.app_[0]=params_.middle_fat_pocket_intensity_+static_cast<unsigned char>(75);
    else
      params_.app_[0]=params_.middle_fat_pocket_intensity_;
    app_data_[indx] = params_.app_;
    nobs_data_[indx] = nobs;
  }
}

bool boxm2_vecf_middle_fat_pocket_scene::is_type_data_index(unsigned data_index, boxm2_vecf_middle_fat_pocket_scene::anat_type type) const{

   if(type == MIDDLE_FAT_POCKET){
     auto middle_fat_pocket = static_cast<unsigned char>(middle_fat_pocket_data_[data_index]);
     return middle_fat_pocket > static_cast<unsigned char>(0);
   }
   return false;
}

bool boxm2_vecf_middle_fat_pocket_scene::is_type_global(vgl_point_3d<double> const& global_pt, boxm2_vecf_middle_fat_pocket_scene::anat_type  type) const{
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

bool boxm2_vecf_middle_fat_pocket_scene::find_nearest_data_index(boxm2_vecf_middle_fat_pocket_scene::anat_type type, vgl_point_3d<double> const& probe, double cell_len, unsigned& data_indx, int& found_depth) const{
  unsigned depth;
  bool found_probe = blk_->data_index(probe, data_indx, depth, cell_len);
  if(!found_probe)
    return false;
  if(!is_type_data_index(data_indx, type))
    return false;
  found_depth = static_cast<int>(depth);
  return true;
}
bool boxm2_vecf_middle_fat_pocket_scene::inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const{
  bool valid = middle_fat_pocket_geo_.inverse_vector_field(target_pt, inv_vf);
  if(!valid) return false;
   vgl_point_3d<double> rp = target_pt + inv_vf;
  if(!source_bb_.contains(rp))
    return false;
  unsigned dindx = 0;
  if(!blk_->data_index(rp, dindx))
    return false;
  if(!is_type_data_index(dindx, MIDDLE_FAT_POCKET))
    return false;
  return true;
}
void boxm2_vecf_middle_fat_pocket_scene::inverse_vector_field(std::vector<vgl_vector_3d<double> >& vf,
                                                      std::vector<bool>& valid) const{
  vul_timer t;
  auto nt = static_cast<unsigned>(box_cell_centers_.size());
  vf.resize(nt);// initialized to 0
  valid.resize(nt, false);
  unsigned cnt = 0;
  for(unsigned i = 0; i<nt; ++i){
    vgl_vector_3d<double> inv_vf;
    const vgl_point_3d<double> p = box_cell_centers_[i].cell_center_;
    if(!inverse_vector_field(p, inv_vf))
      continue;
    cnt++;
    valid[i]=true;
    vf[i].set(inv_vf.x(), inv_vf.y(), inv_vf.z());
  }
  std::cout << "computed " << cnt << " pts out of "<< nt << " for middle_fat_pocket vector field in " << t.real()/1000.0 << " sec.\n";
}

bool boxm2_vecf_middle_fat_pocket_scene::coupled_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const{
  return middle_fat_pocket_geo_.closest_inverse_vector_field(target_pt, inv_vf);
}

//
// interpolate data around the inverted position of the target in the source reference frame. Interpolation weights are based
// on a Gaussian distribution with respect to distance from the source location
//
void boxm2_vecf_middle_fat_pocket_scene::interpolate_vector_field(vgl_point_3d<double> const& src, unsigned sindx, unsigned dindx, unsigned tindx,
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

bool boxm2_vecf_middle_fat_pocket_scene::apply_vector_field(cell_info const& target_cell, vgl_vector_3d<double> const& inv_vf){
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
  boxm2_data_traits<BOXM2_ALPHA>::datatype alpha = 0.0f;
  vgl_point_3d<double> trg_cent_in_source = target_cell.cell_center_;
  double side_len = target_cell.side_length_;
  unsigned tindx = target_cell.data_index_;
  vgl_point_3d<double> src = trg_cent_in_source + inv_vf;//add inverse vector field
  int depth = target_cell.depth_;//for debug purposes
  int found_depth;//for debug purposes
  unsigned sindx, dindx;
  if(!this->find_nearest_data_index(MIDDLE_FAT_POCKET, src, side_len, dindx, found_depth))
    return false;
  sindx = data_index_to_cell_index_[dindx];
  app = app_data_[dindx];
  alpha = alpha_data_[dindx];
  target_app_data_[tindx] = app;
  target_alpha_data_[tindx] =alpha;
  return true;
}

void boxm2_vecf_middle_fat_pocket_scene::apply_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf,
                                                              std::vector<bool> const& valid){
  auto n = static_cast<unsigned>(box_cell_centers_.size());
  //clear target
  int valid_count = 0;
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
    // cells have vector field defined but not fat pocket
    if(!this->apply_vector_field(box_cell_centers_[j], vf[j])){
      target_alpha_data_[box_cell_centers_[j].data_index_] = 0.0f;
      continue;
    }
    valid_count++;
  }
  std::cout << "Apply middle_fat_pocket vector field to " << valid_count << " out of " << n << " cells in " << t.real()/1000.0 << " sec.\n";
}

int boxm2_vecf_middle_fat_pocket_scene::prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index){
  int max_level = blk_->max_level();
  if(!valid_unrefined_[pt_index])
    return -1;
  // map the target back to source
  vgl_point_3d<double> pt_in_source = sub_block_pt +  vfield_unrefined_[pt_index];
  // sub_block axis-aligned corners in source
  vgl_point_3d<double> sbc_min(pt_in_source.x()-0.5*targ_dims_.x(),
                               pt_in_source.y()-0.5*targ_dims_.y(),
                               pt_in_source.z()-0.5*targ_dims_.z());
  vgl_point_3d<double> sbc_max(pt_in_source.x()+0.5*targ_dims_.x(),
                               pt_in_source.y()+0.5*targ_dims_.y(),
                               pt_in_source.z()+0.5*targ_dims_.z());

  vgl_box_3d<double> target_box_in_source;
  target_box_in_source.add(sbc_min);
  target_box_in_source.add(sbc_max);

  // the source blocks intersecting the inverse mapped target box
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
void boxm2_vecf_middle_fat_pocket_scene::inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const& unrefined_target_pts){
  auto n = static_cast<unsigned>(unrefined_target_pts.size());
  vfield_unrefined_.resize(n, vgl_vector_3d<double>(0.0, 0.0, 0.0));
  valid_unrefined_.resize(n, false);
  for(unsigned vf_index = 0; vf_index<n; ++vf_index){
    const vgl_point_3d<double>& p = unrefined_target_pts[vf_index];
    vgl_vector_3d<double> invf;
    if(!middle_fat_pocket_geo_.inverse_vector_field(p, invf))
      continue;
    vgl_point_3d<double> p_in_source = p + invf;
    if(!source_bb_.contains(p_in_source))
      continue;
    valid_unrefined_[vf_index] = true;
    vfield_unrefined_[vf_index].set(p_in_source.x() - p.x(),
                                    p_in_source.y() - p.y(),
                                    p_in_source.z() - p.z());
  }
}

void boxm2_vecf_middle_fat_pocket_scene::map_to_target(boxm2_scene_sptr target_scene){
  vul_timer t;
  //middle_fat_pocket_geo_.apply_deformation_params();
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


bool boxm2_vecf_middle_fat_pocket_scene::set_params(boxm2_vecf_articulated_params const& params){
  try{
    auto const& params_ref = dynamic_cast<boxm2_vecf_middle_fat_pocket_params const &>(params);
    params_ = boxm2_vecf_middle_fat_pocket_params(params_ref);
    middle_fat_pocket_geo_.set_params(params_);
    return true;
  }catch(std::exception e){
    std::cout<<" Can't downcast middle_fat_pocket parameters! PARAMETER ASSIGNMENT PHAILED!"<<std::endl;
    return false;
  }
}


// find the middle_fat_pocket cell locations in the target volume
void boxm2_vecf_middle_fat_pocket_scene::determine_target_box_cell_centers(){
  if(!blk_){
    std::cout << "Null source block -- FATAL!\n";
    return;
  }
  vgl_box_3d<double> offset_box(source_bb_.centroid()  ,source_bb_.width(),source_bb_.height(),source_bb_.depth(),vgl_box_3d<double>::centre);
  if(target_blk_){
    box_cell_centers_ = target_blk_->cells_in_box(source_bb_);
  }else{
   std::cout << "Null target block -- FATAL!\n";
    return;
  }
}
//for debug purposes can be removed
void boxm2_vecf_middle_fat_pocket_scene::print_vf_centroid_scan(double off_coef) const{
  middle_fat_pocket_geo_.print_vf_centroid_scan(off_coef);
}
