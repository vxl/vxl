#include "boxm2_volm_matcher.h"
#include "boxm2_volm_wr3db_index.h"
#include <vcl_where_root_dir.h>
#include <vnl/vnl_random.h>
#include <vul/vul_timer.h>

boxm2_volm_matcher::boxm2_volm_matcher(volm_query_sptr query, boxm2_volm_wr3db_index_sptr ind, unsigned long ei, bocl_device_sptr gpu) : 
    query_(query), ind_(ind), ei_(ei), gpu_(gpu)
{
  vul_timer transfer;
  // define the work item and work group based on number of camera and the number of voxels inside the query
  work_dim = 2;
  unsigned int n_cam = query_->get_cam_num();     // number of camera;
  unsigned int n_obj = query_->get_query_size();  // number of objects;
  local_threads[0] = 8;
  local_threads[1] = 8;
  cl_ni=(unsigned)RoundUp(n_cam,(int)local_threads[0]);  // row is camera
  cl_nj=(unsigned)RoundUp(n_obj,(int)local_threads[1]);  // col is voxel
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  // create the 1D query_array buffer for all devices, each 1D array has length cl_ni x cl_nj
  queries_buff = new unsigned char[cl_ni*cl_nj];
  vcl_vector<vcl_vector<unsigned char> >& layers = query_->min_dist();
  // initialize 1D the query 
  for(unsigned i = 0; i < cl_ni*cl_nj; i++) {
    queries_buff[i] = (unsigned char)255;
  }
  // fill the query array from query 
  for(unsigned i = 0; i < query_->get_cam_num(); i++) {
    for(unsigned k = 0; k < query_->get_query_size(); k++){
      unsigned idx = i * cl_nj + k;
      queries_buff[idx] = (query_->min_dist())[i][k];
    }
  }
  // create bocl_mem for query, buffer has been defined in queries_buff
  query_cl = new bocl_mem(gpu_->context(), queries_buff, sizeof(unsigned char)*cl_ni*cl_nj, " query " );
  if(!query_cl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )){
    delete query_cl;
    vcl_cout << " ERROR: create buffer failed for query " << vcl_endl;
  }
  vcl_cout << "  1. Transfering queries to 1D array for all devices --- \t" << transfer.all()/1000.0 << " seconds. " << vcl_endl;
  // create command queue lists for all devices
  if(!this->setup_queue()){
    vcl_cerr << "ERROR happens when create queue and setup queries for each device\n";
    delete query_cl;
  }
  //: compile kernel
  vcl_string identifier = gpu_->device_identifier();
  if(kernels.find(identifier) == kernels.end()) {
    vcl_cout << "===========Compiling kernels for device " << identifier << "==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    if(!compile_kernel(gpu_,ks)){ 
      vcl_cerr << " ERROR happens when compiling kernels on device " << identifier << "\n";
      delete query_cl;
    }
    kernels[identifier] = ks;
  }
  vcl_cout << "  2. Create queues and compile kernels for all device --- \t" << transfer.all()/1000.0 << " seconds. " << vcl_endl;
}

boxm2_volm_matcher::~boxm2_volm_matcher()
{
  delete queries_buff;
}

bool boxm2_volm_matcher::setup_queue()
{
  // create queue
  bocl_device_sptr device = gpu_;
  cl_int status = SDK_FAILURE;
  queue = clCreateCommandQueue(device->context(), *(device->device_id()), CL_QUEUE_PROFILING_ENABLE, &status);
  if( !check_val(status, CL_SUCCESS, error_to_string(status)) ) {
    vcl_cerr << "ERROR when creating queue on device " << device->device_identifier() << "\n" ;
    return false;
  }
  return true;
}

bool boxm2_volm_matcher::compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels)
{
  //: declare the kernel source
  vcl_vector<vcl_string> src_paths;
  vcl_string volm_cl_source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/volm/cl/";
  src_paths.push_back(volm_cl_source_dir + "generalized_volm_matching_layer.cl");
  //: create the kernel
  vcl_cout << src_paths[0] << vcl_endl;
  bocl_kernel* volm_match = new bocl_kernel();
  if(!volm_match->create_kernel(&device->context(), device->device_id(), src_paths, "generalized_volm_matching_layer", "", "generalized_volm_matching_layer") ){
      return false;
  }
  vec_kernels.push_back(volm_match);
  return true;
}

