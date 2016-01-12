////////////////////////////////////////////////////////////////////////////////
// Adaptive Cast Cone Ray
// Main cast ray function for fixed grid of shallow octree ray tracing.
//
// Note: This is a bare-bones skeleton for ray tracing.  It is necessary to
// define two preprocessor variables for this to do anything useful:
//    - AUX_ARGS : list of additional arguments to to this function (for instance data arrays)
//    - STEP_CELL : function and signature to be substituted in at the voxel level
////////////////////////////////////////////////////////////////////////////////

//example defines (passed in on C++ side)
//#define STEP_CELL step_cell_render(aux_args.mixture_array,aux_args.alpha_array,data_ptr,d,vis,aux_args.expected_int);
#define BLOCK_EPSILON .006125f
#define TREE_EPSILON  .005f
#define MIN_T .1f
#define UNIT_SPHERE_RADIUS 0.6203504908994f // = 1/vcl_cbrt(vnl_math::pi*4/3);
#define MAX_RAY_LEVEL 4
#define SPLIT_FACTOR 1.0f

////////////////////////////////////////////////////////////////////////////////
//Given Ray pyramid and scene bounding box, fills out T value pyramids
////////////////////////////////////////////////////////////////////////////////
inline void intersect_block_pyramid(AuxArgs* aux_args, __constant RenderSceneInfo* linfo, float4 ray_o)
{
  //get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar localI = (uchar)get_local_id(0);
  uchar localJ = (uchar)get_local_id(1);

  int offset = 1;
  for(int curr_level=3; curr_level>=0; --curr_level) {
    //intersect appropriate rays
    if(localI%offset==0 && localJ%offset==0) {
      float tFar, tblock;
      float4 ray_d = ray_pyramid_access( aux_args->rays, curr_level, localI/offset, localJ/offset);
      intersect_block(ray_o, ray_d, linfo, &tFar, &tblock);
      tblock = max(tblock, MIN_T); //(tblock > 0.0f) ? tblock : 0.0f;    //make sure tnear is at least 0...
      tFar -= BLOCK_EPSILON;                       //xit surpasses it (and breaks from the outer loop)

      //set the tnear/tfar pyramid
      image_pyramid_set( aux_args->tfar, curr_level, localI/offset, localJ/offset, tFar);
    }
    offset+=offset; //offset = offset*2
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  //----RAY PYRAMID and T PYRAMID DEBUG CODE------------------------------------
  //set lowest level
  // The goal here is to have a single thread (id 0) intersect the scene with
  // the fattest ray.
  float tFar, currT;
  float4 ray_d = ray_pyramid_access(aux_args->rays, 0, 0, 0);  //gets the fattest ray
  intersect_block(ray_o, ray_d, linfo, &tFar, &currT);
  tFar -= BLOCK_EPSILON;
  aux_args->tfar->pyramid[0][0] = tFar;
  barrier(CLK_LOCAL_MEM_FENCE);

  //intersect block for t-near - just need to do lowest level one first
  int ray_level = aux_args->active_rays[llid]-1;
  if(ray_level >= 0) {
    float tFar, currT;
    float4 ray_d = ray_pyramid_access(aux_args->rays, 0, 0, 0);  //gets the fattest ray
    intersect_block(ray_o, ray_d, linfo, &tFar, &currT);
    aux_args->currT[llid] = max(currT, MIN_T);
  }
  barrier(CLK_LOCAL_MEM_FENCE);
}

//tfar
inline float active_t_far( AuxArgs* aux_args )
{
  //get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar localI = (uchar)get_local_id(0);
  uchar localJ = (uchar)get_local_id(1);

  //index into this thread's leader
  uchar thread_leader = aux_args->master_threads[llid];
  uchar level = aux_args->active_rays[thread_leader] - 1; //0 is reserved for inactive threads

  //offset (the amount to divide localI and localJ to get correctly indexed pyramid i,j)
  uchar offset = 1<<(3-level);
  float tfar = image_pyramid_access( aux_args->tfar, level, localI/offset, localJ/offset);
  return tfar;
}

//helper method that computes local index + positive offset
inline uchar localIndex( uchar i_offset, uchar j_offset )
{
  return (uchar) ( (get_local_id(0)+i_offset) +
                    get_local_size(0)* (get_local_id(1)+j_offset) );
}

//helper computes pixel level visibility
inline float compute_pixel_vis(__local uchar* masters, __local uchar* active_rays, __local float* vis)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  //get thread leader and the leader's level
  uchar thread_leader = masters[llid];
  int ray_level = active_rays[thread_leader]-1;

  //choose exponent factor = .25 ^ (3-level)
  float exp_factor = 1.0f;
  if(ray_level==0)
    exp_factor = .25f*.25f*.25f;
  else if(ray_level==1)
    exp_factor = .25f*.25f;
  else if(ray_level==2)
    exp_factor = .25f;

  //calc active ones first
  ray_level = active_rays[llid] - 1;
  if(ray_level >= 0 && ray_level < 3) {   //don't need to operate on finest rays
    float tempVis = vis[llid];
    vis[llid] = pow(tempVis, exp_factor);
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  //store all rays now
  vis[llid] = vis[thread_leader];
  barrier(CLK_LOCAL_MEM_FENCE);
}

//reads current T based on master thread
inline float read_currT(__local float* t, __local uchar* master_threads)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar thread_leader = master_threads[llid];
  return t[thread_leader];
}

