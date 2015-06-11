#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable

//requires cell_minx
inline int calc_blkI(float cell_minx, float cell_miny, float cell_minz, int4 dims)
{
  return convert_int(cell_minz + (cell_miny + cell_minx*dims.y)*dims.z);
}

inline bool valid_leaf(int i, __local uchar* tree, int max_d){
  int curr_d = get_depth(i);
  //if current bit is 0 and parent bit is 1, you're at a leaf
  bool validParent = tree_bit_at(tree, (i-1)>>3) || (i==0); // special case for root
  return validParent && ( tree_bit_at(tree, i)==0 || curr_d == max_d );
}

void filter_mog(MOG_TYPE* mog, uchar8* nbr_exint, uchar8* nbr_exist, float* filter_weights){
  uchar8 lhs = as_uchar8(*mog);
  float nrm = (float)NORM;
  // number of neighbors for debug
  //float nnbr = (float)((*nbr_exist).s0 +(*nbr_exist).s1 +(*nbr_exist).s2 +(*nbr_exist).s3+(*nbr_exist).s4 + (*nbr_exist).s5);
  //  nnbr /= 6.0f; //fraction of neighbors
  float8  nexint = convert_float8(*nbr_exint)/nrm;
  float mu0 = (float)lhs.s0/255.0f;
  // initialize weights to (0,1) neighbor existence
  float wmx = (float)(*nbr_exist).s0; float wpx = (float)(*nbr_exist).s1;
  float wmy = (float)(*nbr_exist).s2; float wpy = (float)(*nbr_exist).s3;
  float wmz = (float)(*nbr_exist).s4; float wpz = (float)(*nbr_exist).s5;
  float w0 = filter_weights[0];
  // multiply with filter weights
  wmx *= filter_weights[1]; wpx *= filter_weights[2];
  wmy *= filter_weights[3]; wpy *= filter_weights[4];
  wmz *= filter_weights[5]; wpz *= filter_weights[6];;
  //sum the weights
  float sumw = w0+wmx+wpx+wmy+wpy+wmz+wpz;
  if(sumw == 0.0f)
    return;
  float mu = w0*mu0;
  mu += wmx*nexint.s0 + wpx*nexint.s1;
  mu += wmy*nexint.s2 + wpy*nexint.s3;
  mu += wmz*nexint.s4 + wpz*nexint.s5;
  mu = (nrm*mu)/sumw;
  lhs = (uchar8)((uchar)mu, 32, 255, 0, 0, 0, 0, 0);
  CONVERT_FUNC_SAT_RTE(*mog, lhs);
}
void filter_alpha(float* alpha, float8* nbr_alpha, uchar8* nbr_exist, float* filter_weights){
  float8 w = *filter_weights;
  // initialize weights to (0,1) neighbor existence
  float wmx = (float)(*nbr_exist).s0; float wpx = (float)(*nbr_exist).s1;
  float wmy = (float)(*nbr_exist).s2; float wpy = (float)(*nbr_exist).s3;
  float wmz = (float)(*nbr_exist).s4; float wpz = (float)(*nbr_exist).s5;
  float w0 = filter_weights[0];
  // multiply with filter weights
  wmx *= filter_weights[1]; wpx *= filter_weights[2];
  wmy *= filter_weights[3]; wpy *= filter_weights[4];
  wmz *= filter_weights[5]; wpz *= filter_weights[6];;
  //sum the weights
  float sumw = w0+wmx+wpx+wmy+wpy+wmz+wpz;
  if(sumw == 0.0f)
    return;
  float a = w0*(*alpha) + wmx*(*nbr_alpha).s0 + wpx*(*nbr_alpha).s1;
  a += wmy*(*nbr_alpha).s2 + wpy*(*nbr_alpha).s3;
  a += wmz*(*nbr_alpha).s4 + wpz*(*nbr_alpha).s5;
  a /= sumw;
  *alpha = a;
}
__kernel void filter_block_six_neighbors(__constant  float           * centerX,
					 __constant  float           * centerY,
					 __constant  float           * centerZ,
					 __global    float           * filter_weights,
					 __global    int             * num_iter,
					 __constant  uchar           * lookup,             //0-255 num bits lookup table
					 __global    int4            * temp_tree_array,       // tree structure for each block
					 __global    float           * temp_alpha_array,      // alpha for each block
					 __global    MOG_TYPE        * temp_mog_array,        // appearance for each block
					 __global    RenderSceneInfo * source_linfo,
					 __global    int4            * source_tree_array,       // tree structure for each block
					 __global    float           * source_alpha_array,      // alpha for each block
					 __global    MOG_TYPE        * source_mog_array,        // appearance for each block
					 __global    int             * max_depth,               // coarsness or fineness
					                                                        //at which voxels should be matched.
					 __global    float           * output,
					 __local     uchar16         * local_trees_temp,
					 __local     uchar16         * local_trees_source,
					 __local     uchar16         * neighbor_trees)
{
  for(int k = 0; k<6;k++)
    output[k]=0.0f;// good result
  float weights[8];
  for(int k = 0; k<8; ++k)
    weights[k] = filter_weights[k];
  int gid = get_global_id(0);
  int lid = get_local_id(0);
  __local uchar16* neighbor_tree = &neighbor_trees[lid];
  //The resolution of the kernels is fixed at the highest res cell
  int  MAX_CELLS = 585;
  /* if(max_depth[0] == 1) */
  /*   MAX_CELLS=9; */
  /* else if (max_depth[0] == 2) */
  /*   MAX_CELLS=73; */
  /* else if (max_depth[0] == 3) */
  /*   MAX_CELLS=585; */
  int nx = source_linfo->dims.x, ny = source_linfo->dims.y, nz = source_linfo->dims.z;
  int nt = nx*ny*nz;
  if(gid < nt){
  // find the 3-d index of the sub-tree that this thread is processing
    int index_x =(int)( (float)gid/(source_linfo->dims.y * source_linfo->dims.z));
    int rem_x   =( gid- (float)index_x*(source_linfo->dims.y * source_linfo->dims.z));
    int index_y = rem_x/source_linfo->dims.z;
    int rem_y =  rem_x - index_y*source_linfo->dims.z;
    int index_z =rem_y;
  // the 3-d index must be inside the block
    if((index_x >= 0 &&  index_x <= source_linfo->dims.x -1 &&
	index_y >= 0 &&  index_y <= source_linfo->dims.y -1 &&
	index_z >= 0 &&  index_z <= source_linfo->dims.z -1  )){

  //local storage allocated to this thread
      local_trees_source[lid]=as_uchar16(source_tree_array[gid]);
      __local uchar16* local_tree_source = &local_trees_source[lid];
      local_trees_temp[lid]=as_uchar16(temp_tree_array[gid]);
      __local uchar16* local_tree_temp = &local_trees_temp[lid];

  bool filter_from_source = true; //otherwise filter from temp
  int nit = *num_iter;
  for(int iter_i = 0; iter_i<nit; ++iter_i)
  {
      for(int i = 0; i < MAX_CELLS; ++i)
      {
	bool valid = filter_from_source?valid_leaf(i,local_tree_source , max_depth[0]):valid_leaf(i,local_tree_temp , max_depth[0]);
	if(valid){
	      //get geometry of current leaf cell
	      int currDepth = get_depth(i);
	      float side_len = 1.0f/((float)(1<<currDepth));
	      float4 cellCenter = (float4) ( (float) index_x + centerX[i],                  //abs center within block
					     (float) index_y + centerY[i],
					     (float) index_z + centerZ[i],
					     0.0f );
	      uchar8 nbr_exist = (uchar8)(0); //local register to temporarily store neighbor existence
	      uchar8 nbr_exp_int = (uchar8)(0); //local register to temporarily store expected intensity
	      float8 nbr_alpha = (float8)(-log(0.001f)/side_len); //local register to temporarily store neighbor alphas
	      float4 lcent; //local center
	      int neighborBitIndex, idx, blkI;
	      float del = 0.0f, exint = 0.0f, nrm = (float)NORM;
	      float8 data;

	      // compute the neighbor locations and test if they exist
	      // just unroll here to avoid a lot of argument passing overhead
	      // neighbors along x
	      del = cellCenter.x-side_len;
	      float4 xm = (float4)( del , cellCenter.y, cellCenter.z, 0.0f);
	      nbr_exist.s0 = (uchar)((del > 0)? 1 : 0);
	      if(nbr_exist.s0){
		blkI = calc_blkI( floor(xm.x), floor(xm.y), floor(xm.z), source_linfo->dims);
		neighbor_trees[lid] = filter_from_source?as_uchar16(source_tree_array[blkI]):as_uchar16(temp_tree_array[blkI]);
		lcent = xm - floor(xm);
		neighborBitIndex = (int) traverse_to(neighbor_tree, lcent, currDepth);
		if( valid_leaf(neighborBitIndex, neighbor_tree, max_depth[0])){
		  idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
		  CONVERT_FUNC(udata,(filter_from_source?source_mog_array[idx]:temp_mog_array[idx]));
		  data=convert_float8(udata)/nrm;
		  EXPECTED_INT(exint,data);
		  nbr_exp_int.s0 = (uchar)(exint*nrm);
		  nbr_alpha.s0 = filter_from_source?source_alpha_array[idx]:temp_alpha_array[idx];
		}
	      }

	      del = cellCenter.x+side_len;
	      float4 xp = (float4)( del, cellCenter.y, cellCenter.z, 0.0f);
	      nbr_exist.s1 = (uchar)((del < source_linfo->dims.x)? 1 : 0);
	      if(nbr_exist.s1){
		blkI = calc_blkI( floor(xp.x), floor(xp.y), floor(xp.z), source_linfo->dims);
		neighbor_trees[lid] = filter_from_source?as_uchar16(source_tree_array[blkI]):as_uchar16(temp_tree_array[blkI]);
		lcent = xp - floor(xp);
		neighborBitIndex = (int) traverse_to(neighbor_tree, lcent, currDepth);
		if(valid_leaf(neighborBitIndex, neighbor_tree, max_depth[0])){
		  idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
		  CONVERT_FUNC(udata,(filter_from_source?source_mog_array[idx]:temp_mog_array[idx]));
		  data=convert_float8(udata)/nrm;
		  EXPECTED_INT(exint,data);
		  nbr_exp_int.s1 = (uchar)(exint*nrm);
		  nbr_alpha.s1 = filter_from_source?source_alpha_array[idx]:temp_alpha_array[idx];
		}
	      }

	      // neighbors along y
	      del = cellCenter.y-side_len;
	      float4 ym = (float4)( cellCenter.x, del, cellCenter.z, 0.0f);
	      nbr_exist.s2 = (uchar)((del > 0)? 1 : 0);
	      if(nbr_exist.s2){
		blkI = calc_blkI( floor(ym.x), floor(ym.y), floor(ym.z), source_linfo->dims);
		neighbor_trees[lid] = filter_from_source?as_uchar16(source_tree_array[blkI]):as_uchar16(temp_tree_array[blkI]);
		lcent = ym - floor(ym);
		neighborBitIndex = (int) traverse_to(neighbor_tree, lcent, currDepth);
		if(valid_leaf(neighborBitIndex, neighbor_tree, max_depth[0])){
		  idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
		  CONVERT_FUNC(udata,(filter_from_source?source_mog_array[idx]:temp_mog_array[idx]));
		  data=convert_float8(udata)/nrm;
		  EXPECTED_INT(exint,data);
		  nbr_exp_int.s2 = (uchar)(exint*nrm);
		  nbr_alpha.s2 = filter_from_source?source_alpha_array[idx]:temp_alpha_array[idx];
		}
	      }
	      del = cellCenter.y+side_len;
	      float4 yp = (float4)( cellCenter.x, del, cellCenter.z, 0.0f);
	      nbr_exist.s3 = (uchar)((del < source_linfo->dims.y)? 1 : 0);
	      if(nbr_exist.s3){
		blkI = calc_blkI( floor(yp.x), floor(yp.y), floor(yp.z), source_linfo->dims);
		neighbor_trees[lid] = filter_from_source?as_uchar16(source_tree_array[blkI]):as_uchar16(temp_tree_array[blkI]);
		lcent = yp - floor(yp);
		neighborBitIndex = (int) traverse_to(neighbor_tree, lcent, currDepth);
		if( valid_leaf(neighborBitIndex, neighbor_tree, max_depth[0])){
		  idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
		  CONVERT_FUNC(udata,(filter_from_source?source_mog_array[idx]:temp_mog_array[idx]));
		  data=convert_float8(udata)/nrm;
		  EXPECTED_INT(exint,data);
		  nbr_exp_int.s3 = (uchar)(exint*nrm);
		  nbr_alpha.s3 = filter_from_source?source_alpha_array[idx]:temp_alpha_array[idx];
		}
	      }
	      // neighbors along z
	      del = cellCenter.z-side_len;
	      float4 zm = (float4)( cellCenter.x, cellCenter.y, del, 0.0f);
	      nbr_exist.s4 = (uchar)((del > 0)? 1 : 0);
	      if(nbr_exist.s4){
		blkI = calc_blkI( floor(zm.x), floor(zm.y), floor(zm.z), source_linfo->dims);
		neighbor_trees[lid] = filter_from_source?as_uchar16(source_tree_array[blkI]):as_uchar16(temp_tree_array[blkI]);
		lcent = zm - floor(zm);
		neighborBitIndex = (int) traverse_to(neighbor_tree, lcent, currDepth);
		if( valid_leaf(neighborBitIndex, neighbor_tree, max_depth[0])){
		  idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
		  CONVERT_FUNC(udata,(filter_from_source?source_mog_array[idx]:temp_mog_array[idx]));
		  data=convert_float8(udata)/nrm;
		  EXPECTED_INT(exint,data);
		  nbr_exp_int.s4 = (uchar)(exint*nrm);
		  nbr_alpha.s4 = filter_from_source?source_alpha_array[idx]:temp_alpha_array[idx];
		}
	      }
	      del = cellCenter.z+side_len;
	      float4 zp = (float4)( cellCenter.x, cellCenter.y, del, 0.0f);
	      nbr_exist.s5 = (uchar)((del < source_linfo->dims.z)? 1 : 0);
	      if(nbr_exist.s5){
		blkI = calc_blkI( floor(zp.x), floor(zp.y), floor(zp.z), source_linfo->dims);
		neighbor_trees[lid] = filter_from_source?as_uchar16(source_tree_array[blkI]):as_uchar16(temp_tree_array[blkI]);
		lcent = zp - floor(zp);
		neighborBitIndex = (int) traverse_to(neighbor_tree, lcent, currDepth);
		if( valid_leaf(neighborBitIndex, neighbor_tree, max_depth[0]) ){
		  idx = data_index_relative( neighbor_tree, neighborBitIndex, lookup) + data_index_root(neighbor_tree);
		  CONVERT_FUNC(udata,(filter_from_source?source_mog_array[idx]:temp_mog_array[idx]));
		  data=convert_float8(udata)/nrm;
		  EXPECTED_INT(exint,data);
		  nbr_exp_int.s5 = (uchar)(exint*nrm);
		  nbr_alpha.s5 = filter_from_source?source_alpha_array[idx]:temp_alpha_array[idx];
		}
	      }

	      //data index for the current cell in source
	      int sidx = data_index_relative( local_tree_source, i, lookup) + data_index_root(local_tree_source);
	      //data index for the current cell in temp
	      int tidx = data_index_relative( local_tree_temp, i, lookup) + data_index_root(local_tree_temp);
	      // filter using neighborhood just computed
	      if(filter_from_source){
		MOG_TYPE mog = source_mog_array[sidx];
		filter_mog(&mog, &nbr_exp_int, &nbr_exist, &weights);
		float calpha = source_alpha_array[sidx];
		filter_alpha(&calpha, &nbr_alpha, &nbr_exist, &weights);
		temp_alpha_array[tidx] = calpha;
		temp_mog_array[tidx] = mog;
	      }else{
		MOG_TYPE mog = temp_mog_array[tidx];
		filter_mog(&mog, &nbr_exp_int, &nbr_exist, &weights);
		float calpha = temp_alpha_array[tidx];
		filter_alpha(&calpha, &nbr_alpha, &nbr_exist, &weights);
		temp_alpha_array[sidx] = calpha;
		temp_mog_array[sidx] = mog;
	      }
	}
      }
      filter_from_source = !filter_from_source;
  }

  // if processing ended up with temp holding the result, i.e. "from" is source
  // then the data must be copied back to source
  //note that end of loop complements filter_from_source before reaching here
  if(!filter_from_source)
    {
      for(int i = 0; i < MAX_CELLS; ++i)
	{
	  if(valid_leaf(i, local_tree_temp, max_depth[0])){
	    int sidx = data_index_relative( local_tree_source, i, lookup) + data_index_root(local_tree_source);
	    int tidx = data_index_relative( local_tree_temp, i, lookup) + data_index_root(local_tree_temp);
	    source_alpha_array[sidx]=temp_alpha_array[tidx];
	    source_mog_array[sidx]=temp_mog_array[tidx];
	  }
	}
    }
    }
  }
}
