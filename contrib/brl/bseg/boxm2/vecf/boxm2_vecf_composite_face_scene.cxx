#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "boxm2_vecf_composite_face_scene.h"
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_bounding_box.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <boxm2/cpp/algo/boxm2_refine_block_multi_data.h>
boxm2_vecf_composite_face_scene::boxm2_vecf_composite_face_scene(std::string const& face_scene_paths){
  //points defining the jaw coupled vector field bounding box in the source scene
  //the vector field from the mandible is propagated to all skin voxels within the box
  // unless invalid as defined by the articulated mouth region
  vgl_point_3d<double> p0(-82.0,-60.0, 0.0);  vgl_point_3d<double> p1(82.0,-60.0, 0.0);
  vgl_point_3d<double> p2(-56.0,-150.0,0.0);  vgl_point_3d<double> p3(56.0,-150.0,0.0);
  vgl_point_3d<double> p4(0,-60.,120.0);
  mandible_skin_coupling_box_.add(p0); mandible_skin_coupling_box_.add(p1); mandible_skin_coupling_box_.add(p2);
  mandible_skin_coupling_box_.add(p3); mandible_skin_coupling_box_.add(p4);
  // points defining the coupled field regions for the skin to the middle fat pocket
#if 1
  vgl_point_3d<double> fp0(58.9,-3.52, 52.65); vgl_point_3d<double> fp1(31.63,-39.0, 44.62);
  vgl_point_3d<double> fp2(69.68, 23.3, -7.1); vgl_point_3d<double> fp3(59.74, -24.98, -33.65);
  vgl_point_3d<double> fp4(41.34, -9.6, 87.54);vgl_point_3d<double> fp5(42.2, -56.64, 69.57);
  vgl_point_3d<double> fp6(86.76, 11.98, 25.09);vgl_point_3d<double> fp7(75.9, -51.5, 15.36);

  middle_fat_pocket_skin_coupling_box_.add(fp0);   middle_fat_pocket_skin_coupling_box_.add(fp1);
  middle_fat_pocket_skin_coupling_box_.add(fp2);   middle_fat_pocket_skin_coupling_box_.add(fp3);
  middle_fat_pocket_skin_coupling_box_.add(fp4);   middle_fat_pocket_skin_coupling_box_.add(fp5);
  middle_fat_pocket_skin_coupling_box_.add(fp6);   middle_fat_pocket_skin_coupling_box_.add(fp7);
#else
  vgl_point_3d<double> fp0(48.252353668213,-8.134105682373,84.791580200195);
  vgl_point_3d<double> fp1(50.227447509766,-19.818675994873,82.815269470215);
  vgl_point_3d<double> fp2(69.978309631348,-18.322017669678,68.044876098633);
  vgl_point_3d<double> fp3(71.953407287598,-2.248519897461,67.758796691895);
  vgl_point_3d<double> fp4(48.252353668213,-8.134105682373,80.791580200195);
  vgl_point_3d<double> fp5(50.227447509766,-19.818675994873,78.815269470215);
  vgl_point_3d<double> fp6(69.978309631348,-18.322017669678,65.044876098633);
  vgl_point_3d<double> fp7(71.953407287598,-2.248519897461,63.758796691895);
  middle_fat_pocket_skin_coupling_box_.add(fp0);   middle_fat_pocket_skin_coupling_box_.add(fp1);
  middle_fat_pocket_skin_coupling_box_.add(fp2);   middle_fat_pocket_skin_coupling_box_.add(fp3);
  middle_fat_pocket_skin_coupling_box_.add(fp4);   middle_fat_pocket_skin_coupling_box_.add(fp5);
  middle_fat_pocket_skin_coupling_box_.add(fp6);   middle_fat_pocket_skin_coupling_box_.add(fp7);
#endif
  std::ifstream istr(face_scene_paths.c_str());
  // construct paths to component scene xml files
  std::map<std::string, std::string> scene_path_map;
  std::string component, path;
  while(istr >> component >> path)
    scene_path_map[component] = path;

  std::string base_path, mandible_path, cranium_path, skin_path, mouth_path,fat_pocket_scene_path,fat_pocket_geo_path,param_path;

  std::map<std::string, std::string>::iterator pit;

  pit = scene_path_map.find("base_path");
  if(pit == scene_path_map.end()){
    std::cout << "FATAL - base_path not defined\n";
    return;
  }else
    base_path = pit->second;
  pit = scene_path_map.find("mandible_path");
  if(pit == scene_path_map.end()){
    std::cout << "FATAL - mandible_path not defined\n";
    return;
  }else
    mandible_path = base_path + pit->second;
  if(!vul_file::exists(mandible_path)){
    std::cout << "FATAL - " << mandible_path << " does not exist\n";
    return;
  }
  pit = scene_path_map.find("cranium_path");
  if(pit == scene_path_map.end()){
    std::cout << "FATAL - cranium_path not defined\n";
    return;
  }else
    cranium_path = base_path + pit->second;

  if(!vul_file::exists(cranium_path)){
    std::cout << "FATAL - " << cranium_path << " does not exist\n";
    return;
  }
  pit = scene_path_map.find("skin_path");
  if(pit == scene_path_map.end()){
    std::cout << "FATAL - skin_path not defined\n";
    return;
  }else
    skin_path = base_path + pit->second;

  if(!vul_file::exists(skin_path)){
    std::cout << "FATAL - " << skin_path << " does not exist\n";
    return;
  }

  pit = scene_path_map.find("mouth_path");
  if(pit == scene_path_map.end()){
    std::cout << "FATAL - mouth_path not defined\n";
    return;
  }else
    mouth_path = base_path + pit->second;

  if(!vul_file::exists(mouth_path)){
    std::cout << "FATAL - " << mouth_path << " does not exist\n";
    return;
  }

  pit = scene_path_map.find("fat_pocket_scene_path");
  if(pit == scene_path_map.end()){
    std::cout << "FATAL - fat_pocket_scene_path not defined\n";
    return;
  }else
    fat_pocket_scene_path = base_path + pit->second;

  if(!vul_file::exists(fat_pocket_scene_path)){
    std::cout << "FATAL - " << fat_pocket_scene_path << " does not exist\n";
    return;
  }

  pit = scene_path_map.find("fat_pocket_geo_path");
  if(pit == scene_path_map.end()){
    std::cout << "FATAL - fat_pocket_geo_path not defined\n";
    return;
  }else
    fat_pocket_geo_path = base_path + pit->second;

  if(!vul_file::exists(fat_pocket_geo_path)){
    std::cout << "FATAL - " << fat_pocket_geo_path << " does not exist\n";
    return;
  }

  pit = scene_path_map.find("param_path");
  if(pit != scene_path_map.end()){
    param_path = pit->second;
    if(!vul_file::exists(param_path)){
      std::cout << "FATAL - " << param_path << " does not exist\n";
      return;
    }
    std::ifstream pistr(param_path.c_str());
    pistr >> params_;
  }

  //load the scenes
  mandible_ = new boxm2_vecf_mandible_scene(mandible_path);
  cranium_ = new boxm2_vecf_cranium_scene(cranium_path);
  skin_ = new boxm2_vecf_skin_scene(skin_path);
  middle_fat_pocket_ = new boxm2_vecf_middle_fat_pocket_scene(fat_pocket_scene_path, fat_pocket_geo_path, false);
  vgl_pointset_3d<double> ptset;
  std::ifstream mstr(mouth_path.c_str());
  if(!mstr)
    return;
  mstr >> ptset;
  mstr.close();
  mouth_geo_ = boxm2_vecf_mouth(ptset);
}
bool  boxm2_vecf_composite_face_scene::inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf, std::string& anatomy_type) const{
  vgl_vector_3d<double> mandible_inv_vf, cranium_inv_vf, skin_inv_vf, middle_fat_pocket_inv_vf;
  bool mandible_valid=false, cranium_valid=false, skin_valid=false, middle_fat_pocket_valid = false, in_mouth = false;
  if(mandible_)
    mandible_valid = mandible_->inverse_vector_field(target_pt, mandible_inv_vf);
  if(cranium_)
    cranium_valid = cranium_->inverse_vector_field(target_pt, cranium_inv_vf);
  if(skin_){
    if(mandible_skin_coupling_box_.contains(target_pt)){
      skin_valid = mandible_->coupled_vector_field(target_pt, skin_inv_vf);
      in_mouth = mouth_geo_.in(target_pt);
    }else if(middle_fat_pocket_skin_coupling_box_.contains(target_pt)){
      skin_valid = middle_fat_pocket_->coupled_vector_field(target_pt, skin_inv_vf);
      if(!skin_valid)
        skin_valid = skin_->inverse_vector_field(target_pt, skin_inv_vf);
    }else
      skin_valid = skin_->inverse_vector_field(target_pt, skin_inv_vf);
  }
  if(middle_fat_pocket_){
    middle_fat_pocket_valid = middle_fat_pocket_->inverse_vector_field(target_pt, middle_fat_pocket_inv_vf);
    if(middle_fat_pocket_valid&&skin_valid)
      skin_valid = false;
  }
  bool not_valid = (!mandible_valid&&!cranium_valid&&!skin_valid&&!middle_fat_pocket_valid);
  if(not_valid){
    return false;
  }
  bool mouth_skin_invalid = skin_valid&&in_mouth;
  if(mouth_skin_invalid){
    anatomy_type = "mouth";
    inv_vf.set(0.0, 0.0, 0.0);
  }else if(mandible_valid){
    anatomy_type="mandible";
    inv_vf.set(mandible_inv_vf.x(), mandible_inv_vf.y(), mandible_inv_vf.z());
  }else if(skin_valid){
    anatomy_type="skin";
    inv_vf.set(skin_inv_vf.x(), skin_inv_vf.y(), skin_inv_vf.z());
  }else if(middle_fat_pocket_valid){
    anatomy_type="middle_fat_pocket";
    inv_vf.set(middle_fat_pocket_inv_vf.x(), middle_fat_pocket_inv_vf.y(), middle_fat_pocket_inv_vf.z());
  }else if(cranium_valid){
    anatomy_type="cranium";
    inv_vf.set(cranium_inv_vf.x(), cranium_inv_vf.y(), cranium_inv_vf.z());
  }
  return true;
}
//: compute the inverse vector field, first undoing the affine map to the target
void  boxm2_vecf_composite_face_scene::inverse_vector_field(std::vector<vgl_vector_3d<double> >& vfield, std::vector<std::string>& type) const{
  vul_timer t;

  //the target cell centers. the vector field could potentially be defined at all target points
  auto nt = static_cast<unsigned>(target_cell_centers_.size());
  vfield.resize(nt, vgl_vector_3d<double>(0.0, 0.0, 0.0));// initialized to 0
  type.resize(nt, "invalid");
  unsigned mandible_cnt = 0, skin_cnt = 0, cranium_cnt = 0;
  for(unsigned i = 0; i<nt; ++i){
    vgl_vector_3d<double> inv_vf;
    std::string anatomy_type;
    const vgl_point_3d<double>& p_inv = target_cell_centers_[i].cell_center_;
    bool valid = this->inverse_vector_field(p_inv, inv_vf, anatomy_type);
    if(!valid)
      continue;
    vfield[i].set(inv_vf.x(), inv_vf.y(), inv_vf.z());
    type[i]=anatomy_type;
  }
}
void boxm2_vecf_composite_face_scene::extract_unrefined_cell_info(){
  if(!target_blk_){
    std::cout << "FATAL! - NULL target block\n";
    return;
  }
  // inverse mapping for global affine face transformation to a specific subject
  const vgl_h_matrix_3d<double>& Ainv = params_.trans_.get_inverse();

  //iterate through the trees of the target. At this point they are unrefined
  unrefined_cell_info_.resize(targ_n_.x()*targ_n_.y()*targ_n_.z());
  for(unsigned ix = 0; ix<targ_n_.x(); ++ix){
    for(unsigned iy = 0; iy<targ_n_.y(); ++iy){
      for(unsigned iz = 0; iz<targ_n_.z(); ++iz){
        double x = targ_origin_.x() + ix*targ_dims_.x();
        double y = targ_origin_.y() + iy*targ_dims_.y();
        double z = targ_origin_.z() + iz*targ_dims_.z();
        vgl_point_3d<double> p(x, y, z);
        auto lindex = static_cast<unsigned>(target_linear_index(ix, iy, iz));
        unrefined_cell_info cinf;
        cinf.linear_index_ = lindex;
        cinf.ix_ = ix; cinf.iy_ = iy; cinf.iz_ = iz;
        cinf.pt_=Ainv*p;
        unrefined_cell_info_[lindex]=cinf;
      }
    }
   }
}
void boxm2_vecf_composite_face_scene::map_to_target(boxm2_scene_sptr target){
  vul_timer t;
  static bool first = true;
  if(first){
  if(!target_data_extracted_)
    this->extract_target_block_data(target);

  this->extract_unrefined_cell_info();//on articulated_scene
  std::vector<vgl_point_3d<double> > tgt_pts;
  for(auto & cit : unrefined_cell_info_){
    tgt_pts.push_back(cit.pt_);
  }

  // compute inverse vector field for prerefining the target
  if(mandible_){
   // mandible_->extract_target_block_data(target);
    mandible_->inverse_vector_field_unrefined(tgt_pts);
  }
  if(cranium_){
   // cranium_->extract_target_block_data(target);
   cranium_->inverse_vector_field_unrefined(tgt_pts);
  }
  if(middle_fat_pocket_){
   // cranium_->extract_target_block_data(target);
   middle_fat_pocket_->inverse_vector_field_unrefined(tgt_pts);
  }
  if(skin_){
    //skin_->extract_target_block_data(target);
    skin_->inverse_vector_field_unrefined(tgt_pts);
  }

  this->prerefine_target(target);
  this->extract_target_block_data(target);

  if(mandible_)
     mandible_->extract_target_block_data(target);
  if(cranium_)
    cranium_->extract_target_block_data(target);
  if(middle_fat_pocket_)
    middle_fat_pocket_->extract_target_block_data(target);
  if(skin_)
    skin_->extract_target_block_data(target);
  first = false;
  }
  this->extract_target_cell_centers();
  std::vector<vgl_vector_3d<double> > vfield;
  std::vector<std::string> type;
  this->inverse_vector_field(vfield,  type);
  this->apply_vector_field_to_target(vfield, type);
}

