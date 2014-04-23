#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable


#ifdef VISIBILITY_BASED
__kernel void fuse_blockwise_based_visibility(__constant  float           * centerX,
                                              __constant  float           * centerY,
                                              __constant  float           * centerZ,
                                              __constant  uchar           * bit_lookup,             //0-255 num bits lookup table
                                              __global  RenderSceneInfo   * sceneA_linfo,
                                              __global  RenderSceneInfo   * sceneB_linfo,
                                              __global    int4            * sceneA_tree_array,       // tree structure for each block
                                              __global    float           * sceneA_alpha_array,      // alpha for each block
                                              __global    float           * sceneA_vis_score_array,      // alpha for each block
                                              __global    uchar8          * sceneA_app_array,      // alpha for each block
                                              __global    int4            * sceneB_tree_array,       // tree structure for each block
                                              __global    float           * sceneB_alpha_array,      // alpha for each block
                                              __global    float           * sceneB_vis_score_array,      // alpha for each block
                                              __global    uchar8          * sceneB_app_array,
                                              __global    float           * output,
                                              __local     uchar           * cumsum_wkgp,
                                              __local     uchar16         * local_trees_A,
                                              __local     uchar16         * local_trees_B)   // cumulative sum helper for data pointer*/
{
    int gid = get_global_id(0);
    int lid = get_local_id(0);
    int  MAX_CELLS = 585;
    int numTreesA = sceneA_linfo->dims.x * sceneA_linfo->dims.y * sceneA_linfo->dims.z;
    int numTreesB = sceneB_linfo->dims.x * sceneB_linfo->dims.y * sceneB_linfo->dims.z;
    if(numTreesA!= numTreesB)
        return;
    //: each thread will work on a sblock of A to match it to block in B
    if (gid < numTreesA)
    {
        local_trees_A[lid] = as_uchar16(sceneA_tree_array[gid]);
        local_trees_B[lid] = as_uchar16(sceneB_tree_array[gid]);

        __local uchar16* local_tree_A = &local_trees_A[lid];
        __local uchar* A_tree_ptr = &local_trees_A[lid];
        __local uchar * cumsum = &cumsum_wkgp[lid*10];
        __local uchar * B_tree_ptr = &local_trees_B[lid];

        // iterate through leaves
        cumsum[0] = (*local_tree_A).s0;
        int cumIndex = 1;
        for (int i=0; i<MAX_CELLS; i++)
        {
            //if current bit is 0 and parent bit is 1, you're at a leaf
            int pi = (i-1)>>3;           //Bit_index of parent bit
            bool validParent = tree_bit_at(A_tree_ptr, pi) || (i==0); // special case for root
            int currDepth = get_depth(i);
            if (validParent && ( tree_bit_at(A_tree_ptr, i)==0 )) {
                //find side length for cell of this level = block_len/2^currDepth
                float cell_len_A = sceneA_linfo->block_len/(float) (1<<currDepth);
                //: for each leaf node xform the cell and find the correspondence in another block.
                //get alpha value for this cell;
                int dataIndex = data_index_relative(A_tree_ptr, i, bit_lookup) + data_index_root(A_tree_ptr); //gets absolute position
                float vis_score_A = sceneA_vis_score_array[dataIndex];
                float xp = centerX[i];
                float yp = centerY[i];
                float zp = centerZ[i];
                float cell_minx,cell_miny,cell_minz,cell_len_B;
                ushort bit_index = traverse_three(B_tree_ptr,xp,yp,zp,&cell_minx,&cell_miny,&cell_minz,&cell_len_B);
                //printf("%d, ",(int)bit_index);
                if(bit_index >=0 && bit_index < 585 )
                {
                    unsigned int dataIndexB = data_index_root(B_tree_ptr)+data_index_relative(B_tree_ptr,bit_index,bit_lookup);
                    float vis_score_B = sceneB_vis_score_array[dataIndexB];
                    float alphaB = sceneB_alpha_array[dataIndexB];
                    if(vis_score_A < 0.5 && vis_score_B > 0.4)
                    {
                        sceneA_alpha_array[dataIndex] = alphaB* cell_len_B /cell_len_A ;
                        sceneA_app_array[dataIndex] = sceneB_app_array[dataIndexB];
                    }
                }
            }
        }
    }
}


#endif //VISIBILITY_BASED

