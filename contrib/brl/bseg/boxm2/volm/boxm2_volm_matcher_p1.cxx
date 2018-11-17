// This is brl/bseg/boxm2/volm/boxm2_volm_matcher_p1.cxx
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <utility>
#include "boxm2_volm_matcher_p1.h"
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define GBYTE 1073741824

boxm2_volm_matcher_p1::boxm2_volm_matcher_p1(volm_camera_space_sptr const& cam_space,
                                             volm_query_sptr const& query,
                                             std::vector<volm_geo_index_node_sptr>  leaves,
                                             float const& buffer_capacity,
                                             std::string const& geo_index_folder,
                                             unsigned const& tile_id,
                                             std::vector<float>  depth_interval,
                                             vgl_polygon<double> const& cand_poly,
                                             const bocl_device_sptr& gpu,
                                             bool const& is_candidate,
                                             bool const& is_last_pass,
                                             std::string  out_folder,
                                             float const& threshold,
                                             unsigned const& max_cam_per_loc,
                                             std::vector<volm_weight> weights)
: cam_space_(cam_space), query_(query), leaves_(std::move(leaves)), ind_buffer_(buffer_capacity),
  weights_(std::move(weights)),
  fallback_size_buff_(nullptr), layer_size_buff_(nullptr), is_candidate_(is_candidate), cand_poly_(cand_poly),
  is_last_pass_(is_last_pass), out_folder_(std::move(out_folder)), depth_interval_(std::move(depth_interval)),
  gpu_(gpu), is_grd_reg_(true), is_sky_reg_(true), is_obj_reg_(true), n_cam_(nullptr), n_obj_(nullptr),
  grd_id_buff_(nullptr), grd_dist_buff_(nullptr), grd_land_buff_(nullptr), grd_land_wgt_buff_(nullptr),
  grd_id_offset_buff_(nullptr), grd_weight_buff_(nullptr), grd_wgt_attri_buff_(nullptr),
  sky_id_buff_(nullptr), sky_id_offset_buff_(nullptr), sky_weight_buff_(nullptr),
  obj_id_buff_(nullptr), obj_id_offset_buff_(nullptr), obj_min_dist_buff_(nullptr), obj_order_buff_(nullptr),
  obj_weight_buff_(nullptr), obj_wgt_attri_buff_(nullptr),
  obj_orient_buff_(nullptr), obj_land_buff_(nullptr), obj_land_wgt_buff_(nullptr), depth_interval_buff_(nullptr),
  depth_length_buff_(nullptr), threshold_(threshold), max_cam_per_loc_(max_cam_per_loc)
{
  valid_cam_indices_ = cam_space_->valid_indices();
  layer_size_ = query_->get_query_size();

  volm_fallback_label::size(fallback_size_);

  ind_ = new boxm2_volm_wr3db_index(layer_size_, ind_buffer_);
  ind_orient_ = new boxm2_volm_wr3db_index(layer_size_, ind_buffer_);
  ind_label_ = new boxm2_volm_wr3db_index(layer_size_, ind_buffer_);
  //ind_combine_ = new boxm2_volm_wr3db_index(layer_size_, ind_buffer_);
  file_name_pre_ << geo_index_folder << "geo_index_tile_" << tile_id;

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
  if (fallback_size_buff_)  delete     fallback_size_buff_;
  if (layer_size_buff_)     delete        layer_size_buff_;
  if (depth_length_buff_)   delete      depth_length_buff_;
  if (grd_id_buff_)         delete []         grd_id_buff_;
  if (grd_dist_buff_)       delete []       grd_dist_buff_;
  if (grd_land_buff_)       delete []       grd_land_buff_;
  if (grd_land_wgt_buff_)   delete []   grd_land_wgt_buff_;
  if (grd_id_offset_buff_)  delete []  grd_id_offset_buff_;
  if (grd_weight_buff_)     delete        grd_weight_buff_;
  if (grd_wgt_attri_buff_)  delete []  grd_wgt_attri_buff_;
  if (sky_id_buff_)         delete []         sky_id_buff_;
  if (sky_id_offset_buff_)  delete []  sky_id_offset_buff_;
  if (sky_weight_buff_)     delete        sky_weight_buff_;
  if (obj_id_buff_)         delete []         obj_id_buff_;
  if (obj_id_offset_buff_)  delete []  obj_id_offset_buff_;
  if (obj_min_dist_buff_)   delete []   obj_min_dist_buff_;
  if (obj_orient_buff_)     delete []     obj_orient_buff_;
  if (obj_order_buff_)      delete []      obj_order_buff_;
  if (obj_land_buff_)       delete []       obj_land_buff_;
  if (obj_land_wgt_buff_)   delete []   obj_land_wgt_buff_;
  if (obj_weight_buff_)     delete []     obj_weight_buff_;
  if (obj_wgt_attri_buff_)  delete []  obj_wgt_attri_buff_;
  if (depth_interval_buff_) delete [] depth_interval_buff_;
}

