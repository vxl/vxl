// This is brl/bseg/boxm2/volm/boxm2_volm_matcher_p1.cxx
#include "boxm2_volm_matcher_p1.h"
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <vcl_algorithm.h>
#include <vcl_iomanip.h>

#define GBYTE 1073741824


boxm2_volm_matcher_p1::boxm2_volm_matcher_p1(volm_query_sptr const& query,
                                             vcl_vector<volm_geo_index_node_sptr> const& leaves,
                                             float const& buffer_capacity,
                                             vcl_string const& geo_index_folder,
                                             unsigned const& tile_id,
                                             vcl_vector<float> const& depth_interval,
                                             vgl_polygon<double> const& cand_poly,
                                             bocl_device_sptr gpu,
                                             bool const& is_candidate,
                                             bool const& is_last_pass,
                                             vcl_string const& out_folder,
                                             float const& threshold,
                                             unsigned const& max_cam_per_loc,
                                             bool const& use_orient) :
  query_(query), leaves_(leaves), depth_interval_(depth_interval), ind_buffer_(buffer_capacity),
  cand_poly_(cand_poly), gpu_(gpu), is_candidate_(is_candidate), is_last_pass_(is_last_pass),
  out_folder_(out_folder), threshold_(threshold), max_cam_per_loc_(max_cam_per_loc), use_orient_(use_orient)
{ 
    layer_size_ = query_->get_query_size(); 
    ind_ = new boxm2_volm_wr3db_index(layer_size_, ind_buffer_);
    ind_orient_ = new boxm2_volm_wr3db_index(layer_size_, ind_buffer_);
    file_name_pre_ << geo_index_folder << "geo_index_tile_" << tile_id;
    // initialize the pionters
    n_cam_ = 0;
    n_obj_ = 0;
    layer_size_buff_ = 0;
    depth_length_buff_ = 0;
    grd_id_buff_ = 0;
    grd_dist_buff_ = 0;
    grd_id_offset_buff_ = 0;
    grd_weight_buff_ = 0;
    sky_id_buff_ = 0;
    sky_id_offset_buff_ = 0;
    sky_weight_buff_ = 0;
    obj_id_buff_ = 0;
    obj_id_offset_buff_ = 0;
    obj_min_dist_buff_ = 0;
    obj_order_buff_ = 0;
    obj_weight_buff_ = 0;
    obj_orient_buff_ = 0;
    depth_interval_buff_ = 0;
    depth_length_buff_ = 0;
    is_grd_reg_ = true;
    is_sky_reg_ = true;
    is_obj_reg_ = true;
    if (!query_->depth_scene()->ground_plane().size())
      is_grd_reg_ = false;
    if (!query_->depth_scene()->sky().size())
      is_sky_reg_ = false;
    if (!query_->depth_regions().size())
      is_obj_reg_ = false;
}

boxm2_volm_matcher_p1::~boxm2_volm_matcher_p1()
{
  if (n_cam_)               delete                  n_cam_;
  if (n_obj_)               delete                  n_obj_;
  if (layer_size_buff_)     delete        layer_size_buff_;
  if (depth_length_buff_)   delete      depth_length_buff_;
  if (grd_id_buff_)         delete []         grd_id_buff_;
  if (grd_dist_buff_)       delete []       grd_dist_buff_;
  if (grd_id_offset_buff_)  delete []  grd_id_offset_buff_;
  if (grd_weight_buff_)     delete        grd_weight_buff_;
  if (sky_id_buff_)         delete []         sky_id_buff_;
  if (sky_id_offset_buff_)  delete []  sky_id_offset_buff_;
  if (sky_weight_buff_)     delete        sky_weight_buff_;
  if (obj_id_buff_)         delete []         obj_id_buff_;
  if (obj_id_offset_buff_)  delete []  obj_id_offset_buff_;
  if (obj_min_dist_buff_)   delete []   obj_min_dist_buff_;
  if (obj_orient_buff_)     delete []     obj_orient_buff_;
  if (obj_order_buff_)      delete []      obj_order_buff_;
  if (obj_weight_buff_)     delete []     obj_weight_buff_;
  if (depth_interval_buff_) delete [] depth_interval_buff_;
}

