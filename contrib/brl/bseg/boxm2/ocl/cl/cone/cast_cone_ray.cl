////////////////////////////////////////////////////////////////////////////////
// Cast Ray Bit
// Main cast ray function for fixed grid of shallow octree ray tracing.
//
// Note: This is a bare-bones skeleton for ray tracing.  It is necessary to
// define two preprocessor variables for this to do anything useful:
//    - AUX_ARGS : list of additional arguments to to this function (for instance data arrays)
//    - STEP_CELL : function and signature to be substituted in at the voxel level
//
// This function also depends on 'bit_tree_library_functions.cl'
////////////////////////////////////////////////////////////////////////////////

//example defines (passed in on C++ side)
//#define STEP_CELL step_cell_render(aux_args.mixture_array,aux_args.alpha_array,data_ptr,d,vis,aux_args.expected_int);
#define BLOCK_EPSILON .006125f
#define TREE_EPSILON  .005f
#define MIN_T 1.0f
#define UNIT_SPHERE_RADIUS 0.6203504908994f // = 1/vcl_cbrt(vnl_math::pi*4/3);

void cast_cone_ray(
                    //---- RAY ARGUMENTS -------------------------------------------------
                    int i, int j,                                     //pixel information
                    float ray_ox, float ray_oy, float ray_oz,         //ray origin
                    float ray_dx, float ray_dy, float ray_dz,         //ray direction
                    float cone_half_angle,

                    //---- SCENE ARGUMENTS------------------------------------------------
                    __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
                    __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16

                    //---- UTILITY ARGUMENTS----------------------------------------------
                    __local     uchar16            * local_tree,      //local tree for traversing
                    __constant  uchar              * bit_lookup,      //0-255 num bits lookup table

                    __constant  float              * centerX,
                    __constant  float              * centerY,
                    __constant  float              * centerZ,

                    __local     uchar              * cumsum,          //cumulative sum helper for data pointer
                    __local     uchar              * visit_list,      //visit list for BFS, uses 10 chars per thread
                                float              * vis,             //passed in as starting visibility

                    //----aux arguments defined by host at compile time-------------------
                    AuxArgs aux_args )
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  //cache these three pointers
  __local uchar16* ltree = &local_tree[llid];
  __local uchar*   csum  = &cumsum[llid*10];
  __local uchar*   listMem = &visit_list[llid*73];

  //cell spheres have the same volume as blocks
  float volume_scale = linfo->block_len*linfo->block_len*linfo->block_len;

  //determine the minimum face:
  //get parameters tnear and tfar for the scene
  float max_facex = (ray_dx > 0.0f) ? (linfo->dims.x) : 0.0f;
  float max_facey = (ray_dy > 0.0f) ? (linfo->dims.y) : 0.0f;
  float max_facez = (ray_dz > 0.0f) ? (linfo->dims.z) : 0.0f;
  float tFar = calc_tfar(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, max_facex, max_facey, max_facez);
  float min_facex = (ray_dx < 0.0f) ? (linfo->dims.x) : 0.0f;
  float min_facey = (ray_dy < 0.0f) ? (linfo->dims.y) : 0.0f;
  float min_facez = (ray_dz < 0.0f) ? (linfo->dims.z) : 0.0f;
  float tblock = calc_tnear(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, min_facex, min_facey, min_facez);
  tblock = (tblock > 0.0f) ? tblock : 0.0f;    //make sure tnear is at least 0...
  tFar -= BLOCK_EPSILON;   //make sure tfar is within the last block so texit surpasses it (and breaks from the outer loop)
  if (tFar <= tblock)
    return;

  //put ray in two float 4s for now
  float4 ray_o = (float4) (ray_ox, ray_oy, ray_oz, 0.0f);
  float4 ray_d = (float4) (ray_dx, ray_dy, ray_dz, 0.0f);

  //calculate sinAlpha (or radius if alpha is 0 depending on camera model...)
  float sinAlpha = fabs(sin(cone_half_angle));

  //make sure tnear is at least 0...
  float currT = max( tblock, MIN_T );

  //curr radius
  float currR = currT * sinAlpha;

  //iterate over spheres
  float count = 0;
  while (currT < tFar)
  {
    //if this pixel is no longer visible, quit
    //if ( (*vis) < .01f ) return;

    //intersect the current sphere with
    float4 currSphere = (float4) ( ray_o + currT * ray_d );
    currSphere.w = currR;

    //minimum/maximum subblock eclipsed
    int4 minCell = (int4) ( convert_int( clamp(currSphere.x - currR, 0.0f, linfo->dims.x-1.0f) ),
                            convert_int( clamp(currSphere.y - currR, 0.0f, linfo->dims.y-1.0f) ),
                            convert_int( clamp(currSphere.z - currR, 0.0f, linfo->dims.z-1.0f) ), 0 );
    int4 maxCell = (int4) ( convert_int( clamp(currSphere.x + currR+1, 0.0f, linfo->dims.x-1.0f) ),
                            convert_int( clamp(currSphere.y + currR+1, 0.0f, linfo->dims.y-1.0f) ),
                            convert_int( clamp(currSphere.z + currR+1, 0.0f, linfo->dims.z-1.0f) ), 0 );

    ///////////////////////////////////////////////////////////////////////////
    // 1. pass over all cells intersecting this ball
    ///////////////////////////////////////////////////////////////////////////
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

          /////////////////////////////////////////////////////////////////////
          //do an intersection with the root outside the loop
          //calculate the theoretical radius of this cell
          float side_len = 1.0f;
          float4 cellSphere = (float4) ( (float) x+.5f, (float) y+.5f, (float) z+.5f, UNIT_SPHERE_RADIUS );
          float  intersect_volume = sphere_intersection_volume(currSphere, cellSphere);

          //if it intersects, do one of two things
          if( intersect_volume > 0.0f ) {
            if( (*ltree).s0 == 0){
              int data_ptr = data_index_root(ltree);
              STEP_CELL; //step_cell_cone(aux_args, data_ptr, intersect_volume, side_len * linfo->block_len,&intensity_norm, &weighted_int, &prob_surface);
            }
            else { //push back root
              push_back( &toVisit, 0 );
            }
          }
          // done with first intersection - if nonzero volume, try children
          /////////////////////////////////////////////////////////////////////

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
                  // replaced by:step_cell_cone(aux_args, data_ptr, intersect_volume, side_len * linfo->block_len);
                  STEP_CELL;
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

    ///////////////////////////////////////////////////////////////////////////
    // 2. calculate ball properties
    ///////////////////////////////////////////////////////////////////////////
    //replaced by something like: compute_ball_properties(aux_args);
    COMPUTE_BALL_PROPERTIES;

    ///////////////////////////////////////////////////////////////////////////
    // 3. redistribute data loop - used to redistribute information
    // gleaned from ball/cell back to the cells
    ///////////////////////////////////////////////////////////////////////////
#ifdef REDISTRIBUTE
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

          /////////////////////////////////////////////////////////////////////
          //do an intersection with the root outside the loop
          //calculate the theoretical radius of this cell
          float side_len = 1.0f;
          float4 cellSphere = (float4) ( (float) x+.5f, (float) y+.5f, (float) z+.5f, UNIT_SPHERE_RADIUS );
          float  intersect_volume = sphere_intersection_volume(currSphere, cellSphere);

          //if it intersects, do one of two things
          if( intersect_volume > 0.0f ) {
            if( (*ltree).s0 == 0){
              int data_ptr = data_index_root(ltree);
              REDISTRIBUTE; //step_cell_cone(aux_args, data_ptr, intersect_volume, side_len * linfo->block_len,&intensity_norm, &weighted_int, &prob_surface);
            }
            else { //push back root
              push_back( &toVisit, 0 );
            }
          }
          // done with first intersection - if nonzero volume, try children
          /////////////////////////////////////////////////////////////////////

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
#endif //REDISTRIBUTE

    ////////////////////////////////////////////////////////////////////////////
    // 4. Continue iterating over the cone ray
    ////////////////////////////////////////////////////////////////////////////
    //calculate the next sphere's R and T
    float rPrime = sinAlpha * (currR + currT) / (1.0-sinAlpha);
    currT += (rPrime + currR);
    currR = rPrime;
  }
}

