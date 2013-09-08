//------------------------------------------------------------------------------
// Fill the alpha for parents by Max Probability of the children
//------------------------------------------------------------------------------
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable



/////////////////////////////////////////////////////////////////
// Given zero based alpha array, merges tree
/////////////////////////////////////////////////////////////////
__kernel void update_parents_alpha( __constant RenderSceneInfo  * linfo,
                                    __constant uchar           * bit_lookup,       // used to get data_index
                                    __global   int4            * tree_array,
                                    __global   float           * alpha_array,
                                    __local    uchar16         * tree,
                                    __local    uchar           * all_cumsum)
{
  unsigned gid = get_global_id(0);
  unsigned lid = get_local_id(0);


  if(gid < linfo->dims.x*linfo->dims.y*linfo->dims.z)
  {
  tree[lid] = as_uchar16(tree_array[gid]);
  __local uchar* local_tree = &tree[lid];
  __local uchar*   cumsum   = &all_cumsum[10*lid];
  //cast local pointers to uchar arrays

  cumsum[0] = local_tree[0];

  int cumIndex = 1;
  int numSplit = 0;

  //if there are no children
  if (tree_bit_at(local_tree,0)==0)
    return ;
  //: traversing the tree bottom-up
  for (int i=584; i> 0; ) {
      int pi = (i-1)>>3;           //Bit_index of parent bit
      bool validParent = tree_bit_at(local_tree, pi) ;
      if (validParent ) {
          int count = 0;
          float maxalpha  = 0.0 ;
          while (count < 8)
          {
              int dataIndex = data_index_cached(local_tree, i, bit_lookup, cumsum, &cumIndex) + data_index_root(local_tree); //gets absolute position
              float alpha   = alpha_array[dataIndex];
              maxalpha = alpha > maxalpha? alpha : maxalpha;
              count++; i--;
          }
          int parentdataIndex = data_index_cached(local_tree, pi, bit_lookup, cumsum, &cumIndex) + data_index_root(local_tree); //gets absolute position
          alpha_array[parentdataIndex] = maxalpha/2.0;
      }
      else
          i-=8;
  }
  }
}

/////////////////////////////////////////////////////////////////
// Given zero based alpha array, merges tree
/////////////////////////////////////////////////////////////////
__kernel void init_alpha( __constant RenderSceneInfo  * linfo,
                         __constant uchar           * bit_lookup,       // used to get data_index
                         __global   int4            * tree_array,
                         __global   float           * alpha_array,
                         __global   float           * pinit,
                         __global   float           * thresh,
                         __local    uchar16         * tree,
                         __local    uchar           * all_cumsum)
{
    unsigned gid = get_global_id(0);
    unsigned lid = get_local_id(0);


    if(gid < linfo->dims.x*linfo->dims.y*linfo->dims.z)
    {
        tree[lid] = as_uchar16(tree_array[gid]);
        __local uchar* local_tree = &tree[lid];
        __local uchar*   cumsum   = &all_cumsum[10*lid];
        //cast local pointers to uchar arrays

        cumsum[0] = local_tree[0];

        int cumIndex = 1;
        int numSplit = 0;

        //FOR ALL LEAVES IN CURRENT TREE
        for (int i = 0; i < 585; ++i) {
            if ( is_leaf(local_tree, i) ) {

                ///////////////////////////////////////
                //LEAF CODE
                int currDepth = get_depth(i);
                float side_len = 1.0f/(float) (1<<currDepth);

                int currIdx = data_index_relative(local_tree, i, bit_lookup) + data_index_root(local_tree);

                float curr_alpha = alpha_array[currIdx];
                float curr_prob = 1- exp(-curr_alpha* side_len * linfo->block_len) ;

                if(curr_prob <  thresh[0])
                {
                    float alpha = -log(1 - pinit[0])/(side_len * linfo->block_len );
                    alpha_array[currIdx] = alpha;
                }
            }
        }
    }
}
