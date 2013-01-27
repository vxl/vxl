#include "boxm2_volm_obj_based_matcher.h"
#include "boxm2_volm_wr3db_index.h"
#include <vcl_where_root_dir.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_random.h>
#include <vul/vul_timer.h>

bool boxm2_volm_obj_based_matcher::obj_based_matcher()
{
  n_cam_ = new unsigned;
  *n_cam_ = (unsigned)query_->get_cam_num();
  n_cam_cl_mem_ = new bocl_mem(gpu_->context(), n_cam_, sizeof(unsigned), " n_cam " );
  if (!n_cam_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for N_CAM\n";
    delete n_cam_cl_mem_;
    return false;
  }

#if 0
  // check cam string
  for (unsigned cam_id = 0; cam_id < *n_cam_; ++cam_id) {
    vcl_cout << " cam_id = " << cam_id
             << " cam_string = " << query_->get_cam_string(cam_id)
             << vcl_endl;
  }
#endif

  n_obj_ = new unsigned;
  *n_obj_ = (unsigned)(query_->depth_regions()).size();
  n_obj_cl_mem_ = new bocl_mem(gpu_->context(), n_obj_, sizeof(unsigned), " n_obj " );
  if (!n_obj_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for N_OBJ\n";
    delete n_cam_cl_mem_;  delete n_obj_cl_mem_;
    return false;
  }

  // transfer all queries information to 1D array structure
  query_global_mem_ = 0;
  query_local_mem_ = 0;
  vul_timer transfer_query;
  if (!this->transfer_query()) {
    vcl_cerr << "\n ERROR: transfering query to 1D structure failed.\n";
    return false;
  }

  vcl_cout << "\t 4.1 Setting up query for GPU matcher ------> \t" << transfer_query.all()/1000.0 << " seconds." << vcl_endl;

  // create queue
  if (!this->create_queue()) {
    vcl_cerr << "\n ERROR: creating matcher queue failed.\n";
    return false;
  }

  // compile the kernel
  vcl_string identifier = gpu_->device_identifier();
  if (kernels_.find(identifier) == kernels_.end()) {
    vcl_cout << "\t 4.2 Comipling kernels for device " << identifier << vcl_endl;
#ifdef DEBUG
    vcl_cout << "Device Info:\n" << gpu_->info() << vcl_endl;
#endif
    vcl_vector<bocl_kernel*> ks;
    if (!this->compile_kernel(ks)) {
      vcl_cerr << "\n ERROR: compiling matcher kernel failed.\n";
      return false;
    }
    kernels_[identifier] = ks;
  }

  // calculate available memory space for indices
  if (query_->get_query_size() != ind_->layer_size()) {
    vcl_cerr << "\n ERROR: generate query and index have different voxel size\n";
    return false;
  }
  cl_ulong index_unit_size = ind_->layer_size();
  cl_ulong tol_global_mem = (cl_ulong)(1073741824 * 1.0);  // leave 1G empty space in global memory
  cl_ulong avail_index_mem, avail_index_num;
  // available index memory = availiable global memory - query memory - output score memory - meaning value memory
  if ( tol_global_mem+query_global_mem_ > avail_global_mem_) {
    vcl_cerr << "\n ERROR: no enough global memory for index, reduce the size of extra empty space\n";
    delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;        delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;        delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
    delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
    delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
    return false;
  }
  else {
    avail_index_mem = avail_global_mem_ - query_global_mem_ - tol_global_mem ;
    avail_index_num = avail_index_mem / (index_unit_size + 4 * (*n_cam_) + (*n_cam_)*(*n_obj_));
  }

  unsigned loop_num = ei_ / avail_index_num;

#if 1
  vcl_cout << "\t 4.3 Measuring available device memory for indices\n"
           << "\t\t available GLOBAL memory = " << avail_global_mem_ << " byte (" << avail_global_mem_/1073741824.0 << " GB)\n"
           << "\t\t available LOCAL memory = " << avail_local_mem_ << " Byte (" << avail_local_mem_/1024.0 << " KB)\n"
           << "\t\t required GLOBAL memory for query = " << query_global_mem_ << " Byte (" << query_global_mem_/1048576.0 << " MB)\n"
           << "\t\t required LOCAL memory for query = " << query_local_mem_ << " Byte\n"
           << "\t\t index has unit size = " << index_unit_size << " Byte\n"
           << "\t\t available GLOBAL memory for indices (leave " << (float)tol_global_mem/1073741824.0 << "GB empty space) = " << avail_index_mem << " Byte (" << avail_index_mem/1073741824.0
           << " GB) for index ----> " << avail_index_num << " indices" << vcl_endl;


#endif

  // Start the matcher
  vcl_cout << "\t 4.4 Start the object based matcher" << vcl_endl;
  cl_uint status;
  vcl_size_t max_work_group_size;
  vcl_size_t max_work_item_sizes[3];
  //Get device max work gropu size
  status = clGetDeviceInfo(*gpu_->device_id(),CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(vcl_size_t),(void*)&max_work_group_size, NULL);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_MAX_WORK_GROUP_SIZE failed."))
    return false;
  status = clGetDeviceInfo(*gpu_->device_id(), CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(vcl_size_t) * 3, (void*)max_work_item_sizes, NULL);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_MAX_WORK_ITEM_SIZES failed."))
    return false;

#if 0
  vcl_cout << " Maximum work group size = " << (cl_uint) max_work_group_size << vcl_endl;
  vcl_cout << " Maximum work item sizes: (" << (cl_uint) max_work_item_sizes[0] << ','
                                            << (cl_uint) max_work_item_sizes[1] << ','
                                            << (cl_uint) max_work_item_sizes[2] << ")\n";
#endif

  vul_timer total_exe_time;
  float    gpu_matcher_time = 0.0f;
  for (unsigned i = 0; i <= loop_num; ++i) {
    // construct indices index_offset buffer and cl_mem
    cl_ulong start, end, ind_size;
    if (i < loop_num) {
        start = i * avail_index_num;  end = (i+1) * avail_index_num;
    }
    else {
        start = i * avail_index_num;  end = ei_;
    }
    cl_ulong* n_ind_ = new cl_ulong;
    *n_ind_ = end - start;
    bocl_mem* n_ind_cl_mem_ = new bocl_mem(gpu_->context(), n_ind_, sizeof(unsigned), " n_ind " );
    if (!n_ind_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed fro N_INDEX\n";
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;        delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
      delete sky_id_cl_mem_;        delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
      delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
      delete n_ind_cl_mem_;         delete n_ind_;
      return false;
    }

    ind_size = (end - start) * index_unit_size;
    unsigned char*         ind_buff;
    bocl_mem*            ind_cl_mem;
    unsigned*       ind_offset_buff;
    bocl_mem*     ind_offset_cl_mem;
    ind_buff = new unsigned char[ind_size];
    ind_offset_buff = new unsigned[(*n_ind_)+1];
    if (!this->fill_index(start, end, ind_buff, ind_offset_buff)) {
      vcl_cerr << "\n ERROR: constructing index_buff for index from " << start << " to " << end << " failed\n";
      delete [] ind_buff;
      return false;
    }
    ind_cl_mem = new bocl_mem(gpu_->context(), ind_buff, sizeof(unsigned char)*ind_size, " index " );
    if (!ind_cl_mem->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for INDEX\n";
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;        delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
      delete sky_id_cl_mem_;        delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
      delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
      delete ind_cl_mem;            delete [] ind_buff;
      delete n_ind_cl_mem_;         delete n_ind_;
      return false;
    }
    ind_offset_cl_mem = new bocl_mem(gpu_->context(), ind_offset_buff, sizeof(unsigned)*((*n_ind_)+1), " index_offset " );
    if (!ind_offset_cl_mem->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for INDEX_OFFSET\n";
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;        delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
      delete sky_id_cl_mem_;        delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
      delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
      delete ind_cl_mem;            delete [] ind_buff;            delete ind_offset_cl_mem;     delete [] ind_offset_buff;
      delete n_ind_cl_mem_;         delete n_ind_;
      return false;
    }
    // define NDRange based on n_ind and n_cam, meaning work item (i,j) refers to camera j on index i
    matcher_work_dim_ = 2;
    matcher_local_threads_[0] = 32;
    matcher_local_threads_[1] = 32;
    cl_ulong cl_ni=(unsigned)RoundUp(*n_ind_,(int)matcher_local_threads_[0]);  // row is index
    cl_ulong cl_nj=(unsigned)RoundUp(*n_cam_,(int)matcher_local_threads_[1]);  // col is camera
    matcher_global_threads_[0]=cl_ni;
    matcher_global_threads_[1]=cl_nj;
    if ( matcher_local_threads_[0] > (cl_uint)max_work_item_sizes[0] || matcher_local_threads_[1] > (cl_uint)max_work_item_sizes[1] ) {
      vcl_cerr << "\n ERROR: work_item_size [" << matcher_local_threads_[0] << ", " << matcher_local_threads_[1] << ']'
               << " is larger than device maximum work item size [" << (cl_uint)max_work_item_sizes[0] << ", " << (cl_uint)max_work_item_sizes[1] << "]\n";
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;        delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
      delete sky_id_cl_mem_;        delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
      delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
      delete ind_cl_mem;            delete [] ind_buff;            delete ind_offset_cl_mem;     delete [] ind_offset_buff;
      delete n_ind_cl_mem_;         delete n_ind_;
      return false;
    }
    if ( matcher_local_threads_[0] * matcher_local_threads_[1] > max_work_group_size ) {
      vcl_cerr << "\n ERROR: work_item_size [" << matcher_local_threads_[0] << 'x' << matcher_local_threads_[1] << ']'
               << " is larger than device maximum work group size " << max_work_group_size << '\n';
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;        delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
      delete sky_id_cl_mem_;        delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
      delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
      delete ind_cl_mem;            delete [] ind_buff;            delete ind_offset_cl_mem;     delete [] ind_offset_buff;
      delete n_ind_cl_mem_;         delete n_ind_;
      return false;
    }

#if 1
    vcl_cout << "\t\t i = " << i << ", start = " << start << ", end = " << end
             << " number of indices = " << end - start
             << "--> index_buff_size = " << ind_size << '\n'
             << "\t\t NDRange is work_group_size = [" << matcher_local_threads_[0] << 'x' << matcher_local_threads_[1] << ']'
             << ", work_item_size = [" << matcher_global_threads_[0] << 'x' << matcher_global_threads_[1] << ']'
             << ", overall work groups = " << (matcher_global_threads_[0])/(matcher_local_threads_[0]) * (matcher_global_threads_[1])/(matcher_local_threads_[1])
             << vcl_endl;
#endif
#if 0
    for (unsigned i = 0 ; i < (*n_ind_); ++i) {
      unsigned index_buff_id = i + start;
      unsigned start_id = ind_offset_buff[i];
      unsigned end_id = ind_offset_buff[i+1];
      for (unsigned k = start_id; k < end_id; ++k) {
        vcl_cout << " INDEX_BUFF_ID_OFF " << index_buff_id << ", index_buff_f[" << k
                 << "] = " << (int)ind_buff[k] << vcl_endl;
      }
    }
#endif

    // construct the average depth value buffer for each object on each work-item
    //  CAN NOT do it inside the kernel since we don't know the number of object before hand
      // create buffer for average depth array for each work-item
    unsigned mu_size = *(n_obj_) * (*n_ind_) * (*n_cam_);
    unsigned char* mu_buff = new unsigned char[mu_size];
    bocl_mem* mu_cl_mem = new bocl_mem(gpu_->context(), mu_buff, sizeof(unsigned char)*(mu_size), " mu " );
    if (!mu_cl_mem->create_buffer( CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for MU array\n";
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;        delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
      delete sky_id_cl_mem_;        delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
      delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
      delete ind_cl_mem;            delete [] ind_buff;            delete ind_offset_cl_mem;     delete [] ind_offset_buff;
      delete n_ind_cl_mem_;         delete n_ind_;
      delete mu_cl_mem;             delete [] mu_buff;
      return false;
    }

    // construct the score buffer, the buffer should be 1D array with score(id) = ind_id * n_cam + vox_id
    // and the outcome will be stored in vector index_max_score and index_cam_score; or something else ?
    float*      score_buff;
    bocl_mem* score_cl_mem;
    score_buff = new float[(*n_cam_)*(*n_ind_)];
    score_cl_mem = new bocl_mem(gpu_->context(), score_buff, sizeof(float)*((*n_cam_)*(*n_ind_)), " score " );
    if (!score_cl_mem->create_buffer( CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR )) {
      vcl_cerr << "\n ERROR: creating bocl_mem failed for SCORE\n";
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;        delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
      delete sky_id_cl_mem_;        delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
      delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
      delete ind_cl_mem;            delete [] ind_buff;            delete ind_offset_cl_mem;     delete [] ind_offset_buff;
      delete n_ind_cl_mem_;         delete n_ind_;
      delete mu_cl_mem;             delete [] mu_buff;
      delete score_cl_mem;          delete [] score_buff;
      return false;
    }

    // start the obj_based kernel matcher
    vcl_string identifier = gpu_->device_identifier();

    if (!this->execute_matcher_kernel(gpu_, queue_, kernels_[identifier][0],
                                      n_ind_cl_mem_,
                                      ind_cl_mem,
                                      ind_offset_cl_mem,
                                      mu_cl_mem,
                                      score_cl_mem) ) {
      vcl_cerr << "\n ERROR: matcher kernel execution failed\n";
      delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
      delete grd_id_cl_mem_;        delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
      delete sky_id_cl_mem_;        delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
      delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
      delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
      delete ind_cl_mem;            delete [] ind_buff;            delete ind_offset_cl_mem;     delete [] ind_offset_buff;
      delete n_ind_cl_mem_;         delete n_ind_;
      delete mu_cl_mem;             delete [] mu_buff;
      delete score_cl_mem;          delete [] score_buff;
      return false;
    }
    // block again before read score from device
    status = clFinish(queue_);
    // read score
    score_cl_mem->read_to_buffer(queue_);
    status = clFinish(queue_);
    // count time
    gpu_matcher_time += kernels_[identifier][0]->exec_time();

#if 0
    if (!mu_calculation_test(*n_ind_, ind_buff, ind_offset_buff, score_buff))
      vcl_cerr << "\n ERROR: mu error ...\n";
#endif

    // find the maximum
    for (unsigned ind_id = 0; ind_id < (*n_ind_); ++ind_id) {
      // find maximum for index ind_id
      float max_score = 0.0f;
      unsigned max_cam_id = 0;
      for (unsigned cam_id = 0; cam_id < (*n_cam_); ++cam_id) {
        unsigned id = cam_id + ind_id * (*n_cam_);
        unsigned index_id = start + ind_id;
        float current_score = score_buff[id];
        index_score_all.push_back(boxm2_volm_score(index_id, cam_id, current_score));
        if (current_score > max_score) {
          max_score = current_score;  max_cam_id = cam_id;
        }
      }
      index_max_score.push_back(max_score);
      index_max_camera.push_back(max_cam_id);
    }

    // clean cl_mem before next round matcher
    delete n_ind_cl_mem_;
    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, " release N_INDEX failed on device " + gpu_->device_identifier() + error_to_string(status));
    delete ind_cl_mem;
    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, " release INDEX failed on device " + gpu_->device_identifier() + error_to_string(status));
    delete ind_offset_cl_mem;
    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, " release INDEX_OFFSET failed on device " + gpu_->device_identifier() + error_to_string(status));
    delete mu_cl_mem;
    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, " release MU(average depth value) failed on device " + gpu_->device_identifier() + error_to_string(status));
    delete score_cl_mem;
    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, " release SCORE failed on device " + gpu_->device_identifier() + error_to_string(status));

    delete [] score_buff;
    delete [] mu_buff;
    delete [] ind_offset_buff;
    delete [] ind_buff;
    delete n_ind_;
  } // end of the loop over all indices

  // time
  float total_time = total_exe_time.all();
  vcl_cout << "\t\t total time for " << ei_ << " indices and " << *n_cam_ << " cameras -------> " << total_time/1000.0 << " seconds (" << total_time << " ms)\n"
           << "\t\t GPU kernel execution ------------------> " << gpu_matcher_time/1000.0 << " seconds (" << gpu_matcher_time << " ms)\n"
           << "\t\t CPU host execution --------------------> " << (total_time - gpu_matcher_time)/1000.0 << " seconds (" << total_time - gpu_matcher_time << " ms)" << vcl_endl;