bool boxm2_volm_matcher_p1::volm_matcher_p1(int const& num_locs_to_kernel)
{
  n_cam_ = new unsigned;
  *n_cam_ = (unsigned)query_->get_cam_num();
  n_cam_cl_mem_ = new bocl_mem(gpu_->context(), n_cam_, sizeof(unsigned), " n_cam " );
  if (!n_cam_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    std::cerr << "\n ERROR: creating bocl_mem failed for N_CAM\n";
    delete n_cam_cl_mem_;
    return false;
  }
  n_obj_ = new unsigned;
  *n_obj_ = (unsigned)(query_->depth_regions()).size();
  n_obj_cl_mem_ = new bocl_mem(gpu_->context(), n_obj_, sizeof(unsigned), " n_obj " );
  if (!n_obj_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    std::cerr << "\n ERROR: creating bocl_mem failed for N_OBJ\n";
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
    std::cerr << "\n ERROR: transferring query to 1D structure failed.\n";
    return false;
  }

  std::cout << "\t 4.1.1 Setting up query in pass 1 matcher for GPU ------> \t" << trans_query_time.all()/1000.0 << " seconds." << std::endl;
  // create queue
  if (!this->create_queue()) {
    std::cerr << "\n ERROR: creating pass 1 matcher queue failed.\n";
    return false;
  }

  // create depth_interval
  depth_interval_buff_ = new float[depth_interval_.size()];
  for (unsigned i = 0; i < depth_interval_.size(); i++)
    depth_interval_buff_[i] = depth_interval_[i];

  depth_interval_cl_mem_ = new bocl_mem(gpu_->context(), depth_interval_buff_, sizeof(float)*((unsigned)depth_interval_.size()), " depth_interval ");
  if (!depth_interval_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    std::cerr << "\n ERROR: creating bocl_mem failed for DEPTH_INTERVAL\n";
    this->clean_query_cl_mem();
    delete depth_interval_cl_mem_;
    return false;
  }
  depth_length_buff_ = new unsigned;
  *depth_length_buff_ = (unsigned)depth_interval_.size();
  depth_length_cl_mem_ = new bocl_mem(gpu_->context(), depth_length_buff_, sizeof(unsigned), " depth_length ");
  if (!depth_length_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    std::cerr << "\n ERROR: creating bocl_mem failed for DEPTH_LENGTH\n";
    this->clean_query_cl_mem();
    delete depth_interval_cl_mem_;    delete depth_length_cl_mem_;
    return false;
  }

  // create cl_mem for layer_size
  layer_size_buff_ = new unsigned;
  *layer_size_buff_ = layer_size_;
  layer_size_cl_mem_ = new bocl_mem(gpu_->context(), layer_size_buff_, sizeof(unsigned), " layer_size ");
  if (!layer_size_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    std::cerr << "\n ERROR: creating bocl_mem failed for LAYER_SIZE\n";
    this->clean_query_cl_mem();
    delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
    delete layer_size_cl_mem_;
    return false;
  }

  // create cl_mem for fallback_size
  fallback_size_buff_ = new unsigned char;
  *fallback_size_buff_ = fallback_size_;
  fallback_size_cl_mem_ = new bocl_mem(gpu_->context(), fallback_size_buff_, sizeof(unsigned char), " fallback size ");
  if (!fallback_size_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    std::cerr << "\n ERROR: creating bocl_mem failed for FALLBACK SIZE\n";
    this->clean_query_cl_mem();
    delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
    delete layer_size_cl_mem_;
    delete fallback_size_cl_mem_;
    return false;
  }

  // compile the kernel
  std::string identifier = gpu_->device_identifier();
  if (kernels_.find(identifier) == kernels_.end()) {
    std::cout << "\t 4.1.2 Comipling kernels for device " << identifier << std::endl;
    std::vector<bocl_kernel*> ks;
    if (!this->compile_kernel(ks)) {
      std::cerr << "\n ERROR: compiling matcher kernel failed.\n";
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;    delete depth_length_cl_mem_;
      return false;
    }
    kernels_[identifier] = ks;
  }

  // calculate available memory space for indices , not query_global_mem_ includes everything previously defined
  cl_ulong avail_global_mem = device_global_mem_ - query_global_mem_ ;
  auto extra_global_mem = (cl_ulong)(1.5*GBYTE);  // leave extra 1.5 GB space for kernel to run
  if (avail_global_mem < extra_global_mem) {
    std::cerr << "\n ERROR: available memory is smaller than pre-defined extra memory, reduce the extra memory space (current value = "
             << extra_global_mem / GBYTE  << ")\n";
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
    std::cerr << "\n ERROR: available memory can not take a single index, reduce the extra memory space (current value = "
             << extra_global_mem / GBYTE  << ")\n";
    return false;
  }

  // define the number of location that will be passed into kernel, if argument num_locs_to_kernel is given
  // the ni is defined by user input. otherwise, it will be calculated based on the index size and number of trial
  // camera at each location
  //  Note the num_locs_to_kernel should never exceed the memory limitation of GPU card


  unsigned ni;
  if (num_locs_to_kernel > 0)
    ni = num_locs_to_kernel;
  else {
    if (layer_size_ < 20000) {
      if (*n_cam_ < 10000)      ni = 64;
      else if (*n_cam_ < 15000) ni = 32;
      else if (*n_cam_ < 20000) ni = 16;
      else                      ni = 16;
    }
    else {
      if (*n_cam_ < 10000)      ni = 8;
      else if (*n_cam_ < 15000) ni = 4;
      else                      ni = 2;
    }
  }
  std::cout << "\t 4.1.3: device have total " << device_global_mem_ << " Byte (" << (float)device_global_mem_/(float)GBYTE << " GB) memory space\n"
           << "\t        query requires " << query_global_mem_ << " Byte (" << (float)query_global_mem_/(float)GBYTE << " GB)\n"
           << "\t        leave " << extra_global_mem  << " Byte (" << (float)extra_global_mem/(float)GBYTE << " GB) extra empty space on device\n"
           << "\t        a single index require " << per_index_mem << " Byte given " << nc << " cameras and " << no << " objects\n"
           << "\t        ---> kernel can calcualte " << ni << " indices per lunching" << std::endl;

  // define the work group size and NDRange dimension
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

  // initialize the indices
  std::string index_dst_file = leaves_[leaf_id]->get_index_name(file_name_pre_.str());
  std::string index_ori_file = leaves_[leaf_id]->get_label_index_name(file_name_pre_.str(), "orientation");
  std::string index_lnd_file = leaves_[leaf_id]->get_label_index_name(file_name_pre_.str(), "land");
#if 0
  std::cerr << " index_combine_file = " << index_combine_file << '\n';
#endif
  ind_->initialize_read(index_dst_file);
  ind_orient_->initialize_read(index_ori_file);
  ind_label_->initialize_read(index_lnd_file);

  while (leaf_id < leaves_.size())
  {
    auto* index_buff_ = new unsigned char[ni*layer_size_];
    //unsigned char* index_combine_buff_ = new unsigned char[ni*layer_size_];
    auto* index_orient_buff_ = new unsigned char[ni*layer_size_];
    auto* index_land_buff_ = new unsigned char[ni*layer_size_];

    // fill the index buffer
    std::vector<unsigned> l_id;  // leaf_id for indices filled into buffer
    std::vector<unsigned> h_id;  // hypo_id in this leaf_id for indices filled into buffer
    unsigned actual_n_ind = ni; // handling the last round where the number of loaded indices is smaller than pre-computed ni


    if (!this->fill_index(ni, layer_size_, leaf_id, index_buff_, index_orient_buff_, index_land_buff_, l_id, h_id, actual_n_ind) ) {
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
      delete layer_size_cl_mem_;
      delete fallback_size_cl_mem_;
      delete [] index_buff_;  delete [] index_orient_buff_;  delete [] index_land_buff_;
      //delete [] index_combine_buff_;
      return false;
    }

    // resize the index buffer if actual loaded index size is smaller than pre-defined
    if (actual_n_ind != ni)
      ni = actual_n_ind;
    if (ni == 0) {
      continue;
    }

    total_index_num += ni;
    auto* score_buff_ = new float[ni*nc];
    auto*    mu_buff_ = new float[ni*no*nc];
    auto* n_ind_ = new unsigned;
    *n_ind_ = ni;
    bocl_mem* n_ind_cl_mem_ = new bocl_mem(gpu_->context(), n_ind_, sizeof(unsigned), " n_ind ");
    if (!n_ind_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for N_IND\n";
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
      delete layer_size_cl_mem_;
      delete fallback_size_cl_mem_;
      delete n_ind_cl_mem_;     delete n_ind_;
      delete [] index_buff_;    delete [] score_buff_;    delete [] mu_buff_;
      delete [] index_orient_buff_;  delete [] index_land_buff_;
      //delete [] index_combine_buff_;
      return false;
    }

    // define work group size based on number of cameras, nc,  and number of indices, ni;
    auto cl_ni = (cl_ulong)RoundUp(*n_ind_, (int)local_threads_[0]);   // row is index
    auto cl_nj = (cl_ulong)RoundUp(*n_cam_, (int)local_threads_[1]);   // col is camera
    global_threads_[0] = cl_ni;
    global_threads_[1] = cl_nj;

    if (round_cnt % 1000 == 0)
      std::cout << " --------  in round " << round_cnt << " ------\n"
               << " Number of indices have been matched = " << total_index_num << '\n'
               << " Giving " << nc << " camera hypos per location and " << ni << " locations pre lunching\n"
               << " NDRange structure:\n"
               << " \t dimension = " << work_dim_ << '\n'
               << " \t work group size = (" << local_threads_[0] << ", " << local_threads_[1] << ")\n"
               << " \t number of work item =  (" << global_threads_[0] << ", " << global_threads_[1] << ")\n"
               << " \t number of work group = (" << global_threads_[0]/local_threads_[0]
               << ", " << global_threads_[1]/local_threads_[1] << ')' << std::flush
               << std::endl;
    round_cnt++;
#if 0
    // check loaded indices and associated ids
    std::cout << " -------> leaf_id_updated = " << leaf_id << std::endl;
    for (unsigned i = 0; i < ni; i++) {
      std::cout << " i = " << i
               << ", leaf_id = " << l_id[i]
               << ", hypo_id = " << h_id[i]
               << ",\nindex_depth\t";
      unsigned start = i*layer_size_;
      unsigned end = (i+1)*layer_size_;
      for (unsigned j = start; j < end; j++)
        std::cout << ' ' << (int)index_buff_[j];
      std::cout << ",\nindex_orient\t";
      for (unsigned j = start; j < end; j++)
        std::cout << ' ' << (int)index_orient_buff_[j];
      std::cout << '\n';
    }
#endif

    // create cl_mem_ for index, score and mu
    // Note: here the data passed into device may be smaller than the pre-assigned index_buff size (since actual_n_ind < pre-calculated ni)
    bocl_mem* index_cl_mem_ = new bocl_mem(gpu_->context(), index_buff_, sizeof(unsigned char)*ni*layer_size_, " index ");
    if (!index_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for INDEX\n";
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
      delete layer_size_cl_mem_;
      delete fallback_size_cl_mem_;
      delete n_ind_cl_mem_;     delete n_ind_;
      delete index_cl_mem_;
      delete [] index_buff_;    delete [] score_buff_;    delete [] mu_buff_;
      delete [] index_orient_buff_;  delete [] index_land_buff_;
      return false;
    }

#if 0
    // create combined index (both orientation and land type, decouple them inside kernel)
    bocl_mem* index_combine_cl_mem_ = new bocl_mem(gpu_->context(), index_combine_buff_, sizeof(unsigned char)*ni*layer_size_, " index_combine ");
    if (!index_combine_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for INDEX_COMBINE\n";
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
      delete layer_size_cl_mem_;
      delete fallback_size_cl_mem_;
      delete n_ind_cl_mem_;     delete [] n_ind_;
      delete index_cl_mem_;     delete index_combine_cl_mem_;
      delete [] index_buff_;    delete [] score_buff_;    delete [] mu_buff_;
      delete [] index_combine_buff_;
    }
#endif
#if 1
    // create index orientation
    bocl_mem* index_orient_cl_mem_ = new bocl_mem(gpu_->context(), index_orient_buff_, sizeof(unsigned char)*ni*layer_size_, " index_orient ");
    if (!index_orient_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for INDEX_ORIENT\n";
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
      delete layer_size_cl_mem_;
      delete fallback_size_cl_mem_;
      delete n_ind_cl_mem_;     delete n_ind_;
      delete index_cl_mem_;
      delete [] index_buff_;    delete [] score_buff_;    delete [] mu_buff_;
      delete [] index_orient_buff_; delete index_orient_cl_mem_;
      return false;
    }

    // create index land cl_mem
    bocl_mem* index_land_cl_mem_ = new bocl_mem(gpu_->context(), index_land_buff_, sizeof(unsigned char)*ni*layer_size_, " index_land ");
    if (!index_land_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for INDEX_LAND\n";
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
      delete layer_size_cl_mem_;
      delete fallback_size_cl_mem_;
      delete n_ind_cl_mem_;         delete n_ind_;
      delete index_cl_mem_;         delete [] index_buff_;
      delete index_orient_cl_mem_;  delete [] index_orient_buff_;
      delete index_land_cl_mem_;    delete [] index_land_buff_;
      delete [] score_buff_;        delete [] mu_buff_;
      return false;
    }
#endif // 0

    // create score cl_mem
    bocl_mem* score_cl_mem_ = new bocl_mem(gpu_->context(), score_buff_, sizeof(float)*ni*nc, " score ");
    if (!score_cl_mem_->create_buffer( CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for SCORE\n";
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
      delete layer_size_cl_mem_;
      delete fallback_size_cl_mem_;
      delete n_ind_cl_mem_;          delete n_ind_;
      delete index_cl_mem_;          delete [] index_buff_;
      /*delete index_combine_cl_mem_;  delete [] index_combine_buff_;*/
      delete index_orient_cl_mem_;   delete [] index_orient_buff_;
      delete index_land_cl_mem_;     delete [] index_land_buff_;
      delete score_cl_mem_;          delete [] score_buff_;
      delete [] mu_buff_;
      return false;
    }

    // create mu cl_mem
    bocl_mem* mu_cl_mem_ = new bocl_mem(gpu_->context(), mu_buff_, sizeof(float)*ni*nc*no, " mu ");
    if (!mu_cl_mem_->create_buffer( CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for MU\n";
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
      delete layer_size_cl_mem_;
      delete fallback_size_cl_mem_;
      delete n_ind_cl_mem_;          delete n_ind_;
      delete index_cl_mem_;          delete [] index_buff_;
      /*delete index_combine_cl_mem_;  delete [] index_combine_buff_;*/
      delete index_orient_cl_mem_;   delete [] index_orient_buff_;
      delete index_land_cl_mem_;     delete [] index_land_buff_;
      delete score_cl_mem_;          delete [] score_buff_;
      delete mu_cl_mem_;             delete [] mu_buff_;
      return false;
    }

    // start the obj_based kernel matcher
    std::string identifier = gpu_->device_identifier();
    if (!this->execute_matcher_kernel_orient(gpu_, queue_, kernels_[identifier], n_ind_cl_mem_,
                                             index_cl_mem_,
                                             index_orient_cl_mem_,
                                             index_land_cl_mem_,
                                             score_cl_mem_,
                                             mu_cl_mem_))
    {
      std::cerr << "\n ERROR: executing pass 1 kernel(with orientation) failed on device " << identifier << '\n';
      this->clean_query_cl_mem();
      delete depth_interval_cl_mem_;        delete depth_length_cl_mem_;
      delete layer_size_cl_mem_;
      delete fallback_size_cl_mem_;
      delete n_ind_cl_mem_;          delete n_ind_;
      delete index_cl_mem_;          delete [] index_buff_;
      //delete index_combine_cl_mem_;  delete [] index_combine_buff_;
      delete index_orient_cl_mem_;   delete [] index_orient_buff_;
      delete index_land_cl_mem_;     delete [] index_land_buff_;
      delete score_cl_mem_;          delete [] score_buff_;
      delete mu_cl_mem_;             delete [] mu_buff_;
      return false;
    }

    // block everything to ensure the reading score
    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, " release N_INDEX failed on device " + gpu_->device_identifier() + error_to_string(status));
    // read score
    score_cl_mem_->read_to_buffer(queue_);
    //mu_cl_mem_->read_to_buffer(queue_);
    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, " release N_INDEX failed on device " + gpu_->device_identifier() + error_to_string(status));
    // count time
    if (is_grd_reg_ && is_sky_reg_)
      gpu_matcher_time += kernels_[identifier][0]->exec_time();
    else if (!is_grd_reg_ && is_sky_reg_)
      gpu_matcher_time += kernels_[identifier][1]->exec_time();
    else if (is_grd_reg_ && !is_sky_reg_)
      gpu_matcher_time += kernels_[identifier][2]->exec_time();
    else
      gpu_matcher_time += kernels_[identifier][3]->exec_time();

    // post-processing data
    for (unsigned ind_id = 0; ind_id < ni; ind_id++)
    {
      // for each location, find maximum score, top cameras and best camera giving max_score
      float max_score = 0.0f;
      unsigned max_cam_id = 0;
      float min_score_in_list = 0.0f;
      std::vector<unsigned> cam_ids;
      std::vector<float> cam_scores;
      for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
        unsigned id = cam_id + ind_id*nc;
        float s = score_buff_[id];
        // find the max_score and max_cam_id
        if (s > max_score) {
          max_score = s;  max_cam_id = valid_cam_indices_[cam_id];
        }
        // decide to keep the camera or not
        if (s > threshold_) {
          if (cam_ids.size() < max_cam_per_loc_) {
            cam_ids.push_back(valid_cam_indices_[cam_id]);
            cam_scores.push_back(s);
          }
          else if (s > min_score_in_list) {
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
            cam_ids[min_score_id] = valid_cam_indices_[cam_id];
          }
        }
      }
      // put score date of location ind_id to score_all
      score_all_.push_back(new volm_score(l_id[ind_id], h_id[ind_id], max_score, max_cam_id, cam_ids));
      score_cam_.emplace_back(l_id[ind_id], h_id[ind_id], cam_ids, cam_scores);
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
    //delete index_combine_cl_mem_;
    //status = clFinish(queue_);
    //check_val(status, MEM_FAILURE, " release INDEX_COMBINE failed on device " + gpu_->device_identifier() + error_to_string(status));
    delete index_orient_cl_mem_;
    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, " release INDEX_ORIENT failed on device " + gpu_->device_identifier() + error_to_string(status));
    delete index_land_cl_mem_;
    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, " release INDEX_LAND failed on device " + gpu_->device_identifier() + error_to_string(status));

    // do the test