bool boxm2_volm_matcher_p1::volm_matcher_p1()
{
  n_cam_ = new unsigned;
  *n_cam_ = (unsigned)query_->get_cam_num();
  n_cam_cl_mem_ = new bocl_mem(gpu_->context(), n_cam_, sizeof(unsigned), " n_cam " );
  if (!n_cam_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for N_CAM" << vcl_endl;
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
  unsigned nc = *n_cam_;
  unsigned no = *n_obj_;
  // transfer all queries information to 1D array structure
  query_global_mem_ = 0;
  query_local_mem_ = 0;
  vul_timer trans_query_time;
  if (!this->transfer_query()) {
    vcl_cerr << "\n ERROR: transfering query to 1D structure failed." << vcl_endl;
    return false;
  }

#if 1
  if (use_orient_) {
    vcl_cout << " we are using orientation attributes as follows " << vcl_endl;
    // check the transferred orientation
    vcl_vector<unsigned char>& obj_orient = query_->obj_orient();
    for (unsigned i = 0; i < *n_obj_; i++) {
      vcl_cout << " i = " << i << ", query_orient = "  << (int)obj_orient[i]
               << ", orient_buff = " << (int)obj_orient_buff_[i] << vcl_endl; 
    }
    vcl_cout << " The orient cl_mem_ has size " << obj_orient_cl_mem_->num_bytes() << vcl_endl;
    vcl_cout << " Weight parameters looks like" << vcl_endl;
    if(is_sky_reg_) 
      vcl_cout << " sky_weight = " << *sky_weight_buff_ << vcl_endl;
    else
      vcl_cout << " no sky region for this query " << vcl_endl;
    if(is_grd_reg_)
      vcl_cout << " grd_weight = " << *grd_weight_buff_ << vcl_endl;
    else
      vcl_cout << " no grd region for this query " << vcl_endl;
    if(is_obj_reg_) {
      for (unsigned i = 0; i < *n_obj_; i++) {
        vcl_cout << " i = " << i << ", weight = " << obj_weight_buff_[i] << vcl_endl;
      }
    }
  }
#endif


  vcl_cout << "\t 4.1.1 Setting up query in pass 1 matcher for GPU ------> \t" << trans_query_time.all()/1000.0 << " seconds." << vcl_endl;
  
  // create queue
  if (!this->create_queue()) {
    vcl_cerr << "\n ERROR: creating pass 1 matcher queue failed." << vcl_endl;
    return false;
  }

  // create depth_interval 
  depth_interval_buff_ = new float[depth_interval_.size()];
  for (unsigned i = 0; i < depth_interval_.size(); i++)
    depth_interval_buff_[i] = depth_interval_[i];

  depth_interval_cl_mem_ = new bocl_mem(gpu_->context(), depth_interval_buff_, sizeof(float)*(depth_interval_.size()), " depth_interval ");
  if(!depth_interval_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for DEPTH_INTERVAL" << vcl_endl;
    this->clean_query_cl_mem();
    delete depth_interval_cl_mem_;
    return false;
  }
  depth_length_buff_ = new unsigned;
  *depth_length_buff_ = depth_interval_.size();
  depth_length_cl_mem_ = new bocl_mem(gpu_->context(), depth_length_buff_, sizeof(unsigned), " depth_length ");
  if (!depth_length_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for DEPTH_LENGTH" << vcl_endl;
    this->clean_query_cl_mem();
    delete depth_interval_cl_mem_;    delete depth_length_cl_mem_;
    return false;
  }
  // compile the kernel
  vcl_string identifier = gpu_->device_identifier();
  if (kernels_.find(identifier) == kernels_.end()) {
    vcl_cout << "\t 4.1.2 Comipling kernels for device " << identifier << vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    if (!this->compile_kernel(ks)) {
      vcl_cerr << "\n ERROR: compiling matcher kernel failed." << vcl_endl;
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;    delete depth_length_cl_mem_;
      return false;
    }
    kernels_[identifier] = ks;
  }

  // calculate available memory space for indices
  cl_ulong avail_global_mem = device_global_mem_ - query_global_mem_ - sizeof(float)*(depth_interval_.size());
  cl_ulong extra_global_mem = (cl_ulong)(1.5*GBYTE);  // leave extra 1.5 GB space for kernel to run
  if (avail_global_mem < extra_global_mem) {
    vcl_cerr << "\n ERROR: available memory is smaller than pre-defined extra memory, reduce the extra memory space (current value = "
             << extra_global_mem / GBYTE  << ')' << vcl_endl;
    this->clean_query_cl_mem();
    delete depth_interval_cl_mem_;    delete depth_length_cl_mem_;
    return false;
  }
  cl_ulong index_global_mem = avail_global_mem - extra_global_mem;  // in byte

  // note that for each index, we require space for 
  // a float score array with length n_cam
  // a float mean value array with length n_cam*n_obj
  // an uchar index array with length layer_size
  cl_ulong per_index_mem = nc*no*sizeof(float) + nc*sizeof(float) + sizeof(unsigned char)*layer_size_;
  if (index_global_mem < per_index_mem) {
    this->clean_query_cl_mem();
    delete depth_interval_cl_mem_;    delete depth_length_cl_mem_;
    vcl_cerr << "\n ERROR: available memory can not take a single index, reduce the extra memory space (current value = "
             << extra_global_mem / GBYTE  << ')' << vcl_endl; 
    return false;
  }
  unsigned ni = index_global_mem / per_index_mem;

  // create cl_mem for layer_size
  layer_size_buff_ = new unsigned;
  *layer_size_buff_ = layer_size_;
  layer_size_cl_mem_ = new bocl_mem(gpu_->context(), layer_size_buff_, sizeof(unsigned), " layer_size ");
  if (!layer_size_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for LAYER_SIZE" << vcl_endl;
    this->clean_query_cl_mem();
    delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
    delete layer_size_cl_mem_;
    return false;
  }

  // hack here for debug purpose
  if (use_orient_)
    ni = 32;  // since we double the index size, including index_orient and index_depth
  else
    ni = 64;
  
  vcl_cout << "\t 4.1.3: device have total " << device_global_mem_ << " Byte (" << (float)device_global_mem_/(float)GBYTE << " GB) memory space\n"
           << "\t        query requres " << query_global_mem_ << " Byte (" << (float)query_global_mem_/(float)GBYTE << " GB)\n"
           << "\t        leave " << extra_global_mem  << " Byte (" << (float)extra_global_mem/(float)GBYTE << " GB) extra empty space on device\n"
           << "\t        a single index require " << per_index_mem << " Byte given " << nc << " cameras and " << no << " objects\n"
           << "\t        ---> kernel can calcualte " << ni << " indices per lunching " << vcl_endl;

  // define the work group size and NDRange dimenstion
  work_dim_ = 2;
  local_threads_[0] = 8;
  local_threads_[1] = 8;
  // note the global work goupe size is defined inside the loop since we may have different number of
  // indices passed into buffer

  // -----------------------------------------------------------
  // start loop over all indices
  unsigned leaf_id = 0;
  unsigned round_cnt = 0;
  float gpu_matcher_time = 0.0f;
  vul_timer total_matcher_time;
  cl_int status;
  cl_ulong total_index_num = 0;
  
  vcl_string index_file = leaves_[leaf_id]->get_index_name(file_name_pre_.str());
  vcl_string index_orient_file = leaves_[leaf_id]->get_label_index_name(file_name_pre_.str(), "orientation");
  ind_->initialize_read(index_file);
  ind_orient_->initialize_read(index_orient_file);

  while (leaf_id < leaves_.size()) {
    unsigned char* index_buff_ = new unsigned char[ni*layer_size_];
    unsigned char* index_orient_buff_ = 0;
    

    // fill the index buffer
    vcl_vector<unsigned> l_id;  // leaf_id for indices filled into buffer
    vcl_vector<unsigned> h_id;  // hypo_id in this leaf_id for indices filled into buffer
    unsigned actual_n_ind = ni; // handling the last round where the number of loaded indices is smaller than pre-computed ni
    
    if (use_orient_) {
      index_orient_buff_ = new unsigned char[ni*layer_size_];
      if (!this->fill_index_orient(ni, layer_size_, leaf_id, index_buff_, index_orient_buff_, l_id, h_id, actual_n_ind) ) {
        this->clean_query_cl_mem();
        delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
        delete layer_size_cl_mem_;
        delete [] index_buff_;
        if(index_orient_buff_) delete [] index_orient_buff_;
        return false;
      }
    }
    else {
      if (!this->fill_index(ni, layer_size_, leaf_id, index_buff_, l_id, h_id, actual_n_ind) ) {
        vcl_cerr << "\n ERROR: passing index into index buffer failed for " << leaf_id << " leaf" << vcl_endl;
        this->clean_query_cl_mem();
        delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
        delete layer_size_cl_mem_;
        delete [] index_buff_;
        if (use_orient_) { delete [] index_orient_buff_; }
        return false;
      }
    }

    // resize the index buffer if actual loaded index size is smaller than pre-defined
    if (actual_n_ind != ni)
      ni = actual_n_ind;
    if (ni == 0) {
      continue;
    }

    total_index_num += ni;
    float* score_buff_ = new float[ni*nc];
    float*    mu_buff_ = new float[ni*no*nc];
    unsigned* n_ind_ = new unsigned;
    *n_ind_ = ni;
    bocl_mem* n_ind_cl_mem_ = new bocl_mem(gpu_->context(), n_ind_, sizeof(unsigned), " n_ind ");
    if (!n_ind_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for N_IND" << vcl_endl;
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
      delete layer_size_cl_mem_;
      delete n_ind_cl_mem_;     delete [] n_ind_;
      delete [] index_buff_;    delete [] score_buff_;    delete [] mu_buff_;
      if (use_orient_) { delete [] index_orient_buff_; }
      return false;
    }

    // define work group size based on number of cameras, nc,  and number of indices, ni;
    cl_ulong cl_ni = (cl_ulong)RoundUp(*n_ind_, (int)local_threads_[0]);   // row is index
    cl_ulong cl_nj = (cl_ulong)RoundUp(*n_cam_, (int)local_threads_[1]);   // col is camera
    global_threads_[0] = cl_ni;
    global_threads_[1] = cl_nj;

    vcl_cout << " --------  in round " << round_cnt++ << " ------ " << vcl_endl;
    vcl_cout << " Giving " << nc << " camera hypos per location and " << ni << " locations pre lunching\n";
    vcl_cout << " NDRange stucture:\n";
    vcl_cout << " \t dimension = " << work_dim_ << vcl_endl;
    vcl_cout << " \t work group size = (" << local_threads_[0] << ", " << local_threads_[1] << ")\n";
    vcl_cout << " \t number of work item =  (" << global_threads_[0] << ", " << global_threads_[1] << ")\n";
    vcl_cout << " \t number of work group = (" << global_threads_[0]/local_threads_[0]
             << ", " << global_threads_[1]/local_threads_[1] << ')' << vcl_endl;

#if 0
    // check loaded indices and associated ids
    vcl_cout << " -------> leaf_id_updated = " << leaf_id << vcl_endl;
    for(unsigned i = 0; i < ni; i++) {
      vcl_cout << " i = " << i << ", leaf_id = " << l_id[i] << " hypo_id = " << h_id[i] << "\nindex_depth\t";
      unsigned start = i*layer_size_;
      unsigned end = (i+1)*layer_size_;
      for (unsigned j = start; j < end; j++)
        vcl_cout << " " << (int)index_buff_[j];
      vcl_cout << "\n\nindex_orient\t";
      if(use_orient_) {
        for (unsigned j = start; j < end; j++)
          vcl_cout << " " << (int)index_orient_buff_[j];
        vcl_cout << '\n';
      }
      
    }
#endif

    // create cl_mem_ for index, score and mu
    // Note: here the data passed into device may be smaller than the pre-assigned index_buff size (since actual_n_ind < pre-calculated ni)
    bocl_mem* index_cl_mem_ = new bocl_mem(gpu_->context(), index_buff_, sizeof(unsigned char)*ni*layer_size_, " index ");
    if (!index_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for INDEX" << vcl_endl;
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
      delete layer_size_cl_mem_;
      delete n_ind_cl_mem_;     delete [] n_ind_;
      delete index_cl_mem_;
      delete [] index_buff_;    delete [] score_buff_;    delete [] mu_buff_;
      if (use_orient_) { delete [] index_orient_buff_; }
      return false;
    }
    // create index orientation if necessary
    bocl_mem* index_orient_cl_mem_ = 0;
    if (use_orient_) {
      index_orient_cl_mem_ = new bocl_mem(gpu_->context(), index_orient_buff_, sizeof(unsigned char)*ni*layer_size_, " index_orient ");
      if (!index_orient_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
        vcl_cerr << "\n ERROR: creating bocl_mem failed for INDEX_ORIENT" << vcl_endl;
        this->clean_query_cl_mem();
        delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
        delete layer_size_cl_mem_;
        delete n_ind_cl_mem_;     delete [] n_ind_;
        delete index_cl_mem_;
        delete [] index_buff_;    delete [] score_buff_;    delete [] mu_buff_;
        if (use_orient_) { delete [] index_orient_buff_; }
        return false;
      }
    }

    bocl_mem* score_cl_mem_ = new bocl_mem(gpu_->context(), score_buff_, sizeof(float)*ni*nc, " score ");
    if (!score_cl_mem_->create_buffer( CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for SCORE" << vcl_endl;
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
      delete layer_size_cl_mem_;
      delete n_ind_cl_mem_;
      delete index_cl_mem_;     delete score_cl_mem_;
      delete [] index_buff_;    delete [] score_buff_;    delete [] mu_buff_;
      if (use_orient_) { delete [] index_orient_buff_; }
      return false;
    }
    bocl_mem* mu_cl_mem_ = new bocl_mem(gpu_->context(), mu_buff_, sizeof(float)*ni*nc*no, " mu ");
    if (!mu_cl_mem_->create_buffer( CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for MU" << vcl_endl;
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
      delete layer_size_cl_mem_;
      delete n_ind_cl_mem_;     delete [] n_ind_;
      delete index_cl_mem_;     delete score_cl_mem_;     delete mu_cl_mem_;
      delete [] index_buff_;    delete [] score_buff_;    delete [] mu_buff_;
      if (use_orient_) { delete [] index_orient_buff_; }
      return false;
    }

    // start the obj_based kernel matcher
    vcl_string identifier = gpu_->device_identifier();
    if (use_orient_) {
      if (!this->execute_matcher_kernel_orient(gpu_, queue_, kernels_[identifier], n_ind_cl_mem_, index_cl_mem_, index_orient_cl_mem_, score_cl_mem_, mu_cl_mem_)) {
        vcl_cerr << "\n ERROR: executing pass 1 kernel(with orientation) failed on device " << identifier << vcl_endl;
        this->clean_query_cl_mem();
        delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
        delete layer_size_cl_mem_;
        delete n_ind_cl_mem_;     delete [] n_ind_;
        delete index_cl_mem_;     delete score_cl_mem_;     delete mu_cl_mem_;
        delete [] index_buff_;    delete [] score_buff_;    delete [] mu_buff_;
        if (use_orient_) { delete [] index_orient_buff_; }
        return false;
      }
    } else {
      if (!this->execute_matcher_kernel(gpu_, queue_, kernels_[identifier], n_ind_cl_mem_, index_cl_mem_, score_cl_mem_, mu_cl_mem_)) {
        vcl_cerr << "\n ERROR: executing pass 1 kernel failed on device " << identifier << vcl_endl;
        this->clean_query_cl_mem();
        delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
        delete layer_size_cl_mem_;
        delete n_ind_cl_mem_;     delete [] n_ind_;
        delete index_cl_mem_;     delete score_cl_mem_;     delete mu_cl_mem_;
        delete [] index_buff_;    delete [] score_buff_;    delete [] mu_buff_;
        if (use_orient_) { delete [] index_orient_buff_; }
        return false;
      }
    }
    // block everything to ensure the reading score
    status = clFinish(queue_);
    // read score
    score_cl_mem_->read_to_buffer(queue_);
    mu_cl_mem_->read_to_buffer(queue_);
    status = clFinish(queue_);
    // count time
    if (use_orient_) {
      if (is_grd_reg_ && is_sky_reg_)
        gpu_matcher_time += kernels_[identifier][4]->exec_time();
      else if (!is_grd_reg_ && is_sky_reg_)
        gpu_matcher_time += kernels_[identifier][5]->exec_time();
      else if (is_grd_reg_ && !is_sky_reg_)
        gpu_matcher_time += kernels_[identifier][6]->exec_time();
      else
        gpu_matcher_time += kernels_[identifier][7]->exec_time();
    } else {
      if (is_grd_reg_ && is_sky_reg_)
        gpu_matcher_time += kernels_[identifier][0]->exec_time();
      else if (!is_grd_reg_ && is_sky_reg_)
        gpu_matcher_time += kernels_[identifier][1]->exec_time();
      else if (is_grd_reg_ && !is_sky_reg_)
        gpu_matcher_time += kernels_[identifier][2]->exec_time();
      else
        gpu_matcher_time += kernels_[identifier][3]->exec_time();
    }
    
    // post-processing data
    for (unsigned ind_id = 0; ind_id < ni; ind_id++) {
      // for each location, find maximum score, top cameras and best camera giving max_score
      float max_score = 0.0f;
      unsigned max_cam_id = 0;
      float min_score_in_list = 0.0f;
      vcl_vector<unsigned> cam_ids;
      vcl_vector<float> cam_scores;
      for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
        unsigned id = cam_id + ind_id*nc;
        float s = score_buff_[id];
        // find the max_score and max_cam_id
        if (s > max_score) {
          max_score = s;  max_cam_id = cam_id;
        }
        // decide to keep the camera or not
        if (s > threshold_) {
          if (cam_ids.size() < max_cam_per_loc_) {
            cam_ids.push_back(cam_id);
            cam_scores.push_back(s);
          } else if (s > min_score_in_list) {
            // check the list and replace the camera with min_score in the list
            min_score_in_list = cam_scores[0];
            unsigned min_score_id = 0;
            for (unsigned jj = 0; jj < cam_ids.size(); jj++) {
              if (min_score_in_list > cam_scores[jj]) {
                min_score_in_list = cam_scores[jj];
                min_score_id = jj;
              }
            }
            // replace the id that have min_score among the camera id list
            cam_scores[min_score_id] = s;
            cam_ids[min_score_id] = cam_id;
          }
        }
      }
      // put score date of location ind_id to score_all
      score_all_.push_back(new volm_score(l_id[ind_id], h_id[ind_id], max_score, max_cam_id, cam_ids));
      score_cam_.push_back(boxm2_volm_score_out(l_id[ind_id], h_id[ind_id], cam_ids, cam_scores));
    }
    
    // clean cl_mem before next round matcher
    delete n_ind_cl_mem_;
    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, " release N_INDEX failed on device " + gpu_->device_identifier() + error_to_string(status));
    delete index_cl_mem_;
    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, " release INDEX failed on device " + gpu_->device_identifier() + error_to_string(status));
    delete score_cl_mem_;
    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, " release SCORE failed on device " + gpu_->device_identifier() + error_to_string(status));
    delete mu_cl_mem_;
    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, " release MU(average depth value) failed on device " + gpu_->device_identifier() + error_to_string(status));
    if (use_orient_) { 
      // delete cl_mem for index orientation
      delete index_orient_cl_mem_;
      status = clFinish(queue_);
      check_val(status, MEM_FAILURE, " release INDEX_ORIENT failed on device " + gpu_->device_identifier() + error_to_string(status));
    }
   
    // do the test to ensure kernel is correct