#if 0
  // check out the maximum score
  for (unsigned ind_id = 0; ind_id < ei_; ++ind_id) {
    vcl_cout << " index id = " << ind_id
             << " max_score = " << index_max_score[ind_id]
             << " max_camera = " << index_max_camera[ind_id]
             << vcl_endl;
  }
  for (unsigned i = 0 ; i < index_score_all.size(); ++i) {
    vcl_cout << " i = " << i
             << " ind_id = " << index_score_all[i].ind_id
             << " cam_id = " << index_score_all[i].cam_id
             << " score = " << index_score_all[i].score
             << vcl_endl;
  }
#endif

  // finish all indices, clear all query bocl_mem
  delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
  delete grd_id_cl_mem_;        delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
  delete sky_id_cl_mem_;        delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
  delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
  delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
  return true;
}

boxm2_volm_obj_based_matcher::~boxm2_volm_obj_based_matcher()
{
  delete                 n_cam_;
  delete                 n_obj_;
  delete []        grd_id_buff_;
  delete []      grd_dist_buff_;
  delete [] grd_id_offset_buff_;
  delete       grd_weight_buff_;
  delete []        sky_id_buff_;
  delete [] sky_id_offset_buff_;
  delete       sky_weight_buff_;
  delete []        obj_id_buff_;
  delete [] obj_id_offset_buff_;
  delete []  obj_min_dist_buff_;
//delete []  obj_max_dist_buff;
  delete []     obj_order_buff_;
  delete []    obj_weight_buff_;
}

