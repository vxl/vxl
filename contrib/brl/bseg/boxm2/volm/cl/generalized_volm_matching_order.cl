//#pragma opencl extension cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
# if NVIDIA
  #pragma OPENCL EXTENSION cl_khr_go_sharing : enable
#endif

__kernel
void generalized_volm_matching_order (__global unsigned char* index,
                                      __global unsigned*      order,
                                      __global unsigned*      order_offset,
                                      __global unsigned*      order_size,
                                      __global    float*      score_order,
                                      __global    float*      debug_output)
{
  unsigned cam_id;
  cam_id = get_global_id(0);

  unsigned n_cam, n_obj;
  n_cam = order_size[0];
  n_obj = order_size[1];

  if (cam_id < n_cam) {
    unsigned offset_start, offset_end;
    offset_start = cam_id * n_obj;
    offset_end   = cam_id * n_obj + n_obj;
    float score = 1.0f;
    float mu = 0.0f;
    float last = 0.0f;
    // loop over all objects
    for (unsigned k = offset_start; (k < offset_end && score); k++) {
      unsigned obj_start = order_offset[k];
      unsigned obj_end = order_offset[k+1];
      for (unsigned i = obj_start; i < obj_end; i++) {
        mu += (float)index[order[i]];
      }
      mu = mu / (float)(obj_end - obj_start);
#if 0
      if(cam_id == 1 && k == (offset_start + 1)) {
        debug_output[0] = (float)cam_id;
        debug_output[1] = mu;
        debug_output[2] = last;
        debug_output[3] = (float)k;
        debug_output[4] = (float)offset_start;
        debug_output[5] = (float)offset_end;
        debug_output[6] = (float)obj_start;
        debug_output[7] = (float)obj_end;
      }
#endif
      if ( (last-254)*(last-254) < 1E-8 ) {
        score = 0.0f;
      }
      else if ( last < mu ) {
        last = mu;
        mu = 0.0f;
      }
      else
      {
        score = 0.0f;
      }
    }
    score_order[cam_id] = score; 
  }
}
