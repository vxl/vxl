//Author: Octi Biris
//Kernel designed to warp a scene according to a 16-parameter transformation and resample the appearance and surface likelihood
//
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
inline float alph(float p, float len){
  if(p>=1.0f) p = 0.999f;
  return -log(1-p)/len;
}
/* This is how the trilinear interpolation relative neighbor buffer is organized, based on the target cell's location */
/*into the source scene, its absolute neighbors are determined from the relative neighbor buffer.*/
/* rel_nbrs[0] = -1 -1 -1  rel_nbrs[2] = 1 -1 -1 */
/* rel_nbrs[1] = -1  1 -1  rel_nbrs[3] = 1  1 -1 */

/* rel_nbrs[4] = -1 -1  1  rel_nbrs[6] = 1 -1 1 */
/* rel_nbrs[5] = -1  1  1  rel_nbrs[7] = 1  1 1 */


__kernel void warp_and_resample_trilinear_similarity(__constant  float           * centerX,//0
                                                     __constant  float           * centerY,//1
                                                     __constant  float           * centerZ,//2
                                                     __constant  uchar           * bit_lookup,//3             //0-255 num bits lookup table
                                                     __global  RenderSceneInfo   * target_scene_linfo,//4
                                                     __global  RenderSceneInfo   * source_scene_linfo,//5
                                                     __global    int4            * target_scene_tree_array,//6       // tree structure for each block
                                                     __global    float           * target_scene_alpha_array,//7      // alpha for each block
                                                     __global    MOG_TYPE        * target_scene_mog_array,//8        // appearance for each block
#ifdef HAS_RGB
                                                     __global    uchar8          * target_rgb_array,
                                                     __global    uchar8          * source_rgb_array,
#endif

                                                     __global    int4            * source_scene_tree_array,//9       // tree structure for each block
                                                     __global    float           * source_scene_alpha_array,//10      // alpha for each block
                                                     __global    MOG_TYPE        * source_scene_mog_array,//11        // appearance for each block
                                                     //                                          __global    ushort4         * source_nobs_array,
                                                     __global    float           * translation,//12
                                                     __global    float           * rotation,//13
                                                     __global    float           * scale,//14
                                                     __global    int             * max_depth,//15               // coarsness or fineness
                                                     //at which voxels should be matched.
                                                     __global    float           * output,//16
                                                     __local     uchar           * cumsum_wkgp,//17
                                                     __local     uchar16         * local_trees_target,//18
                                                     __local     uchar16         * local_trees_source,
                                                     __local     uchar16         * neighbor_trees)//20
{
  int gid = get_global_id(0);
  int lid = get_local_id(0);
  //default values for empty cells
  MOG_INIT(mog_init);
  //-log(1.0f - init_prob)/side_length  init_prob = 0.001f
  float alpha_init = 0.001f/source_scene_linfo->block_len;
  //
  int numTrees = target_scene_linfo->dims.x * target_scene_linfo->dims.y * target_scene_linfo->dims.z;
  float4 source_scene_origin = source_scene_linfo->origin;
  float4 source_scene_blk_dims = convert_float4(source_scene_linfo->dims) ;
  float4 source_scene_maxpoint = source_scene_origin + convert_float4(source_scene_linfo->dims) * source_scene_linfo->block_len ;
  //: each thread will work on a sub_block(tree) of target to match it to locations in the source block
  if (gid < numTrees) {
    local_trees_target[lid] = as_uchar16(target_scene_tree_array[gid]);
    int index_x =(int)( (float)gid/(target_scene_linfo->dims.y * target_scene_linfo->dims.z));
    int rem_x   =( gid- (float)index_x*(target_scene_linfo->dims.y * target_scene_linfo->dims.z));
    int index_y = rem_x/target_scene_linfo->dims.z;
    int rem_y =  rem_x - index_y*target_scene_linfo->dims.z;
    int index_z =rem_y;
    if((index_x >= 0 &&  index_x <= target_scene_linfo->dims.x -1 &&
        index_y >= 0 &&  index_y <= target_scene_linfo->dims.y -1 &&
        index_z >= 0 &&  index_z <= target_scene_linfo->dims.z -1  )) {
      __local uchar16* local_tree = &local_trees_target[lid];
      __local uchar16* neighbor_tree = &neighbor_trees[lid];
      __local uchar * cumsum = &cumsum_wkgp[lid*10];
      // iterate through leaves
      cumsum[0] = (*local_tree).s0;
      int cumIndex = 1;
      int MAX_INNER_CELLS, MAX_CELLS;
      get_max_inner_outer(&MAX_INNER_CELLS, &MAX_CELLS, target_scene_linfo->root_level);
      MAX_CELLS = 585;
      for (int i=0; i<MAX_CELLS; i++) {
        //if current bit is 0 and parent bit is 1, you're at a leaf
        int pi = (i-1)>>3;           //Bit_index of parent bit
        bool validParent = tree_bit_at(local_tree, pi) || (i==0); // special case for root
        int currDepth = get_depth(i);
        if (validParent && ( tree_bit_at(local_tree, i)==0 )) {

          //: for each leaf node xform the cell and find the correspondence in another block.
          //get the index into this cell data
          int dataIndex = data_index_cached(local_tree, i, bit_lookup, cumsum, &cumIndex) + data_index_root(local_tree); //gets absolute position
          // set intial values in case source is not accessed
          /* target_scene_mog_array[dataIndex] = mog_init; */
          /* target_scene_alpha_array[dataIndex] = alpha_init; */

          // transform coordinates to source scene
          float xg = target_scene_linfo->origin.x + ((float)index_x+centerX[i])*target_scene_linfo->block_len ;
          float yg = target_scene_linfo->origin.y + ((float)index_y+centerY[i])*target_scene_linfo->block_len ;
          float zg = target_scene_linfo->origin.z + ((float)index_z+centerZ[i])*target_scene_linfo->block_len ;

          float txg = scale[0]*(rotation[0]*xg +rotation[1]*yg + rotation[2]*zg) + translation[0];
          float tyg = scale[1]*(rotation[3]*xg +rotation[4]*yg + rotation[5]*zg) + translation[1];
          float tzg = scale[2]*(rotation[6]*xg +rotation[7]*yg + rotation[8]*zg) + translation[2];

          // float txg = xg; float tyg= yg; float tzg = zg;
          // is the transformed point inside the source domain
          if(txg > source_scene_origin.x && txg < source_scene_maxpoint.x &&
             tyg > source_scene_origin.y && tyg < source_scene_maxpoint.y &&
             tzg > source_scene_origin.z && tzg < source_scene_maxpoint.z ){
            float8 err_float = (float8) (txg,tyg,tzg,-10,0,0,0,0);
            int s_sub_blk_x = (int) floor((txg - source_scene_origin.x)/(source_scene_linfo->block_len)) ;
            int s_sub_blk_y = (int) floor((tyg - source_scene_origin.y)/(source_scene_linfo->block_len)) ;
            int s_sub_blk_z = (int) floor((tzg - source_scene_origin.z)/(source_scene_linfo->block_len)) ;

            // convert to linear tree index
            int s_tree_index = s_sub_blk_x * ( source_scene_blk_dims.y)*
              ( source_scene_blk_dims.z) +s_sub_blk_y * (source_scene_blk_dims.z)+s_sub_blk_z;

            local_trees_source[lid] = as_uchar16(source_scene_tree_array[s_tree_index]);
            __local uchar * curr_tree_ptr = &local_trees_source[lid];

            float source_lx = clamp((txg - source_scene_origin.x)/source_scene_linfo->block_len - s_sub_blk_x,0.0f,1.0f);
            float source_ly = clamp((tyg - source_scene_origin.y)/source_scene_linfo->block_len - s_sub_blk_y,0.0f,1.0f);
            float source_lz = clamp((tzg - source_scene_origin.z)/source_scene_linfo->block_len - s_sub_blk_z,0.0f,1.0f);

            float cell_minx,cell_miny,cell_minz,cell_len;
            ushort bit_index = traverse_three(curr_tree_ptr,source_lx,source_ly,source_lz,
                                              &cell_minx,&cell_miny,&cell_minz,&cell_len);
            float4 cell_center = ((float4) (cell_minx,cell_miny,cell_minz,0) + cell_len/2 + (float4)(s_sub_blk_x,s_sub_blk_y,s_sub_blk_z,0)) * source_scene_linfo->block_len +source_scene_origin;
            float4 source_p =(float4)(txg,tyg,tzg,0);
            int source_depth = get_depth(bit_index);

            if(bit_index >=0 && bit_index < MAX_CELLS ){

              cell_center.s3=0;
              unsigned int alpha_offset = data_index_root(curr_tree_ptr)+data_index_relative(curr_tree_ptr,bit_index,bit_lookup);
              if( alpha_offset >=0  && alpha_offset < source_scene_linfo->data_len ) {
                // here is where interpolation occurs
                int currDepth = get_depth(i);
                float side_len = 1.0f/((float)(1<<currDepth));
                MOG_TYPE mog_original = source_scene_mog_array  [alpha_offset];
                float  alpha_original = source_scene_alpha_array[alpha_offset];
#ifdef DO_ALPHA
                target_scene_alpha_array[dataIndex] = alpha_original;
#endif
                target_scene_mog_array[dataIndex]   = mog_original;
                // source_lx /= side_len; source_ly /= side_len; source_lz /= side_len;
#ifdef DO_INTERP
                float alphas[8];
                float params[8];
                float weights[12];
                float4 abs_neighbors[8];
                float4 rgb_params[8];
                float cell_len_rw  = cell_len * source_scene_linfo->block_len;
                int nbs_ok = collect_neighbors_and_weights(abs_neighbors,weights,source_p,cell_center, cell_len_rw);
                if(!nbs_ok)
                  continue;
                int nb_count = 0; float sum = 0;
                for (unsigned i=0; i<8; i++){
                  int neighborBitIndex, nb_data_index;
                  float4 nb_cell_center;
                  unsigned nb_point_ok = data_index_world_point(source_scene_linfo, source_scene_tree_array,
                                                                neighbor_trees,lid,abs_neighbors[i],
                                                                &nb_cell_center,
                                                                &nb_data_index,
                                                                &neighborBitIndex,bit_lookup);
                  if (nb_point_ok){

                    alphas[i] = source_scene_alpha_array[nb_data_index] ;
                    nb_count++;
#ifdef MOG_TYPE_8
                    CONVERT_FUNC_FLOAT8(mog,source_scene_mog_array[nb_data_index]);
                    mog/=NORM;
                    EXPECTED_INT(params[i],mog);
#endif
#ifdef HAS_RGB
                    CONVERT_FUNC_FLOAT8(rgb_tuple,source_rgb_array[nb_data_index]);
                    rgb_tuple/=NORM;
                    rgb_params[i].s0123 = rgb_tuple.s0123;
#endif
                  }else{
                    alphas[i] =0.0; params[i]=0;
                  }
                }

                // interpolate alpha over the source

                CONVERT_FUNC_FLOAT8(mog_float,mog_original);
                mog_float/=NORM; float exp_int_orig;
                EXPECTED_INT(exp_int_orig,mog_float); //for debug
                MOG_TYPE mog_interped;
                float alpha_interped = interp_float_weights(alphas,weights);
#ifdef MOG_TYPE_8
                float expected_int   = interp_float_weights(params,weights);
                uchar8 expected_int_interped  = (uchar8)((uchar)(expected_int * NORM), 32, 255, 0, 0, 0, 0, 0); // hack-city
                CONVERT_FUNC_SAT_RTE(mog_interped, expected_int_interped);
#endif

#ifdef HAS_RGB
                uchar8 curr_rgb_tuple = source_rgb_array[alpha_offset];
                float4 float_rgb_tuple_interped   = interp_float4_weights(rgb_params,weights); //use the flow interp for float4s
                uchar4 uchar_rgb_tuple_interped  = convert_uchar4_sat_rte(float_rgb_tuple_interped * NORM); // hack-city
                curr_rgb_tuple.s0123 = uchar_rgb_tuple_interped;
                target_rgb_array[dataIndex] = curr_rgb_tuple;
#endif //rgb

#ifdef DO_ALPHA
                target_scene_alpha_array[dataIndex] = alpha_interped;
#endif
                target_scene_mog_array[dataIndex]   = mog_interped;
#endif //interp

              }
            }
          }
        }
      }
    }
  }
}