bool boxm2_volm_obj_based_matcher::transfer_query()
{
  // construct the ground_id, ground_dist, ground_offset 1D array
  unsigned grd_vox_size = query_->get_ground_id_size();
  grd_id_buff_ = new unsigned[grd_vox_size];
  grd_dist_buff_ = new unsigned char[grd_vox_size];
  grd_id_offset_buff_ = new unsigned[*n_cam_+1];
  unsigned grd_count = 0;
  vcl_vector<vcl_vector<unsigned> >& grd_id = query_->ground_id();
  vcl_vector<vcl_vector<unsigned char> >& grd_dist = query_->ground_dist();
  for (unsigned cam_id = 0; cam_id < *n_cam_; ++cam_id) {
    grd_id_offset_buff_[cam_id] = grd_count;
    for (unsigned vox_id = 0; vox_id < grd_id[cam_id].size(); ++vox_id) {
      unsigned i = grd_count + vox_id;
      grd_id_buff_[i] = grd_id[cam_id][vox_id];
      grd_dist_buff_[i] = grd_dist[cam_id][vox_id];
    }
    grd_count += (unsigned)grd_id[cam_id].size();
  }
  grd_id_offset_buff_[*n_cam_] = grd_count;

#if 0
  // check ground info
  for (unsigned cam_id = 0; cam_id < *n_cam_; ++cam_id) {
    for (unsigned vox_id = 0; vox_id < grd_id[cam_id].size(); ++vox_id) {
      vcl_cout << " cam = " << cam_id << ", vox_id = " << vox_id
               << ", grd_id = " << grd_id[cam_id][vox_id]
               << ", grd_dist = " << (int)grd_dist[cam_id][vox_id] << vcl_endl;
    }
  }
  vcl_cout << " ------ 1D array ground info -------" << vcl_endl;
  for (unsigned cam_id = 0; cam_id < *n_cam_; ++cam_id) {
    unsigned start = grd_id_offset_buff_[cam_id];
    unsigned end = grd_id_offset_buff_[cam_id+1];
    vcl_cout << " cam = " << cam_id << ", start = " << start << ", end = " << end << vcl_endl;
    for (unsigned i = start; i < end; ++i) {
      vcl_cout << " cam = " << cam_id << ", vox_id = " << i
               << ", grd_id_buff = " << grd_id_buff_[i]
               << ", grd_dist_buff = " << (int)grd_dist_buff_[i] << vcl_endl;
    }
  }
#endif

  // construct sky_id buff
  unsigned sky_vox_size = query_->get_sky_id_size();
  sky_id_buff_ = new unsigned[sky_vox_size];
  sky_id_offset_buff_ = new unsigned[*n_cam_+1];
  unsigned sky_count = 0;
  vcl_vector<vcl_vector<unsigned> >& sky_id = query_->sky_id();
  for (unsigned cam_id = 0; cam_id < *n_cam_; ++cam_id) {
    sky_id_offset_buff_[cam_id] = sky_count;
    for (unsigned vox_id = 0; vox_id < sky_id[cam_id].size(); ++vox_id) {
      unsigned i = sky_count + vox_id;
      sky_id_buff_[i] = sky_id[cam_id][vox_id];
    }
    sky_count += (unsigned)sky_id[cam_id].size();
  }
  sky_id_offset_buff_[*n_cam_] = sky_count;

#if 0
  // check 1d sky_buff
  for (unsigned cam_id = 0; cam_id < *n_cam_; ++cam_id) {
    for (unsigned vox_id = 0; vox_id < (query_->sky_id())[cam_id].size(); ++vox_id) {
      vcl_cout << " cam = " << cam_id << ", vox_id = " << vox_id
               << ", sky_id = " << (query_->sky_id())[cam_id][vox_id] << vcl_endl;
    }
  }
  vcl_cout << " ------------ 1D sky array ---------------" << vcl_endl;
  for (unsigned cam_id = 0; cam_id < *n_cam_; ++cam_id) {
    unsigned start = sky_id_offset_buff_[cam_id];
    unsigned end = sky_id_offset_buff_[cam_id+1];
    for (unsigned i = start; i < end; ++i) {
      vcl_cout << " cam = " << cam_id << ", vox_id = " << i
               << ", sky_id_buff = " << sky_id_buff_[i] << vcl_endl;
    }
  }
#endif

  // construct obj_id, obj_offset 1D array
  unsigned obj_vox_size = query_->get_dist_id_size();
  unsigned obj_offset_size = (*n_cam_) * (*n_obj_);
  obj_id_buff_ = new unsigned[obj_vox_size];
  obj_id_offset_buff_ = new unsigned[obj_offset_size+1];
  vcl_vector<vcl_vector<vcl_vector<unsigned> > >& dist_id = query_->dist_id();
  unsigned obj_count = 0;
  for (unsigned cam_id = 0; cam_id < *n_cam_; ++cam_id) {
    for (unsigned obj_id = 0; obj_id < *n_obj_; ++obj_id) {
      unsigned offset_id = obj_id + cam_id * (*n_obj_);
      obj_id_offset_buff_[offset_id] = obj_count;
      for (unsigned vox_id = 0; vox_id < dist_id[cam_id][obj_id].size(); ++vox_id) {
        unsigned i = obj_count + vox_id;
        obj_id_buff_[i] = dist_id[cam_id][obj_id][vox_id];
      }
      obj_count += (unsigned)dist_id[cam_id][obj_id].size();
    }
  }
  obj_id_offset_buff_[obj_offset_size] = obj_count;

#if 0
  // check 1D obj_id_buff
  for (unsigned cam_id = 0; cam_id < *n_cam_; ++cam_id)
    for (unsigned obj_id = 0; obj_id < *n_obj_; ++obj_id)
      for (unsigned vox_id = 0; vox_id < dist_id[cam_id][obj_id].size(); ++vox_id)
        vcl_cout << " cam = " << cam_id << ", obj_id = " << obj_id << ", vox_id = " << vox_id
                 << ", obj_index_id = " << dist_id[cam_id][obj_id][vox_id] << vcl_endl;

  vcl_cout << " ------------ 1D array -------------" << vcl_endl;
  for (unsigned cam_id = 0; cam_id < *n_cam_; ++cam_id)
    for (unsigned obj_id = 0; obj_id < *n_obj_; ++obj_id) {
      unsigned offset_id = obj_id + cam_id * (*n_obj_);
      unsigned start = obj_id_offset_buff_[offset_id];
      unsigned end = obj_id_offset_buff_[offset_id+1];
      vcl_cout << " cam = " << cam_id << ", obj = " << obj_id << ", start = " << start << ", end = " << end << vcl_endl;
      for (unsigned i = start; i < end; ++i) {
        vcl_cout << " cam = " << cam_id << ", obj = " << obj_id << ", vox_id = " << i
                 << ", obj_index_id_buff = " << obj_id_buff_[i] << vcl_endl;
      }
    }
#endif

  // construct min_dist, max_dist and order_obj for non-ground, non-sky object
  obj_min_dist_buff_ = new unsigned char[*n_obj_];
  //max_obj_dist_buff = new unsigned char[n_obj_];
  obj_order_buff_ = new unsigned char[*n_obj_];
  for (unsigned obj_id = 0; obj_id < *n_obj_; ++obj_id) {
    obj_min_dist_buff_[obj_id] = query_->min_obj_dist()[obj_id];
    obj_order_buff_[obj_id] = query_->order_obj()[obj_id];
  }
  // construct weight parameters
  grd_weight_buff_ = new float;
  sky_weight_buff_ = new float;
  obj_weight_buff_ = new float[*n_obj_];

  *grd_weight_buff_ = query_->grd_weight();
  *sky_weight_buff_ = query_->sky_weight();
  for (unsigned obj_id = 0; obj_id < *n_obj_; ++obj_id)
    obj_weight_buff_[obj_id] = query_->obj_weight()[obj_id];

#if 0
  // check min_dist and order array
  vcl_cout << " grd_weight = " << *grd_weight_buff_ << '\n'
           << " sky_weight = " << *sky_weight_buff_ << '\n';
  for (unsigned i = 0; i < *n_obj_; ++i) {
  vcl_cout << " object " << i << " , min_dist = " << (int)obj_min_dist_buff_[i]
           << ", order = " << obj_order_buff_[i]
           << ", weight = " << obj_weight_buff_[i]
           << vcl_endl;
  }
#endif

  // check whether query size has exceeded the available global memory and local_memory
  avail_global_mem_ = (gpu_->info()).total_global_memory_;
  avail_local_mem_ = (gpu_->info()).total_local_memory_;

  // the obj_min_dist, obj_order and all weight parameters are put in local memory
  query_local_mem_ = 2 * (*n_obj_) * sizeof(unsigned char);                           // local memoery size == min_dist + order
  query_global_mem_ = sizeof(unsigned) * 2;                                           // n_cam + n_obj;
  query_global_mem_ += (sizeof(unsigned char) + sizeof(unsigned))*grd_vox_size +
                       sizeof(unsigned char)*(*n_cam_+1) +
                       sizeof(float);                                                 // ground size == id + dist + offset + weight
  query_global_mem_ += sizeof(unsigned) * sky_vox_size +
                       sizeof(unsigned) * (*n_cam_+1) +
                       sizeof(float);                                                 // sky size == id + offset + weight
  query_global_mem_ += sizeof(unsigned) * obj_vox_size +
                       sizeof(unsigned) * (obj_offset_size+1) +
                       sizeof(float) * (*n_obj_);                                     // obj size == id + offset + weight
  query_global_mem_ += query_local_mem_;                                              // things passed to local_mem still need to passing to global_mem

  if (query_global_mem_ > avail_global_mem_) {
    vcl_cerr << " ERROR: the required global memory for query "  << query_global_mem_/1073741824.0
             << " GByte is larger than available global memory " << avail_global_mem_/1073741824.0
             << " GB\n";
    return false;
  }
  if (query_local_mem_ > avail_local_mem_) {
    vcl_cerr << " ERROR: the required local memoery for query " << query_local_mem_/1024.0
             << " KByte is larger than available local memory " << avail_local_mem_/1024.0
             << " KB\n";
    return false;
  }

  // construct global cl_mem for query
  grd_id_cl_mem_ = new bocl_mem(gpu_->context(), grd_id_buff_, sizeof(unsigned)*grd_vox_size, " grd_id " );
  if (!grd_id_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for GROUND_ID\n";
    delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;
    return false;
  }
  grd_dist_cl_mem_ = new bocl_mem(gpu_->context(), grd_dist_buff_, sizeof(unsigned char)*grd_vox_size, " grd_dist " );
  if (!grd_dist_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for GROUND_DIST\n";
    delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;
    return false;
  }
  grd_id_offset_cl_mem_ = new bocl_mem(gpu_->context(), grd_id_offset_buff_, sizeof(unsigned)*(*n_cam_+1), " grd_offset " );
  if (!grd_id_offset_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for GROUND_OFFSET\n";
    delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;
    return false;
  }
  grd_weight_cl_mem_ = new bocl_mem(gpu_->context(), grd_weight_buff_, sizeof(float), " grd_weight " );
  if (!grd_weight_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for GROUND_WEIGHT\n";
    delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
    return false;
  }
  sky_id_cl_mem_ = new bocl_mem(gpu_->context(), sky_id_buff_, sizeof(unsigned)*sky_vox_size, " sky_id " );
  if (!sky_id_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for SKY_ID\n";
    delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;
    return false;
  }
  sky_id_offset_cl_mem_ = new bocl_mem(gpu_->context(), sky_id_offset_buff_, sizeof(unsigned)*(*n_cam_+1), " sky_offset " );
  if (!sky_id_offset_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for SKY_OFFSET\n";
    delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;  delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;
    return false;
  }
  sky_weight_cl_mem_ = new bocl_mem(gpu_->context(), sky_weight_buff_, sizeof(float), " sky_weight " );
  if (!sky_weight_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for SKY_WEIGHT\n";
    delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
    return false;
  }
  obj_id_cl_mem_ = new bocl_mem(gpu_->context(), obj_id_buff_, sizeof(unsigned)*obj_vox_size, " obj_id " );
  if (!obj_id_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for OBJ_ID\n";
    delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
    delete obj_id_cl_mem_;
    return false;
  }
  obj_id_offset_cl_mem_ = new bocl_mem(gpu_->context(), obj_id_offset_buff_, sizeof(unsigned)*(obj_offset_size+1), " obj_offset " );
  if (!obj_id_offset_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for OBJ_OFFSET\n";
    delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
    delete obj_id_cl_mem_;  delete obj_id_offset_cl_mem_;
    return false;
  }
  obj_weight_cl_mem_ = new bocl_mem(gpu_->context(), obj_weight_buff_, sizeof(float)*(*n_obj_), " obj_weight " );
  if (!obj_weight_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for OBJ_WEIGHT\n";
    delete n_cam_cl_mem_;   delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;  delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;  delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
    delete obj_id_cl_mem_;  delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
    return false;
  }
  obj_min_dist_cl_mem_ = new bocl_mem(gpu_->context(), obj_min_dist_buff_, sizeof(unsigned char)*(*n_obj_), " obj_min_dist " );
  if (!obj_min_dist_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for OBJ_MIN_DIST\n";
    delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;        delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;        delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
    delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
    delete obj_min_dist_cl_mem_;
    return false;
  }
  obj_order_cl_mem_ = new bocl_mem(gpu_->context(), obj_order_buff_, sizeof(unsigned char)*(*n_obj_), " obj_order " );
  if (!obj_order_cl_mem_->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for OBJ_ORDER\n";
    delete n_cam_cl_mem_;         delete n_obj_cl_mem_;
    delete grd_id_cl_mem_;        delete grd_dist_cl_mem_;       delete grd_id_offset_cl_mem_;  delete grd_weight_cl_mem_;
    delete sky_id_cl_mem_;        delete sky_id_offset_cl_mem_;  delete sky_weight_cl_mem_;
    delete obj_id_cl_mem_;        delete obj_id_offset_cl_mem_;  delete obj_weight_cl_mem_;
    delete obj_min_dist_cl_mem_;  delete obj_order_cl_mem_;
    return false;
  }
  return true;
}