#if 0
    if (ni < 16)
      this->volm_matcher_p1_test_ori(ni, index_buff_, index_orient_buff_, index_land_buff_, score_buff_, mu_buff_);
#endif

    // finish current round, clean host memory
    delete n_ind_;
    delete [] index_buff_;    delete [] score_buff_;    delete [] mu_buff_;
    delete [] index_orient_buff_;  delete [] index_land_buff_;
    //delete [] index_combine_buff_;
  } // end of loop over all leaves

  // time evaluation
  auto total_time = (float)total_matcher_time.all();
  std::cout << "\t\t total time for " << total_index_num << " indices and " << *n_cam_ << " cameras -------> " << total_time/1000.0 << " seconds (" << total_time << " ms)\n"
           << "\t\t GPU kernel execution ------------------> " << gpu_matcher_time/1000.0 << " seconds (" << gpu_matcher_time << " ms)\n"
           << "\t\t CPU host execution --------------------> " << (total_time - gpu_matcher_time)/1000.0 << " seconds (" << total_time - gpu_matcher_time << " ms)" << std::endl;

  clReleaseCommandQueue(queue_);
  // clear query_cl_mem
  this->clean_query_cl_mem();
  delete depth_interval_cl_mem_;
  delete depth_length_cl_mem_;
  delete layer_size_cl_mem_;
  delete fallback_size_cl_mem_;
  // finalize the index
  ind_->finalize();
  ind_orient_->finalize();
  ind_label_->finalize();
  //ind_combine_->finalize();
  return true;
}

bool boxm2_volm_matcher_p1::fill_index(unsigned const& n_ind,
                                       unsigned const& layer_size,
                                       unsigned& leaf_id,
                                       unsigned char* index_buff,
                                       unsigned char* index_orient_buff,
                                       unsigned char* index_land_buff,
                                       std::vector<unsigned>& l_id,
                                       std::vector<unsigned>& h_id,
                                       unsigned& actual_n_ind)
{
  if (is_last_pass_) {
    std::cerr << " pass 1 check whether we have last_pass is NOT implemented yet ...\n";
    return false;
  }
  else
  {
    // for no previous output case
    unsigned cnt = 0;
    unsigned li;
    for (li = leaf_id; li < leaves_.size(); li++) {
      if (!leaves_[li]->hyps_ )
        continue;
      vgl_point_3d<double> h_pt;
      while (cnt < n_ind && leaves_[li]->hyps_->get_next(0,1,h_pt) ) {
        if (is_candidate_) {
          //if (cand_poly_.contains(h_pt.x(), h_pt.y())) {  // having candidate list and current hypo is inside it --> accept
          if (this->inside_candidate(cand_poly_, h_pt.x(), h_pt.y())) {
            unsigned char* values = index_buff + cnt * layer_size;
            unsigned char* values_ori = index_orient_buff + cnt * layer_size;
            unsigned char* values_lnd = index_land_buff + cnt * layer_size;
            //unsigned char* values_combine = index_combine_buff + cnt * layer_size;
            ind_->get_next(values, layer_size);
            ind_orient_->get_next(values_ori, layer_size);
            ind_label_->get_next(values_lnd, layer_size);
            //ind_combine_->get_next(values_combine, layer_size);
            cnt++;
#if 0
            std::cout << "=> leaf_id = " << li
                     << ", hypo_id = " << leaves_[li]->hyps_->current_-1
                     << ", h_pt = " << h_pt << std::endl;
#endif
            l_id.push_back(li);  h_id.push_back(leaves_[li]->hyps_->current_-1);
          }
          else {                                       // having candidate list but current hypo is outside candidate list --> ignore
            std::vector<unsigned char> values(layer_size);
            std::vector<unsigned char> values_ori(layer_size);
            std::vector<unsigned char> values_lnd(layer_size);
            ind_->get_next(values);
            ind_orient_->get_next(values_ori);
            ind_label_->get_next(values_lnd);
            //std::vector<unsigned char> values_combine(layer_size);
            //ind_combine_->get_next(values_combine);
          }
        }
        else {                                         // no candidate list, put all indices into buffer
          unsigned char* values = index_buff + cnt * layer_size;
          unsigned char* values_ori = index_orient_buff + cnt * layer_size;
          unsigned char* values_lnd = index_land_buff + cnt * layer_size;
          ind_->get_next(values, layer_size);
          ind_orient_->get_next(values_ori, layer_size);
          ind_label_->get_next(values_lnd, layer_size);
          //unsigned char* values_combine = index_combine_buff + cnt * layer_size;
          //ind_combine_->get_next(values_combine, layer_size);
          cnt++;
          l_id.push_back(li);  h_id.push_back(leaves_[li]->hyps_->current_-1);
        }
      }
      if (cnt == n_ind) {
        leaf_id = li;
        break;
      }
      else {
        if (is_leaf_finish(li)) {
          ind_->finalize();
          ind_orient_->finalize();
          ind_label_->finalize();
          //ind_combine_->finalize();
          if (li < leaves_.size()-1) {
            ind_->initialize_read(leaves_[li+1]->get_index_name(file_name_pre_.str()));
            ind_orient_->initialize_read(leaves_[li+1]->get_label_index_name(file_name_pre_.str(), "orientation"));
            ind_label_->initialize_read(leaves_[li+1]->get_label_index_name(file_name_pre_.str(), "land"));
            //ind_orient_->finalize();
            //ind_label_->finalize();
            //ind_combine_->initialize_read(leaves_[li+1]->get_label_index_name(file_name_pre_.str(), "combined"));
#if 0
            std::cerr << " leaf = " << li+1 << ", combine_file = " << leaves_[li+1]->get_label_index_name(file_name_pre_.str(), "combined") << '\n';
#endif
          }
        }
      }
    } // loop over all leaves
    if (li == leaves_.size())
      leaf_id = li;
    if (cnt != n_ind)
      actual_n_ind = cnt;
    return true;
  }
}

// check if the hypothesis inside given leaf has been loaded
bool boxm2_volm_matcher_p1::is_leaf_finish(unsigned const& leaf_id)
{
  return leaves_[leaf_id]->hyps_->current_  ==  leaves_[leaf_id]->hyps_->size();
}

