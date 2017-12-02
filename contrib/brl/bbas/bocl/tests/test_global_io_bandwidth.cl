#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
__kernel
void
test_locking_mechanism( __global  uint *  len,
                        __global  float4* input_array,
                        __global  float*  result_array,
                        __global  int *   result_flag,
                        __local   float*  local_mem)
{
  int gid   = get_global_id(0);
  int llid  = get_local_id(0);
  int lsize = get_local_size(0);
  bool flag=true;
  int cnt=0;
  int cnt2=0;
  int isactive=1;
  float loc_total = 0;

  //atom_add(result_flag, convert_int(input_array[gid].x));

  //locally add up the array
  if(llid == 0) {
    for(int i=0; i<lsize; i++) {
      loc_total += input_array[gid+i].x;
    }
  }

  if(llid == 0) {
    while(true)
    {
      if(atom_cmpxchg(result_flag,0,1)==0)
      {
        float buff = result_array[0];
        loc_total += buff;
        result_array[0] = loc_total;
        atom_xchg(result_flag,0);
        break;
      }
    }
  }


/*
  if(llid == 0) {
    while(atom_cmpxchg(result_flag, 0, 1) == 0);
    result_array[0] += loc_total;
    atom_xchg(result_flag,0);
  }
  barrier(CLK_GLOBAL_MEM_FENCE);

  if(llid == 0) {
    while(atom_cmpxchg(result_flag, 0, 1) == 0);
    result_array[0] += loc_total;
    atom_xchg(result_flag,0);
  }
  barrier(CLK_GLOBAL_MEM_FENCE);
*/


/*
  result_array[]


  if(llid == 0) {

    while(true && cnt < 2)
    {
      if(atom_cmpxchg(&lock_array[data_ptr],0,1)==0)
      {
            float2 cl_beta  = cum_len_beta[data_ptr];
            float2 mean_vis = convert_float2(mean_obs_cum_vis[data_ptr])/255.0f;
            cached_aux_data[llid].x=cached_aux_data[llid].x+cl_beta.x;
            if(cached_aux_data[llid].x>1e-10f)
            {
                mean_vis.x=(mean_vis.x*cl_beta.x+cached_aux_data[llid].y)/(cached_aux_data[llid].x);
                mean_vis.y=(mean_vis.y*cl_beta.x+cached_aux_data[llid].z)/(cached_aux_data[llid].x);
            }
            cum_len_beta[data_ptr] = (float2) (cached_aux_data[llid].x, 0.0f);
            mean_obs_cum_vis[data_ptr] = convert_uchar2_sat_rte(mean_vis*255.0f);

            //DEBUG ASSIGNMENT
            alpha_array[data_ptr] = cached_aux_data[llid].x;

            atom_xchg(&lock_array[data_ptr],0);
            cnt=2000;
        }
        //if(cnt=-1)
        //    break;
      }
      cnt++;

  }

*/


/*
  while(isactive==1)
  {
      if(llid>0)
          isactive=0;

      if((*result_flag)==0 && isactive )
      {
          int old=atom_cmpxchg(result_flag,0,gid+1);
          barrier(CLK_GLOBAL_MEM_FENCE);

          if(old==0)
          {
              cnt2++;
              float temp=result_array[0];
              temp+=(gid+1);
              result_array[0]=temp;
              //atom_xchg(result_flag,0);
              result_flag[0]=0;
              isactive=0;
          }
          barrier(CLK_GLOBAL_MEM_FENCE);
      }
      cnt=cnt+1;
  }
  result_array[gid+1]=cnt2;
*/



}
__kernel
void
test_atom_cmpxchg(__global uint * len, __global float4* input_array,
                                  __global int* result_array,
                                  __global int * result_flag,
                                  __local float* local_mem)
{
  int gid=get_global_id(0);

  int lid=get_local_id(0);
  int globalsize=get_global_size(0);
  int worksize=get_local_size(0);

  int groupid=get_group_id(0);
  bool flag=true;

  int cmp=as_int(444.0f);//convert_int(input_array[gid].x);
  //result_array[gid]=cmp;
  barrier(CLK_LOCAL_MEM_FENCE);



  int temp=atom_cmpxchg(&result_array[0],cmp,0);

  if(temp==cmp)
    result_array[gid]=10000;
  else
    result_array[gid]= 555;
}
__kernel
void
test_single_thread_read_bandwidth(__global uint * len, __global float4* input_array,
                                  __global float* result_array, __global int * result_flag,
                                  __local float* local_mem)
{
  int gid=get_global_id(0);

  int lid=get_local_id(0);
  int globalsize=get_global_size(0);
  int worksize=get_local_size(0);
  bool flag=true;
  float4 temp;
  if (lid==0)
  {
    for (int i=0;i<worksize;i++)
    {
      temp=input_array[gid+i];
      flag=flag && temp.x==(float)i &&  temp.y==0.0 && temp.z==0.0 && temp.w==0.0;
    }
    if (flag)
      result_array[gid]=(float)worksize;
  }
}


