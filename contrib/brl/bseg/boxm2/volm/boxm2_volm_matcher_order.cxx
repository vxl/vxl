#include "boxm2_volm_matcher_order.h"
#include "boxm2_volm_wr3db_index.h"
#include <vcl_where_root_dir.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_random.h>
#include <vul/vul_timer.h>

boxm2_volm_matcher_order::boxm2_volm_matcher_order(volm_query_sptr query, boxm2_volm_wr3db_index_sptr ind, unsigned long ei, bocl_device_sptr gpu) :
    query_(query), ind_(ind), ei_(ei), gpu_(gpu)
{
  vul_timer transfer;
  // define the work item and work group based on number of camera and the number of voxels inside the query
  work_dim = 2;
  n_cam = query_->get_cam_num();     // number of camera;
  n_vox = query_->get_query_size();  // number of voxels;
  local_threads[0] = 8;
  local_threads[1] = 8;
  cl_ni=(unsigned)RoundUp(n_cam,(int)local_threads[0]);  // row is camera
  cl_nj=(unsigned)RoundUp(n_vox,(int)local_threads[1]);  // col is voxel
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  // create the 1D query_array buffer for all devices, each 1D array has length cl_ni x cl_nj
  queries_buff = new unsigned char[cl_ni*cl_nj];
  vcl_fill(queries_buff, queries_buff + cl_ni*cl_nj, (unsigned char)255);
  // fill the query array from query
  for (unsigned i = 0; i < n_cam; i++) {
    for (unsigned k = 0; k < n_vox; k++) {
      unsigned idx = i * cl_nj + k;
      queries_buff[idx] = (query_->min_dist())[i][k];
#if 0
      vcl_cout << " insider matcher, cam " << i 
               << ", --> query[" << k << "] = " << (int)(query_->min_dist())[i][k] << vcl_endl;
#endif
    }
  }
  query_cl = new bocl_mem(gpu_->context(), queries_buff, sizeof(unsigned char)*cl_ni*cl_nj, " query " );
  if (!query_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    delete query_cl;
    vcl_cout << " ERROR: create bocl_mem failed for QUERY" << vcl_endl;
  }

  // create bocl_mem for weight parameters, which equals the total number of rays
  weight_buff = new float[cl_ni];
  for (unsigned wIdx = 0; wIdx < n_cam; wIdx++) {
    weight_buff[wIdx] = (float)query_->get_valid_ray_num(wIdx);
  }
  vcl_fill(weight_buff+n_cam, weight_buff+cl_ni, 1.0f);
  weight = new bocl_mem(gpu_->context(), weight_buff, sizeof(float)*cl_ni, " weight " );
  if (!weight->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cout << "\n ERROR: creating bocl_mem failed for WEIGHT" << vcl_endl;
    delete weight;      delete [] weight_buff;
    delete query_cl;
  }

  // create voxel_size for weight summation kernel
  v_size_buff = new unsigned[2];
  v_size_buff[0] = cl_nj;
  v_size_buff[1] = n_vox;
  voxel_size = new bocl_mem(gpu_->context(), v_size_buff, sizeof(unsigned)*2, " voxel_size " );
  if (!voxel_size->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR) ) {
    vcl_cout << "\n ERROR: creating bocl_mem failed for VOXEL_SIZE" << vcl_endl;
    delete voxel_size;
    delete weight;
    delete query_cl;
  }

  // create object order, order offset buffer/bocl_mem for order kernel
  if (!this->create_order_array()){
    delete query_cl;
    delete weight;
    delete query_cl;
    vcl_cout << " ERROR: create bocl_mem/buffer failed for ORDER" << vcl_endl;
  }

    // define the weight parameter for score from order kernel and min_dist kernel
  weight_all_buff = new float[2];
  weight_all_buff[0] = 0.3;      // weight for min_dist score
  weight_all_buff[1] = 0.7;      // weight for order score
  weight_all_cl = new bocl_mem(gpu_->context(), weight_all_buff, sizeof(float)*2, " weight_all ");
  if (!weight_all_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR)) {
    vcl_cout << "\n ERROR: create bocl_mem failed for WEIGHT_ALL" << vcl_endl;
    delete voxel_size;
    delete weight;
    delete query_cl;
    delete order_cl;
    delete order_offset_cl;
    delete order_size_cl;
  }

  // create command queue lists for all devices
  if (!this->setup_queue_order()) {
    vcl_cerr << "ERROR happens when create queue and setup queries for each device\n";
    delete voxel_size;
    delete weight;
    delete query_cl;
    delete order_cl;
    delete order_offset_cl;
    delete order_size_cl;
    delete weight_all_cl;
  }

  vcl_cout << "  1. Transfering queries/order to 1D array for all devices --- \t" << transfer.all()/1000.0 << " seconds.\n";
  vcl_cout << " \t query size = " << n_cam * n_vox / (1024*1024) << " MB\n";
  vcl_cout << " \t order size = " << sizeof(unsigned) * n_cam * n_obj / (1024*1024) << " MB\n";
  vcl_cout << " \t weight size = " << sizeof(float)*cl_ni / (1024*1024) << " MB" << vcl_endl;

  // compile kernel
  vcl_string identifier = gpu_->device_identifier();
  if (kernels.find(identifier) == kernels.end()) {
    vcl_cout << "===========Compiling kernels for device " << identifier << "==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    if (!compile_kernel_order(gpu_,ks)) {
      vcl_cerr << " ERROR happens when compiling kernels on device " << identifier << '\n';
      delete voxel_size;
      delete weight;
      delete query_cl;
      delete order_cl;
      delete order_offset_cl;
      delete order_size_cl;
      delete weight_all_cl;
    }
    kernels[identifier] = ks;
  }
  vcl_cout << "  2. Create queues and compile kernels for all device --- \t" << transfer.all()/1000.0 << " seconds." << vcl_endl;
}

