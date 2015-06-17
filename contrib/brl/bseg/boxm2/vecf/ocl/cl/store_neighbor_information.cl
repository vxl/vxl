//:
// \file
// \brief Gather up neighbors and store thier expected intensities in the scene
// \author J. L. Mundy
// \date October 18, 2014

//requires cell_minx
inline int calc_blkI(float cell_minx, float cell_miny, float cell_minz, int4 dims)
{
  return convert_int(cell_minz + (cell_miny + cell_minx*dims.y)*dims.z);
}

inline bool valid_leaf(int i, __local uchar* tree, int max_d){
  int curr_d = get_depth(i);
  //if current bit is 0 and parent bit is 1, you're at a leaf
  bool validParent = tree_bit_at(tree, (i-1)>>3) || (i==0); // special case for root
  return validParent && ( tree_bit_at(tree, i)==0 );
}

inline float prob(float alpha, float len){
return  1.0f - exp(-alpha * len);
}
__kernel void extract_neighbors_block( __constant float           * centerX,                //center of current cell
				       __constant float           * centerY,
				       __constant float           * centerZ,
				       __constant uchar           * lookup,
				       __global RenderSceneInfo   * linfo,
				       __global   int4            * tree_array,
				       __global   MOG_TYPE        * mog_array,
				       __global   float           * alpha_array,
				       __global   uchar8          * nbr_exint,             //expected intensities of neighbors
				       __global   float8          * nbr_prob,
				       __global   uchar8          * nbr_exists,            //does neighbor exist
				       __global    int            * max_depth,
				       __global    float          * output,               // debug output
				       __local    uchar16         * local_tree_arg,
				       __local    uchar16         * neighbor_tree_arg
				       )
{

  int gid = get_global_id(0);
  int llid = get_local_id(0);
  int numTrees = linfo->dims.x * linfo->dims.y * linfo->dims.z;
  __local uchar16* neighbor_tree = &neighbor_tree_arg[llid];
  //The resolution of the kernels is fixed at the highest res cell

  int MAX_CELLS=585;
  /* if(max_depth[0] == 1) */
  /*   MAX_CELLS=9; */
  /* else if (max_depth[0] == 2) */
  /*   MAX_CELLS=73; */
  /* else if (max_depth[0] == 3) */
  /*   MAX_CELLS=585; */
  // must be inside the block

  if(gid < numTrees)
  {
    local_tree_arg[llid]=as_uchar16(tree_array[gid]);
    __local uchar16* local_tree = &local_tree_arg[llid];
    int index_x =(int)( (float)gid/(linfo->dims.y * linfo->dims.z));
    int rem_x   =( gid- (float)index_x*(linfo->dims.y * linfo->dims.z));
    int index_y = rem_x/linfo->dims.z;
    int rem_y =  rem_x - index_y*linfo->dims.z;
    int index_z =rem_y;
    if((index_x >= 0 &&  index_x <= linfo->dims.x -1 &&
	index_y >= 0 &&  index_y <= linfo->dims.y -1 &&
	index_z >= 0 &&  index_z <= linfo->dims.z -1  ))
      {
    //loop over all leaves on the local tree
	for(int i = 0; i < MAX_CELLS; ++i)
	  {
	    if(valid_leaf(i, local_tree, max_depth[0])){
	      //get geometry of current leaf cell
	      int currDepth = get_depth(i);
	      float side_len = 1.0f/((float)(1<<currDepth));
	      float4 cellCenter = (float4) ( (float) index_x + centerX[i],                  //abs center within block
					     (float) index_y + centerY[i],
					     (float) index_z + centerZ[i],
					     0.0f );
	      uchar8 temp = (uchar8)(0); //local register to temporarily store neighbor existence
	      uchar8 temp_ex = (uchar8)(0); //local register to temporarily store expected intensity
	      float8 temp_prb = (float8)(0.001f); //local register to temporarily store neighbor probs
	      float4 lcent; //local center
	      int neighborBitIndex, idx, blkI;
	      float del = 0.0f, exint = 0.0f, nrm = (float)NORM;
	      float8 data;

	      // compute the neighbor locations and test if they exist, store in global mem
	      // just unroll here to avoid a lot of argument passing overhead
	      // neighbors along x
	      del = cellCenter.x-side_len;
	      float4 xm = (float4)( del , cellCenter.y, cellCenter.z, 0.0f);
	      temp.s0 = (uchar)((del > 0)? 1 : 0);
	      if(temp.s0){
		blkI = calc_blkI( floor(xm.x), floor(xm.y), floor(xm.z), linfo->dims);
		neighbor_tree_arg[llid]=as_uchar16(tree_array[blkI]);
		lcent = xm - floor(xm);
		neighborBitIndex = (int) traverse_to(neighbor_tree, lcent, currDepth);
		if( valid_leaf(neighborBitIndex, neighbor_tree, max_depth[0])){
		  idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
		  CONVERT_FUNC(udata,mog_array[idx]);
		  data=convert_float8(udata)/nrm;
		  EXPECTED_INT(exint,data);
		  temp_ex.s0 = (uchar)(exint*nrm);
		  temp_prb.s0 = prob(alpha_array[idx], side_len);
		}
	      }

	      del = cellCenter.x+side_len;
	      float4 xp = (float4)( del, cellCenter.y, cellCenter.z, 0.0f);
	      temp.s1 = (uchar)((del < linfo->dims.x)? 1 : 0);
	      if(temp.s1){
		blkI = calc_blkI( floor(xp.x), floor(xp.y), floor(xp.z), linfo->dims);
		neighbor_tree_arg[llid] = as_uchar16(tree_array[blkI]);
		lcent = xp - floor(xp);
		neighborBitIndex = (int) traverse_to(neighbor_tree, lcent, currDepth);
		if(valid_leaf(neighborBitIndex, neighbor_tree, max_depth[0])){
		  idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
		  CONVERT_FUNC(udata,mog_array[idx]);
		  data=convert_float8(udata)/nrm;
		  EXPECTED_INT(exint,data);
		  temp_ex.s1 = (uchar)(exint*nrm);
		  temp_prb.s1 = prob(alpha_array[idx], side_len);
		}
	      }

	      // neighbors along y
	      del = cellCenter.y-side_len;
	      float4 ym = (float4)( cellCenter.x, del, cellCenter.z, 0.0f);
	      temp.s2 = (uchar)((del > 0)? 1 : 0);
	      if(temp.s2){
		blkI = calc_blkI( floor(ym.x), floor(ym.y), floor(ym.z), linfo->dims);
		neighbor_tree_arg[llid] = as_uchar16(tree_array[blkI]);
		lcent = ym - floor(ym);
		neighborBitIndex = (int) traverse_to(neighbor_tree, lcent, currDepth);
		if(valid_leaf(neighborBitIndex, neighbor_tree, max_depth[0])){
		  idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
		  CONVERT_FUNC(udata,mog_array[idx]);
		  data=convert_float8(udata)/nrm;
		  EXPECTED_INT(exint,data);
		  temp_ex.s2 = (uchar)(exint*nrm);
		  temp_prb.s2 = prob(alpha_array[idx], side_len);
		}
	      }
	      del = cellCenter.y+side_len;
	      float4 yp = (float4)( cellCenter.x, del, cellCenter.z, 0.0f);
	      temp.s3 = (uchar)((del < linfo->dims.y)? 1 : 0);
	      if(temp.s3){
		blkI = calc_blkI( floor(yp.x), floor(yp.y), floor(yp.z), linfo->dims);
		neighbor_tree_arg[llid] = as_uchar16(tree_array[blkI]);
		lcent = yp - floor(yp);
		neighborBitIndex = (int) traverse_to(neighbor_tree, lcent, currDepth);
		if( valid_leaf(neighborBitIndex, neighbor_tree, max_depth[0])){
		  idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
		  CONVERT_FUNC(udata,mog_array[idx]);
		  data=convert_float8(udata)/nrm;
		  EXPECTED_INT(exint,data);
		  temp_ex.s3 = (uchar)(exint*nrm);
		  temp_prb.s3 = prob(alpha_array[idx], side_len);
		}
	      }
	      // neighbors along z
	      del = cellCenter.z-side_len;
	      float4 zm = (float4)( cellCenter.x, cellCenter.y, del, 0.0f);
	      temp.s4 = (uchar)((del > 0)? 1 : 0);
	      if(temp.s4){
		blkI = calc_blkI( floor(zm.x), floor(zm.y), floor(zm.z), linfo->dims);
		neighbor_tree_arg[llid] = as_uchar16(tree_array[blkI]);
		lcent = zm - floor(zm);
		neighborBitIndex = (int) traverse_to(neighbor_tree, lcent, currDepth);
		if( valid_leaf(neighborBitIndex, neighbor_tree, max_depth[0])){
		  idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
		  CONVERT_FUNC(udata,mog_array[idx]);
		  data=convert_float8(udata)/nrm;
		  EXPECTED_INT(exint,data);
		  temp_ex.s4 = (uchar)(exint*nrm);
		  temp_prb.s4 = prob(alpha_array[idx], side_len);
		}
	      }
	      del = cellCenter.z+side_len;
	      float4 zp = (float4)( cellCenter.x, cellCenter.y, del, 0.0f);
	      temp.s5 = (uchar)((del < linfo->dims.z)? 1 : 0);
	      if(temp.s5){
		blkI = calc_blkI( floor(zp.x), floor(zp.y), floor(zp.z), linfo->dims);
		neighbor_tree_arg[llid] = as_uchar16(tree_array[blkI]);
		lcent = zp - floor(zp);
		neighborBitIndex = (int) traverse_to(neighbor_tree, lcent, currDepth);
		if( valid_leaf(neighborBitIndex, neighbor_tree, max_depth[0]) ){
		  idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
		  CONVERT_FUNC(udata,mog_array[idx]);
		  data=convert_float8(udata)/nrm;
		  EXPECTED_INT(exint,data);
		  temp_ex.s5 = (uchar)(exint*nrm);
		  temp_prb.s5 = prob(alpha_array[idx], side_len);
		}
	      }
	      int cidx = data_index_relative( local_tree, i, lookup) + data_index_root(local_tree);
	      nbr_exint[cidx]=temp_ex;
	      nbr_exists[cidx]=temp;
	      nbr_prob[cidx]=temp_prb;
	    }
	  }
      }
  }
}
