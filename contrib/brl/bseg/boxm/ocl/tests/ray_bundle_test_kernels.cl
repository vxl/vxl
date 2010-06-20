#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
__kernel
void
test_load_data_using_loc_codes(__global int4* cells, __global float16* cell_data,
                               __global int4* results,
                               __local uchar*    ray_bundle_array,
                               __local int*    cell_ptrs,
                               __local short4*   cached_loc_codes,
                               __local float16*  cached_data)
{
  /* in this test the bundle must be 2x2 */
  int result_ptr = 0;

  if (get_local_id(0)==0 && get_local_id(1)==0)
    {
      cached_loc_codes[0]=(short4)(1,1,3,0); //cell_ptr 45
      cached_loc_codes[1]=(short4)(3,1,3,0); //54
      cached_loc_codes[2]=(short4)(1,3,3,0); //63
      cached_loc_codes[3]=(short4)(3,3,3,0); //72
    }
  barrier(CLK_LOCAL_MEM_FENCE);

  int ret= load_data_using_loc_codes(ray_bundle_array,cached_loc_codes);
  if (get_local_id(0)==0 && get_local_id(1)==0)
    {
      /* Check the load_data return value */
      results[result_ptr++]= (int4)ret;
      /* Check the ptr in ray bundle array*/
      for (uchar i = 0; i<4; ++i) {
        results[result_ptr++]=(int4)(ray_bundle_array[i]);
      }

      cached_loc_codes[0]=(short4)(1,1,3,0);
      cached_loc_codes[1]=(short4)(1,1,3,0);
      cached_loc_codes[2]=(short4)(1,1,3,0);
      cached_loc_codes[3]=(short4)(1,1,3,0);
    }
  barrier(CLK_LOCAL_MEM_FENCE);

  ret= load_data_using_loc_codes(ray_bundle_array,cached_loc_codes);
  if (get_local_id(0)==0 && get_local_id(1)==0)
    {
      results[result_ptr++]=(int4)ret;
      /* Check the data transfered to the local data cache */
      for (uchar i = 0; i<4; ++i) {
        results[result_ptr++]=(int4)ray_bundle_array[i];
      }
    }
  barrier(CLK_LOCAL_MEM_FENCE);
}

__kernel
void
test_load_data_mutable_using_loc_codes(__global int4* cells, __global float16* cell_data,
                                       __global int4* results,
                                       __local uchar4*    ray_bundle_array,
                                       __local int*    cell_ptrs,
                                       __local short4*   cached_loc_codes,
                                       __local float16*  cached_data)
{
  /* in this test the bundle must be 2x2 */
  int result_ptr = 0;

  if (get_local_id(0)==0 && get_local_id(1)==0)
    {
      cached_loc_codes[0]=(short4)(1,1,3,0); //cell_ptr 45
      cached_loc_codes[1]=(short4)(3,1,3,0); //54
      cached_loc_codes[2]=(short4)(1,3,3,0); //63
      cached_loc_codes[3]=(short4)(3,3,3,0); //72
    }
  barrier(CLK_LOCAL_MEM_FENCE);

  int ret= load_data_mutable_using_loc_codes(ray_bundle_array,cached_loc_codes);
  if (get_local_id(0)==0 && get_local_id(1)==0)
    {
      /* Check the load_data return value */
      results[result_ptr++]= (int4)ret;
      /* Check the ptr in ray bundle array*/
      results[result_ptr++] = convert_int4(ray_bundle_array[0]);
      results[result_ptr++] = convert_int4(ray_bundle_array[1]);
      results[result_ptr++] = convert_int4(ray_bundle_array[2]);
      results[result_ptr++] = convert_int4(ray_bundle_array[3]);

      cached_loc_codes[0]=(short4)(1,1,3,0);
      cached_loc_codes[1]=(short4)(1,1,3,0);
      cached_loc_codes[2]=(short4)(1,1,3,0);
      cached_loc_codes[3]=(short4)(1,1,3,0);
    }
  barrier(CLK_LOCAL_MEM_FENCE);

  ret= load_data_mutable_using_loc_codes(ray_bundle_array,cached_loc_codes);
  if (get_local_id(0)==0 && get_local_id(1)==0)
    {
      results[result_ptr++]=(int4)ret;
      /* Check the data transfered to the local data cache */
      results[result_ptr++] = convert_int4(ray_bundle_array[0]);
      results[result_ptr++] = convert_int4(ray_bundle_array[1]);
      results[result_ptr++] = convert_int4(ray_bundle_array[2]);
      results[result_ptr++] = convert_int4(ray_bundle_array[3]);
    }
  barrier(CLK_LOCAL_MEM_FENCE);
}

