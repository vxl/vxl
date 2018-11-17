#include <iostream>
#include <algorithm>
#include <limits>
#include <set>
#include "boxm2_vecf_ocl_orbit_scene.h"
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
#include <vcl_where_root_dir.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/vecf/boxm2_vecf_articulated_scene.h>
static double gauss(double d, double sigma){
  return std::exp((-0.5*d*d)/(sigma*sigma));
}
bool boxm2_vecf_ocl_orbit_scene::compile_kernels()
{
  std::string options;
  // sets apptypesize_ and app_type
  this->get_scene_appearance( options);
  std::cout<<" compiling trans kernel with options "<<options<< std::endl;
  std::vector<std::string> src_paths;
  std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  std::string vecf_source_dir = std::string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/vecf/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir     + "boxm2_ocl_helpers.cl");
  src_paths.push_back(vecf_source_dir + "interp_helpers.cl");
  src_paths.push_back(vecf_source_dir + "orbit_funcs.cl");
  src_paths.push_back(vecf_source_dir + "eyelid_funcs.cl");
  src_paths.push_back(vecf_source_dir + "map_orbit_to_target.cl");

  if (is_right_)
    options += " -D RIGHT_EYE ";
#ifdef DEBUG_CL
    options += " -D DEBUG_CL ";
#endif
  std::cout<<"compiling map to orbit kernel with "<<options<<std::endl;
  auto* map_sphere_kernel = new bocl_kernel();
  map_sphere_kernel->create_kernel(&device_->context(),device_->device_id(), src_paths, "map_orbit_to_target", options, "map_orbit_to_target kernel");
  kernels.push_back(map_sphere_kernel);
  return true;
}

typedef boxm2_data_traits<BOXM2_PIXEL>::datatype pixtype;
// fill the background alpha and intensity values to be slightly dark
void boxm2_vecf_ocl_orbit_scene::fill_block(){
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
        iris_->data()[indx] = static_cast<pixtype>(false);
        pupil_->data()[indx] = static_cast<pixtype>(false);
        eyelid_->data()[indx]= static_cast<pixtype>(false);
        lower_eyelid_->data()[indx]= static_cast<pixtype>(false);
        eyelid_crease_->data()[indx]= static_cast<pixtype>(false);
      }
    }
  }
}
// currently unused, except for display purposes
void boxm2_vecf_ocl_orbit_scene::fill_target_block(){
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
  target_alpha_base_->write_to_buffer(queue_);
  target_app_base_->write_to_buffer(queue_);
}