bool boxm2_volm_obj_based_matcher::create_queue()
{
  bocl_device_sptr device = gpu_;
  cl_int status = SDK_FAILURE;
  queue_ = clCreateCommandQueue(gpu_->context(),
                                *(gpu_->device_id()),
                                CL_QUEUE_PROFILING_ENABLE,
                                &status);
  if ( !check_val(status, CL_SUCCESS, error_to_string(status)) )
    return false;
  return true;
}

bool boxm2_volm_obj_based_matcher::compile_kernel(vcl_vector<bocl_kernel*>& vec_kernels)
{
  // declare the kernel source
  vcl_vector<vcl_string> src_paths;
  vcl_string volm_cl_source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/volm/cl/";
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_obj_based_matching.cl");
  // create the obj_based matching kernel
  bocl_kernel* kern_matcher = new bocl_kernel();
  if (!kern_matcher->create_kernel(&gpu_->context(), gpu_->device_id(), src_paths,
                                   "generalized_volm_obj_based_matching",
                                   "",
                                   "generalized_volm_obj_based_matching") )
    return false;
  vec_kernels.push_back(kern_matcher);
  return true;
}

bool boxm2_volm_obj_based_matcher::fill_index(cl_ulong start, cl_ulong end, unsigned char* index_buff, unsigned* index_offset_buff)
{
  unsigned layer_size = ind_->layer_size(); // byte
  cl_ulong read_in_num = end - start;
  unsigned char* values;
  // read in indices from start to end
  for (unsigned i = 0; i < read_in_num; ++i) {
    values = index_buff + i * layer_size;
    ind_->get_next(values, layer_size);
  }
  // create the index_offset_buff
  unsigned count = 0;
  for (unsigned i = 0; i <= read_in_num; ++i) {
    index_offset_buff[i] = count;
    count += layer_size;
  }
  return true;
}

