#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

#define  ACTIVE 1;
#define  NEXT_ADR_VALID 2;

/*
* Determine the necessary cell data items that are required by the
* rays in the ray bundle. The requested cell data items are defined by the
* cached loc_codes for each ray. As the ray bundle is scanned, a check is made
* of the ray neighbors to see if the required cell data is already in
* use, i.e. a neighboring ray has the same required loc_code. If so,
* the pointer for that ray is assigned the neighbor's data pointer.
* If not, a new data pointer is inserted corresponding to the cell data
* for the required loc_code. Before each round of data indexing,
* the ray_bundle_array is initialized to the same index as the local
* work group id, which is the address that would be valid if no data is
* shared, i.e. a one-to-one correspondence between ray id and cache adrress.
* This function is valid only if byte_addressable_store is supported by
* the opencl SDK.
*/
int load_data_using_loc_codes(__local uchar*    ray_bundle_array, /* bundle pointer array */
                              __local short4*   cached_loc_codes)
                              // __local float16*  cached_data)
{
    uchar nbi = (uchar)get_local_size(0);
    uchar nbj = (uchar)get_local_size(1);
    uchar llid = (uchar)(get_local_id(0) + nbi*get_local_id(1));

    /* initialize pointer to local id */
    ray_bundle_array[llid]=llid;
    barrier(CLK_LOCAL_MEM_FENCE);

    // clear the cache
    //serialized with thread 0 doing all the work
    if (llid==0) {
        for (uchar j = 0; j<nbj; ++j)
            for (uchar i = 0; i<nbi; ++i)
            {
                uchar indx = i + nbi*j;
                if (cached_loc_codes[indx].x>-1)
                {
                    //ray_bundle_array[indx]=(uchar)indx;
                    /* load data */
                    uchar org_ptr = 0;
                    uchar tptr = indx;

                    bool found = false;
                    /* j = 0 */
                    if (j==0) {/* for first row, only left neighbor is valid */
                        if (i>0)/* except in first column */
                        {
                            tptr = indx-1;
                            org_ptr = ray_bundle_array[tptr];
                            if (cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
                                cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
                                cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
                                cached_loc_codes[tptr].w==cached_loc_codes[indx].w ) {
                                    ray_bundle_array[indx]=org_ptr;
                                    found = true;
                            }
                        }
                    }
                    if (!found&&j>0) {
                        /* above neighbor is always valid for j>0*/
                        tptr = indx - nbi;
                        org_ptr = ray_bundle_array[tptr];
                        if (cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
                            cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
                            cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
                            cached_loc_codes[tptr].w==cached_loc_codes[indx].w ) {
                                ray_bundle_array[indx]=org_ptr;
                                found = true;
                        }
                        /* more than one column and upper right neighbor */
                        if (!found&&i<(nbi-1)&&nbi>1) {
                            tptr = indx - nbi + 1;
                            org_ptr = ray_bundle_array[tptr];
                            if (cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
                                cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
                                cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
                                cached_loc_codes[tptr].w==cached_loc_codes[indx].w ) {
                                    ray_bundle_array[indx]=org_ptr;
                                    found = true;
                            }
                        }
                        // upper left neighbor is valid for i>0 and j>0
                        if (!found&&i>0) {
                            tptr = indx - nbi - 1;
                            org_ptr = ray_bundle_array[tptr];
                            if (cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
                                cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
                                cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
                                cached_loc_codes[tptr].w==cached_loc_codes[indx].w ) {
                                    ray_bundle_array[indx]=org_ptr;
                                    found = true;
                            }
                        }
                        // left neighbor is valid for i>0
                        if (!found&&i>0) {
                            tptr = indx -  1;
                            org_ptr = ray_bundle_array[tptr];
                            if (cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
                                cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
                                cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
                                cached_loc_codes[tptr].w==cached_loc_codes[indx].w ) {
                                    ray_bundle_array[indx]=org_ptr;
                                    found = true;
                            }
                        }
                    }
                }
            }
    }
    /* This function only inserts single ray_bundle_array pointers to access the
    * data in a read-only mode, e.g., for expected image generation
    */
    barrier(CLK_LOCAL_MEM_FENCE);

    return 1;
}

