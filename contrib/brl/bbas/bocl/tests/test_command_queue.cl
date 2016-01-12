#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
__kernel
void
test_command_queue( __global float* input,
                    __global float* output,
                    __global int*   start)
{
  int gid = get_global_id(0)*get_global_size(0) + get_global_id(1);
  //int offset = (*start);

  //each global diddy processes 50 floats in the buffer
  for(int i=0; i<50; i++)
  {
    int index = 50*gid+i;
    float in = input[index];
    output[index] = in*in;
  }

}
