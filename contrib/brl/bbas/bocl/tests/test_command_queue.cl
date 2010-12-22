#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
__kernel
void
test_command_queue( __global float* buffer)
{
  int gid = get_global_id(0)*get_global_size(0) + get_global_id(1);
  //each global diddy processes 100 floats in the buffer
  for(int i=0; i<50; i++) 
  {
    buffer[50*gid+i] = i; 
  }

}