// execute kernel with orientation considered
bool boxm2_volm_matcher_p1::execute_matcher_kernel_orient(const bocl_device_sptr&                  device,
                                                          cl_command_queue&                  queue,
                                                          std::vector<bocl_kernel*>        kern_vec,
                                                          bocl_mem*                  n_ind_cl_mem_,
                                                          bocl_mem*                  index_cl_mem_,
                                                          bocl_mem*           index_orient_cl_mem_,
                                                          bocl_mem*             index_land_cl_mem_,
                                                          bocl_mem*                  score_cl_mem_,
                                                          bocl_mem*                     mu_cl_mem_)
{
  // create a debug buffer
  cl_int status;
  unsigned debug_size = 100;
  auto* debug_buff_ = new float[debug_size];
  std::fill(debug_buff_, debug_buff_+debug_size, (float)12.31);
  bocl_mem* debug_cl_mem_ = new bocl_mem(gpu_->context(), debug_buff_, sizeof(float)*debug_size, " debug ");
  if (!debug_cl_mem_->create_buffer( CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR )) {
    std::cerr << "\n ERROR: creating bocl_mem failed for DEBUG\n";
    delete debug_cl_mem_;     delete [] debug_buff_;
    return false;
  }

  bocl_kernel* kern;
  // choose whether kernel to use
  if (is_grd_reg_ && is_sky_reg_) {              // both sky and ground
#if 0
    std::cout << "\t using both sky and grd kernel with orientatl attribute" << std::endl;
#endif
    kern = kern_vec[0];
    // set up argument list
    kern->set_arg(n_cam_cl_mem_);
    kern->set_arg(n_obj_cl_mem_);
    kern->set_arg(grd_id_cl_mem_);        kern->set_arg(grd_id_offset_cl_mem_);
    kern->set_arg(grd_dist_cl_mem_);
    kern->set_arg(grd_land_cl_mem_);      kern->set_arg(grd_land_wgt_cl_mem_);
    kern->set_arg(grd_weight_cl_mem_);    kern->set_arg(grd_wgt_attri_cl_mem_);
    kern->set_arg(sky_id_cl_mem_);        kern->set_arg(sky_id_offset_cl_mem_);    kern->set_arg(sky_weight_cl_mem_);
  }
  else if ( !is_grd_reg_ && is_sky_reg_) {     // no ground but sky
#if 0
    std::cout << "\t using NO grd kernel with orientation attribute" << std::endl;
#endif
    kern = kern_vec[1];
    // set up argument list
    kern->set_arg(n_cam_cl_mem_);
    kern->set_arg(n_obj_cl_mem_);
    kern->set_arg(sky_id_cl_mem_);        kern->set_arg(sky_id_offset_cl_mem_);    kern->set_arg(sky_weight_cl_mem_);

#if 0
    // check the global memory
    std::cerr << " ------------ INSIDE execute_kernel -------------------------------\n"
             << " n_cam_cl_mem = " << n_cam_cl_mem_->num_bytes() << '\n'
             << " n_obj_cl_mem = " << n_obj_cl_mem_->num_bytes() << '\n'
             << " sky_id_cl_mem_ = " << sky_id_cl_mem_->num_bytes() << '\n'
             << " sky_offset_cl_mem_ = " << sky_id_offset_cl_mem_->num_bytes() << '\n'
             << " sky_weight_cl_mem_ = " << sky_weight_cl_mem_->num_bytes() << '\n';
#endif
  }
  else if (  is_grd_reg_ && !is_sky_reg_) {    // no sky but ground
#if 0
    std::cout << "\t using NO sky kernel with orientation attribute" << std::endl;
#endif
    kern = kern_vec[2];
    // set up argument list
    kern->set_arg(n_cam_cl_mem_);
    kern->set_arg(n_obj_cl_mem_);
    kern->set_arg(grd_id_cl_mem_);        kern->set_arg(grd_id_offset_cl_mem_);
    kern->set_arg(grd_dist_cl_mem_);
    kern->set_arg(grd_land_cl_mem_);      kern->set_arg(grd_land_wgt_cl_mem_);
    kern->set_arg(grd_weight_cl_mem_);    kern->set_arg(grd_wgt_attri_cl_mem_);
  }
  else {                                       // neither sky nor ground
#if 0
    std::cout << "\t using NO grd NOR sky kernel" << std::endl;
#endif
    kern = kern_vec[3];
    kern->set_arg(n_cam_cl_mem_);
    kern->set_arg(n_obj_cl_mem_);
  }

  // set up argument list
  kern->set_arg(obj_id_cl_mem_);
  kern->set_arg(obj_id_offset_cl_mem_);
  kern->set_arg(obj_min_dist_cl_mem_);
  kern->set_arg(obj_orient_cl_mem_);
  kern->set_arg(obj_land_cl_mem_);
  kern->set_arg(obj_land_wgt_cl_mem_);
  kern->set_arg(obj_weight_cl_mem_);
  kern->set_arg(obj_wgt_attri_cl_mem_);
  kern->set_arg(n_ind_cl_mem_);
  kern->set_arg(layer_size_cl_mem_);
  kern->set_arg(fallback_size_cl_mem_);
  kern->set_arg(index_cl_mem_);
  kern->set_arg(index_orient_cl_mem_);
  kern->set_arg(index_land_cl_mem_);
  kern->set_arg(score_cl_mem_);
  kern->set_arg(mu_cl_mem_);
  kern->set_arg(depth_interval_cl_mem_);
  kern->set_arg(depth_length_cl_mem_);
  kern->set_arg(debug_cl_mem_);
  // set up local memory argument
  kern->set_local_arg((*n_obj_)*sizeof(unsigned char));       // local memory for obj_min_dist
  kern->set_local_arg((*n_obj_)*sizeof(unsigned char));       // local memory for orientation
  kern->set_local_arg((*n_obj_)*sizeof(unsigned char)*4);    // local memory for land fallback category
  kern->set_local_arg((*n_obj_)*sizeof(float)*4);
  kern->set_local_arg((*n_obj_)*sizeof(float));               // local memory for obj_weight
  kern->set_local_arg((*n_obj_)*sizeof(float)*4);             // local memory for obj_wgt_attri
  if (is_grd_reg_) {
    kern->set_local_arg(3*(*n_obj_)*sizeof(float));           // local memory for grd_wgt_attri
  }
  kern->set_local_arg((*depth_length_buff_)*sizeof(float));   // local memory for depth_interval table

#if 0
  std::cerr << " obj_id = " << obj_id_cl_mem_->num_bytes() << '\n'
           << " obj_offset = " << obj_id_offset_cl_mem_->num_bytes() << '\n'
           << " obj_min_dist = " << obj_min_dist_cl_mem_->num_bytes() << '\n'
           << " obj_weight = " << obj_weight_cl_mem_->num_bytes() << '\n'
           << " n_ind = " << n_ind_cl_mem_->num_bytes() << '\n'
           << " lyaer_size = " << layer_size_cl_mem_->num_bytes() << '\n'
           << " index_cl_mem_ = " << index_cl_mem_->num_bytes() << '\n'
           << " score_cl_mem = " << score_cl_mem_->num_bytes() << '\n'
           << " mu_cl_mem_ = " << mu_cl_mem_->num_bytes() << '\n'
           << " depth_interval = " << depth_interval_cl_mem_->num_bytes() << '\n'
           << " depth_length = " << depth_length_cl_mem_->num_bytes() << '\n'
           << " debug_cl_mem_ = " << debug_cl_mem_->num_bytes() << '\n'
           << " local for min_dist = " << (*n_obj_)*sizeof(unsigned char) << '\n'
           << " local for weight = " << (*n_obj_)*sizeof(float) << '\n'
           << " local for depth_interval = " << (*depth_length_buff_)*sizeof(float) << '\n';
#endif

  // execute kernel
  if (!kern->execute(queue, work_dim_, local_threads_, global_threads_)) {
    std::cerr << "\n ERROR: kernel execution failed\n";
    delete debug_cl_mem_;
    delete[] debug_buff_;
    return false;
  }
  status = clFinish(queue);  // block to ensure kernel finishes
  if (status != 0) {
    std::cerr << "\n ERROR: " << status << "  kernel matcher failed: " + error_to_string(status) << '\n';
    return false;
  }
  // clear bocl_kernel argument list
  kern->clear_args();

#if 0
  // read debug data from device
  debug_cl_mem_->read_to_buffer(queue);
  unsigned i = 0;
  while ((debug_buff_[i]-12.31)*(debug_buff_[i]-12.31)>0.0001 && i < 1000) {
    std::cout << " debug[" << i << "] = " << debug_buff_[i] << std::endl;
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

// clear all query cl_mem pointer
bool boxm2_volm_matcher_p1::clean_query_cl_mem()
{
  delete n_cam_cl_mem_;
  delete n_obj_cl_mem_;
  if ( is_grd_reg_ ) {
    delete grd_id_cl_mem_;   delete grd_dist_cl_mem_;
    delete grd_land_cl_mem_; delete grd_land_wgt_cl_mem_;
    delete grd_id_offset_cl_mem_;
  }
  if ( is_sky_reg_ ) { delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_; }
  if ( is_obj_reg_ ) {
    delete obj_id_cl_mem_;     delete obj_id_offset_cl_mem_;
    delete obj_orient_cl_mem_; delete obj_min_dist_cl_mem_;
    delete obj_land_cl_mem_;   delete obj_land_wgt_cl_mem_;
    delete obj_order_cl_mem_;
  }
  return this->clean_weight_cl_mem();
}

// clear all query weight cl_mem pointer
bool boxm2_volm_matcher_p1::clean_weight_cl_mem()
{
  if ( is_grd_reg_ ) { delete grd_weight_cl_mem_;   delete grd_wgt_attri_cl_mem_; }
  if ( is_sky_reg_ ) { delete sky_weight_cl_mem_; }
  if ( is_obj_reg_ ) { delete obj_weight_cl_mem_;   delete obj_wgt_attri_cl_mem_; }
  return true;
}

// compile kernel
bool boxm2_volm_matcher_p1::compile_kernel(std::vector<bocl_kernel*>& vec_kernels)
{
  // declare the kernel source
  std::vector<std::string> src_paths;
  std::string volm_cl_source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/volm/cl/";
  // add orientation kernels in
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_obj_based_matching_with_orient.cl");
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_obj_based_matching_no_grd_with_orient.cl");
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_obj_based_matching_no_sky_with_orient.cl");
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_obj_based_matching_no_grd_no_sky_with_orient.cl");

  // create the matching orientation kernel
  auto* kern_matcher_with_orient = new bocl_kernel();
  if (!kern_matcher_with_orient->create_kernel(&gpu_->context(), gpu_->device_id(), src_paths,
                                               "generalized_volm_obj_based_matching_with_orient",
                                               "",
                                               "generalized_volm_obj_based_matching_with_orient") )
    return false;
  vec_kernels.push_back(kern_matcher_with_orient);

  // create with_orientation kernel for queries without ground
  auto* kern_matcher_no_grd_with_orient = new bocl_kernel();
  if (!kern_matcher_no_grd_with_orient->create_kernel(&gpu_->context(), gpu_->device_id(), src_paths,
                                                      "generalized_volm_obj_based_matching_no_grd_with_orient",
                                                      "",
                                                      "generalized_volm_obj_based_matching_no_grd_with_orient") )
    return false;
  vec_kernels.push_back(kern_matcher_no_grd_with_orient);

  // create with_orientation kernel for queries without sky
  auto* kern_matcher_no_sky_with_orient = new bocl_kernel();
  if (!kern_matcher_no_sky_with_orient->create_kernel(&gpu_->context(), gpu_->device_id(), src_paths,
                                                      "generalized_volm_obj_based_matching_no_sky_with_orient",
                                                      "",
                                                      "generalized_volm_obj_based_matching_no_sky_with_orient") )
    return false;
  vec_kernels.push_back(kern_matcher_no_sky_with_orient);

  // create with_orientation kernel for queries without sky nor ground
  auto* kern_matcher_no_grd_no_sky_with_orient = new bocl_kernel();
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

bool boxm2_volm_matcher_p1::write_matcher_result(std::string const& tile_fname_bin, std::string const& tile_fname_txt)
{
  // write the output store binary as tile
  // write the scores out as binary
  volm_score::write_scores(score_all_, tile_fname_bin);
  // for testing purpose, generate a temporary txt file for output
  if (vul_file::exists(tile_fname_txt))
    vul_file::delete_file_glob(tile_fname_txt);

  auto all_loc = (unsigned)score_all_.size();
  for (unsigned i = 0; i < all_loc; i++) {
    volm_score_sptr score = score_all_[i];
    std::vector<unsigned> cam_ids = score->cam_id_;
    // write the txt output
    std::ofstream txt_ofs(tile_fname_txt.c_str(), std::ios::app);
    txt_ofs << std::setprecision(6) << score->leaf_id_ << ' ' << score->hypo_id_ << ' ' << score->max_score_ << ' ' << score->max_cam_id_ << '\n';
    for (unsigned int cam_id : cam_ids)
      txt_ofs << ' ' << cam_id;
    txt_ofs << '\n';
    txt_ofs.close();
  }
  return true;
}

bool boxm2_volm_matcher_p1::write_matcher_result(std::string const& tile_fname_bin)
{
  volm_score::write_scores(score_all_, tile_fname_bin);
  return true;
}

// write the score for all cameras whose score higher than threshold
bool boxm2_volm_matcher_p1::write_gt_cam_score(unsigned const& leaf_id, unsigned const& hypo_id, std::string const& out_fname)
{
  std::ofstream ofs(out_fname.c_str());
  for (auto & i : score_cam_) {
    if (i.l_id_ == leaf_id && i.h_id_ == hypo_id) {
      ofs << i.l_id_ << ' ' << i.h_id_ << '\n';
      std::vector<unsigned> cam_ids = i.cam_id_;
      std::vector<float> cam_scores = i.cam_score_;
      for (unsigned jj = 0; jj < cam_ids.size(); jj++) {
        ofs << std::setprecision(6) << cam_ids[jj] << ' ' << cam_scores[jj] << ' ' << query_->get_cam_string(cam_ids[jj]) << '\n';
      }
      return true;
    }
  }
  ofs.close();
  return false;
}

bool boxm2_volm_matcher_p1::transfer_query()
{
  if ( !is_grd_reg_ && !is_sky_reg_ && !is_obj_reg_ ) {
    std::cerr << "\n ERROR: no depth_map_region defined in query image, check the labelme.xml\n";
    return false;
  }
  if (!is_obj_reg_) {
    std::cerr << "\n ERROR: current pass 01 matcher is not able to match query without any non_grd, non_sky object, add at least one in the labelme.xml\n";
    return false;
  }

  query_global_mem_ = 3 * sizeof(unsigned);   // n_cam + n_obj + n_ind
  query_local_mem_  = 3 * sizeof(unsigned);   // n_cam + n_obj + n_ind

  // transfer weight parameter from volm_weights
  if (!this->transfer_weight()) {
    std::cerr << "\n ERROR: failed when transferring volm_weight parameters to 1D weight buffer\n";
    return false;
  }

  // construct the ground_id, ground_dist, ground_offset 1D array
  if (is_grd_reg_)
  {
    unsigned grd_vox_size = query_->get_ground_id_size();
    grd_id_buff_ = new unsigned[grd_vox_size];
    grd_dist_buff_ = new unsigned char[grd_vox_size];
    grd_id_offset_buff_ = new unsigned[*n_cam_+1];
    unsigned grd_count = 0;
    std::vector<std::vector<unsigned> >& grd_id = query_->ground_id();
    std::vector<std::vector<unsigned char> >& grd_dist = query_->ground_dist();
    for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
      grd_id_offset_buff_[cam_id] = grd_count;
      for (unsigned vox_id = 0; vox_id < grd_id[cam_id].size(); vox_id++) {
        unsigned i = grd_count + vox_id;
        grd_id_buff_[i] = grd_id[cam_id][vox_id];
        grd_dist_buff_[i] = grd_dist[cam_id][vox_id];
      }
      grd_count += (unsigned)grd_id[cam_id].size();
    }
    grd_id_offset_buff_[*n_cam_] = grd_count;

    // construct grd_land_buff and grd_land_wgt_buff
    grd_land_buff_ = new unsigned char[grd_vox_size*fallback_size_];
    grd_land_wgt_buff_ = new float[grd_vox_size*fallback_size_];
    std::vector<std::vector<std::vector<unsigned char> > > grd_land = query_->ground_land_id();
    std::vector<std::vector<std::vector<float> > > grd_land_wgt = query_->ground_land_wgt();
    unsigned cnt = 0;
    for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++)
      for (unsigned vox_id = 0; vox_id < grd_land[cam_id].size(); vox_id++)
        for (unsigned lnd_id = 0; lnd_id < fallback_size_; lnd_id++) {
          grd_land_buff_[cnt] = grd_land[cam_id][vox_id][lnd_id];
          grd_land_wgt_buff_[cnt++] = grd_land_wgt[cam_id][vox_id][lnd_id];
        }

#if 0
    // check the grd_land_buff and grd_land_wgt_buff_
    std::cout << " ----------------- date in query ---------------" << std::endl;
    for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
      for (unsigned vox_id = 0; vox_id < grd_land[cam_id].size(); vox_id++) {
        std::cout << " cam_id = " << cam_id << ", vox_id = " << vox_id << " --> lnd_id = [";
        for (std::vector<unsigned char>::iterator vit = grd_land[cam_id][vox_id].begin(); vit != grd_land[cam_id][vox_id].end(); ++vit)
          std::cout << (int)(*vit) << ' ';
        std::cout << "] ---> lnd_wgt = [";
        for (std::vector<float>::iterator vit = grd_land_wgt[cam_id][vox_id].begin(); vit != grd_land_wgt[cam_id][vox_id].end(); ++vit)
          std::cout << *vit << ' ';
        std::cout << ']' << std::endl;
      }
    }

    std::cout << " ++++++++++++++++++ date in 1D buffer ++++++++++++++" << std::endl;
    for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
      /*unsigned land_start = grd_id_offset_buff_[cam_id] * 4;
      unsigned land_end = grd_id_offset_buff_[cam_id+1] * 4;*/
      unsigned vox_start = grd_id_offset_buff_[cam_id];
      unsigned vox_end = grd_id_offset_buff_[cam_id+1];
      for (unsigned vox_id = vox_start; vox_id < vox_end; vox_id++) {
        std::cout << " in buffer, cam_id = " << cam_id << ", vox_global_id = " << vox_id << ", vox_id = " << vox_id - vox_start << " ---> lnd_id = [";
        for (unsigned ii = 0; ii < fallback_size_; ii++) {
          unsigned lnd_id = vox_id * 4 + ii ;
          std::cout << (int)grd_land_buff_[lnd_id] << ' ';
        }
        std::cout << "] ---> lnd_wgt = [";
        for (unsigned ii = 0; ii < fallback_size_; ii++) {
          unsigned lnd_id = vox_id * 4 + ii;
          std::cout << grd_land_wgt_buff_[lnd_id] << ' ';
        }
        std::cout << ']' << std::endl;
      }
    }
#endif

    grd_id_cl_mem_ = new bocl_mem(gpu_->context(), grd_id_buff_, sizeof(unsigned)*grd_vox_size, " grd_id " );
    if (!grd_id_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for GROUND_ID\n";
      this->clean_weight_cl_mem();
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;
      return false;
    }
    grd_dist_cl_mem_ = new bocl_mem(gpu_->context(), grd_dist_buff_, sizeof(unsigned char)*grd_vox_size, " grd_dist " );
    if (!grd_dist_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for GROUND_DIST\n";
      this->clean_weight_cl_mem();
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;
      return false;
    }
    grd_id_offset_cl_mem_ = new bocl_mem(gpu_->context(), grd_id_offset_buff_, sizeof(unsigned)*(*n_cam_+1), " grd_offset " );
    if (!grd_id_offset_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for GROUND_OFFSET\n";
      this->clean_weight_cl_mem();
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;
      return false;
    }
    grd_land_cl_mem_ = new bocl_mem(gpu_->context(), grd_land_buff_, sizeof(unsigned char)*grd_vox_size*fallback_size_, " grd_lnd ");
    if (!grd_land_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for GROUND_LAND\n";
      this->clean_weight_cl_mem();
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;
      delete grd_land_cl_mem_;
      return false;
    }
    grd_land_wgt_cl_mem_ = new bocl_mem(gpu_->context(), grd_land_wgt_buff_, sizeof(float)*grd_vox_size*fallback_size_, " grd_lnd_wgt ");
    if (!grd_land_wgt_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for GROUND_LAND_WGT\n";
      this->clean_weight_cl_mem();
      delete n_cam_cl_mem_;    delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;   delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;
      delete grd_land_cl_mem_; delete grd_land_wgt_cl_mem_;
      return false;
    }

    query_global_mem_ += (sizeof(unsigned char)+sizeof(unsigned))*grd_vox_size;                // ground id and dist
    query_global_mem_ +=  sizeof(unsigned)*(query_->ground_offset().size());                   // ground offset array
    query_global_mem_ += (sizeof(unsigned char) + sizeof(float))*grd_vox_size*fallback_size_;  // ground fallback land category
  }

  // construct sky_id buff and cl_mem
  if (is_sky_reg_)
  {
    unsigned sky_vox_size = query_->get_sky_id_size();
    sky_id_buff_ = new unsigned[sky_vox_size];
    sky_id_offset_buff_ = new unsigned[*n_cam_+1];
    unsigned sky_count = 0;
    std::vector<std::vector<unsigned> >& sky_id = query_->sky_id();
    for (unsigned cam_id = 0; cam_id < *n_cam_; cam_id++) {
      sky_id_offset_buff_[cam_id] = sky_count;
      for (unsigned vox_id = 0; vox_id < sky_id[cam_id].size(); vox_id++) {
        unsigned i = sky_count + vox_id;
        sky_id_buff_[i] = sky_id[cam_id][vox_id];
      }
      sky_count += (unsigned)sky_id[cam_id].size();
    }
    sky_id_offset_buff_[*n_cam_] = sky_count;

    sky_id_cl_mem_ = new bocl_mem(gpu_->context(), sky_id_buff_, sizeof(unsigned)*sky_vox_size, " sky_id " );
    if (!sky_id_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for SKY_ID\n";
      this->clean_weight_cl_mem();
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      if (is_grd_reg_) {
        delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;
        delete grd_land_cl_mem_; delete grd_land_wgt_cl_mem_;
      }
      delete sky_id_cl_mem_;
      return false;
    }
    sky_id_offset_cl_mem_ = new bocl_mem(gpu_->context(), sky_id_offset_buff_, sizeof(unsigned)*(*n_cam_+1), " sky_offset " );
    if (!sky_id_offset_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for SKY_OFFSET\n";
      this->clean_weight_cl_mem();
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      if (is_grd_reg_) {
        delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;
        delete grd_land_cl_mem_; delete grd_land_wgt_cl_mem_;
      }
      delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;
      return false;
    }
    query_global_mem_ += sizeof(unsigned)*sky_vox_size;                          // sky id
    query_global_mem_ += sizeof(unsigned)*query_->sky_offset().size();           // sky offset
  }

  // construct obj_id, obj_offset 1D array
  if (is_obj_reg_)
  {
    unsigned obj_vox_size = query_->get_dist_id_size();
    unsigned obj_offset_size = (*n_cam_) * (*n_obj_);
    obj_id_buff_ = new unsigned[obj_vox_size];
    obj_id_offset_buff_ = new unsigned[obj_offset_size+1];
    std::vector<std::vector<std::vector<unsigned> > >& dist_id = query_->dist_id();
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
    obj_order_buff_ = new unsigned char[*n_obj_];
    for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++) {
      obj_min_dist_buff_[obj_id] = query_->min_obj_dist()[obj_id];
      obj_order_buff_[obj_id] = query_->order_obj()[obj_id];
    }

    // construct land and land_wgt fallback category buffer
    obj_land_buff_ = new unsigned char[*n_obj_*fallback_size_];
    obj_land_wgt_buff_ = new float[*n_obj_*fallback_size_];
    unsigned cnt = 0;
    for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++) {
      for (unsigned ii = 0; ii < fallback_size_; ii++) {
        obj_land_buff_[cnt] = query_->obj_land_id()[obj_id][ii];
        obj_land_wgt_buff_[cnt++] = query_->obj_land_wgt()[obj_id][ii];
      }
    }

