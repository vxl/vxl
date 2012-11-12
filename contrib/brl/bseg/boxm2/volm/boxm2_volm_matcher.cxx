#include "boxm2_volm_matcher.h"
#include "boxm2_volm_wr3db_index.h"
#include <vcl_where_root_dir.h>
#include <vnl/vnl_random.h>
#include <vul/vul_timer.h>

boxm2_volm_matcher::boxm2_volm_matcher(volm_query_sptr query, boxm2_volm_wr3db_index_sptr ind, unsigned long ei, vcl_vector<bocl_device_sptr> gpus) : 
    query_(query), ind_(ind), ei_(ei), gpus_(gpus)
{
#if 0
  //: define the work item and work group based on number of camera and the number of objects
  work_dim = 2;
  unsigned int n_cam = query_->get_cam_num();  // number of camera;
  unsigned int n_obj = query_->get_obj_num();  // number of objects;

  local_threads[0] = 8;
  local_threads[1] = 8;
  unsigned query_size = query_->get_query_size();
  cl_ni=(unsigned)RoundUp(n_obj,(int)local_threads[0]); // row is object
  cl_nj=(unsigned)RoundUp(n_cam,(int)local_threads[1]); // col is camera
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;
#if 0  
  vcl_cout << " cam_num = " << n_cam << ", obj_num = " << n_obj << vcl_endl;
  vcl_cout << " cl_ni = " << cl_ni << ", cl_nj = " << cl_nj << vcl_endl;
#endif  
  //: transfer query to 1D array, store the offset and counter for each object
  //: the counter and offset array should have length of cl_ni x cl_nj
  
  //: declare input
  query_offset_buff = new unsigned int[cl_ni*cl_nj];
  query_counter_buff = new unsigned int[cl_ni*cl_nj];
  query_prop_buff = new unsigned int[cl_ni*cl_nj];
  query_voxel_buff = new unsigned int[query_size];

  //: declare ouput
  score_obj_buff = new float[cl_ni*cl_nj];

  //: initialize the arrays (the correctness of query_voxel_buff should be ensured when create query, meansing the voxel index stored in query should neven exceed container size
  for(unsigned i = 0; i < (cl_ni*cl_nj); i++){ // clear the invalid start[k] = counter[k] = 0 when k > (n_cam*n_obj)
	  query_offset_buff[i] = 0;
	  query_counter_buff[i] = 0;
	  query_prop_buff[i] = 1231;
	  score_obj_buff[i] = 0;
  }

  unsigned int total_voxel = 0;
  unsigned int total_offset = 0;
  unsigned int total_counter = 0;
  unsigned int total_prop = 0;
  
  //: sky object
  if(query_->sky().size()) {
	query_offset_buff[total_offset++] = 0;

    vcl_vector<unsigned int>& sky = query_->sky();
    vcl_vector<unsigned int>& sky_idx = query_->sky_offset();
    unsigned int sky_sz = (unsigned int)sky.size();
    unsigned int sky_idx_sz = (unsigned int)sky_idx.size();
    for(unsigned i = 0; i < sky_sz; i++)
      query_voxel_buff[total_voxel++] = sky[i];
    for(unsigned i = 0; i < sky_idx.size()-1; i++) { // note the sky_idx[last] = total number of voxel for all cameras
      query_counter_buff[total_counter++] = sky_idx[i+1] - sky_idx[i];
	  query_offset_buff[total_offset++] = query_offset_buff[total_offset-1] + query_counter_buff[total_counter-1];
	  query_prop_buff[total_prop++] = 0;
    }
	query_counter_buff[total_counter++] = (unsigned int)sky.size() - sky_idx[sky_idx.size()-1];
	query_offset_buff[total_offset++] = query_offset_buff[total_offset-1] + query_counter_buff[total_counter-1];
	query_prop_buff[total_prop++] = 0;
  }

  //: non_planar objects
  if(query_->non_planar().size()) {
	if(!total_offset)
      query_offset_buff[total_offset++] = 0;

    unsigned int np_size = (unsigned int)(query_->non_planar().size());
    for(unsigned i = 0; i < np_size; i++) {
      vcl_vector<unsigned int>& np = query_->non_planar()[i];
	  vcl_vector<vnl_vector_fixed<unsigned int, 3> >& np_idx = query_->non_planar_offset()[i];
	  //: store the voxels whose distance is smaller than min_dist
	  for(unsigned j = 0; j < np_idx.size(); j++) {
        for(unsigned k = np_idx[j][0]; k < np_idx[j][1]; k++)
          query_voxel_buff[total_voxel++] = np[k];
      query_counter_buff[total_counter++] = np_idx[j][1] - np_idx[j][0];
      query_offset_buff[total_offset++] = query_offset_buff[total_offset-1] + query_counter_buff[total_counter-1];
      query_prop_buff[total_prop++] = 1;
	  }
	  //: store voxels whose distance is larger than min_dist but inside boundary
	  for(unsigned j = 0; j < np_idx.size(); j++) {
        for(unsigned k = np_idx[j][1]; k < np_idx[j][2]; k++)
          query_voxel_buff[total_voxel++] = np[k];
      query_counter_buff[total_counter++] = np_idx[j][2] - np_idx[j][1];
      query_offset_buff[total_offset++] = query_offset_buff[total_offset-1] + query_counter_buff[total_counter-1];
      query_prop_buff[total_prop++] = 2;
	  }
	  //: store voxel outside the boundary
	  for(unsigned j = 0; j < (np_idx.size()-1); j++){
        for(unsigned k = np_idx[j][2]; k < np_idx[j+1][0]; k++)
          query_voxel_buff[total_voxel++] = np[k];
      query_counter_buff[total_counter++] = np_idx[j+1][0] - np_idx[j][2];
      query_offset_buff[total_offset++] = query_offset_buff[total_offset-1] + query_counter_buff[total_counter-1];
      query_prop_buff[total_prop++] = 3;
	  }
	  for(unsigned k = np_idx[np_idx.size()-1][2]; k < np.size(); k++)
        query_voxel_buff[total_voxel++] = np[k];
	  query_counter_buff[total_counter++] = (unsigned int)np.size() - np_idx[np_idx.size()-1][2];
	  query_offset_buff[total_offset++] = query_offset_buff[total_offset-1] + query_counter_buff[total_counter-1];
	  query_prop_buff[total_prop++] = 3;
    } // loop overl all non_planar objects
  }

  //: flat object
  if(query_->flat().size()) {
	  // do something
  }

  if(query_->wall().size()) {
	  // do something
  }
#endif
  vul_timer transfer;
  //: define the work item and work group based on number of camera and the number of voxels in the first layer of voxel having 2*vmin resolution
  work_dim = 2;
  unsigned int n_cam = query_->get_cam_num();  // number of camera;
  unsigned int n_obj = query_->get_query_size();  // number of objects;
  local_threads[0] = 8;
  local_threads[1] = 8;
  cl_ni=(unsigned)RoundUp(n_cam,(int)local_threads[0]); // row is camera
  cl_nj=(unsigned)RoundUp(n_obj,(int)local_threads[1]); // col is voxel
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  for(unsigned gpuIdx = 0; gpuIdx < gpus.size(); gpuIdx++) {
    //: create the 1D query_array buffer for all devices, each 1D array has length cl_ni x cl_nj
    unsigned char* query_layer_buff = new unsigned char[cl_ni*cl_nj];
	vcl_vector<vcl_vector<unsigned char> >& layers = query_->min_dist();
	//: initialize the query 
    for(unsigned i = 0; i < cl_ni*cl_nj; i++) {
      query_layer_buff[i] = (unsigned char)255;
    }
	//: fill the query array from query 
    for(unsigned i = 0; i < query_->get_cam_num(); i++) {
      for(unsigned k = 0; k < query_->get_query_size(); k++){
		unsigned idx = i * cl_nj + k;
		query_layer_buff[idx] = (query_->min_dist())[i][k];
	  }
    }
	queries_buff.push_back(query_layer_buff);
  }

  vcl_cout << "  1. Transfering queries to 1D array for all devices --- \t" << transfer.all()/1000.0 << " seconds. " << vcl_endl;

#if 0
  //: test the output
  vcl_cout << " cam number = " << n_cam << vcl_endl;
  vcl_cout << " obj number = " << n_obj << vcl_endl;
  vcl_cout << " [cl_ni , cl_nj] = " << cl_ni << " " << cl_nj << vcl_endl;
  vcl_cout << " device number = " << gpus_.size() << vcl_endl;
  vcl_cout << "query voxels" << vcl_endl;
  for(unsigned i = 0; i < cl_ni; i++) {
    for(unsigned k = 0; k < cl_nj; k++){
	  unsigned idx = i * query_->get_query_size() + k;
      vcl_cout << "[ " << i << " " << k << " " << idx << " ] = " << (int)queries_buff[0][idx]  << vcl_endl;
	}
	vcl_cout << "\n";
  }
#endif
  
  //: set up queries and command queue lists for all devices
  if(!this->setup_queue()){
	vcl_cerr << "ERROR happens when create queue and setup queries for each device\n";
	for(unsigned i = 0; i < queries.size(); i++)
		delete queries[i];
  }

  //: compile kernel
  for(unsigned i = 0; i < gpus_.size(); i++) {
    bocl_device_sptr device = gpus_[i];
	vcl_string identifier = device->device_identifier();
	if(kernels.find(identifier) == kernels.end()) {
      vcl_cout << "===========Compiling kernels for device " << identifier << "==========="<<vcl_endl;
	  vcl_vector<bocl_kernel*> ks;
	  if(!compile_kernel(device,ks)){ 
        vcl_cerr << "ERROR happens when compiling kernels on device " << identifier << "\n";
		for(unsigned i = 0; i < queries.size(); i++)
		  delete queries[i];
	  }
      kernels[identifier]=ks;
	}
  }

  vcl_cout << "  2. Create queues and compile kernels for all device --- \t" << transfer.all()/1000.0 << " seconds. " << vcl_endl;
}