#ifdef ORIENTATION_BASED
__kernel void fuse_blockwise_based_orientation(__constant  float           * centerX,
                                              __constant  float           * centerY,
                                              __constant  float           * centerZ,
                                              __constant  uchar           * bit_lookup,             //0-255 num bits lookup table
                                              __global  RenderSceneInfo * sceneA_linfo,
                                              __global  RenderSceneInfo * sceneB_linfo,
                                              __global    int4            * sceneA_tree_array,       // tree structure for each block
                                              __global    float           * sceneA_alpha_array,      // alpha for each block
                                              __global    float           * sceneA_expectation_array,      // alpha for each block
                                              __global    float           * sceneA_vis_array,      // alpha for each block
                                              __global    uchar8          * sceneA_app_array,      // alpha for each block
                                              __global    int4            * sceneB_tree_array,       // tree structure for each block
                                              __global    float           * sceneB_alpha_array,      // alpha for each block
                                              __global    float           * sceneB_expectation_array,      // alpha for each block
                                              __global    float           * sceneB_vis_array,      // alpha for each block
                                              __global    uchar8          * sceneB_app_array,
                                              __global    float           * output,
                                              __local     uchar           * cumsum_wkgp,
                                              __local     uchar16         * local_trees_A,
                                              __local     uchar16         * local_trees_B)   // cumulative sum helper for data pointer*/
{
    int gid = get_global_id(0);
    int lid = get_local_id(0);
    int  MAX_CELLS = 585;

    int numTreesA = sceneA_linfo->dims.x * sceneA_linfo->dims.y * sceneA_linfo->dims.z;
    int numTreesB = sceneB_linfo->dims.x * sceneB_linfo->dims.y * sceneB_linfo->dims.z;
    if(numTreesA!= numTreesB)
        return;
    //: each thread will work on a sblock of A to match it to block in B
    if (gid < numTreesA)
    {
        local_trees_A[lid] = as_uchar16(sceneA_tree_array[gid]);
        local_trees_B[lid] = as_uchar16(sceneB_tree_array[gid]);
        //    printf("%d,%d\n",gid,lid);
        __local uchar16* local_tree_A = &local_trees_A[lid];
        __local uchar* A_tree_ptr = &local_trees_A[lid];
        __local uchar * cumsum = &cumsum_wkgp[lid*10];
        __local uchar * B_tree_ptr = &local_trees_B[lid];

        // iterate through leaves
        cumsum[0] = (*local_tree_A).s0;
        int cumIndex = 1;
        for (int i=0; i<MAX_CELLS; i++)
        {
            //if current bit is 0 and parent bit is 1, you're at a leaf
            int pi = (i-1)>>3;           //Bit_index of parent bit
            bool validParent = tree_bit_at(A_tree_ptr, pi) || (i==0); // special case for root
            int currDepth = get_depth(i);
            if (validParent && ( tree_bit_at(A_tree_ptr, i)==0 )) {
                //find side length for cell of this level = block_len/2^currDepth
                float cell_len_A = sceneA_linfo->block_len/(float) (1<<currDepth);
                //: for each leaf node xform the cell and find the correspondence in another block.
                //get alpha value for this cell;
                int dataIndex = data_index_relative(A_tree_ptr, i, bit_lookup) + data_index_root(A_tree_ptr); //gets absolute position
                float vis_A = sceneA_vis_array[dataIndex];
                float exp_A = sceneA_expectation_array[dataIndex];
                float alphaA = sceneA_alpha_array[dataIndex];
                float score_A = 0.0f ;
                if( vis_A > 1e-3f)
                    score_A = exp_A/vis_A;

                float xp = centerX[i];
                float yp = centerY[i];
                float zp = centerZ[i];
                float cell_minx,cell_miny,cell_minz,cell_len_B;
                ushort bit_index = traverse_three(B_tree_ptr,xp,yp,zp,&cell_minx,&cell_miny,&cell_minz,&cell_len_B);
                if(bit_index >=0 && bit_index < 585 )
                {
                    unsigned int dataIndexB = data_index_root(B_tree_ptr)+data_index_relative(B_tree_ptr,bit_index,bit_lookup);
                    float vis_B = sceneB_vis_array[dataIndexB];
                    float exp_B = sceneB_expectation_array[dataIndexB];

                    float score_B = 0.0f ;
                    if( vis_B > 1e-3f)
                        score_B = exp_B/vis_B;

                    float alphaB = sceneB_alpha_array[dataIndexB];

                    if(score_A > 0.0 && score_B > 0.0 )
                    {

                        sceneA_alpha_array[dataIndex] =(alphaA*score_A + 4* alphaB *score_B)/(score_A+score_B);
                        //printf("%f,%f\n",alphaA,sceneA_alpha_array[dataIndex]);
                        if(score_B > score_A )//&& score_A < 0.5 )
                        {
                            //sceneA_alpha_array[dataIndex] = alphaB;
                            sceneA_app_array[dataIndex] = sceneB_app_array[dataIndexB];
                        }
                    }
                    //printf("%f,%f,%f,%f\n",alphaA,alphaB,score_A,score_B);
                    //sceneA_alpha_array[dataIndex] = alphaA ;//(alphaA*score_A )/score_A ;//+ alphaB* cell_len_B /cell_len_A *score_B)/(score_A+score_B) ;
                }
            }
        }
    }
}


#endif //VISIBILITY_BASED

