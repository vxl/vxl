#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable

__kernel
void estimate_mi_depth_map_to_vol(__constant  uchar        * bit_lookup,       //0-255 num bits lookup table
                                  __global    float        * xdepth,
                                  __global    float        * ydepth,
                                  __global    float        * zdepth,
                                  __constant  float        * sceneB_origin,
                                  __constant  int          * sceneB_bbox_ids,  // bbox : [minx,miny,minz,maxx,maxy,maxz]
                                  __constant  float        * sceneB_blk_dims,  // [ width, depth, height ]
                                  __global    float        * sub_block_len,    // [ width, depth, height ]
                                  __global    int4         * sceneB_trees,
                                  __global    float        * sceneB_alphas,
                                  __global    unsigned int * sceneB_tree_offsets,
                                  __global    unsigned int * sceneB_alpha_offsets,
                                  __global    float        * translation,
                                  __global    float        * rotation,
                                  __global    int          * nbins,
                                  __global    int          * global_joint_histogram,
                                  __global    float        * output,
                                  __local     int          * joint_histogram,
                                  __local     uchar16      * local_trees_B)   // cumulative sum helper for data pointer
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);


  int img_index = j*get_global_size(0)+i ;
  // todo cjheck for i,j
  int blk_num_x = sceneB_bbox_ids[6];
  int blk_num_y = sceneB_bbox_ids[7];
  int blk_num_z = sceneB_bbox_ids[8];

  if (llid == 0)
  {
    for (unsigned int k = 0; k < (*nbins)*(*nbins); k++)
      joint_histogram[k] = 0 ;
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  float x = xdepth[img_index];
  float y = ydepth[img_index];
  float z = zdepth[img_index];

  float xformed_x = rotation[0]*x +rotation[1]*y + rotation[2]*z + translation[0];
  float xformed_y = rotation[3]*x +rotation[4]*y + rotation[5]*z + translation[1];
  float xformed_z = rotation[6]*x +rotation[7]*y + rotation[8]*z + translation[2];

  int blk_index_x = (int) floor((xformed_x - sceneB_origin[0])/sceneB_blk_dims[0] );
  int blk_index_y = (int) floor((xformed_y - sceneB_origin[1])/sceneB_blk_dims[1] );
  int blk_index_z = (int) floor((xformed_z - sceneB_origin[2])/sceneB_blk_dims[2] );

  if (blk_index_x >= sceneB_bbox_ids[0] && blk_index_x<=sceneB_bbox_ids[3] &&
      blk_index_y >= sceneB_bbox_ids[1] && blk_index_y<=sceneB_bbox_ids[4] &&
      blk_index_z >= sceneB_bbox_ids[2] && blk_index_z<=sceneB_bbox_ids[5])
  {
    int blk_offset_index = blk_index_x * ( sceneB_bbox_ids[5] - sceneB_bbox_ids[2] + 1 )* ( sceneB_bbox_ids[4] - sceneB_bbox_ids[1]+1 )
                         + blk_index_y * ( sceneB_bbox_ids[5] - sceneB_bbox_ids[2] + 1 )
                         + blk_index_z;

    float local_blk_x = xformed_x - sceneB_origin[0]- (blk_index_x -  sceneB_bbox_ids[0])*sceneB_blk_dims[0];
    float local_blk_y = xformed_y - sceneB_origin[1]- (blk_index_y -  sceneB_bbox_ids[1])*sceneB_blk_dims[1];
    float local_blk_z = xformed_z - sceneB_origin[2]- (blk_index_z -  sceneB_bbox_ids[2])*sceneB_blk_dims[2];

    int tree_index_x = (int) floor((local_blk_x)/(*sub_block_len));
    int tree_index_y = (int) floor((local_blk_y)/(*sub_block_len));
    int tree_index_z = (int) floor((local_blk_z)/(*sub_block_len));

    int tree_offset = tree_index_x * ( blk_num_y)* ( blk_num_z) +tree_index_y * ( blk_num_z)+tree_index_z;
    unsigned int offset = sceneB_tree_offsets[blk_offset_index] + tree_offset;

    local_trees_B[llid] = as_uchar16(sceneB_trees[offset]);
    __local uchar * curr_tree_ptr = &local_trees_B[llid];

    float local_tree_x = (local_blk_x)/(*sub_block_len) - tree_index_x;
    float local_tree_y = (local_blk_y)/(*sub_block_len) - tree_index_y;
    float local_tree_z = (local_blk_z)/(*sub_block_len) - tree_index_z;

    float cell_minx,cell_miny,cell_minz,cell_len;
    ushort bit_index =  traverse_three(curr_tree_ptr,local_tree_x,local_tree_y,local_tree_z,
                                       &cell_minx,&cell_miny,&cell_minz, &cell_len);

    unsigned int alpha_blk_offset =data_index_root(curr_tree_ptr)+data_index_relative(curr_tree_ptr,bit_index,bit_lookup);
    unsigned int alpha_offset = sceneB_alpha_offsets[blk_offset_index]+ alpha_blk_offset;

    float alphaB = sceneB_alphas[alpha_offset];
    
    float probB  = (1 - exp(-alphaB*cell_len*(*sub_block_len)));
    
    if (probB >= 0.0f && probB <= 1.0f) {
      int hist_index_B =(int)(((*nbins)-1)*probB) ;// (int)clamp((int)floor(probB*(*nbins)),0,(*nbins)-1);
      //int hist_index_A =(int)(((*nbins)-1)*1.0f) ;// (int)clamp((int)floor(prob*(*nbins)),0,(*nbins)-1);
      atom_inc(&joint_histogram[2+hist_index_B]);
    }
  }

  //barrier(CLK_LOCAL_MEM_FENCE);
  //atom_add(&global_joint_histogram[llid],joint_histogram[llid]) ;
  //barrier(CLK_LOCAL_MEM_FENCE);

  if (llid == 0)
  {
    for (unsigned int k = 0; k < (*nbins)*(*nbins); k++)
      atom_add(&global_joint_histogram[k],joint_histogram[k]) ;
  }
  //barrier(CLK_LOCAL_MEM_FENCE);
  barrier(CLK_GLOBAL_MEM_FENCE);
}