boxm2_volm_matcher::~boxm2_volm_matcher()
{
  for(unsigned i = 0; i < queries_buff.size(); i++) {
    delete [] queries_buff[i]; 
  }
}

bool boxm2_volm_matcher::setup_queue()
{
  for(unsigned i = 0; i < gpus_.size(); i++) {
    bocl_device_sptr device = gpus_[i];
	//: create queue for current device
	cl_int status = SDK_FAILURE;
	cl_command_queue queue = clCreateCommandQueue(device->context(), *(device->device_id()), CL_QUEUE_PROFILING_ENABLE, &status);
	if( !check_val(status, CL_SUCCESS, error_to_string(status)) ) {
	  vcl_cerr << "ERROR when creating queue on device " << device->device_identifier() << "\n" ;
	  return false;
    }
	queues.push_back(queue);
	//: create bocl_mem query for each device, buffer has been defined in queries_buff
	bocl_mem* query = new bocl_mem(device->context(), queries_buff[i], sizeof(unsigned char)*cl_ni*cl_nj, " query " );
	query->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
	queries.push_back(query);
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
  unsigned numGPU = (unsigned)gpus_.size();
  unsigned layer_size = (unsigned)query_->get_query_size();
  unsigned long EI = (unsigned long)RoundUp(ei_, (int)numGPU);
  unsigned count = 0;
  vul_timer transfer;
  float gpu_time = 0.0f;
  vcl_cout << "  3. Start to match all " << ei_ << "(" << EI << ") indices with " << query_->get_cam_num() << " cameras per index " ;
  for(unsigned indIdx = 0; indIdx < EI; indIdx += numGPU) {
	if(!(indIdx%8)) vcl_cout << "." ;
	for(unsigned i = 0; i < numGPU; i++) {
	  bocl_device_sptr device = gpus_[i];
	  vcl_string identifier = device->device_identifier();
	  //vcl_cout << "now handling GPU " << identifier << vcl_endl;
      //: create index buffer for every device
      unsigned char* index_buff = new unsigned char[cl_nj];
	  for(unsigned k = 0; k < cl_nj; k++)
        index_buff[k] = (unsigned char)0;
      if(count < ei_) {
        vcl_vector<unsigned char> values(layer_size);
        ind_->get_next(values);
		count++;
        for(unsigned k = 0; k < layer_size; k++) 
          index_buff[k] = values[k];
	  }
	  //: create bocl_mem for each device
	  bocl_mem* index = new bocl_mem(device->context(), index_buff, sizeof(unsigned char)*cl_nj, " index " );
	  index->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
	  //: create score output buffer and bocl_mem for each device
	  unsigned char* score_buff = new unsigned char[cl_ni*cl_nj];
	  for(unsigned k = 0; k < cl_ni*cl_nj; k++)
        score_buff[k] = 0;
	  bocl_mem* score = new bocl_mem(device->context(), score_buff, sizeof(unsigned char)*cl_ni*cl_nj, " score " );
	  score->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
	  //: execute kernel on each device for matching
	  if(!this->execute_kernel(device, queues[i], queries[i], score, index, kernels[identifier][0])){
		delete index;
		status = clFinish(queues[i]);
		check_val(status, MEM_FAILURE, " release index buffer FAILED on device " + device->device_identifier() + error_to_string(status));
		if(!index_buff)  delete [] index_buff;
		vcl_cerr << "ERROR happens when execute kernel on device " << identifier << "\n";
		return 0.0f;
	  }
	  gpu_time += kernels[identifier][0]->exec_time();
	  //: extract the output
	  score->read_to_buffer(queues[i]);
	  status = clFinish(queues[i]);
	  check_val(status, MEM_FAILURE, "read to output buffers FAILED on device " + device->device_identifier() + error_to_string(status));
	  //: summerize the score for each camera
	  vcl_vector<float> score_cam(cl_ni);
	  for(unsigned camIdx = 0; camIdx < cl_ni; camIdx++) {
        score_cam[camIdx] = 0;
		for(unsigned objIdx = 0; objIdx < cl_nj; objIdx++) {
		  unsigned idx = camIdx * cl_nj + objIdx;
		  //if(indIdx == 0 && camIdx < query_->get_cam_num() && camIdx == 125 && objIdx < query_->get_query_size()){
		  //  vcl_cout << " cam" << camIdx << ", obj " << objIdx 
			 //        << ", score[" << idx << "] = " << (int)score_buff[idx] 
		  //           << ", query_depth = " << (int)queries_buff[0][idx]
		  //           << vcl_endl;
		  //}
          score_cam[camIdx] += (float)score_buff[idx];
		}
	  }

	  //: find the maximum score and normalize for current index
	  float max_score = 0;
	  unsigned int cam_id = 0;
	  for(unsigned k = 0; k < cl_ni; k++)
        if(max_score < score_cam[k]){
			max_score = score_cam[k];
			cam_id = k;
		}
	  //vcl_cout << "indIdx = " << indIdx << ", max_score = " << (int)max_score << vcl_endl;
	  score_all_.push_back(max_score/query_->get_valid_ray_num(cam_id));
	  cam_all_id_.push_back(cam_id);
	  //: release device and host memory for index and score
	  delete score;
	  delete index;
	  status = clFinish(queues[i]);
	  check_val(status, MEM_FAILURE, " release index and score memory FAILED on device " + device->device_identifier() + error_to_string(status));
	  if(!index_buff) {
        delete [] index_buff;
		delete [] score_buff;
	  }
    } //loop over device
  } //loop over all index
  vcl_cout << "\n";
  vcl_cout << "  \t kernel execution time ---\t " << gpu_time << " nanoseconds" << vcl_endl;
  vcl_cout << "  \t total time for matching " << EI << " indices ---\t " << transfer.all()/1000 << " seconds " << vcl_endl;
  //: release device memory for queries
  for(unsigned i = 0; i < gpus_.size(); i++){
    delete queries[i];
	clFinish(queues[i]);
    check_val(status, MEM_FAILURE, " release query memory FAILED on device " + gpus_[i]->device_identifier() + error_to_string(status));
  }
  //: release all queues
  for(unsigned i = 0; i < gpus_.size(); i++) {
    clReleaseCommandQueue(queues[i]);
  }
  return true;
}

bool boxm2_volm_matcher::execute_kernel(bocl_device_sptr device,
                                       cl_command_queue& queue,
                                               bocl_mem* query,
                                               bocl_mem* score,
                                               bocl_mem* index,
                                            bocl_kernel* kern)
{
  //: create a buff for debug
  cl_int status;
  float* debug_buff = new float[100];
  bocl_mem* debug_out = new bocl_mem(device->context(), debug_buff, sizeof(float)*100, " debug_output " );
  debug_out->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  //: set up argument list (push_back args into bocl_kernel)
  kern->set_arg(query);
  kern->set_arg(index);
  kern->set_arg(score);
  kern->set_arg(debug_out);
  //: execute kernel
  if(!kern->execute(queue, work_dim, local_threads, global_threads)){
    delete debug_out;
	status = clFinish(queue);
	check_val(status, MEM_FAILURE, " release debug buffer FAILED on device " + device->device_identifier() + error_to_string(status));
	if(!debug_buff)  delete [] debug_buff;
	return false;
  }
  //: clFinish to ensure the execution is finished
  status = clFinish(queue);
  if (status != 0) {
    delete debug_out;
	status = clFinish(queue);
	check_val(status, MEM_FAILURE, " release debug buffer FAILED on device " + device->device_identifier() + error_to_string(status));
	if(!debug_buff)  delete [] debug_buff;
	return false;
  }
  //: clear the kernal argument (has nothing to do with the queue)
  kern->clear_args();
#if 0
  //: read debug from device
  debug_out->read_to_buffer(queue);
  for(unsigned i = 0; i < 6; i++){
	  vcl_cout << " debug_output[" << i << "] = " << debug_buff[i] << vcl_endl; 
  }
#endif
  //: clear the debug buffer
  delete debug_out;

  status = clFinish(queue);
  check_val(status, MEM_FAILURE, " release debug buffer FAILED on device " + device->device_identifier() + error_to_string(status));
  if(!debug_buff)  delete [] debug_buff;

  return true;
}

bool boxm2_volm_matcher::write_score(vcl_string const& out_prefix)
{
  //: write the score
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

#if 0
double boxm2_volm_matcher::matching_cost()
{ 
  //for(unsigned int i = 0; i < (unsigned int)global_threads[0]; i++)
  //  for(unsigned int j = 0; j < (unsigned int)global_threads[1]; j++)
  //    this->test_volm_matcher(i,j);

  //: compile kernel
  if(!this->compile_kernel()) {
    vcl_cerr << "ERROR happens when compile kernel in cost_matcher\n";
  }

  float gpu_time = 0.0;
  //: declare Opencl memory
  //:  input
  bocl_mem_sptr query_voxel = new bocl_mem(device_->context(), query_voxel_buff, sizeof(unsigned int)*query_->get_query_size(), " query_voxel " );
  query_voxel->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  
  bocl_mem_sptr query_offset = new bocl_mem(device_->context(), query_offset_buff, sizeof(unsigned int)*cl_ni*cl_nj, " query_offset " );
  query_offset->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  
  bocl_mem_sptr query_counter = new bocl_mem(device_->context(), query_counter_buff, sizeof(unsigned int)*cl_ni*cl_nj, " query_counter " );
  query_counter->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  
  bocl_mem_sptr query_prop = new bocl_mem(device_->context(), query_prop_buff, sizeof(unsigned int)*cl_ni*cl_nj, " query_prop " );
  query_prop->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  //: output
  bocl_mem_sptr score_obj = new bocl_mem(device_->context(), score_obj_buff, sizeof(float)*cl_ni*cl_nj, " score_obj " );
  score_obj->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  //: debug array
  float debug_buff[1000];
  for(unsigned i = 0; i < 1000; i++)
	  debug_buff[i] = 1231.0f;
  bocl_mem_sptr debug_output = new bocl_mem(device_->context(), debug_buff, sizeof(float)*1000, " debug_output " );
  debug_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  
  //: create queue
  cl_int status = SDK_FAILURE;
  cl_command_queue queue = clCreateCommandQueue(device_->context(), *(device_->device_id()), CL_QUEUE_PROFILING_ENABLE, &status);
  if ( !check_val(status, CL_SUCCESS, error_to_string(status)) ) {
	  vcl_cerr << "ERROR when creating queue\n" ;
  }

  //: set argument in kernel ... The sequence of arguments has to be same as cl function
  kern->set_arg(query_voxel.ptr());
  kern->set_arg(query_offset.ptr());
  kern->set_arg(query_counter.ptr());
  kern->set_arg(query_prop.ptr());

  kern->set_arg(score_obj.ptr());
  kern->set_arg(debug_output.ptr());

  //: execute kernel on gpu
  kern->execute(queue, work_dim, local_threads, global_threads);

  //: output
  score_obj->read_to_buffer(queue);
  debug_output->read_to_buffer(queue);

  
  

  /*unsigned k = 0;
  while(debug_buff[k] != 1231){
	  vcl_cout << "debug[" << k << "] = " << debug_buff[k] << vcl_endl;
	  k++;
  }*/

  //: clean kernel
  status = clFinish(queue);
  check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));

  gpu_time += kern->exec_time();
  vcl_cout << " query size = " << query_->get_query_size() 
	       << " , cam = " << query_->get_cam_num()
		   << " ---> gpu matching time = " << gpu_time << " ms, " << vcl_endl;

  clFinish(queue);
    //clear render kernel args so it can reset em on next execution
  kern->clear_args();
  clReleaseCommandQueue(queue);

  return 12.31;
}