boxm2_volm_matcher_order::~boxm2_volm_matcher_order()
{
  delete [] queries_buff;
  delete [] order_buff;
  delete [] order_offset_buff;
  delete [] order_size_buff;
  delete [] v_size_buff;
  delete [] weight_buff;
  delete [] weight_all_buff;
}

bool boxm2_volm_matcher_order::create_order_array()
{
  // get number of object
  unsigned n_obj = (unsigned)((query_->order_set()).size());
  // create buffer for order and order_offset
  unsigned offset_size = n_cam * n_obj + 1;
  order_offset_buff = new unsigned[offset_size];
  vcl_vector<unsigned> order_all;
  unsigned count = 0;
  order_offset_buff[count++] = 0;
  unsigned offset_last = order_offset_buff[0];
  for (unsigned cam_id = 0; cam_id < n_cam; cam_id++) {
    for (unsigned obj_id = 0; obj_id < n_obj; obj_id++) {
      unsigned offset_curr = (unsigned)((query_->order_index())[cam_id][obj_id].size());
      order_offset_buff[count++] = offset_last + offset_curr;
      offset_last = order_offset_buff[count-1];
      order_all.insert(order_all.end(), (query_->order_index())[cam_id][obj_id].begin(), (query_->order_index())[cam_id][obj_id].end());
    }
  }
  unsigned order_size = (unsigned)order_all.size();
  order_buff = new unsigned[order_size];
  for (unsigned i = 0; i < order_size; i++)
    order_buff[i] = order_all[i];
  
  // create bocl_mem for order and order_offset
  order_cl = new bocl_mem(gpu_->context(), order_buff, sizeof(unsigned)*order_size, " order " );
  if (!order_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
    vcl_cout << "\n ERROR: creating bocl_mem failed for ORDER" << vcl_endl;
    delete order_cl;
    return false;
  }
  order_offset_cl = new bocl_mem(gpu_->context(), order_offset_buff, sizeof(unsigned)*offset_size, " order_offset " );
  if (!order_offset_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR)) {
    vcl_cout << "\n ERROR: creating bocl_mem failed for ORDER_OFFSET" << vcl_endl;
    delete order_cl;
    delete order_offset_cl;
    return false;
  }
  // create order size buffer
  order_size_buff = new unsigned[2];
  order_size_buff[0] = n_cam;
  order_size_buff[1] = n_obj;
  order_size_cl = new bocl_mem(gpu_->context(), order_size_buff, sizeof(unsigned)*2, " order_size " );
  if (!order_size_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR)) {
    vcl_cout << "\n ERROR: create bocl_mem failed for ORDER_SIZE" << vcl_endl;
    delete order_cl;
    delete order_offset_cl;
    delete order_size_cl;
    return false;
  }
  return true;
}

