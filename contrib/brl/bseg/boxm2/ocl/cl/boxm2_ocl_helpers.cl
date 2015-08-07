inline bool is_at_level(int level,__local uchar16* tree, int i){
        int pi = (i-1)>>3;           //Bit_index of parent bit
        int currDepth = get_depth(i);
        bool validParent = tree_bit_at(tree, pi) || (i==0); // special case for roo
        return validParent && level == currDepth;
}

inline int calc_blkI(float cell_minx, float cell_miny, float cell_minz, int4 dims)
{
  return convert_int(cell_minz + (cell_miny + cell_minx*dims.y)*dims.z);
}

inline void get_max_inner_outer(int* MAX_INNER_CELLS, int* MAX_CELLS, int root_level)
{
  //USE rootlevel to determine MAX_INNER and MAX_CELLS
  if(root_level == 0)
    *MAX_INNER_CELLS=1, *MAX_CELLS=1;
  if(root_level == 1)
    *MAX_INNER_CELLS=1, *MAX_CELLS=9;
  else if (root_level == 2)
    *MAX_INNER_CELLS=9, *MAX_CELLS=73;
  else if (root_level == 3)
    *MAX_INNER_CELLS=73, *MAX_CELLS=585;
}


int get_parent_lvl(int level,int i){
  int depth = get_depth(i);
  int p_index = (i-1)>>3;
  if (depth <= level)
    return i;
  depth = get_depth(p_index);
  while (depth!=level){
    p_index = (p_index-1)>>3;
    depth = get_depth(p_index);
  }
  return p_index;

}

// dec: ifdef'ing this fn out because it doesn't seem to be called, and missing defition of "traverse_three_lvl", causing compile problems.
#if 0
int get_data_index_stream_cache(float x,float y,float z,int lid,int level,unsigned int* alpha_offset,__constant int* sceneB_bbox_ids,__constant float* sceneB_blk_dims,
                                __constant float* sceneB_origin,__global float*  sub_block_len, __global unsigned int* sceneB_tree_offsets,__global unsigned int* sceneB_alpha_offsets,
        float* cell_len,__global int4* sceneB_trees,__local uchar16* local_trees_B,__constant uchar* bit_lookup){

  int blk_num_x = sceneB_bbox_ids[6];
  int blk_num_y = sceneB_bbox_ids[7];
  int blk_num_z = sceneB_bbox_ids[8];

  int blk_index_x = (int) floor((x - sceneB_origin[0])/sceneB_blk_dims[0] );
  int blk_index_y = (int) floor((y - sceneB_origin[1])/sceneB_blk_dims[1] );
  int blk_index_z = (int) floor((z - sceneB_origin[2])/sceneB_blk_dims[2] ); //determine the block subscript index of the coordinate


  if (blk_index_x >= sceneB_bbox_ids[0] && blk_index_x<=sceneB_bbox_ids[3] &&
      blk_index_y >= sceneB_bbox_ids[1] && blk_index_y<=sceneB_bbox_ids[4] &&
      blk_index_z >= sceneB_bbox_ids[2] && blk_index_z<=sceneB_bbox_ids[5])
    {
      unsigned int blk_offset_index = blk_index_x * ( sceneB_bbox_ids[5] - sceneB_bbox_ids[2] + 1 )* ( sceneB_bbox_ids[4] - sceneB_bbox_ids[1]+1 )
        + blk_index_y * ( sceneB_bbox_ids[5] - sceneB_bbox_ids[2] + 1 )
        + blk_index_z; // determine the block linear index of each coordinate

      float local_blk_x = x - sceneB_origin[0]- (blk_index_x -  sceneB_bbox_ids[0])*sceneB_blk_dims[0];
      float local_blk_y = y - sceneB_origin[1]- (blk_index_y -  sceneB_bbox_ids[1])*sceneB_blk_dims[1];
      float local_blk_z = z - sceneB_origin[2]- (blk_index_z -  sceneB_bbox_ids[2])*sceneB_blk_dims[2]; //local position (in trees) within block .
      int tree_index_x = (int) floor((local_blk_x)/(*sub_block_len));
      int tree_index_y = (int) floor((local_blk_y)/(*sub_block_len));
      int tree_index_z = (int) floor((local_blk_z)/(*sub_block_len));
      // Tree index subscript from local coordiante
      int tree_offset = tree_index_x * ( blk_num_y)* ( blk_num_z) +tree_index_y * ( blk_num_z)+tree_index_z;

      unsigned int offset = sceneB_tree_offsets[blk_offset_index] + tree_offset;
      //tree offset. offset the number of trees to get to the current block then offset the tree_offset
      local_trees_B[lid] = as_uchar16(sceneB_trees[offset]);
      __local  uchar*   curr_tree_ptr = &local_trees_B[lid];

      float local_tree_x = (local_blk_x)/(*sub_block_len) - tree_index_x;
      float local_tree_y = (local_blk_y)/(*sub_block_len) - tree_index_y;
      float local_tree_z = (local_blk_z)/(*sub_block_len) - tree_index_z;
      // coordinate offset within tree.
      float cell_minx,cell_miny,cell_minz;unsigned bit_index;
      bit_index =  traverse_three_lvl(curr_tree_ptr,local_tree_x,local_tree_y,local_tree_z,
                                      &cell_minx,&cell_miny,&cell_minz, cell_len,level);
      /* if (is_at_level(level,curr_tree_ptr,bit_index)) */
      /*        return 0; */

      unsigned int alpha_blk_offset = data_index_root(curr_tree_ptr)+data_index_relative(curr_tree_ptr,bit_index,bit_lookup);
      *alpha_offset = sceneB_alpha_offsets[blk_offset_index]+ alpha_blk_offset;


      return 1;

}else
    return 0;
}
#endif

