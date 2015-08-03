//Kernel which computes cell centers in world coordinates and saves to buffer
//
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable


__kernel void compute_cell_centers( __constant  float           * centerX,//0
                                    __constant  float           * centerY,//1
                                    __constant  float           * centerZ,//2
                                    __constant  uchar           * bit_lookup,  //0-255 num bits lookup table
                                    __global  RenderSceneInfo * scene_linfo,
                                    __global    int4            * scene_tree_array, // tree structure for each block
                                    __global    float4          * cell_centers,
                                    __global    int             * max_depth, // coarsness or fineness
                                    __local     uchar           * cumsum_wkgp,
                                    __local     uchar16         * local_trees)
{
  int gid = get_global_id(0);
  int lid = get_local_id(0);

  int numTrees = scene_linfo->dims.x * scene_linfo->dims.y * scene_linfo->dims.z;
  float4 scene_origin = scene_linfo->origin;
  float4 scene_blk_dims = convert_float4(scene_linfo->dims) ;
  float4 scene_maxpoint = scene_origin + convert_float4(scene_linfo->dims) * scene_linfo->block_len ;
  //: each thread will work on a sub_block(tree) of scene to match it to locations in the block
  if (gid < numTrees) {
    local_trees[lid] = as_uchar16(scene_tree_array[gid]);
    int index_x =(int)( (float)gid/(scene_linfo->dims.y * scene_linfo->dims.z));
    int rem_x   =( gid- (float)index_x*(scene_linfo->dims.y * scene_linfo->dims.z));
    int index_y = rem_x/scene_linfo->dims.z;
    int rem_y =  rem_x - index_y*scene_linfo->dims.z;
    int index_z =rem_y;
    if((index_x >= 0 &&  index_x <= scene_linfo->dims.x -1 &&
        index_y >= 0 &&  index_y <= scene_linfo->dims.y -1 &&
        index_z >= 0 &&  index_z <= scene_linfo->dims.z -1  )) {
      __local uchar16* local_tree = &local_trees[lid];
      __local uchar * cumsum = &cumsum_wkgp[lid*10];
      // iterate through leaves
      cumsum[0] = (*local_tree).s0;
      int cumIndex = 1;
      int MAX_INNER_CELLS, MAX_CELLS;
      get_max_inner_outer(&MAX_INNER_CELLS, &MAX_CELLS, scene_linfo->root_level);
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

          // transform coordinates to world coordinate system
          float xg = scene_linfo->origin.x + ((float)index_x+centerX[i])*scene_linfo->block_len ;
          float yg = scene_linfo->origin.y + ((float)index_y+centerY[i])*scene_linfo->block_len ;
          float zg = scene_linfo->origin.z + ((float)index_z+centerZ[i])*scene_linfo->block_len ;
          // save coordinates to buffer
          cell_centers[dataIndex] = (float4)(xg, yg, zg, 1.0f);
        }
      }
    }
  }
}
