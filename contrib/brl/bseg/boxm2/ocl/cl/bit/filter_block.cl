//:
// \file

//------------------------------------------------------------------------------
// Filters Bit Scene
//-----------------------------------------------------------------------------

//requires cell_minx
inline int calc_blkI(float cell_minx, float cell_miny, float cell_minz, int4 dims)
{
  return convert_int(cell_minz + (cell_miny + cell_minx*dims.y)*dims.z);
}

inline void get_max_inner_outer(int* MAX_INNER_CELLS, int* MAX_CELLS, int root_level)
{
  //USE rootlevel to determine MAX_INNER and MAX_CELLS
  if (root_level == 1)
    *MAX_INNER_CELLS=1, *MAX_CELLS=9;
  else if (root_level == 2)
    *MAX_INNER_CELLS=9, *MAX_CELLS=73;
  else if (root_level == 3)
    *MAX_INNER_CELLS=73, *MAX_CELLS=585;
}

inline void get_neighbor_points( float4 cellCenter, float side_len, int4 dims, linked_list* neighbors)
{
  //neighbors along X
  if ( cellCenter.x + side_len < dims.x )
    push_back( neighbors, (float4)( cellCenter.x+side_len, cellCenter.y, cellCenter.z, 0.0f) );
  if ( cellCenter.x - side_len > 0 )
    push_back( neighbors, (float4)( cellCenter.x-side_len, cellCenter.y, cellCenter.z, 0.0f) );

  //neighbors along Y
  if ( cellCenter.y + side_len < dims.y )
    push_back( neighbors, (float4)( cellCenter.x, cellCenter.y+side_len, cellCenter.z, 0.0f) );
  if ( cellCenter.y - side_len > 0 )
    push_back( neighbors, (float4)( cellCenter.x, cellCenter.y-side_len, cellCenter.z, 0.0f) );

  //neighbors along Z
  if ( cellCenter.z + side_len < dims.z )
    push_back( neighbors, (float4)( cellCenter.x, cellCenter.y, cellCenter.z+side_len, 0.0f) );
  if ( cellCenter.z - side_len > 0 )
    push_back( neighbors, (float4)( cellCenter.x, cellCenter.y, cellCenter.z-side_len, 0.0f) );
}

inline void sum_alpha(          float* totalAlphaL,
                                int bitIndex,
                       __local  uchar16* neighbor_tree,
                       __local  uchar* csum,
                                int* cumIndex,
                                float block_len,
                       __global float* alphas,
                       __constant uchar* lookup )
{
  int ndepth = get_depth(bitIndex);
  float nlen = 1.0f / (float) (1<<ndepth);
  //int dataIndex = data_index_cached( neighbor_tree, bitIndex, lookup, csum, &cumIndex) + data_index_root(neighbor_tree);
  int dataIndex = data_index_relative(neighbor_tree, bitIndex, lookup) + data_index_root(neighbor_tree);
  (*totalAlphaL) += alphas[dataIndex] * nlen * block_len;
}


