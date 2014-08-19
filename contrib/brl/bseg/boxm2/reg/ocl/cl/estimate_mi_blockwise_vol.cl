//PUBLIC RELEASE APPROVAL FROM AFRL
//Case Number: RY-14-0126
//PA Approval Number: 88ABW-2014-1143
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable

__kernel void estimate_mi_blockwise_vol(__constant  float           * centerX,
                                        __constant  float           * centerY,
                                        __constant  float           * centerZ,
                                        __constant  uchar           * bit_lookup,             //0-255 num bits lookup table
                                        __global  RenderSceneInfo * sceneA_linfo,                                        __global  RenderSceneInfo * sceneB_linfo,
                                        __global    int4            * sceneA_tree_array,       // tree structure for each block
                                        __global    float           * sceneA_alpha_array,      // alpha for each block
                                        __global    int4            * sceneB_tree_array,       // tree structure for each block
                                        __global    float           * sceneB_alpha_array,      // alpha for each block
                                        __global    float           * translation,
                                        __global    float           * rotation,
                                        __global    float           * scale,
                                        __global    int             * nbins,
                                        __global    int             * max_depth,                  // coarsness or fineness at which voxels should be matched.
                                        __global    int             * global_joint_histogram,
                                        __global    float           * output,
                                        __local     int             * joint_histogram,
                                        __local     uchar16         * local_trees,
                                        __local     uchar           * cumsum_wkgp,
                                        __local     uchar16         * local_trees_A,
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

    int numTrees = sceneA_linfo->dims.x * sceneA_linfo->dims.y * sceneA_linfo->dims.z;
    float4 sceneB_origin = sceneB_linfo->origin;
    float4 sceneB_blk_dims = convert_float4(sceneB_linfo->dims) ;
    float4 sceneB_maxpoint = sceneB_origin + convert_float4(sceneB_linfo->dims) * sceneB_linfo->block_len ;
    //: each thread will work on a sblock of A to match it to block in B
    if (gid < numTrees)
    {
        local_trees_A[lid] = as_uchar16(sceneA_tree_array[gid]);
        int index_x =(int)( (float)gid/(sceneA_linfo->dims.y * sceneA_linfo->dims.z));
        int rem_x   =( gid- (float)index_x*(sceneA_linfo->dims.y * sceneA_linfo->dims.z));
        int index_y = rem_x/sceneA_linfo->dims.z;
        int rem_y =  rem_x - index_y*sceneA_linfo->dims.z;
        int index_z =rem_y;
        if((index_x >= 0 &&  index_x <= sceneA_linfo->dims.x -1 &&
            index_y >= 0 &&  index_y <= sceneA_linfo->dims.y -1 &&
            index_z >= 0 &&  index_z <= sceneA_linfo->dims.z -1  ))
        {
            __local uchar16* local_tree = &local_trees_A[lid];
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

                    //find side length for cell of this level = block_len/2^currDepth
                    float side_len = sceneA_linfo->block_len/(float) (1<<currDepth);
                    //: for each leaf node xform the cell and find the correspondence in another block.
                    //get alpha value for this cell;
                    int dataIndex = data_index_cached(local_tree, i, bit_lookup, cumsum, &cumIndex) + data_index_root(local_tree); //gets absolute position
                    float alpha   = sceneA_alpha_array[dataIndex];
                    //get alpha value
                    float probA = 1 - exp(-alpha * side_len);
                    float xg = sceneA_linfo->origin.x + ((float)index_x+centerX[i])*sceneA_linfo->block_len ;
                    float yg = sceneA_linfo->origin.y + ((float)index_y+centerY[i])*sceneA_linfo->block_len ;
                    float zg = sceneA_linfo->origin.z + ((float)index_z+centerZ[i])*sceneA_linfo->block_len ;

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
                                int hist_index_B =clamp((int)(0.5+((*nbins)-1)*probB),0,(*nbins)-1);
                                int hist_index_A =clamp((int)(0.5+((*nbins)-1)*probA),0,(*nbins)-1);
                                int index = hist_index_A*(*nbins)+hist_index_B;
                                atomic_inc(&global_joint_histogram[index]);
                            }
                        }
                    }
                }
            }
        }
    }
}
