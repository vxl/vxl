__kernel 
void
test_multiply( __global float* input, 
               __global float* output)
{
  int gid = get_global_id(0)*get_global_size(0) + get_global_id(1);
  float in    = input[gid]; 
  output[gid] = in * in; 
}