bool boxm2_volm_matcher_order::setup_queue_order()
{
  // create queue
  bocl_device_sptr device = gpu_;
  cl_int status = SDK_FAILURE;
  queue = clCreateCommandQueue(device->context(), *(device->device_id()), CL_QUEUE_PROFILING_ENABLE, &status);
  if ( !check_val(status, CL_SUCCESS, error_to_string(status)) ) {
    vcl_cerr << "ERROR when creating queue on device " << device->device_identifier() << '\n';
    return false;
  }
  return true;
}

bool boxm2_volm_matcher_order::compile_kernel_order(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels)
{
  // declare the kernel source
  vcl_vector<vcl_string> src_paths;
  vcl_string volm_cl_source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/volm/cl/";
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_matching_layer.cl");
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_matching_order.cl");
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_matching_score_order_weight_sum_order.cl");
  // create the kernel for min_dist matcher
  bocl_kernel* volm_match = new bocl_kernel();
  if (!volm_match->create_kernel(&device->context(), device->device_id(), src_paths,
                                 "generalized_volm_matching_layer",
                                 "",
                                 "generalized_volm_matching_layer") ) {
    return false;
  }
  vec_kernels.push_back(volm_match);

  // create the kernel for order matcher
  bocl_kernel* volm_match_order = new bocl_kernel();
  if (!volm_match_order->create_kernel(&device->context(), device->device_id(), src_paths,
                                       "generalized_volm_matching_order",
                                       "",
                                       "generalized_volm_matching_order") ) {
    return false;
  }
  vec_kernels.push_back(volm_match_order);

  // create the kernel for summarization
  bocl_kernel* volm_sum = new bocl_kernel();
  if (!volm_sum->create_kernel(&device->context(), device->device_id(), src_paths,
                               "generalized_volm_matching_score_order_weight_sum_order",
                               "",
                               "generalized_volm_matching_score_order_weight_sum_order") ) {
    return false;
  }
  vec_kernels.push_back(volm_sum);
  return true;
}

bool boxm2_volm_matcher_order::matching_cost_layer_order()
{
  cl_int status;
  unsigned layer_size = (unsigned)query_->get_query_size();
  vul_timer transfer;
  float gpu_time_min_dist = 0.0f;
  float gpu_time_order = 0.0f;
  float gpu_time_sum = 0.0f;
  unsigned count = (unsigned)(0.1*ei_);
  vcl_cerr << "  3. Start to match all " << ei_ << '(' << ei_ << ") indices with " << n_cam << " cameras per index " ;
  for (unsigned ind_idx = 0; ind_idx < ei_; ind_idx++) {
    if (!(ind_idx % count) ) vcl_cerr << '.';
    bocl_device_sptr device = gpu_;
    vcl_string identifier = device->device_identifier();

    // create index buffer and index bocl_mem
    unsigned char* index_buff = new unsigned char[cl_nj];
    ind_->get_next(index_buff, cl_nj);
    bocl_mem* index = new bocl_mem(device->context(), index_buff, sizeof(unsigned char)*cl_nj, " index " );
    if (!index->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )) {
      delete index;       delete [] index_buff;
      delete voxel_size;  delete weight;  delete query_cl;
      delete order_cl;    delete order_offset_cl;  delete order_size_cl;
      delete weight_all_cl;
      vcl_cout << " ERROR: creating bocl_mem failed for SCORE when handling index " << ind_idx << vcl_endl;
      return false;
    }

    // create score output buffer and index bocl_mem
    unsigned char* score_buff = new unsigned char[cl_ni*cl_nj];
#if 0
    vcl_fill(score_buff, score_buff + cl_ni*cl_nj, (unsigned char)0);
    for (unsigned k = 0; k < cl_ni*cl_nj; k++)
      score_buff[k] = (unsigned char)0;
#endif
    bocl_mem* score = new bocl_mem(device->context(), score_buff, sizeof(unsigned char)*cl_ni*cl_nj, " score " );
    if (!score->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR )) {
      vcl_cout << "\n ERROR: creating bocl_mem failed for SCORE when handling index " << ind_idx << vcl_endl;
      delete index;       delete [] index_buff;
      delete score;       delete [] score_buff;
      delete voxel_size;  delete weight;  delete query_cl;
      delete order_cl;    delete order_offset_cl;  delete order_size_cl;
      delete weight_all_cl;
      return false;
    }

    // execute kernel on each device for min_dist matching
    if (!this->execute_match_kernel_min_dist_order(device, queue, query_cl, score, index, kernels[identifier][0])) {
      vcl_cout << "\n ERROR Executing MATCH_MIN_DIST kernel failed for index " << ind_idx << " on device " << identifier << vcl_endl;
      delete index;       delete [] index_buff;
      delete score;       delete [] score_buff;
      delete voxel_size;  delete weight;  delete query_cl;
      delete order_cl;    delete order_offset_cl;  delete order_size_cl;
      delete weight_all_cl;
      return false;
    }
    gpu_time_min_dist += kernels[identifier][0]->exec_time();
    
    // execute kernel for order matching
    float* score_order_buff = new float[n_cam];
    bocl_mem* score_order_cl = new bocl_mem(device->context(), score_order_buff, sizeof(float)*n_cam, " score_order " );
    if ( !score_order_cl->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR) ) {
      vcl_cout << "\n ERROR: create bocl_mem failed for SCORE_ORDER when handling index " << ind_idx << vcl_endl;
      delete index;           delete [] index_buff;
      delete score;           delete [] score_buff;
      delete score_order_cl;  delete [] score_order_buff;
      delete voxel_size;      delete weight;           delete query_cl;
      delete order_cl;        delete order_offset_cl;  delete order_size_cl;
      delete weight_all_cl;
      return false;
    }

    if (!this->execute_match_kernel_order_order(device, queue, index, order_cl, order_offset_cl,
                                                order_size_cl, score_order_cl, kernels[identifier][1])) {
      vcl_cout << "\n ERROR: executing MATCH_ORDER kernel failed for index " << ind_idx << " on device " << identifier << vcl_endl;
      delete index;           delete [] index_buff;
      delete score;           delete [] score_buff;
      delete score_order_cl;  delete [] score_order_buff;
      delete voxel_size;      delete weight;           delete query_cl;
      delete order_cl;        delete order_offset_cl;  delete order_size_cl;
      delete weight_all_cl;
      return false;
    }
    gpu_time_order += kernels[identifier][1]->exec_time();