bool boxm2_vecf_composite_face_scene::set_params(boxm2_vecf_articulated_params const& params){
  try{
    auto const& params_ref = dynamic_cast<boxm2_vecf_composite_face_params const &>(params);
    params_ =boxm2_vecf_composite_face_params(params_ref);
  }catch(std::exception e){
    std::cout<<" Can't downcast to composite_face parameters! PARAMATER ASSIGNMENT PHAILED!"<<std::endl;
    return false;
  }
  if(skin_)
    skin_->set_params(params_.skin_params_);
  if(mandible_){
    mandible_->set_params( params_.mandible_params_);
    mouth_geo_.set_mandible_params(params_.mandible_params_);
    mouth_geo_.set_params(params_.mouth_params_);
    std::cout << "======> Set jaw angle " << params_.mandible_params_.jaw_opening_angle_rad_ << '\n';
  }
  if(middle_fat_pocket_)
    middle_fat_pocket_->set_params(params_.middle_fat_pocket_params_);
  return true;
}

void boxm2_vecf_composite_face_scene::inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const&  /*unrefined_target_pts*/){
}

int boxm2_vecf_composite_face_scene::prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index){
  int max_depth = -1;
  if(mandible_){
    int depth_mandible = mandible_->prerefine_target_sub_block(sub_block_pt, pt_index);
    if(depth_mandible>max_depth)
      max_depth = depth_mandible;
  }
  if(cranium_){
    int depth_cranium = cranium_->prerefine_target_sub_block(sub_block_pt, pt_index);
    if(depth_cranium>max_depth)
      max_depth = depth_cranium;
  }
  if(middle_fat_pocket_){
    int depth_fat_pocket = middle_fat_pocket_->prerefine_target_sub_block(sub_block_pt, pt_index);
    if(depth_fat_pocket>max_depth)
      max_depth = depth_fat_pocket;
  }
  if(skin_){
    int depth_skin = skin_->prerefine_target_sub_block(sub_block_pt, pt_index);
    if(depth_skin>max_depth)
      max_depth = depth_skin;
  }
  return max_depth;
}