#if 0
    if (use_orient_)
      this->volm_matcher_p1_test_ori(ni, index_buff_, index_orient_buff_, score_buff_, mu_buff_);
    else
      this->volm_matcher_p1_test(ni, index_buff_, score_buff_, mu_buff_);
#endif

    // finish current round, clean host memory
    delete n_ind_;
    delete [] index_buff_;    delete [] score_buff_;    delete [] mu_buff_;
    if(use_orient_) { delete [] index_orient_buff_; }
  } // end of loop over all leaves
  
  // time
  float total_time = total_matcher_time.all();
  vcl_cout << "\t\t total time for " << total_index_num << " indices and " << *n_cam_ << " cameras -------> " << total_time/1000.0 << " seconds (" << total_time << " ms)\n" ;
  vcl_cout << "\t\t GPU kernel execution ------------------> " << gpu_matcher_time/1000.0 << " seconds (" << gpu_matcher_time << " ms)\n";
  vcl_cout << "\t\t CPU host execution --------------------> " << (total_time - gpu_matcher_time)/1000.0 << " seconds (" << total_time - gpu_matcher_time << " ms)" << vcl_endl;

  // clear query_cl_mem
  this->clean_query_cl_mem();
  delete depth_interval_cl_mem_;        
  delete depth_length_cl_mem_;
  delete layer_size_cl_mem_;
  // finalize the index
  ind_->finalize();
  ind_orient_->finalize();
  return true;
}

bool boxm2_volm_matcher_p1::fill_index(unsigned const& n_ind,
                                       unsigned const& layer_size,
                                       unsigned& leaf_id,
                                       unsigned char* index_buff,
                                       vcl_vector<unsigned>& l_id,
                                       vcl_vector<unsigned>& h_id,
                                       unsigned& actual_n_ind)
{
  if (is_last_pass_) {
    vcl_cerr << " pass 1 check whether we have last_pass is NOT implemented yet ... " << vcl_endl;
    return false;
  } else {
    // for no previous output case
    unsigned cnt = 0;
    unsigned li;
    for (li = leaf_id; li < leaves_.size(); li++) {
      if (!leaves_[li]->hyps_ )
        continue;
      vgl_point_3d<double> h_pt;
      while (cnt < n_ind && leaves_[li]->hyps_->get_next(0,1,h_pt) ) {
        if(is_candidate_) {
          if (cand_poly_.contains(h_pt.x(), h_pt.y())) {  // having candiate list and current hypo is inside it --> accept
            unsigned char* values = index_buff + cnt * layer_size;
            ind_->get_next(values, layer_size);
            cnt++;
            l_id.push_back(li);  h_id.push_back(leaves_[li]->hyps_->current_-1);
          } else {                                       // having candidate list but current hypo is outside candidate list --> ignore
            vcl_vector<unsigned char> values(layer_size);
            ind_->get_next(values);
          }
        } else {                                         // no candidate list, put all indices into buffer
          unsigned char* values = index_buff + cnt * layer_size;
          ind_->get_next(values, layer_size);
          cnt++;
          l_id.push_back(li);  h_id.push_back(leaves_[li]->hyps_->current_-1);
        }
      }
      if (cnt == n_ind) {
        leaf_id = li; 
        break;
      } else {
        if (is_leaf_finish(li)){
          ind_->finalize();
          if(li < leaves_.size()-1)
            ind_->initialize_read(leaves_[li+1]->get_index_name(file_name_pre_.str()));
        }
      }
    } // loop over all leaves
    if(li == leaves_.size())
      leaf_id = li;
    if(cnt != n_ind) 
      actual_n_ind = cnt;
    return true;
  }
}


bool boxm2_volm_matcher_p1::fill_index_orient(unsigned const& n_ind,
                                              unsigned const& layer_size,
                                              unsigned& leaf_id,
                                              unsigned char* index_buff,
                                              unsigned char* index_orient_buff,
                                              vcl_vector<unsigned>& l_id,
                                              vcl_vector<unsigned>& h_id,
                                              unsigned& actual_n_ind)
{
  if (is_last_pass_) {
    vcl_cerr << " pass 1 check whether we have last_pass is NOT implemented yet ... " << vcl_endl;
    return false;
  } else {
    // for no previous output case
    unsigned cnt = 0;
    unsigned li;
    for (li = leaf_id; li < leaves_.size(); li++) {
      if (!leaves_[li]->hyps_ )
        continue;
      vgl_point_3d<double> h_pt;
      while (cnt < n_ind && leaves_[li]->hyps_->get_next(0,1,h_pt) ) {
        if(is_candidate_) {
          if (cand_poly_.contains(h_pt.x(), h_pt.y())) {  // having candiate list and current hypo is inside it --> accept
            unsigned char* values = index_buff + cnt * layer_size;
            unsigned char* values_orient = index_orient_buff + cnt * layer_size;
            ind_->get_next(values, layer_size);
            ind_orient_->get_next(values_orient, layer_size);
            cnt++;
            vcl_cout << " leaf_id = " << li << " hypo_id = " << leaves_[li]->hyps_->current_-1
                     << " h_pt = " << h_pt << vcl_endl;
            l_id.push_back(li);  h_id.push_back(leaves_[li]->hyps_->current_-1);
          } else {                                       // having candidate list but current hypo is outside candidate list --> ignore
            vcl_vector<unsigned char> values(layer_size);
            ind_->get_next(values);
            vcl_vector<unsigned char> values_ori(layer_size);
            ind_orient_->get_next(values_ori);
          }
        } else {                                         // no candidate list, put all indices into buffer
          unsigned char* values = index_buff + cnt * layer_size;
          unsigned char* values_orient = index_orient_buff + cnt * layer_size;
          ind_->get_next(values, layer_size);
          ind_orient_->get_next(values_orient, layer_size);
          cnt++;
          l_id.push_back(li);  h_id.push_back(leaves_[li]->hyps_->current_-1);
        }
      }
      if (cnt == n_ind) {
        leaf_id = li; 
        break;
      } else {
        if (is_leaf_finish(li)){
          ind_->finalize();
          ind_orient_->finalize();
          if(li < leaves_.size()-1) {
            ind_->initialize_read(leaves_[li+1]->get_index_name(file_name_pre_.str()));
            ind_orient_->initialize_read(leaves_[li+1]->get_label_index_name(file_name_pre_.str(), "orientation"));
          }
        }
      }
    } // loop over all leaves
    if(li == leaves_.size())
      leaf_id = li;
    if(cnt != n_ind) 
      actual_n_ind = cnt;
    return true;
  }
}

// check if the hypothesis inside given leaf has been loaded
bool boxm2_volm_matcher_p1::is_leaf_finish(unsigned const& leaf_id)
{
  return ( leaves_[leaf_id]->hyps_->current_ ==  leaves_[leaf_id]->hyps_->size() );
}