void cast_adaptive_cone_ray(
                            //---- RAY ARGUMENTS -------------------------------------------------
                            int i, int j,                                     //pixel information
                            float4 ray_o,                                     //ray origin
                            float4 ray_d,                                     //ray direction + half angle in the w

                            //---- SCENE ARGUMENTS------------------------------------------------
                            __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
                            __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16

                            //---- UTILITY ARGUMENTS----------------------------------------------
                            __local     uchar16            * local_tree,      //local tree for traversing (8x8 matrix)
                            __constant  uchar              * bit_lookup,      //0-255 num bits lookup table
                            __constant  float              * centerX,         // center points for ...
                            __constant  float              * centerY,         // each of the 585 possible cells ...
                            __constant  float              * centerZ,         // indexed by bit index
                            __local     uchar              * cumsum,          //cumulative sum helper for data pointer
                            __local     uchar              * visit_list,      //visit list for BFS, uses 10 chars per thread

                            //----aux arguments defined by host at compile time-------------------
                            AuxArgs aux_args )
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  uchar localI = (uchar)get_local_id(0);
  uchar localJ = (uchar)get_local_id(1);

  //cache these three pointers
  __local uchar16* ltree = &local_tree[llid];
  __local uchar*   csum  = &cumsum[llid*10];
  __local uchar*   listMem = &visit_list[llid*73];

  //cell spheres have the same volume as blocks
  //float volume_scale = linfo->block_len; //*linfo->block_len*linfo->block_len;

  //////////////////////////////////////////////////////////////////////////////
  // Compute pyramid values for tnear/tfar
  //////////////////////////////////////////////////////////////////////////////
  //calc tnear tfar for tpyramids
  intersect_block_pyramid( &aux_args, linfo, ray_o );
  float currT, currR, tFar, sinAlpha;
  tFar = active_t_far( &aux_args );
  currT = read_currT(aux_args.currT, aux_args.master_threads);

  //////////////////////////////////////////////////////////////////////////////
  // Begin block ray trace
  //////////////////////////////////////////////////////////////////////////////
  float count=0.0f; //debug counter
  int safety=0; int cnt=0;
  while ( currT < tFar && ++safety < 100000 )
  {
    ///////////////////////////////////////////////////////////////////////////
    // 0. grab currently activated rays, and compute their spheres
    ///////////////////////////////////////////////////////////////////////////
    int ray_level = aux_args.active_rays[llid]-1;  //0=fatest, 1=next, .., 3=finest
    if(ray_level >= 0 && ray_level < 3) {

      //intersect the current sphere with
      float4 currRayD = ray_pyramid_access_safe(aux_args.rays, ray_level);
      float4 currSphere = (float4) ( ray_o + currT * currRayD );

      //calc and store the sphere's radius
      sinAlpha = fabs( sin(currRayD.w) );
      currR = currT * sinAlpha;
      currSphere.w = currR;

      //minimum/maximum subblock eclipsed
      int4 minCell = (int4) ( convert_int( clamp(currSphere.x - currR, 0.0f, linfo->dims.x-1.0f) ),
                              convert_int( clamp(currSphere.y - currR, 0.0f, linfo->dims.y-1.0f) ),
                              convert_int( clamp(currSphere.z - currR, 0.0f, linfo->dims.z-1.0f) ), 0 );
      int4 maxCell = (int4) ( convert_int( clamp(currSphere.x + currR+1, 0.0f, linfo->dims.x-1.0f) ),
                              convert_int( clamp(currSphere.y + currR+1, 0.0f, linfo->dims.y-1.0f) ),
                              convert_int( clamp(currSphere.z + currR+1, 0.0f, linfo->dims.z-1.0f) ), 0 );
      //count = (1+maxCell.x-minCell.x)*(1+maxCell.y-minCell.y)*(1+maxCell.z-minCell.z);

      //-----------------------------------------------------------------------
      //update active ray matrix
      //-----------------------------------------------------------------------
      bool split=false;  //split ray?
      for (int x=minCell.x; x<maxCell.x; ++x) {
        for (int y=minCell.y; y<maxCell.y; ++y) {
          for (int z=minCell.z; z<maxCell.z; ++z) {

            ////////////////////////////////////////////////////////////////////
            // Begin octree traversal
            //load current block/tree, initialize cumsum buffer and cumIndex
            int blkIndex = calc_blkInt(x, y, z, linfo->dims);
            (*ltree) = as_uchar16(tree_array[blkIndex]);
            csum[0] = (*ltree).s0;
            int cumIndex = 1;

            //visit list for BFS through tree (denotes parents of cells that ought to be visited)
            linked_list toVisit = new_linked_list(listMem, 73);
            push_back( &toVisit, -1 );

            /////////////////////////////////////////////////////////////////////
            //list keeps track of parents whose children need to be intersected
            //saves 8xSpace in local memory
            unsigned deepest_gen = linfo->root_level;
            int max_cell = ((1 << (3*(deepest_gen+1))) - 1) / 7;
            max_cell = min(585, max_cell);

            //iterate through tree if there are children to get to
            while ( !empty(&toVisit) )
            {
              //get front node off the top of the list, do an intersection for all 8 children
              int pindex = (int) pop_front( &toVisit );
              for(int currBitIndex=8*pindex + 1; currBitIndex < 8*pindex + 9; ++currBitIndex) {
                if(currBitIndex < 0) continue; //don't opeerate on pre root children

                //calculate the theoretical radius of this cell
                int curr_depth = get_depth(currBitIndex);
                float side_len = 1.0f / (float) (1<<curr_depth);

                 //intersect the cell,
                float4 cellSphere = (float4) ( (float) x + centerX[currBitIndex],
                                               (float) y + centerY[currBitIndex],
                                               (float) z + centerZ[currBitIndex],
                                               UNIT_SPHERE_RADIUS * side_len );
                float intersect_volume =  sphere_intersection_volume(currSphere, cellSphere);

                //if it intersects, do one of two things
                if ( intersect_volume > 0.0f ) {
                  //if the tree is a leaf, then update it's contribution
                  if ( tree_bit_at(ltree, currBitIndex) == 0 ) {
                    if(currR > SPLIT_FACTOR*UNIT_SPHERE_RADIUS*side_len && ray_level < 3 /*MAX_RAY_LEVEL-1*/)
                      split=true;
                  }
                  else {
                    push_back( &toVisit, currBitIndex );  //add children to the visit list
                  }
                }

              } //end child for loop
            } //end BFS while
            // end Octree traversal portion of cone ray trace
            ////////////////////////////////////////////////////////////////////

          } //end z for
        } //end y for
      } //end x for

      //----SPLIT-----//
      int side_len = 1<<(3-ray_level); //sqrt of number of threads this current active ray controls
      if(split) {
        count=ray_level+1;

        //calc prev sphere, then split that sphere
        float prevR, prevT, splitR, splitT;
        prev_sphere(currT, currR, &prevT, &prevR);
        calc_cone_split(prevR, prevT, currRayD.w, &splitR, &splitT);
        currR = splitR;
        currT = splitT;

        //turn on the four neighboring threads, split ray matrix information
        int next_level = ray_level+1;
        for(int ioff=0; ioff<2; ++ioff) {
          for(int joff=0; joff<2; ++joff) {

            //"neighbor" threads are not necessarily neighboring in workspace (only at finest level they are)
            int di = ioff * (side_len/2);
            int dj = joff * (side_len/2);

            //calc local thread ID (in 8x8 workspace)
            uchar id = (localI+di) + (localJ+dj)*get_local_size(0);
            aux_args.active_rays[id] = next_level + 1; //ray level is stored+1, then add one more for increase in level
            aux_args.master_threads[id] = id;

            //store splitT
            aux_args.currT[id] = splitT;

            //---- maybe can do this with all threads, set master thread matrix
            for(int ii=0; ii<side_len/2; ++ii) {
              for(int jj=0; jj<side_len/2; ++jj) {
                uchar slid= (localI+di+ii) + (localJ+dj+jj)*get_local_size(0);
                aux_args.master_threads[slid] = id;
              }
            }
            //------------------------------------------------------------------
          } //end i for
        } //end j for
        barrier(CLK_LOCAL_MEM_FENCE);

        //Must split some aux args as well
        split_ray(aux_args, side_len);

      } // END SPLIT IF
    } //end ray_level if (is_active)
    barrier(CLK_LOCAL_MEM_FENCE);

    ///////////////////////////////////////////////////////////////////////////
    // 1. With new Active Ray Matrix,
    // pass over all cells intersecting this ball, compute step_CELL
    ///////////////////////////////////////////////////////////////////////////
    ray_level = aux_args.active_rays[llid]-1;  //0=fatest, 1=next, .., 3=finest
    float gamma_integral=0.0f;
    if(ray_level >= 0) {

      //recompute sphere (as this may have become active)
      currT = read_currT(aux_args.currT,aux_args.master_threads);
      float4 currRayD = ray_pyramid_access_safe(aux_args.rays, ray_level);
      float4 currSphere = (float4) ( ray_o + currT * currRayD );

      //calc and store the sphere's radius
      sinAlpha = fabs( sin(currRayD.w) );
      currR = currT * sinAlpha;
      currSphere.w = currR;

      //minimum/maximum subblock eclipsed
      int4 minCell = (int4) ( convert_int( clamp(currSphere.x - currR, 0.0f, linfo->dims.x-1.0f) ),
                              convert_int( clamp(currSphere.y - currR, 0.0f, linfo->dims.y-1.0f) ),
                              convert_int( clamp(currSphere.z - currR, 0.0f, linfo->dims.z-1.0f) ), 0 );
      int4 maxCell = (int4) ( convert_int( clamp(currSphere.x + currR+1, 0.0f, linfo->dims.x-1.0f) ),
                              convert_int( clamp(currSphere.y + currR+1, 0.0f, linfo->dims.y-1.0f) ),
                              convert_int( clamp(currSphere.z + currR+1, 0.0f, linfo->dims.z-1.0f) ), 0 );

      for (int x=minCell.x; x<maxCell.x; ++x) {
        for (int y=minCell.y; y<maxCell.y; ++y) {
          for (int z=minCell.z; z<maxCell.z; ++z) {

            //load current block/tree, initialize cumsum buffer and cumIndex
            int blkIndex = calc_blkInt(x, y, z, linfo->dims);
            (*ltree) = as_uchar16(tree_array[blkIndex]);
            csum[0] = (*ltree).s0;
            int cumIndex = 1;

            //visit list for BFS through tree (denotes parents of cells that ought to be visited)
            linked_list toVisit = new_linked_list(listMem, 73);
            push_back( &toVisit, -1 );

            /////////////////////////////////////////////////////////////////////
            //list keeps track of parents whose children need to be intersected
            //saves 8xSpace in local memory
            unsigned deepest_gen = linfo->root_level;
            int max_cell = ((1 << (3*(deepest_gen+1))) - 1) / 7;
            max_cell = min(585, max_cell);

            //iterate through tree if there are children to get to
            cnt=0;
            while ( !empty(&toVisit) )
            {
              ++cnt;
              //get front node off the top of the list, do an intersection for all 8 children
              int pindex = (int) pop_front( &toVisit );
              for(int currBitIndex=8*pindex + 1; currBitIndex < 8*pindex + 9; ++currBitIndex) {
                if(currBitIndex < 0) continue;

                //calculate the theoretical radius of this cell
                int curr_depth = get_depth(currBitIndex);
                float side_len = 1.0f / (float) (1<<curr_depth);

                //intersect the cell,
                float4 cellSphere = (float4) ( (float) x + centerX[currBitIndex],
                                               (float) y + centerY[currBitIndex],
                                               (float) z + centerZ[currBitIndex],
                                               UNIT_SPHERE_RADIUS * side_len );
                float intersect_volume =  sphere_intersection_volume(currSphere, cellSphere);

                //if it intersects, do one of two things
                if ( intersect_volume > 0.0f ) {
                  //if the tree is a leaf, then update it's contribution
                  if ( tree_bit_at(ltree, currBitIndex) == 0 ) {
                    //data index is relative data (data_index_cached) plus data_index_root
                    int data_ptr = data_index_cached(ltree, currBitIndex, bit_lookup, csum, &cumIndex) + data_index_root(ltree);
                    //intersect_volume = intersect_volume*volume_scale/side_len/side_len;
                    STEP_CELL; // replaced by:step_cell_cone(aux_args, data_ptr, intersect_volume, side_len * linfo->block_len);
                    //gamma_integral +=  aux_args.alphas[data_ptr]*intersect_volume*volume_scale/side_len/side_len;;
                  }
                  else {
                    push_back( &toVisit, currBitIndex );  //add children to the visit list
                  }
                }

              } //end child for loop
            } //end BFS while
            // end step CELL portion of cone ray trace
            ////////////////////////////////////////////////////////////////////

          } //end z for
        } //end y for
      } //end x for
    } //end ray_level if (is_active)

    ///////////////////////////////////////////////////////////////////////////
    // 2. calculate ball properties
    ///////////////////////////////////////////////////////////////////////////
    //replaced by something like: compute_ball_properties(aux_args);
    //THIS IS ACTIVE FOR ALL RAYS
    COMPUTE_BALL_PROPERTIES;
    barrier(CLK_LOCAL_MEM_FENCE);

    ///////////////////////////////////////////////////////////////////////////
    // 3. redistribute data loop - used to redistribute information
    // gleaned from ball/cell back to the cells
    ///////////////////////////////////////////////////////////////////////////
#ifdef REDISTRIBUTE
    ray_level = aux_args.active_rays[llid]-1;  //0=fatest, 1=next, .., 3=finest
    if(ray_level >= 0) {

      //recompute sphere (as this may have become active)
      currT = read_currT(aux_args.currT,aux_args.master_threads);
      float4 currRayD = ray_pyramid_access_safe(aux_args.rays, ray_level);
      float4 currSphere = (float4) ( ray_o + currT * currRayD );

      //calc and store the sphere's radius
      sinAlpha = fabs( sin(currRayD.w) );
      currR = currT * sinAlpha;
      currSphere.w = currR;

      //minimum/maximum subblock eclipsed
      int4 minCell = (int4) ( convert_int( clamp(currSphere.x - currR, 0.0f, linfo->dims.x-1.0f) ),
                              convert_int( clamp(currSphere.y - currR, 0.0f, linfo->dims.y-1.0f) ),
                              convert_int( clamp(currSphere.z - currR, 0.0f, linfo->dims.z-1.0f) ), 0 );
      int4 maxCell = (int4) ( convert_int( clamp(currSphere.x + currR+1, 0.0f, linfo->dims.x-1.0f) ),
                              convert_int( clamp(currSphere.y + currR+1, 0.0f, linfo->dims.y-1.0f) ),
                              convert_int( clamp(currSphere.z + currR+1, 0.0f, linfo->dims.z-1.0f) ), 0 );
      for (int x=minCell.x; x<maxCell.x; ++x) {
        for (int y=minCell.y; y<maxCell.y; ++y) {
          for (int z=minCell.z; z<maxCell.z; ++z) {

            //load current block/tree, initialize cumsum buffer and cumIndex
            int blkIndex = calc_blkInt(x, y, z, linfo->dims);
            (*ltree) = as_uchar16(tree_array[blkIndex]);
            csum[0] = (*ltree).s0;
            int cumIndex = 1;

            //visit list for BFS through tree (denotes parents of cells that ought to be visited)
            linked_list toVisit = new_linked_list(listMem, 73);
            push_back( &toVisit, -1 );

            /////////////////////////////////////////////////////////////////////
            //list keeps track of parents whose children need to be intersected
            //saves 8xSpace in local memory
            unsigned deepest_gen = linfo->root_level;
            int max_cell = ((1 << (3*(deepest_gen+1))) - 1) / 7;
            max_cell = min(585, max_cell);

            //iterate through tree if there are children to get to
            while ( !empty(&toVisit) )
            {
              //get front node off the top of the list, do an intersection for all 8 children
              int pindex = (int) pop_front( &toVisit );
              for(int currBitIndex=8*pindex + 1; currBitIndex < 8*pindex + 9; ++currBitIndex) {
                if(currBitIndex < 0) continue;

                //calculate the theoretical radius of this cell
                int curr_depth = get_depth(currBitIndex);
                float side_len = 1.0f / (float) (1<<curr_depth);

                //intersect the cell,
                float4 cellSphere = (float4) ( (float) x + centerX[currBitIndex],
                                               (float) y + centerY[currBitIndex],
                                               (float) z + centerZ[currBitIndex],
                                               UNIT_SPHERE_RADIUS * side_len );
                float intersect_volume =  sphere_intersection_volume(currSphere, cellSphere);

                //if it intersects, do one of two things
                if ( intersect_volume > 0.0f ) {
                  //if the tree is a leaf, then update it's contribution
                  if ( tree_bit_at(ltree, currBitIndex) == 0 ) {
                    //data index is relative data (data_index_cached) plus data_index_root
                    int data_ptr = data_index_cached(ltree, currBitIndex, bit_lookup, csum, &cumIndex) + data_index_root(ltree);
                    REDISTRIBUTE; // step_cell_cone(aux_args, data_ptr, intersect_volume, side_len * linfo->block_len,&intensity_norm, &weighted_int, &prob_surface);
                  }
                  else {
                    push_back( &toVisit, currBitIndex );  //add children to the visit list
                  }
                }

              } //end child for loop
            } //end BFS while
            // end step CELL portion of cone ray trace
            ////////////////////////////////////////////////////////////////////

        } //end z for
      } //end y for
    } //end x for
  } // end active if
#endif //REDISTRIBUTE

    ////////////////////////////////////////////////////////////////////////////
    // 4. Update active T values
    ////////////////////////////////////////////////////////////////////////////
    ray_level = aux_args.active_rays[llid]-1;  //0=fatest, 1=next, .., 3=finest
    if(ray_level >= 0) {

      //read in currT, currR was previously set in the last loop
      currT = read_currT(aux_args.currT,aux_args.master_threads);
      float nextT, nextR;
      next_sphere(currT, currR, &nextT, &nextR);

      //set t value in local memory for all threads to grab...
      aux_args.currT[llid] = nextT;
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    //each thread needs to read their new T value
    tFar = active_t_far( &aux_args );
    currT = read_currT(aux_args.currT,aux_args.master_threads);
  } //end ray trace while loop

  //stores pixel vis across all pixels
  //compute_pixel_vis(aux_args.master_threads,aux_args.active_rays,aux_args.vis);
  //aux_args.vis[llid] = safety;
}