#ifdef SURFACE_DENSITY_BASED
__kernel void fuse_blockwise_based_surface_density(__constant  float           * centerX,
                                              __constant  float           * centerY,
                                              __constant  float           * centerZ,
                                              __constant  uchar           * bit_lookup,             //0-255 num bits lookup table
                                              __global  RenderSceneInfo * sceneA_linfo,
                                              __global  RenderSceneInfo * sceneB_linfo,
                                              __global    int4            * sceneA_tree_array,       // tree structure for each block
                                              __global    float           * sceneA_alpha_array,      // alpha for each block
                                              __global    float           * sceneA_expectation_array,      // alpha for each block
                                              __global    float           * sceneA_vis_array,      // alpha for each block
                                              __global    uchar8          * sceneA_app_array,      // alpha for each block
                                              __global    int4            * sceneB_tree_array,       // tree structure for each block
                                              __global    float           * sceneB_alpha_array,      // alpha for each block
                                              __global    float           * sceneB_expectation_array,      // alpha for each block
                                              __global    float           * sceneB_vis_array,      // alpha for each block
                                              __global    uchar8          * sceneB_app_array,
                                              __global    float           * output,
                                              __local     uchar           * cumsum_wkgp,
                                              __local     uchar16         * local_trees_A,
                                              __local     uchar16         * local_trees_B)   // cumulative sum helper for data pointer*/
{
    int gid = get_global_id(0);
    int lid = get_local_id(0);
    int  MAX_CELLS = 585;
    int numTreesA = sceneA_linfo->dims.x * sceneA_linfo->dims.y * sceneA_linfo->dims.z;
    int numTreesB = sceneB_linfo->dims.x * sceneB_linfo->dims.y * sceneB_linfo->dims.z;
    if(numTreesA!= numTreesB)
        return;
    //: each thread will work on a sblock of A to match it to block in B
    if (gid < numTreesA)
    {
        local_trees_A[lid] = as_uchar16(sceneA_tree_array[gid]);
        local_trees_B[lid] = as_uchar16(sceneB_tree_array[gid]);

        __local uchar16* local_tree_A = &local_trees_A[lid];
        __local uchar* A_tree_ptr = &local_trees_A[lid];
        __local uchar * cumsum = &cumsum_wkgp[lid*10];
        __local uchar * B_tree_ptr = &local_trees_B[lid];

        // iterate through leaves
        cumsum[0] = (*local_tree_A).s0;
        int cumIndex = 1;
        for (int i=0; i<MAX_CELLS; i++)
        {
            //if current bit is 0 and parent bit is 1, you're at a leaf
            int pi = (i-1)>>3;           //Bit_index of parent bit
            bool validParent = tree_bit_at(A_tree_ptr, pi) || (i==0); // special case for root
            int currDepth = get_depth(i);
            if (validParent && ( tree_bit_at(A_tree_ptr, i)==0 )) {
                //find side length for cell of this level = block_len/2^currDepth
                float cell_len_A = sceneA_linfo->block_len/(float) (1<<currDepth);
                //: for each leaf node xform the cell and find the correspondence in another block.
                //get alpha value for this cell;
                int dataIndex = data_index_relative(A_tree_ptr, i, bit_lookup) + data_index_root(A_tree_ptr); //gets absolute position
                float vis_A = sceneA_vis_array[dataIndex];
                float exp_A = sceneA_expectation_array[dataIndex];
                float alphaA = sceneA_alpha_array[dataIndex];
                float score_A = 0.0f ;
                if( vis_A > 1e-3f)
                    score_A = exp_A/vis_A;

                float xp = centerX[i];
                float yp = centerY[i];
                float zp = centerZ[i];
                float cell_minx,cell_miny,cell_minz,cell_len_B;
                ushort bit_index = traverse_three(B_tree_ptr,xp,yp,zp,&cell_minx,&cell_miny,&cell_minz,&cell_len_B);
                if(bit_index >=0 && bit_index < 585 )
                {
                    unsigned int dataIndexB = data_index_root(B_tree_ptr)+data_index_relative(B_tree_ptr,bit_index,bit_lookup);
                    float vis_B = sceneB_vis_array[dataIndexB];
                    float exp_B = sceneB_expectation_array[dataIndexB];

                    float score_B = 0.0f ;
                    if( vis_B > 1e-3f)
                        score_B = exp_B/vis_B;

                    float alphaB = sceneB_alpha_array[dataIndexB];
                    if(score_A > 0.0 || score_B > 0.0 )
                    {
                        sceneA_alpha_array[dataIndex] =(alphaA*score_B + alphaB *score_A)/(score_A+score_B);
                        if(score_B < score_A )
                        {
                           // sceneA_app_array[dataIndex] = sceneB_app_array[dataIndexB];
                        }
                    }
                }
            }
        }
    }
}


#endif //SURFACE_DENSITY_BASED