//:
// \file some text
// \brief  Refines empty cells around surface geometry based on the size of a filter
// \author Octi Biris
// \date 6/13/15

//requires cell_minx



__kernel void label_cells_to_refine(
                                    __constant RenderSceneInfo * linfo,
                                    __global   uchar16         * trees,
                                    __global   float           * alpha,                //could be alpha or any other float quantity
                                    __global   short           * to_refine, //temporal gradient
                                    __global   float           * p_thresh,
                                    __global   float           * output,
                                    __global   float4          * filter,                 //holds position and coefficient
                                    __constant unsigned        * filter_size,              //number of coefficients
                                    __constant uchar           * lookup,
                                    __constant float           * centerX,                //center of current cell
                                    __constant float           * centerY,
                                    __constant float           * centerZ,
                                    __local    uchar16         * all_local_tree,
                                    __local    uchar16         * all_neighbor_tree)
{
  //make sure local_tree points to the right one in shared memory
  uchar llid = (uchar)(get_local_id(0) + (get_local_id(1) + get_local_id(2)*get_local_size(1))*get_local_size(0));
  __local uchar16* local_tree    = &all_local_tree[llid];
  __local uchar16* neighbor_tree = &all_neighbor_tree[llid];


  //global id should be the same as treeIndex
  unsigned gidX = get_global_id(0);
  unsigned gidY = get_global_id(1);
  unsigned gidZ = get_global_id(2);

  //The resolution of the kernels is fixed at the highest res cell


  //tree Index is global id
  unsigned treeIndex = calc_blkI(gidX, gidY, gidZ, linfo->dims);
  if(gidX < linfo->dims.x && gidY < linfo->dims.y && gidZ <linfo->dims.z)
    {
      int MAX_INNER_CELLS, MAX_CELLS;
      get_max_inner_outer(&MAX_INNER_CELLS, &MAX_CELLS, linfo->root_level);

      //get currdsent tree information
      (*local_tree)    = trees[treeIndex];

      //loop over all leaves on the local tree
      for(int i = 0; i < MAX_CELLS; i++)
        {   //check if cell is at the current level and has no children
          if ( is_leaf(local_tree,i)){
            int currDepth = get_depth(i);
            //get properties of current leaf cell
            float response = 0.0;
            float side_len = 1.0f/(float) (1<<currDepth);
            float4 localCenter = (float4) (centerX[i], centerY[i], centerZ[i], 0.0f);  //local center within block
            float4 cellCenter = (float4) ((float) gidX + centerX[i],                  //abs center within block
                                          (float) gidY + centerY[i],
                                          (float) gidZ + centerZ[i],
                                          0.0f );

            int currIdx = data_index_relative(local_tree, i, lookup) + data_index_root(local_tree);
            float prob = 1.0f - exp(-alpha[currIdx] * side_len * linfo->block_len);
#ifdef REFINE_ALL
            to_refine[currIdx] = 1;
#else
            if(prob > p_thresh[0]){
              //              to_refine[currIdx] = 1;
              //iterate through the filter coefficients and positions
              for( uint ci = 0; ci < filter_size[0]; ci++ ) {

                //get neighbor location, and corresponding tree (may be current tree, may not) -- it may be more efficient to collect all neighbors in a linked  list...
                float4 nbCenter = (float4) (cellCenter.x+(side_len*filter[ci].x), cellCenter.y+(side_len*filter[ci].y), cellCenter.z+(side_len*filter[ci].z), 0.0f);

                //check if neighbor is out of bounds
                bool in_bounds = (nbCenter.x > 0) && (nbCenter.x < linfo->dims.x);
                in_bounds = in_bounds && (nbCenter.y > 0) && (nbCenter.y < linfo->dims.y);
                in_bounds = in_bounds && (nbCenter.z > 0) && (nbCenter.z < linfo->dims.z);

                if (!in_bounds){
                  continue;
                }

                int blkI = calc_blkI( floor(nbCenter.x),
                                      floor(nbCenter.y),
                                      floor(nbCenter.z), linfo->dims);

                (*neighbor_tree) = trees[blkI];

                //get neighbor local center, traverse to it
                float4 locCenter = nbCenter - floor(nbCenter);
                int neighborBitIndex = (int) traverse_to(neighbor_tree, locCenter, currDepth);
                int nb_depth = get_depth(neighborBitIndex);
                int idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
                if(nb_depth < currDepth)
                  to_refine[idx] = 1;
              }
            }
#endif
          }
        }
    }
}
