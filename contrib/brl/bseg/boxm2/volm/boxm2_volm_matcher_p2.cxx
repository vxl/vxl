// This is brl/bseg/boxm2/volm/boxm2_volm_matcher_p2.cxx
#include "boxm2_volm_matcher_p2.h"
#include "boxm2_volm_wr3db_index.h"
#include <vul/vul_timer.h>


boxm2_volm_matcher_p2::~boxm2_volm_matcher_p2()
{
  delete                    n_cam_;
  delete                    n_obj_;
  delete []           grd_id_buff_;
  delete []       grd_offset_buff_;
  delete []         grd_nlcd_buff_;
  delete          grd_orient_buff_;
  delete          grd_weight_buff_;

  delete []           sky_id_buff_;
  delete []       sky_offset_buff_;
  delete          sky_orient_buff_;
  delete          sky_weight_buff_;
  
  delete []           obj_id_buff_;
  delete []       obj_offset_buff_;
  delete []       obj_orient_buff_;
  delete []         obj_nlcd_buff_;
  delete []       obj_weight_buff_;
}

bool boxm2_volm_matcher_p2::clean_query_cl_mem()
{
  delete             n_cam_cl_mem_;  
  delete             n_obj_cl_mem_;
  delete            grd_id_cl_mem_;
  delete        grd_offset_cl_mem_;
  delete          grd_nlcd_cl_mem_;
  delete        grd_orient_cl_mem_;
  delete        grd_weight_cl_mem_;
  delete            sky_id_cl_mem_;
  delete        sky_offset_cl_mem_;
  delete        sky_orient_cl_mem_;
  delete        sky_weight_cl_mem_;
  delete            obj_id_cl_mem_;
  delete        obj_offset_cl_mem_;
  delete        obj_orient_cl_mem_;
  delete          obj_nlcd_cl_mem_;
  delete        obj_weight_cl_mem_;
  return true;
}

bool boxm2_volm_matcher_p2::volm_matcher_p2()
{
  // need to modified later to read camera from p1 p2 reduced space
  n_cam_ = new unsigned;
  *n_cam_ = (unsigned)query_->get_cam_num();
  n_cam_cl_mem_ = new bocl_mem(gpu_->context(), n_cam_, sizeof(unsigned), " n_cam " );
  if (!n_cam_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed from N_CAM" << vcl_endl;
    delete n_cam_cl_mem_;
    return false;
  }

  n_obj_ = new unsigned;
  *n_obj_ = (unsigned)(query_->depth_regions()).size();
  n_obj_cl_mem_ = new bocl_mem(gpu_->context(), n_obj_, sizeof(unsigned), " n_obj " );
  if (!n_obj_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for N_OBJ" << vcl_endl;
    delete n_cam_cl_mem_;  delete n_obj_cl_mem_;
    return false;
  }

  // construct query array
  query_global_mem_ = 0;
  query_local_mem_ = 0;
  vul_timer transfer_query_time;
  if (!this->transfer_query()) {
    vcl_cerr << "\n ERROR: creating query array failed" << vcl_endl;
    return false;
  }
  vcl_cout << "\t Creating query for Pass 2 matcher -------> \t" 
           << transfer_query_time.all()/1000.0 << " seconds." << vcl_endl;

  // create queue
  if (!this->create_queue()) {
    vcl_cerr << "\n ERROR: creating opencl queue failed" << vcl_endl;
    this->clean_query_cl_mem();
    return false;
  }

  // compile kernel
  vcl_string identifier = gpu_->device_identifier();
  if (kernels_.find(identifier) == kernels_.end()) {
    //vcl_cout << "\t Comipling kernels for device " << identifier << vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    if (!this->compile_kernel(ks)) {
      vcl_cerr << "\n ERROR: compiling matcher kernel failed." << vcl_endl;
      this->clean_query_cl_mem();
      return false;
    }
    kernels_[identifier] = ks;
  }

  // calculate numbers of indices that are able to run per kernel based on available memory

  // Start the matcher for all indices


  return true;
}

bool boxm2_volm_matcher_p2::fill_index()
{
  return true;
}

bool boxm2_volm_matcher_p2::create_queue()
{
  bocl_device_sptr device = gpu_;
  cl_int status = SDK_FAILURE;
  queue_ = clCreateCommandQueue(gpu_->context(), *(gpu_->device_id()),
                                CL_QUEUE_PROFILING_ENABLE, &status);
  if (!check_val(status, CL_SUCCESS, error_to_string(status)))
    return false;
  return true;
}

bool boxm2_volm_matcher_p2::compile_kernel(vcl_vector<bocl_kernel*>& vec_kernels)
{
  return true;
}

bool execute_matcher_kernel()
{
  return true;
}