__kernel
void
test_workgroup_uncoalesced_read_bandwidth(__constant uint * len, __global float4* input_array,
                                          __global float* result_array, __global int * result_flag,
                                          __local float4* local_mem,__local uint* localid)
{
  int gid=get_global_id(0);
  int lid=get_local_id(0);
  int worksize=get_local_size(0);
  int globalsize=get_global_size(0);

  int index=(gid+lid*worksize)%globalsize;
  float4 temp=input_array[index];
  bool flag=temp.x==(float)lid && temp.y==0.0 && temp.z==0.0 && temp.w==0;
  if (lid==0)
  {
    if (flag)
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

  float4 temp=input_array[gid];
  bool flag=temp.x==(float)lid && temp.y==0.0 && temp.z==0.0 && temp.w==0.0;
  if (lid==0)
  {
    if (flag)
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
  if (lid==0)
  {
    for (int i=0;i<worksize;i++)
    {
      float4 temp=input_array[gid+i];
      local_mem[lid]=temp.x;
      flag=flag && local_mem[lid]==(float)i && temp.y==0.0 && temp.z==0.0 && temp.w==0;
    }
    if (flag)
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

  int index=(gid+lid*worksize)%globalsize;
  float4 temp=input_array[index];
  local_mem[lid]=temp.x;
  bool flag=local_mem[lid]==(float)lid && temp.y==0.0 && temp.z==0.0 && temp.w==0;
  if (lid==0)
  {
    if (flag)
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

  float4 temp=input_array[gid];
  local_mem[lid]=temp.x;
  bool flag=local_mem[lid]==(float)lid && temp.y==0.0 && temp.z==0.0 && temp.w==0;
  if (lid==0)
  {
    if (flag)
      result_array[gid]=worksize;
  }
}
__kernel
void
test_workgroup_prefetch_bandwidth_local_memory(__constant uint * len, __global float4* input_array,
                                                     __global float* result_array, __global int * result_flag,
                                                     __local float4* local_mem, __local uint* firstthreadid)
{
  int gid=get_global_id(0);
  int lid=get_local_id(0);
  int globalsize=get_global_size(0);
  int worksize=get_local_size(0);

  if(lid==0)
  {
      (*firstthreadid)=gid;
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  event_t eventid = (event_t)0;
  event_t e = async_work_group_copy(local_mem, input_array+gid, (size_t)worksize, eventid);
  wait_group_events (1, &eventid);


  bool flag=local_mem[lid].x==(float)lid && local_mem[lid].y==0.0 && local_mem[lid].z==0.0 && local_mem[lid].w==0;
  if (lid==0)
  {
    if (flag)
      result_array[gid]=worksize;
  }
}

#ifdef USEIMAGE
const sampler_t RowSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP ;

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
  if (lid==0)
  {
    int2 pos;
    for (int i=0;i<worksize;i++)
    {
      pos.y=(gid+i)/w;
      pos.x=(gid+i)%w;
      temp=read_imagef(input_array,RowSampler,pos);
      flag=flag && temp.x==(float)i &&  temp.y==0.0 && temp.z==0.0 && temp.w==0;
    }
    if (flag)
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

  uint w=get_image_width(input_array);
  int2 pos;
  pos.y=gid/w;
  pos.x=gid%w;

  float4 temp;
  temp=read_imagef(input_array,RowSampler,pos);
  bool flag=temp.x==(float)lid && temp.y==0.0 && temp.z==0.0 && temp.w==0;
  if (lid==0)
  {
    if (flag)
      result_array[gid]=worksize;
  }
}

#endif