__kernel
void
test_map_work_space(__global int4* cells, __global float16* cell_data,
                    __global int4* results,
                    __local uchar*    ray_bundle_array,
                    __local float*    exit_points,
                    __local short4*   cached_loc_codes,
                    __local float16*  cached_data)
{
  int mapped_id0=0, mapped_id1=0;
  map_work_space_2d(&mapped_id0, &mapped_id1);
  int global_id0 = get_global_id(0), global_id1 = get_global_id(1);
  int gs0 = get_global_size(0);
  /* keep track of what thread executes by a global counter (results[0]) */
  //int offset = results[0].x;
  results[global_id1*gs0+global_id0] = (int4)(global_id0, global_id1, mapped_id0, mapped_id1);
  //results[0]=(int4)(offset+1,0,0,0);
}

__kernel
void
test_ray_entry_point(__global int4* cells,
                     __global float16* cell_data,
                     __global int4* results,
                     __global uint* n_levels_p,
                     __global float4* cam_center_g,
                     __global float16* cam_svd_g, /* cam pseudo inverse */
                     __global uint4* roi_g, /* image roi */
                     __global float4* bbox_g,/* bounding box,global coords*/
                     __local float16* cam_svd,
                     __local float4* cam_center,
                     __local float4* bbox,
                     __local uint4* roi,
                     __local uchar*    ray_bundle_array,
                     __local float*    exit_points,
                     __local short4*   cached_loc_codes,
                     __local float16*  cached_data)
{
  int local_bundle_index = get_local_id(0)+(get_local_size(0))*get_local_id(1);
  /* work item 0 moves data from global memory to local memory */
  if (local_bundle_index == 0) {
    cam_svd[0]=cam_svd_g[0];  // conjugate transpose of U
    cam_svd[1]=cam_svd_g[1];  // V
    cam_svd[2]=cam_svd_g[2];  // Winv(first4) and ray_origin(last four)
    *cam_center =cam_center_g[0];    // ray_origin
    *bbox=*bbox_g;
    *roi=*roi_g;
  }
  barrier(CLK_LOCAL_MEM_FENCE); /* wait for work item 0 to finish */
  uint grp_i = get_global_id(0), grp_j = get_global_id(1);
  float4 ray_o, ray_d;
  float4 cell_min, cell_max;
  uint n_levels = *n_levels_p;
  short4 root = (short4)(0,0,0,n_levels-1);
  cell_bounding_box(root, n_levels, &cell_min, &cell_max);

  int res =  ray_entry_point(cam_svd, cam_center, bbox, roi,
                             cell_min, cell_max, grp_i, grp_j,
                             local_bundle_index,
                             &ray_o, &ray_d, exit_points);
  if (res==0) {
    results[0]=(int4)(-1,-1,-1,-1);
    return;
  }
  int gs0 = get_global_size(0);
  int ptr = grp_i + gs0*grp_j;
  float p0 = 1000.0f*exit_points[3*local_bundle_index];
  float p1 = 1000.0f*exit_points[(3*local_bundle_index)+1];
  float p2 = 1000.0f*exit_points[(3*local_bundle_index)+2];
  float p3 = 1000.0f*local_bundle_index;
  float4 temp = (float4)(p0, p1, p2, p3);
  results[ptr] = convert_int4(temp);
}

