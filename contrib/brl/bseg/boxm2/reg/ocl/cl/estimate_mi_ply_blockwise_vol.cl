//PUBLIC RELEASE APPROVAL FROM AFRL
//Case Number: RY-14-0126
//PA Approval Number: 88ABW-2014-1143
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable


inline void AtomicAdd(volatile __global float *source, const float operand) {
    union {
        unsigned int intVal;
        float floatVal;
    } newVal;
    union {
        unsigned int intVal;
        float floatVal;
    } prevVal;
    do {
        prevVal.floatVal = *source;
        newVal.floatVal = prevVal.floatVal + operand;
    } while (atomic_cmpxchg((volatile __global unsigned int *)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);
}

__kernel void estimate_mi_blockwise_vol(__constant  float           * centerX,
                                        __constant  float           * centerY,
                                        __constant  float           * centerZ,
                                        __constant  uchar           * bit_lookup,             //0-255 num bits lookup table
                                        __global  RenderSceneInfo * sceneB_linfo,
                                        __global    int4            * sceneB_tree_array,       // tree structure for each block
                                        __global    float           * sceneB_alpha_array,      // alpha for each block
                                        __global    float           * sceneA_pts,       // tree structure for each block
                                        __global    int	            * num_sceneA_pts,      // alpha for each block
                                        __global    float           * translation,
                                        __global    float           * rotation,
                                        __global    float           * scale,
                                        __global    int             * max_depth,                  // coarsness or fineness at which voxels should be matched.
                                        __global    float           * global_sum,
                                        __global    float           * output,
                                        __local     uchar           * cumsum_wkgp,
                                        __local     uchar16         * local_trees_B)   // cumulative sum helper for data pointer*/
{
    int gid = get_global_id(0);
    int lid = get_local_id(0);
    float s = *scale;
    int  MAX_CELLS = 1;
    if(max_depth[0] == 1)
      MAX_CELLS=9;
    else if (max_depth[0] == 2)
      MAX_CELLS=73;
    else if (max_depth[0] == 3)
      MAX_CELLS=585;

    int numpts = *num_sceneA_pts;
    float4 sceneB_origin = sceneB_linfo->origin;
    float4 sceneB_blk_dims = convert_float4(sceneB_linfo->dims) ;
    float4 sceneB_maxpoint = sceneB_origin + convert_float4(sceneB_linfo->dims) * sceneB_linfo->block_len ;
    //: each thread will work on a sblock of A to match it to block in B
    if (gid < numpts)
    {
        float xg = sceneA_pts[gid*3+0];
		float yg = sceneA_pts[gid*3+1];
		float zg = sceneA_pts[gid*3+2];


		float txg = s*(rotation[0]*xg +rotation[1]*yg + rotation[2]*zg + translation[0]);
		float tyg = s*(rotation[3]*xg +rotation[4]*yg + rotation[5]*zg + translation[1]);
		float tzg = s*(rotation[6]*xg +rotation[7]*yg + rotation[8]*zg + translation[2]);

		if(txg > sceneB_origin.x && txg < sceneB_maxpoint.x &&
		   tyg > sceneB_origin.y && tyg < sceneB_maxpoint.y &&
		   tzg > sceneB_origin.z && tzg < sceneB_maxpoint.z )
        {
			int b_sub_blk_x = (int) floor((txg - sceneB_origin.x)/(sceneB_linfo->block_len)) ;
			int b_sub_blk_y = (int) floor((tyg - sceneB_origin.y)/(sceneB_linfo->block_len)) ;
			int b_sub_blk_z = (int) floor((tzg - sceneB_origin.z)/(sceneB_linfo->block_len)) ;

			int b_tree_index = b_sub_blk_x * ( sceneB_blk_dims.y)* ( sceneB_blk_dims.z) +b_sub_blk_y * (sceneB_blk_dims.z)+b_sub_blk_z;

			local_trees_B[lid] = as_uchar16(sceneB_tree_array[b_tree_index]);
			__local uchar * curr_tree_ptr = &local_trees_B[lid];

			float B_tree_lx = clamp((txg - sceneB_origin.x)/sceneB_linfo->block_len - b_sub_blk_x,0.0f,1.0f);
			float B_tree_ly = clamp((tyg - sceneB_origin.y)/sceneB_linfo->block_len - b_sub_blk_y,0.0f,1.0f);
			float B_tree_lz = clamp((tzg - sceneB_origin.z)/sceneB_linfo->block_len - b_sub_blk_z,0.0f,1.0f);

			float cell_minx,cell_miny,cell_minz,cell_len;
			ushort bit_index = traverse_deepest(curr_tree_ptr,B_tree_lx,B_tree_ly,B_tree_lz,
												 &cell_minx,&cell_miny,&cell_minz,&cell_len, max_depth[0] );

			if(bit_index >=0 && bit_index < 585 )
			{
				unsigned int alpha_offset = data_index_root(curr_tree_ptr)+data_index_relative(curr_tree_ptr,bit_index,bit_lookup);
				if( alpha_offset >=0  && alpha_offset < sceneB_linfo->data_len )
				{
					float alphaB = sceneB_alpha_array[alpha_offset];
					float probB = 1 - exp(-alphaB*cell_len*(sceneB_linfo->block_len));

					AtomicAdd(&global_sum[0],probB);
				}
			}

        }
    }
}
