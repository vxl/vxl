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
#include <vnl/vnl_math.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_closest_point.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <vnl/vnl_random.h>
#include <vcl_algorithm.h>
#include <vcl_limits.h>
#include <vcl_set.h>
#include <vul/vul_timer.h>

typedef boxm2_data_traits<BOXM2_PIXEL>::datatype pixtype;
typedef vnl_vector_fixed<unsigned char, 16> uchar16;
// fill the background alpha and intensity values to be slightly dark
// !!!!!this method only works for unrefined trees!!!
void boxm2_vecf_mandible_scene::fill_block(){
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
        mandible_data_[indx]  = static_cast<pixtype>(false);
#if 0
        left_ramus_->data()[indx]  = static_cast<pixtype>(false);
        left_angle_->data()[indx]  = static_cast<pixtype>(false);
        body_->data()[indx]        = static_cast<pixtype>(false);
        right_angle_->data()[indx] = static_cast<pixtype>(false);
        right_ramus_->data()[indx] = static_cast<pixtype>(false);
#endif
      }
    }
  }
}
// !!!!!this method only works for unrefined trees!!!
// currently unused, except for display purposes
void boxm2_vecf_mandible_scene::fill_target_block(){
  params_.app_[0]=static_cast<unsigned char>(10);
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  // replace datavalues
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
        target_alpha_data_[indx]=0.005f;//to see contrast against white
        target_app_data_[indx] = params_.app_;
        target_nobs_data_[indx] = nobs;
      }
    }
  }
}


void boxm2_vecf_mandible_scene::extract_block_data(){

  vcl_vector<boxm2_block_id> blocks = base_model_->get_block_ids();

  vcl_vector<boxm2_block_id>::iterator iter_blk = blocks.begin();
  blk_ = boxm2_cache::instance()->get_block(base_model_, *iter_blk);
  vcl_cout << "Extracting from block with " << blk_->num_cells() << " cells\n";
  sigma_ = static_cast<float>(blk_->sub_block_dim().x());

  alpha_base_  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_ALPHA>::prefix());
  alpha_base_->enable_write();
  alpha_data_= reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(alpha_base_->data_buffer());

  app_base_  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  app_base_->enable_write();
  app_data_= reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*>(app_base_->data_buffer());

  nobs_base_  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
  nobs_base_->enable_write();
  nobs_data_=reinterpret_cast<boxm2_data_traits<BOXM2_NUM_OBS>::datatype*>(nobs_base_->data_buffer());

  mandible_base_  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_PIXEL>::prefix("mandible"));
  mandible_base_->enable_write();
  mandible_data_ = reinterpret_cast<boxm2_data_traits<BOXM2_PIXEL>::datatype*>(mandible_base_->data_buffer());
  mandible_size_ = static_cast<int>(mandible_base_->buffer_length());
#if 0
  boxm2_data_base *  left_ramus_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_PIXEL>::prefix("left_ramus"));
  left_ramus_base->enable_write();
  left_ramus_=new boxm2_data<BOXM2_PIXEL>(left_ramus_base->data_buffer(),left_ramus_base->buffer_length(),left_ramus_base->block_id());

  boxm2_data_base *  left_angle_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_PIXEL>::prefix("left_angle"));
  left_angle_base->enable_write();
  left_angle_=new boxm2_data<BOXM2_PIXEL>(left_angle_base->data_buffer(),left_angle_base->buffer_length(),left_angle_base->block_id());

  boxm2_data_base *  body_base  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_PIXEL>::prefix("body"));
  body_base->enable_write();
  body_=new boxm2_data<BOXM2_PIXEL>(body_base->data_buffer(),body_base->buffer_length(),body_base->block_id());

  boxm2_data_base *  right_angle_base = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_PIXEL>::prefix("right_angle"));
  right_angle_base->enable_write();
  right_angle_=new boxm2_data<BOXM2_PIXEL>(right_angle_base->data_buffer(),right_angle_base->buffer_length(),right_angle_base->block_id());

  boxm2_data_base *  right_ramus_base = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_PIXEL>::prefix("right_ramus"));
  right_ramus_base->enable_write();
  right_ramus_=new boxm2_data<BOXM2_PIXEL>(right_ramus_base->data_buffer(),right_ramus_base->buffer_length(),right_ramus_base->block_id());