#if 0
    // check the constructed 1D obj_land_array
    std::cout << " ---------------- in the volm_query -----------------" << std::endl;
    for (unsigned obj_id = 0; obj_id < *n_obj_; obj_id++) {
      std::cout << " obj_id = " << obj_id << " ---> lnd_id = [";
      for (std::vector<unsigned char>::iterator vit = query_->obj_land_id()[obj_id].begin(); vit != query_->obj_land_id()[obj_id].end(); ++vit)
        std::cout << (int)(*vit) << ' ';
      std::cout << "] ---> lnd_wgt = [";
      for (std::vector<float>::iterator vit = query_->obj_land_wgt()[obj_id].begin(); vit != query_->obj_land_wgt()[obj_id].end(); ++vit)
        std::cout << *vit << ' ';
      std::cout << ']' << std::endl;
    }
    // I am back
    std::cout << " ++++++++++++++++ in the 1D array ++++++++++++++++++++++" << std::endl;
    for (unsigned k = 0; k < *n_obj_; k++) {
      std::cout << " obj_id = " << k << " ---> lnd_id_buff = [";
      unsigned land_start = k*fallback_size_;
      unsigned land_end = (k+1)*fallback_size_;
      for (unsigned ii = land_start; ii < land_end; ii++)
        std::cout << (int)obj_land_buff_[ii] << ' ';
      std::cout << "] ---> lnd_wgt_buff = [";
      for (unsigned ii = land_start; ii < land_end; ii++)
        std::cout << obj_land_wgt_buff_[ii] << ' ';
      std::cout << ']' << std::endl;
    }