__kernel
void
test_seg_len_obs(__global int4* cells, __global float16* cell_data,
                 __global int4* results,
                 __local uchar4*    ray_bundle_array,
                 __local int*    cell_ptrs,
                 __local short4*   cached_loc_codes,
                 __local float16*  cached_data,
                 __local float4*   cached_aux_data,
                 __local float4*   image_vect)
{
  /* in this test the bundle must be 2x2 */
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  /* clear aux data */
  cached_aux_data[llid]=(float4)0.0f;
  barrier(CLK_LOCAL_MEM_FENCE);

  int result_ptr = 0;
  if (llid==0)
    {
      /* setup example image array */
      image_vect[0].x=10.0f;
      image_vect[1].x=20.0f;
      image_vect[2].x= 30.0f;
      image_vect[3].x=40.0f;
#if 0 /* not used at present */
      cell_ptrs[0]=45;
      cell_ptrs[1]=54;
      cell_ptrs[2]=63;
      cell_ptrs[3]=72;
#endif

      cached_loc_codes[0]=(short4)(1,1,3,0); //cell_ptr 45
      cached_loc_codes[1]=(short4)(3,1,3,0); //54
      cached_loc_codes[2]=(short4)(1,3,3,0); //63
      cached_loc_codes[3]=(short4)(3,3,3,0); //72
    }

  barrier(CLK_LOCAL_MEM_FENCE);
  int ret = load_data_mutable_using_loc_codes(ray_bundle_array,cached_loc_codes);

  /* Check the load_data return value */
  results[result_ptr++]= (int4)ret;
  float seg_len = (float)(llid+1);
  cached_aux_data[llid]=(float4)0.0f;
  barrier(CLK_LOCAL_MEM_FENCE);

  seg_len_obs(seg_len, image_vect, ray_bundle_array, cached_aux_data);
  barrier(CLK_LOCAL_MEM_FENCE);
  if (llid==0)
    {
      /* test the resulting aux data */
      for(unsigned i = 0; i<4; ++i)
        results[result_ptr++] = convert_int4(cached_aux_data[i]);
      // check the case where all of the rays lie in the same cell
      /*      cell_ptrs[0]=45;cell_ptrs[1]=45;cell_ptrs[2]=45;cell_ptrs[3]=45;*/
      cached_loc_codes[0]=(short4)(1,1,3,0); //cell_ptr 45
      cached_loc_codes[1]=(short4)(1,1,3,0); 
      cached_loc_codes[2]=(short4)(1,1,3,0); 
      cached_loc_codes[3]=(short4)(1,1,3,0); 
    }

  barrier(CLK_LOCAL_MEM_FENCE);
  ret = load_data_mutable_using_loc_codes(ray_bundle_array,cached_loc_codes);
  seg_len_obs(seg_len, image_vect, ray_bundle_array, cached_aux_data);
  barrier(CLK_LOCAL_MEM_FENCE);
  if (llid==0)
    {
      results[result_ptr++]= (int4)ret;
      /* Check the aux_data return value */
      for(unsigned i = 0; i<4; ++i)
        results[result_ptr++] = convert_int4(cached_aux_data[i]);
    }
  barrier(CLK_LOCAL_MEM_FENCE);
}