#if 0
    // read to host
    score_order_cl->read_to_buffer(queue);
    vcl_cout << "\n ------ score for order matcher ---- " << vcl_endl;
    for (unsigned cam_id = 0; cam_id < n_cam; cam_id++) {
      vcl_cout << " index " << ind_idx << ", cam_id = " << cam_id 
               << " , score_order[ " << cam_id << "] = " << score_order_buff[cam_id] << vcl_endl;
    }

#endif

    // delete index from host and device
    delete index;
    status = clFinish(queue);
    check_val(status, MEM_FAILURE, " release INDEX failed on device " + device->device_identifier() + error_to_string(status));
    delete [] index_buff;

    // execute kernel for score summary
    float * score_cam_buff = new float[cl_ni];
    bocl_mem* score_cam = new bocl_mem(device->context(), score_cam_buff, sizeof(float)*cl_ni, " score_cam " );
    if (!score_cam->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR )) {
      vcl_cout << "\n ERROR: creating bocl_mem failed for SCORE_CAM when handling index " << ind_idx << vcl_endl;
      delete score;           delete [] score_buff;
      delete score_cam;       delete [] score_cam_buff;
      delete score_order_cl;  delete [] score_order_buff;
      delete voxel_size;      delete weight;           delete query_cl;
      delete order_cl;        delete order_offset_cl;  delete order_size_cl;
      delete weight_all_cl;
      return false;
    }
    if (!this->execute_weight_sum_kernel_order(device, queue, score, score_order_cl, weight_all_cl, voxel_size, weight, score_cam, kernels[identifier][2])) {
      vcl_cout << "\n ERROR Executing WEIGHTED_SUM kernel failed for index " << ind_idx << " on device " << identifier << vcl_endl;
      delete score;           delete [] score_buff;
      delete score_cam;       delete [] score_cam_buff;
      delete score_order_cl;  delete [] score_order_buff;
      delete voxel_size;      delete weight;           delete query_cl;
      delete order_cl;        delete order_offset_cl;  delete order_size_cl;
      delete weight_all_cl;
      return false;
    }
    gpu_time_sum += kernels[identifier][2]->exec_time();
    // extract score_cam output
    score_cam->read_to_buffer(queue);