#endif

    // create corresponding cl_mem
    obj_id_cl_mem_ = new bocl_mem(gpu_->context(), obj_id_buff_, sizeof(unsigned)*obj_vox_size, " obj_id " );
    if (!obj_id_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for OBJ_ID\n";
      this->clean_weight_cl_mem();
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      if (is_grd_reg_) {
        delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;
        delete grd_land_cl_mem_; delete grd_land_wgt_cl_mem_;
      }
      if (is_obj_reg_) { delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_; }
      delete obj_id_cl_mem_;
      return false;
    }
    obj_id_offset_cl_mem_ = new bocl_mem(gpu_->context(), obj_id_offset_buff_, sizeof(unsigned)*(obj_offset_size+1), " obj_offset " );
    if (!obj_id_offset_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for OBJ_OFFSET\n";
      this->clean_weight_cl_mem();
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      if (is_grd_reg_) {
        delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;
        delete grd_land_cl_mem_; delete grd_land_wgt_cl_mem_;
      }
      if (is_obj_reg_) { delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_; }
      delete obj_id_cl_mem_;  delete obj_id_offset_cl_mem_;
      return false;
    }
    obj_min_dist_cl_mem_ = new bocl_mem(gpu_->context(), obj_min_dist_buff_, sizeof(unsigned char)*(*n_obj_), " obj_min_dist " );
    if (!obj_min_dist_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for OBJ_MIN_DIST\n";
      this->clean_weight_cl_mem();
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      if (is_grd_reg_) {
        delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;
        delete grd_land_cl_mem_; delete grd_land_wgt_cl_mem_;
      }
      if (is_obj_reg_) { delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_; }
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;
      delete obj_min_dist_cl_mem_;
      return false;
    }
    obj_order_cl_mem_ = new bocl_mem(gpu_->context(), obj_order_buff_, sizeof(unsigned char)*(*n_obj_), " obj_order " );
    if (!obj_order_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for OBJ_ORDER\n";
      this->clean_weight_cl_mem();
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      if (is_grd_reg_) {
        delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;
        delete grd_land_cl_mem_; delete grd_land_wgt_cl_mem_;
      }
      if (is_obj_reg_) { delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_; }
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;
      delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
      return false;
    }
    obj_land_cl_mem_ = new bocl_mem(gpu_->context(), obj_land_buff_, sizeof(unsigned char)*(*n_obj_)*fallback_size_, " obj_land " );
    if (!obj_land_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for OBJ_LAND\n";
      this->clean_weight_cl_mem();
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      if (is_grd_reg_) {
        delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;
        delete grd_land_cl_mem_; delete grd_land_wgt_cl_mem_;
      }
      if (is_obj_reg_) { delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_; }
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;
      delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
      delete obj_land_cl_mem_;
      return false;
    }
    obj_land_wgt_cl_mem_ = new bocl_mem(gpu_->context(), obj_land_wgt_buff_, sizeof(float)*(*n_obj_)*fallback_size_, " obj_land_wgt ");
    if (!obj_land_wgt_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for OBJ_LAND_WGT\n";
      this->clean_weight_cl_mem();
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      if (is_grd_reg_) {
        delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;
        delete grd_land_cl_mem_; delete grd_land_wgt_cl_mem_;
      }
      if (is_obj_reg_) { delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_; }
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;
      delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
      delete obj_land_cl_mem_;      delete obj_land_wgt_cl_mem_;
      return false;
    }
    query_global_mem_ += sizeof(unsigned)*obj_vox_size;                                      // object id
    query_global_mem_ += sizeof(unsigned)*query_->dist_offset().size();                      // object offset
    query_global_mem_ += sizeof(unsigned char)*(*n_obj_);                                    // object dist
    query_global_mem_ += (sizeof(unsigned char) + sizeof(float))*fallback_size_*(*n_obj_);   // object land fallback category and fallback weight
    query_local_mem_ += sizeof(unsigned char)*(*n_obj_);                                     // object dist on local memory
    query_local_mem_ += (sizeof(unsigned char) + sizeof(float))*fallback_size_*(*n_obj_);    // object land category on local memory
  }

  // implement orientation transfer function
  if (!this->transfer_orient()) {
    std::cerr << " ERROR: transferring query orientation failed\n";
    return false;
  }

  // count the memory for other stuff
  query_global_mem_ += sizeof(unsigned);                       // depth_interval length
  query_global_mem_ += sizeof(float)*depth_interval_.size();   // depth_interval buffer
  query_global_mem_ += sizeof(unsigned)*2;                     // layer_size and fallback size
  query_local_mem_  += sizeof(unsigned);                       // depth_interval length
  query_local_mem_ += sizeof(float)*depth_interval_.size();    // depth_interval buffer
  query_local_mem_ += sizeof(unsigned)*2;                      // layer_size and fallback size

  // check the device memory
  // check whether query size has exceeded the available global memory and local_memory
  device_global_mem_ = (gpu_->info()).total_global_memory_;
  device_local_mem_ = (gpu_->info()).total_local_memory_;

  if (query_global_mem_ > device_global_mem_) {
    std::cerr << " ERROR: the required global memory for query "  << query_global_mem_/1073741824.0
             << " GByte is larger than available global memory " << device_global_mem_/1073741824.0
             << " GB\n";
    this->clean_query_cl_mem();
    return false;
  }
  if (query_local_mem_ > device_local_mem_) {
    std::cerr << " ERROR: the required local memory for query " << query_local_mem_/1024.0
             << " KByte is larger than available local memory " << device_local_mem_/1024.0
             << " KB\n";
    this->clean_query_cl_mem();
    return false;
  }
  return true;
}

bool boxm2_volm_matcher_p1::transfer_orient()
{
  // temporary only consider horizontal and vertical
  // depth_map_region::HORIZONTAL --- horizontal --- ori = 1
  // depth_map_region::FRONT_PARALLEL, SLANTED_RIGHT, SLANTED_LEFT --- vertical --- ori = 2
  // all other defined orientation in depth_map_scene (query) --- invalid --- ori = 0

  std::vector<unsigned char>& obj_orient = query_->obj_orient();
  obj_orient_buff_ = new unsigned char[*n_obj_];
  for (unsigned i = 0; i < *n_obj_; i++) {
    if (obj_orient[i] == depth_map_region::HORIZONTAL)
      obj_orient_buff_[i] = (unsigned char)1;  // horizontal
    else if (obj_orient[i] == depth_map_region::FRONT_PARALLEL ||
             obj_orient[i] == depth_map_region::SLANTED_LEFT ||
             obj_orient[i] == depth_map_region::SLANTED_RIGHT)
      obj_orient_buff_[i] = (unsigned char)2; // vertical
    else
      obj_orient_buff_[i] = (unsigned char)0;  // invalid
  }
#if 0
  // check the transferred orientation
  for (unsigned i = 0; i < *n_obj_; i++) {
    std::cout << " i = " << i << ", query_orient = "  << (int)obj_orient[i]
             << ", orient_buff = " << (int)obj_orient_buff_[i] << std::endl;
  }
#endif
  // declare cl_mem for orient
  obj_orient_cl_mem_ = new bocl_mem(gpu_->context(), obj_orient_buff_, sizeof(unsigned char)*(*n_obj_), " obj_orient ");
  if (!obj_orient_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    std::cerr << "\n ERROR: creating bocl_mem failed for OBJ_ORIENT\n";
    this->clean_query_cl_mem();
    return false;
  }

  // count the memory usage
  query_global_mem_ += sizeof(unsigned char)*(*n_obj_);   // obj orientation on global memory
  query_local_mem_  += sizeof(unsigned char)*(*n_obj_);   // obj orientation on local memory

  return true;
}