// execute kernel
bool boxm2_volm_matcher_p1::execute_matcher_kernel(bocl_device_sptr                         device,
                                                   cl_command_queue&                         queue,
                                                   vcl_vector<bocl_kernel*>               kern_vec,
                                                   bocl_mem*                         n_ind_cl_mem_,
                                                   bocl_mem*                         index_cl_mem_,
                                                   bocl_mem*                         score_cl_mem_,
                                                   bocl_mem*                            mu_cl_mem_)
{
  // create a debug buffer
  cl_int status;
  unsigned debug_size = 2000;
  float* debug_buff_ = new float[debug_size];
  vcl_fill(debug_buff_, debug_buff_+debug_size, (float)12.31);
  bocl_mem* debug_cl_mem_ = new bocl_mem(gpu_->context(), debug_buff_, sizeof(float)*debug_size, " debug ");
  if (!debug_cl_mem_->create_buffer( CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for DEBUG" << vcl_endl;
    delete debug_cl_mem_;     delete [] debug_buff_;
    return false;
  }

  bocl_kernel* kern;
  // choose whether kernel to use
  if (is_grd_reg_ && is_sky_reg_) {              // both sky and ground
    vcl_cout << "\t using both sky and grd kernel" << vcl_endl;
    kern = kern_vec[0];
    // set up argument list
    kern->set_arg(n_cam_cl_mem_);
    kern->set_arg(n_obj_cl_mem_);
    kern->set_arg(grd_id_cl_mem_);        kern->set_arg(grd_id_offset_cl_mem_);    kern->set_arg(grd_dist_cl_mem_);    kern->set_arg(grd_weight_cl_mem_);
    kern->set_arg(sky_id_cl_mem_);        kern->set_arg(sky_id_offset_cl_mem_);    kern->set_arg(sky_weight_cl_mem_);
  } else if ( !is_grd_reg_ && is_sky_reg_) {     // no ground but sky
    vcl_cout << "\t using NO grd kernel" << vcl_endl;
    kern = kern_vec[1];
    // set up argument list
    kern->set_arg(n_cam_cl_mem_);
    kern->set_arg(n_obj_cl_mem_);
    kern->set_arg(sky_id_cl_mem_);        kern->set_arg(sky_id_offset_cl_mem_);    kern->set_arg(sky_weight_cl_mem_);

#if 0
    // check the global memory
    vcl_cerr << " ------------ INSIDE execute_kernel ------------------------------- " << vcl_endl;
    vcl_cerr << " n_cam_cl_mem = " << n_cam_cl_mem_->num_bytes() << vcl_endl;
    vcl_cerr << " n_obj_cl_mem = " << n_obj_cl_mem_->num_bytes() << vcl_endl;
    vcl_cerr << " sky_id_cl_mem_ = " << sky_id_cl_mem_->num_bytes() << vcl_endl;
    vcl_cerr << " sky_offset_cl_mem_ = " << sky_id_offset_cl_mem_->num_bytes() << vcl_endl;
    vcl_cerr << " sky_weight_cl_mem_ = " << sky_weight_cl_mem_->num_bytes() << vcl_endl;
#endif

  } else if (  is_grd_reg_ && !is_sky_reg_) {    // no sky but ground
    vcl_cout << "\t using NO sky kernel" << vcl_endl;
    kern = kern_vec[2];
    // set up argument list
    kern->set_arg(n_cam_cl_mem_);
    kern->set_arg(n_obj_cl_mem_);
    kern->set_arg(grd_id_cl_mem_);        kern->set_arg(grd_id_offset_cl_mem_);    kern->set_arg(grd_dist_cl_mem_);    kern->set_arg(grd_weight_cl_mem_);
  } else {                                       // neight sky nor ground
    vcl_cout << "\t using NO grd NOR sky kernel" << vcl_endl;
    kern = kern_vec[3];
    kern->set_arg(n_cam_cl_mem_);
    kern->set_arg(n_obj_cl_mem_);
  }

  // set up argument list
  kern->set_arg(obj_id_cl_mem_);
  kern->set_arg(obj_id_offset_cl_mem_);
  kern->set_arg(obj_min_dist_cl_mem_);
  kern->set_arg(obj_weight_cl_mem_);
  kern->set_arg(n_ind_cl_mem_);
  kern->set_arg(layer_size_cl_mem_);
  kern->set_arg(index_cl_mem_);
  kern->set_arg(score_cl_mem_);
  kern->set_arg(mu_cl_mem_);
  kern->set_arg(depth_interval_cl_mem_);
  kern->set_arg(depth_length_cl_mem_);
  kern->set_arg(debug_cl_mem_);
  // set up local memory argument
  kern->set_local_arg((*n_obj_)*sizeof(unsigned char));      // local memory for obj_min_dist
  kern->set_local_arg((*n_obj_)*sizeof(float));               // local memory for obj_weight
  kern->set_local_arg((*depth_length_buff_)*sizeof(float));   // local memory for depth_interval table

#if 0
  vcl_cerr << " obj_id = " << obj_id_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " obj_offset = " << obj_id_offset_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " obj_min_dist = " << obj_min_dist_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " obj_weight = " << obj_weight_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " n_ind = " << n_ind_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " lyaer_size = " << layer_size_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " index_cl_mem_ = " << index_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " score_cl_mem = " << score_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " mu_cl_mem_ = " << mu_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " depth_interval = " << depth_interval_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " depth_length = " << depth_length_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " debug_cl_mem_ = " << debug_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " local for min_dist = " << (*n_obj_)*sizeof(unsigned char) << vcl_endl;
  vcl_cerr << " local for weight = " << (*n_obj_)*sizeof(float) << vcl_endl;
  vcl_cerr << " local for depth_interval = " << (*depth_length_buff_)*sizeof(float) << vcl_endl;
#endif

  // execute kernel
  if (!kern->execute(queue, work_dim_, local_threads_, global_threads_)) {
    vcl_cerr << "\n ERROR: kernel execuation failed" << vcl_endl;
    delete debug_cl_mem_;
    delete debug_buff_;
    return false;
  }
  status = clFinish(queue);  // block to ensure kernel finishes
  if (status != 0) {
    vcl_cerr << "\n ERROR: " << status << "  kernel matcher failed: " + error_to_string(status) << '\n';
    return false;
  }
  // clear bocl_kernel argument list
  kern->clear_args();
#if 1
  // read debug data from device
  debug_cl_mem_->read_to_buffer(queue);
  unsigned i = 0;
  while( (debug_buff_[i]-12.31)*(debug_buff_[i]-12.31)>0.0001 && i < 1000){
    vcl_cout << " debug[" << i << "] = " << debug_buff_[i] << vcl_endl;
    i++;
  }
#endif
  // clear debug buffer
  delete debug_cl_mem_;
  status = clFinish(queue);
  check_val(status, MEM_FAILURE, " release DEBUG failed on device " + device->device_identifier() + error_to_string(status));
  
  delete [] debug_buff_;
  return true;
}

// execute kernel with orientation considered
bool boxm2_volm_matcher_p1::execute_matcher_kernel_orient(bocl_device_sptr                  device,
                                                          cl_command_queue&                  queue,
                                                          vcl_vector<bocl_kernel*>        kern_vec,
                                                          bocl_mem*                  n_ind_cl_mem_,
                                                          bocl_mem*                  index_cl_mem_,
                                                          bocl_mem*           index_orient_cl_mem_,
                                                          bocl_mem*                  score_cl_mem_,
                                                          bocl_mem*                     mu_cl_mem_)
{
  // create a debug buffer
  cl_int status;
  unsigned debug_size = 100;
  float* debug_buff_ = new float[debug_size];
  vcl_fill(debug_buff_, debug_buff_+debug_size, (float)12.31);
  bocl_mem* debug_cl_mem_ = new bocl_mem(gpu_->context(), debug_buff_, sizeof(float)*debug_size, " debug ");
  if (!debug_cl_mem_->create_buffer( CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for DEBUG" << vcl_endl;
    delete debug_cl_mem_;     delete [] debug_buff_;
    return false;
  }

  bocl_kernel* kern;
  // choose whether kernel to use
  if (is_grd_reg_ && is_sky_reg_) {              // both sky and ground
    vcl_cout << "\t using both sky and grd kernel with orientatl attribute" << vcl_endl;
    kern = kern_vec[4];
    // set up argument list
    kern->set_arg(n_cam_cl_mem_);
    kern->set_arg(n_obj_cl_mem_);
    kern->set_arg(grd_id_cl_mem_);        kern->set_arg(grd_id_offset_cl_mem_);    kern->set_arg(grd_dist_cl_mem_);    kern->set_arg(grd_weight_cl_mem_);
    kern->set_arg(sky_id_cl_mem_);        kern->set_arg(sky_id_offset_cl_mem_);    kern->set_arg(sky_weight_cl_mem_);
  } else if ( !is_grd_reg_ && is_sky_reg_) {     // no ground but sky
    vcl_cout << "\t using NO grd kernel with orientation attribute" << vcl_endl;
    kern = kern_vec[5];
    // set up argument list
    kern->set_arg(n_cam_cl_mem_);
    kern->set_arg(n_obj_cl_mem_);
    kern->set_arg(sky_id_cl_mem_);        kern->set_arg(sky_id_offset_cl_mem_);    kern->set_arg(sky_weight_cl_mem_);

#if 0
    // check the global memory
    vcl_cerr << " ------------ INSIDE execute_kernel ------------------------------- " << vcl_endl;
    vcl_cerr << " n_cam_cl_mem = " << n_cam_cl_mem_->num_bytes() << vcl_endl;
    vcl_cerr << " n_obj_cl_mem = " << n_obj_cl_mem_->num_bytes() << vcl_endl;
    vcl_cerr << " sky_id_cl_mem_ = " << sky_id_cl_mem_->num_bytes() << vcl_endl;
    vcl_cerr << " sky_offset_cl_mem_ = " << sky_id_offset_cl_mem_->num_bytes() << vcl_endl;
    vcl_cerr << " sky_weight_cl_mem_ = " << sky_weight_cl_mem_->num_bytes() << vcl_endl;
#endif

  } else if (  is_grd_reg_ && !is_sky_reg_) {    // no sky but ground
    vcl_cout << "\t using NO sky kernel with orientation attribute" << vcl_endl;
    kern = kern_vec[6];
    // set up argument list
    kern->set_arg(n_cam_cl_mem_);
    kern->set_arg(n_obj_cl_mem_);
    kern->set_arg(grd_id_cl_mem_);        kern->set_arg(grd_id_offset_cl_mem_);    kern->set_arg(grd_dist_cl_mem_);    kern->set_arg(grd_weight_cl_mem_);
  } else {                                       // neight sky nor ground
    vcl_cout << "\t using NO grd NOR sky kernel" << vcl_endl;
    kern = kern_vec[7];
    kern->set_arg(n_cam_cl_mem_);
    kern->set_arg(n_obj_cl_mem_);
  }

  // set up argument list
  kern->set_arg(obj_id_cl_mem_);
  kern->set_arg(obj_id_offset_cl_mem_);
  kern->set_arg(obj_min_dist_cl_mem_);
  kern->set_arg(obj_orient_cl_mem_);
  kern->set_arg(obj_weight_cl_mem_);
  kern->set_arg(n_ind_cl_mem_);
  kern->set_arg(layer_size_cl_mem_);
  kern->set_arg(index_cl_mem_);
  kern->set_arg(index_orient_cl_mem_);
  kern->set_arg(score_cl_mem_);
  kern->set_arg(mu_cl_mem_);
  kern->set_arg(depth_interval_cl_mem_);
  kern->set_arg(depth_length_cl_mem_);
  kern->set_arg(debug_cl_mem_);
  // set up local memory argument
  kern->set_local_arg((*n_obj_)*sizeof(unsigned char));       // local memory for obj_min_dist
  kern->set_local_arg((*n_obj_)*sizeof(unsigned char));       // local memory for orientation
  kern->set_local_arg((*n_obj_)*sizeof(float));               // local memory for obj_weight
  kern->set_local_arg((*depth_length_buff_)*sizeof(float));   // local memory for depth_interval table

#if 0
  vcl_cerr << " obj_id = " << obj_id_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " obj_offset = " << obj_id_offset_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " obj_min_dist = " << obj_min_dist_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " obj_weight = " << obj_weight_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " n_ind = " << n_ind_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " lyaer_size = " << layer_size_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " index_cl_mem_ = " << index_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " score_cl_mem = " << score_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " mu_cl_mem_ = " << mu_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " depth_interval = " << depth_interval_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " depth_length = " << depth_length_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " debug_cl_mem_ = " << debug_cl_mem_->num_bytes() << vcl_endl;
  vcl_cerr << " local for min_dist = " << (*n_obj_)*sizeof(unsigned char) << vcl_endl;
  vcl_cerr << " local for weight = " << (*n_obj_)*sizeof(float) << vcl_endl;
  vcl_cerr << " local for depth_interval = " << (*depth_length_buff_)*sizeof(float) << vcl_endl;
#endif

  // execute kernel
  if (!kern->execute(queue, work_dim_, local_threads_, global_threads_)) {
    vcl_cerr << "\n ERROR: kernel execuation failed" << vcl_endl;
    delete debug_cl_mem_;
    delete debug_buff_;
    return false;
  }
  status = clFinish(queue);  // block to ensure kernel finishes
  if (status != 0) {
    vcl_cerr << "\n ERROR: " << status << "  kernel matcher failed: " + error_to_string(status) << '\n';
    return false;
  }
  // clear bocl_kernel argument list
  kern->clear_args();
#if 1
  // read debug data from device
  debug_cl_mem_->read_to_buffer(queue);
  unsigned i = 0;
  while( (debug_buff_[i]-12.31)*(debug_buff_[i]-12.31)>0.0001 && i < 1000){
    vcl_cout << " debug[" << i << "] = " << debug_buff_[i] << vcl_endl;
    i++;
  }
#endif
  // clear debug buffer
  delete debug_cl_mem_;
  status = clFinish(queue);
  check_val(status, MEM_FAILURE, " release DEBUG failed on device " + device->device_identifier() + error_to_string(status));
  
  delete [] debug_buff_;
  return true;
}

// clare all query cl_mem pointer
bool boxm2_volm_matcher_p1::clean_query_cl_mem()
{
  delete n_cam_cl_mem_;
  delete n_obj_cl_mem_;
  if ( is_grd_reg_ ) { delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_; }
  if ( is_sky_reg_ ) { delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_; }
  if ( is_obj_reg_ ) { delete obj_id_cl_mem_;  delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_; delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_; }
  if ( use_orient_ ) { delete obj_orient_cl_mem_; }
  return true;
}

// compile kernel
bool boxm2_volm_matcher_p1::compile_kernel(vcl_vector<bocl_kernel*>& vec_kernels)
{
  // declare the kernel source
  vcl_vector<vcl_string> src_paths;
  vcl_string volm_cl_source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/volm/cl/";
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_obj_based_matching.cl");
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_obj_based_matching_no_grd.cl");
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_obj_based_matching_no_sky.cl");
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_obj_based_matching_no_grd_no_sky.cl");
  // add orientation kernels in
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_obj_based_matching_with_orient.cl");
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_obj_based_matching_no_grd_with_orient.cl");
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_obj_based_matching_no_sky_with_orient.cl");
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_obj_based_matching_no_grd_no_sky_with_orient.cl");
  // create the obj_based matching kernel
  bocl_kernel* kern_matcher = new bocl_kernel();
  if (!kern_matcher->create_kernel(&gpu_->context(), gpu_->device_id(), src_paths,
                                   "generalized_volm_obj_based_matching",
                                   "",
                                   "generalized_volm_obj_based_matching") )
    return false;
  vec_kernels.push_back(kern_matcher);
  // create the obj_based matching kernel for queries without ground
  bocl_kernel* kern_matcher_no_grd = new bocl_kernel();
  if (!kern_matcher_no_grd->create_kernel(&gpu_->context(), gpu_->device_id(), src_paths,
                                          "generalized_volm_obj_based_matching_no_grd",
                                          "",
                                          "generalized_volm_obj_based_matching_no_grd") )
    return false;
  vec_kernels.push_back(kern_matcher_no_grd);
  // create the matching kernel for queries without sky
  bocl_kernel* kern_matcher_no_sky = new bocl_kernel();
  if (!kern_matcher_no_sky->create_kernel(&gpu_->context(), gpu_->device_id(), src_paths,
                                          "generalized_volm_obj_based_matching_no_sky",
                                          "",
                                          "generalized_volm_obj_based_matching_no_sky") )
    return false;
  vec_kernels.push_back(kern_matcher_no_sky);
  // create the matching kernel for queries without sky nor grd
  bocl_kernel* kern_matcher_no_sky_no_grd = new bocl_kernel();
  if (!kern_matcher_no_sky_no_grd->create_kernel(&gpu_->context(), gpu_->device_id(), src_paths,
                                                 "generalized_volm_obj_based_matching_no_grd_no_sky",
                                                 "",
                                                 "generalized_volm_obj_based_matching_no_grd_no_sky") )
    return false;
  vec_kernels.push_back(kern_matcher_no_sky_no_grd);

  // create the matching orientation kernel
  bocl_kernel* kern_matcher_with_orient = new bocl_kernel();
  if (!kern_matcher_with_orient->create_kernel(&gpu_->context(), gpu_->device_id(), src_paths,
                                               "generalized_volm_obj_based_matching_with_orient",
                                               "",
                                               "generalized_volm_obj_based_matching_with_orient") )
    return false;
  vec_kernels.push_back(kern_matcher_with_orient);

  // create with_orientation kernel for queries without ground
  bocl_kernel* kern_matcher_no_grd_with_orient = new bocl_kernel();
  if (!kern_matcher_no_grd_with_orient->create_kernel(&gpu_->context(), gpu_->device_id(), src_paths,
                                                      "generalized_volm_obj_based_matching_no_grd_with_orient",
                                                      "",
                                                      "generalized_volm_obj_based_matching_no_grd_with_orient") )
    return false;
  vec_kernels.push_back(kern_matcher_no_grd_with_orient);

  // create with_orientation kernel for queries without sky
  bocl_kernel* kern_matcher_no_sky_with_orient = new bocl_kernel();
  if (!kern_matcher_no_sky_with_orient->create_kernel(&gpu_->context(), gpu_->device_id(), src_paths,
                                                      "generalized_volm_obj_based_matching_no_sky_with_orient",
                                                      "",
                                                      "generalized_volm_obj_based_matching_no_sky_with_orient") )
    return false;
  vec_kernels.push_back(kern_matcher_no_sky_with_orient);

  // create with_orientation kernel for queries without sky nor ground
  bocl_kernel* kern_matcher_no_grd_no_sky_with_orient = new bocl_kernel();
  if (!kern_matcher_no_grd_no_sky_with_orient->create_kernel(&gpu_->context(), gpu_->device_id(), src_paths,
                                                                "generalized_volm_obj_based_matching_no_grd_no_sky_with_orient",
                                                                "",
                                                                "generalized_volm_obj_based_matching_no_grd_no_sky_with_orient") )
    return false;
  vec_kernels.push_back(kern_matcher_no_grd_no_sky_with_orient);
  return true;
}

bool boxm2_volm_matcher_p1::create_queue()
{
  cl_int status = SDK_FAILURE;
  queue_ = clCreateCommandQueue(gpu_->context(),
                               *(gpu_->device_id()),
                               CL_QUEUE_PROFILING_ENABLE,
                               &status);
  if ( !check_val(status, CL_SUCCESS, error_to_string(status)) )
    return false;
  return true;
}

bool boxm2_volm_matcher_p1::write_matcher_result(vcl_string const& tile_fname_bin, vcl_string const& tile_fname_txt)
{
  // write the output store binary as tile
  // write the scores out as binary
  volm_score::write_scores(score_all_, tile_fname_bin);
  // for testing purpose, generate a temparay txt file for output
  if (vul_file::exists(tile_fname_txt))
    vul_file::delete_file_glob(tile_fname_txt);

  unsigned all_loc = score_all_.size();
  for (unsigned i = 0; i < all_loc; i++) {
    volm_score_sptr score = score_all_[i];
    vcl_vector<unsigned> cam_ids = score->cam_id_;
    // write the txt output
    vcl_ofstream txt_ofs(tile_fname_txt, vcl_ios_app);
    txt_ofs << vcl_setprecision(6) << score->leaf_id_ << ' ' << score->hypo_id_ << ' ' << score->max_score_ << ' ' << score->max_cam_id_ << '\n';
    for (unsigned jj = 0; jj < cam_ids.size(); jj++)
      txt_ofs << ' ' << cam_ids[jj];
    txt_ofs << '\n';
    txt_ofs.close();
  }
  
#if 0
  // write the output into file
  // for testing purpose, create a text file to store the highest score for each locations
  
  // clear previous matcher output
  for (unsigned i = 0; i < leaves_.size(); i++) {
    vcl_string txt_fname = leaves_[i]->get_score_txt_name(out_fname_pre, 1);
    vcl_string bin_fname = leaves_[i]->get_score_bin_name(out_fname_pre, 1);
    if (vul_file::exists(txt_fname))
      if (!vul_file::delete_file_glob(txt_fname)) {
        vcl_cerr << "\n ERROR: trying to delete previous output file " << txt_fname << " failed" << vcl_endl;
        return false;
      }
    if (vul_file::exists(bin_fname))
      if (!vul_file::delete_file_glob(bin_fname)) {
        vcl_cerr << "\n ERROR: trying to delete previous output file " << bin_fname << " failed" << vcl_endl;
        return false;
      }
  }

  // write output
  unsigned all_loc = score_all_.size();
  for (unsigned i = 0; i < all_loc; i++) {
    volm_score_sptr score = score_all_[i];
    unsigned leaf_id = score->leaf_id_;
    vcl_vector<unsigned> cam_ids = score->cam_id_;
    vcl_string txt_fname = leaves_[leaf_id]->get_score_txt_name(out_fname_pre, 1);
    vcl_string bin_fname = leaves_[leaf_id]->get_score_bin_name(out_fname_pre, 1);
    // write the txt output
    vcl_ofstream txt_ofs(txt_fname, vcl_ios_app);
    txt_ofs << vcl_setprecision(6) << score->hypo_id_ << " " << score->max_score_ << " " << score->max_cam_id_ << '\n';
    for (unsigned jj = 0; jj < cam_ids.size(); jj++)
      txt_ofs << ' ' << cam_ids[jj];
    txt_ofs << '\n';
  }
#endif
  return true;
}

bool boxm2_volm_matcher_p1::write_matcher_result(vcl_string const& tile_fname_bin)
{
  volm_score::write_scores(score_all_, tile_fname_bin);
  return true;
}

// write the score for all cameras whose score higher than threshold
bool boxm2_volm_matcher_p1::write_gt_cam_score(unsigned const& leaf_id, unsigned const& hypo_id, vcl_string const& out_fname)
{
  vcl_ofstream ofs(out_fname);
  for (unsigned i = 0; i < score_cam_.size(); i++) {
    if (score_cam_[i].l_id_ == leaf_id && score_cam_[i].h_id_ == hypo_id) {
      ofs << score_cam_[i].l_id_ << ' ' << score_cam_[i].h_id_ << '\n';
      vcl_vector<unsigned> cam_ids = score_cam_[i].cam_id_;
      vcl_vector<float> cam_scores = score_cam_[i].cam_score_;
      for (unsigned jj = 0; jj < cam_ids.size(); jj++) {
        ofs << vcl_setprecision(6) << cam_ids[jj] << ' ' << cam_scores[jj] << ' ' << query_->get_cam_string(cam_ids[jj]) << '\n';
      }
      return true;
    }
  }
  ofs.close();
  return false;
}

bool boxm2_volm_matcher_p1::transfer_query()
{
  if ( !is_grd_reg_ && !is_sky_reg_ && !is_obj_reg_ ){
    vcl_cerr << "\n ERROR: no depth_map_region defined in query image, check the labelme.xml" << vcl_endl;
    return false;
  }
  if (!is_obj_reg_) {
    vcl_cerr << "\n ERROR: current pass 01 matcher is not able to match query without any non_grd, non_sky object, add at least one in the labelme.xml" << vcl_endl;
    return false;
  }


  // check whether we use orientation attribute, if yes, modify the weight parameters first
  float weight = 0;
  if (use_orient_) {
    float w = 0;
    if (is_grd_reg_ && is_sky_reg_) {       // both ground and sky
      w = 1.0f/6.0f;
      grd_weight_buff_ = new float;    *grd_weight_buff_ = w;
      sky_weight_buff_ = new float;    *sky_weight_buff_ = w;
      obj_weight_buff_ = new float[*n_obj_];
      for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++)
        obj_weight_buff_[obj_id] = w;
    } else if (is_grd_reg_) {                  // no sky but ground
      w = 1.0f/5.0f;
      grd_weight_buff_ = new float;     *grd_weight_buff_ = w;
      for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++)
        obj_weight_buff_[obj_id] = w;
    } else if (is_sky_reg_) {                  // no ground but sky
      w = 0.25f;
      sky_weight_buff_ = new float;    *sky_weight_buff_ = w;
      obj_weight_buff_ = new float[*n_obj_];
      for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++)
        obj_weight_buff_[obj_id] = w;
    } else {                                   // no ground no sky
      w = 1.0f/3.0f;
      obj_weight_buff_ = new float[*n_obj_];
      for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++)
        obj_weight_buff_[obj_id] = w;
    }
  } else {
    // use the buffer define in volm_query
    if (is_grd_reg_ && is_sky_reg_) {       // both ground and sky
      grd_weight_buff_ = new float;    *grd_weight_buff_ = query_->grd_weight();
      sky_weight_buff_ = new float;    *sky_weight_buff_ = query_->sky_weight();
      obj_weight_buff_ = new float[*n_obj_];
      for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++)
        obj_weight_buff_[obj_id] = query_->obj_weight()[obj_id];
    } else if (is_grd_reg_) {                  // no sky but ground
      grd_weight_buff_ = new float;     *grd_weight_buff_ = query_->grd_weight();
      for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++)
        obj_weight_buff_[obj_id] = query_->obj_weight()[obj_id];
    } else if (is_sky_reg_) {                  // no ground but sky
      sky_weight_buff_ = new float;    *sky_weight_buff_ = query_->sky_weight();
      obj_weight_buff_ = new float[*n_obj_];
      for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++)
        obj_weight_buff_[obj_id] = query_->obj_weight()[obj_id];
    } else {                                   // no ground no sky
      obj_weight_buff_ = new float[*n_obj_];
      for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++)
        obj_weight_buff_[obj_id] = query_->obj_weight()[obj_id];
    }
  }

  query_global_mem_ = 3 * sizeof(unsigned);   // n_cam + n_obj + n_ind
   query_local_mem_ = 3 * sizeof(unsigned);   // n_cam + n_obj + n_ind

  // construct the ground_id, ground_dist, ground_offset 1D array
  if (is_grd_reg_) {
    unsigned grd_vox_size = query_->get_ground_id_size();
    grd_id_buff_ = new unsigned[grd_vox_size];
    grd_dist_buff_ = new unsigned char[grd_vox_size];
    grd_id_offset_buff_ = new unsigned[*n_cam_+1];
    unsigned grd_count = 0;
    vcl_vector<vcl_vector<unsigned> >& grd_id = query_->ground_id();
    vcl_vector<vcl_vector<unsigned char> >& grd_dist = query_->ground_dist();
    for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
      grd_id_offset_buff_[cam_id] = grd_count;
      for (unsigned vox_id = 0; vox_id < grd_id[cam_id].size(); vox_id++) {
        unsigned i = grd_count + vox_id;
        grd_id_buff_[i] = grd_id[cam_id][vox_id];
        grd_dist_buff_[i] = grd_dist[cam_id][vox_id];
      }
      grd_count += (unsigned)grd_id[cam_id].size();
    }
    // construct weight parameters
    grd_id_offset_buff_[*n_cam_] = grd_count;
    //grd_weight_buff_ = new float;
    //*grd_weight_buff_ = query_->grd_weight();
    grd_id_cl_mem_ = new bocl_mem(gpu_->context(), grd_id_buff_, sizeof(unsigned)*grd_vox_size, " grd_id " );
    if (!grd_id_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for GROUND_ID" << vcl_endl;
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;
      return false;
    }
    grd_dist_cl_mem_ = new bocl_mem(gpu_->context(), grd_dist_buff_, sizeof(unsigned char)*grd_vox_size, " grd_dist " );
    if (!grd_dist_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for GROUND_DIST" << vcl_endl;
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;
      return false;
    }
    grd_id_offset_cl_mem_ = new bocl_mem(gpu_->context(), grd_id_offset_buff_, sizeof(unsigned)*(*n_cam_+1), " grd_offset " );
    if (!grd_id_offset_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for GROUND_OFFSET" << vcl_endl;
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;
      return false;
    }
    grd_weight_cl_mem_ = new bocl_mem(gpu_->context(), grd_weight_buff_, sizeof(float), " grd_weight " );
    if (!grd_weight_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for GROUND_WEIGHT" << vcl_endl;
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
      return false;
    }
    query_global_mem_ += (sizeof(unsigned char)+sizeof(unsigned))*grd_vox_size;  // ground id and dist
    query_global_mem_ += sizeof(unsigned)*(query_->ground_offset().size());      // ground offset array
    query_global_mem_ += sizeof(float);                                          // ground weight
  }
  
  // construct sky_id buff
  if (is_sky_reg_) {
    unsigned sky_vox_size = query_->get_sky_id_size();
    sky_id_buff_ = new unsigned[sky_vox_size];
    sky_id_offset_buff_ = new unsigned[*n_cam_+1];
    unsigned sky_count = 0;
    vcl_vector<vcl_vector<unsigned> >& sky_id = query_->sky_id();
    for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
      sky_id_offset_buff_[cam_id] = sky_count;
      for (unsigned vox_id = 0; vox_id < sky_id[cam_id].size(); vox_id++) {
        unsigned i = sky_count + vox_id;
        sky_id_buff_[i] = sky_id[cam_id][vox_id];
      }
      sky_count += (unsigned)sky_id[cam_id].size();
    }
    sky_id_offset_buff_[*n_cam_] = sky_count;
    // construct weight parameters
    //sky_weight_buff_ = new float;
    //*sky_weight_buff_ = query_->sky_weight();
    sky_id_cl_mem_ = new bocl_mem(gpu_->context(), sky_id_buff_, sizeof(unsigned)*sky_vox_size, " sky_id " );
    if(!sky_id_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for SKY_ID" << vcl_endl;
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      if(is_grd_reg_) { delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_; }
      delete sky_id_cl_mem_;
      return false;
    }
    sky_id_offset_cl_mem_ = new bocl_mem(gpu_->context(), sky_id_offset_buff_, sizeof(unsigned)*(*n_cam_+1), " sky_offset " );
    if(!sky_id_offset_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for SKY_OFFSET" << vcl_endl;
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      if(is_grd_reg_) { delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_; }
      delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;
      return false;
    }
    sky_weight_cl_mem_ = new bocl_mem(gpu_->context(), sky_weight_buff_, sizeof(float), " sky_weight " );
    if(!sky_weight_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for SKY_WEIGHT" << vcl_endl;
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      if(is_grd_reg_) { delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_; }
      delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
      return false;
    }
    query_global_mem_ += sizeof(unsigned)*sky_vox_size;                          // sky id
    query_global_mem_ += sizeof(unsigned)*query_->sky_offset().size();           // sky dist
    query_global_mem_ += sizeof(float);                                          // sky weight
  }
  
  // construct obj_id, obj_offset 1D array
  if (is_obj_reg_) {
    unsigned obj_vox_size = query_->get_dist_id_size();
    unsigned obj_offset_size = (*n_cam_) * (*n_obj_);
    obj_id_buff_ = new unsigned[obj_vox_size];
    obj_id_offset_buff_ = new unsigned[obj_offset_size+1];
    vcl_vector<vcl_vector<vcl_vector<unsigned> > >& dist_id = query_->dist_id(); 
    unsigned obj_count = 0;
    for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
      for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++) {
        unsigned offset_id = obj_id + cam_id * (*n_obj_);
        obj_id_offset_buff_[offset_id] = obj_count;
        for (unsigned vox_id = 0; vox_id < dist_id[cam_id][obj_id].size(); vox_id++) {
          unsigned i = obj_count + vox_id;
          obj_id_buff_[i] = dist_id[cam_id][obj_id][vox_id];
        }
        obj_count += (unsigned)dist_id[cam_id][obj_id].size();
      }
    }
    obj_id_offset_buff_[obj_offset_size] = obj_count;
    // construct min_dist, max_dist and order_obj for non-ground, non-sky object
    obj_min_dist_buff_ = new unsigned char[*n_obj_];
    //max_obj_dist_buff = new unsigned char[n_obj_];
    obj_order_buff_ = new unsigned char[*n_obj_];
    for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++) {
      obj_min_dist_buff_[obj_id] = query_->min_obj_dist()[obj_id];
      obj_order_buff_[obj_id] = query_->order_obj()[obj_id];
    }
    //obj_weight_buff_ = new float[*n_obj_];
    //for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++)
    //  obj_weight_buff_[obj_id] = query_->obj_weight()[obj_id];
    // create corresponding cl_mem
    obj_id_cl_mem_ = new bocl_mem(gpu_->context(), obj_id_buff_, sizeof(unsigned)*obj_vox_size, " obj_id " );
    if(!obj_id_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for OBJ_ID" << vcl_endl;
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      if(is_grd_reg_) { delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_; }
      if(is_obj_reg_) { delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_; }
      delete obj_id_cl_mem_;
      return false;
    }
    obj_id_offset_cl_mem_ = new bocl_mem(gpu_->context(), obj_id_offset_buff_, sizeof(unsigned)*(obj_offset_size+1), " obj_offset " );
    if(!obj_id_offset_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for OBJ_OFFSET" << vcl_endl;
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      if(is_grd_reg_) { delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_; }
      if(is_obj_reg_) { delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_; }
      delete obj_id_cl_mem_;  delete obj_id_offset_cl_mem_;
      return false;
    }
    obj_weight_cl_mem_ = new bocl_mem(gpu_->context(), obj_weight_buff_, sizeof(float)*(*n_obj_), " obj_weight " );
    if(!obj_weight_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for OBJ_WEIGHT" << vcl_endl;
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      if(is_grd_reg_) { delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_; }
      if(is_obj_reg_) { delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_; }
      delete obj_id_cl_mem_;  delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
      return false;
    }
    obj_min_dist_cl_mem_ = new bocl_mem(gpu_->context(), obj_min_dist_buff_, sizeof(unsigned char)*(*n_obj_), " obj_min_dist " );
    if(!obj_min_dist_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for OBJ_MIN_DIST" << vcl_endl;
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      if(is_grd_reg_) { delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_; }
      if(is_obj_reg_) { delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_; }
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
      delete obj_min_dist_cl_mem_;
      return false;
    }
    obj_order_cl_mem_ = new bocl_mem(gpu_->context(), obj_order_buff_, sizeof(unsigned char)*(*n_obj_), " obj_order " );
    if(!obj_order_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for OBJ_ORDER" << vcl_endl;
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      if(is_grd_reg_) { delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_; }
      if(is_obj_reg_) { delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_; }
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
      delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
      return false;
    }
    query_global_mem_ += sizeof(unsigned)*obj_vox_size;                          // object id
    query_global_mem_ += sizeof(unsigned)*query_->dist_offset().size();          // object offset
    query_global_mem_ += sizeof(float)*(*n_obj_);                                // object weight
    query_global_mem_ += sizeof(unsigned char)*2*(*n_obj_);                      // object dist and order
    query_local_mem_ += sizeof(unsigned char)*2*(*n_obj_);                       // object dist and order
    query_local_mem_ += sizeof(unsigned char)*2*(*n_obj_);                       // object dist and order
    query_local_mem_ += sizeof(float)*(2+*n_obj_);                               // grd, sky, object weight
    
  }
  // implement orientation transfer function
  if (use_orient_) {
    if (!this->transfer_orient()) {
      vcl_cerr << " ERROR: transfering query orientaion failed " << vcl_endl;
      return false;
    }
    query_global_mem_ += sizeof(unsigned char)*(*n_obj_);
    query_local_mem_ += sizeof(unsigned char)*(*n_obj_);
  }

  // check the device memory
  // check whether query size has exceeded the available global memory and local_memory
  device_global_mem_ = (gpu_->info()).total_global_memory_;
  device_local_mem_ = (gpu_->info()).total_local_memory_;

  if (query_global_mem_ > device_global_mem_) {
    vcl_cerr << " ERROR: the required global memory for query "  << query_global_mem_/1073741824.0
             << " GByte is larger than available global memory " << device_global_mem_/1073741824.0 
             << " GB" << vcl_endl;
    this->clean_query_cl_mem();
    return false;
  }
  if (query_local_mem_ > device_local_mem_) {
    vcl_cerr << " ERROR: the required local memoery for query " << query_local_mem_/1024.0
             << " KByte is larger than available local memory " << device_local_mem_/1024.0
             << " KB" << vcl_endl;
    this->clean_query_cl_mem();
    return false;
  }
  return true;
}