int load_data_using_cell_ptrs(__local uchar*    ray_bundle_array, /* bundle pointer array */
                              __local int*   cell_ptrs)
                              // __local float16*  cached_data)
{
    uchar nbi = (uchar)get_local_size(0);
    uchar nbj = (uchar)get_local_size(1);
    uchar llid = (uchar)(get_local_id(0) + nbi*get_local_id(1));

    /* initialize pointer to local id */
    ray_bundle_array[llid]=llid;
    barrier(CLK_LOCAL_MEM_FENCE);

    int count=0;
    // clear the cache
    //serialized with thread 0 doing all the work
    if (llid==0) {
        for (uchar j = 0; j<nbj; ++j)
            for (uchar i = 0; i<nbi; ++i)
            {
                uchar indx = i + nbi*j;
                if (cell_ptrs[indx]>-1)
                {
                    //ray_bundle_array[indx]=(uchar)indx;
                    /* load data */
                    uchar org_ptr = 0;
                    uchar tptr = indx;

                    bool found = false;
                    /* j = 0 */
                    if (j==0) {/* for first row, only left neighbor is valid */
                        if (i>0)/* except in first column */
                        {
                            tptr = indx-1;
                            org_ptr = ray_bundle_array[tptr];
                            if (cell_ptrs[tptr]==cell_ptrs[indx] ) {
                                ray_bundle_array[indx]=org_ptr;
                                found = true;
                                count++;
                            }
                        }
                    }
                    if (!found&&j>0) {
                        /* above neighbor is always valid for j>0*/
                        tptr = indx - nbi;
                        org_ptr = ray_bundle_array[tptr];
                        if (cell_ptrs[tptr]==cell_ptrs[indx] ) {
                            ray_bundle_array[indx]=org_ptr;
                            found = true;
                            count++;
                        }
                        /* more than one column and upper right neighbor */
                        if (!found&&i<(nbi-1)&&nbi>1) {
                            tptr = indx - nbi + 1;
                            org_ptr = ray_bundle_array[tptr];
                            if (cell_ptrs[tptr]==cell_ptrs[indx] )
                            {
                                ray_bundle_array[indx]=org_ptr;
                                found = true;
                                count++;
                            }
                        }
                        // upper left neighbor is valid for i>0 and j>0
                        if (!found&&i>0) {
                            tptr = indx - nbi - 1;
                            org_ptr = ray_bundle_array[tptr];
                            if (cell_ptrs[tptr]==cell_ptrs[indx] )
                            {
                                ray_bundle_array[indx]=org_ptr;
                                found = true;
                                count++;
                            }
                        }
                        // left neighbor is valid for i>0
                        if (!found&&i>0) {
                            tptr = indx -  1;
                            org_ptr = ray_bundle_array[tptr];
                            if (cell_ptrs[tptr]==cell_ptrs[indx] ){
                                ray_bundle_array[indx]=org_ptr;
                                found = true;
                                count++;
                            }
                        }
                    }
                }
            }
    }
    /* This function only inserts single ray_bundle_array pointers to access the
    * data in a read-only mode, e.g., for expected image generation
    */
    barrier(CLK_LOCAL_MEM_FENCE);

    return count;
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

/* Determine pointers to the necessary cell data items that are required by the
* rays in the ray bundle. The requested cells are defined by the
* required loc_codes for each ray. As the bundle is scanned, a check is made
* of the ray neighbors to see if the required cell data has already
* been associated with a neighboring ray. If so, the pointer for that ray
* is assigned to that of the neighbor. If not, the pointer to the data in
* the cache is determined and assigned to the ray_bundle_array data pointer,
* (the .x slot of the pointer vector).
* This function differs from the library function "load_data"
* in that additional mechanisms are provided that prevent conflicts
* when the data is written, as in model updating. In this function, the
* ray_bundle_array contains information in the form of a linked list that
* supports serial processing of each multiple ray data item to prevent
* conflicts. More specifically, when a data item is shared by a number of
* rays, a "master" ray is allowed to run sequentially to scan the connected
* region defined by the linked list and to update the cell data as needed.
* In this case, the ray_bundle_array is uchar4 instead of uchar as in
* "load_data." The content of the ray_bundle_array vector is:
*      x       y          z         w
*   [ ptr | next_ptr | curr_ptr | flags ]
*
*      o ptr is the address in the arrays of loc_codes and cached data
*        required by the ray
*      o next_ptr is the next ray slot (linear index) in the connected region
*      o curr_ptr is the current ray slot in the region (linear index)
*      o flags define the state of a ray slot
*         - f1 -> active slot
*         - f2 -> next_ptr valid
*
* This function requires that the byte_addressable_store option is
* supported by the opencl SDK.
*/
int load_data_mutable_using_loc_codes( __local uchar4*   ray_bundle_array, /* bundle state*/
                                      __local short4*   cached_loc_codes)
{
    uchar nbi = (uchar)get_local_size(0);
    uchar nbj = (uchar)get_local_size(1);
    uchar llid = (uchar)(get_local_id(0) + nbi*get_local_id(1));
    ray_bundle_array[llid]=(uchar4)(llid,0,0,0);
    barrier(CLK_LOCAL_MEM_FENCE);
    // clear the cache
    //serialized with thread 0 doing all the work
    if (llid==0) {
        for (uchar j = 0; j<nbj; ++j)
            for (uchar i = 0; i<nbi; ++i) {
                uchar indx = i + nbi*j;
                if (cached_loc_codes[indx].x>-1)
                {
                    //ray_bundle_array[indx]=(uchar)indx;
                    /* load data */
                    uchar org_ptr = 0;
                    uchar tptr = indx;

                    bool found = false;
                    /* j = 0 */
                    if (j==0) {/* for first row, only left neighbor is valid */
                        if (i>0)/* except for the first column */
                        {
                            tptr = indx-1;
                            org_ptr = ray_bundle_array[tptr].x;
                            if (cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
                                cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
                                cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
                                cached_loc_codes[tptr].w==cached_loc_codes[indx].w ) {
                                    update_state_ptr(ray_bundle_array, indx, org_ptr);
                                    found = true;
                            }
                        }
                    }
                    if (!found&&j>0) {
                        /* above neighbor is always valid for j>0 */
                        tptr = indx - nbi;
                        org_ptr = ray_bundle_array[tptr].x;
                        if (cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
                            cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
                            cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
                            cached_loc_codes[tptr].w==cached_loc_codes[indx].w ) {
                                update_state_ptr(ray_bundle_array, indx, org_ptr);
                                found = true;
                        }
                        /* more than one column and upper right neighbor */
                        if (!found&&i<(nbi-1)&&nbi>1) {
                            tptr = indx - nbi + 1;
                            org_ptr = ray_bundle_array[tptr].x;
                            if (cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
                                cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
                                cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
                                cached_loc_codes[tptr].w==cached_loc_codes[indx].w ) {
                                    update_state_ptr(ray_bundle_array, indx, org_ptr);
                                    found = true;
                            }
                        }
                        /* upper left neighbor is valid for i>0 and j>0 */
                        if (!found&&i>0) {
                            tptr = indx - nbi - 1;
                            org_ptr = ray_bundle_array[tptr].x;
                            if (cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
                                cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
                                cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
                                cached_loc_codes[tptr].w==cached_loc_codes[indx].w ) {
                                    update_state_ptr(ray_bundle_array, indx, org_ptr);
                                    found = true;
                            }
                        }
                        /* left neighbor is valid for i>0 */
                        if (!found&&i>0) {
                            tptr = indx -  1;
                            org_ptr = ray_bundle_array[tptr].x;
                            if (cached_loc_codes[tptr].x==cached_loc_codes[indx].x &&
                                cached_loc_codes[tptr].y==cached_loc_codes[indx].y &&
                                cached_loc_codes[tptr].z==cached_loc_codes[indx].z &&
                                cached_loc_codes[tptr].w==cached_loc_codes[indx].w ) {
                                    update_state_ptr(ray_bundle_array, indx, org_ptr);
                                    found = true;
                            }
                        }
                    }
                    if (!found) {
                        /*establish the new data pointers. Next address is not enabled*/
                        ray_bundle_array[indx].x = indx;
                        ray_bundle_array[indx].z = indx;
                        ray_bundle_array[indx].w = ray_bundle_array[indx].w | ACTIVE;
                    }
                }
            }
    }
    /* note that no data is actually loaded, only the pointers are set up */
    barrier(CLK_LOCAL_MEM_FENCE);

    return 1;
}
int load_data_mutable_using_cell_ptrs( __local uchar4*   ray_bundle_array, /* bundle state*/
                                       __local int*   cell_ptrs)
{
    uchar nbi = (uchar)get_local_size(0);
    uchar nbj = (uchar)get_local_size(1);
    uchar llid = (uchar)(get_local_id(0) + nbi*get_local_id(1));
    ray_bundle_array[llid]=(uchar4)(llid,0,0,0);
    barrier(CLK_LOCAL_MEM_FENCE);
    // clear the cache
    //serialized with thread 0 doing all the work
    if (llid==0) {
        for (uchar j = 0; j<nbj; ++j)
            for (uchar i = 0; i<nbi; ++i) {
                uchar indx = i + nbi*j;
                if (cell_ptrs[indx]>-1)
                {
                    //ray_bundle_array[indx]=(uchar)indx;
                    /* load data */
                    uchar org_ptr = 0;
                    uchar tptr = indx;

                    bool found = false;
                    /* j = 0 */
                    if (j==0) {/* for first row, only left neighbor is valid */
                        if (i>0)/* except for the first column */
                        {
                            tptr = indx-1;
                            org_ptr = ray_bundle_array[tptr].x;
                            if (cell_ptrs[tptr]==cell_ptrs[indx]) {
                                update_state_ptr(ray_bundle_array, indx, org_ptr);
                                found = true;
                            }
                        }
                    }
                    if (!found&&j>0) {
                        /* above neighbor is always valid for j>0 */
                        tptr = indx - nbi;
                        org_ptr = ray_bundle_array[tptr].x;
                        if (cell_ptrs[tptr]==cell_ptrs[indx]) {
                            update_state_ptr(ray_bundle_array, indx, org_ptr);
                            found = true;
                        }
                        /* more than one column and upper right neighbor */
                        if (!found&&i<(nbi-1)&&nbi>1) {
                            tptr = indx - nbi + 1;
                            org_ptr = ray_bundle_array[tptr].x;
                            if (cell_ptrs[tptr]==cell_ptrs[indx]) {
                                update_state_ptr(ray_bundle_array, indx, org_ptr);
                                found = true;
                            }
                        }
                        /* upper left neighbor is valid for i>0 and j>0 */
                        if (!found&&i>0) {
                            tptr = indx - nbi - 1;
                            org_ptr = ray_bundle_array[tptr].x;
                            if (cell_ptrs[tptr]==cell_ptrs[indx]) {
                                update_state_ptr(ray_bundle_array, indx, org_ptr);
                                found = true;
                            }
                        }
                        /* left neighbor is valid for i>0 */
                        if (!found&&i>0) {
                            tptr = indx -  1;
                            org_ptr = ray_bundle_array[tptr].x;
                            if (cell_ptrs[tptr]==cell_ptrs[indx]) {
                                update_state_ptr(ray_bundle_array, indx, org_ptr);
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        /*establish the new data pointers. Next address is not enabled*/
                        ray_bundle_array[indx].x = indx;
                        ray_bundle_array[indx].z = indx;
                        ray_bundle_array[indx].w = ray_bundle_array[indx].w | ACTIVE;
                    }
                }
            }
    }
    /* note that no data is actually loaded, only the pointers are set up */
    barrier(CLK_LOCAL_MEM_FENCE);

    return 1;
}


///*
// *
// * Map the 2-d group id so that the mapped ids step by 2x in both column
// * and row. The odd column indices are addressed after 1/2 of the
// * groups have been processed. The two skipped odd row indices
// * are processed after two even rows are processed.
// *
// */
//
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

    if (g_1d>=(ngi*ngj)/4)
    {
        offset0=1;
        if (g_1d>=(ngi*ngj)/2)
        {
            offset0=0;
            offset1=1;
            if (g_1d>=3*(ngi*ngj)/4)
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

void map_work_space_2d_offset(int* mapped_id0,
                              int* mapped_id1,
                              int offset0,
                              int offset1)
{
    unsigned lid0 = get_local_id(0);
    unsigned lid1 = get_local_id(1);
    int group_id0 = get_group_id(0), group_id1 = get_group_id(1);

    /* map the group id to permuted 2d coordinates */
    int ls0 = get_local_size(0), ls1 = get_local_size(1);

    /* map the individual bundle coordinates */
    (*mapped_id0) = (2*group_id0+offset0)*ls0 + lid0;
    (*mapped_id1) = (2*group_id1+offset1)*ls1 + lid1;
}

#if 0
// Determine initial ray bundle entry points for the entire tree bounding box
// This operation can be done by all the work items in parallel. Note that
// the ray direction is the same in global and local cell coordinates,
// since the mapping is an isotropic scaling. The ray is returned with
// the side effect that the exit point is inserted in the local cache
//
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
#endif

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
                 __local uchar4*  ray_bundle_array,
                 __local float4*  cached_aux_data)
{
    /* linear thread id */
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    /* limit access to threads that do not own a connected region */
    if (ray_bundle_array[llid].x!=llid)
    {
        /* store seg_len in the corresponding aux data slot to be accessed
        * by other threads since these aux_data items are not used, they
        * can be cleared and used to store the seg_len of non-owner rays
        */
        cached_aux_data[llid]=(float4)0.0f;
        cached_aux_data[llid].x = seg_len;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    uchar temp = ray_bundle_array[llid].w & ACTIVE;
    /* now, limit access to the threads that own each connected region */
    if (ray_bundle_array[llid].x==llid &&temp)
    {
        /* The region owner (base) is now the only active thread within the region*/
        /* process the base ray */
        cached_aux_data[llid].x += seg_len; /* seg_len sum */
        /* weighted observations */
        cached_aux_data[llid].y += (image_vect[llid].x)*seg_len;
        uchar adr = llid;/* linked list pointer */
        /* traverse the linked list and increment sums */
        temp = ray_bundle_array[adr].w & NEXT_ADR_VALID;
        while ( temp > 0)
        {
            adr = ray_bundle_array[adr].y;
            cached_aux_data[llid].x += cached_aux_data[adr].x;
            cached_aux_data[llid].y += (image_vect[adr].x)*cached_aux_data[adr].x;
            temp = ray_bundle_array[adr].w & NEXT_ADR_VALID;
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
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
                  __local float16*  cached_data,
                  __local float4*  cached_aux_data)
{
    /* linear thread id */
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    cached_data[llid].sd = seg_len;
    barrier(CLK_LOCAL_MEM_FENCE); /*wait for all threads to complete */
    float temp1 = 0.0f, temp2 = 0.0f; /* minimize registers */
    /* now, limit access to the threads that own each connected region */
    bool temp = ray_bundle_array[llid].w & ACTIVE;
    if (ray_bundle_array[llid].x==llid && temp)
    {
        /* if total length of rays is too small, do nothing */
        temp1 = cached_aux_data[llid].x; /* length sum */
        if (temp1>1.0e-10f)
        {
            /* The mean intensity for the cell */
            temp2 = cached_aux_data[llid].y/temp1; /* mean observation */
            temp1 = gauss_3_mixture_prob_density(temp2,
                cached_data[llid].s1,
                cached_data[llid].s2,
                cached_data[llid].s3,
                cached_data[llid].s5,
                cached_data[llid].s6,
                cached_data[llid].s7,
                cached_data[llid].s9,
                cached_data[llid].sa,
                (1.0f-cached_data[llid].s3
                -cached_data[llid].s7)
                );/* PI */
            /* temporary slot to store PI*/
            cached_data[llid].se =temp1;
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE); /*wait for all threads to complete */

    /* Below, all active threads participate in updating the pre-vis images */
    if (temp) {
        /* pointer to the cached cell data for this ray */
        char adr = ray_bundle_array[llid].x;

        /*alpha integral          alpha           *        seg_len      */
        image_vect[llid].y += cached_data[adr].s0*cached_data[llid].sd;

        temp2 = exp(-image_vect[llid].y); /* vis_prob_end */

        /* updated pre                      Omega         *       PI         */
        image_vect[llid].w += (image_vect[llid].z - temp2)*cached_data[adr].se;
        /* updated visibility probability */
        image_vect[llid].z = temp2;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
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
//__kernel void proc_norm_image(__global float4* image, __global float4* p_inf)
//{
//    /* linear global id of the normalization image */
//    int lgid = get_global_id(0) + get_global_size(0)*get_global_id(1);
//    float4 vect = image[lgid];
//    float mult = (p_inf[0].x>0.0f) ? 1.0f :
//        gauss_prob_density(vect.x, p_inf[0].y, p_inf[0].z);
//    /* compute the norm image */
//    vect.x = vect.w + mult * vect.z;
//    /* the following  quantities have to be re-initialized before
//    *the bayes_ratio kernel is executed
//    */
//    vect.y = 0.0f;/* clear alpha integral */
//    vect.z = 1.0f; /* initial vis = 1.0 */
//    vect.w = 0.0f; /* initial pre = 0.0 */
//    /* write it back */
//    image[lgid] = vect;
//}

__kernel void proc_norm_image(__global float4* image, __global float4* p_inf,__global uint4   * imgdims)
{
    /* linear global id of the normalization image */
    int lgid = get_global_id(0) + get_global_size(0)*get_global_id(1);

    int i=0;
    int j=0;
    map_work_space_2d(&i,&j);

    if (i>=(*imgdims).z && j>=(*imgdims).w)
        return;

    float4 vect = image[j*get_global_size(0)+i];
    float mult = (p_inf[0].x>0.0f) ? 1.0f :
        gauss_prob_density(vect.x, p_inf[0].y, p_inf[0].z);
    /* compute the norm image */
    vect.x = vect.w + mult * vect.z;
    /* the following  quantities have to be re-initialized before
    *the bayes_ratio kernel is executed
    */
    vect.y = 0.0f;/* clear alpha integral */
    vect.z = 1.0f; /* initial vis = 1.0 */
    vect.w = 0.0f; /* initial pre = 0.0 */
    /* write it back */
    image[j*get_global_size(0)+i] = vect;
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
                 __local float16*  cached_data,
                 __local float4*  cached_aux_data)
{
    /* linear thread id */
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    /* insert seg_len into the local cache. This step
    is carried out by all threads */
    cached_data[llid].sd = seg_len;
    barrier(CLK_LOCAL_MEM_FENCE);

    /* ray must be active after data is loaded */
    bool active = ray_bundle_array[llid].w & ACTIVE;

    float temp1 = 0.0f, temp2 = 0.0f; /* minimize registers */
    /*
    * if the ray is active and owner of the octree cell and the
    * total length of rays is large enough then compute PI
    *
    */
    if (active&& ray_bundle_array[llid].x==llid &&
        cached_aux_data[llid].x > 1.0e-10f) {    /* if  too small, do nothing */
            temp1 = cached_aux_data[llid].x; /* length sum */
            /* The mean intensity for the cell */
            temp2 = cached_aux_data[llid].y/temp1; /* mean observation */
            temp1 = gauss_3_mixture_prob_density(temp2,
                                                cached_data[llid].s1,
                                                cached_data[llid].s2,
                                                cached_data[llid].s3,
                                                cached_data[llid].s5,
                                                cached_data[llid].s6,
                                                cached_data[llid].s7,
                                                cached_data[llid].s9,
                                                cached_data[llid].sa,
                                                (1.0f-cached_data[llid].s3
                                                -cached_data[llid].s7)
                                                );/* PI */
            /* temporary slot to store PI*/
            cached_data[llid].se = temp1;
    }
    barrier(CLK_LOCAL_MEM_FENCE); /*wait for all threads to complete */
    /* Below, all active threads participate in updating the alpha integral,
    * pre and vis images */
    //active = ray_bundle_array[llid].w & ACTIVE;
    if (active) {
        /* pointer to the cached cell data for this ray */
        uchar adr = ray_bundle_array[llid].x;

        /*alpha integral          alpha           *        seg_len      */
        image_vect[llid].y += cached_data[adr].s0*cached_data[llid].sd;

        temp2 = exp(-image_vect[llid].y); /* vis_prob_end */

        /* updated pre                      Omega         *       PI         */
        image_vect[llid].w += (image_vect[llid].z - temp2)*cached_data[adr].se;

        /* updated visibility probability */
        image_vect[llid].z = temp2;
    }

    barrier(CLK_LOCAL_MEM_FENCE); /*wait for all threads to complete */


    /* region owner scans the region and computes Bayes ratio and weighted vis.
    * The aux_data cell elements are assigned as:
    *
    *   [seg_len sum | weighted obs | update_ratio (beta) | weighted vis]
    *
    */
    //  active = ray_bundle_array[llid].w & ACTIVE;
    if (ray_bundle_array[llid].x==llid && active) {
        /* compute data for base ray cell */
        /*   cell.vis         +=        vis(i,j)    *      seg_len */
        cached_aux_data[llid].w += image_vect[llid].z*cached_data[llid].sd;
        /* Bayes ratio */
        /* ( pre + PI*vis)/norm)*seg_len */
        if(image_vect[llid].x>1e-10f)
        {
            cached_aux_data[llid].z +=
                /*      pre(i,j)        +        PI        *       vis(i,j) */
                ((image_vect[llid].w + cached_data[llid].se*image_vect[llid].z)/image_vect[llid].x)*cached_data[llid].sd;
        }
        /*     norm(i,j)        seg_len */
        /* If ray has no neighbors - then just return */
        uchar next_adr_valid = ray_bundle_array[llid].w & NEXT_ADR_VALID;
        uchar adr = llid; /* linked list pointer */
        while (next_adr_valid>0) {
            adr = ray_bundle_array[adr].y;/* follow the linked list */

            /*    cell.vis           +=     vis(i,j)      *      seg_len     */
            cached_aux_data[llid].w += image_vect[adr].z*cached_data[adr].sd;
            /* Bayes ratio */
            /* ( pre + PI*vis)/norm)*seg_len */
            if( image_vect[adr].x>1e-10f)
            {

                cached_aux_data[llid].z +=
                    ((image_vect[adr].w + cached_data[llid].se*image_vect[adr].z)/
                    image_vect[adr].x)*cached_data[adr].sd;
            }
            next_adr_valid = ray_bundle_array[adr].w & NEXT_ADR_VALID;
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE); /*wait for all threads to complete */
}



void update_cell(float16 * data, float4 aux_data,float t_match, float init_sigma, float min_sigma)
{
    if (aux_data.x>1e-5)
    {
        float mu0 = (*data).s1, sigma0 = (*data).s2, w0 = (*data).s3;
        float mu1 = (*data).s5, sigma1 = (*data).s6, w1 = (*data).s7;
        float mu2 = (*data).s9, sigma2 = (*data).sa;
        float w2=0.0f;


        if(w0>0.0f && w1>0.0f)
            w2=1-(*data).s3-(*data).s7;

        short Nobs0 = (short)(*data).s4,
              Nobs1 = (short)(*data).s8,
              Nobs2 = (short)(*data).sb;
        float Nobs_mix = (*data).sc;

        update_gauss_3_mixture(aux_data.y/aux_data.x,
                               aux_data.w/aux_data.x,
                               t_match,
                               init_sigma,min_sigma,
                               &mu0,&sigma0,&w0,&Nobs0,
                               &mu1,&sigma1,&w1,&Nobs1,
                               &mu2,&sigma2,&w2,&Nobs2,
                               &Nobs_mix);
        (*data).s0*=aux_data.z/aux_data.x;
        (*data).s1=mu0; (*data).s2=sigma0, (*data).s3=w0;(*data).s4=(float)Nobs0;
        (*data).s5=mu1; (*data).s6=sigma1, (*data).s7=w1;(*data).s8=(float)Nobs1;
        (*data).s9=mu2; (*data).sa=sigma2, (*data).sb=(float)Nobs2;
        (*data).sc=(float)Nobs_mix;
    }
}