void get_component(unsigned i,__global float* unpacked_mean,float8* component_mean){
  unsigned pos =0;
  float* component_mean_ptr= (float*)component_mean;
  for(unsigned j=i * 6;j< i * 6+6;j++)
    component_mean_ptr[pos++]=unpacked_mean[j];
}
inline float gauss_prob_density_f8(float8 x, float8 mu, float8 sigma)
{
  if ( any(sigma <= 0.0f) )
    return 1.0f;

  float8 pwr = (x-mu)*(x-mu) / (sigma*sigma);
  return (0.004031442f * (1.0f/(sigma.s0 * sigma.s1 * sigma.s2 * sigma.s3 * sigma.s4 * sigma.s5)))
    * exp(-0.5f* (pwr.s0 + pwr.s1 + pwr.s2 + pwr.s3 + pwr.s4 + pwr.s5));

}

void unpack_rgb_mean(int16* packed_mean, float* unpacked_mean,float* unpacked_var){
  int* mixture_ptr = (int*)(packed_mean);
  for(short i = 0; i < 8;i++){
    float4 tmp_mu = convert_float4(as_uchar4(mixture_ptr[2*i]))/255.0f ;
    float4 tmp_var = convert_float4(as_uchar4(mixture_ptr[2*i+1]))/255.0f ;
    float* tmp_mu_ptr = (float*)(&tmp_mu);
    float* tmp_var_ptr = (float*)(&tmp_var);
      for(short k=0; k < 3;k++){
        unpacked_mean[ 3 * i + k]=tmp_mu_ptr[k];
        unpacked_var [ 3 * i + k]=tmp_var_ptr[k];
      }
  }
}

void pack_rgb_mean(int16* packed_mean, float* unpacked_mean,float* unpacked_var){
  int* mixture_ptr = (int*)(packed_mean);
  for(short i = 0; i < 8; i++) {
    float4 tmp_mu,tmp_var;
    float* tmp_mu_ptr = (float*)(&tmp_mu);
    float* tmp_var_ptr = (float*)(&tmp_var);
    for (short k = 0; k < 3; k++){
      tmp_mu_ptr[k] = unpacked_mean[3 * i + k];
      tmp_var_ptr[k]= unpacked_var[3 * i + k];
    }
    tmp_mu.s3 = 0.0f;tmp_var.s3=0.0f;
    mixture_ptr[2*i] = as_int(convert_uchar4(tmp_mu * 255.0f));
    mixture_ptr[2*i+1] = as_int(convert_uchar4(tmp_var * 255.0f));
  }
}