bool boxm2_volm_obj_based_matcher::execute_matcher_kernel(bocl_device_sptr             device,
                                                          cl_command_queue&             queue,
                                                          bocl_kernel*                   kern,
                                                          bocl_mem*             n_ind_cl_mem_,
                                                          bocl_mem*              index_cl_mem,
                                                          bocl_mem*       index_offset_cl_mem,
                                                          bocl_mem*                 mu_cl_mem,
                                                          bocl_mem*              score_cl_mem)
{
  // create buffer for debug
  cl_int status;
  float* debug_buff = new float[20];
  vcl_fill(debug_buff, debug_buff+20, (float)12.31);
  bocl_mem* debug_cl_mem = new bocl_mem(device->context(), debug_buff, sizeof(float)*1000, " debug " );
  if (!debug_cl_mem->create_buffer( CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR )) {
    vcl_cerr << "\n ERROR: creating bocl_mem failed for DEBUG array\n";
    delete debug_cl_mem;  delete [] debug_buff;
    return false;
  }
  // set up argument list
  kern->set_arg(n_cam_cl_mem_);
  kern->set_arg(n_obj_cl_mem_);
  kern->set_arg(grd_id_cl_mem_);
  kern->set_arg(grd_id_offset_cl_mem_);
  kern->set_arg(grd_dist_cl_mem_);
  kern->set_arg(grd_weight_cl_mem_);
  kern->set_arg(sky_id_cl_mem_);
  kern->set_arg(sky_id_offset_cl_mem_);
  kern->set_arg(sky_weight_cl_mem_);
  kern->set_arg(obj_id_cl_mem_);
  kern->set_arg(obj_id_offset_cl_mem_);
  kern->set_arg(obj_min_dist_cl_mem_);
  kern->set_arg(obj_weight_cl_mem_);
  kern->set_arg(n_ind_cl_mem_);
  kern->set_arg(index_cl_mem);
  kern->set_arg(index_offset_cl_mem);
  kern->set_arg(mu_cl_mem);
  kern->set_arg(score_cl_mem);
  // set up local memory arguments
  kern->set_local_arg((*n_obj_)*sizeof(unsigned char));  // local memory for obj_min_dist
  kern->set_local_arg((*n_obj_)*sizeof(float));          // local memory for obj_weight
  // set up debug
  kern->set_arg(debug_cl_mem);

  // execute the kernel

  if (!kern->execute(queue, matcher_work_dim_, matcher_local_threads_, matcher_global_threads_)) {
    vcl_cerr << "\n ERROR: kernel execuation failed\n";
    delete debug_cl_mem;  delete [] debug_buff;
    return false;
  }

  // block until matcher kernel finishes
  status = clFinish(queue);
  // clear bocl_kernel argument list
  kern->clear_args();
#if 1
  // read debug data from device
  debug_cl_mem->read_to_buffer(queue);
  for (int i=0; (debug_buff[i]-12.31)*(debug_buff[i]-12.31)>0.0001 && i<1000; ++i)
    vcl_cout << " debug[" << i << "] = " << debug_buff[i] << vcl_endl;
#endif
  // clear debug buffer
  delete debug_cl_mem;
  status = clFinish(queue);
  check_val(status, MEM_FAILURE, " release DEBUG failed on device " + device->device_identifier() + error_to_string(status));

  delete [] debug_buff;
  return true;
}