void boxm2_vecf_ocl_orbit_scene::extract_block_data(){

  std::vector<boxm2_block_id> blocks = base_model_->get_block_ids();

  auto iter_blk = blocks.begin();
  blk_ = boxm2_cache::instance()->get_block(base_model_, *iter_blk);
  sigma_ = blk_->sub_block_dim().x();
  int status;

  //  target_pts_ = opencl_cache_->get_data(base_model_,*iter_blk,boxm2_data_traits<BOXM2_POINT>::prefix("source"));

  this->update_source_gpu_buffers() ;

  alpha_data_            = new boxm2_data<BOXM2_ALPHA>((char*)source_alpha_base_->cpu_buffer(),source_alpha_base_->num_bytes(),*iter_blk);
  app_data_              = new boxm2_data<BOXM2_MOG3_GREY>((char*)source_app_base_->cpu_buffer(),source_app_base_->num_bytes(),*iter_blk);
  color_app_data_        = new boxm2_data<BOXM2_GAUSS_RGB>((char*)source_color_base_->cpu_buffer(),source_color_base_->num_bytes(),*iter_blk);
  nobs_data_             = new boxm2_data<BOXM2_NUM_OBS>((char*)source_nobs_base_->cpu_buffer(),source_nobs_base_->num_bytes(),*iter_blk);
  source_vis_score_data_ = new boxm2_data<BOXM2_VIS_SCORE>((char*)source_vis_base_->cpu_buffer(),source_vis_base_->num_bytes(),*iter_blk);

  sphere_        = new boxm2_data<BOXM2_PIXEL>((char*)sphere_base_->cpu_buffer(),sphere_base_->num_bytes(),*iter_blk);
  iris_          = new boxm2_data<BOXM2_PIXEL>((char*)iris_base_->cpu_buffer(),iris_base_->num_bytes(),*iter_blk);
  pupil_         = new boxm2_data<BOXM2_PIXEL>((char*)pupil_base_->cpu_buffer(),pupil_base_->num_bytes(),*iter_blk);
  eyelid_        = new boxm2_data<BOXM2_PIXEL>((char*)eyelid_base_->cpu_buffer(),eyelid_base_->num_bytes(),*iter_blk);
  lower_eyelid_  = new boxm2_data<BOXM2_PIXEL>((char*)lower_eyelid_base_->cpu_buffer(),lower_eyelid_base_->num_bytes(),*iter_blk);
  eyelid_crease_ = new boxm2_data<BOXM2_PIXEL>((char*)eyelid_crease_base_->cpu_buffer(),eyelid_crease_base_->num_bytes(),*iter_blk);

    centerX_ = new bocl_mem(device_->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
    centerY_ = new bocl_mem(device_->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
    centerZ_ = new bocl_mem(device_->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
    centerX_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerY_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerZ_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    // output buffer for debugging

     // bit lookup buffer
    boxm2_ocl_util::set_bit_lookup(lookup_arr_);
    lookup_=new bocl_mem(device_->context(), lookup_arr_, sizeof(cl_uchar)*256, "bit lookup buffer");
    lookup_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

}
void boxm2_vecf_ocl_orbit_scene::extract_target_block_data(boxm2_scene_sptr target_scene){
  std::vector<boxm2_block_id> blocks = target_scene->get_block_ids();
  auto iter_blk = blocks.begin();
  target_blk_ = boxm2_cache::instance()->get_block(target_scene, *iter_blk);
  //  target_pts_ = opencl_cache_->get_data(target_scene,*iter_blk,boxm2_data_traits<BOXM2_POINT>::prefix("target"));

  this->update_target_gpu_buffers(target_scene,*iter_blk);

  target_alpha_data_     = new boxm2_data<BOXM2_ALPHA>(    (char*)target_alpha_base_ ->cpu_buffer(),target_alpha_base_->num_bytes(),*iter_blk);
  target_app_data_       = new boxm2_data<BOXM2_MOG3_GREY>((char*)target_app_base_   ->cpu_buffer(),target_app_base_->num_bytes(),*iter_blk);
  target_nobs_data_      = new boxm2_data<BOXM2_NUM_OBS>(  (char*)target_nobs_base_  ->cpu_buffer(),target_nobs_base_->num_bytes(),*iter_blk);
  target_vis_score_data_ = new boxm2_data<BOXM2_VIS_SCORE>((char*)target_vis_base_   ->cpu_buffer(),target_vis_base_->num_bytes(),*iter_blk);
  target_color_data_     = new boxm2_data<BOXM2_GAUSS_RGB>((char*)target_color_base_ ->cpu_buffer(),target_color_base_->num_bytes(),*iter_blk);

  if(has_background_){
    std::cout<< " Darkening background "<<std::endl;
    this->fill_target_block();
  }
  this->determine_target_box_cell_centers(); // get cell centers in target corresponding to the source block (blk_)
}
bool boxm2_vecf_ocl_orbit_scene::update_target_gpu_buffers(const boxm2_scene_sptr& target_scene, const boxm2_block_id& id,bool write){
  target_alpha_base_ = opencl_cache_->get_data(target_scene,id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
  target_app_base_   = opencl_cache_->get_data(target_scene,id,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  target_nobs_base_  = opencl_cache_->get_data(target_scene,id,boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
  target_vis_base_   = opencl_cache_->get_data(target_scene,id,boxm2_data_traits<BOXM2_VIS_SCORE>::prefix(color_apm_id_));
  target_color_base_ = opencl_cache_->get_data(target_scene,id,boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_apm_id_));
  if(write){
    target_alpha_base_->write_to_buffer(queue_);
    target_app_base_  ->write_to_buffer(queue_);
    target_nobs_base_ ->write_to_buffer(queue_);
    target_vis_base_  ->write_to_buffer(queue_);
    target_color_base_->write_to_buffer(queue_);
  }
  return true;

}

bool boxm2_vecf_ocl_orbit_scene::update_source_gpu_buffers(bool write){


  boxm2_block_id id = blk_->block_id();

  source_alpha_base_ = opencl_cache_->get_data(base_model_,id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
  source_app_base_   = opencl_cache_->get_data(base_model_,id,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  source_nobs_base_  = opencl_cache_->get_data(base_model_,id,boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
  source_vis_base_   = opencl_cache_->get_data(base_model_,id,boxm2_data_traits<BOXM2_VIS_SCORE>::prefix(color_apm_id_));
  source_color_base_ = opencl_cache_->get_data(base_model_,id,boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_apm_id_));

  sphere_base_       = opencl_cache_->get_data(base_model_,id,boxm2_data_traits<BOXM2_PIXEL>::prefix("sphere"));
  iris_base_         = opencl_cache_->get_data(base_model_,id,boxm2_data_traits<BOXM2_PIXEL>::prefix("iris"));
  pupil_base_        = opencl_cache_->get_data(base_model_,id,boxm2_data_traits<BOXM2_PIXEL>::prefix("pupil"));
  eyelid_base_       = opencl_cache_->get_data(base_model_,id,boxm2_data_traits<BOXM2_PIXEL>::prefix("eyelid"));
  lower_eyelid_base_ = opencl_cache_->get_data(base_model_,id,boxm2_data_traits<BOXM2_PIXEL>::prefix("lower_eyelid"));
  eyelid_crease_base_= opencl_cache_->get_data(base_model_,id,boxm2_data_traits<BOXM2_PIXEL>::prefix("eyelid_crease"));

  if(write) {
    source_alpha_base_ ->write_to_buffer(queue_);
    source_app_base_   ->write_to_buffer(queue_);
    source_nobs_base_  ->write_to_buffer(queue_);
    source_vis_base_   ->write_to_buffer(queue_);
    source_color_base_ ->write_to_buffer(queue_);
    sphere_base_       ->write_to_buffer(queue_);
    iris_base_         ->write_to_buffer(queue_);
    pupil_base_        ->write_to_buffer(queue_);
    eyelid_base_       ->write_to_buffer(queue_);
    lower_eyelid_base_ ->write_to_buffer(queue_);
    eyelid_crease_base_ ->write_to_buffer(queue_);
    clFinish(queue_);
  }


  return true;
}

// after loading the block initialize all the cell indices from the block labels, e.g., cell == SPHERE, cell == LOWER_LID, etc.
void boxm2_vecf_ocl_orbit_scene::cache_cell_centers_from_anatomy_labels(){
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
        auto sphere_index       = static_cast<unsigned>(sphere_cell_centers_.size());
        auto eyelid_index       = static_cast<unsigned>(eyelid_cell_centers_.size());
        auto lower_eyelid_index = static_cast<unsigned>(lower_eyelid_cell_centers_.size());
        auto eyelid_crease_index = static_cast<unsigned>(eyelid_crease_cell_centers_.size());
                                                                    // no warnings
        bool sphere       = static_cast<unsigned char>(sphere_->data()[indx])      >0;
        bool iris         = static_cast<unsigned char>(iris_->data()[indx])        >0;
        bool pupil        = static_cast<unsigned char>(pupil_->data()[indx])       >0;
        bool eyelid       = static_cast<unsigned char>(eyelid_->data()[indx])      >0;
        bool lower_eyelid = static_cast<unsigned char>(lower_eyelid_->data()[indx])>0;
        bool eyelid_crease = static_cast<unsigned char>(eyelid_crease_->data()[indx])>0;


        if(sphere){
          sphere_cell_centers_.push_back(p);
          sphere_cell_data_index_.push_back(indx);
          data_index_to_cell_index_[indx] = sphere_index;
          //iris and pupil must be also sphere cells
          if(iris){
            iris_cell_centers_.push_back(p);
            iris_cell_data_index_.push_back(indx);
          }
          if(pupil){
            pupil_cell_centers_.push_back(p);
            pupil_cell_data_index_.push_back(indx);
          }
        }
        if(eyelid){
          eyelid_cell_centers_.push_back(p);
          eyelid_cell_data_index_.push_back(indx);
          eyelid_data_index_to_cell_index_[indx] = eyelid_index;
        }
        if(lower_eyelid){
          lower_eyelid_cell_centers_.push_back(p);
          lower_eyelid_cell_data_index_.push_back(indx);
          lower_eyelid_data_index_to_cell_index_[indx]=lower_eyelid_index;
        }
        if(eyelid_crease){
          eyelid_crease_cell_centers_.push_back(p);
          eyelid_crease_cell_data_index_.push_back(indx);
          eyelid_crease_data_index_to_cell_index_[indx]=eyelid_crease_index;
        }
      }
    }
  }
  std::cout << "Reset indices " << static_cast<double>(t.real())/1000.0 << " sec.\n";
}

//Main constructor
boxm2_vecf_ocl_orbit_scene::boxm2_vecf_ocl_orbit_scene(std::string const& scene_file, const bocl_device_sptr& device, const boxm2_opencl_cache_sptr& opencl_cache, bool is_single_instance , bool is_right ):
  boxm2_vecf_articulated_scene(scene_file), is_right_(is_right),alpha_data_(nullptr), app_data_(nullptr), nobs_data_(nullptr), sphere_(nullptr), iris_(nullptr), pupil_(nullptr),device_(device),opencl_cache_(opencl_cache)
{

  this->extrinsic_only_ = false; int status;
  queue_ = clCreateCommandQueue(device_->context(),*(device_->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);


  this->init_eyelids();

  boxm2_lru_cache::create(base_model_);
  is_single_instance_ = is_single_instance;
  target_blk_ = nullptr;
  target_data_extracted_ = false;
  this->compile_kernels();
  this->extract_block_data();
  this->has_background_ = false;
  if(has_background_){
    this->fill_block();
  }
  this->rebuild();
 }


void boxm2_vecf_ocl_orbit_scene::rebuild(){
  if(this->extrinsic_only_){
    std::cout<<" warning! rebuild called but scene accepts only extrinsic articulations!"<<std::endl;
    return;
  }

    this->params_.init_sphere();
    this->reset_buffers();
    this->init_eyelids();
    this->create_eye();
    this->create_lower_eyelid();
    this->create_eyelid_crease();
    this->create_eyelid();
    this->determine_target_box_cell_centers();
    this->update_source_gpu_buffers(true);
}
 void boxm2_vecf_ocl_orbit_scene::build_sphere(){
   double len = 3 * blk_->sub_block_dim().x();
  double d_thresh = 0.8660*len;//sqrt(3)/2 x len, diagonal distance
  double r0 = params_.eye_radius_;
  double y0 = params_.y_off_ ;
  double rmax = r0+ 2.0;// 2.0 added to provide sufficient margin
  vgl_box_3d<double> bb;
  bb.add(vgl_point_3d<double>(-rmax, -y0, 0.0));
  bb.add(vgl_point_3d<double>(+rmax, -y0 , 0.0));
  bb.add(vgl_point_3d<double>(0.0, -rmax-y0, 0.0));
  bb.add(vgl_point_3d<double>(0.0, rmax-y0, 0.0));
  bb.add(vgl_point_3d<double>(0.0, -y0, -rmax));
  bb.add(vgl_point_3d<double>(0.0, -y0, +rmax));
  vgl_sphere_3d<double> sp(0.0, -y0, 0.0, params_.eye_radius_);
   // cell in a box centers are in global coordinates
  std::vector<cell_info> ccs = blk_->cells_in_box(bb);
  for(auto & cc : ccs){
    const vgl_point_3d<double>& cell_center = cc.cell_center_;
    unsigned indx = cc.data_index_;
    double d = vgl_distance(cell_center, sp);
    if(d < d_thresh){
      if(!is_type_global(cell_center, SPHERE)){
        sphere_cell_centers_.push_back(cell_center);
        sphere_cell_data_index_.push_back(indx);
        data_index_to_cell_index_[indx]=static_cast<unsigned>(sphere_cell_centers_.size())-1;
        float blending_factor = gauss(d,sigma_);
        alpha_data_->data()[indx]= - std::log(1.0 - ( 0.95 ))/ static_cast<float>(this->subblock_len()) * blending_factor;
        sphere_->data()[indx] = static_cast<pixtype>(true);
      }
    }
  }
}
void boxm2_vecf_ocl_orbit_scene::build_iris(){
  iris_cell_centers_.clear();
  iris_cell_data_index_.clear();
  double iris_half_ang = std::atan(params_.iris_radius_/params_.eye_radius_);
  vgl_sphere_3d<double> sph(0.0, -params_.y_off_, 0.0, params_.eye_radius_);
  for(auto cit = sphere_cell_centers_.begin();
      cit != sphere_cell_centers_.end(); ++cit){
    const vgl_point_3d<double>& cell_center = *cit;
    double az = 0.0, el =0.0;
    sph.cartesian_to_spherical(cell_center, el, az);
    if(el<=iris_half_ang){
      unsigned sp_i = static_cast<unsigned>(cit-sphere_cell_centers_.begin());
      // add it to the base set
      auto iit = std::find(iris_cell_centers_.begin(), iris_cell_centers_.end(), cell_center);
      if(iit==iris_cell_centers_.end()){
        iris_cell_centers_.push_back(cell_center);
        unsigned indx = sphere_cell_data_index_[sp_i];
        iris_->data()[indx] = static_cast<pixtype>(true);
        iris_cell_data_index_.push_back(indx);
      }
    }
  }
}
void boxm2_vecf_ocl_orbit_scene::build_pupil(){
  pupil_cell_centers_.clear();
  pupil_cell_data_index_.clear();
  double pupil_half_ang = std::atan(params_.pupil_radius_/params_.eye_radius_);
  vgl_sphere_3d<double> sph(0.0, -params_.y_off_, 0.0, params_.eye_radius_);
  for(auto cit = sphere_cell_centers_.begin();
      cit != sphere_cell_centers_.end(); ++cit){
    const vgl_point_3d<double>& cell_center = *cit;
    double az = 0.0, el =0.0;
    sph.cartesian_to_spherical(cell_center, el, az);
    if(el<=pupil_half_ang){
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
// neighbors of sphere cells
void boxm2_vecf_ocl_orbit_scene::find_cell_neigborhoods(){
  vul_timer t;
  double distance = params_.neighbor_radius()*subblock_len();
  for(unsigned i = 0; i<sphere_cell_centers_.size(); i++){
      vgl_point_3d<double>& p = sphere_cell_centers_[i];
      unsigned indx_i = sphere_cell_data_index_[i];
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
  std::cout << "Find sphere cell neighborhoods in " << static_cast<double>(t.real())/1000.0 << " sec.\n";
}

//run through all the sphere points (sclera) and paint them white
void boxm2_vecf_ocl_orbit_scene::paint_sclera(){

  params_.app_[0]=params_.sclera_intensity_;
  vnl_vector_fixed<unsigned char,8> color;
  color[0] = 255; color[2]= 0; color[4]=0;
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  auto ns = static_cast<unsigned>(sphere_cell_centers_.size());
  for(unsigned i = 0; i<ns; ++i){
    unsigned indx = sphere_cell_data_index_[i];
#if 0
    float d = static_cast<float>(closest_sphere_distance_norm_[i]);
    if(d >0.0f)
      alpha_data_->data()[indx]= -5.0f*std::log(d);//factor of 5 to increase occlusion
        else
#endif
          //          float intensity  = ( float ) (params_.app_[0] ) * blending_factor;
          //          params_.app_[0]= (unsigned char) intensity;
          app_data_->data()[indx] = params_.app_;
          nobs_data_->data()[indx] = nobs;
    //    color_app_data_->data()[indx] =color;
  }
}
//run through all the iris points and paint them
//with the specified intensity
void boxm2_vecf_ocl_orbit_scene::paint_iris(){
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
void boxm2_vecf_ocl_orbit_scene::paint_pupil(){
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
void boxm2_vecf_ocl_orbit_scene::create_eye(){
  vul_timer t;
  this->build_sphere();
  this->build_iris();
  this->build_pupil();
  this->paint_sclera();
  this->paint_iris();
  this->paint_pupil();
  //  std::cout << "Create eye in " << t.real()/1000.0 << " sec.\n";
}

 void boxm2_vecf_ocl_orbit_scene::recreate_eye(){
   this->create_eye();
   this->find_cell_neigborhoods();
   // paint the appearance in base gaze direction, +z
 }
 bool boxm2_vecf_ocl_orbit_scene::is_type_data_index(unsigned data_index, boxm2_vecf_ocl_orbit_scene::anat_type type) const{

   if(type == SPHERE){
     auto sphere = static_cast<unsigned char>(sphere_->data()[data_index]);
     return sphere>0;
   }
   if(type == IRIS){
     auto iris = static_cast<unsigned char>(iris_->data()[data_index]);
     return iris>0;
   }
   if(type == PUPIL){
     auto pupil = static_cast<unsigned char>(pupil_->data()[data_index]);
     return pupil>0;
   }
   if(type == UPPER_LID){
     auto upper_lid = static_cast<unsigned char>(eyelid_->data()[data_index]);
     return upper_lid>0;
   }
   if(type == LOWER_LID){
     auto lower_lid = static_cast<unsigned char>(lower_eyelid_->data()[data_index]);
     return lower_lid>0;
   }
   if(type == EYELID_CREASE){
     auto eyelid_crease = static_cast<unsigned char>(eyelid_crease_->data()[data_index]);
     return eyelid_crease>0;
   }
   return false;
 }

bool boxm2_vecf_ocl_orbit_scene::is_type_global(vgl_point_3d<double> const& global_pt, boxm2_vecf_ocl_orbit_scene::anat_type  type) const{
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

 bool boxm2_vecf_ocl_orbit_scene::find_nearest_data_index(boxm2_vecf_ocl_orbit_scene::anat_type type, vgl_point_3d<double> const& probe, unsigned& data_indx) const{
   double r = this->subblock_len();
   double dr = 2*r;
   vgl_point_3d<double> org = blk_->local_origin();
   vgl_vector_3d<double> loc = (probe-org)/r;
   double x0 = std::floor(loc.x()), y0 = std::floor(loc.y()), z0 = std::floor(loc.z());
   x0 = x0*r + org.x();   y0 = y0*r + org.y();  z0 = z0*r + org.z();
   double dmin = std::numeric_limits<double>::max();
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
           data_index_min = data_index;
         }
       }
   if(dmin>params_.neighbor_radius()*r)
    return false;
   data_indx = data_index_min;
   return true;
 }



void boxm2_vecf_ocl_orbit_scene::create_eyelid(){
  vul_timer t;
   this->build_eyelid();
   this->paint_eyelid();
}

void boxm2_vecf_ocl_orbit_scene::recreate_eyelid(){
  this->create_eyelid();
  //this->find_eyelid_cell_neigborhoods();
}


void boxm2_vecf_ocl_orbit_scene::build_eyelid(){
  double len = 2 * blk_->sub_block_dim().x();
  ;
  double margin = params_.eyelid_radius() * 0.15;
  double d_thresh = margin;
  vgl_box_3d<double> bb = eyelid_geo_.bounding_box(margin);
  // cells in  box centers are in global coordinates
  std::vector<cell_info> ccs = blk_->cells_in_box(bb);
  for(auto & cc : ccs){
    const vgl_point_3d<double>& cell_center = cc.cell_center_;
    unsigned indx = cc.data_index_;
    double d = eyelid_geo_.distance(cell_center);

    if(d < d_thresh){
      if(!eyelid_geo_.inside(cell_center))
        continue;
      if(!is_type_global(cell_center, SPHERE)){
        eyelid_cell_centers_.push_back(cell_center);
        eyelid_cell_data_index_.push_back(indx);
        eyelid_->data()[indx] = static_cast<pixtype>(true);
        eyelid_data_index_to_cell_index_[indx]=static_cast<unsigned>(eyelid_cell_centers_.size())-1;
        float blending_factor = gauss(d,sigma_);
        alpha_data_->data()[indx]= - std::log(1.0 - ( 0.95 ))/ static_cast<float>(this->subblock_len()) * blending_factor;
      }
    }
  }
}

void boxm2_vecf_ocl_orbit_scene::paint_eyelid(){
  params_.app_[0]=params_.eyelid_intensity_;
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  auto ns = static_cast<unsigned>(eyelid_cell_centers_.size());
  for(unsigned i = 0; i<ns; ++i){
    unsigned indx = eyelid_cell_data_index_[i];
    if(is_type_data_index(indx,LOWER_LID))
      continue;
    app_data_->data()[indx] = params_.app_;
    nobs_data_->data()[indx] = nobs;
  }
}

//: construct lolwer eyelid (voxelize and paint)
void boxm2_vecf_ocl_orbit_scene::create_lower_eyelid(){
  this->build_lower_eyelid();
  this->paint_lower_eyelid();
}
//:read block eyelid data and reset indices
void boxm2_vecf_ocl_orbit_scene::recreate_lower_eyelid(){
  this->create_lower_eyelid();
  //  this->find_lower_eyelid_cell_neigborhoods();
}


//: scan dense set of points on the spherical shell to define surface voxels
void boxm2_vecf_ocl_orbit_scene::build_lower_eyelid(){
   double len = 4 * blk_->sub_block_dim().x();
  //  double d_thresh = 1.1 * len;//sqrt(3)/2 x len, diagonal distance
  double d_thresh =  params_.neighbor_radius()*len;
  ;
  vgl_box_3d<double> bb = lower_eyelid_geo_.bounding_box();
 // cells in  box centers are in global coordinates
  std::vector<cell_info> ccs = blk_->cells_in_box(bb);
  for(auto & cc : ccs){
    const vgl_point_3d<double>& cell_center = cc.cell_center_;
    unsigned indx = cc.data_index_;
        double d = lower_eyelid_geo_.distance(cell_center);
    if(d < d_thresh){
      if(!lower_eyelid_geo_.inside(cell_center))
        continue;
        if(!is_type_global(cell_center, LOWER_LID) && !is_type_global(cell_center, SPHERE)){
          lower_eyelid_cell_centers_.push_back(cell_center);
          lower_eyelid_cell_data_index_.push_back(indx);
          lower_eyelid_->data()[indx] = static_cast<pixtype>(true);
          lower_eyelid_data_index_to_cell_index_[indx]=
            static_cast<unsigned>(lower_eyelid_cell_centers_.size())-1;
          float blending_factor = gauss(d,sigma_);
          alpha_data_->data()[indx]= - std::log(1.0 - ( 0.95 ))/ static_cast<float>(this->subblock_len()) * blending_factor;
        }
    }
  }
}
//: assign appearance to eyelid voxels
void boxm2_vecf_ocl_orbit_scene::paint_lower_eyelid(){
  params_.app_[0]=params_.lower_eyelid_intensity_;
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  auto ns = static_cast<unsigned>(lower_eyelid_cell_centers_.size());
  for(unsigned i = 0; i<ns; ++i){
    unsigned indx = lower_eyelid_cell_data_index_[i];
    if(is_type_data_index(indx,UPPER_LID))
      continue;
    app_data_->data()[indx] = params_.app_;
    nobs_data_->data()[indx] = nobs;
  }
}

//: construct eyelid crease (voxelize and paint)
void boxm2_vecf_ocl_orbit_scene::create_eyelid_crease(){
  this->build_eyelid_crease();
  this->paint_eyelid_crease();
}
//:read block eyelid data and reset indices
void boxm2_vecf_ocl_orbit_scene::recreate_eyelid_crease(){
  this->create_eyelid_crease();
  //  this->find_eyelid_crease_cell_neigborhoods();

}

//: scan dense set of points on the spherical shell to define surface voxels
void boxm2_vecf_ocl_orbit_scene::build_eyelid_crease(){
  //  double len = this->subblock_len();
  double len = 4 * blk_->sub_block_dim().x();
  double d_thresh =0.86602540*len;//sqrt(3)/2 x len, diagonal distance

  vgl_box_3d<double> bb = eyelid_crease_geo_.bounding_box();
  // cells in  box centers are in global coordinates
  std::vector<cell_info> ccs = blk_->cells_in_box(bb);
  for(auto & cc : ccs){
    const vgl_point_3d<double>& cell_center = cc.cell_center_;
    unsigned indx = cc.data_index_;
    double d = eyelid_crease_geo_.distance(cell_center);
    if(d < d_thresh){
      if(!eyelid_crease_geo_.inside(cell_center))
        continue;
      bool already_found = is_type_global(cell_center, EYELID_CREASE);
      if(!already_found && !is_type_global(cell_center, SPHERE)){
        eyelid_crease_cell_centers_.push_back(cell_center);
        eyelid_crease_cell_data_index_.push_back(indx);
        eyelid_crease_->data()[indx] = static_cast<pixtype>(true);
        float blending_factor = gauss(d,sigma_);
        alpha_data_->data()[indx]= - std::log(1.0 - ( 0.95 ))/ static_cast<float>(this->subblock_len()) * blending_factor;
        eyelid_crease_data_index_to_cell_index_[indx]=
          static_cast<unsigned>(eyelid_crease_cell_centers_.size())-1;
      }
    }
  }
}
// assign appearance to crease voxels
// upper part of crease is different intensity
void boxm2_vecf_ocl_orbit_scene::paint_eyelid_crease(){
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  auto ns = static_cast<unsigned>(eyelid_crease_cell_centers_.size());
  for(unsigned i = 0; i<ns; ++i){
    const vgl_point_3d<double>& p = eyelid_crease_cell_centers_[i];
    double t = eyelid_crease_geo_.t(p.x(), p.y());
    if(t<params_.eyelid_crease_ct_)
      params_.app_[0]= params_.eyelid_crease_upper_intensity_;
        else
      params_.app_[0]=params_.eyelid_crease_lower_intensity_;
    unsigned indx = eyelid_crease_cell_data_index_[i];
    if(is_type_data_index(indx,UPPER_LID))
      continue;
    app_data_->data()[indx] = params_.app_;
    nobs_data_->data()[indx] = nobs;
  }
}
void  boxm2_vecf_ocl_orbit_scene::inverse_vector_field_eye(vgl_rotation_3d<double> const& rot, std::vector<vgl_vector_3d<double> >& vf,
                                                       std::vector<bool>& valid) const{

  vul_timer t;
  auto nt = static_cast<unsigned>(box_cell_centers_.size());


  auto* source_pts_cpu = (  vnl_vector_fixed<float,4>*) source_pts_->cpu_buffer();
  auto* target_pts_cpu = (  vnl_vector_fixed<float,4>*) source_pts_->cpu_buffer();
  vf.resize(nt);// initialized to 0
  valid.resize(nt, false);
  vgl_rotation_3d<double> inv_rot = rot.inverse();
  // sphere center assumed to be at the origin, fix later
  double r0 = params_.eye_radius_;
  double len = 2.0; //2.0 to provide sufficent margin
  len *= params_.neighbor_radius();
  double rmax = r0+len;
  double rmin = r0-len;
  double y0 = params_.y_off_;
  vgl_box_3d<double> sb; // sphere bounding box, slightly larger than the sphere
  sb.add(vgl_point_3d<double>(-rmax, -y0, 0.0));
  sb.add(vgl_point_3d<double>(+rmax, -y0, 0.0));
  sb.add(vgl_point_3d<double>(0.0, -rmax - y0, 0.0));
  sb.add(vgl_point_3d<double>(0.0, +rmax - y0, 0.0));
  sb.add(vgl_point_3d<double>(0.0, -y0, -rmax));
  sb.add(vgl_point_3d<double>(0.0, -y0, +rmax));
  vgl_sphere_3d<double> smin(0.0, y0, 0.0,rmin);
  unsigned cnt = 0, ncont = 0;

  for(unsigned i = 0; i<nt; ++i){
    vgl_point_3d<double> p = (box_cell_centers_[i].cell_center_)-params_.offset_;

    if(!sb.contains(p)){
      continue;
    }
    if(smin.contains(p)){
      ncont++;
      continue;
    }
    cnt++;
    // vf only defined for cells on the sphere
    if(!is_type_global(p, SPHERE))
      continue;
    valid[i]=true;
    unsigned dindx;
    vgl_point_3d<double> rp = inv_rot * p;
    vgl_point_3d<double> xformed_p(rp.x() - p.x(), rp.y() - p.y(), rp.z() - p.z());
    if (care_){ //see if we truly need the closest data index
      if(!this->find_nearest_data_index(SPHERE, xformed_p, dindx))
        continue;
    }else{
      if(! blk_->data_index(xformed_p, dindx))
        continue;
    }
    unsigned tindx = box_cell_centers_[i].data_index_;

    //is a sphere voxel cell so define the vector field

    source_pts_cpu[dindx][0] = p.x();     source_pts_cpu[dindx][1] = p.y();     source_pts_cpu[dindx][2] = p.z();
    target_pts_cpu[tindx][0] = p.x();     target_pts_cpu[tindx][1] = p.y();     target_pts_cpu[tindx][2] = p.z();

  }
}

void  boxm2_vecf_ocl_orbit_scene::inverse_vector_field_eyelid(double dt, std::vector<vgl_vector_3d<double> >& vfield, std::vector<unsigned char>& valid) const{
  vul_timer t;

  auto nt = static_cast<unsigned>(box_cell_centers_.size());
  vfield.resize(nt);// initialized to 0
  auto* source_pts_cpu = (  vnl_vector_fixed<float,4>*) source_pts_->cpu_buffer();
  auto* target_pts_cpu = (  vnl_vector_fixed<float,4>*) source_pts_->cpu_buffer();

  valid.resize(nt, static_cast<unsigned char>(0));
  vgl_box_3d<double> eb = eyelid_geo_.bounding_box();
  for(unsigned i = 0; i<nt; ++i){
    vgl_point_3d<double> p = (box_cell_centers_[i].cell_center_) - params_.offset_;

    if (is_right_){
      vgl_vector_3d<double> flip(-2 * p.x(),0,0);
      p = p + flip;
    }

    if(!eb.contains(p))
      continue;
    // vf only defined for cells on the closed eyelid
    if(!is_type_global(p, UPPER_LID))
      continue;
    double tc = eyelid_geo_.t(p.x(), p.y());
    if(!eyelid_geo_.valid_t(tc))
      continue;
    // inverse field so negate dt
    double ti = tc-dt;
    if(!eyelid_geo_.valid_t(ti)){
      valid[i]=static_cast<unsigned char>(1);
      continue; // vf not defined so clear cell alpha
    }
    unsigned dindx=0;
    vgl_point_3d<double> xformed_p = p + eyelid_geo_.vf(p.x(),tc, -dt);
    if (care_){ //see if we truly need the closest data index
      if(!this->find_nearest_data_index(UPPER_LID, xformed_p, dindx))
        continue;
    }else{
      if(! blk_->data_index(xformed_p, dindx))
        continue;
    }

    unsigned tindx = box_cell_centers_[i].data_index_;
    source_pts_cpu[dindx][0] = p.x();     source_pts_cpu[dindx][1] = p.y();     source_pts_cpu[dindx][2] = p.z();
    target_pts_cpu[tindx][0] = p.x();     target_pts_cpu[tindx][1] = p.y();     target_pts_cpu[tindx][2] = p.z();
    valid[i]=static_cast<unsigned char>(2);
  }
  //  std::cout << "Create eyelid vector field in " << t.real()/1000.0 << " sec.\n";
}

void  boxm2_vecf_ocl_orbit_scene::inverse_vector_field_lower_eyelid(std::vector<vgl_vector_3d<double> >& vfield, std::vector<bool>& valid) const{
  vul_timer t;
  auto nt = static_cast<unsigned>(box_cell_centers_.size());
  auto* source_pts_cpu = (  vnl_vector_fixed<float,4>*) source_pts_->cpu_buffer();
  auto* target_pts_cpu = (  vnl_vector_fixed<float,4>*) source_pts_->cpu_buffer();

  vfield.resize(nt);// initialized to 0
  valid.resize(nt, false);
  vgl_box_3d<double> eb = lower_eyelid_geo_.bounding_box();
  for(unsigned i = 0; i<nt; ++i){
    vgl_point_3d<double> p = (box_cell_centers_[i].cell_center_) - params_.offset_;
    if (is_right_){
      vgl_vector_3d<double> flip(-2 * p.x(),0,0);
      p = p + flip;
    }

    if(!eb.contains(p))
      continue;
    // vf only defined for cells on the closed eyelid
    if(!is_type_global(p, LOWER_LID))
      continue;
    double tc = lower_eyelid_geo_.t(p.x(), p.y());
    if(!lower_eyelid_geo_.valid_t(tc))
      continue;
   // no movement of the lower lid for now
    valid[i]=true;
    unsigned dindx=0;
    vgl_point_3d<double> xformed_p = p;
    if (care_){ //see if we truly need the closest data index
      if(!this->find_nearest_data_index(LOWER_LID, xformed_p, dindx))
        continue;
    }else{
      if(! blk_->data_index(xformed_p, dindx))
        continue;
    }
    unsigned tindx = box_cell_centers_[i].data_index_;
    eyelid_geo_.vf(0, 0, 0);
    source_pts_cpu[dindx][0] = p.x();     source_pts_cpu[dindx][1] = p.y();     source_pts_cpu[dindx][2] = p.z();
    target_pts_cpu[tindx][0] = p.x();     target_pts_cpu[tindx][1] = p.y();     target_pts_cpu[tindx][2] = p.z();
    valid[i]=static_cast<unsigned char>(2);
  }

}

void  boxm2_vecf_ocl_orbit_scene::inverse_vector_field_eyelid_crease(std::vector<vgl_vector_3d<double> >& vfield, std::vector<bool>& valid) const{
  vul_timer t;
  auto* source_pts_cpu = (  vnl_vector_fixed<float,4>*) source_pts_->cpu_buffer();
  auto* target_pts_cpu = (  vnl_vector_fixed<float,4>*) source_pts_->cpu_buffer();

  auto nt = static_cast<unsigned>(box_cell_centers_.size());
  vfield.resize(nt);// initialized to 0
  valid.resize(nt, false);
  vgl_box_3d<double> eb = eyelid_crease_geo_.bounding_box();
  for(unsigned i = 0; i<nt; ++i){
    vgl_point_3d<double> p = (box_cell_centers_[i].cell_center_)-params_.offset_;
    if (is_right_){
      vgl_vector_3d<double> flip(-2 * p.x(),0,0);
      p = p + flip;
    }
    if(!eb.contains(p))
      continue;
    // vf only defined for cells crease
    if(!is_type_global(p, EYELID_CREASE))
      continue;
    double tc = eyelid_crease_geo_.t(p.x(), p.y());
    if(!eyelid_crease_geo_.valid_t(tc))
      continue;
    vgl_point_3d<double> xformed_p = p;
    unsigned dindx;
    if (care_){ //see if we truly need the closest data index
      if(!this->find_nearest_data_index(LOWER_LID, xformed_p, dindx))
        continue;
    }else{
      if(! blk_->data_index(xformed_p, dindx))
        continue;
    }

    unsigned tindx = box_cell_centers_[i].data_index_;
    source_pts_cpu[dindx][0] = p.x();     source_pts_cpu[dindx][1] = p.y();     source_pts_cpu[dindx][2] = p.z();
    target_pts_cpu[tindx][0] = p.x();     target_pts_cpu[tindx][1] = p.y();     target_pts_cpu[tindx][2] = p.z();

    valid[i]=true;
  }
  //  std::cout << "Create eyelid vector field in " << t.real()/1000.0 << " sec.\n";
}

//
// interpolate data around the inverted position of the target in the source reference frame. Interpolation weights are based
// on a Gaussian distribution with respect to distance from the source location
//
void boxm2_vecf_ocl_orbit_scene::interpolate_vector_field(vgl_point_3d<double> const& src, unsigned sindx, unsigned dindx, unsigned tindx,
                                                      std::vector<vgl_point_3d<double> > & cell_centers,
                                                      std::map<unsigned, std::vector<unsigned> >& cell_neighbor_cell_index,
                                                      std::map<unsigned, std::vector<unsigned> >&cell_neighbor_data_index){
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
  boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype color_app;
  typedef vnl_vector_fixed<double,8> double8;
  double8 curr_color;

  const vgl_point_3d<double>& scell = cell_centers[sindx];
  //appearance and alpha data at source cell
  app = app_data_->data()[dindx];
  color_app = color_app_data_->data()[dindx];
  curr_color[0] = ((double) color_app[0]) / 255; curr_color[2] = ((double)color_app[2])/255; curr_color[4]= ((double)color_app[4])/255;
  boxm2_data_traits<BOXM2_ALPHA>::datatype alpha0 = alpha_data_->data()[dindx];
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
    app = app_data_->data()[nidx];
    color_app = color_app_data_->data()[dindx];
    double alpha = alpha_data_->data()[nidx];
    curr_color[0] = ((double) color_app[0]) / 255; curr_color[2] = ((double)color_app[2])/255; curr_color[4]= ((double)color_app[4])/255;
    sumint   += w * app[0];
    sumalpha += w * alpha;
    sumcolor += w * curr_color;
  }
  sumint/=sumw;
  app[0] = static_cast<unsigned char>(sumint);
  sumalpha /= sumw;
  sumcolor/=sumw;
  color_app[0] = (unsigned char) (sumcolor[0] * 255); color_app[2] = (unsigned char)(sumcolor[2]*255); color_app[4]= (unsigned char) (sumcolor[4] * 255);
  auto alpha = static_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype>(sumalpha);
  target_app_data_->data()[tindx] = app;
  target_alpha_data_->data()[tindx] = alpha;
  target_color_data_->data()[tindx] = color_app;
  target_vis_score_data_->data()[tindx] = 1;
}

void boxm2_vecf_ocl_orbit_scene::apply_eye_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf,
                                                              std::vector<bool> const& valid){
  //  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
  vnl_vector_fixed<unsigned char,8> color = random_color();

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
    if(!this->find_nearest_data_index(SPHERE, src, dindx))
      continue;
    sindx = data_index_to_cell_index_[dindx];
    unsigned tindx = box_cell_centers_[j].data_index_;

    //target_color_data_->data()[tindx] = color;
    // if( target_vis_score_data_->data()[tindx]> 0.6)
    //   target_vis_score_data_->data()[tindx] = 1;
    this->interpolate_vector_field(src, sindx, dindx, tindx,
                                   sphere_cell_centers_, cell_neighbor_cell_index_,
                                   cell_neighbor_data_index_);
  }
  //  std::cout << "Apply eye vector field in " << t.real()/1000.0 << " sec.\n";
}

void boxm2_vecf_ocl_orbit_scene::apply_eyelid_vector_field_to_target(std::vector<vgl_vector_3d<double> > const& vf,
                                                                 std::vector<unsigned char> const& valid){
  vul_timer t;
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
  vnl_vector_fixed<unsigned char,8> color = random_color();
  int n = static_cast<unsigned>(box_cell_centers_.size());

  if(n==0)
    return;//shouldn't happen
  // iterate over the target cells and interpolate info from source
  for(int j = 0; j<n; ++j){
    if(!valid[j])
      continue;
    // data index to the cell j in the target scene
    unsigned tindx = box_cell_centers_[j].data_index_;

    //vector field source is outside eyelid so target becomes transparent
    if(valid[j]==1){
      app[0] = static_cast<unsigned char>(185);
      target_app_data_->data()[tindx] = app;                                                 //transparent
      target_alpha_data_->data()[tindx]=static_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype>(0.00001);
      continue;
    }
    //valid == 2 for a defined vector field value
    if(valid[j]>2)
      continue;
    // target cell center translated back to source box, and should be an eyelid point
    // but we don't need to check, since what is needed is the source cell for the rotation
    vgl_point_3d<double> p = box_cell_centers_[j].cell_center_-params_.offset_;
    if (is_right_){
      vgl_vector_3d<double> flip(-2 * p.x(),0,0);
      p = p + flip;
    }
    vgl_point_3d<double> src = p + vf[j];//add vector field

    // find closest sphere voxel cell
    unsigned sindx=0, dindx=0;
    if(!find_nearest_data_index(UPPER_LID, src, dindx))
      continue;

    sindx = eyelid_data_index_to_cell_index_[dindx];
    //    target_color_data_->data()[tindx] = color;

    //    target_vis_score_data_->data()[tindx] = 1;
    interpolate_vector_field(src, sindx, dindx, tindx,eyelid_cell_centers_,
                             eyelid_cell_neighbor_cell_index_,eyelid_cell_neighbor_data_index_);
  }
  //  std::cout << "Apply eyelid vector field in " << t.real()/1000.0 << " sec.\n";
}

void boxm2_vecf_ocl_orbit_scene::apply_lower_eyelid_vector_field_to_target(std::vector<vgl_vector_3d<double> > const&  /*vf*/,
                                                                       std::vector<bool> const& valid){

   vul_timer t;
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;

  vnl_vector_fixed<unsigned char,8> color = random_color();
  int n = static_cast<unsigned>(box_cell_centers_.size());
    if(n==0)
    return;//shouldn't happen
  // iterate over the target cells and interpolate info from source
  for(int j = 0; j<n; ++j){
    if(!valid[j])
      continue;
    // data index to the cell j in the target scene
    unsigned tindx = box_cell_centers_[j].data_index_;

    // target cell center translated back to source box, and should be an eyelid point
    // but we don't need to check, since what is needed is the source cell for the rotation
    // for now no movement ie vf is always zer0
    vgl_point_3d<double> src = box_cell_centers_[j].cell_center_-params_.offset_;
    if (is_right_){
      vgl_vector_3d<double> flip(-2 * src.x(),0,0);
      src = src + flip;
    }

    // find closest sphere voxel cell
    unsigned sindx=0, dindx=0;
    if(!find_nearest_data_index(LOWER_LID, src, dindx))
      continue;
    //    std::cout<<(int)app_data_->data()[dindx][0]<<" ";
    sindx = lower_eyelid_data_index_to_cell_index_[dindx];
    //    target_color_data_->data()[tindx] = color;
    //    target_vis_score_data_->data()[tindx] = 1;
    interpolate_vector_field(src, sindx, dindx, tindx, lower_eyelid_cell_centers_,
                             lower_eyelid_cell_neighbor_cell_index_,lower_eyelid_cell_neighbor_data_index_);
  }
  //  std::cout << "Apply lower lower_eyelid vector field in " << t.real()/1000.0 << " sec.\n";
}

void boxm2_vecf_ocl_orbit_scene::apply_eyelid_crease_vector_field_to_target(std::vector<vgl_vector_3d<double> > const&  /*vf*/,
                                                                       std::vector<bool> const& valid){

   vul_timer t;
  vnl_vector_fixed<unsigned char,8> color = random_color();
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
  int n = static_cast<unsigned>(box_cell_centers_.size());

  if(n==0)
    return;//shouldn't happen
  // iterate over the target cells and interpolate info from source
  for(int j = 0; j<n; ++j){
    if(!valid[j])
      continue;
    // data index to the cell j in the target scene
    unsigned tindx = box_cell_centers_[j].data_index_;

    // target cell center translated back to source box, and should be an eyelid point
    // but we don't need to check, since what is needed is the source cell for the rotation
    // for now no movement ie vf is always zer0
    vgl_point_3d<double> src = box_cell_centers_[j].cell_center_-params_.offset_;
    if (is_right_){
      vgl_vector_3d<double> flip(-2 * src.x(),0,0);
      src = src + flip;
    }

    // find closest sphere voxel cell
    unsigned sindx=0, dindx=0;
    if(!find_nearest_data_index(EYELID_CREASE, src, dindx))
      continue;

    sindx = eyelid_crease_data_index_to_cell_index_[dindx];
    //    target_color_data_->data()[tindx] = color;
    interpolate_vector_field(src, sindx, dindx, tindx, eyelid_crease_cell_centers_,
                             eyelid_crease_cell_neighbor_cell_index_,eyelid_crease_cell_neighbor_data_index_);
  }
  //  std::cout << "Apply lower eyelid_crease vector field in " << t.real()/1000.0 << " sec.\n";
}

void boxm2_vecf_ocl_orbit_scene::map_to_target(boxm2_scene_sptr target_scene){
  static bool first = true;
  if(!target_data_extracted_){
    this->extract_target_block_data(target_scene);
    target_data_extracted_  = true;
    if (is_single_instance_){
      first = false; // won't work for two eyes
    }
  }

  if(intrinsic_change_ && is_single_instance_){
    intrinsic_change_ = false;
    this->clear_target(target_scene);
  }
  if (!this->map_orbit_to_target_single_pass(target_scene))
    std::cout<<" Orbit failed to map to target "<<std::endl;
  return;
}


bool boxm2_vecf_ocl_orbit_scene::set_params(boxm2_vecf_articulated_params const& params){
  try{
    auto const& params_ref = dynamic_cast<boxm2_vecf_orbit_params const &>(params);
    intrinsic_change_ = this->vfield_params_change_check(params_ref); // assuming intrinsic parameters changed,i.e. eye color and the orbit scene needs to be rebuilt and repainted
    params_ =boxm2_vecf_orbit_params(params_ref);
#if _DEBUG
    std::cout<< "intrinsic change? "<<intrinsic_change_<<std::endl;
#endif
    if(intrinsic_change_){
      this->rebuild();
    }
    return true;
  }catch(std::exception e){
    std::cout<<" Can't downcast orbit parameters! PARAMATER ASSIGNMENT PHAILED!"<<std::endl;
    return false;
  }
}
vnl_vector_fixed<unsigned char,8> boxm2_vecf_ocl_orbit_scene::random_color(bool  /*yuv*/){
  unsigned char  R = static_cast<unsigned char>(rand()) % (unsigned char) 255;
  unsigned char  G = static_cast<unsigned char>(rand()) % (unsigned char) 255;
  unsigned char  B = static_cast<unsigned char>(rand()) % (unsigned char) 255;
  vnl_vector_fixed<unsigned char,8> ret;
  ret.fill(0);
  ret[0] = R; ret[1] =G; ret[2] =B;

  return ret;
}
void boxm2_vecf_ocl_orbit_scene::init_eyelids(){
  eyelid_geo_ =boxm2_vecf_eyelid(this->params_);
  eyelid_geo_.set_tmin(params_.eyelid_tmin_);
  eyelid_geo_.set_tmax(params_.eyelid_tmax_);

  lower_eyelid_geo_ =boxm2_vecf_eyelid(this->params_);
  lower_eyelid_geo_.set_tmin(params_.lower_eyelid_tmin_);
  lower_eyelid_geo_.set_tmax(params_.lower_eyelid_tmax_);

  eyelid_crease_geo_ =boxm2_vecf_eyelid_crease(this->params_);
  eyelid_crease_geo_.set_tmin(params_.eyelid_crease_tmin_);
  eyelid_crease_geo_.set_tmax(params_.eyelid_crease_tmax_);
  vnl_vector_fixed<double, 5> t0_coeffs = this->params_.eyelid_coefs_t0();
  vnl_vector_fixed<double, 5> t1_coeffs = this->params_.eyelid_coefs_t1();

  for (unsigned i=0; i<5;i++){
    eyelid_cl_buf_[i]   = t0_coeffs[i];
    eyelid_cl_buf_[i+5] = t1_coeffs[i];

    lower_eyelid_cl_buf_[i]   = t0_coeffs[i];
    lower_eyelid_cl_buf_[i+5] = t1_coeffs[i];

    eyelid_crease_cl_buf_[i]   = t0_coeffs[i];
    eyelid_crease_cl_buf_[i+5] = t1_coeffs[i];
}
  eyelid_cl_buf_[10] = this->params_.scale_x();
  eyelid_cl_buf_[11] = this->params_.scale_y();
  eyelid_cl_buf_[12] = this->params_.dphi_rad();
  eyelid_cl_buf_[13] = this->params_.eyelid_tmin_;
  eyelid_cl_buf_[14] = this->params_.eyelid_tmax_;
  eyelid_cl_buf_[15] = this->params_.mid_superior_margin_z_;
  eyelid_cl_buf_[16] = this->params_.mid_inferior_margin_z_;
  eyelid_cl_buf_[17] = this->params_.medial_socket_radius();
  eyelid_cl_buf_[18] = this->params_.lateral_socket_radius();
  eyelid_cl_buf_[19] = this->params_.eyelid_radius();

  lower_eyelid_cl_buf_[10] = this->params_.scale_x();
  lower_eyelid_cl_buf_[11] = this->params_.scale_y();
  lower_eyelid_cl_buf_[12] = this->params_.dphi_rad();

  eyelid_crease_cl_buf_[10] = this->params_.scale_x();
  eyelid_crease_cl_buf_[11] = this->params_.scale_y();
  eyelid_crease_cl_buf_[12] = this->params_.dphi_rad();
  eyelid_crease_cl_buf_[13] = this->params_.eyelid_crease_scale_y();

  eyelid_geo_cl_       = new bocl_mem(device_->context(), eyelid_cl_buf_ ,        sizeof(float)*21, " eyelid struct " );
 eyelid_crease_geo_cl_ = new bocl_mem(device_->context(), eyelid_crease_cl_buf_ , sizeof(float)*16, " eyelid crease struct " );
 lower_eyelid_geo_cl_  = new bocl_mem(device_->context(), lower_eyelid_cl_buf_ ,  sizeof(float)*16, "lower eyelid struct " );

 eyelid_geo_cl_        ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
 eyelid_crease_geo_cl_ ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
 lower_eyelid_geo_cl_  ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );


  eyelid_geo_cl_       ->write_to_buffer(queue_);
  lower_eyelid_geo_cl_ ->write_to_buffer(queue_);
  eyelid_crease_geo_cl_->write_to_buffer(queue_);
}
bool boxm2_vecf_ocl_orbit_scene::vfield_params_change_check(const boxm2_vecf_orbit_params & params){
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
}
void boxm2_vecf_ocl_orbit_scene::reset_buffers(bool color_only){
  std::vector<boxm2_block_id> blocks = base_model_->get_block_ids();
  boxm2_block_metadata mdata = base_model_->get_block_metadata_const(blocks[0]);


  app_data_      ->set_default_value(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(), mdata);
  color_app_data_->set_default_value(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_apm_id_), mdata);
  source_app_base_   = opencl_cache_->get_data(base_model_,blocks[0],boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  source_color_base_ = opencl_cache_->get_data(base_model_,blocks[0],boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_apm_id_));

  source_color_base_->write_to_buffer(queue_);
  //  source_app_base_  ->write_to_buffer(queue_);

  if (color_only)
    return;
  alpha_data_    ->set_default_value(boxm2_data_traits<BOXM2_ALPHA>::prefix(), mdata);
  sphere_        ->set_default_value(boxm2_data_traits<BOXM2_PIXEL>::prefix("sphere"), mdata);
  iris_          ->set_default_value(boxm2_data_traits<BOXM2_PIXEL>::prefix("iris"), mdata);
  pupil_         ->set_default_value(boxm2_data_traits<BOXM2_PIXEL>::prefix("pupil"), mdata);
  eyelid_        ->set_default_value(boxm2_data_traits<BOXM2_PIXEL>::prefix("eyelid"), mdata);
  lower_eyelid_  ->set_default_value(boxm2_data_traits<BOXM2_PIXEL>::prefix("lower_eyelid"), mdata);
  eyelid_crease_ ->set_default_value(boxm2_data_traits<BOXM2_PIXEL>::prefix("eyelid_crease"), mdata);

  sphere_cell_centers_.clear();
  sphere_cell_data_index_.clear();

  cell_neighbor_cell_index_.clear();
  data_index_to_cell_index_.clear();
  cell_neighbor_data_index_.clear();


  iris_cell_centers_.clear();
  iris_cell_data_index_.clear();

  pupil_cell_centers_.clear();
  pupil_cell_data_index_.clear();

  eyelid_cell_centers_.clear();
  eyelid_cell_data_index_.clear();
  eyelid_cell_neighbor_cell_index_.clear();
  eyelid_data_index_to_cell_index_.clear();
  eyelid_cell_neighbor_data_index_.clear();

  lower_eyelid_cell_centers_.clear()  ;
  lower_eyelid_cell_data_index_.clear();
  lower_eyelid_cell_neighbor_cell_index_.clear();
  lower_eyelid_data_index_to_cell_index_.clear();
  lower_eyelid_cell_neighbor_data_index_.clear();

  eyelid_crease_cell_centers_.clear()  ;
  eyelid_crease_cell_data_index_.clear();
  eyelid_crease_cell_neighbor_cell_index_.clear();
  eyelid_crease_data_index_to_cell_index_.clear();
  eyelid_crease_cell_neighbor_data_index_.clear();


}
// find the orbit cell locations in the target volume
void boxm2_vecf_ocl_orbit_scene::determine_target_box_cell_centers(){
  vgl_box_3d<double> source_box = blk_->bounding_box_global();
  vgl_box_3d<double> offset_box(source_box.centroid() + params_.offset_ ,source_box.width(),source_box.height(),source_box.depth(),vgl_box_3d<double>::centre);
  if(target_blk_){
    box_cell_centers_ = target_blk_->cells_in_box(offset_box);
    // vgl_box_3d<double> target_box = target_blk_->bounding_box_global();
    // std::cout<< "fraction of cells  " << ((float)box_cell_centers_.size())/(target_blk_->cells_in_box(target_box).size())<<std::endl;
  }
 }

bool boxm2_vecf_ocl_orbit_scene::get_scene_appearance(std::string&      options)
{

    std::vector<std::string> apps = base_model_->appearances();
    bool foundDataType = false;
    for (const auto & app : apps) {
      boxm2_data_type app_type = boxm2_data_info::data_type(app);
        if ( app_type == BOXM2_MOG3_GREY )
        {
            app_type_ = BOXM2_MOG3_GREY;
            foundDataType = true;
            options="-D MOG_TYPE_8 ";
        }
        else if ( app_type == BOXM2_MOG3_GREY_16 )
        {
            app_type_ = BOXM2_MOG3_GREY_16;
            foundDataType = true;
            options="-D MOG_TYPE_16 ";
        }
    }
    if (!foundDataType) {
        std::cout<<"ERROR: boxm2_vecf_ocl_transform_base_model: unsupported gray appearance type" << std::endl;
        return false;
    }

  color_app_type_id_ = "";
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(this->color_apm_id_) )
    {
      color_app_type_id_ = boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(this->color_apm_id_);
      std::cout<<"found color data type "<<color_app_type_id_<< " in source base_model"<<std::endl;
      options +="-D HAS_RGB ";
    }

  }

    return true;
}
 bool boxm2_vecf_ocl_orbit_scene::map_orbit_to_target_single_pass(const boxm2_scene_sptr& target_scene){
  float rotation_buff   [9];
  float offset_buff     [3];

  bool good_buffs=true;
  vnl_vector_fixed<double, 3> Z(0.0, 0.0, 1.0);
  vnl_vector_fixed<double, 3> to_dir(params_.eye_pointing_dir_.x(),
                                     params_.eye_pointing_dir_.y(),
                                     params_.eye_pointing_dir_.z());
  vgl_rotation_3d<double> rot(Z, to_dir);
  offset_buff[0] = params_.offset_.x();
  offset_buff[1] = params_.offset_.y();
  offset_buff[2] = params_.offset_.z();
  float curr_dt = -params_.eyelid_dt_;
  vnl_matrix_fixed<double, 3, 3> R = rot.inverse().as_matrix();
   rotation_buff[0] =(float) R(0,0);  rotation_buff[3] =(float) R(1,0);  rotation_buff[6] =(float) R(2,0);
   rotation_buff[1] =(float) R(0,1);  rotation_buff[4] =(float) R(1,1);  rotation_buff[7] =(float) R(2,1);
   rotation_buff[2] =(float) R(0,2);  rotation_buff[5] =(float) R(1,2);  rotation_buff[8] =(float) R(2,2);

   bocl_mem_sptr  rotation_l = new bocl_mem(device_->context(), rotation_buff, sizeof(float)*9, " rotation " );
   good_buffs &=  rotation_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  offset_l = new bocl_mem(device_->context(), offset_buff, sizeof(float)*3, " offset " );
   good_buffs &=  offset_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   bocl_mem_sptr  dt_l = new bocl_mem(device_->context(), &curr_dt , sizeof(float), " dt buff " );
   good_buffs &=  dt_l->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   std::size_t local_threads[1]={64};
   static std::size_t global_threads[1]={1};

    std::vector<boxm2_block_id> blocks_target = target_scene->get_block_ids();
     std::vector<boxm2_block_id> blocks_source = base_model_->get_block_ids();
     auto iter_blk_target = blocks_target.begin();
     auto iter_blk_source = blocks_source.begin();

     if(blocks_target.size()!=1||blocks_source.size()!=1)
       return false;

   boxm2_scene_info*    info_buffer_source      = base_model_->get_blk_metadata(*iter_blk_source);
   boxm2_scene_info*    info_buffer_target      = target_scene->get_blk_metadata(*iter_blk_target);
   auto target_data_size =  (std::size_t) target_blk_->num_cells();
   info_buffer_target->data_buffer_length = target_data_size;
   bocl_mem_sptr blk_info_target  = new bocl_mem(device_->context(), info_buffer_target, sizeof(boxm2_scene_info), " Scene Info Target" );
  good_buffs &= blk_info_target->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  blk_info_target->write_to_buffer(queue_);


  bocl_mem_sptr blk_info_source  = new bocl_mem(device_->context(), info_buffer_source, sizeof(boxm2_scene_info), " Scene Info Source" );
   auto source_data_size =  (std::size_t) blk_->num_cells();
   info_buffer_source->data_buffer_length = (int) source_data_size;
   blk_info_source->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
   blk_info_source->write_to_buffer(queue_);

#ifdef DEBUG_CL
   float* output = new float[target_data_size];
   bocl_mem_sptr output_cl = new bocl_mem(device_->context(), output, target_data_size * sizeof(float), "Output buf" );
   good_buffs &=  output_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   float* output_mog = new float[source_data_size *8];
   bocl_mem_sptr output_mog_cl = new bocl_mem(device_->context(), output_mog, source_data_size * sizeof(float)*8, "Output buf" );
   good_buffs &=  output_mog_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   float* output_color = new float[source_data_size *4];
   bocl_mem_sptr output_color_cl = new bocl_mem(device_->context(), output_color, source_data_size * sizeof(float)*4, "Output buf" );
   good_buffs &=  output_color_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   float* nb_top = new float[source_data_size*16];
   bocl_mem_sptr nb_top_cl = new bocl_mem(device_->context(), nb_top, source_data_size * sizeof(float) *16, "nb top buf" );
   good_buffs &=  nb_top_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   float* nb_bot = new float[source_data_size*16];
   bocl_mem_sptr nb_bot_cl = new bocl_mem(device_->context(), nb_bot, source_data_size * sizeof(float) *16, "nb top buf" );
   good_buffs &=  nb_bot_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

   output_cl->zero_gpu_buffer(queue_);
   output_mog_cl->zero_gpu_buffer(queue_);
   output_color_cl->zero_gpu_buffer(queue_);
   nb_bot_cl->zero_gpu_buffer(queue_);

#endif

   if(!good_buffs){
     std::cout<<" good buffs returned false"<<std::endl;
     return false;
   }
     global_threads[0] = (unsigned) RoundUp(info_buffer_target->scene_dims[0]*info_buffer_target->scene_dims[1]*info_buffer_target->scene_dims[2],(int)local_threads[0]);

   bocl_mem*  blk_source      = opencl_cache_->get_block(base_model_, *iter_blk_source);
   bocl_mem*  blk_target      = opencl_cache_->get_block(target_scene,*iter_blk_target);
   this->update_target_gpu_buffers(target_scene, *iter_blk_target);
   this->update_source_gpu_buffers();
   bocl_kernel* map_to_target_kern = kernels[0];
   map_to_target_kern->set_arg(centerX_.ptr());
   map_to_target_kern->set_arg(centerY_.ptr());
   map_to_target_kern->set_arg(centerZ_.ptr());
   map_to_target_kern->set_arg(lookup_.ptr());
   map_to_target_kern->set_arg(blk_info_target.ptr());
   map_to_target_kern->set_arg(blk_info_source.ptr());
   map_to_target_kern->set_arg(blk_target);
   map_to_target_kern->set_arg(blk_source);
   map_to_target_kern->set_arg(target_alpha_base_);
   map_to_target_kern->set_arg(target_app_base_);
   map_to_target_kern->set_arg(target_color_base_);
   map_to_target_kern->set_arg(target_vis_base_);
   map_to_target_kern->set_arg(source_alpha_base_);
   map_to_target_kern->set_arg(source_app_base_);
   map_to_target_kern->set_arg(source_color_base_);
   map_to_target_kern->set_arg(source_vis_base_);
   map_to_target_kern->set_arg(rotation_l.ptr());
   map_to_target_kern->set_arg(offset_l.ptr());
   map_to_target_kern->set_arg(sphere_base_);
   map_to_target_kern->set_arg(iris_base_);
   map_to_target_kern->set_arg(pupil_base_);
   map_to_target_kern->set_arg(eyelid_base_);
   map_to_target_kern->set_arg(eyelid_crease_base_);
   map_to_target_kern->set_arg(lower_eyelid_base_);
   map_to_target_kern->set_arg(eyelid_geo_cl_.ptr());
   map_to_target_kern->set_arg(dt_l.ptr());
#ifdef DEBUG_CL
   map_to_target_kern->set_arg(output_cl.ptr());
   map_to_target_kern->set_arg(nb_top_cl.ptr());
   map_to_target_kern->set_arg(nb_bot_cl.ptr());
   map_to_target_kern->set_arg(output_mog_cl.ptr());
   map_to_target_kern->set_arg(output_color_cl.ptr());
#endif
   map_to_target_kern->set_local_arg(local_threads[0]*10*sizeof(cl_uchar) );    // cumsum buffer,
   map_to_target_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees target
   map_to_target_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees source
   map_to_target_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // reflected local trees source
   map_to_target_kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // neighbor trees

   if(!map_to_target_kern->execute(queue_, 1, local_threads, global_threads))
     {
       std::cout<<"Kernel Failed to Execute "<<std::endl;
       return false;
    }

   target_alpha_base_->read_to_buffer(queue_);
   target_color_base_->read_to_buffer(queue_);
   target_vis_base_->read_to_buffer(queue_);
   target_app_base_->read_to_buffer(queue_);
#ifdef DEBUG_CL
   nb_top_cl->read_to_buffer(queue_);
   nb_bot_cl->read_to_buffer(queue_);
   output_mog_cl->read_to_buffer(queue_);
   output_color_cl->read_to_buffer(queue_);
   output_cl->read_to_buffer(queue_);
#endif

   int status = clFinish(queue_);
   bool good_kern = check_val(status, CL_SUCCESS, "ORBIT MAP TO TARGET KERNEL FAILED: " + error_to_string(status));
   map_to_target_kern->clear_args();

   if(!good_kern)
     return false;
#ifdef DEBUG_CL
   // for (unsigned i = 0;i<source_data_size;i++){
   //   if( output[i]!=0 && output_mog[i*8]!=0)
   //     std::cout<<output[i]<<" ";
   // }
   bool no_sphere =true;
   for(unsigned i=0,k=0;i<source_data_size  ;i+=1,k+=1){
     if(eyelid_->data()[i] && !is_right_ && output_color[4*i]>200){
       no_sphere = false;
       vgl_point_3d<double> p_src(nb_bot[i * 16+ 3],nb_bot[i * 16+ 7],nb_bot[i * 16+ 11]);
       vgl_point_3d<double> points[8];

       if(output_mog[i*8]!= output_mog[i*8+1] ){
         double params[8];

         for(unsigned j=0;j<4;j+=1){
           points[j].set( nb_bot[i * 16 + j*4 ], nb_bot[i * 16 + j*4 +1],  nb_bot[i * 16 + j*4 +2]);
           params[j] =output_mog[k*8+j];
           std::cout<< points[j]<<" === "<<output_mog[k*8+j]<<std::endl;

         }
         for(unsigned j=0;j<4;j+=1){
           points[4+j].set( nb_top[i * 16 + j*4 ], nb_top[i * 16 + j*4 +1],  nb_top[i * 16 + j*4 +2]);
           params[4+j] =output_mog[k*8+4+j];
           std::cout<< points[4+j]<<" === "<<output_mog[k*8+4+j]<<std::endl;
         }
         double val = boxm2_vecf_articulated_scene::interp_generic_double(points,params,p_src);
         std::cout<<"****************** "<<output[i]<<" at "<< p_src <<" versus " <<val<< std::endl;
         std::cout<<"++++++++++++++++++ "<<"color val  at "<< p_src <<output_color[4*i]<<" "<<output_color[4*i+1]<<" "<<output_color[4*i+2]<<" "<<std::endl;
         if(fabs(val- output[i])>0.1)
           std::cout<<"DANGER "<<val<<" "<<output[i]<<std::endl;
       }
     }
   }
   if(no_sphere)
     std::cout<<"no spheres have been found!"<<std::endl;


   delete [] output;
   delete [] output_mog;
   delete [] output_color;
   delete [] nb_bot;
   delete [] nb_top;
#endif
   return true;

 }
