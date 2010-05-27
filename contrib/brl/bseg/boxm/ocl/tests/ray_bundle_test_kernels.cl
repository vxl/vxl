#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
int load_data(__global int4*    cells,
              __global float16* cell_data,
              uchar nbi,      // bundle width
              uchar nbj,      // bundle height
              short n_levels, // number of tree levels
              __local uchar*    ray_bundle_array, // bundle pointer array
              __local float*    exit_points,      // required exit points
              __local short4*   cached_loc_codes,
              __local float16*  cached_data,
              int4* ld_res)
{
  //serialized with thread 0 doing all the work
  if (get_local_id(0)!=0)
    return 0;
  (*ld_res) = (int4)0;
  // clear the cache
  for (uchar j = 0; j<nbj; ++j)
    for (uchar i = 0; i<nbi; ++i) {
      int ptr = i+ (j*nbi);// 1-d array index
      ray_bundle_array[ptr]=(uchar)0;
      cached_loc_codes[ptr]= (short4)-1;
      cached_data[ptr]= (float16)0.0f;
    }

  // note for now the traversal is from the root, however if a sub-tree
  // is cached then the traversal can start at the root of the sub-tree
  short4 root_code = (short4)(0,0,0,n_levels-1); // location code of root
  int root_ptr = 0; // cell index for root
  short4 loca; // loc code with multiple uses
  uchar offset = 0;
  float4 exit_pt = (float4)1;
  // load data
  for (uchar j = 0; j<nbj; ++j)
    for (uchar i = 0; i<nbi; ++i) {
      int ptr = i+ (j*nbi); // 1-d array index
      //fetch the exit point for ray (i,j)
      uchar tptr = 3*ptr;
      exit_pt.x = exit_points[tptr];
      exit_pt.y = exit_points[tptr+1];
      exit_pt.z = exit_points[tptr+2];

      // n n
      // n x    The data for x might be at any of the bundle locations, n.
      // check to see if neighbors already have the required data Note. Could
      // be anywhere in the row above
      if (i>0) {
        tptr = ptr-1;
        loca = cached_loc_codes[ray_bundle_array[tptr]];
        if (cell_contains_exit_pt(n_levels, loca, exit_pt)) {
          ray_bundle_array[ptr]=ray_bundle_array[tptr];
          (*ld_res).x = 1;
          continue;
        }

        if (j>0) { // check neighbors in previous row
          tptr = ptr - nbi;
          loca = cached_loc_codes[ray_bundle_array[tptr]];
          if (cell_contains_exit_pt(n_levels, loca, exit_pt)) {
          ray_bundle_array[ptr]=ray_bundle_array[tptr];
          (*ld_res).y = 1;
          continue;
          }
          tptr--;
          loca = cached_loc_codes[ray_bundle_array[tptr]];
          if (cell_contains_exit_pt(n_levels, loca, exit_pt)) {
            ray_bundle_array[ptr]=ray_bundle_array[tptr];
          (*ld_res).z = 1;
            continue;
          }
        }
      }
      // data not in cache already
      // get tree cell corresponding to exit point
      loca = loc_code(exit_pt, n_levels-1);
      int cell_ptr = traverse_force(cells, root_ptr, root_code, loca, &loca);
      // loca now contains the loc_code of the found cell
      if (cell_ptr<0) // traversal failed
        return (int)0;
      // put data items in cache
      ray_bundle_array[ptr] = offset;
      cached_loc_codes[offset] = loca;
      cached_data[offset++] = cell_data[cells[cell_ptr].z];
      (*ld_res).x += 2;
    }
  return 1;
}

__kernel
void
test_load_data(__global int4* cells, __global float16* cell_data,
               __global int4* results,
               __local uchar*    ray_bundle_array,
               __local float*    exit_points,
               __local short4*   cached_loc_codes,
               __local float16*  cached_data)
{
  if (get_local_id(0)!=0)
    return;
  int4 res = (int4)0;
  uchar ni = 2;
  uchar nj = 2;
  short n_levels = 3;
  exit_points[0]=0.25f;  exit_points[1]= 0.25f;  exit_points[2]= 1.0f;
  exit_points[3]=0.75f;  exit_points[4]= 0.25f;  exit_points[5]= 1.0f;
  exit_points[6]=0.25f;  exit_points[7]= 0.75f;  exit_points[8]= 1.0f;
  exit_points[9]=0.75f;  exit_points[10]=0.75f;  exit_points[11]=1.0f;
  int ret = load_data(cells, cell_data, ni, nj, n_levels, ray_bundle_array,
                      exit_points,cached_loc_codes,cached_data, &res);
  int result_ptr = 0;
  results[result_ptr++]= (int4)ret;
  results[result_ptr++]= res;
  for (uchar i = 0; i<4; ++i) {
    results[result_ptr++]=convert_int4(cached_loc_codes[i]);
#if 0
    float4 p=(float4)(exit_points[3*i], exit_points[3*i+1], exit_points[3*i+2], 0.0f);
    short4 cd = loc_code(p, n_levels-1);
    results[result_ptr++] = convert_int4(cached_loc_codes[i]);
#endif
  }
#if 0
  int count = 40000000;
  for (int i = 0; i<count; ++i)
    cached_data[0]=cell_data[0];
  int result_ptr = 0;
  results[result_ptr++]= (int4)count;
#endif
}