__kernel
void
test_pre_infinity(__global int4* cells, __global float16* cell_data,
                  __global int4* results,
                  __local uchar4*    ray_bundle_array,
                  __local float*    exit_points,
                  __local short4*   cached_loc_codes,
                  __local float16*  cached_data,
                  __local float4*   cached_aux_data,
                  __local float4*   image_vect)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  cached_aux_data[llid]=(float4)0.0f;
  barrier(CLK_LOCAL_MEM_FENCE);
  int result_ptr = 0;
  /* in this test the bundle must be 2x2 */
  /* setup example image array */
  if(llid == 0){
    for (unsigned i = 0; i<4; ++i)
      image_vect[i]=(float4)0.0f;
    image_vect[0].x=0.1f;   image_vect[1].x=0.2f; image_vect[2].x= 0.5f;
    image_vect[3].x=0.7f;
    image_vect[0].z=1.0f;   image_vect[1].z=1.0f; image_vect[2].z= 1.0f;
    image_vect[3].z=1.0f;
    cached_loc_codes[0]=(short4)(1,1,3,0); //cell_ptr 45
    cached_loc_codes[1]=(short4)(3,1,3,0); //54
    cached_loc_codes[2]=(short4)(1,3,3,0); //63
    cached_loc_codes[3]=(short4)(3,3,3,0); //72
    /* setup alpha and the appearance model */
    cached_data[0].s0 = 0.693147f; /* exp = 0.5*/
    cached_data[0].s1 = 0.1f; /* mean */
    cached_data[0].s2 = 0.05f; /* sd */
    cached_data[0].s3 = 1.0f; /* weight */
    cached_data[0].s4 = 0.0f; /* Nobs0 */

    cached_data[1].s0 = 1.38629f; /* exp = 0.25*/
    cached_data[1].s1 = 0.2f; /* mean */
    cached_data[1].s2 = 0.05f; /* sd */
    cached_data[1].s3 = 1.0f; /* weight */
    cached_data[1].s4 = 0.0f; /* Nobs0 */

    cached_data[2].s0 = 0.223144f; /* exp = 0.8*/
    cached_data[2].s1 = 0.5f; /* mean */
    cached_data[2].s2 = 0.05f; /* sd */
    cached_data[2].s3 = 1.0f; /* weight */
    cached_data[2].s4 = 0.0f; /* Nobs0 */

    cached_data[3].s0 = 2.30259f; /* exp = 0.1*/
    cached_data[3].s1 = 0.7f; /* mean */
    cached_data[3].s2 = 0.05f; /* sd */
    cached_data[3].s3 = 1.0f; /* weight */
    cached_data[3].s4 = 0.0f; /* Nobs0 */
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  int ret = load_data_mutable_using_loc_codes(ray_bundle_array,cached_loc_codes);
  if(llid==0){
  results[result_ptr++]= (int4)ret;
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  float seg_len = 1.0f;
  seg_len_obs(seg_len, image_vect, ray_bundle_array, cached_aux_data);
  pre_infinity(seg_len, image_vect, ray_bundle_array, cached_data, cached_aux_data);
  barrier(CLK_LOCAL_MEM_FENCE);

  if(llid == 0){
    for (unsigned i = 0; i<4; ++i)
      results[result_ptr++] = convert_int4(1000.0f*cached_aux_data[i]);
    for (unsigned i = 0; i<4; ++i)
      results[result_ptr++] = convert_int4(1000.0f*image_vect[i]);
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  /* test where all rays lie in the same cell */
  if(llid == 0){
    for (unsigned i = 0; i<4; ++i)
      image_vect[i]=(float4)0.0f;
    image_vect[0].x=0.5f;   image_vect[1].x=0.6f; image_vect[2].x= 0.4f;
    image_vect[3].x=0.55f;
    image_vect[0].z=1.0f;   image_vect[1].z=1.0f; image_vect[2].z= 1.0f;
    image_vect[3].z=1.0f;
    cached_loc_codes[0]=(short4)(1,1,3,0); //cell_ptr 45
    cached_loc_codes[1]=(short4)(1,1,3,0); 
    cached_loc_codes[2]=(short4)(1,1,3,0); 
    cached_loc_codes[3]=(short4)(1,1,3,0); 
    /* revise the cached data for cell 0 */
    cached_data[0].s0 = 0.693147f; /* exp = 0.5*/
    cached_data[0].s1 = 0.4f; /* mean0 */
    cached_data[0].s2 = 0.1f; /* sd0 */
    cached_data[0].s3 = 0.333f; /* weight0 */
    cached_data[0].s4 = 0.0f; /* Nobs0 */
    cached_data[0].s5 = 0.5f; /* mean1 */
    cached_data[0].s6 = 0.1f; /* sd1 */
    cached_data[0].s7 = 0.333f; /* weight1 */
    cached_data[0].s8 = 0.0f; /* Nobs1 */
    cached_data[0].s9 = 0.6f; /* mean2 */
    cached_data[0].sa = 0.1f; /* sd2 */
    cached_data[0].sb = 0.0f; /* Nobs2 */
    cached_data[0].sc = 0.0f; /* Nobs_mix */
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  cached_aux_data[llid]=(float4)0.0f;
  ret = load_data_mutable_using_loc_codes(ray_bundle_array,cached_loc_codes);
  barrier(CLK_LOCAL_MEM_FENCE);
  float len = 0.25f*(float)(llid +1);/*vary the seg_len to reveal bugs*/
  seg_len_obs(len, image_vect, ray_bundle_array, cached_aux_data);
  pre_infinity(len, image_vect, ray_bundle_array, cached_data,cached_aux_data);
  if(llid == 0){
    results[result_ptr++]= (int4)ret;
    for (unsigned i = 0; i<4; ++i)
      results[result_ptr++] = convert_int4(1000.0f*cached_aux_data[i]);
    for (unsigned i = 0; i<4; ++i)
      results[result_ptr++] = convert_int4(1000.0f*image_vect[i]);
  }
  barrier(CLK_LOCAL_MEM_FENCE);
}
#if 0
__kernel
void
test_bayes_ratio(__global int4* cells, __global float16* cell_data,
                 __global int4* results,
                 __local uchar4*    ray_bundle_array,
                 __local float*    exit_points,
                 __local short4*   cached_loc_codes,
                 __local float16*  cached_data,
                 __local float4*   image_vect)
{
  short n_levels = 3;
  int result_ptr = 0;
  /* in this test the bundle must be 2x2 */
  /* setup example image array */
  for (unsigned i = 0; i<4; ++i)
    image_vect[i]=(float4)0.0f;
  image_vect[0].x=0.1f;   image_vect[1].x=0.2f; image_vect[2].x= 0.5f;
  image_vect[3].x=0.7f;
  image_vect[0].z=1.0f;   image_vect[1].z=1.0f; image_vect[2].z= 1.0f;
  image_vect[3].z=1.0f;
  /* exit points that access individual cells */
  exit_points[0]=0.25f;  exit_points[1]= 0.25f;  exit_points[2]= 1.0f;
  exit_points[3]=0.75f;  exit_points[4]= 0.25f;  exit_points[5]= 1.0f;
  exit_points[6]=0.25f;  exit_points[7]= 0.75f;  exit_points[8]= 1.0f;
  exit_points[9]=0.75f;  exit_points[10]=0.75f;  exit_points[11]=1.0f;
  barrier(CLK_LOCAL_MEM_FENCE);
  int ret = load_data_mutable(cells, cell_data,n_levels, ray_bundle_array,
                              exit_points,cached_loc_codes,cached_data);
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  float seg_len = 1.0f;
  seg_len_obs(seg_len, image_vect, ray_bundle_array, cached_data);
  /* revise the cached data for this test */
  cached_data[0].s0 = 0.693147f; /* exp = 0.5*/
  cached_data[0].s1 = 1.0f; /* one component */
  cached_data[0].s3 = 0.1f; /* mean */
  cached_data[0].s4 = 0.05f; /* sd */
  cached_data[0].s5 = 1.0f; /* weight */

  cached_data[1].s0 = 1.38629f; /* exp = 0.25*/
  cached_data[1].s1 = 1.0f; /* one component */
  cached_data[1].s3 = 0.2f; /* mean */
  cached_data[1].s4 = 0.05f; /* sd */
  cached_data[1].s5 = 1.0f; /* weight */

  cached_data[2].s0 = 0.223144f; /* exp = 0.8*/
  cached_data[2].s1 = 1.0f; /* one component */
  cached_data[2].s3 = 0.5f; /* mean */
  cached_data[2].s4 = 0.05f; /* sd */
  cached_data[2].s5 = 1.0f; /* weight */

  cached_data[3].s0 = 2.30259f; /* exp = 0.1*/
  cached_data[3].s1 = 1.0f; /* one component */
  cached_data[3].s3 = 0.7f; /* mean */
  cached_data[3].s4 = 0.05f; /* sd */
  cached_data[3].s5 = 1.0f; /* weight */
  pre_infinity(seg_len, image_vect, ray_bundle_array, cached_data);

  /* set up norm image - initially 1 */
  for (unsigned i = 0; i<4; ++i)
    image_vect[i]=(float4)(1.0f,0.0f,1.0f,0.0f);
  bayes_ratio(seg_len, image_vect, ray_bundle_array, cached_data);

  for (unsigned i = 0; i<4; ++i)
    results[result_ptr++] = convert_int4(1000.0f*image_vect[i]);

  results[result_ptr].x = (int)(1000.0f*cached_data[0].s2);/*seg_len sum*/
  results[result_ptr].y = (int)(1000.0f*cached_data[0].sc);/*obs sum */
  results[result_ptr].z = (int)(1000.0f*cached_data[0].sd);/*Bayes ratio */
  results[result_ptr++].w = (int)(1000.0f*cached_data[0].se);/*weighted vis*/

  results[result_ptr].x = (int)(1000.0f*cached_data[1].s2);
  results[result_ptr].y = (int)(1000.0f*cached_data[1].sc);
  results[result_ptr].z = (int)(1000.0f*cached_data[1].sd);
  results[result_ptr++].w = (int)(1000.0f*cached_data[1].se);

  results[result_ptr].x = (int)(1000.0f*cached_data[2].s2);
  results[result_ptr].y = (int)(1000.0f*cached_data[2].sc);
  results[result_ptr].z = (int)(1000.0f*cached_data[2].sd);
  results[result_ptr++].w = (int)(1000.0f*cached_data[2].se);

  results[result_ptr].x = (int)(1000.0f*cached_data[3].s2);
  results[result_ptr].y = (int)(1000.0f*cached_data[3].sc);
  results[result_ptr].z = (int)(1000.0f*cached_data[3].sd);
  results[result_ptr++].w = (int)(1000.0f*cached_data[3].se);

  /* test where all rays lie in the same cell */
  exit_points[0]=0.25f;  exit_points[1]= 0.25f;  exit_points[2]= 1.0f;
  exit_points[3]=0.251f;  exit_points[4]= 0.252f;  exit_points[5]= 1.0f;
  exit_points[6]=0.253f;  exit_points[7]= 0.254f;  exit_points[8]= 1.0f;
  exit_points[9]=0.255f;  exit_points[10]=0.256f;  exit_points[11]=1.0f;
  barrier(CLK_LOCAL_MEM_FENCE);
  ret = load_data_mutable(cells, cell_data, n_levels, ray_bundle_array,
                          exit_points,cached_loc_codes,cached_data);

  for (unsigned i = 0; i<4; ++i)
    image_vect[i]=(float4)0.0f;
  image_vect[0].x=0.5f;   image_vect[1].x=0.6f; image_vect[2].x= 0.4f;
  image_vect[3].x=0.55f;
  image_vect[0].z=1.0f;   image_vect[1].z=1.0f; image_vect[2].z= 1.0f;
  image_vect[3].z=1.0f;

  seg_len_obs(seg_len, image_vect, ray_bundle_array, cached_data);
  /* revise the cached data for cell 0 */
  cached_data[0].s0 = 0.693147f; /* exp = 0.5*/
  cached_data[0].s1 = 3.0f; /* three components */
  cached_data[0].s3 = 0.4f; /* mean0 */
  cached_data[0].s4 = 0.1f; /* sd0 */
  cached_data[0].s5 = 0.333f; /* weight0 */
  cached_data[0].s6 = 0.5f; /* mean1 */
  cached_data[0].s7 = 0.1f; /* sd1 */
  cached_data[0].s8 = 0.333f; /* weight1 */
  cached_data[0].s9 = 0.6f; /* mean3 */
  cached_data[0].sa = 0.1f; /* sd3 */
  cached_data[0].sb = 0.333f; /* weight3 */
  pre_infinity(seg_len, image_vect, ray_bundle_array, cached_data);

  /* vary norm image to detect bugs */
  for (unsigned i = 0; i<4; ++i)
    image_vect[i]=(float4)(0.25f*((float)(i+1)),0.0f,1.0f,0.0f);

  bayes_ratio(seg_len, image_vect, ray_bundle_array, cached_data);

  for (unsigned i = 0; i<4; ++i)
    results[result_ptr++] = convert_int4(1000.0f*image_vect[i]);

  results[result_ptr].x = (int)(1000.0f*cached_data[0].s2);
  results[result_ptr].y = (int)(1000.0f*cached_data[0].sc);
  results[result_ptr].z = (int)(1000.0f*cached_data[0].sd);
  results[result_ptr].w = (int)(1000.0f*cached_data[0].se);
}
#endif // 0