bool boxm2_volm_matcher_p1::transfer_orient()
{
  // temporary only consider horizontal and front_parallel 
  vcl_vector<unsigned char>& obj_orient = query_->obj_orient();
  obj_orient_buff_ = new unsigned char[*n_obj_];
  for (unsigned i = 0; i < *n_obj_; i++) {
    if (obj_orient[i] == depth_map_region::HORIZONTAL)
      obj_orient_buff_[i] = (unsigned char)1;  // horizontal
    else
      obj_orient_buff_[i] = (unsigned char)2;  // vertical(front parallel)
  }
#if 0
  // check the transferred orientation
  for (unsigned i = 0; i < *n_obj_; i++) {
    vcl_cout << " i = " << i << ", query_orient = "  << (int)obj_orient[i]
             << ", orient_buff = " << (int)obj_orient_buff_[i] << vcl_endl; 
  }
#endif
  // declare cl_mem for orient
  obj_orient_cl_mem_ = new bocl_mem(gpu_->context(), obj_orient_buff_, sizeof(unsigned char)*(*n_obj_), " obj_orient ");
  if (!obj_orient_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for OBJ_ORIENT" << vcl_endl;
    this->clean_query_cl_mem();
    return false;
  }
  return true;
}

bool boxm2_volm_matcher_p1::volm_matcher_p1_test(unsigned n_ind,
                                                 unsigned char* index,
                                                 float* score_buff,
                                                 float* mu_buff)
{
  vcl_vector<float> mu;
  unsigned nc = *n_cam_;
  unsigned no = *n_obj_;

  if (is_obj_reg_) {
    // calcualte mean depth value
    for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
      unsigned start_ind = ind_id * (layer_size_);
      for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
        for (unsigned k = 0; k < no; k++) {
          unsigned offset_id = k + no * cam_id;
          unsigned start_obj = obj_id_offset_buff_[offset_id];
          unsigned end_obj = obj_id_offset_buff_[offset_id+1];
          float    mu_obj = 0;
          unsigned cnt = 0;
          for (unsigned i = start_obj; i < end_obj; i++) {
            unsigned id = start_ind + obj_id_buff_[i];
            if ( (unsigned)index[id] < 253 ) {
              mu_obj += depth_interval_[index[id]];
              cnt += 1;
            }
          }
          mu_obj = (cnt > 0) ? mu_obj/cnt : 0;

          mu.push_back(mu_obj);
        }
      }
    }
  }
  // calculate sky score
  vcl_vector<float> score_sky_all;
  if (is_sky_reg_) {
    for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
      unsigned start_ind = ind_id * (layer_size_);
      for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
        unsigned start = sky_id_offset_buff_[cam_id];
        unsigned end = sky_id_offset_buff_[cam_id+1];
        unsigned cnt = 0;
        for (unsigned k = start; k < end; k++) {
          unsigned id = start_ind + sky_id_buff_[k];
          if (index[id] == 254) cnt++;
        }
        float score_sky = (end != start) ? (float)cnt/(end-start) : 0.0f;
        score_sky *= (*sky_weight_buff_);
        score_sky_all.push_back(score_sky);
      }
    }
  }
  // calculate the ground score
  // define the altitude ratio, suppose the altitude in index could ba up to 3 meter
  // assuming the read-in alt values in query is normally ~1m, the altiutide ratio would be (2-1)/1 ~2
  // the altitude ratio defined the tolerance for ground distance d as delta_d = alt_ratio * d
  // this delte_d coule be in depth_interval unit ?
  unsigned char alt_ratio = 2;
  vcl_vector<float> score_grd_all;
  if (is_grd_reg_) {
    for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
      unsigned start_ind = ind_id * layer_size_;
      for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
        unsigned start = grd_id_offset_buff_[cam_id];
        unsigned end = grd_id_offset_buff_[cam_id+1];
        unsigned cnt = 0;
        for (unsigned k = start; k < end; k++) {
          unsigned id = start_ind + grd_id_buff_[k];
          unsigned char ind_d = index[id];
          unsigned char grd_d = grd_dist_buff_[k];
          unsigned char delta_d = alt_ratio * grd_d;
          if ( ind_d >= (grd_d-delta_d) && ind_d <= (grd_d+delta_d) ) cnt++;


        }
        float score_grd = (end!=start) ? (float)cnt/(end-start) : 0.0f;
        score_grd *= (*grd_weight_buff_);
        score_grd_all.push_back(score_grd);
      }
    }
  }


  // calculate the object score
  vcl_vector<float> score_order_all;
  vcl_vector<float> score_min_all;
  if( is_obj_reg_) {  
    for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
      unsigned start_ind = ind_id * layer_size_;
      for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
        float score_order = 0.0f;
        float score_min = 0.0f;
        unsigned mu_start_id = cam_id*no + ind_id*no*nc;
        for (unsigned k = 0; k < no; k++) {
          unsigned offset_id = k + no*cam_id;
          unsigned start_obj = obj_id_offset_buff_[offset_id];
          unsigned end_obj = obj_id_offset_buff_[offset_id+1];
          float score_k = 0.0f;
          float score_min_k = 0.0f;
          for (unsigned i = start_obj; i < end_obj; i++) {
            unsigned id = start_ind + obj_id_buff_[i];
            unsigned d = index[id];
            unsigned s_vox = 1;
            unsigned s_min = 0;
            unsigned min_k = obj_min_dist_buff_[k];
            if ( d < 253 ){  // valid object voxel
              // do the order checking
              for (unsigned mu_id = 0; (mu_id < k && s_vox); mu_id++){
                float depth_d = depth_interval_[d];
                float depth_m = mu[mu_id+mu_start_id];
                if(mu[mu_id+mu_start_id] != 0)
                  s_vox = s_vox * ( depth_interval_[d] >= mu[mu_id+mu_start_id] );
              }
              for (unsigned mu_id = k+1; (mu_id < no && s_vox); mu_id++) {
                float depth_d = depth_interval_[d];
                float depth_m = mu[mu_id+mu_start_id];
                if(mu[mu_id+mu_start_id] != 0)
                s_vox = s_vox * ( depth_interval_[d] <= mu[mu_id+mu_start_id] );
              }
              if ( d > obj_min_dist_buff_[k] )
                s_min = 1;
            } else {
              s_vox = 0;
            }
            score_k += (float)s_vox;
            score_min_k += (float)s_min;
          } // end for loop over voxel in object k
          // normalized the order score for object k
          score_k = (end_obj != start_obj) ? score_k/(end_obj-start_obj) : 0;
          score_k = score_k * obj_weight_buff_[k];
          // normalized the min_dist score for object k
          score_min_k = (end_obj != start_obj) ? score_min_k/(end_obj-start_obj) : 0;
          score_min_k = score_min_k * obj_weight_buff_[k];
          // summerize order score for index ind_id and camera cam_id
          score_order += score_k;
          score_min += score_min_k; 
        }  // end for loop over objects
        score_order = score_order / no;
        score_order_all.push_back(score_order);
        score_min = score_min / no;
        score_min_all.push_back(score_min);
      } // end of loop over cameras
    } // end of loop over indices
  }
  

  // get the overall score
  vcl_vector<float> score_all;
  for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
    for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
      unsigned id = cam_id + ind_id * nc;
      if (is_grd_reg_ && is_sky_reg_)
        score_all.push_back(score_sky_all[id] + score_grd_all[id] + score_order_all[id] + score_min_all[id]);
      else if ( !is_grd_reg_ && is_sky_reg_)
        score_all.push_back(score_sky_all[id] + score_order_all[id] + score_min_all[id]);
      else if ( !is_sky_reg_ && is_grd_reg_)
        score_all.push_back(score_grd_all[id] + score_order_all[id] + score_min_all[id]);
      else 
        score_all.push_back(score_order_all[id] + score_min_all[id]);
    }
  }

  // output all sky and ground score for checking
