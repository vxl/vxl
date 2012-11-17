//#pragma opencl extension cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
# if NVIDIA
  #pragma OPENCL EXTENSION cl_khr_go_sharing : enable
#endif

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
  unsigned char ql = query_layer[idx];
  unsigned char il = ind_layer[j];
  unsigned score = 0;
  
  if(ql == 254 && ql == il) {  // sky
    score = 1;
  }else if(ql <= il && il != 254){
    score = 1;
  }
  score_obj[idx] = score;

  //if(query_layer[idx] <= ind_layer[j]) {  // sky
  //  score_obj[idx] = 1.0f;
  //}

//  if( i == 125 && j == 9 ) {
//	  debug_output[0] = (float)i;
//	  debug_output[1] = (float)j;
//	  debug_output[2] = (float)idx;
//	  debug_output[3] = (float)query_layer[idx];
//	  debug_output[4] = (float)ind_layer[j];
//	  debug_output[5] = (float)score_obj[idx];
// }

}