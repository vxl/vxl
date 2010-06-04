#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable

__kernel
void
test_single_thread_read_bandwidth(__constant uint * len, __global float* input_array,
                                  __global float* result_array, __global int * result_flag,
                                  __local float* local_mem)
{
  int gid=get_global_id(0);
  int lid=get_local_id(0);

  int worksize=get_local_size(0);
  float res=0.0;
  bool flag=true;
  if (lid==0)
  {
    for (int i=0;i<worksize;i++)
    {
      float temp=input_array[gid+i];
      flag=flag && temp==(float)i;
    }
    if (flag)
      result_array[gid]=worksize;
  }
}

__kernel
void
test_workgroup_uncoalesced_read_bandwidth(__constant uint * len, __global float* input_array,
                                          __global float* result_array, __global int * result_flag,
                                          __local float* local_mem)
{
  int gid=get_global_id(0);
  int lid=get_local_id(0);

  int worksize=get_local_size(0);
  int globalsize=get_global_size(0);

  bool flag=true;
  int index=(gid+lid*worksize)%globalsize;

  float temp=input_array[index];
  flag=flag && temp==(float)lid;
  if (lid==0)
  {
    if (flag)
      result_array[gid]=worksize;
  }
}

__kernel
void
test_workgroup_coalesced_read_bandwidth(__constant uint * len, __global float* input_array,
                                        __global float* result_array, __global int * result_flag,
                                        __local float* local_mem)
{
  int gid=get_global_id(0);
  int lid=get_local_id(0);
  int globalsize=get_global_size(0);
  int worksize=get_local_size(0);
  bool flag=true;
  int index=(gid+lid*worksize)%globalsize;
  float temp=input_array[gid];
  flag=flag && temp==(float)lid;
  if (lid==0)
  {
    if (flag)
      result_array[gid]=worksize;
  }
}

__kernel
void
test_single_thread_read_bandwidth_local_meory(__constant uint * len, __global float* input_array,
                                              __global float* result_array, __global int * result_flag,
                                              __local float* local_mem)
{
  int gid=get_global_id(0);
  int lid=get_local_id(0);

  int worksize=get_local_size(0);
  float res=0.0;
  bool flag=true;
  if (lid==0)
  {
    for (int i=0;i<worksize;i++)
    {
      local_mem[i]=input_array[gid+i];
      flag=flag && local_mem[i]==(float)i;
    }
    if (flag)
      result_array[gid]=worksize;
  }
}

__kernel
void
test_workgroup_uncoalesced_read_bandwidth_local_meory(__constant uint * len, __global float* input_array,
                                                      __global float* result_array, __global int * result_flag,
                                                      __local float* local_mem)
{
  int gid=get_global_id(0);
  int lid=get_local_id(0);

  int worksize=get_local_size(0);
  int globalsize=get_global_size(0);

  bool flag=true;
  int index=(gid+lid*worksize)%globalsize;

  local_mem[lid]=input_array[index];
  flag=flag && local_mem[lid]==(float)lid;
  if (lid==0)
  {
    if (flag)
      result_array[gid]=worksize;
  }
}
__kernel
void
test_workgroup_coalesced_read_bandwidth_local_memory(__constant uint * len, __global float* input_array,
                                                     __global float* result_array, __global int * result_flag,
                                                     __local float* local_mem)
{
  int gid=get_global_id(0);
  int lid=get_local_id(0);
  int globalsize=get_global_size(0);
  int worksize=get_local_size(0);
  bool flag=true;
  int index=(gid+lid*worksize)%globalsize;
  local_mem[lid]=input_array[gid];
  flag=flag && local_mem[lid]==(float)lid;
  if (lid==0)
  {
    if (flag)
      result_array[gid]=worksize;
  }
}