#if 0
  for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
    for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
      unsigned id = cam_id + ind_id * nc;
      vcl_cout << " ind_id = " << ind_id << " cam_id = " << cam_id 
               << " score_sky[" << id << "] = " << score_sky_all[id] 
               << "\t\t score_grd[" << id << "] = " << score_grd_all[id]
               << "\t\t score_order[" << id << "] = " << score_order_all[id]
               << "\t\t score_min[" << id << "] = " << score_min_all[id]
               << vcl_endl;
    }
  }
#endif
  // output for objects
#if 0
  // mean values
  for (unsigned ind_id = 0; ind_id < n_ind; ind_id++)
    for (unsigned cam_id = 0; cam_id < nc; cam_id++)
      for (unsigned obj_id = 0; obj_id < no; obj_id++) {
        unsigned start_ind = ind_id * layer_size_;
        unsigned id = obj_id + cam_id * no + ind_id * nc * no;
        //if( mu[id] - mu_buff[id]  != 0){
          vcl_cout << " ind_id = " << ind_id
                   << " cam_id = " << cam_id
                   << " obj_id = " << obj_id
                   << " start_ind = " << start_ind
                   << " id = " << id
                   << " mu_cpu = " << mu[id]
                   << " mu_gpu = " << mu_buff[id]
                   << " mu_diff = " << mu[id] - mu_buff[id]
                   << vcl_endl;
       //}
      }