__kernel void warp_and_resample_trilinear_vecf( __constant  uchar           * bit_lookup,//3             //0-255 num bits lookup table
                                               __global  RenderSceneInfo * target_scene_linfo,//4
                                               __global  RenderSceneInfo * source_scene_linfo,//5
                                               __global    int4            * target_scene_tree_array,//6       // tree structure for each block
                                               __global    float           * target_scene_alpha_array,//7      // alpha for each block
                                               __global    MOG_TYPE        * target_scene_mog_array,//8        // appearance for each block
                                               __global    int4            * source_scene_tree_array,//9       // tree structure for each block
                                               __global    float           * source_scene_alpha_array,//10      // alpha for each block
                                               __global    MOG_TYPE        * source_scene_mog_array,//11        // appearance for each block
                                               //                                          __global    ushort4         * source_nobs_array,
                                               __global    float4          * inv_transformed_pts, //15
                                               __global    int             * max_depth,//16               // coarsness or fineness
                                               //at which voxels should be matched.
                                               __local     uchar           * cumsum_wkgp,//18
                                               __local     uchar16         * local_trees_target,//19
                                               __local     uchar16         * local_trees_source,
                                               __local     uchar16         * neighbor_trees)
{
  int gid = get_global_id(0);
  int lid = get_local_id(0);
  //default values for empty cells
  MOG_INIT(mog_init);
  //-log(1.0f - init_prob)/side_length  init_prob = 0.001f
  float alpha_init = 0.001f/source_scene_linfo->block_len;
  //
  int numTrees = target_scene_linfo->dims.x * target_scene_linfo->dims.y * target_scene_linfo->dims.z;
  float4 source_scene_origin = source_scene_linfo->origin;
  float4 source_scene_blk_dims = convert_float4(source_scene_linfo->dims) ;
  float4 source_scene_maxpoint = source_scene_origin + convert_float4(source_scene_linfo->dims) * source_scene_linfo->block_len ;
  //: each thread will work on a sub_block(tree) of target to match it to locations in the source block
  if (gid < numTrees) {
    local_trees_target[lid] = as_uchar16(target_scene_tree_array[gid]);
    int index_x =(int)( (float)gid/(target_scene_linfo->dims.y * target_scene_linfo->dims.z));
    int rem_x   =( gid- (float)index_x*(target_scene_linfo->dims.y * target_scene_linfo->dims.z));
    int index_y = rem_x/target_scene_linfo->dims.z;
    int rem_y =  rem_x - index_y*target_scene_linfo->dims.z;
    int index_z =rem_y;
    if((index_x >= 0 &&  index_x <= target_scene_linfo->dims.x -1 &&
        index_y >= 0 &&  index_y <= target_scene_linfo->dims.y -1 &&
        index_z >= 0 &&  index_z <= target_scene_linfo->dims.z -1  )) {
      __local uchar16* local_tree = &local_trees_target[lid];
      __local uchar16* neighbor_tree = &neighbor_trees[lid];
      __local uchar * cumsum = &cumsum_wkgp[lid*10];
      // iterate through leaves
      cumsum[0] = (*local_tree).s0;
      int cumIndex = 1;
      int MAX_INNER_CELLS, MAX_CELLS;
      get_max_inner_outer(&MAX_INNER_CELLS, &MAX_CELLS, target_scene_linfo->root_level);
      MAX_CELLS = 585;
      for (int i=0; i<MAX_CELLS; i++) {
        //if current bit is 0 and parent bit is 1, you're at a leaf
        int pi = (i-1)>>3;           //Bit_index of parent bit
        bool validParent = tree_bit_at(local_tree, pi) || (i==0); // special case for root
        int currDepth = get_depth(i);
        if (validParent && ( tree_bit_at(local_tree, i)==0 )) {

          //: for each leaf node xform the cell and find the correspondence in another block.
          //get the index into this cell data
          int dataIndex = data_index_cached(local_tree, i, bit_lookup, cumsum, &cumIndex) + data_index_root(local_tree); //gets absolute position
          // set intial values in case source is not accessed
          target_scene_mog_array[dataIndex] = mog_init;
          target_scene_alpha_array[dataIndex] = alpha_init;

          // get transformed coordinates mapping to source scene
          float txg = inv_transformed_pts[dataIndex].x;
          float tyg = inv_transformed_pts[dataIndex].y;
          float tzg = inv_transformed_pts[dataIndex].z;

          // float txg = xg; float tyg= yg; float tzg = zg;
          // is the transformed point inside the source domain
          if(txg > source_scene_origin.x && txg < source_scene_maxpoint.x &&
             tyg > source_scene_origin.y && tyg < source_scene_maxpoint.y &&
             tzg > source_scene_origin.z && tzg < source_scene_maxpoint.z ){
            float8 err_float = (float8) (txg,tyg,tzg,-10,0,0,0,0);
            int s_sub_blk_x = (int) floor((txg - source_scene_origin.x)/(source_scene_linfo->block_len)) ;
            int s_sub_blk_y = (int) floor((tyg - source_scene_origin.y)/(source_scene_linfo->block_len)) ;
            int s_sub_blk_z = (int) floor((tzg - source_scene_origin.z)/(source_scene_linfo->block_len)) ;

            // convert to linear tree index
            int s_tree_index = s_sub_blk_x * ( source_scene_blk_dims.y)*
              ( source_scene_blk_dims.z) +s_sub_blk_y * (source_scene_blk_dims.z)+s_sub_blk_z;

            local_trees_source[lid] = as_uchar16(source_scene_tree_array[s_tree_index]);
            __local uchar * curr_tree_ptr = &local_trees_source[lid];

            float source_lx = clamp((txg - source_scene_origin.x)/source_scene_linfo->block_len - s_sub_blk_x,0.0f,1.0f);
            float source_ly = clamp((tyg - source_scene_origin.y)/source_scene_linfo->block_len - s_sub_blk_y,0.0f,1.0f);
            float source_lz = clamp((tzg - source_scene_origin.z)/source_scene_linfo->block_len - s_sub_blk_z,0.0f,1.0f);

            float cell_minx,cell_miny,cell_minz,cell_len;
            ushort bit_index = traverse_three(curr_tree_ptr,source_lx,source_ly,source_lz,
                                              &cell_minx,&cell_miny,&cell_minz,&cell_len);
            float4 cell_center = ((float4) (cell_minx,cell_miny,cell_minz,0) + cell_len/2 + (float4)(s_sub_blk_x,s_sub_blk_y,s_sub_blk_z,0)) * source_scene_linfo->block_len +source_scene_origin;
            float4 source_p =(float4)(txg,tyg,tzg,0);
            int source_depth = get_depth(bit_index);

            if(bit_index >=0 && bit_index < MAX_CELLS ){

              cell_center.s3=0;
              unsigned int alpha_offset = data_index_root(curr_tree_ptr)+data_index_relative(curr_tree_ptr,bit_index,bit_lookup);
              if( alpha_offset >=0  && alpha_offset < source_scene_linfo->data_len ) {
                // here is where interpolation occurs
                int currDepth = get_depth(i);
                float side_len = 1.0f/((float)(1<<currDepth));
                // source_lx /= side_len; source_ly /= side_len; source_lz /= side_len;
                float alphas[8];
                float params[8];
                float4 abs_neighbors[8];
                float4 dP = source_p - cell_center;
                float cell_len_rw  = cell_len * source_scene_linfo->block_len;
                if(dP.x >= 0){ //source point is to the right of the cell center along x
                  abs_neighbors[0].x = abs_neighbors[1].x =abs_neighbors[4].x = abs_neighbors[5].x = cell_center.x; // x-left neighbor is the cell center
                  abs_neighbors[2].x = abs_neighbors[3].x =abs_neighbors[6].x = abs_neighbors[7].x = cell_center.x +cell_len_rw; // x-right neighbor  is the cell to the right of cell center
                }else{ //source point is to the left of the cell center along x
                  abs_neighbors[0].x = abs_neighbors[1].x =abs_neighbors[4].x = abs_neighbors[5].x = cell_center.x - cell_len_rw; // x-left neighbor is the cell to the left of cell center
                  abs_neighbors[2].x = abs_neighbors[3].x =abs_neighbors[6].x = abs_neighbors[7].x = cell_center.x ; // x-right neighbor is the cell center
                }
                if(dP.y >= 0){ //source point is to the right of the cell center along y
                  abs_neighbors[0].y = abs_neighbors[2].y =abs_neighbors[4].y = abs_neighbors[6].y = cell_center.y; // y-left neighbor is the cell center
                  abs_neighbors[1].y = abs_neighbors[3].y =abs_neighbors[5].y = abs_neighbors[7].y = cell_center.y +cell_len_rw; // y-right neighbor  is the cell to the right of cell center
                }else{ //source point is to the left of the cell center along y
                  abs_neighbors[0].y = abs_neighbors[2].y =abs_neighbors[4].y = abs_neighbors[6].y = cell_center.y -cell_len_rw; // y-left neighbor is the cell to the left of cell center
                  abs_neighbors[1].y = abs_neighbors[3].y =abs_neighbors[5].y = abs_neighbors[7].y = cell_center.y; // y-right neighbor the cell center
                }
                if(dP.z >= 0){ //source point is above the cell center along z
                  abs_neighbors[0].z = abs_neighbors[1].z =abs_neighbors[2].z = abs_neighbors[3].z = cell_center.z; // z-bottom neighbor is the cell center
                  abs_neighbors[4].z = abs_neighbors[5].z =abs_neighbors[6].z = abs_neighbors[7].z = cell_center.z +cell_len_rw; // z-top neighbor  is the cell on top of cell center
                }else{ //source point is below the cell center along z
                  abs_neighbors[0].z = abs_neighbors[1].z =abs_neighbors[2].z = abs_neighbors[3].z = cell_center.z -cell_len_rw; // z-bottom neighbor is the cell below cell center
                  abs_neighbors[4].z = abs_neighbors[5].z =abs_neighbors[6].z = abs_neighbors[7].z = cell_center.z ; // z-top neighbor  is the  cell center
                }

                if (abs_neighbors[0].x == abs_neighbors[2].x || abs_neighbors[0].y == abs_neighbors[1].y || abs_neighbors[0].z == abs_neighbors[4].z )
                  continue;
                int nb_count = 0; float sum = 0;
                for (unsigned i=0; i<8; i++){

                  float4 nbCenter = (abs_neighbors[i] - source_scene_origin) / source_scene_linfo->block_len;

                  bool in_bounds_x = (nbCenter.x > 0) && (nbCenter.x < source_scene_linfo->dims.x);
                  bool in_bounds_y = (nbCenter.y > 0) && (nbCenter.y < source_scene_linfo->dims.y);
                  bool in_bounds_z =  (nbCenter.z > 0) && (nbCenter.z < source_scene_linfo->dims.z);

                  bool in_bounds  = in_bounds_x && in_bounds_y && in_bounds_z;

                  if (in_bounds){
                    int blkI = calc_blkI( floor(nbCenter.x),
                                          floor(nbCenter.y),
                                          floor(nbCenter.z), source_scene_linfo->dims);

                    (*neighbor_tree) = as_uchar16( source_scene_tree_array[blkI]);
                    //get neighbor local center, traverse to it
                    float4 locCenter = nbCenter - floor(nbCenter);
                    int neighborBitIndex = (int)traverse_to(neighbor_tree,locCenter,source_depth);
                    int nb_data_index = data_index_relative( neighbor_tree, neighborBitIndex, bit_lookup) + data_index_root(neighbor_tree);
                    alphas[i] = source_scene_alpha_array[nb_data_index] ;
                    nb_count++;
#ifdef MOG_TYPE_8
                    CONVERT_FUNC_FLOAT8(mog,source_scene_mog_array[nb_data_index]);
                    mog/=NORM;
                    EXPECTED_INT(params[i],mog);
#endif

                  }else{
                    alphas[i] =0.0; params[i]=0;
                  }
                }

                // interpolate alpha over the source
                MOG_TYPE mog_original = source_scene_mog_array  [alpha_offset];
                CONVERT_FUNC_FLOAT8(mog_float,mog_original);
                mog_float/=NORM; float exp_int_orig;
                EXPECTED_INT(exp_int_orig,mog_float); //for debug

                float  alpha_original = source_scene_alpha_array[alpha_offset];
                MOG_TYPE mog_interped;
                float alpha_interped = interp_generic_float(abs_neighbors,alphas,source_p);
#ifdef MOG_TYPE_8
                float expected_int   = interp_generic_float(abs_neighbors,params,source_p);
                uchar8 expected_int_interped  = (uchar8)((uchar)(expected_int * NORM), 32, 255, 0, 0, 0, 0, 0); // hack-city
                CONVERT_FUNC_SAT_RTE(mog_interped, expected_int_interped);
#endif


                target_scene_alpha_array[dataIndex] = alpha_interped;
                target_scene_mog_array[dataIndex] = mog_interped;
              }
            }
          }
        }
      }
    }
  }
}
