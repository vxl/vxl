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
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  float seg_len = (float)(llid+1), obs = ((float)(llid+1))*10.0f;
  seg_len_obs(seg_len, obs, ray_bundle_array, cached_data);
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
  seg_len_obs(seg_len, obs, ray_bundle_array, cached_data);
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