bool boxm2_volm_matcher::test_volm_matcher(unsigned int i, unsigned int j)
{
  unsigned idx  = j + i*(unsigned)global_threads[1];
  if(query_layer_buff[idx] == 254 && query_layer_buff[idx] == ind_layer_buff[j]) {  // sky
    score_obj_buff[idx] += 1.0f;
  }else if(query_layer_buff[idx] <= ind_layer_buff[j]){
    score_obj_buff[idx] += 1.0f;
  }


  /*unsigned llid  = j + i*(unsigned)global_threads[1];
  switch(query_prop_buff[llid]){
    case 0:
      vcl_cout << "i = " << i << ", j = " << j << " ---> llid = " << llid << ", prop = " << query_prop_buff[llid] << ", SKY " << vcl_endl;
      for(unsigned k = 0; k < query_counter_buff[llid]; k++) {
        unsigned idx = k + query_offset_buff[llid];
		score_obj_buff[llid]++;
	  }
      break;
	case 1:
      vcl_cout << "i = " << i << ", j = " << j << " ---> llid = " << llid << ", prop = " << query_prop_buff[llid] << ", NON_PLANAR+SMALL " << vcl_endl;
	  for(unsigned k = 0; k < query_counter_buff[llid]; k++) {
        unsigned idx = k + query_offset_buff[llid];
		score_obj_buff[llid]++;
	  }
      break;
	case 2:
      vcl_cout << "i = " << i << ", j = " << j << " ---> llid = " << llid << ", prop = " << query_prop_buff[llid] << ", NON_PLANAR+MIDDLE " << vcl_endl;
	  for(unsigned k = 0; k < query_counter_buff[llid]; k++) {
        unsigned idx = k + query_offset_buff[llid];
		score_obj_buff[llid]++;
	  }
      break;
	case 3:
      vcl_cout << "i = " << i << ", j = " << j << " ---> llid = " << llid << ", prop = " << query_prop_buff[llid] << ", NON_PLANAR+OUT " << vcl_endl;
	  for(unsigned k = 0; k < query_counter_buff[llid]; k++) {
        unsigned idx = k + query_offset_buff[llid];
		score_obj_buff[llid]++;
	  }
	  break;
    default:
        vcl_cout << "i = " << i << ", j = " << j << " ---> idx = " << llid << ", prop = " << query_prop_buff[llid] << ", Unknown object properties " << vcl_endl;
	}
*/
  return true;
}
#endif