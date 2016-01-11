__kernel
void
test_multiply( __global int* input,
               __global int* output)
{
  int gid = get_global_id(0)*get_global_size(0) + get_global_id(1);
  int in    = input[gid];
  output[gid] = in * in;
}