#if 0
    // read to host
    vcl_cout << "\n ------ score for min_dist matcher ---- " << vcl_endl;
    for(unsigned cam_id = 0; cam_id < n_cam; cam_id++) {
      vcl_cout << " index " << ind_idx << ", cam_id = " << cam_id 
               << " , score_total[ " << cam_id << "] = " << score_cam_buff[cam_id] << vcl_endl;
    }
#endif

    status = clFinish(queue);
    check_val(status, MEM_FAILURE, " read SCORE_CAM output buffer FAILED on device " + device->device_identifier() + error_to_string(status));
    // find the maximum from score_cam_buff using map
    float max_score = score_cam_buff[0];
    unsigned max_cam_id = 0;
    for (unsigned k = 1; k < n_cam; k++) {
      float current_score = score_cam_buff[k];
      if (current_score > max_score) {
        max_score = current_score;
        max_cam_id = k;
      } 
    }
    score_all_.push_back(max_score);
    cam_all_id_.push_back(max_cam_id);

    // release device and host memory for score, score_order and score_cam
    delete score;
    delete score_cam;
    delete score_order_cl;
    status = clFinish(queue);
    check_val(status, MEM_FAILURE, " release SCORE/WEIGHT/SCORE_CAM failed on device " + device->device_identifier() + error_to_string(status));
    delete [] score_buff;
    delete [] score_order_buff;
    delete [] score_cam_buff;
  } //loop over all indices

  vcl_cout << '\n'
           << "  \t min_dist matcher kernel execution time ---\t " << gpu_time_min_dist << " milliseconds\n"
           << "  \t order matcher kernel execution time ---\t " << gpu_time_order << " milliseconds\n"
           << "  \t weight sum kernel execution time ---\t " << gpu_time_sum << " milliseconds\n"
           << "  \t total time for matching " << ei_ << " indices with " << n_cam << " cameras ---\t " << transfer.all() << " milliseconds" << vcl_endl;
  // release device memory for queries, weight
  delete query_cl;
  delete weight;
  delete voxel_size;
  delete order_cl;
  delete order_offset_cl;
  delete order_size_cl;
  delete weight_all_cl;
  clFinish(queue);
  check_val(status, MEM_FAILURE, " release query memory FAILED on device " + gpu_->device_identifier() + error_to_string(status));
  clReleaseCommandQueue(queue);

  ind_->finalize();
  return true;
}

bool boxm2_volm_matcher_order::execute_match_kernel_min_dist_order(bocl_device_sptr  device,
                                                                   cl_command_queue& queue,
                                                                   bocl_mem*         query,
                                                                   bocl_mem*         score,
                                                                   bocl_mem*         index,
                                                                   bocl_kernel*      kern)
{
  // create a buff for debug
  cl_int status;
  float* debug_buff = new float[100];
  bocl_mem* debug_out = new bocl_mem(device->context(), debug_buff, sizeof(float)*100, " debug_output " );
  debug_out->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  // set up argument list (push_back args into bocl_kernel)
  kern->set_arg(query);
  kern->set_arg(index);
  kern->set_arg(score);
  kern->set_arg(debug_out);
  // execute kernel
  if (!kern->execute(queue, work_dim, local_threads, global_threads)) {
    delete debug_out;
    status = clFinish(queue);
    check_val(status, MEM_FAILURE, " release debug buffer FAILED on device " + device->device_identifier() + error_to_string(status));
    if (!debug_buff)  delete [] debug_buff;
    return false;
  }
  // clFinish to ensure the execution is finished
  status = clFinish(queue);
  if (status != 0) {
    delete debug_out;
    status = clFinish(queue);
    check_val(status, MEM_FAILURE, " release debug buffer FAILED on device " + device->device_identifier() + error_to_string(status));
    delete [] debug_buff;
    return false;
  }
  // clear the kernel argument (has nothing to do with the queue)
  kern->clear_args();
#if 0
  // read debug from device
  debug_out->read_to_buffer(queue);
  for (unsigned i = 0; i < 6; i++) {
      vcl_cout << " debug_output[" << i << "] = " << debug_buff[i] << vcl_endl;
  }
#endif
  // clear the debug buffer
  delete debug_out;
  status = clFinish(queue);
  check_val(status, MEM_FAILURE, " release debug buffer FAILED on device " + device->device_identifier() + error_to_string(status));
  delete [] debug_buff;

  return true;
}