#endif

  // score_order
  //for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
  //  for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
  //    unsigned id = cam_id + ind_id * nc;
  //    vcl_cout << " ind_id = " << ind_id << " cam_id = " << cam_id << " score_order[" << id << "] = " << score_order_all[id] << vcl_endl;
  //  }
  //}

  // score_min
  //for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
  //  for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
  //    unsigned id = cam_id + ind_id * nc;
  //    vcl_cout << " ind = " << ind_id << " cam = " << cam_id << " score_min[" << id << "] = " << score_min_all[id] << vcl_endl;
  //  }
  //}

    // check the score output
  for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
    for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
      unsigned id = cam_id + ind_id * nc;
      vcl_cout << " ind = " << ind_id << " cam = " << cam_id << " id = " << id
               << "\t score_cpu = " << score_all[id]
               << "\t score_gpu = " << score_buff[id]
               << "\t diff = " << score_all[id] - score_buff[id]
               << vcl_endl;
    }
  }
  return true;
}


bool boxm2_volm_matcher_p1::volm_matcher_p1_test_ori(unsigned n_ind,
                                                     unsigned char* index,
                                                     unsigned char* index_orient,
                                                     float* score_buff,
                                                     float* mu_buff)
{
  vcl_vector<float> mu;
  unsigned nc = *n_cam_;
  unsigned no = *n_obj_;

  if (is_obj_reg_) {
    // calcualte mean depth value
    for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
      unsigned start_ind = ind_id * (layer_size_);
      for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
        for (unsigned k = 0; k < no; k++) {
          unsigned offset_id = k + no * cam_id;
          unsigned start_obj = obj_id_offset_buff_[offset_id];
          unsigned end_obj = obj_id_offset_buff_[offset_id+1];
          float    mu_obj = 0;
          unsigned cnt = 0;
          for (unsigned i = start_obj; i < end_obj; i++) {
            unsigned id = start_ind + obj_id_buff_[i];
            if ( (unsigned)index[id] < 253 ) {
              mu_obj += depth_interval_[index[id]];
              cnt += 1;
            }
          }
          mu_obj = (cnt > 0) ? mu_obj/cnt : 0;

          mu.push_back(mu_obj);
        }
      }
    }
  }
  // calculate sky score
  vcl_vector<float> score_sky_all;
  if (is_sky_reg_) {
    for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
      unsigned start_ind = ind_id * (layer_size_);
      for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
        unsigned start = sky_id_offset_buff_[cam_id];
        unsigned end = sky_id_offset_buff_[cam_id+1];
        unsigned cnt = 0;
        for (unsigned k = start; k < end; k++) {
          unsigned id = start_ind + sky_id_buff_[k];
          if (index[id] == 254) cnt++;
        }
        float score_sky = (end != start) ? (float)cnt/(end-start) : 0.0f;
        score_sky *= (*sky_weight_buff_);
        score_sky_all.push_back(score_sky);
      }
    }
  }
  // calculate the ground score
  // define the altitude ratio, suppose the altitude in index could ba up to 3 meter
  // assuming the read-in alt values in query is normally ~1m, the altiutide ratio would be (2-1)/1 ~2
  // the altitude ratio defined the tolerance for ground distance d as delta_d = alt_ratio * d
  // this delte_d coule be in depth_interval unit ?
  unsigned char alt_ratio = 2;
  vcl_vector<float> score_grd_all;
  vcl_vector<float> score_grd_dist;
  vcl_vector<float> score_grd_ori;
  if (is_grd_reg_) {
    for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
      unsigned start_ind = ind_id * layer_size_;
      for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
        unsigned start = grd_id_offset_buff_[cam_id];
        unsigned end = grd_id_offset_buff_[cam_id+1];
        unsigned cnt = 0;
        unsigned ori_cnt = 0;
        for (unsigned k = start; k < end; k++) {
          unsigned id = start_ind + grd_id_buff_[k];
          unsigned char ind_d = index[id];
          unsigned char grd_d = grd_dist_buff_[k];
          unsigned char delta_d = alt_ratio * grd_d;
          if ( ind_d >= (grd_d-delta_d) && ind_d <= (grd_d+delta_d) ) cnt++;
          // calculate ground orientation score, ground plane has to be horizontal
          if (index_orient[id] == 1) ori_cnt++;
        }
        float score_grd = (end!=start) ? (float)cnt/(end-start) : 0.0f;
        float score_grd_ori = (end!=start) ? (float)ori_cnt/(end-start) : 0.0f;
        score_grd *= (*grd_weight_buff_);
        score_grd_ori *= (*grd_weight_buff_);
        score_grd_all.push_back(score_grd + score_grd_ori);
      }
    }
  }


  // calculate the object score
  vcl_vector<float> score_order_all;
  vcl_vector<float> score_min_all;
  vcl_vector<float> score_ori_all;
  if( is_obj_reg_) {  
    for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
      unsigned start_ind = ind_id * layer_size_;
      for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
        float score_order = 0.0f;
        float score_min = 0.0f;
        float score_ori = 0.0f;
        unsigned mu_start_id = cam_id*no + ind_id*no*nc;
        for (unsigned k = 0; k < no; k++) {
          unsigned offset_id = k + no*cam_id;
          unsigned start_obj = obj_id_offset_buff_[offset_id];
          unsigned end_obj = obj_id_offset_buff_[offset_id+1];
          float score_k = 0.0f;
          float score_min_k = 0.0f;
          float score_ori_k = 0.0f;
          for (unsigned i = start_obj; i < end_obj; i++) {
            unsigned id = start_ind + obj_id_buff_[i];
            unsigned d = index[id];
            unsigned s_vox = 1;
            unsigned s_min = 0;
            unsigned s_ori = 0;
            unsigned min_k = obj_min_dist_buff_[k];
            if ( d < 253 ){  // valid object voxel
              // do the order checking
              for (unsigned mu_id = 0; (mu_id < k && s_vox); mu_id++){
                float depth_d = depth_interval_[d];
                float depth_m = mu[mu_id+mu_start_id];
                if(mu[mu_id+mu_start_id] != 0)
                  s_vox = s_vox * ( depth_interval_[d] >= mu[mu_id+mu_start_id] );
              }
              for (unsigned mu_id = k+1; (mu_id < no && s_vox); mu_id++) {
                float depth_d = depth_interval_[d];
                float depth_m = mu[mu_id+mu_start_id];
                if(mu[mu_id+mu_start_id] != 0)
                s_vox = s_vox * ( depth_interval_[d] <= mu[mu_id+mu_start_id] );
              }
              if ( d > obj_min_dist_buff_[k] )
                s_min = 1;
            } else {
              s_vox = 0;
            }
            // check the orientation score
            unsigned char ind_ori = index_orient[id];
            if ( ind_ori > 0 && ind_ori < 10) {
              if ( ind_ori == obj_orient_buff_[k] )
                s_ori = 1;
              if ( ind_ori != 1 && obj_orient_buff_[k]==2 )
                s_ori = 1;
            }
            score_k += (float)s_vox;
            score_min_k += (float)s_min;
            score_ori_k += (float)s_ori;
          } // end for loop over voxel in object k
          // normalized the order score for object k
          score_k = (end_obj != start_obj) ? score_k/(end_obj-start_obj) : 0;
          score_k = score_k * obj_weight_buff_[k];
          // normalized the min_dist score for object k
          score_min_k = (end_obj != start_obj) ? score_min_k/(end_obj-start_obj) : 0;
          score_min_k = score_min_k * obj_weight_buff_[k];
          // normalized the orient score for object k
          score_ori_k = (end_obj != start_obj) ? score_ori_k/(end_obj-start_obj) : 0;
          score_ori_k = score_ori_k * obj_weight_buff_[k];
          // summerize order score for index ind_id and camera cam_id
          score_order += score_k;
          score_min += score_min_k;
          score_ori += score_ori_k;
        }  // end for loop over objects
        score_order = score_order / no;
        score_order_all.push_back(score_order);
        score_min = score_min / no;
        score_min_all.push_back(score_min);
        score_ori = score_ori / no;
        score_ori_all.push_back(score_ori);
      } // end of loop over cameras
    } // end of loop over indices
  }
  

  // get the overall score
  vcl_vector<float> score_all;
  for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
    for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
      unsigned id = cam_id + ind_id * nc;
      if (is_grd_reg_ && is_sky_reg_)
        score_all.push_back(score_sky_all[id] + score_grd_all[id] + score_order_all[id] + score_min_all[id] + score_ori_all[id]);
      else if ( !is_grd_reg_ && is_sky_reg_)
        score_all.push_back(score_sky_all[id] + score_order_all[id] + score_min_all[id] + score_ori_all[id]);
      else if ( !is_sky_reg_ && is_grd_reg_)
        score_all.push_back(score_grd_all[id] + score_order_all[id] + score_min_all[id] + score_ori_all[id]);
      else 
        score_all.push_back(score_order_all[id] + score_min_all[id] + score_ori_all[id]);
    }
  }

  // output all sky and ground score for checking