//: Median filter for a block
__kernel void filter_block(__constant RenderSceneInfo * linfo,
                           __global   uchar16         * trees,
                           __global   float           * alphas,
                           __global   float           * new_alphas,
                           __constant uchar           * lookup,
                           __constant float           * centerX,
                           __constant float           * centerY,
                           __constant float           * centerZ,
                           __local    uchar           * cumsum,
                           __local    uchar16         * all_local_tree,
                           __local    uchar16         * all_neighbor_tree,
                           __local    float4          * all_local_neighbors)
{
  //make sure local_tree points to the right one in shared memory
  uchar llid = (uchar)(get_local_id(0) + (get_local_id(1) + get_local_id(2)*get_local_size(1))*get_local_size(0));
  __local uchar16* local_tree    = &all_local_tree[llid];
  __local uchar16* neighbor_tree = &all_neighbor_tree[llid];
  __local uchar*   csum          = &cumsum[llid*10];
  __local float4*  neighborMem   = &all_local_neighbors[llid*6];

  //global id should be the same as treeIndex
  unsigned gidX = get_global_id(0);
  unsigned gidY = get_global_id(1);
  unsigned gidZ = get_global_id(2);

  //tree Index is global id
  unsigned treeIndex = calc_blkI(gidX, gidY, gidZ, linfo->dims);
  if (gidX < linfo->dims.x && gidY < linfo->dims.y && gidZ <linfo->dims.z) {

    int MAX_INNER_CELLS, MAX_CELLS;
    get_max_inner_outer(&MAX_INNER_CELLS, &MAX_CELLS, linfo->root_level);

    //1. get current tree information
    (*local_tree)    = trees[treeIndex];

    //FOR ALL LEAVES IN CURRENT TREE
    for (int i = 0; i < MAX_CELLS; ++i) {
      if ( is_leaf(local_tree, i) ) {

        ///////////////////////////////////////
        //LEAF CODE
        int currDepth = get_depth(i);
        float side_len = 1.0f/(float) (1<<currDepth);

        float4 localCenter = (float4) (centerX[i], centerY[i], centerZ[i], 0.0f);  //local center within block
        float4 cellCenter = (float4) ( (float) gidX + centerX[i],                  //abs center within block
                                       (float) gidY + centerY[i],
                                       (float) gidZ + centerZ[i],
                                       0.0f );

        //loop through neighbor points (+- cell size)
        linked_list neighbors = new_linked_list(neighborMem, 6);
        get_neighbor_points(cellCenter, side_len, linfo->dims, &neighbors);

        //list of neighbor surface probs - choose median from this list
        float probs[7];
        int numProbs = 0;
        while( !empty( &neighbors ) )
        {
          //get neighbor Point, and corresponding tree (may be current tree, may not)
          float4 abCenter = pop_front( &neighbors );
          int blkI = calc_blkI( floor(abCenter.x),
                                floor(abCenter.y),
                                floor(abCenter.z), linfo->dims);
          (*neighbor_tree) = trees[blkI];
          csum[0] = (*neighbor_tree).s0;
          int cumIndex = 1;

          //get neighbor local center, traverse to it
          float4 locCenter = abCenter - floor(abCenter);
          int neighborBitIndex = (int) traverse_to(neighbor_tree, locCenter, currDepth);
          if ( is_leaf(neighbor_tree, neighborBitIndex) )
          {
            //get data index
            //int idx = data_index_cached( neighbor_tree, neighborBitIndex, lookup, csum, &cumIndex) + data_index_root(neighbor_tree);
            int idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
            int deltaDepth = abs( currDepth-get_depth(neighborBitIndex) );

            //grab alpha, calculate probability
            float alphaFactor = 1.0f / (float) (1<<(3*deltaDepth));
            float alpha = alphas[idx];
            float prob = 1.0f - exp(-alpha * side_len * alphaFactor * linfo->block_len);
            probs[numProbs++] = prob;
          }
          else //neighbor is smaller, must combine neighborhood
          {
#if 1
            //stateless depth first traversal here.
            //neighbor bit index max minchild and max child
            int neighborDepth = get_depth(neighborBitIndex);
            int minChild = neighborBitIndex*8+1;
            float totalAlphaL = 0.0f;
            for (int nci=minChild; nci<minChild+8; ++nci) {

              //do leaf calc, otherwise continue to iterate
              if ( is_leaf(neighbor_tree, nci) ) {
                sum_alpha(&totalAlphaL, nci, neighbor_tree, csum, &cumIndex, linfo->block_len, alphas, lookup);
              }

              else if (neighborDepth<2) {
                int minGC = nci*8+1;
                for (int ngi=minGC; ngi<minGC+8; ++ngi) {

                  //do leaf calc, otherwise continue to iterate
                  if ( is_leaf(neighbor_tree, ngi) ) {
                    sum_alpha(&totalAlphaL, ngi, neighbor_tree, csum, &cumIndex, linfo->block_len, alphas, lookup);
                  }
                  else if (neighborDepth<1) {

                    //at this point we know these guys are leaves
                    int minGGC = ngi*8+1;
                    for (int nggi=minGGC; nggi<minGGC+8; ++nggi) {
                      if ( is_leaf(neighbor_tree, nggi) ) {
                        sum_alpha(&totalAlphaL, nggi, neighbor_tree, csum, &cumIndex, linfo->block_len, alphas,lookup);
                      }
                    }
                  }

                } //end second gen (grand children)
              }

            } //end 1st gen traversal

            //store the total prob
            float prob = 1.0f - exp( - totalAlphaL );
            probs[numProbs++] = prob;
#endif
          }
        } //end while over neighbors

        //if you've collected a nonzero amount of probs, update it
        int currIdx = data_index_relative(local_tree, i, lookup) + data_index_root(local_tree);
        float curr_prob = 1.0f-exp(-alphas[currIdx] * side_len * linfo->block_len );
        probs[numProbs++]=curr_prob;
        if (numProbs > 0) {
          sort_vector(probs, numProbs);
          float median = probs[ convert_int_rtn(numProbs/2.0) ];
          median = max(median, 0.0001f);
          //if (curr_prob<median)
          //    median=curr_prob;
          float medAlpha = -1.0 * log(1.0f-median) / ( side_len * linfo->block_len );
          new_alphas[currIdx] = medAlpha;
        }
        else {
          //this should never happen.
          new_alphas[currIdx] = alphas[currIdx];
        }
        //END LEAF CODE
        ///////////////////////////////////////

      } //end leaf IF
    } //end leaf for

  } //end global id IF
}