#if 0
bool boxm2_volm_obj_based_matcher::mu_calculation_test(unsigned n_ind, unsigned char* index, unsigned* index_offset, float* score_buff)
{
  vcl_vector<unsigned char> mu;
  unsigned n_cam = *n_cam_;
  unsigned n_obj = *n_obj_;
  for (unsigned ind_id = 0; ind_id < n_ind; ++ind_id) {  // loop over the index
    // locate index
    unsigned start_ind = index_offset[ind_id];
    for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id)  // loop over cameras for index ind_id
      for (unsigned k = 0; k < n_obj; ++k) {             // loop over objects in camera cam_id
        unsigned offset_id = k + n_obj * cam_id;
        unsigned start_obj = obj_id_offset_buff_[offset_id];
        unsigned end_obj = obj_id_offset_buff_[offset_id+1];
        unsigned mu_obj = 0;
        unsigned count = 0;
        for (unsigned i = start_obj; i < end_obj; ++i) { // loop over voxels in object k
          unsigned id = start_ind + obj_id_buff_[i];
          if ( (unsigned)index[id] != 254) {
            mu_obj += index[id]; ++count;
          }
        }
        mu_obj = (count > 0) ? mu_obj/count : 0;
        mu.push_back((unsigned char)mu_obj);
      }
  }

  // calculate the sky score
  vcl_vector<float> score_sky_all;
  for (unsigned ind_id = 0; ind_id < n_ind; ++ind_id) {
    unsigned start_ind = index_offset[ind_id];
    for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id) {
      unsigned start = sky_id_offset_buff_[cam_id];
      unsigned end = sky_id_offset_buff_[cam_id+1];
      unsigned count = 0;
      for (unsigned k = start; k < end; ++k) {
        unsigned id = start_ind + sky_id_buff_[k];
        if ( index[id] == 254 ) ++count;
      }
      float score_sky;
      score_sky = (end != start) ? (float)count/(end-start) : 0.0f;
      score_sky *= (*sky_weight_buff_);
      score_sky_all.push_back(score_sky);
    }
  }

  // calculate the ground score
  vcl_vector<float> score_grd_all;
  for (unsigned ind_id = 0; ind_id < n_ind; ++ind_id) {
    unsigned start_ind = index_offset[ind_id];
    for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id) {
      unsigned start = grd_id_offset_buff_[cam_id];
      unsigned end = grd_id_offset_buff_[cam_id+1];
      unsigned count = 0;
      for (unsigned k = start; k < end; ++k) {
        unsigned id = start_ind + grd_id_buff_[k];
        unsigned char ind_d = index[id];
        unsigned char grd_d = grd_dist_buff_[k];
        if ( ind_d >= (grd_d-1) && ind_d <= (grd_d+1) ) ++count;
      }
      float score_grd;
      score_grd = (end != start) ? (float)count/(end-start) : 0.0f;
      score_grd *= (*grd_weight_buff_);
      score_grd_all.push_back(score_grd);
    }
  }

  // output all sky and ground score for checking