bool boxm2_volm_matcher_p1::transfer_weight()
{
  // construct all the weight buffer and cl_mem
  // Note that the order of the weight parameters in the weight_param.txt has been ordered accordingly
  if (is_sky_reg_ && is_grd_reg_)
  {
    if (this->weights_.size() != (2+(*n_obj_)) ) {
      std::cerr << "\n ERROR: inconsistency between volm_query and volm_weight\n";
      return false;
    }
    // sky is line 1, grd is line 2
    sky_weight_buff_ = new float;  *sky_weight_buff_ = this->weights_[0].w_obj_;

    grd_weight_buff_ = new float;  *grd_weight_buff_ = this->weights_[1].w_obj_;
    grd_wgt_attri_buff_ = new float[3];
    grd_wgt_attri_buff_[0] = this->weights_[1].w_ori_;
    grd_wgt_attri_buff_[1] = this->weights_[1].w_lnd_;
    grd_wgt_attri_buff_[2] = this->weights_[1].w_dst_;

    obj_weight_buff_ = new float[*n_obj_];
    obj_wgt_attri_buff_ = new float[4*(*n_obj_)];
    unsigned cnt = 0;
    for (unsigned i = 2; i < this->weights_.size(); i++) {
      obj_wgt_attri_buff_[cnt*4]   = this->weights_[i].w_ori_;
      obj_wgt_attri_buff_[cnt*4+1] = this->weights_[i].w_lnd_;
      obj_wgt_attri_buff_[cnt*4+2] = this->weights_[i].w_dst_;
      obj_wgt_attri_buff_[cnt*4+3] = this->weights_[i].w_ord_;
      obj_weight_buff_[cnt++] = this->weights_[i].w_obj_;
    }
    // create cl_mem for sky and ground
    sky_weight_cl_mem_ = new bocl_mem(gpu_->context(), sky_weight_buff_, sizeof(float), " sky_weight " );
    if (!sky_weight_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for SKY_WEIGHT\n";
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      delete sky_weight_cl_mem_;
      return false;
    }
    grd_weight_cl_mem_ = new bocl_mem(gpu_->context(), grd_weight_buff_, sizeof(float), " grd_weight " );
    if (!grd_weight_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: create bocl_mem failed for GRD_WEIGHT\n";
      delete sky_weight_cl_mem_;
      delete grd_weight_cl_mem_;
      return false;
    }
    grd_wgt_attri_cl_mem_ = new bocl_mem(gpu_->context(), grd_wgt_attri_buff_, 3*sizeof(float), " grd_wgt_attri " );
    if (!grd_wgt_attri_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: create bocl_mem failed for GRD_WGT_ATTRI\n";
      delete sky_weight_cl_mem_;
      delete grd_weight_cl_mem_;  delete grd_wgt_attri_cl_mem_;
      return false;
    }

    // count the memory usage
    query_global_mem_ += sizeof(float);                     // sky weight
    query_global_mem_ += sizeof(float)*4;                   // grd weight and grd weight for attributes
    query_global_mem_ += sizeof(float)*5*(*n_obj_);         // obj weight and obj weight for attributes
    query_local_mem_  += sizeof(float);                     // sky weight
    query_local_mem_  += sizeof(float)*4;                   // grd weight and grd weight for attributes
    query_local_mem_  += sizeof(float)*4*(*n_obj_);         // obj weight and obj weight for attributes
  }
  else if (is_sky_reg_)
  {
    for (auto & weight : this->weights_)
      std::cerr << ' ' << weight.w_typ_
               << ' ' << weight.w_ori_
               << ' ' << weight.w_lnd_
               << ' ' << weight.w_dst_
               << ' ' << weight.w_ord_
               << ' ' << weight.w_obj_ << '\n';

    if (this->weights_.size() != (1+(*n_obj_)) ) {
      std::cerr << "\n ERROR: inconsistency between volm_query and volm_weight\n";
      return false;
    }
    // sky is line 1, all other objects starts from line 2
    sky_weight_buff_ = new float;  *sky_weight_buff_ = this->weights_[0].w_obj_;

    obj_weight_buff_ = new float[*n_obj_];
    obj_wgt_attri_buff_ = new float[4*(*n_obj_)];
    unsigned cnt = 0;
    for (unsigned i = 1; i < this->weights_.size(); i++) {
      obj_wgt_attri_buff_[cnt*4]   = this->weights_[i].w_ori_;
      obj_wgt_attri_buff_[cnt*4+1] = this->weights_[i].w_lnd_;
      obj_wgt_attri_buff_[cnt*4+2] = this->weights_[i].w_dst_;
      obj_wgt_attri_buff_[cnt*4+3] = this->weights_[i].w_ord_;
      obj_weight_buff_[cnt++] = this->weights_[i].w_obj_;
    }
    // create cl_mem for sky
    sky_weight_cl_mem_ = new bocl_mem(gpu_->context(), sky_weight_buff_, sizeof(float), " sky_weight " );
    if (!sky_weight_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: creating bocl_mem failed for SKY_WEIGHT\n";
      delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
      delete sky_weight_cl_mem_;
      return false;
    }

    // count the memory usage
    query_global_mem_ += sizeof(float);                     // sky weight
    query_global_mem_ += sizeof(float)*5*(*n_obj_);         // obj weight and obj weight for attributes
    query_local_mem_  += sizeof(float);                     // sky weight
    query_local_mem_  += sizeof(float)*4*(*n_obj_);         // obj weight and obj weight for attributes
  }
  else if (is_grd_reg_)
  {
    if (this->weights_.size() != (1+(*n_obj_)) ) {
      std::cerr << "\n ERROR: inconsistency between volm_query and volm_weight\n";
      return false;
    }
    // grd is line 1, objects starts from line 2
    grd_weight_buff_ = new float;  *grd_weight_buff_ = this->weights_[0].w_obj_;
    grd_wgt_attri_buff_ = new float[3];
    grd_wgt_attri_buff_[0] = this->weights_[0].w_ori_;
    grd_wgt_attri_buff_[1] = this->weights_[0].w_lnd_;
    grd_wgt_attri_buff_[2] = this->weights_[0].w_dst_;

    obj_weight_buff_ = new float[*n_obj_];
    obj_wgt_attri_buff_ = new float[4*(*n_obj_)];
    unsigned cnt = 0;
    for (unsigned i = 1; i < this->weights_.size(); i++) {
      obj_wgt_attri_buff_[cnt*4]   = this->weights_[i].w_ori_;
      obj_wgt_attri_buff_[cnt*4+1] = this->weights_[i].w_lnd_;
      obj_wgt_attri_buff_[cnt*4+2] = this->weights_[i].w_dst_;
      obj_wgt_attri_buff_[cnt*4+3] = this->weights_[i].w_ord_;
      obj_weight_buff_[cnt++] = this->weights_[i].w_obj_;
    }
    // create cl_mem for ground
    grd_weight_cl_mem_ = new bocl_mem(gpu_->context(), grd_weight_buff_, sizeof(float), " grd_weight " );
    if (!grd_weight_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: create bocl_mem failed for GRD_WEIGHT\n";
      delete grd_weight_cl_mem_;
      return false;
    }
    grd_wgt_attri_cl_mem_ = new bocl_mem(gpu_->context(), grd_wgt_attri_buff_, 3*sizeof(float), " grd_wgt_attri " );
    if (!grd_wgt_attri_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      std::cerr << "\n ERROR: create bocl_mem failed for GRD_WGT_ATTRI\n";
      delete grd_weight_cl_mem_;  delete grd_wgt_attri_cl_mem_;
      return false;
    }

    // count the memory usage
    query_global_mem_ += sizeof(float)*4;                   // grd weight and grd weight for attributes
    query_global_mem_ += sizeof(float)*5*(*n_obj_);         // obj weight and obj weight for attributes
    query_local_mem_  += sizeof(float)*4;                   // grd weight and grd weight for attributes
    query_local_mem_  += sizeof(float)*4*(*n_obj_);         // obj weight and obj weight for attributes
  }
  else
  {
    if (this->weights_.size() != (*n_obj_) ) {
      std::cerr << "\n ERROR: inconsistency between volm_query and volm_weight\n";
      return false;
    }
    // objects starts from line1
    obj_weight_buff_ = new float[*n_obj_];
    obj_wgt_attri_buff_ = new float[4*(*n_obj_)];
    for (unsigned i = 0; i < this->weights_.size(); i++) {
      obj_wgt_attri_buff_[i*4]   = this->weights_[i].w_ori_;
      obj_wgt_attri_buff_[i*4+1] = this->weights_[i].w_lnd_;
      obj_wgt_attri_buff_[i*4+2] = this->weights_[i].w_dst_;
      obj_wgt_attri_buff_[i*4+3] = this->weights_[i].w_ord_;
      obj_weight_buff_[i] = this->weights_[i].w_obj_;
    }
    // count the memory usage
    query_global_mem_ += sizeof(float)*5*(*n_obj_);         // obj weight and obj weight for attributes
    query_local_mem_  += sizeof(float)*4*(*n_obj_);         // obj weight and obj weight for attributes
  }

  // create cl_mem for non sky/grd objects
  obj_weight_cl_mem_ = new bocl_mem(gpu_->context(), obj_weight_buff_, (*n_obj_)*sizeof(float), " obj_weight ");
  if (!obj_weight_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    std::cerr << "\n ERROR: create bocl_mem failed for OBJ_WEIGHT\n";
    delete sky_weight_cl_mem_;
    delete grd_weight_cl_mem_;  delete grd_wgt_attri_cl_mem_;
    delete obj_weight_cl_mem_;
    return false;
  }

  obj_wgt_attri_cl_mem_ = new bocl_mem(gpu_->context(), obj_wgt_attri_buff_, 4*(*n_obj_)*sizeof(float), " obj_wgt_attri " );
  if (!obj_wgt_attri_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    std::cerr << "\n ERROR: create bocl_mem failed for OBJ_WGT_ATTRI\n";
    delete sky_weight_cl_mem_;
    delete grd_weight_cl_mem_;  delete grd_wgt_attri_cl_mem_;
    delete obj_weight_cl_mem_;  delete obj_wgt_attri_cl_mem_;
    return false;
  }

#if 0
  // screen output
  std::cout << " we have weight parameters in p1_matcher as follows" << std::endl;
  for (std::vector<volm_weight>::iterator vit = this->weights_.begin(); vit != this->weights_.end(); ++vit)
    std::cout << ' '
             << vit->w_typ_ << ' '
             << vit->w_ori_ << ' '
             << vit->w_lnd_ << ' '
             << vit->w_dst_ << ' '
             << vit->w_ord_ << ' '
             << vit->w_obj_ << std::endl;

  std::cout << " ---------------- WEIGHT BUFFER ------------------" << std::endl;
  if (is_sky_reg_ && is_grd_reg_) {
    std::cout << " sky 0.0 0.0 1.0 0.0 " << *sky_weight_buff_
             << "\n ground "
             << grd_wgt_attri_buff_[0] << ' '
             << grd_wgt_attri_buff_[1] << ' '
             << grd_wgt_attri_buff_[2]
             << " 0.0 " << *grd_weight_buff_ << std::endl;
  }
  else if (is_sky_reg_) {
    std::cout << " sky 0.0 0.0 1.0 0.0 " << *sky_weight_buff_ << std::endl;
  }
  else if (is_grd_reg_) {
    std::cout << " ground "
             << grd_wgt_attri_buff_[0] << ' '
             << grd_wgt_attri_buff_[1] << ' '
             << grd_wgt_attri_buff_[2]
             << " 0.0 " << *grd_weight_buff_ << std::endl;
  }
  for (unsigned i = 0; i < (*n_obj_); i++) {
    std::cout << " object "
             << obj_wgt_attri_buff_[i*4] << ' '
             << obj_wgt_attri_buff_[i*4+1] << ' '
             << obj_wgt_attri_buff_[i*4+2] << ' '
             << obj_wgt_attri_buff_[i*4+3] << ' '
             << obj_weight_buff_[i] << std::endl;
  }
#endif

  return true;
}

