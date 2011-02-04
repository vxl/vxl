#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable

float min_val(unsigned int lid,  unsigned int localSize, __local float*sdata)
{
  for (unsigned int s=localSize/2; s>0; s>>=1) {
    if (lid < s) {
	  if (sdata[lid] > sdata[lid+s])
	    sdata[lid]=sdata[lid+s];
	  }
    barrier(CLK_LOCAL_MEM_FENCE);
  }
  return sdata[0];
}
 
__kernel void 
reduce( __global float* input,/*output minfo array */
        __global int* length,
        __global uint* output, /*to hold min values*/      
        __local float* sdata
       )
{
  unsigned int lid = get_local_id(0);
  unsigned int bid = get_group_id(0);
  unsigned int gid = get_global_id(0);
  unsigned int globalSize = get_global_size(0);
  unsigned int localSize = get_local_size(0);
  
  // do sequential min for arrays bigger than work groups
  float minp=INFINITY;
  unsigned int global_index = gid;
  while (global_index < *length) {
    float element = input[global_index];
    minp = (minp < element) ? minp : element;
    global_index += globalSize;
  }
  
  // perform parallel reduction
  sdata[lid] = minp;
  barrier(CLK_LOCAL_MEM_FENCE);
  
  float val = min_val(lid, localSize, sdata);
  if (lid==0) {
    output[bid]=val*10000;
    barrier(CLK_LOCAL_MEM_FENCE);
    atom_min(output, output[bid]);
  }
}
