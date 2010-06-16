#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

#define  ACTIVE 1;
#define  NEXT_ADR_VALID 2;

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
        uchar org_ptr = 0;
        uchar tptr = 3*indx;
        exit_pt.x = exit_points[tptr];
        exit_pt.y = exit_points[tptr+1];
        exit_pt.z = exit_points[tptr+2];
        bool found = false;
        /* j = 0 */
        if(j==0){
          if(i>0)
            tptr = indx-1;
          org_ptr = ray_bundle_array[tptr];
          loca = cached_loc_codes[org_ptr];
          if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
            ray_bundle_array[indx]=org_ptr;
            found = true;
          }
        }
        if(!found&&j>0){
          /* bundle has only one column */
          if(i==0&&nbi==1){
            tptr = indx - nbi;
            org_ptr = ray_bundle_array[tptr];
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              ray_bundle_array[indx]=org_ptr;
              found = true;
            }
          }
          /* more than one column and upper right neighbor */
          if(!found&&i==0){
            tptr = indx - nbi + 1;
            org_ptr = ray_bundle_array[tptr];
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              ray_bundle_array[indx]=org_ptr;
              found = true;
            }
          }
          /* more than one column and neighbor above (above left N/A)*/
          if(!found&&i==0){
            tptr--;
            org_ptr = ray_bundle_array[tptr];
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              ray_bundle_array[indx]=org_ptr;
              found = true;
            }
          }
          /* interior to row upper right neighbor*/
          if(!found&&i>0&&i<(nbi-1)){
            tptr = indx - nbi + 1;
            org_ptr = ray_bundle_array[tptr];
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              ray_bundle_array[indx]=org_ptr;
              found = true;
            }
          }
          /* interior to row, neighbor above*/
          if(!found&&i>0&&i<(nbi-1)){
            tptr--;
            org_ptr = ray_bundle_array[tptr];
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              ray_bundle_array[indx]=org_ptr;
              found = true;
            }
          }
          /* interior to row, upper left neighbor*/
          if(!found&&i>0&&i<(nbi-1)){
            tptr--;
            org_ptr = ray_bundle_array[tptr];
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              ray_bundle_array[indx]=org_ptr;
              found = true;
            }
          }
          /* end of row, neighbor above */
          if(!found&&i==(nbi-1)){
            tptr = indx - nbi;
            org_ptr = ray_bundle_array[tptr];
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              ray_bundle_array[indx]=org_ptr;
              found = true;
            }
          }
          /* end of row, upper left neighbor */
          if(!found&&i==(nbi-1)){
            tptr--;
            org_ptr = ray_bundle_array[tptr];
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              ray_bundle_array[indx]=org_ptr;
              found = true;
            }
          }

        }
      
        if(!found){
          /* data not in chache already */
          /* get tree cell corresponding to exit point */
          loca = loc_code(exit_pt, n_levels-1);
		  int g_count=0;
          int cell_ptr = traverse_force(cells, root_ptr, root_code, loca, &loca,g_count);
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

int load_data_using_loc_codes(__local uchar*    ray_bundle_array, /* bundle pointer array */
							  __local short4*   cached_loc_codes)
							  // __local float16*  cached_data)
{
	uchar nbi = (uchar)get_local_size(0);
	uchar nbj = (uchar)get_local_size(1);
	uchar llid = (uchar)(get_local_id(0) + nbi*get_local_id(1));

	// clear the cache
	//serialized with thread 0 doing all the work
	if(llid==0){
		for(uchar j = 0; j<nbj; ++j)
			for(uchar i = 0; i<nbi; ++i){
				uchar indx = i + nbi*j;

				if(cached_loc_codes[indx].x>-1)
				{
					//ray_bundle_array[indx]=(uchar)indx;
					/* load data */
					uchar org_ptr = 0;
					uchar tptr = indx;

					bool found = false;
					/* j = 0 */
					if(j==0){
						if(i>0)
						{
							tptr = indx-1;
							org_ptr = ray_bundle_array[tptr];
							//if(!any(convert_int4(abs_diff(cached_loc_codes[tptr],cached_loc_codes[indx])))){
							if(cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
								cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
								cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
								cached_loc_codes[tptr].w==cached_loc_codes[indx].w ){
									ray_bundle_array[indx]=org_ptr;
									found = true;
							}
						}
					}
					if(!found&&j>0){
						/* bundle has only one column */
						// above neighbor everybody has it for j>0
						tptr = indx - nbi;
						org_ptr = ray_bundle_array[tptr];
						if(cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
							cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
							cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
							cached_loc_codes[tptr].w==cached_loc_codes[indx].w ){
								ray_bundle_array[indx]=org_ptr;
								found = true;
						}
						/* more than one column and upper right neighbor */
						if(!found&&i<(nbi-1)&&nbi>1){
							tptr = indx - nbi + 1;
							org_ptr = ray_bundle_array[tptr];
							if(cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
								cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
								cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
								cached_loc_codes[tptr].w==cached_loc_codes[indx].w ){
									ray_bundle_array[indx]=org_ptr;
									found = true;
							}
						}

						// left neighbor is goof for i>0 and j>0
						if(!found&&i>0){
							tptr = indx - nbi - 1;
							org_ptr = ray_bundle_array[tptr];
							if(cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
								cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
								cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
								cached_loc_codes[tptr].w==cached_loc_codes[indx].w ){
									ray_bundle_array[indx]=org_ptr;
									found = true;
							}
						}
						// left neighbor is goof for i>0
						if(!found&&i>0){
							tptr = indx -  1;
							org_ptr = ray_bundle_array[tptr];
							if(cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
								cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
								cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
								cached_loc_codes[tptr].w==cached_loc_codes[indx].w ){
									ray_bundle_array[indx]=org_ptr;
									found = true;
							}
						}
					}
				}
			}
	}

	barrier(CLK_LOCAL_MEM_FENCE); 

	return 1;   
} 


/* 
 * Function for updating states and pointers for load_data_mutable 
 */
void update_state_ptr(__local uchar4*   ray_bundle_array,
                      uchar indx,    /* current ray slot  */
                      uchar org_ptr  /* region base ptr   */)
{
  /*set region base pointer*/
  ray_bundle_array[indx].x=org_ptr;
  ray_bundle_array[indx].w = ray_bundle_array[indx].w | ACTIVE;

  /*next ray slot ptr in linked list inserted in previous slot*/
  uchar prev_ptr = ray_bundle_array[org_ptr].z;
  ray_bundle_array[prev_ptr].y=indx;
  ray_bundle_array[prev_ptr].w = 
    ray_bundle_array[prev_ptr].w | NEXT_ADR_VALID; 

  /*current slot ptr to be used as link when next slot is found*/
  ray_bundle_array[org_ptr].z=indx;

}

/*
 * Load the necessary cell data items that are required by the 
 * rays in the ray bundle. The requested cells are defined by the
 * exit points for each ray. As the data is loaded, a check is made 
 * of the ray neighbors to see if the required cell data has already
 * been loaded. If so, the pointer for that ray is assigned to the
 * cached data item. If not the data is loaded from global memory and 
 * inserted in the data cache. Before each round of data input the cache
 * is cleared. This function differs from the library function "load_data" 
 * in that additional mechanisms are provided that prevent conflicts 
 * when the data is written, as in model updating. In this function, the 
 * ray_bundle_array contains information that supports serial processing 
 * of each multiple ray data item to prevent conflicts. Especially note
 * that the ray_bundle_array is uchar4 instead of uchar as in "load_data."
 * The content of the ray_bundle_array vector is:
 *      x       y          z         w
 *   [ ptr | next_ptr | curr_ptr | flags ]
 *
 *      o ptr is the address in the arrays of loc_codes and chached data
 *        corresponding to the ray 
 *      o next_ptr is the next ray slot (linear index) in the connected region
 *      o curr_ptr is the current ray slot in the region (linear index)
 *      o flags define the state of a ray slot
 *         - f1 -> active slot
 *         - f2 -> next_ptr valid
 *
 * This function requires that the byte_addressable_store option is 
 * supported by the opencl SDK.
 */
int load_data_mutable(__global int4*    cells,
                      __global float16* cell_data,
                      short n_levels,   /* number of tree levels */
                      __local uchar4*   ray_bundle_array, /* bundle state*/
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
        ray_bundle_array[ptr]=(uchar4)0;
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
           n n n   
           n x    The data for x might be at any of the bundle locations, n.
           check to see if neighbors already have the required data Note. Could
           be anywhere in the row above
        */
        bool found = false;
        uchar org_ptr = 0;
        /* j = 0 */
        if(j==0){
          if(i>0)
            tptr = indx-1;
          org_ptr = ray_bundle_array[tptr].x;
          loca = cached_loc_codes[org_ptr];
          if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
            update_state_ptr(ray_bundle_array, indx, org_ptr);
            found = true;
          }
        }
        if(!found&&j>0){

          /* bundle has only one column */
          if(i==0&&nbi==1){
            tptr = indx - nbi;
            org_ptr = ray_bundle_array[tptr].x;
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              update_state_ptr(ray_bundle_array, indx, org_ptr);
              found = true;
            }
          }
          /* more than one column and upper right neighbor */
          if(!found&&i==0){
            tptr = indx - nbi + 1;
            org_ptr = ray_bundle_array[tptr].x;
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              update_state_ptr(ray_bundle_array, indx, org_ptr);
              found = true;
            }
          }
          /* more than one column and neighbor above (above left N/A)*/
          if(!found&&i==0){
            tptr--;
            org_ptr = ray_bundle_array[tptr].x;
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              update_state_ptr(ray_bundle_array, indx, org_ptr);
              found = true;
            }
          }
          /* interior to row upper right neighbor*/
          if(!found&&i>0&&i<(nbi-1)){
            tptr = indx - nbi + 1;
            org_ptr = ray_bundle_array[tptr].x;
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              update_state_ptr(ray_bundle_array, indx, org_ptr);
              found = true;
            }
          }
          /* interior to row, neighbor above*/
          if(!found&&i>0&&i<(nbi-1)){
            tptr--;
            org_ptr = ray_bundle_array[tptr].x;
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              update_state_ptr(ray_bundle_array, indx, org_ptr);
              found = true;
            }
          }
          /* interior to row, upper left neighbor*/
          if(!found&&i>0&&i<(nbi-1)){
            tptr--;
            org_ptr = ray_bundle_array[tptr].x;
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              update_state_ptr(ray_bundle_array, indx, org_ptr);
              found = true;
            }
          }
          /* end of row, neighbor above */
          if(!found&&i==(nbi-1)){
            tptr = indx - nbi;
            org_ptr = ray_bundle_array[tptr].x;
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              update_state_ptr(ray_bundle_array, indx, org_ptr);
              found = true;
            }
          }
          /* end of row, upper left neighbor */
          if(!found&&i==(nbi-1)){
            tptr--;
            org_ptr = ray_bundle_array[tptr].x;
            loca = cached_loc_codes[org_ptr];
            if(cell_contains_exit_pt(n_levels, loca, exit_pt)){
              update_state_ptr(ray_bundle_array, indx, org_ptr);
              found = true;
            }
          }
        }
        if(!found){
          /* data not in chache already */
          /* get tree cell corresponding to exit point */
          loca = loc_code(exit_pt, n_levels-1);
		  int g_count=0;
          int cell_ptr = traverse_force(cells, root_ptr, root_code, loca, &loca,g_count);
          /* loca now contains the loc_code of the found cell */
          if(cell_ptr<0) /* traverse failed */
            return (int)0;
          /* put data items in cache*/
          ray_bundle_array[indx].x = offset;
          ray_bundle_array[indx].z = offset;
          ray_bundle_array[indx].w = ray_bundle_array[indx].w | ACTIVE;
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

void map_work_space_2d(int* mapped_id0,
                       int* mapped_id1)
{
  unsigned lid0 = get_local_id(0);
  unsigned lid1 = get_local_id(1);
  int group_id0 = get_group_id(0), group_id1 = get_group_id(1);

  /* map the group id to permuted 2d coordinates */
  int ls0 = get_local_size(0), ls1 = get_local_size(1);
  int ngi = (get_global_size(0))/ls0, ngj = (get_global_size(1))/ls1;
  /* 1-dimensional group index */
  int g_1d = group_id0+ngi*group_id1;

  /* offset is 1 if the 1-d group index is past the halfway point */
  int offset0 = 0;

  int offset1 = 0;
  
  if(g_1d>=(ngi*ngj)/4)
  {
	  offset0=1;
	  if(g_1d>=(ngi*ngj)/2)
	  {
		  offset0=0;
		  offset1=1;
		  if(g_1d>=3*(ngi*ngj)/4)
		  {
			offset0=1;
		  }
	  }
  }

  /* step by 2 in group column index - add offset for odd indices after 1/2
     the groups have been processed*/
  int mi = (group_id0 <ngi/2)? 2*(group_id0):2*(group_id0-ngi/2);
  int mj = 4*(group_id1%(ngj/4))+2*(group_id0/(ngi/2));

  mi=mi+offset0;
  mj=mj+offset1;
  /* map the individual bundle coordinates */
  (*mapped_id0) = mi*ls0 + lid0;
  (*mapped_id1) = mj*ls1 + lid1;
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

/*
 *  Accumulate ray segment length and the weighted observation
 *  sums as each cell is accessed. It can be the case that multiple rays pass
 *  through the same cell. In order to avoid conflicts, only one thread 
 *  is allowed to update the sums for such cells. The active thread is
 *  identified with the index of the base pointer of the multiple ray
 *  region in the bundle array. The argument image_vect is a linear array of
 *  float4 vectors corresponding to the index of the local ray bundle, in
 *  raster order. The layout of the vector is:
 *  [obs | alpha_integral | vis | pre ]
 *
 */
void seg_len_obs(float seg_len, __local float4* image_vect,
                 __local uchar4*   ray_bundle_array,
                 __local float16*  cached_data)
{
  /* linear thread id */
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  /* first insert seg_len into the local cache. This step
     is carried out by all threads */
  cached_data[llid].sf = seg_len;
  barrier(CLK_LOCAL_MEM_FENCE); /*wait for all threads to complete */

  /*limit access to the thread that owns each ray bundle connected region */
  if(ray_bundle_array[llid].x!=llid)
    return;
  uchar temp = ray_bundle_array[llid].w & ACTIVE;
  if(temp == 0)
    return;
  /* The region owner (base) is now the only active thread within the region*/
  /* process the base ray */
  cached_data[llid].s2 += cached_data[llid].sf; /* seg_len sum */
  /* weighted observations */
  cached_data[llid].sc += (image_vect[llid].x)*cached_data[llid].sf; 

  uchar adr = llid;/* linked list pointer */
  /* traverse the linked list and increment sums */
  temp = ray_bundle_array[adr].w & NEXT_ADR_VALID;
  while( temp > 0)
    {
      adr = ray_bundle_array[adr].y;
      cached_data[llid].s2 += cached_data[adr].sf;
      cached_data[llid].sc += (image_vect[adr].x)*cached_data[adr].sf; 
      temp = ray_bundle_array[adr].w & NEXT_ADR_VALID;
    }
}

/*
 *  Accumulate the pre and vis image arrays based on the cell data.
 *  A vector of images is maintained image_vect that is updated as the rays
 *  step through the volume. The image vector is assigned as:
 *
 *  [obs | alpha_integral | vis | pre ]
 *
 */
void pre_infinity(float seg_len, __local float4* image_vect,
                  __local uchar4*   ray_bundle_array,
                  __local float16*  cached_data)
{
  /* linear thread id */
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  /* first insert seg_len into the local cache. This step
     is carried out by all threads */
  cached_data[llid].sf = seg_len;
  barrier(CLK_LOCAL_MEM_FENCE); /*wait for all threads to complete */

  /*limit access to the thread that owns each ray bundle connected region */
  uchar temp = ray_bundle_array[llid].w & ACTIVE;
  if(temp==0) return;
  float temp1 = 0.0f, temp2 = 0.0f; /* minimize registers */
  if(ray_bundle_array[llid].x==llid){
    /* if total length of rays is too small, do nothing */
    temp1 = cached_data[llid].s2; /* length sum */
    if(temp1<1.0e-4f)
      return;
    /* The mean intensity for the cell */
    temp2 = cached_data[llid].sc/temp1; /* mean observation */
    temp1 = gauss_3_mixture_prob_density(temp2,
                                         cached_data[llid].s3,
                                         cached_data[llid].s4,
                                         cached_data[llid].s5,
                                         cached_data[llid].s6,
                                         cached_data[llid].s7,
                                         cached_data[llid].s8,
                                         cached_data[llid].s9,
                                         cached_data[llid].sa,
                                         (1.0f-cached_data[llid].s5
                                          -cached_data[llid].s8)
                                         );/* PI */
    cached_data[llid].sb = temp1;
    temp2 = cached_data[llid].s0; /* alpha */
  }
  barrier(CLK_LOCAL_MEM_FENCE); /*wait for all threads to complete */
  /* Below, all threads participate in updating the pre-vis images */
  /* pointer to the cached cell data for this ray */
  temp = ray_bundle_array[llid].x;

  /*alpha integral          alpha           *        seg_len      */
  image_vect[llid].y += cached_data[temp].s0*cached_data[llid].sf;

  temp2 = exp(-image_vect[llid].y); /* vis_prob_end */

  /* updated pre                      Omega         *       PI         */
  image_vect[llid].w += (image_vect[llid].z - temp2)*cached_data[temp].sb;

  /* updated visibility probability */
  image_vect[llid].z = temp2;

}
/*
 * Form the denominator of the Bayes update expression,
 * which normalizes the expression to form a probability
 * When this function is called the layout of image_vect is:
 * [obs | * | vis_inf | pre_inf ]
 * The layout is modified by this function to form,
 * [norm | * | * | * ]. The * values indicate that the value is
 * not used for computations, and the slot is free to be used by subsequent 
 * functions.  The argument p_inf defines the probability density function
 * to be used for remote surfaces outside the voxel domain. The vector
 * p_inf is laid out as:
 *  [switch|mean|std_dev|*]. 
 * If switch  > 0, the uniform distribution p(x)=1.0 is used. Otherwise,
 * a Gaussian with the specifived mean and standard deviation is used.
 * 
 */
__kernel void proc_norm_image(__global float4* image, __global float4* p_inf)
{
  /* linear index of the global and local image */
  int lgid = get_global_id(0) + get_global_size(0)*get_global_id(1);
  float4 vect = image[lgid];
  float mult = (p_inf[0].x>0.0f) ? 1.0f : 
    gauss_prob_density(vect.x, p_inf[0].y, p_inf[0].z);
  /* compute the norm image */
  vect.x = vect.w + mult * vect.z;
  /* write it back */
  image[lgid] = vect;
}
/*
 *  Compute the Bayes update ratio for alpha
 *  A vector of images is maintained image_vect that is updated as the rays
 *  step through the volume. The image vector is assigned as:
 *
 *  [norm | alpha_integral | vis | pre ]
 *
 */
void bayes_ratio(float seg_len, __local float4* image_vect,
                 __local uchar4*   ray_bundle_array,
                 __local float16*  cached_data)
{
  /* linear thread id */
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  /* insert seg_len into the local cache. This step
     is carried out by all threads */
  cached_data[llid].sf = seg_len;

  uchar temp = ray_bundle_array[llid].w & ACTIVE;
  if(temp==0) return;
  float temp1 = 0.0f, temp2 = 0.0f; /* minimize registers */
  /* If ray is owner of the octree cell */
  if(ray_bundle_array[llid].x==llid){
    /* if total length of rays is too small, do nothing */
    temp1 = cached_data[llid].s2; /* length sum */
    if(temp1<1.0e-4f)
      return;
    /* The mean intensity for the cell */
    temp2 = cached_data[llid].sc/temp1; /* mean observation */
    temp1 = gauss_3_mixture_prob_density(temp2,
                                         cached_data[llid].s3,
                                         cached_data[llid].s4,
                                         cached_data[llid].s5,
                                         cached_data[llid].s6,
                                         cached_data[llid].s7,
                                         cached_data[llid].s8,
                                         cached_data[llid].s9,
                                         cached_data[llid].sa,
                                         (1.0f-cached_data[llid].s5
                                          -cached_data[llid].s8)
                                         );/* PI */
    cached_data[llid].sb = temp1;
    temp2 = cached_data[llid].s0; /* alpha */
  }
  barrier(CLK_LOCAL_MEM_FENCE); /*wait for all threads to complete */

  /* Below, all threads participate in updating the alpha integral, pre
   * and vis images */
  temp = ray_bundle_array[llid].x;/* pointer to region owner cell data */

  /*alpha integral          alpha           *        seg_len      */
  image_vect[llid].y += cached_data[temp].s0*cached_data[llid].sf;

  temp2 = exp(-image_vect[llid].y); /* vis_prob_end */

  /* updated pre                      Omega         *       PI         */
  image_vect[llid].w += (image_vect[llid].z - temp2)*cached_data[temp].sb;

  /* updated visibility probability */
  image_vect[llid].z = temp2;

  barrier(CLK_LOCAL_MEM_FENCE); /*wait for all threads to complete */

  /* region owner scans the region and computes Bayes ratio and weighted vis*/
  if(ray_bundle_array[llid].x==llid){

    /* compute data for base ray cell */
    /*   cell.vis         +=        vis(i,j)    *      seg_len */
    cached_data[llid].se += image_vect[llid].z*cached_data[llid].sf;

    /* Bayes ratio */
    /* ( pre + PI*vis)/norm)*seg_len */
    cached_data[llid].sd += 
      /*      pre(i,j)        +        PI        *       vis(i,j) */
      ((image_vect[llid].w + cached_data[llid].sb*image_vect[llid].z)/
       image_vect[llid].x)*cached_data[llid].sf;
    /*     norm(i,j)        seg_len */

    /* If ray has no neighbors - then just return */
    temp = ray_bundle_array[llid].w & NEXT_ADR_VALID;
    if(temp==0) return;

    uchar adr = llid; /* linked list pointer */
    while(temp>0){
      adr = ray_bundle_array[adr].y;/* follow the linked list */

      /* cell.vis           +=     vis(i,j)      *      seg_len     */
      cached_data[llid].se += image_vect[adr].z*cached_data[adr].sf;

      /* Bayes ratio */
      /* ( pre + PI*vis)/norm)*seg_len */
      cached_data[llid].sd += 
        ((image_vect[adr].w + cached_data[llid].sb*image_vect[adr].z)/image_vect[adr].x)*cached_data[adr].sf;

      temp = ray_bundle_array[adr].w & NEXT_ADR_VALID;
    }
  }
}