#if 0
  for (unsigned ind_id = 0; ind_id < n_ind; ++ind_id) {
    for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id) {
      unsigned id = cam_id + ind_id * n_cam;
      vcl_cout << " ind_id = " << ind_id << " cam_id = " << cam_id
               << " score_sky[" << id << "] = " << score_sky_all[id]
               << "\t\t score_grd[" << id << "] = " << score_grd_all[id]
               << vcl_endl;
    }
  }
#endif

  // caluclate the object score
  vcl_vector<float> score_order_all;
  vcl_vector<float> score_min_all;
  for (unsigned ind_id = 0; ind_id < n_ind; ++ind_id) {
    unsigned start_ind = index_offset[ind_id];
    for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id) {
      float score_order = 0.0f;
      float score_min = 0.0f;
      unsigned mu_start_id = cam_id*n_obj + ind_id * n_cam * n_obj;
      for (unsigned k = 0; k < n_obj; ++k) {
        unsigned offset_id = k + n_obj*cam_id;
        unsigned start_obj = obj_id_offset_buff_[offset_id];
        unsigned end_obj = obj_id_offset_buff_[offset_id+1];
        float score_k = 0.0f;
        float score_min_k = 0.0f;
        for (unsigned i = start_obj; i < end_obj; ++i) {
          unsigned id = start_ind + obj_id_buff_[i];
          unsigned d = index[id];
          unsigned s_vox = 1;
          unsigned s_min = 0;
          if ( d != 254 ) {
            for (unsigned mu_id = 0; (mu_id < k && s_vox); ++mu_id)
              s_vox = s_vox * ( d >= mu[mu_id + mu_start_id] );
            for (unsigned mu_id = k+1; (mu_id < n_obj && s_vox); ++mu_id)
              s_vox = s_vox * ( d <= mu[mu_id + mu_start_id] );
            if ( d > obj_min_dist_buff_[k] )
              s_min = 1;
          }
          else {
            s_vox = 0;
          }
          score_k += (float)s_vox;
          score_min_k += (float)s_min;
        }
        // normalized the order score for object k
        score_k = (end_obj != start_obj) ? score_k/(end_obj-start_obj) : 0;
        score_k = score_k * obj_weight_buff_[k];
        // normalized the min_dist score for object k
        score_min_k = (end_obj != start_obj) ? score_min_k/(end_obj-start_obj) : 0;
        score_min_k = score_min_k * obj_weight_buff_[k];
        // summerize order score for index ind_id and camera cam_id
        score_order += score_k;
        score_min += score_min_k;
      }
      score_order = score_order / n_obj;
      score_order_all.push_back(score_order);
      score_min = score_min / n_obj;
      score_min_all.push_back(score_min);
    }
  }

  // output