void boxm2_vecf_composite_face_scene::compute_target_box( std::string const& pc_path){
  std::ifstream istr(pc_path.c_str());
  if(!istr){
    std::cout << "FATAL - can't open point cloud path " << pc_path << '\n';
    return;
  }
  vgl_pointset_3d<double> ptset;
  istr >> ptset;
  target_box_ = vgl_bounding_box(ptset);
}

boxm2_scene_sptr boxm2_vecf_composite_face_scene::
construct_target_scene(std::string const& scene_dir,std::string const& scene_name, std::string const& data_path,
                      double sub_block_len, bool save_scene_xml){
  if(target_box_.is_empty())
    return nullptr;
  std::vector<std::string> prefixes;
  prefixes.emplace_back("boxm2_mog3_grey");
  prefixes.emplace_back("boxm2_num_obs");
  boxm2_scene_sptr tscene = new boxm2_scene(scene_dir, scene_name, data_path, prefixes, target_box_, sub_block_len);
  if(save_scene_xml)
    tscene->save_scene();
  return tscene;
}

void boxm2_vecf_composite_face_scene::extract_target_cell_centers(){
  vgl_box_3d<double> target_bb = target_blk_->bounding_box_global();
  target_cell_centers_ = target_blk_->cells_in_box(target_bb);
  const vgl_h_matrix_3d<double>& Ainv = params_.trans_.get_inverse();
  for(auto & target_cell_center : target_cell_centers_){
    const vgl_point_3d<double>& c =     target_cell_center.cell_center_;
    target_cell_center.cell_center_ = Ainv*c;
  }
}


