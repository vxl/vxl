__kernel
void
test_local_mem_access( __global int* input,
                       __global int* output)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar localI = (uchar)get_local_id(0);
  uchar localJ = (uchar)get_local_id(1);

  //single local value per workspace
  __local float single[1];
  single[0] = 0.0f;
  barrier(CLK_LOCAL_MEM_FENCE);

  //only allow thread 0 to modify the local memory
  float currT = 0.0f;
  if(llid==0) {
    currT = 10.0f;
    single[0] = currT;
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  //mem_fence(CLK_LOCAL_MEM_FENCE);

  //now have all threads read in from local memory
  currT = single[0];
  barrier(CLK_LOCAL_MEM_FENCE);

  //output in global memory
  if(get_group_id(0)==0 && get_group_id(1)==0)
    output[llid] = currT;
}