#if 0
  // mean values
  for (unsigned ind_id = 0; ind_id < n_ind; ++ind_id)
    for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id)
      for (unsigned obj_id = 0; obj_id < n_obj; ++obj_id) {
        unsigned start_ind = index_offset[ind_id];
        unsigned id = obj_id + cam_id * n_obj + ind_id * n_cam * n_obj;
        vcl_cout << " ind_id = " << ind_id
                 << " cam_id = " << cam_id
                 << " obj_id = " << obj_id
                 << " start_ind = " << start_ind
                 << " id = " << id
                 << " mu = " << (int)mu[id]
                 << vcl_endl;
      }

  // score_order
  for (unsigned ind_id = 0; ind_id < n_ind; ++ind_id) {
    for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id) {
      unsigned id = cam_id + ind_id * n_cam;
      vcl_cout << " ind_id = " << ind_id << " cam_id = " << cam_id << " score_order[" << id << "] = " << score_order_all[id] << vcl_endl;
    }
  }

  // score_min
  for (unsigned ind_id = 0; ind_id < n_ind; ++ind_id) {
    for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id) {
      unsigned id = cam_id + ind_id * n_cam;
      vcl_cout << " ind = " << ind_id << " cam = " << cam_id << " score_min[" << id << "] = " << score_min_all[id] << vcl_endl;
    }
  }
#endif

  // get the overall score
  vcl_vector<float> score_all;
  for (unsigned ind_id = 0; ind_id < n_ind; ++ind_id) {
    for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id) {
      unsigned id = cam_id + ind_id * n_cam;
      score_all.push_back(score_sky_all[id] + score_grd_all[id] + score_order_all[id] + score_min_all[id]);
    }
  }

  // check the score output
  for (unsigned ind_id = 0; ind_id < n_ind; ++ind_id) {
    for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id) {
      unsigned id = cam_id + ind_id * n_cam;
      vcl_cout << " ind = " << ind_id << " cam = " << cam_id << " id = " << id
               << "\t score_cpu = " << score_all[id]
               << "\t score_gpu = " << score_buff[id]
               << "\t diff = " << score_all[id] - score_buff[id]
               << vcl_endl;
    }
  }

#if 0
  // output score
  for (unsigned ind_id = 0; ind_id < n_ind; ++ind_id) {
    for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id) {
      unsigned id = cam_id + ind_id * n_cam;
      vcl_cout << " ind = " << ind_id << " cam = " << cam_id
               << " score_sky[" << id << "] = " << score_sky_all[id]
               << "\t score_grd[" << id << "] = " << score_grd_all[id]
               << "\t score_obj[" << id << "] = " << score_order_all[id] + score_min_all[id]
               << "\t score_all[" << id << "] = " << score_all[id]
               << vcl_endl;
    }
  }
#endif
  return true;
}
#endif