bool boxm2_volm_matcher_p2::transfer_query()
{
  // ground id, offset, orientation, nlcd, weight
  vcl_vector<vcl_vector<unsigned> >& grd_id = query_->ground_id();
  vcl_vector<vcl_vector<unsigned char> >& grd_land = query_->ground_land_id();
  vcl_vector<unsigned>& grd_offset = query_->ground_offset();
  unsigned grd_vox_size = query_->get_ground_id_size();
  grd_id_buff_ = new unsigned[grd_vox_size];
  grd_offset_buff_ = new unsigned[grd_offset.size()];
  grd_nlcd_buff_ = new unsigned char[grd_vox_size];
  grd_orient_buff_ = new unsigned char;
  grd_weight_buff_ = new float;
  
  *grd_orient_buff_ = query_->ground_orient();
  *grd_weight_buff_ = query_->grd_weight();
  unsigned id = 0;
  for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
    unsigned n_vox = grd_id[cam_id].size();
    for (unsigned vox_id = 0; vox_id < n_vox; vox_id++) {
      grd_id_buff_[id] = grd_id[cam_id][vox_id];
      grd_nlcd_buff_[id++] = grd_land[cam_id][vox_id];
    }
  }
  for (unsigned i = 0; i < grd_offset.size(); i++)
    grd_offset_buff_[i] = grd_offset[i];

  grd_id_cl_mem_ = new bocl_mem(gpu_->context(), grd_id_buff_, sizeof(unsigned)*grd_vox_size, " grd_id " );
  if (!grd_id_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for GRD_ID_CL_MEM" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;
    return false;
  }
  grd_nlcd_cl_mem_ = new bocl_mem(gpu_->context(), grd_nlcd_buff_, sizeof(unsigned char)*grd_vox_size, " grd_nlcd ");
  if (!grd_nlcd_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for GRD_NLCD_CL_MEM" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;     delete grd_nlcd_cl_mem_;
    return false;
  }
  grd_offset_cl_mem_ = new bocl_mem(gpu_->context(), grd_offset_buff_, sizeof(unsigned)*grd_offset.size(), " grd_offset ");
  if (!grd_offset_cl_mem_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for GRD_OFFSET_CL_MEM" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;     delete grd_nlcd_cl_mem_;      delete grd_offset_cl_mem_;
    return false;
  }
  grd_orient_cl_mem_ = new bocl_mem(gpu_->context(), grd_orient_buff_, sizeof(unsigned char), " grd_orient ");
  if (!grd_orient_cl_mem_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for GRD_ORIENT_CL_MEM" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;     delete grd_nlcd_cl_mem_;      delete grd_offset_cl_mem_;     delete grd_orient_cl_mem_;
    return false;
  }
  grd_weight_cl_mem_ = new bocl_mem(gpu_->context(), grd_weight_buff_, sizeof(float), " grd_weight ");
  if (!grd_weight_cl_mem_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for GRD_WEIGHT_CL_MEM" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;     delete grd_nlcd_cl_mem_;      delete grd_offset_cl_mem_;     delete grd_orient_cl_mem_;    delete grd_weight_cl_mem_;
    return false;
  }

  // sky id, offset, orientation, 
  vcl_vector<vcl_vector<unsigned> >& sky_id = query_->sky_id();
  vcl_vector<unsigned>& sky_offset = query_->sky_offset();
  unsigned sky_vox_size = query_->get_sky_id_size();
  sky_id_buff_ = new unsigned[sky_vox_size];
  sky_offset_buff_ = new unsigned[sky_offset.size()];
  sky_orient_buff_ = new unsigned char;
  sky_weight_buff_ = new float;

  *sky_orient_buff_ = query_->sky_orient();
  *sky_weight_buff_ = query_->sky_weight();
  id = 0;
  for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
    unsigned n_vox = sky_id[cam_id].size();
    
    for (unsigned vox_id = 0; vox_id < n_vox; vox_id++) {
      sky_id_buff_[id++] = sky_id[cam_id][vox_id];
    }
  }
  for (unsigned i = 0; i < sky_offset.size(); i++)
    sky_offset_buff_[i] = sky_offset[i];
  
  sky_id_cl_mem_ = new bocl_mem(gpu_->context(), sky_id_buff_, sizeof(unsigned)*sky_vox_size, " sky_id ");
  if (!sky_id_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for SKY_ID" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;     delete grd_nlcd_cl_mem_;      delete grd_offset_cl_mem_;     delete grd_orient_cl_mem_;    delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;
    return false;
  }
  sky_offset_cl_mem_ = new bocl_mem(gpu_->context(), sky_offset_buff_, sizeof(unsigned)*sky_offset.size(), " sky_offset ");
  if (!sky_offset_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for SKY_OFFSET" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;     delete grd_nlcd_cl_mem_;      delete grd_offset_cl_mem_;     delete grd_orient_cl_mem_;    delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;     delete sky_offset_cl_mem_;
    return false;
  }
  sky_orient_cl_mem_ = new bocl_mem(gpu_->context(), sky_orient_buff_, sizeof(unsigned char), " sky_orient ");
  if (!sky_orient_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for SKY_ORIENT" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;     delete grd_nlcd_cl_mem_;      delete grd_offset_cl_mem_;     delete grd_orient_cl_mem_;    delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;     delete sky_offset_cl_mem_;    delete sky_orient_cl_mem_;
    return false;
  }
  sky_weight_cl_mem_ = new bocl_mem(gpu_->context(), sky_weight_buff_, sizeof(float), " sky_weight ");
  if (!sky_weight_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for SKY_WEIGHT" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;     delete grd_nlcd_cl_mem_;      delete grd_offset_cl_mem_;     delete grd_orient_cl_mem_;    delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;     delete sky_offset_cl_mem_;    delete sky_orient_cl_mem_;     delete sky_weight_cl_mem_;
    return false;
  }

  // object id, offset, nlcd, 
  vcl_vector<vcl_vector<vcl_vector<unsigned> > >& obj_id_vec = query_->dist_id();
  vcl_vector<unsigned>& obj_offset = query_->dist_offset();
  vcl_vector<float> & obj_weight = query_->obj_weight();
  vcl_vector<unsigned char>& obj_orient = query_->obj_orient();
  vcl_vector<unsigned char>& obj_land = query_->obj_land_id();
  unsigned obj_vox_size = query_->get_dist_id_size();
  obj_id_buff_ = new unsigned[obj_vox_size];
  obj_offset_buff_ = new unsigned[obj_offset.size()];
  obj_orient_buff_ = new unsigned char[*n_obj_];
  obj_nlcd_buff_ = new unsigned char[*n_obj_];
  obj_weight_buff_ = new float[*n_obj_];
  id = 0;
  for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++)
    for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++)
      for (unsigned vox_id = 0; vox_id < obj_id_vec[cam_id][obj_id].size(); vox_id++)
        obj_id_buff_[id++] = obj_id_vec[cam_id][obj_id][vox_id];

  for (unsigned i = 0; i < obj_offset.size(); i++)
    obj_offset_buff_[i] = obj_offset[i];

  for (unsigned i = 0; i < *n_obj_; i++) {
    obj_orient_buff_[i] = obj_orient[i];
    obj_weight_buff_[i] = obj_weight[i];
    obj_nlcd_buff_[i] = obj_land[i];
  }

  obj_id_cl_mem_ = new bocl_mem(gpu_->context(), obj_id_buff_, sizeof(unsigned)*obj_vox_size, " obj_id ");
  if (!obj_id_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for OBJ_ID" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;     delete grd_nlcd_cl_mem_;      delete grd_offset_cl_mem_;     delete grd_orient_cl_mem_;    delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;     delete sky_offset_cl_mem_;    delete sky_orient_cl_mem_;     delete sky_weight_cl_mem_;
    delete obj_id_cl_mem_;
    return false;
  }
  obj_offset_cl_mem_ = new bocl_mem(gpu_->context(), obj_offset_buff_, sizeof(unsigned)*obj_offset.size(), " obj_ffset ");
  if (!obj_offset_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for OBJ_OFFSET" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;     delete grd_nlcd_cl_mem_;      delete grd_offset_cl_mem_;     delete grd_orient_cl_mem_;    delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;     delete sky_offset_cl_mem_;    delete sky_orient_cl_mem_;     delete sky_weight_cl_mem_;
    delete obj_id_cl_mem_;     delete obj_offset_cl_mem_;
    return false;
  }
  obj_orient_cl_mem_ = new bocl_mem(gpu_->context(), obj_orient_buff_, sizeof(unsigned char)*(*n_obj_), " obj_orient ");
  if (!obj_orient_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for OBJ_ORIENT" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;     delete grd_nlcd_cl_mem_;      delete grd_offset_cl_mem_;     delete grd_orient_cl_mem_;    delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;     delete sky_offset_cl_mem_;    delete sky_orient_cl_mem_;     delete sky_weight_cl_mem_;
    delete obj_id_cl_mem_;     delete obj_offset_cl_mem_;    delete obj_orient_cl_mem_;
    return false;
  }
  obj_nlcd_cl_mem_ = new bocl_mem(gpu_->context(), obj_nlcd_buff_, sizeof(unsigned char)*(*n_obj_), " obj_nlcd ");
  if (!obj_nlcd_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for OBJ_NLCD" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;     delete grd_nlcd_cl_mem_;      delete grd_offset_cl_mem_;     delete grd_orient_cl_mem_;    delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;     delete sky_offset_cl_mem_;    delete sky_orient_cl_mem_;     delete sky_weight_cl_mem_;
    delete obj_id_cl_mem_;     delete obj_offset_cl_mem_;    delete obj_orient_cl_mem_;     delete obj_nlcd_cl_mem_;
    return false;
  }
  obj_weight_cl_mem_ = new bocl_mem(gpu_->context(), obj_weight_buff_, sizeof(float)*(*n_obj_), " obj_weight ");
  if (!obj_weight_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating cl_mem failed for OBJ_WEIGHT" << vcl_endl;
    delete n_cam_cl_mem_;      delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;     delete grd_nlcd_cl_mem_;      delete grd_offset_cl_mem_;     delete grd_orient_cl_mem_;    delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;     delete sky_offset_cl_mem_;    delete sky_orient_cl_mem_;     delete sky_weight_cl_mem_;
    delete obj_id_cl_mem_;     delete obj_offset_cl_mem_;    delete obj_orient_cl_mem_;     delete obj_nlcd_cl_mem_;      delete obj_weight_cl_mem_;
    return false;
  }