void boxm2_vecf_composite_face_scene::apply_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf, std::vector<std::string> const& type){
  boxm2_data_traits<BOXM2_ALPHA>::datatype alpha = 0.0f;
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
  unsigned n_valid = 0;
  auto n = static_cast<unsigned>(vf.size());
  bool show_mouth = params_.mouth_params_.show_mouth_region_;
  for(unsigned j = 0; j<n; ++j){
    const std::string& t = type[j];
    bool fail = true;
    if(t == "mouth"){
      app[0]= static_cast<unsigned char>(0.0);
      unsigned tindx = target_cell_centers_[j].data_index_;
      alpha = 0.0f;//default to no occlusion
      if(show_mouth)
        alpha = 10.0f;
      target_alpha_data_[tindx] = alpha;
      target_app_data_[tindx]=app;
      continue;
    }else if(t == "mandible"){
      fail = !mandible_->apply_vector_field(target_cell_centers_[j], vf[j]);
    }else if(t == "cranium"){
      fail = !cranium_->apply_vector_field(target_cell_centers_[j], vf[j]);
    }else if(t == "middle_fat_pocket"){
      fail = !middle_fat_pocket_->apply_vector_field(target_cell_centers_[j], vf[j]);
    }else if(t == "skin"){
      fail = !skin_->apply_vector_field(target_cell_centers_[j], vf[j]);
    }
    if(t == "invalid" || fail){
      unsigned tindx = target_cell_centers_[j].data_index_;
      alpha = 0.0f;//default to no occlusion
      target_alpha_data_[tindx] = alpha;
      continue;
    }else
      n_valid++;
  }
  std::cout << "mapped "  << n_valid << " source points out of " << n << " total points\n";
}