bool boxm2_volm_matcher::matching_cost_layer()
{
  cl_int status;
  unsigned layer_size = (unsigned)query_->get_query_size();
  vul_timer transfer;
  float gpu_time = 0.0f;
  unsigned count = (unsigned)(0.1*ei_);
  vcl_cerr << "  3. Start to match all " << ei_ << "(" << ei_ << ") indices with " << query_->get_cam_num() << " cameras per index " ;
  for(unsigned indIdx = 0; indIdx < ei_; indIdx++) {
    if(!(indIdx % count) ) vcl_cerr << "." ;
    bocl_device_sptr device = gpu_;
    vcl_string identifier = device->device_identifier();
    // create index buffer and index bocl_mem
    unsigned char* index_buff = new unsigned char[cl_nj];
    for(unsigned k = 0; k < cl_nj; k++)
      index_buff[k] = (unsigned char)0;
    vcl_vector<unsigned char> values(layer_size);
    ind_->get_next(values);
    for(unsigned k = 0; k < layer_size; k++) 
      index_buff[k] = values[k];
    bocl_mem* index = new bocl_mem(device->context(), index_buff, sizeof(unsigned char)*cl_nj, " index " );
    if(!index->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR )){
      delete index;
      delete query_cl;
      delete [] index_buff;
      vcl_cout << " ERROR: create buffer failed for index when handling index " << indIdx << vcl_endl;
      return false;
    }
    // create score output buffer and index bocl_mem
    unsigned char* score_buff = new unsigned char[cl_ni*cl_nj];
    for(unsigned k = 0; k < cl_ni*cl_nj; k++)
      score_buff[k] = 0;
    bocl_mem* score = new bocl_mem(device->context(), score_buff, sizeof(unsigned char)*cl_ni*cl_nj, " score " );
    if(!score->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR )){
      delete index;
      delete score;
      delete query_cl;
      delete [] index_buff;
      delete [] score_buff;
      vcl_cout << "\n ERROR: create buffer failed for score when handling index " << indIdx << vcl_endl;
      return false;
    }
    // execute kernel on each device for matching
    if(!this->execute_kernel(device, queue, query_cl, score, index, kernels[identifier][0])){
      delete index;
      delete score;
      delete query_cl;
      status = clFinish(queue);
      check_val(status, MEM_FAILURE, "\n release index buffer FAILED on device " + device->device_identifier() + error_to_string(status));
      delete [] index_buff;
      delete [] score_buff;
      vcl_cerr << "\n ERROR happens when execute kernel for index " << indIdx << " on device " << identifier << "\n";
      return false;
    }
    gpu_time += kernels[identifier][0]->exec_time();
    // extract the output
    score->read_to_buffer(queue);
    status = clFinish(queue);
    check_val(status, MEM_FAILURE, "\n Read to output buffers FAILED on device " + device->device_identifier() + error_to_string(status));
    // summerize the score for each camera
    vcl_vector<float> score_cam(cl_ni);
    for(unsigned camIdx = 0; camIdx < cl_ni; camIdx++) {
      score_cam[camIdx] = 0;
      for(unsigned objIdx = 0; objIdx < cl_nj; objIdx++) {
        unsigned idx = camIdx * cl_nj + objIdx;
        score_cam[camIdx] += (float)score_buff[idx];
      }
    }
    // find the maximum score and normalize for current index
    float max_score = 0;
    unsigned int cam_id = 0;
    for(unsigned k = 0; k < cl_ni; k++)
      if(max_score < score_cam[k]){
        max_score = score_cam[k];
        cam_id = k;
      }
    score_all_.push_back(max_score/query_->get_valid_ray_num(cam_id));
    cam_all_id_.push_back(cam_id);
    // release device and host memory for index and score
    delete score;
    delete index;
    status = clFinish(queue);
    check_val(status, MEM_FAILURE, " release index and score memory FAILED on device " + device->device_identifier() + error_to_string(status));
    delete [] index_buff;
    delete [] score_buff;
  } //loop over all indices
  vcl_cout << "\n";
  vcl_cout << "  \t kernel execution time ---\t " << gpu_time << " milliseconds" << vcl_endl;
  vcl_cout << "  \t total time for matching " << ei_ << " indices with " << query_->get_cam_num() << " cameras ---\t " << transfer.all()/1000 << " seconds " << vcl_endl;
  // release device memory for queries
  delete query_cl;
  clFinish(queue);
  check_val(status, MEM_FAILURE, " release query memory FAILED on device " + gpu_->device_identifier() + error_to_string(status));
  clReleaseCommandQueue(queue);
  return true;
}

bool boxm2_volm_matcher::execute_kernel(bocl_device_sptr device,
                                       cl_command_queue& queue,
                                               bocl_mem* query,
                                               bocl_mem* score,
                                               bocl_mem* index,
                                            bocl_kernel* kern)
{
  // create a buff for debug
  cl_int status;
  float* debug_buff = new float[100];
  bocl_mem* debug_out = new bocl_mem(device->context(), debug_buff, sizeof(float)*100, " debug_output " );
  debug_out->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  //: set up argument list (push_back args into bocl_kernel)
  kern->set_arg(query);
  kern->set_arg(index);
  kern->set_arg(score);
  kern->set_arg(debug_out);
  // execute kernel
  if(!kern->execute(queue, work_dim, local_threads, global_threads)){
    delete debug_out;
    status = clFinish(queue);
    check_val(status, MEM_FAILURE, " release debug buffer FAILED on device " + device->device_identifier() + error_to_string(status));
    if(!debug_buff)  delete [] debug_buff;
    return false;
  }
  // clFinish to ensure the execution is finished
  status = clFinish(queue);
  if (status != 0) {
    delete debug_out;
    status = clFinish(queue);
    check_val(status, MEM_FAILURE, " release debug buffer FAILED on device " + device->device_identifier() + error_to_string(status));
    if(!debug_buff)  delete [] debug_buff;
    return false;
  }
  // clear the kernal argument (has nothing to do with the queue)
  kern->clear_args();
#if 0
  // read debug from device
  debug_out->read_to_buffer(queue);
  for(unsigned i = 0; i < 6; i++){
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

bool boxm2_volm_matcher::write_score(vcl_string const& out_prefix)
{
  // write the score
  vcl_string out_score = out_prefix + "_score.bin";
  vsl_b_ofstream os(out_score.c_str());
  if(!os)
    return false;
  //vsl_b_write(os,score_all_.size());
  for(unsigned i = 0; i < score_all_.size(); i++)
    vsl_b_write(os, score_all_[i]);
  os.close(); 

  //: write the camera id
  vcl_string out_cam = out_prefix + "_camera.bin";
  vsl_b_ofstream osc(out_cam.c_str());
  if(!osc)
    return false;
  for(unsigned i = 0; i < cam_all_id_.size(); i++)
    vsl_b_write(osc, cam_all_id_[i]);
  osc.close();

  return true;
}