#if 0
  vcl_cout << " -------- CHECK the 1D ground buffer ------ " << vcl_endl;
  for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
    unsigned n_vox = grd_id[cam_id].size();
    for (unsigned vox_id = 0; vox_id < n_vox; vox_id++)
      vcl_cout << " cam_id = " << cam_id 
               << ", grd_id = " << grd_id[cam_id][vox_id]
               << ", grd_nlcd = " << (int)grd_nlcd[cam_id][vox_id]
               << vcl_endl;
  }
  vcl_cout << " ------------- 1D ground buffer --------- " << vcl_endl;
  for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
    unsigned start = grd_offset_buff_[cam_id];
    unsigned end = grd_offset_buff_[cam_id+1];
    for (unsigned i = start; i < end; i++) 
      vcl_cout << " cam_id = " << cam_id
               << ", grd_id_buff = " << grd_id_buff_[i]
               << ", grd_nlcd_buff = " << (int)grd_nlcd_buff_[i]
               << vcl_endl;
  }
#endif
#if 0
  vcl_cout << " -------- CHECK the sky vector ------ " << vcl_endl;
  for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
    unsigned n_vox = sky_id[cam_id].size();
    for (unsigned vox_id = 0; vox_id < n_vox; vox_id++)
      vcl_cout << " cam_id = " << cam_id 
               << ", sky_id = " << sky_id[cam_id][vox_id]
               << ", sky_orient = " << (int)query_->sky_orient()
               << vcl_endl;
  }
  vcl_cout << " ------------- 1D sky buffer --------- " << vcl_endl;
  for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
    unsigned start = sky_offset_buff_[cam_id];
    unsigned end = sky_offset_buff_[cam_id+1];
    for (unsigned i = start; i < end; i++)
      vcl_cout << " cam_id = " << cam_id
               << ", sky_id_buff = " << sky_id_buff_[i]
               << ", sky_orient_buff = " << (int)*sky_orient_buff_
               << vcl_endl;
  }