#if 0
  for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
    for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
      unsigned id = cam_id + ind_id * nc;
      vcl_cout << " ind_id = " << ind_id << " cam_id = " << cam_id 
               << " score_sky[" << id << "] = " << score_sky_all[id] 
               << "\t\t score_grd[" << id << "] = " << score_grd_all[id]
               << "\t\t score_order[" << id << "] = " << score_order_all[id]
               << "\t\t score_min[" << id << "] = " << score_min_all[id]
               << "\t\t score_orient[" << id << " = " << score_ori_all[id]
               << vcl_endl;
    }
  }
#endif
  // output for objects
#if 0
  // mean values
  for (unsigned ind_id = 0; ind_id < n_ind; ind_id++)
    for (unsigned cam_id = 0; cam_id < nc; cam_id++)
      for (unsigned obj_id = 0; obj_id < no; obj_id++) {
        unsigned start_ind = ind_id * layer_size_;
        unsigned id = obj_id + cam_id * no + ind_id * nc * no;
        //if( mu[id] - mu_buff[id]  != 0){
          vcl_cout << " ind_id = " << ind_id
                   << " cam_id = " << cam_id
                   << " obj_id = " << obj_id
                   << " start_ind = " << start_ind
                   << " id = " << id
                   << " mu_cpu = " << mu[id]
                   << " mu_gpu = " << mu_buff[id]
                   << " mu_diff = " << mu[id] - mu_buff[id]
                   << vcl_endl;
       //}
      }
#endif

#if 0
    // score for object
    for (unsigned ind_id = 0; ind_id < n_ind; ind_id++)
    for (unsigned cam_id = 0; cam_id < nc; cam_id++)
      for (unsigned obj_id = 0; obj_id < no; obj_id++) {
        unsigned id = cam_id + ind_id * nc;
        //if( mu[id] - mu_buff[id]  != 0){
          vcl_cout << " ind_id = " << ind_id
                   << " cam_id = " << cam_id
                   << " obj_id = " << obj_id
                   << " id = " << id
                   << "\t score_order = " << score_order_all[id]
                   << "\t score_min = " << score_min_all[id]
                   << "\t score_ori = " << score_ori_all[id]
                   << vcl_endl;
      }
#endif
    // check the score output
  for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
    for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
      unsigned id = cam_id + ind_id * nc;
      vcl_cout << " ind = " << ind_id << " cam = " << cam_id << " id = " << id
               << "\t score_cpu = " << score_all[id]
               << "\t score_gpu = " << score_buff[id]
               << "\t diff = " << score_all[id] - score_buff[id]
               << vcl_endl;
    }
  }
  return true;
}