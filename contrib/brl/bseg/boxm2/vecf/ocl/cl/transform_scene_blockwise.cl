#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable

__kernel void transform_scene_blockwise(__constant  float           * centerX,//0
                                        __constant  float           * centerY,//1
                                        __constant  float           * centerZ,//2
                                        __constant  uchar           * bit_lookup,//3             //0-255 num bits lookup table
                                        __global  RenderSceneInfo * target_scene_linfo,//4
                                        __global  RenderSceneInfo * source_scene_linfo,//5
                                        __global    int4            * target_scene_tree_array,//6       // tree structure for each block
                                        __global    float           * target_scene_alpha_array,//7      // alpha for each block
					__global    MOG_TYPE        * target_scene_mog_array,//8        // appearance for each block
                                        __global    int4            * source_scene_tree_array,//9       // tree structure for each block
                                        __global    float           * source_scene_alpha_array,//10      // alpha for each block
					__global    MOG_TYPE        * source_scene_mog_array,//11        // appearance for each block
                                        __global    float           * translation,//12
                                        __global    float           * rotation,//13
                                        __global    float           * scale,//14
                                        __global    int             * max_depth,//15               // coarsness or fineness
					                                                       //at which voxels should be matched.
                                        __global    float           * output,//16
					__local     uchar           * cumsum_wkgp,//17
                                        __local     uchar16         * local_trees_target,//18
                                        __local     uchar16         * local_trees_source)//19
{
    int gid = get_global_id(0);
    int lid = get_local_id(0);
    int  MAX_CELLS = 1;
    if(max_depth[0] == 1)
      MAX_CELLS=9;
    else if (max_depth[0] == 2)
      MAX_CELLS=73;
    else if (max_depth[0] == 3)
      MAX_CELLS=585;
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
    if (gid < numTrees)
    {
        local_trees_target[lid] = as_uchar16(target_scene_tree_array[gid]);
        int index_x =(int)( (float)gid/(target_scene_linfo->dims.y * target_scene_linfo->dims.z));
        int rem_x   =( gid- (float)index_x*(target_scene_linfo->dims.y * target_scene_linfo->dims.z));
        int index_y = rem_x/target_scene_linfo->dims.z;
        int rem_y =  rem_x - index_y*target_scene_linfo->dims.z;
        int index_z =rem_y;
        if((index_x >= 0 &&  index_x <= target_scene_linfo->dims.x -1 &&
            index_y >= 0 &&  index_y <= target_scene_linfo->dims.y -1 &&
            index_z >= 0 &&  index_z <= target_scene_linfo->dims.z -1  ))
        {
            __local uchar16* local_tree = &local_trees_target[lid];
	    __local uchar * cumsum = &cumsum_wkgp[lid*10];
            // iterate through leaves
            cumsum[0] = (*local_tree).s0;
            int cumIndex = 1;
            for (int i=0; i<MAX_CELLS; i++) {
                //if current bit is 0 and parent bit is 1, you're at a leaf
                int pi = (i-1)>>3;           //Bit_index of parent bit
                bool validParent = tree_bit_at(local_tree, pi) || (i==0); // special case for root
                int currDepth = get_depth(i);
                if (validParent && ( tree_bit_at(local_tree, i)==0 || currDepth== max_depth[0] )) {

                    //: for each leaf node xform the cell and find the correspondence in another block.

                    //get the index into this cell data
                    int dataIndex = data_index_cached(local_tree, i, bit_lookup, cumsum, &cumIndex) + data_index_root(local_tree); //gets absolute position
		    // set intial values in case source is not accessed
		    target_scene_mog_array[dataIndex] = mog_init;
		    target_scene_alpha_array[dataIndex] = alpha_init;
		    // transform coordinates to source scene
                    float xg = target_scene_linfo->origin.x + ((float)index_x+centerX[i])*target_scene_linfo->block_len ;
                    float yg = target_scene_linfo->origin.y + ((float)index_y+centerY[i])*target_scene_linfo->block_len ;
                    float zg = target_scene_linfo->origin.z + ((float)index_z+centerZ[i])*target_scene_linfo->block_len ;

                    float txg = scale[0]*(rotation[0]*xg +rotation[1]*yg + rotation[2]*zg) + translation[0];
                    float tyg = scale[1]*(rotation[3]*xg +rotation[4]*yg + rotation[5]*zg) + translation[1];
                    float tzg = scale[2]*(rotation[6]*xg +rotation[7]*yg + rotation[8]*zg) + translation[2];
		    // is the transformed point inside the source domain
                    if(txg > source_scene_origin.x && txg < source_scene_maxpoint.x &&
                       tyg > source_scene_origin.y && tyg < source_scene_maxpoint.y &&
                       tzg > source_scene_origin.z && tzg < source_scene_maxpoint.z )
                    {
		      // source sub block indices
                        int s_sub_blk_x = (int) floor((txg - source_scene_origin.x)/(source_scene_linfo->block_len)) ;
                        int s_sub_blk_y = (int) floor((tyg - source_scene_origin.y)/(source_scene_linfo->block_len)) ;
                        int s_sub_blk_z = (int) floor((tzg - source_scene_origin.z)/(source_scene_linfo->block_len)) ;

			// convert to linear tree index
                        int s_tree_index = s_sub_blk_x * ( source_scene_blk_dims.y)*
			  ( source_scene_blk_dims.z) +s_sub_blk_y * (source_scene_blk_dims.z)+s_sub_blk_z;

                        local_trees_source[lid] = as_uchar16(source_scene_tree_array[s_tree_index]);
                        __local uchar * curr_tree_ptr = &local_trees_source[lid];

                        float source_tree_lx = clamp((txg - source_scene_origin.x)/source_scene_linfo->block_len - s_sub_blk_x,0.0f,1.0f);
                        float source_tree_ly = clamp((tyg - source_scene_origin.y)/source_scene_linfo->block_len - s_sub_blk_y,0.0f,1.0f);
                        float source_tree_lz = clamp((tzg - source_scene_origin.z)/source_scene_linfo->block_len - s_sub_blk_z,0.0f,1.0f);

                        float cell_minx,cell_miny,cell_minz,cell_len;
                        ushort bit_index = traverse_deepest(curr_tree_ptr,source_tree_lx,source_tree_ly,source_tree_lz,
                                                             &cell_minx,&cell_miny,&cell_minz,&cell_len, max_depth[0] );

                        if(bit_index >=0 && bit_index < MAX_CELLS )
                        {
                            unsigned int alpha_offset = data_index_root(curr_tree_ptr)+data_index_relative(curr_tree_ptr,bit_index,bit_lookup);
                            if( alpha_offset >=0  && alpha_offset < source_scene_linfo->data_len )
                            {
			      target_scene_alpha_array[dataIndex] = source_scene_alpha_array[alpha_offset];
		    	      target_scene_mog_array[dataIndex]   = source_scene_mog_array[alpha_offset];
                            }
                        }
                    }
                }
            }
        }
    }
}
