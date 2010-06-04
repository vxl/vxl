#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
/*
 * Load the necessary cell data items that are required by the 
 * rays in the ray bundle. The requested cells are defined by the
 * exit points for each ray. As the data is loaded, a check is made 
 * of the ray neighbors to see if the required cell data has already
 * been loaded. If so, the pointer for that ray is assigned to the
 * cached data item. If not the data is loaded from global memory and 
 * inserted in the data cache. Before each round of data input the cache
 * is cleared. This function requires that the byte_addressable_store
 * is supported by the opencl SDK.
 */
int load_data(__global int4*    cells,
              __global float16* cell_data,
              short n_levels,   /* number of tree levels */
              __local uchar*    ray_bundle_array, /* bundle pointer array */
              __local float*    exit_points, /* required exit points */
              __local short4*   cached_loc_codes,
              __local float16*  cached_data)
{
  int16 temp;
  uchar nbi = (uchar)get_local_size(0);
  uchar nbj = (uchar)get_local_size(1);
  uchar llid = (uchar)(get_local_id(0) + nbi*get_local_id(1));

  //serialized with thread 0 doing all the work
  if(llid==0){

    // clear the cache
    for(uchar jj = 0; jj<nbj; ++jj)
      for(uchar ii = 0; ii<nbi; ++ii){
        uchar ptr = ii+ (jj*nbi);/* 1-d array index */
        ray_bundle_array[ptr]=(uchar)0;
        cached_loc_codes[ptr]= (short4)-1;
        cached_data[ptr]= (float16)0.0f;
      }
    uchar offset = 0;
    for(uchar j = 0; j<nbj; ++j)
      for(uchar i = 0; i<nbi; ++i){
        uchar indx = i + nbi*j;
        /* note for now the traversal is from the root, however if a sub-tree
           is chached then the traversal can start at the root of the sub-tree
        */
        short4 root_code = (short4)(0,0,0,n_levels-1); /* location code of root */
        int root_ptr = 0; /* cell index for root */
        short4 loca; /*loc code with multiple uses */
        float4 exit_pt = (float4)1;
        /* load data */
        uchar tptr = 3*indx;
        exit_pt.x = exit_points[tptr];
        exit_pt.y = exit_points[tptr+1];
        exit_pt.z = exit_points[tptr+2];


        /* 
           n n    
           n x    The data for x might be at any of the bundle locations, n.
           check to see if neighbors already have the required data Note. Could
           be anywhere in the row above
        */
        uchar found = 0;
        if(i>0){
          tptr = indx-1;
          loca = cached_loc_codes[ray_bundle_array[tptr]];
          if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
            ray_bundle_array[indx]=ray_bundle_array[tptr];
            found = 1;
          }
          if(found==0&&j>0){ /*check neighbors in previous row */
            tptr = indx - nbi;
            loca = cached_loc_codes[ray_bundle_array[tptr]];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              ray_bundle_array[indx]=ray_bundle_array[tptr];
              found = 1;
            }else if(found==0){
              tptr--;
              loca = cached_loc_codes[ray_bundle_array[tptr]];
              if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
                ray_bundle_array[indx]=ray_bundle_array[tptr];
                found = 1;
              }
            }
          }
        }
        if(found == 0){
          /* data not in chache already */
          /* get tree cell corresponding to exit point */
          loca = loc_code(exit_pt, n_levels-1);
          int cell_ptr = traverse_force(cells, root_ptr, root_code, loca, &loca);
          /* loca now contains the loc_code of the found cell */
          if(cell_ptr<0) /* traverse failed */
            return (int)0;
          /* put data items in cache*/
          ray_bundle_array[indx] = offset;
          cached_loc_codes[offset] = loca;
          /*Insert the cell data request into the local data slot that owns the
            data */
          temp = (int16)(1, cells[cell_ptr].z,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
          cached_data[offset++]=as_float16(temp);
        }
      }
  }

  barrier(CLK_LOCAL_MEM_FENCE); 
  /* Load the required cell data. Each work item checks to see if:
     1) the flag to see if the data item slot needs to be loaded 
     2) if the item is to be loaded by *this* work item
  */
  temp = as_int16(cached_data[llid]);
  /* Check if this work item is supposed to load the data,
     if so, load the data into the slot corresponding to this 
     work item.
  */
  if(temp.s0 !=0)
    cached_data[llid] = cell_data[temp.s1];

  /* wait for all workitems to finish loading */
  barrier(CLK_LOCAL_MEM_FENCE); 

  return 1;   
} 
/* 
 *
 * Map the 2-d group id so that the mapped ids step by 2x in both column
 * and row. The odd column indices are addressed after 1/2 of the
 * groups have been processed. The two skipped odd row indices 
 * are processed after two even rows are processed.
 *
 */
void map_work_space_2d(int local_id0, int local_id1, 
                       int group_id0, int group_id1,
                       int* mapped_id0,
                       int* mapped_id1)
{
  /* map the group id to permuted 2d coordinates */
  int ls0 = get_local_size(0), ls1 = get_local_size(1);
  int ngi = (get_global_size(0))/ls0, ngj = (get_global_size(0))/ls1;
  /* 1-dimensional group index */
  int g_1d = group_id0+ngi*group_id1;

  /* offset is 1 if the 1-d group index is past the halfway point */
  int offset = (g_1d>=(ngi*ngj)/2) ? 1 : 0;

  /* step by 2 in group column index - add offset for odd indices after 1/2
     the groups have been processed*/
  int mi = 2*(group_id0%2)+ offset;

  /* step by 2 in group row index - then fill in odd indices that were
     skipped */
  int mj = 2*(group_id0/2)+ group_id1%2;

  /* map the individual bundle coordinates */
  (*mapped_id0) = mi*ls0 + local_id0;
  (*mapped_id1) = mj*ls1 + local_id1;
}
/*
 *  Determine initial ray bundle entry points for the entire tree bounding box
 *  This operation can be done by all the work items in parallel. Note that 
 *  the ray direction is the same in global and local cell coordinates,
 *  since the mapping is an isotropic scaling. The ray is returned with
 *  the side effect that the exit point is inserted in the local cache
 */

int ray_entry_point(__local float16* cam, __local float4* cam_center,
                    __local float4* bbox, __local uint4* roi, 
                    float4 cell_min, float4 cell_max,
                    uint grp_i, uint grp_j, uint local_bundle_index, 
                    float4* ray_o, float4* ray_d,
                    __local float* exit_points)
{
  /* The image coordinate is (grp_i, grp_j), i.e., the 2-d global index of 
   * the work group */
  if (grp_i<(*roi).x || grp_i>(*roi).y || grp_j<(*roi).z || grp_j> (*roi).w)
    return 0;
  /* find a world point by multiplying the image point by the pseudo-inverse
   * of the camera matrix. The SVD form of the pseudo inverse is stored in 
   * the cam data structure. 
   */
  *ray_d = backproject(grp_i,grp_j,cam[0],cam[1],cam[2],(*cam_center));
  *ray_o= (*cam_center-*bbox)/(*bbox).w;
  (*ray_o).w = 1.0f;

  float4 entry_pt;
  //find entry point of overall tree bounding box
  if (!cell_entry_point(*ray_o, *ray_d, cell_min, cell_max, &entry_pt))
    return 0;
  int ptr = 3*local_bundle_index;
  *(exit_points+ptr) = entry_pt.x;
  *(exit_points+ptr+1) = entry_pt.y;
  *(exit_points+ptr+2) = entry_pt.z;

  return 1;
}

  