bool boxm2_volm_matcher_order::execute_match_kernel_order_order(bocl_device_sptr device,
                                                                cl_command_queue& queue,
                                                                bocl_mem* index,
                                                                bocl_mem* order,
                                                                bocl_mem* order_offset,
                                                                bocl_mem* order_size,
                                                                bocl_mem* score_order,
                                                                bocl_kernel* kern)
{
  cl_int status;
  float* debug_buff = new float[100];
  bocl_mem* debug_out = new bocl_mem(device->context(), debug_buff, sizeof(float)*100, " debug_output " );
  debug_out->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  // define a one dimension NR stucture, each work item is associated with one camera
  vcl_size_t local_threads_order = 8;
  vcl_size_t global_threads_order = (unsigned)RoundUp(n_cam,(int)local_threads_order);
  // set up argument list
  kern->set_arg(index);
  kern->set_arg(order);
  kern->set_arg(order_offset);
  kern->set_arg(order_size);
  kern->set_arg(score_order);
  kern->set_arg(debug_out);
  // execture kernel
  if (!kern->execute(queue, 1, &local_threads_order, &global_threads_order)) {
    delete debug_out;
    delete [] debug_buff;
    return false;
  }
#if 0
  // read debug from device
  debug_out->read_to_buffer(queue);
  vcl_cout << "\n ------- order_debug ------ " << vcl_endl;
  for (unsigned i = 0; i < 8; i++) {
      vcl_cout << " debug_output[" << i << "] = " << debug_buff[i] << vcl_endl;
  }
#endif
  // clear the kernel argument (has nothing to do with the queue)
  kern->clear_args();
  // clear the debug buffer
  delete debug_out;
  status = clFinish(queue);
  check_val(status, MEM_FAILURE, " release debug buffer FAILED on device " + device->device_identifier() + error_to_string(status));
  delete [] debug_buff;
  return true;
  return true;
}


bool boxm2_volm_matcher_order::execute_weight_sum_kernel_order(bocl_device_sptr device,
                                                               cl_command_queue& queue,
                                                               bocl_mem* score,
                                                               bocl_mem* score_order,
                                                               bocl_mem* weight_all,
                                                               bocl_mem* voxel_size,
                                                               bocl_mem* weight,
                                                               bocl_mem* score_cam,
                                                               bocl_kernel* kern)
{
  cl_int status;
  float* debug_buff = new float[100];
  bocl_mem* debug_out = new bocl_mem(device->context(), debug_buff, sizeof(float)*100, " debug_output " );
  debug_out->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  // redefine NR table stucture, where the current work_dim = 1
  vcl_size_t local_threads_sum = 8;
  vcl_size_t global_threads_sum = (unsigned)RoundUp(n_cam,(int)local_threads_sum);
  // set up argument list (push_back args into bocl_kernel)
  kern->set_arg(score);
  kern->set_arg(score_order);
  kern->set_arg(weight_all);
  kern->set_arg(voxel_size);
  kern->set_arg(weight);
  kern->set_arg(score_cam);
  kern->set_arg(debug_out);
  // execute kernel
  if (!kern->execute(queue, 1, &local_threads_sum, &global_threads_sum)) {
    delete debug_out;
    delete [] debug_buff;
    return false;
  }
  // clear the kernel argument (has nothing to do with the queue)
  kern->clear_args();
  // clear the debug buffer
  delete debug_out;
  status = clFinish(queue);
  check_val(status, MEM_FAILURE, " release debug buffer FAILED on device " + device->device_identifier() + error_to_string(status));
  delete [] debug_buff;
  return true;
}

bool boxm2_volm_matcher_order::write_score_order(vcl_string const& out_prefix)
{
  // write the score
  vcl_string out_score = out_prefix + "_score.bin";
  vsl_b_ofstream os(out_score.c_str());
  if (!os)
    return false;
  //vsl_b_write(os,score_all_.size());
  for (unsigned i = 0; i < score_all_.size(); i++)
    vsl_b_write(os, score_all_[i]);
  os.close();

  // write the camera id
  vcl_string out_cam = out_prefix + "_camera.bin";
  vsl_b_ofstream osc(out_cam.c_str());
  if (!osc)
    return false;
  for (unsigned i = 0; i < cam_all_id_.size(); i++)
    vsl_b_write(osc, cam_all_id_[i]);
  osc.close();

  return true;
}