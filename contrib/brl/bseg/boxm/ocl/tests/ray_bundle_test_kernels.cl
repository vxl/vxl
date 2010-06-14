#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
__kernel
void
test_load_data(__global int4* cells, __global float16* cell_data,
               __global int4* results,
               __local uchar*    ray_bundle_array,
               __local float*    exit_points,
               __local short4*   cached_loc_codes,
               __local float16*  cached_data)
{
  /* in this test the bundle must be 2x2 */
  short n_levels = 3;
  exit_points[0]=0.25f;  exit_points[1]= 0.25f;  exit_points[2]= 1.0f;
  exit_points[3]=0.75f;  exit_points[4]= 0.25f;  exit_points[5]= 1.0f;
  exit_points[6]=0.25f;  exit_points[7]= 0.75f;  exit_points[8]= 1.0f;
  exit_points[9]=0.75f;  exit_points[10]=0.75f;  exit_points[11]=1.0f;
  barrier(CLK_LOCAL_MEM_FENCE); 
  int ret = load_data(cells, cell_data,n_levels, ray_bundle_array,
                      exit_points,cached_loc_codes,cached_data);
  int result_ptr = 0;
  /* Check the load_data return value */
  results[result_ptr++]= (int4)ret;
  /* Check the loc_codes */
  for (uchar i = 0; i<4; ++i) {
    results[result_ptr++]=convert_int4(cached_loc_codes[i]);
  }

  /* Check the data transfered to the local data cache */
  for(uchar i = 0; i<4; ++i){
    float16 temp = cached_data[i];
    results[result_ptr++]=(int4)temp.s0;
  }
  /* Check the ray_bundle_array */
  results[result_ptr].x = ray_bundle_array[0];
  results[result_ptr].y = ray_bundle_array[1];
  results[result_ptr].z = ray_bundle_array[2];
  results[result_ptr++].w = ray_bundle_array[3];
  /* Test where all rays lie in the same cell */
  exit_points[0]=0.25f;  exit_points[1]= 0.25f;  exit_points[2]= 1.0f; 
  exit_points[3]=0.251f;  exit_points[4]= 0.252f;  exit_points[5]= 1.0f; 
  exit_points[6]=0.253f;  exit_points[7]= 0.254f;  exit_points[8]= 1.0f; 
  exit_points[9]=0.255f;  exit_points[10]=0.256f;  exit_points[11]=1.0f; 
  barrier(CLK_LOCAL_MEM_FENCE); 
  ret = load_data(cells, cell_data, n_levels, ray_bundle_array,
                  exit_points,cached_loc_codes,cached_data);

  /* Check the load_data return value */
  results[result_ptr++]= (int4)ret;
 /* Check the loc_codes */

  results[result_ptr++]=convert_int4(cached_loc_codes[0]);

  /* Check the data transfered to the local data cache */
    float16 temp = cached_data[0];
    results[result_ptr++]=(int4)temp.s0;

  /* Check the ray_bundle_array */
  results[result_ptr].x = ray_bundle_array[0];
  results[result_ptr].y = ray_bundle_array[1];
  results[result_ptr].z = ray_bundle_array[2];
  results[result_ptr].w = ray_bundle_array[3];
}
__kernel
void
test_load_data_mutable(__global int4* cells, __global float16* cell_data,
                       __global int4* results,
                       __local uchar4*    ray_bundle_array,
                       __local float*    exit_points,
                       __local short4*   cached_loc_codes,
                       __local float16*  cached_data)
{
  /* in this test the bundle must be 2x2 */
  short n_levels = 3;
  exit_points[0]=0.25f;  exit_points[1]= 0.25f;  exit_points[2]= 1.0f;
  exit_points[3]=0.75f;  exit_points[4]= 0.25f;  exit_points[5]= 1.0f;
  exit_points[6]=0.25f;  exit_points[7]= 0.75f;  exit_points[8]= 1.0f;
  exit_points[9]=0.75f;  exit_points[10]=0.75f;  exit_points[11]=1.0f;
  barrier(CLK_LOCAL_MEM_FENCE); 
  int ret = load_data_mutable(cells, cell_data,n_levels, ray_bundle_array,
                              exit_points,cached_loc_codes,cached_data);
  int result_ptr = 0;
  /* Check the load_data return value */
  results[result_ptr++]= (int4)ret;
  /* Check the loc_codes */
  for (uchar i = 0; i<4; ++i) {
    results[result_ptr++]=convert_int4(cached_loc_codes[i]);
  }

  /* Check the data transfered to the local data cache */
  for(uchar i = 0; i<4; ++i){
    float16 temp = cached_data[i];
    results[result_ptr++]=(int4)temp.s0;
  }
  /* Check the ray_bundle_array */
  results[result_ptr++] = convert_int4(ray_bundle_array[0]);
  results[result_ptr++] = convert_int4(ray_bundle_array[1]);
  results[result_ptr++] = convert_int4(ray_bundle_array[2]);
  results[result_ptr++] = convert_int4(ray_bundle_array[3]);

  /* Test where all rays lie in the same cell */
  exit_points[0]=0.25f;  exit_points[1]= 0.25f;  exit_points[2]= 1.0f; 
  exit_points[3]=0.251f;  exit_points[4]= 0.252f;  exit_points[5]= 1.0f; 
  exit_points[6]=0.253f;  exit_points[7]= 0.254f;  exit_points[8]= 1.0f; 
  exit_points[9]=0.255f;  exit_points[10]=0.256f;  exit_points[11]=1.0f; 
  barrier(CLK_LOCAL_MEM_FENCE); 
  ret = load_data_mutable(cells, cell_data, n_levels, ray_bundle_array,
                          exit_points,cached_loc_codes,cached_data);

  /* Check the load_data return value */
  results[result_ptr++]= (int4)ret;
 /* Check the loc_codes */

  results[result_ptr++]=convert_int4(cached_loc_codes[0]);

  /* Check the data transfered to the local data cache */
    float16 temp = cached_data[0];
    results[result_ptr++]=(int4)temp.s0;

  /* Check the ray_bundle_array */
  results[result_ptr++] = convert_int4(ray_bundle_array[0]);
  results[result_ptr++] = convert_int4(ray_bundle_array[1]);
  results[result_ptr++] = convert_int4(ray_bundle_array[2]);
  results[result_ptr] = convert_int4(ray_bundle_array[3]);
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
  int local_id0 = get_local_id(0), local_id1 = get_local_id(1);
  int group_id0 = get_group_id(0), group_id1 = get_group_id(1);
  int mapped_id0=0, mapped_id1=0;
  map_work_space_2d(local_id0, local_id1, group_id0, group_id1,
                    &mapped_id0, &mapped_id1);
  int global_id0 = get_global_id(0), global_id1 = get_global_id(1);
  int gs0 = get_global_size(0);
  /* keep track of what thread executes by a global counter (results[0]) */
  int offset = results[0].x;
  results[offset+1] = (int4)(global_id0, global_id1, mapped_id0, mapped_id1);
  results[0].x=offset+1;
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
  if(local_bundle_index == 0){
    
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
  if(res==0){
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
                 __local float*    exit_points,
                 __local short4*   cached_loc_codes,
                 __local float16*  cached_data,
                 __local float4*   image_vect)
{
  short n_levels = 3;
  /* in this test the bundle must be 2x2 */
  /* setup example image array */
  image_vect[0].x=10.0f;   image_vect[1].x=20.0f; image_vect[2].x= 30.0f;
  image_vect[3].x=40.0f;
  /* exit points that access individual cells */
  exit_points[0]=0.25f;  exit_points[1]= 0.25f;  exit_points[2]= 1.0f;
  exit_points[3]=0.75f;  exit_points[4]= 0.25f;  exit_points[5]= 1.0f;
  exit_points[6]=0.25f;  exit_points[7]= 0.75f;  exit_points[8]= 1.0f;
  exit_points[9]=0.75f;  exit_points[10]=0.75f;  exit_points[11]=1.0f;
  barrier(CLK_LOCAL_MEM_FENCE); 
  int ret = load_data_mutable(cells, cell_data,n_levels, ray_bundle_array,
                              exit_points,cached_loc_codes,cached_data);
  int result_ptr = 0;
  /* Check the load_data return value */
  results[result_ptr++]= (int4)ret;
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  float seg_len = (float)(llid+1);

  seg_len_obs(seg_len, image_vect, ray_bundle_array, cached_data);
  results[result_ptr].x = (int)cached_data[0].s2;
  results[result_ptr].y = (int)cached_data[0].sc;
  results[result_ptr].z = (int)cached_data[0].se;
  results[result_ptr++].w = (int)cached_data[0].sf;
  results[result_ptr].x = (int)cached_data[1].s2;
  results[result_ptr].y = (int)cached_data[1].sc;
  results[result_ptr].z = (int)cached_data[1].se;
  results[result_ptr++].w = (int)cached_data[1].sf;
  results[result_ptr].x = (int)cached_data[2].s2;
  results[result_ptr].y = (int)cached_data[2].sc;
  results[result_ptr].z = (int)cached_data[2].se;
  results[result_ptr++].w = (int)cached_data[2].sf;
  results[result_ptr].x = (int)cached_data[3].s2;
  results[result_ptr].y = (int)cached_data[3].sc;
  results[result_ptr].z = (int)cached_data[3].se;
  results[result_ptr++].w = (int)cached_data[3].sf;
  /* Test where all rays lie in the same cell */
  exit_points[0]=0.25f;  exit_points[1]= 0.25f;  exit_points[2]= 1.0f; 
  exit_points[3]=0.251f;  exit_points[4]= 0.252f;  exit_points[5]= 1.0f; 
  exit_points[6]=0.253f;  exit_points[7]= 0.254f;  exit_points[8]= 1.0f; 
  exit_points[9]=0.255f;  exit_points[10]=0.256f;  exit_points[11]=1.0f; 
  barrier(CLK_LOCAL_MEM_FENCE); 
  ret = load_data_mutable(cells, cell_data, n_levels, ray_bundle_array,
                          exit_points,cached_loc_codes,cached_data);

  /* Check the load_data return value */
  results[result_ptr++]= (int4)ret;
  seg_len_obs(seg_len, image_vect, ray_bundle_array, cached_data);
  results[result_ptr].x = (int)cached_data[0].s2;
  results[result_ptr].y = (int)cached_data[0].sc;
  results[result_ptr].z = (int)cached_data[0].se;
  results[result_ptr++].w = (int)cached_data[0].sf;
  results[result_ptr].x = (int)cached_data[1].s2;
  results[result_ptr].y = (int)cached_data[1].sc;
  results[result_ptr].z = (int)cached_data[1].se;
  results[result_ptr++].w = (int)cached_data[1].sf;
  results[result_ptr].x = (int)cached_data[2].s2;
  results[result_ptr].y = (int)cached_data[2].sc;
  results[result_ptr].z = (int)cached_data[2].se;
  results[result_ptr++].w = (int)cached_data[2].sf;
  results[result_ptr].x = (int)cached_data[3].s2;
  results[result_ptr].y = (int)cached_data[3].sc;
  results[result_ptr].z = (int)cached_data[3].se;
  results[result_ptr++].w = (int)cached_data[3].sf;
}

__kernel
void
test_pre_infinity(__global int4* cells, __global float16* cell_data,
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
  for(unsigned i = 0; i<4; ++i)
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

  for(unsigned i = 0; i<4; ++i)
    results[result_ptr++] = convert_int4(1000.0f*image_vect[i]);


  /* test where all rays lie in the same cell */
  exit_points[0]=0.25f;  exit_points[1]= 0.25f;  exit_points[2]= 1.0f; 
  exit_points[3]=0.251f;  exit_points[4]= 0.252f;  exit_points[5]= 1.0f; 
  exit_points[6]=0.253f;  exit_points[7]= 0.254f;  exit_points[8]= 1.0f; 
  exit_points[9]=0.255f;  exit_points[10]=0.256f;  exit_points[11]=1.0f; 
  barrier(CLK_LOCAL_MEM_FENCE); 
  ret = load_data_mutable(cells, cell_data, n_levels, ray_bundle_array,
                          exit_points,cached_loc_codes,cached_data);

  for(unsigned i = 0; i<4; ++i)
    image_vect[i]=(float4)0.0f;
  image_vect[0].x=0.5f;   image_vect[1].x=0.6f; image_vect[2].x= 0.4f;
  image_vect[3].x=0.55f;
  image_vect[0].z=1.0f;   image_vect[1].z=1.0f; image_vect[2].z= 1.0f;
  image_vect[3].z=1.0f;
  float len = 0.25f*(float)(llid +1);/*vary the seg_len to reveal bugs*/
  seg_len_obs(len, image_vect, ray_bundle_array, cached_data);
  results[result_ptr].x = 1000.0f*cached_data[0].s2;
  results[result_ptr++].y = 1000.0f*cached_data[0].sc;

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

  pre_infinity(len, image_vect, ray_bundle_array, cached_data);
  for(unsigned i = 0; i<4; ++i)
    results[result_ptr++] = convert_int4(1000.0f*image_vect[i]);

  for(unsigned i = 0; i<4; ++i){
    results[result_ptr].x = (int)(1000.0f*cached_data[i].sf); /*seg_len*/
    results[result_ptr++].y = (int)(1000.0f*cached_data[i].sb); /* PI */
  }
}
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
  for(unsigned i = 0; i<4; ++i)
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
  for(unsigned i = 0; i<4; ++i)
    image_vect[i]=(float4)(1.0f,0.0f,1.0f,0.0f);
  bayes_ratio(seg_len, image_vect, ray_bundle_array, cached_data);

  for(unsigned i = 0; i<4; ++i)
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

  for(unsigned i = 0; i<4; ++i)
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
  for(unsigned i = 0; i<4; ++i)
    image_vect[i]=(float4)(0.25f*((float)(i+1)),0.0f,1.0f,0.0f);

  bayes_ratio(seg_len, image_vect, ray_bundle_array, cached_data);

  for(unsigned i = 0; i<4; ++i)
    results[result_ptr++] = convert_int4(1000.0f*image_vect[i]);

  results[result_ptr].x = (int)(1000.0f*cached_data[0].s2);
  results[result_ptr].y = (int)(1000.0f*cached_data[0].sc);
  results[result_ptr].z = (int)(1000.0f*cached_data[0].sd);
  results[result_ptr].w = (int)(1000.0f*cached_data[0].se);
}