bool boxm2_volm_matcher_p1::volm_matcher_p1_test_ori(unsigned n_ind,
                                                     unsigned char* index,
                                                     const unsigned char* index_ori,
                                                     const unsigned char* index_lnd,
                                                     float* score_buff,
                                                     float* mu_buff)
{
  std::vector<float> mu;
  unsigned nc = *n_cam_;
  unsigned no = *n_obj_;

  if (is_obj_reg_) {
    // calculate mean depth value
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
            if ( (unsigned)index[id] < 253 && (unsigned)index[id] < depth_interval_.size() ) {
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
  std::vector<float> score_sky_all;
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
  // define the altitude ratio, suppose the altitude in index could be up to 3 meter
  // assuming the read-in alt values in query is normally ~1m, the altitude ratio would be (2-1)/1 ~2
  // the altitude ratio defined the tolerance for ground distance d as delta_d = alt_ratio * d
  // this delte_d could be in depth_interval unit ?
  unsigned char alt_ratio = 2;
  std::vector<float> score_grd_all;
  std::vector<float> score_grd_dist_all;
  std::vector<float> score_grd_ori_all;
  std::vector<float> score_grd_lnd_all;
  if (is_grd_reg_)
  {
    for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
      unsigned start_ind = ind_id * layer_size_;
      for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
        unsigned start = grd_id_offset_buff_[cam_id];
        unsigned end = grd_id_offset_buff_[cam_id+1];
        unsigned cnt = 0;
        unsigned ori_cnt = 0;
        float    score_lnd = 0.0f;
        for (unsigned k = start; k < end; k++) {
          unsigned id = start_ind + grd_id_buff_[k];
          unsigned char ind_d = index[id];
          unsigned char ind_lnd, ind_ori;
          ind_lnd = index_lnd[id];  ind_ori = index_ori[id];
          //volm_io_extract_values(index_combine[id], ind_ori, ind_lnd);
          //unsigned char ind_lnd = index_land[id];
          unsigned char grd_d = grd_dist_buff_[k];
          // use physical distance to compute the grd_dst score
          if (ind_id < depth_interval_.size() && grd_d < depth_interval_.size()) {
            float ind_dst = depth_interval_[ind_d];
            float grd_dst = depth_interval_[grd_d];
            float delta_d = alt_ratio * grd_dst;
            if ( ind_dst >= (grd_dst-delta_d) && ind_dst <= (grd_dst+delta_d) ) cnt++;
          }
          // calculate ground orientation score, ground plane has to be horizontal
          if (ind_ori == 1) ori_cnt++;
          // calculate the ground land type score
          if ( ind_lnd != 0 && ind_lnd != 254) {
            for (unsigned ii = 0; ii < *fallback_size_buff_; ii++) {
              unsigned lnd_id = k*(*fallback_size_buff_)+ii;
              if (ind_lnd == grd_land_buff_[lnd_id]) {
                score_lnd += grd_land_wgt_buff_[lnd_id];
                break;
              }
            }
          }
        }
        float score_grd_dst = (end!=start) ? (float)cnt/(end-start) : 0.0f;
        float score_grd_ori = (end!=start) ? (float)ori_cnt/(end-start) : 0.0f;
        float score_grd_lnd = (end!=start) ? score_lnd/(end-start) : 0.0f;
        score_grd_dst *= (*grd_weight_buff_);
        score_grd_ori *= (*grd_weight_buff_);
        score_grd_lnd *= (*grd_weight_buff_);
        score_grd_lnd_all.push_back(score_grd_lnd);
        score_grd_all.push_back(this->grd_wgt_attri_buff_[0] * score_grd_ori +
                                this->grd_wgt_attri_buff_[1] * score_grd_lnd +
                                this->grd_wgt_attri_buff_[2] * score_grd_dst);
      }
    }
  }

  // calculate the object score
  std::vector<float> score_ord_all;
  std::vector<float> score_min_all;
  std::vector<float> score_ori_all;
  std::vector<float> score_lnd_all;
  if ( is_obj_reg_)
  {
    for (unsigned ind_id = 0; ind_id < n_ind; ind_id++)
    {
      unsigned start_ind = ind_id * layer_size_;
      for (unsigned cam_id = 0; cam_id < nc; cam_id++)
      {
        float score_order = 0.0f;
        float score_min = 0.0f;
        float score_ori = 0.0f;
        float score_lnd = 0.0f;
        unsigned mu_start_id = cam_id*no + ind_id*no*nc;
        for (unsigned k = 0; k < no; k++)
        {
          unsigned offset_id = k + no*cam_id;
          unsigned start_obj = obj_id_offset_buff_[offset_id];
          unsigned end_obj = obj_id_offset_buff_[offset_id+1];
          unsigned lnd_start = k*(*fallback_size_buff_);
          unsigned lnd_end = (k+1)*(*fallback_size_buff_);
          float score_ord_k = 0.0f;
          float score_min_k = 0.0f;
          float score_ori_k = 0.0f;
          float score_lnd_k = 0.0f;
          for (unsigned i = start_obj; i < end_obj; i++) {
            unsigned id = start_ind + obj_id_buff_[i];
            unsigned d = index[id];
            unsigned s_ord = 1;
            unsigned s_min = 0;
            unsigned s_ori = 0;

            if ( d < 253 && d < depth_interval_.size()) {  // valid object voxel
              // do the order checking
              for (unsigned mu_id = 0; (mu_id < k && s_ord); mu_id++) {
                float depth_d = depth_interval_[d];
                float depth_m = mu[mu_id+mu_start_id];
                if (depth_m*depth_m > 1E-7)
                  s_ord = s_ord * ( depth_d - depth_m > -1E-5 );
              }
              for (unsigned mu_id = k+1; (mu_id < no && s_ord); mu_id++) {
                float depth_d = depth_interval_[d];
                float depth_m = mu[mu_id+mu_start_id];
                if (depth_m*depth_m > 1E-7)
                s_ord = s_ord * ( depth_d - depth_m < 1E-5 );
              }
              if ( d > obj_min_dist_buff_[k] )
                s_min = 1;
            }
            else {
              s_ord = 0;
            }
            // check the orientation score
            unsigned char ind_ori = index_ori[id];
            unsigned char ind_lnd = index_lnd[id];
            //volm_io_extract_values(index_combine[id], ind_ori, ind_lnd);
            if ( ind_ori == obj_orient_buff_[k] && obj_orient_buff_[k] == 1)
              s_ori = 1;
            if ( ind_ori > 2 && ind_ori < 10 && obj_orient_buff_[k] != 0)
              s_ori = 1;
            // check the land_type score
#ifdef DEBUG
            std::cout << (int)ind_lnd << std::endl;
#endif
            if (ind_lnd != 0 && ind_lnd != 254) {
              for (unsigned ii = lnd_start; ii < lnd_end; ii++) {
                if (ind_lnd == obj_land_buff_[ii]) {
                  score_lnd_k += obj_land_wgt_buff_[ii];
                  break;
                }
              }
            }

            score_ord_k += (float)s_ord;
            score_min_k += (float)s_min;
            score_ori_k += (float)s_ori;
          } // end for loop over voxel in object k

          // normalized the order score for object k
          score_ord_k = (end_obj != start_obj) ? score_ord_k/(end_obj-start_obj) : 0;
          score_ord_k = score_ord_k * obj_weight_buff_[k] * this->obj_wgt_attri_buff_[4*k+3];
#if 1
          if (ind_id == 2 && cam_id == 90) {
            std::cout << " ind_id = " << ind_id
                     << " cam_id = " << cam_id
                     << " k = " << k
                     << " num_rays = " << end_obj - start_obj
                     << " score_ord = " << score_ord_k
                     << std::endl;
          }
#endif
          // normalized the min_dist score for object k
          score_min_k = (end_obj != start_obj) ? score_min_k/(end_obj-start_obj) : 0;
          score_min_k = score_min_k * obj_weight_buff_[k] * this->obj_wgt_attri_buff_[4*k+2];

          // normalized the orient score for object k
          score_ori_k = (end_obj != start_obj) ? score_ori_k/(end_obj-start_obj) : 0;
          score_ori_k = score_ori_k * obj_weight_buff_[k] * this->obj_wgt_attri_buff_[4*k+0];

          // normalized the land type score for object k
          score_lnd_k = (end_obj != start_obj) ? score_lnd_k/(end_obj-start_obj) : 0;
          score_lnd_k = score_lnd_k * obj_weight_buff_[k] * this->obj_wgt_attri_buff_[4*k+1];

          // summarize order score for index ind_id and camera cam_id
          score_order += score_ord_k;
          score_min += score_min_k;
          score_ori += score_ori_k;
          score_lnd += score_lnd_k;
        }  // end for loop over objects

        score_ord_all.push_back(score_order);
        score_min_all.push_back(score_min);
        score_ori_all.push_back(score_ori);
        score_lnd_all.push_back(score_lnd);
      } // end of loop over cameras
    } // end of loop over indices
  }

  // get the overall object score
  std::vector<float> score_obj_all;
  // get the overall score
  std::vector<float> score_all;
  for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
    for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
      unsigned id = cam_id + ind_id * nc;
      if (is_grd_reg_ && is_sky_reg_)
        score_all.push_back(score_sky_all[id] + score_grd_all[id] + score_ord_all[id] + score_min_all[id] + score_ori_all[id] + score_lnd_all[id]);
      else if ( !is_grd_reg_ && is_sky_reg_)
        score_all.push_back(score_sky_all[id] + score_ord_all[id] + score_min_all[id] + score_ori_all[id] + score_lnd_all[id]);
      else if ( !is_sky_reg_ && is_grd_reg_)
        score_all.push_back(score_grd_all[id] + score_ord_all[id] + score_min_all[id] + score_ori_all[id] + score_lnd_all[id]);
      else
        score_all.push_back(score_ord_all[id] + score_min_all[id] + score_ori_all[id] + score_lnd_all[id]);
      score_obj_all.push_back(score_ord_all[id] + score_min_all[id] + score_ori_all[id] + score_lnd_all[id]);
    }
  }

  // output all sky and ground score for checking
#if 1
  for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
    for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
      unsigned id = cam_id + ind_id * nc;
      if (cam_id == 0 && ind_id == 2)
        std::cout << " ind_id = " << ind_id << " cam_id = " << cam_id
                 //<< " score_sky[" << id << "] = " << score_sky_all[id]
                 << "\t score_grd[" << id << "] = " << score_grd_all[id]
                 << "\t score_grd_lnd[" << id << "] = " << score_grd_lnd_all[id]
                 << "\t score_ord[" << id << "] = " << score_ord_all[id]
                 << "\t score_min[" << id << "] = " << score_min_all[id]
                 << "\t score_ori[" << id << "] = " << score_ori_all[id]
                 << "\t score_lnd[" << id << "] = " << score_lnd_all[id]
                 << "\t score_obj[" << id << "] = " << score_ord_all[id] + score_min_all[id] + score_ori_all[id] + score_lnd_all[id]
                 << std::endl;
    }
  }
#endif
  // output for objects
#if 1
  // mean values
  for (unsigned ind_id = 0; ind_id < n_ind; ind_id++)
    for (unsigned cam_id = 0; cam_id < nc; cam_id++)
      for (unsigned obj_id = 0; obj_id < no; obj_id++) {
        unsigned start_ind = ind_id * layer_size_;
        unsigned id = obj_id + cam_id * no + ind_id * nc * no;
      //if ( mu[id] - mu_buff[id] != 0)
        if (ind_id == 2 && cam_id == 90) {
          std::cout << " ind_id = " << ind_id
                   << " cam_id = " << cam_id
                   << " obj_id = " << obj_id
                   << " start_ind = " << start_ind
                   << " id = " << id
                   << " mu_cpu = " << mu[id]
                   << " mu_gpu = " << mu_buff[id]
                   << " mu_diff = " << mu[id] - mu_buff[id]
                   << std::endl;
        }
      }
#endif

#if 1
    // score for object
    for (unsigned ind_id = 0; ind_id < n_ind; ind_id++)
    for (unsigned cam_id = 0; cam_id < nc; cam_id++)
      for (unsigned obj_id = 0; obj_id < no; obj_id++) {
        unsigned id = cam_id + ind_id * nc;
      //if ( mu[id] - mu_buff[id] != 0)
        std::cout << " ind_id = " << ind_id
                 << " cam_id = " << cam_id
                 << " obj_id = " << obj_id
                 << " id = " << id
                 << "\t score_order = " << score_ord_all[id]
                 << "\t score_min = " << score_min_all[id]
                 << "\t score_ori = " << score_ori_all[id]
                 << std::endl;
      }
#endif

  // check the score output
  for (unsigned ind_id = 0; ind_id < n_ind; ind_id++) {
    for (unsigned cam_id = 0; cam_id < nc; cam_id++) {
      unsigned id = cam_id + ind_id * nc;
      if (ind_id == 2 && cam_id == 90)
        std::cout << " ind = " << ind_id << " cam = " << cam_id << " id = " << id
                 << "\t score_cpu = " << score_all[id]
                 << "\t score_gpu = " << score_buff[id]
                 << "\t diff = " << score_all[id] - score_buff[id]
                 << std::endl;
    }
  }
  return true;
}

bool boxm2_volm_matcher_p1::inside_candidate(vgl_polygon<double> const& cand_poly, double const& ptx, double const& pty)
{
  // when sheets overlap, the poly.contain method may return false for pts located inside the overlapped region
  // this function will returns true if the given point is inside any single sheet of the polygon
  unsigned num_sheet = cand_poly.num_sheets();
  for (unsigned i = 0; i < num_sheet; i++) {
    vgl_polygon<double> single_sheet(cand_poly[i]);
    if (single_sheet.contains(ptx, pty))
      return true;
  }
  return false;
}
