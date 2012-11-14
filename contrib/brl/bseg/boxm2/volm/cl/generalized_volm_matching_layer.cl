#pragma opencl extension cl_khr_local_int32_base_atomics: enable
#pragma opencl extension cl_khr_global_int32_base_atomics: enable

__kernel
void generalized_volm_matching_layer (__global unsigned char* query_layer,
                                      __global unsigned char* ind_layer,
                                      __global unsigned char* score_obj,
                                      __global         float* debug_output)
{
  unsigned int i, j;
  // i == camera label, j == layer voxel label
  i = get_global_id(0);
  j = get_global_id(1);
  // (i,j) ---> index in query_layer for voxel j in camera i
  unsigned int idx = i*get_global_size(1) + j;

  if (query_layer[idx] == 254 && query_layer[idx] == ind_layer[j]) {  // sky
    score_obj[idx] = 1;
  }
  else if (query_layer[idx] <= ind_layer[j]){
    score_obj[idx] = 1;
  }

#if 0
  if (query_layer[idx] <= ind_layer[j]) {  // sky
    score_obj[idx] = 1.0f;
  }

  if ( i == 125 && j == 9 ) {
    debug_output[0] = (float)i;
    debug_output[1] = (float)j;
    debug_output[2] = (float)idx;
    debug_output[3] = (float)query_layer[idx];
    debug_output[4] = (float)ind_layer[j];
    debug_output[5] = (float)score_obj[idx];
  }
#endif // 0
}
