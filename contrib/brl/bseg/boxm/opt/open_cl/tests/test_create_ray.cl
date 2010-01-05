__kernel 
void 
test_ray_create(__global float16 *svd_UtVW_,
                __global uint4 *imgcoords,
                __local float16 * Ut,
                __global float4  *origin,
                __local float4 * locorigin,
                __global float4* results)
{

  if(get_local_id(0) == 0)
  {
      Ut[0]=svd_UtVW_[0];  // conjugate transpose of U
      Ut[1]=svd_UtVW_[1];  // V
      Ut[2]=svd_UtVW_[2];  // Winv(first4) and ray_origin(last four)
      (*locorigin)=(*origin);
  }

  barrier(CLK_LOCAL_MEM_FENCE);

  unsigned gid = get_global_id(0);
  unsigned i   = gid/(*imgcoords).y;
  unsigned j   = gid-i*(*imgcoords).y;

  results[gid] = backproject(i,j,Ut[0],Ut[1],Ut[2],(*locorigin));
}

