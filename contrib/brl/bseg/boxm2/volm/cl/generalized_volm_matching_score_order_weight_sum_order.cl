#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
# if NVIDIA
  #pragma OPENCL EXTENSION cl_khr_go_sharing : enable
#endif

// do the weighted summation based on the object order (1D work dim with work item idx = camera id

__kernel
void generalized_volm_matching_score_order_weight_sum_order (__global unsigned char* score_obj,
                                                       __global         float* score_order,
                                                       __global         float* weight_all,
                                                       __global  unsigned int* voxel_size,
                                                       __global         float* weight,
                                                       __global         float* score_cam,
                                                       __global         float* debug_output)
{
  // get camera id
  unsigned int i = get_global_id(0);
  // get number of voxel for current camera (should be constant for all cameras)
  // voxel_size[0] == cl_nj in previous matcher kernel, voxel_size[1] is the actually number of voxels
  unsigned int start = i*voxel_size[0];
  unsigned int end = start+voxel_size[1];
  float cam_weight = weight[i];
  
  // get offset and end for current camera
  float score_min_dist = 0.0f;
  for (unsigned j = start; j < end; j++) {
    //score += weight[j]*(float)score_obj[j];
    score_min_dist += (float)score_obj[j];
  }
  score_min_dist = score_min_dist/cam_weight;  // weight is the number of valid rays for this camera
  score_min_dist = weight_all[0] * score_min_dist;  // weight_all is the weight parameter for order score and min_dist score
  
  score_cam[i] = score_min_dist + weight_all[1]*score_order[i];
}
