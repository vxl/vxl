#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
const sampler_t RowSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP ;

__kernel
void
test_single_thread_read_bandwidth(__global uint * len, __global float4* input_array, 
								  __global float* result_array, __global int * result_flag, 
								  __local float* local_mem)
{

	int gid=get_global_id(0);
	int lid=get_local_id(0);

	int worksize=get_local_size(0);
	bool flag=true;
	float4 temp;
	if(lid==0)
	{
		for(int i=0;i<worksize;i++)
		{
			temp=input_array[gid+i];
			flag=flag && temp.x==(float)i &&  temp.y==0.0 && temp.z==0.0 && temp.w==0;
		}
		if(flag)
			result_array[gid]=(float)worksize;
	}
}


__kernel
void
test_single_thread_read_bandwidth_image(__constant uint * len, __read_only image2d_t input_array, 
									    __global float* result_array, __global int * result_flag, 
									    __local float* local_mem)
{

	int gid=get_global_id(0);
	int lid=get_local_id(0);
	uint w=get_image_width(input_array);
	int worksize=get_local_size(0);
	bool flag=true;
	float4 temp;
	if(lid==0)
	{
		int2 pos;
		for(int i=0;i<worksize;i++)
		{
			pos.y=(gid+i)/w;
			pos.x=(gid+i)%w;
			temp=read_imagef(input_array,RowSampler,pos);
			flag=flag && temp.x==(float)i &&  temp.y==0.0 && temp.z==0.0 && temp.w==0;

		}
		if(flag)
			result_array[gid]=(float)worksize;
	}
}
__kernel
void
test_workgroup_coalesced_read_bandwidth_image(__constant uint * len, __read_only image2d_t input_array,
											  __global float* result_array, __global int * result_flag, 
											  __local float* local_mem)
{
	int gid=get_global_id(0);
	int lid=get_local_id(0);

	int worksize=get_local_size(0);
	int globalsize=get_global_size(0);
	bool flag=true;
	uint w=get_image_width(input_array);
	int2 pos;
	pos.y=gid/w;
	pos.x=gid%w;
	
	float4 temp;
	temp=read_imagef(input_array,RowSampler,pos);
	flag=flag && temp.x==(float)lid && temp.y==0.0 && temp.z==0.0 && temp.w==0;
	if(lid==0)
	{
		if(flag)
			result_array[gid]=worksize;
	}
}
__kernel
void
test_workgroup_uncoalesced_read_bandwidth(__constant uint * len, __global float4* input_array,
                                          __global float* result_array, __global int * result_flag,
                                          __local float* local_mem)
{
	int gid=get_global_id(0);
	int lid=get_local_id(0);

	int worksize=get_local_size(0);
	int globalsize=get_global_size(0);

	bool flag=true;

	int index=(gid+lid*worksize)%globalsize;
	float4 temp=input_array[index];
	flag=flag && temp.x==(float)lid && temp.y==0.0 && temp.z==0.0 && temp.w==0;
	if(lid==0)
	{
		if(flag)
			result_array[gid]=worksize;
	}
}

__kernel
void
test_workgroup_coalesced_read_bandwidth(__constant uint * len, __global float4* input_array,
                                        __global float* result_array, __global int * result_flag,
                                        __local float* local_mem)
{
	int gid=get_global_id(0);
	int lid=get_local_id(0);
	int globalsize=get_global_size(0);
	int worksize=get_local_size(0);
	bool flag=true;
	float4 temp=input_array[gid];
	flag=flag && temp.x==(float)lid && temp.y==0.0 && temp.z==0.0 && temp.w==0.0;
	if(lid==0)
	{
		if(flag)
			result_array[gid]=worksize;
	}
}

__kernel
void
test_single_thread_read_bandwidth_local_meory(__constant uint * len, __global float4* input_array, 
											  __global float* result_array, __global int * result_flag, 
											  __local float* local_mem)
{

	int gid=get_global_id(0);
	int lid=get_local_id(0);

	int worksize=get_local_size(0);
	float res=0.0;
	bool flag=true;
	if(lid==0)
	{
		for(int i=0;i<worksize;i++)
		{	
			float4 temp=input_array[gid+i];
			local_mem[lid]=temp.x;
			flag=flag && local_mem[i]==(float)i && temp.y==0.0 && temp.z==0.0 && temp.w==0;
		}
		if(flag)
			result_array[gid]=worksize;
	}
}

__kernel
void
test_workgroup_uncoalesced_read_bandwidth_local_meory(__constant uint * len, __global float4* input_array,
                                                      __global float* result_array, __global int * result_flag,
                                                      __local float* local_mem)
{
  int gid=get_global_id(0);
  int lid=get_local_id(0);

  int worksize=get_local_size(0);
  int globalsize=get_global_size(0);

  bool flag=true;
  int index=(gid+lid*worksize)%globalsize;

	float4 temp=input_array[index];
	local_mem[lid]=temp.x;
	flag=flag && local_mem[lid]==(float)lid && temp.y==0.0 && temp.z==0.0 && temp.w==0;
	if(lid==0)
	{
		if(flag)
			result_array[gid]=worksize;
	}
}
__kernel
void
test_workgroup_coalesced_read_bandwidth_local_memory(__constant uint * len, __global float4* input_array,
                                                     __global float* result_array, __global int * result_flag,
                                                     __local float* local_mem)
{

	int gid=get_global_id(0);
	int lid=get_local_id(0);
	int globalsize=get_global_size(0);
	int worksize=get_local_size(0);
	bool flag=true;

	float4 temp=input_array[gid];
	local_mem[lid]=temp.x;
	flag=flag && local_mem[lid]==(float)lid && temp.y==0.0 && temp.z==0.0 && temp.w==0;
	if(lid==0)
	{
		if(flag)
			result_array[gid]=worksize;
	}
}