#endif
}
void boxm2_vecf_mandible_scene::extract_target_block_data(boxm2_scene_sptr target_scene){

  vcl_vector<boxm2_block_id> blocks = target_scene->get_block_ids();
  vcl_vector<boxm2_block_id>::iterator iter_blk = blocks.begin();
  target_blk_ = boxm2_cache::instance()->get_block(target_scene, *iter_blk);

  target_alpha_base_  = boxm2_cache::instance()->get_data_base(target_scene,*iter_blk,boxm2_data_traits<BOXM2_ALPHA>::prefix());
  target_alpha_base_->enable_write();
  target_alpha_data_= reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(target_alpha_base_->data_buffer());

  target_app_base_  = boxm2_cache::instance()->get_data_base(target_scene,*iter_blk,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  target_app_base_->enable_write();
  target_app_data_=reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*>(target_app_base_->data_buffer());

  target_nobs_base_  = boxm2_cache::instance()->get_data_base(target_scene,*iter_blk,boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
  target_nobs_base_->enable_write();
  target_nobs_data_= reinterpret_cast<boxm2_data_traits<BOXM2_NUM_OBS>::datatype*>(target_nobs_base_->data_buffer());
  // caution fill target block only works for unrefined target scenes
  // should not be used after refinement!!!!
  if(has_background_){
    vcl_cout<< " Darkening background "<<vcl_endl;
    this->fill_target_block();
  }
}

// after loading the block initialize all the cell indices from the block labels, e.g., cell == LEFT_RAMUS, cell == LEFT_ANGLE, etc.
void boxm2_vecf_mandible_scene::cache_cell_centers_from_anatomy_labels(){
  vgl_box_3d<double> source_box = blk_->bounding_box_global();
  vcl_vector<cell_info> source_cell_centers = blk_->cells_in_box(source_box);
  for(vcl_vector<cell_info>::iterator cit = source_cell_centers.begin();
      cit != source_cell_centers.end(); ++cit){
    unsigned dindx = cit->data_index_;
    bool mandible = mandible_data_[dindx]   > pixtype(0);
    if(mandible){
      unsigned mandible_index  = static_cast<unsigned>(mandible_cell_centers_.size());
      mandible_cell_centers_.push_back(cit->cell_center_);
      mandible_cell_data_index_.push_back(dindx);
      data_index_to_cell_index_[dindx] = mandible_index;
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
// main constructor
boxm2_vecf_mandible_scene::boxm2_vecf_mandible_scene(vcl_string const& scene_file, vcl_string const& geometry_file):
  boxm2_vecf_articulated_scene(scene_file),alpha_base_(VXL_NULLPTR), app_base_(VXL_NULLPTR), nobs_base_(VXL_NULLPTR), mandible_base_(VXL_NULLPTR), target_alpha_base_(VXL_NULLPTR), target_app_base_(VXL_NULLPTR),
  target_nobs_base_(VXL_NULLPTR),left_ramus_(VXL_NULLPTR), left_angle_(VXL_NULLPTR), body_(VXL_NULLPTR), right_angle_(VXL_NULLPTR), right_ramus_(VXL_NULLPTR), intrinsic_change_(false)
{
  mandible_geo_ = boxm2_vecf_mandible(geometry_file);
  this->extrinsic_only_ = true;
  target_blk_ = VXL_NULLPTR;
  target_data_extracted_ = false;
  boxm2_lru_cache::create(base_model_);
  this->extract_block_data();
  this->has_background_ = false;
  this->build_mandible();
  this->paint_mandible();
  vcl_vector<vcl_string> prefixes;
  prefixes.push_back("alpha");
  prefixes.push_back("boxm2_mog3_grey");
  prefixes.push_back("boxm2_num_obs");
  prefixes.push_back("boxm2_pixel_mandible");
  boxm2_surface_distance_refine<boxm2_vecf_mandible>(mandible_geo_, base_model_, prefixes);
  boxm2_surface_distance_refine<boxm2_vecf_mandible>(mandible_geo_, base_model_, prefixes);
  //  boxm2_surface_distance_refine<boxm2_vecf_mandible>(mandible_geo_, base_model_, prefixes);
  this->rebuild();
 }

boxm2_vecf_mandible_scene::boxm2_vecf_mandible_scene(vcl_string const& scene_file, vcl_string const& geometry_file, vcl_string const& params_file_name):
  boxm2_vecf_articulated_scene(scene_file),alpha_base_(VXL_NULLPTR), app_base_(VXL_NULLPTR), nobs_base_(VXL_NULLPTR),target_alpha_base_(VXL_NULLPTR), target_app_base_(VXL_NULLPTR), target_nobs_base_(VXL_NULLPTR),
  left_ramus_(VXL_NULLPTR), left_angle_(VXL_NULLPTR), body_(VXL_NULLPTR), right_angle_(VXL_NULLPTR), right_ramus_(VXL_NULLPTR){

  vcl_ifstream params_file(params_file_name.c_str());
  if (!params_file){
    vcl_cout<<" could not open params file construction fails!: "<<params_file_name<<vcl_endl;
    return;
  }
  params_file >> this->params_;
  this->extrinsic_only_ = true;
  target_blk_ = VXL_NULLPTR;
  target_data_extracted_ = false;
  boxm2_lru_cache::create(base_model_);
  this->extract_block_data();
  this->has_background_ = false;
  this->build_mandible();
  this->paint_mandible();
  vcl_vector<vcl_string> prefixes;
  prefixes.push_back("alpha");
  prefixes.push_back("boxm2_mog3_grey");
  prefixes.push_back("boxm2_num_obs");
  prefixes.push_back("boxm2_pixel_mandible");
  boxm2_surface_distance_refine<boxm2_vecf_mandible>(mandible_geo_, base_model_, prefixes);
  boxm2_surface_distance_refine<boxm2_vecf_mandible>(mandible_geo_, base_model_, prefixes);
  boxm2_surface_distance_refine<boxm2_vecf_mandible>(mandible_geo_, base_model_, prefixes);
  this->rebuild();
}
void boxm2_vecf_mandible_scene::rebuild(){
#if 0
  if(this->extrinsic_only_){
    vcl_cout<<" warning! rebuild called but scene accepts only extrinsic articulations!"<<vcl_endl;
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
  double len = vcl_sqrt(3.0)*blk_->sub_block_dim().x();//was 3xblk_->...
  double d_thresh = len;//sqrt(3)/2 x len, diagonal distance
  vgl_box_3d<double> bb = mandible_geo_.bounding_box();
   // cell in a box centers are in global coordinates
  vcl_vector<cell_info> ccs = blk_->cells_in_box(bb);
  for(vcl_vector<cell_info>::iterator cit = ccs.begin();
      cit != ccs.end(); ++cit){
    const vgl_point_3d<double>& cell_center = cit->cell_center_;
    unsigned indx = cit->data_index_;
    double d = mandible_geo_(cell_center);
    if(d < d_thresh){
      if(!is_type_global(cell_center, MANDIBLE)){
        mandible_cell_centers_.push_back(cell_center);
        mandible_cell_data_index_.push_back(indx);
        data_index_to_cell_index_[indx]=static_cast<unsigned>(mandible_cell_centers_.size())-1;
        //float blending_factor = static_cast<float>(gauss(d,sigma_));
        alpha_data_[indx]= - vcl_log(1.0f - ( 0.99f ))/ static_cast<float>(this->subblock_len());
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
  double distance = params_.neighbor_radius()*subblock_len();
  for(unsigned i = 0; i<mandible_cell_centers_.size(); i++){
      vgl_point_3d<double>& p = mandible_cell_centers_[i];
      unsigned indx_i = mandible_cell_data_index_[i];
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
  vcl_cout << "Find mandible cell neighborhoods in " << static_cast<double>(t.real())/1000.0 << " sec.\n";
}

 void boxm2_vecf_mandible_scene::recreate_mandible(){
   this->rebuild();
 }

void boxm2_vecf_mandible_scene::paint_mandible(){
  params_.app_[0]=params_.mandible_intensity_;
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  unsigned ns = static_cast<unsigned>(mandible_cell_centers_.size());
  for(unsigned i = 0; i<ns; ++i){
    unsigned indx = mandible_cell_data_index_[i];
    app_data_[indx] = params_.app_;
    nobs_data_[indx] = nobs;
  }
}

 bool boxm2_vecf_mandible_scene::is_type_data_index(unsigned data_index, boxm2_vecf_mandible_scene::anat_type type) const{
   if(type == MANDIBLE){
     unsigned char mandible = static_cast<unsigned char>(mandible_data_[data_index]);
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
    //    vcl_cout<<"point "<<global_pt<< " was out of eye scene bounding box "<<vcl_endl;
    //#endif
    return false;
}
  return this->is_type_data_index(indx, type);
}
// found depth is for debug purposes
  bool boxm2_vecf_mandible_scene::find_nearest_data_index(boxm2_vecf_mandible_scene::anat_type type, vgl_point_3d<double> const& probe, double cell_len, unsigned& data_indx, int& found_depth) const{
   //form a box around the probe with a radius of 1/2 the cell diagonal
    double r = 0.5*cell_len*params_.neighbor_radius();
   vgl_point_3d<double> pmin(probe.x()-r, probe.y()-r, probe.z()-r);
   vgl_point_3d<double> pmax(probe.x()+r, probe.y()+r, probe.z()+r);
   vgl_box_3d<double> probe_box;
   probe_box.add(pmin);  probe_box.add(pmax);
   vcl_vector<cell_info> ccs = blk_->cells_in_box(probe_box);
   unsigned dindx = 0;
   int depth_min = 0;
   double dmin = vcl_numeric_limits<double>::max();
   unsigned data_index_min = 0;
   for(vcl_vector<cell_info>::iterator cit = ccs.begin();
       cit != ccs.end(); ++cit){
     dindx = cit->data_index_;
     if(!is_type_data_index(dindx, type))
       continue;
     int depth = cit->depth_;
     double d = vgl_distance(probe, cit->cell_center_);
     if(d<dmin){
       dmin = d;
       data_index_min =dindx;
       depth_min = depth;
     }
   }
   if(dmin>r)
     return false;
   data_indx = data_index_min;
   found_depth = depth_min;
   return true;
}
void  boxm2_vecf_mandible_scene::inverse_vector_field(vgl_rotation_3d<double> const& rot, vcl_vector<vgl_vector_3d<double> >& vf,
                                                      vcl_vector<bool>& valid) const{

  vul_timer t;
  //really has to be the whole scene to take into account max rotation
  vgl_box_3d<double> bb = blk_->bounding_box_global();

  //the target cell centers. the vector field could potentially be defined at all target points
  unsigned nt = static_cast<unsigned>(box_cell_centers_.size());
  vf.resize(nt);// initialized to 0
  valid.resize(nt, false);

  vgl_rotation_3d<double> inv_rot = rot.inverse();
  unsigned box_cnt = 0;
  for(unsigned i = 0; i<nt; ++i){
    vgl_point_3d<double> p = (box_cell_centers_[i].cell_center_)-params_.offset_;
    vgl_point_3d<double> rp = inv_rot * p;// rotated point
    if(!bb.contains(rp))
      continue;
    //rp is inside source bounding box so define vector field
    box_cnt++;
    valid[i]=true;
    vf[i].set(rp.x() - p.x(), rp.y() - p.y(), rp.z() - p.z());
  }
  vcl_cout << "computed " << box_cnt << " pts "<< nt << " for mandible vector field in " << t.real()/1000.0 << " sec.\n";
}

////////// this stuff will be used later =====>
void boxm2_vecf_mandible_scene::create_left_ramus(){
   this->build_left_ramus();
   this->paint_left_ramus();
}

void boxm2_vecf_mandible_scene::recreate_left_ramus(){
  this->create_left_ramus();
  this->find_left_ramus_cell_neigborhoods();
}

void boxm2_vecf_mandible_scene::find_left_ramus_cell_neigborhoods(){
  vul_timer t;
  double distance = params_.neighbor_radius()*subblock_len();
  for(unsigned i = 0; i<left_ramus_cell_centers_.size(); i++){
      vgl_point_3d<double>& p = left_ramus_cell_centers_[i];
      unsigned indx_i = left_ramus_cell_data_index_[i];
      vcl_vector<vgl_point_3d<double> > nbrs = blk_->sub_block_neighbors(p, distance);
      for(unsigned j =0; j<nbrs.size(); ++j){
        vgl_point_3d<double>& q = nbrs[j];
        unsigned indx_n;
        if(!blk_->data_index(q, indx_n))
          continue;
        vcl_map<unsigned, unsigned >::iterator iit= left_ramus_data_index_to_cell_index_.find(indx_n);
        if(iit == left_ramus_data_index_to_cell_index_.end())
          continue;
        if(iit->second==i)
                continue;
        left_ramus_cell_neighbor_cell_index_[i].push_back(iit->second);
        vcl_vector<unsigned>& indices = left_ramus_cell_neighbor_data_index_[indx_i];
        indices.push_back(indx_n);
      }
  }
  vcl_cout << "Find left_ramus cell neighborhoods in " << static_cast<double>(t.real())/1000.0 << " sec.\n";
}

void boxm2_vecf_mandible_scene::build_left_ramus(){
  double len = 4 * blk_->sub_block_dim().x();
  double d_thresh = params_.neighbor_radius()*len;
  double margin = 1.0;//For now
  vgl_box_3d<double> bb;// = mandible_geo_.left_ramus_bounding_box(margin);
  // cells in  box centers are in global coordinates
  vcl_vector<cell_info> ccs = blk_->cells_in_box(bb);
  for(vcl_vector<cell_info>::iterator cit = ccs.begin();
      cit != ccs.end(); ++cit){
    const vgl_point_3d<double>& cell_center = cit->cell_center_;
    unsigned indx = cit->data_index_;
    double d =0.0;// mandible_geo_.left_ramus_surface_distance(cell_center);

    if(d < d_thresh){
      left_ramus_cell_centers_.push_back(cell_center);
      left_ramus_cell_data_index_.push_back(indx);
      left_ramus_->data()[indx] = static_cast<pixtype>(true);
      left_ramus_data_index_to_cell_index_[indx]=static_cast<unsigned>(left_ramus_cell_centers_.size())-1;
      float blending_factor = static_cast<float>(gauss(d,sigma_));
      alpha_data_[indx]= - vcl_log(1.0f - ( 0.95f ))/ static_cast<float>(this->subblock_len()) * blending_factor;
    }
  }
}

void boxm2_vecf_mandible_scene::paint_left_ramus(){
  params_.app_[0]=params_.left_ramus_intensity_;
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  unsigned ns = static_cast<unsigned>(left_ramus_cell_centers_.size());
  for(unsigned i = 0; i<ns; ++i){
    unsigned indx = left_ramus_cell_data_index_[i];
    app_data_[indx] = params_.app_;
    nobs_data_[indx] = nobs;
  }
}
///  <========  End of stuff to be used later===========
//

// interpolate data around the inverted position of the target in the source reference frame. Interpolation weights are based
// on a Gaussian distribution with respect to distance from the source location
//
void boxm2_vecf_mandible_scene::interpolate_vector_field(vgl_point_3d<double> const& src, unsigned sindx, unsigned dindx, unsigned tindx,
                                                      vcl_vector<vgl_point_3d<double> > & cell_centers,
                                                      vcl_map<unsigned, vcl_vector<unsigned> >& cell_neighbor_cell_index,
                                                      vcl_map<unsigned, vcl_vector<unsigned> >&cell_neighbor_data_index){

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
  const vcl_vector<unsigned>& nbr_cells = cell_neighbor_cell_index[sindx];
  const vcl_vector<unsigned>& nbr_data = cell_neighbor_data_index[dindx];
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
  boxm2_data_traits<BOXM2_ALPHA>::datatype alpha = static_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype>(sumalpha);
  target_app_data_[tindx] = app;
  target_alpha_data_[tindx] = alpha;
}

void boxm2_vecf_mandible_scene::apply_vector_field_to_target(vcl_vector<vgl_vector_3d<double> > const& vf,
                                                              vcl_vector<bool> const& valid){
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
  boxm2_data_traits<BOXM2_ALPHA>::datatype alpha = 0.0f;
  unsigned n = static_cast<unsigned>(box_cell_centers_.size());
  int valid_count = 0;
  if(n==0)
    return;//shouldn't happen
  vul_timer t;
  // iterate over the target cells and interpolate info from source
  for(unsigned j = 0; j<n; ++j){

    // if vector field is not defined then skip the target cell
    if(!valid[j]){
      continue;
    }
    //target cell center translated back to source box, and should be a mandible cell
    vgl_point_3d<double> trg_cent_in_source = box_cell_centers_[j].cell_center_-params_.offset_;
    double side_len = box_cell_centers_[j].side_length_;
    //int depth = box_cell_centers_[j].depth_;//for debug purposes
    unsigned tindx = box_cell_centers_[j].data_index_;
    vgl_point_3d<double> src = trg_cent_in_source + vf[j];//add inverse vector field

    // find closest mandible voxel cell in source. if not found then set target data to default values
    unsigned sindx, dindx;
    int found_depth;//for debug purposes
    if(!this->find_nearest_data_index(MANDIBLE, src, side_len, dindx, found_depth)){
      app[0]=(unsigned char)(0);//default to black
      alpha = 0.0f;//default to no occlusion
      target_app_data_[tindx] = app;
      target_alpha_data_[tindx] = alpha;
      continue;
    }

    // the nearest source data is defined, so use it
    app = app_data_[dindx];
    alpha = alpha_data_[dindx];
    target_app_data_[tindx] = app;
    target_alpha_data_[tindx] = alpha;
    valid_count++;

#if 0 // for now just use nearest neighbor
    this->interpolate_vector_field(src, sindx, dindx, tindx,
                                   mandible_cell_centers_, cell_neighbor_cell_index_,
                                   cell_neighbor_data_index_);
#endif
  }
  vcl_cout << "Apply mandible vector field to " << valid_count << " out of " << n << " cells in " << t.real()/1000.0 << " sec.\n";
}
void boxm2_vecf_mandible_scene::prerefine_target(boxm2_scene_sptr target_scene, vgl_rotation_3d<double> const& rot){
  if(!target_blk_){
    vcl_cout << "FATAL! - NULL target block\n";
    return;
  }
  vul_timer t;
  int count0 = 0, count1 = 0, count2 = 0, count3 = 0;
  vgl_rotation_3d<double> inv_rot = rot.inverse();
  vgl_box_3d<double> bb = blk_->bounding_box_global();
  int max_level = blk_->max_level();
  int deepest_cell_depth = 0;

  // don't copy the trees for efficiency
  const boxm2_array_3d<uchar16>& trees = blk_->trees();

  vgl_vector_3d<unsigned> n = target_blk_->sub_block_num();
  // the array of depths found in the source intersecting the inversely transformed sub_block (tree) bounding box.
  vbl_array_3d<int> depths_to_match(n.x(), n.y(), n.z());

  //iterate through the trees of the target. At this point they are unrefined
  vgl_point_3d<double> origin = target_blk_->local_origin();
  vgl_vector_3d<double> dims = target_blk_->sub_block_dim();
  double x=0.0, y=0.0, z=0.0;
  for(unsigned ix = 0; ix<n.x(); ++ix){
    x = origin.x()+ix*dims.x();
    for(unsigned iy = 0; iy<n.y(); ++iy){
      y = origin.y()+iy*dims.y();
      for(unsigned iz = 0; iz<n.z(); ++iz){
        z = origin.z()+iz*dims.z();

        // the center of the sub_block (tree) at (ix, iy, iz)
        vgl_point_3d<double> sub_block_center(x+0.5, y+0.5, z+0.5);

        // map the origin back to source by the offset
        vgl_point_3d<double> center_in_source = sub_block_center-params_.offset_;
        // rotate the target center back to source
        vgl_point_3d<double> rot_center_in_source = inv_rot*center_in_source;
        // sub_block axis-aligned corners in source
        vgl_point_3d<double> sbc_min(rot_center_in_source.x()-0.5*dims.x(),
                                     rot_center_in_source.y()-0.5*dims.y(),
                                     rot_center_in_source.z()-0.5*dims.z());

        vgl_point_3d<double> sbc_max(rot_center_in_source.x()+0.5*dims.x(),
                                     rot_center_in_source.y()+0.5*dims.y(),
                                     rot_center_in_source.z()+0.5*dims.z());

        vgl_box_3d<double> target_box_in_source;
        target_box_in_source.add(sbc_min);
        target_box_in_source.add(sbc_max);

        // rotate the target box in source by the inverse rotation
        // the box is rotated about its centroid
        vgl_orient_box_3d<double> target_obox(target_box_in_source, inv_rot.as_quaternion());
        vgl_box_3d<double> rot_target_box_in_source = target_obox.enclosing_box();

        // the source blocks intersecting the rotated target box
        vcl_vector<vgl_point_3d<int> > int_sblks = blk_->sub_blocks_intersect_box(rot_target_box_in_source);

        // iterate through each intersecting source tree and find the maximum tree depth
        int max_depth = 0;
        for(vcl_vector<vgl_point_3d<int> >::iterator bit = int_sblks.begin();
            bit != int_sblks.end(); ++bit){
          const uchar16& tree_bits = trees(bit->x(), bit->y(), bit->z());
          //safely cast since bit_tree is just temporary
          uchar16& uctree_bits = const_cast<uchar16&>(tree_bits);
          boct_bit_tree bit_tree(uctree_bits.data_block(), max_level);
          int dpth = bit_tree.depth();
          if(dpth>max_depth){
            max_depth = dpth;
          }
        }
        depths_to_match[ix][iy][iz]=max_depth;
                if(max_depth == 0)      count0++;
                else if(max_depth == 1) count1++;
                else if(max_depth == 2) count2++;
                else if(max_depth == 3) count3++;

        //record the deepest tree found
        if(max_depth>deepest_cell_depth){
          deepest_cell_depth = max_depth;
        }
      }
    }
  }
  vcl_cout << "deepest cell depth in prerefine_target " << deepest_cell_depth << '\n';
  vcl_cout << "count0 " << count0 << " count1 " << count1 << " count2 " << count2 << " count3 " << count3 << '\n';

  //fully refine the target trees to the required depth
  vcl_vector<vcl_string> prefixes;
  prefixes.push_back("alpha");  prefixes.push_back("boxm2_mog3_grey"); prefixes.push_back("boxm2_num_obs");
  boxm2_refine_block_multi_data_function(target_scene, target_blk_, prefixes, depths_to_match);
  vcl_cout << "prefine in " << t.real() << " msec\n";
 }

void boxm2_vecf_mandible_scene::map_to_target(boxm2_scene_sptr target_scene){
  vul_timer t;
  // initially extract unrefined target data
  if(!target_data_extracted_)
    this->extract_target_block_data(target_scene);
  // set rotation from params
  vnl_vector_fixed<double,3> X(1.0, 0.0, 0.0);
  vnl_quaternion<double> Q(X,params_.jaw_opening_angle_rad_);
  vgl_rotation_3d<double> rot(Q);
  // refine the target to match the source tree refinement
   this->prerefine_target(target_scene, rot);
  // have to extract target data again to refresh data bases and buffers after refinement
  this->extract_target_block_data(target_scene);
  this->determine_target_box_cell_centers();

  vcl_vector<vgl_vector_3d<double> > invf;
  vcl_vector<bool> valid;
  this->inverse_vector_field(rot, invf, valid);
  this->apply_vector_field_to_target(invf, valid);
  target_data_extracted_  = true;
  vcl_cout << "Map to target in " << t.real()/1000.0 << " secs\n";
}


bool boxm2_vecf_mandible_scene::set_params(boxm2_vecf_articulated_params const& params){
  try{
    boxm2_vecf_mandible_params const& params_ref = dynamic_cast<boxm2_vecf_mandible_params const &>(params);
    intrinsic_change_ = this->vfield_params_change_check(params_ref);
    params_ = boxm2_vecf_mandible_params(params_ref);
#if _DEBUG
    vcl_cout<< "intrinsic change? "<<intrinsic_change_<<vcl_endl;
#endif
    if(intrinsic_change_){
      this->rebuild();
    }
    return true;
  }catch(std::exception e){
    vcl_cout<<" Can't downcast orbit parameters! PARAMETER ASSIGNMENT PHAILED!"<<vcl_endl;
    return false;
  }
}

bool boxm2_vecf_mandible_scene::vfield_params_change_check(const boxm2_vecf_mandible_params & params){
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
void boxm2_vecf_mandible_scene::reset_buffers(){
  vcl_vector<boxm2_block_id> blocks = base_model_->get_block_ids();
  boxm2_block_metadata mdata = base_model_->get_block_metadata_const(blocks[0]);
  #if 0
  app_data_      ->set_default_value(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(), mdata);
  alpha_data_    ->set_default_value(boxm2_data_traits<BOXM2_ALPHA>::prefix(), mdata);
  mandible_data_ ->set_default_value(boxm2_data_traits<BOXM2_PIXEL>::prefix(), mdata);

  left_ramus_    ->set_default_value(boxm2_data_traits<BOXM2_PIXEL>::prefix(), mdata);
  left_angle_    ->set_default_value(boxm2_data_traits<BOXM2_PIXEL>::prefix(), mdata);
  body_          ->set_default_value(boxm2_data_traits<BOXM2_PIXEL>::prefix(), mdata);
  right_angle_   ->set_default_value(boxm2_data_traits<BOXM2_PIXEL>::prefix(), mdata);
  right_ramus_   ->set_default_value(boxm2_data_traits<BOXM2_PIXEL>::prefix(), mdata);
#endif
  mandible_cell_centers_.clear();
  mandible_cell_data_index_.clear();

  cell_neighbor_cell_index_.clear();
  data_index_to_cell_index_.clear();
  cell_neighbor_data_index_.clear();
#if 0
  left_ramus_cell_centers_.clear();
  left_ramus_cell_data_index_.clear();
  left_ramus_cell_neighbor_cell_index_.clear();
  left_ramus_data_index_to_cell_index_.clear();
  left_ramus_cell_neighbor_data_index_.clear();

  left_angle_cell_centers_.clear();
  left_angle_cell_data_index_.clear();
  left_angle_cell_neighbor_cell_index_.clear();
  left_angle_data_index_to_cell_index_.clear();
  left_angle_cell_neighbor_data_index_.clear();

  body_cell_centers_.clear();
  body_cell_data_index_.clear();
  body_cell_neighbor_cell_index_.clear();
  body_data_index_to_cell_index_.clear();
  body_cell_neighbor_data_index_.clear();

  right_angle_cell_centers_.clear();
  right_angle_cell_data_index_.clear();
  right_angle_cell_neighbor_cell_index_.clear();
  right_angle_data_index_to_cell_index_.clear();
  right_angle_cell_neighbor_data_index_.clear();

  right_ramus_cell_centers_.clear();
  right_ramus_cell_data_index_.clear();
  right_ramus_cell_neighbor_cell_index_.clear();
  right_ramus_data_index_to_cell_index_.clear();
  right_ramus_cell_neighbor_data_index_.clear();
#endif
}
#if 0 //to do get reasonable bounds on the target cells to process
// find the source cell locations in the target volume
void boxm2_vecf_mandible_scene::determine_target_box_cell_centers(){
  vgl_box_3d<double> source_box = blk_->bounding_box_global();
  vnl_vector_fixed<double,3> X(1.0, 0.0, 0.0);
  vnl_quaternion<double> Q(X,params_.max_jaw_opening_angle_rad_);
  vgl_orient_box_3d<double> obox(source_box, Q);
  vgl_box_3d<double> rot_source_box = obox.enclosing_box();
  vcl_cout << "source box " << source_box << '\n' << "rotated source box " << rot_source_box << '\n';
  //  vgl_box_3d<double> offset_box(obox.centroid() + params_.offset_ ,obox.width(),obox.height(),obox.depth(),vgl_box_3d<double>::centre);
  if(target_blk_){
    box_cell_centers_ = target_blk_->cells_in_box(rot_source_box);
  }
}
#endif
#if 1
void boxm2_vecf_mandible_scene::determine_target_box_cell_centers(){
  vgl_box_3d<double> source_box = blk_->bounding_box_global();
  vgl_box_3d<double> offset_box(source_box.centroid() + params_.offset_ ,source_box.width(),source_box.height(),source_box.depth(),vgl_box_3d<double>::centre);
  if(target_blk_){
    box_cell_centers_ = target_blk_->cells_in_box(offset_box);
  }
}
#endif