#endif
#if 0
  vcl_cout << " ------------ CHECK the object vector -----" << vcl_endl;
  for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++)
    for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++)
      for (unsigned vox_id = 0; vox_id < obj_id_vec[cam_id][obj_id].size(); vox_id++)
        vcl_cout << " cam = " << cam_id
                 << ", obj = " << obj_id
                 << ", id = " << obj_id_vec[cam_id][obj_id][vox_id]
                 << ", nlcd = " << (int)obj_nlcd[obj_id]
                 << ", orient = " << (int)obj_orient[obj_id]
                 << ", weight = " << obj_weight[obj_id]
                 << vcl_endl;
  vcl_cout << " ----------------  the object buffer ------" << vcl_endl;
  for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
    for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++) {
      unsigned offset_id = obj_id + cam_id * (*n_obj_);
      unsigned start = obj_offset_buff_[offset_id];
      unsigned end = obj_offset_buff_[offset_id+1];
      for (unsigned i = start; i < end; i++)
        vcl_cout << " cam = " << cam_id
                 << ", obj = " << obj_id
                 << ", id = " << obj_id_buff_[i]
                 << ", nlcd = " << (int)obj_nlcd_buff_[obj_id]
                 << ", orient = " << (int)obj_orient_buff_[obj_id]
                 << ", weight = " << obj_weight_buff_[obj_id]
                 << vcl_endl;
    }
  }
#endif

  return true;
}

