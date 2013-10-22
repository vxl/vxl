#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable

__kernel void estimate_mi_vol(__constant  RenderSceneInfo * linfo,
                              __constant  float           * centerX,
                              __constant  float           * centerY,
                              __constant  float           * centerZ,
                              __constant  uchar           * bit_lookup,       //0-255 num bits lookup table
                              __global    int4            * tree_array,       // tree structure for each block
                              __global    float           * alpha_array,      // alpha for each block
                              __constant  float           * sceneB_origin,
                              __constant  int             * sceneB_bbox_ids,  // bbox : [minx,miny,minz,maxx,maxy,maxz]
                              __constant  float           * sceneB_blk_dims,  // [ width, depth, height ]
                              __global    float           * sub_block_len,    // [ width, depth, height ]
                              __global    int4            * sceneB_trees,
                              __global    float           * sceneB_alphas,
                              __global    unsigned int    * sceneB_tree_offsets,
                              __global    unsigned int    * sceneB_alpha_offsets,
                              __global    float           * translation,
                              __global    float           * rotation,
                              __global    float           * scale,
                              __global    int             * nbins,
                              __global    int             * depth,                  // coarsness or fineness at which voxels should be matched.
                              __global    int             * global_joint_histogram,
                              __global    float           * output,
                              __local     int             * joint_histogram,
                              __local     uchar16         * local_trees,
                              __local     uchar           * cumsum_wkgp,
                              __local     uchar16         * local_trees_B)   // cumulative sum helper for data pointer
{

    int gid = get_global_id(0);
    int lid = get_local_id(0);
    int blk_num_x = sceneB_bbox_ids[6];
    int blk_num_y = sceneB_bbox_ids[7];
    int blk_num_z = sceneB_bbox_ids[8];
    float s = *scale;
    int numTrees = linfo->dims.x * linfo->dims.y * linfo->dims.z;

    if (gid < numTrees)
    {


        local_trees[lid] = as_uchar16(tree_array[gid]);
        int index_x = gid/(linfo->dims.y * linfo->dims.z);
        int rem_x= gid- index_x*(linfo->dims.y * linfo->dims.z);
        int index_y = rem_x/linfo->dims.z;
        int rem_y =  rem_x - index_y*linfo->dims.z;
        int index_z =rem_y;
        //if((index_x == 0 ||  index_x == linfo->dims.x -1 ||
        //    index_y == 0 ||  index_y == linfo->dims.y -1 ||
        //    index_z == 0 ||  index_y == linfo->dims.z -1  ))
        //{
        //}
        //else
        {
            //float x = index_x* linfo->block_len;
            //float y = index_y* linfo->block_len;
            //float z = index_z* linfo->block_len;
            __local uchar16* local_tree = &local_trees[lid];
            __local uchar * cumsum = &cumsum_wkgp[lid*10];
            // iterate through leaves
            cumsum[0] = (*local_tree).s0;
            int cumIndex = 1;

            for (int i=0; i<585; i++) {

                //if current bit is 0 and parent bit is 1, you're at a leaf
                int pi = (i-1)>>3;           //Bit_index of parent bit
                bool validParent = tree_bit_at(local_tree, pi) || (i==0); // special case for root
                int currDepth = get_depth(i);
                if (validParent && ( tree_bit_at(local_tree, i)==0 || currDepth== depth[0] )) {
                    //////////////////////////////////////////////////
                    //LEAF CODE HERE
                    //////////////////////////////////////////////////
                    //find side length for cell of this level = block_len/2^currDepth

                    float side_len = linfo->block_len/(float) (1<<currDepth);

                    //get alpha value for this cell;
                    int dataIndex = data_index_cached(local_tree, i, bit_lookup, cumsum, &cumIndex) + data_index_root(local_tree); //gets absolute position
                    float alpha   = alpha_array[dataIndex];
                    //integrate alpha value
                    float prob = 1 - exp(-alpha * side_len);
                    float orig_x = linfo->origin.x + ((float)index_x+centerX[i])*linfo->block_len ;
                    float orig_y = linfo->origin.y + ((float)index_y+centerY[i])*linfo->block_len ;
                    float orig_z = linfo->origin.z + ((float)index_z+centerZ[i])*linfo->block_len ;
                    float xformed_x = s*(rotation[0]*orig_x +rotation[1]*orig_y + rotation[2]*orig_z + translation[0]);
                    float xformed_y = s*(rotation[3]*orig_x +rotation[4]*orig_y + rotation[5]*orig_z + translation[1]);
                    float xformed_z = s*(rotation[6]*orig_x +rotation[7]*orig_y + rotation[8]*orig_z + translation[2]);
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
                        local_trees_B[lid] = as_uchar16(sceneB_trees[offset]);
                        __local uchar * curr_tree_ptr = &local_trees_B[lid];

                        float local_tree_x = (local_blk_x)/(*sub_block_len) - tree_index_x;
                        float local_tree_y = (local_blk_y)/(*sub_block_len) - tree_index_y;
                        float local_tree_z = (local_blk_z)/(*sub_block_len) - tree_index_z;
                        float cell_minx,cell_miny,cell_minz,cell_len;
                        ushort bit_index =  traverse_deepest(curr_tree_ptr,
                                                             local_tree_x,local_tree_y,local_tree_z,
                                                             &cell_minx,&cell_miny,&cell_minz,
                                                             &cell_len, depth[0] );
                        if(bit_index >=0 && bit_index < 585 )
                        {
                            // atomic_inc(&global_joint_histogram[0]);
                            unsigned int alpha_blk_offset =data_index_root(curr_tree_ptr)+data_index_relative(curr_tree_ptr,bit_index,bit_lookup);
                            unsigned int alpha_offset = sceneB_alpha_offsets[blk_offset_index]+ alpha_blk_offset;
                            float alphaB = sceneB_alphas[alpha_offset];

                            float probB = 1 - exp(-alphaB*cell_len*(*sub_block_len));
            /*                if(prob != probB )
                                printf("prob A, B %f ,%f\n",prob,probB);*/
                            int hist_index_B =(int)(0.5+((*nbins)-1)*probB); // (int)clamp((int)floor(probB*(*nbins)),0,(*nbins)-1);
                            int hist_index_A =(int)(0.5+((*nbins)-1)*prob) ; // (int)clamp((int)floor(prob*(*nbins)),0,(*nbins)-1);
                            int index = hist_index_A*(*nbins)+hist_index_B;
 //                                                       if(index > 24 )
 //                               printf("prob A, B %f ,%f, %d\n",prob,probB,index);
                            atomic_inc(&global_joint_histogram[index]);// = global_joint_histogram[0] +1;
                        }
                    }
                    ////////////////////////////////////////////
                    //END LEAF SPECIFIC CODE
                    ////////////////////////////////////////////
                }
            }
        }
    }
}
