#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
inline float alph(float p, float len){
  if(p>=1.0f) p = 0.999f;
  return -log(1-p)/len;
}
inline void get_max_inner_outer(int* MAX_INNER_CELLS, int* MAX_CELLS, int root_level)
{
  //USE rootlevel to determine MAX_INNER and MAX_CELLS
  if(root_level == 1)
    *MAX_INNER_CELLS=1, *MAX_CELLS=9;
  else if (root_level == 2)
    *MAX_INNER_CELLS=9, *MAX_CELLS=73;
  else if (root_level == 3)
    *MAX_INNER_CELLS=73, *MAX_CELLS=585;
}

void interp_mog(MOG_TYPE* mog, uchar8* nbr_exint, uchar8* nbr_exist,
                float lx, float ly, float lz,float4 center,float len){
  uchar8 lhs = as_uchar8(*mog);
  float nrm = (float)NORM;
  // number of neighbors for debug
  //float nnbr = (float)((*nbr_exist).s0 +(*nbr_exist).s1 +(*nbr_exist).s2 +(*nbr_exist).s3+(*nbr_exist).s4 + (*nbr_exist).s5);
  //  nnbr /= 6.0f; //fraction of neighbors
  float8  nexint = convert_float8(*nbr_exint)/nrm;
  float mu0 = (float)lhs.s0/255.0f;

  float fx = lx - center.s0, fy = ly - center.s1, fz = lz - center.s2;
  float emx = (float)(*nbr_exist).s0; float epx = (float)(*nbr_exist).s1;
  float emy = (float)(*nbr_exist).s2; float epy = (float)(*nbr_exist).s3;
  float emz = (float)(*nbr_exist).s4; float epz = (float)(*nbr_exist).s5;
  float w0x = len-fabs(fx), w0y = len-fabs(fy), w0z = len-fabs(fz);
  float wmx = emx*((fx<0)? -fx: 0.0f), wpx = epx*((fx>0)? fx: 0.0f);
  float wmy = emy*((fy<0)? -fy: 0.0f), wpy = epy*((fy>0)? fy: 0.0f);
  float wmz = emz*((fz<0)? -fz: 0.0f), wpz = epz*((fz>0)? fz: 0.0f);
  float sumw = w0x + wmx +wpx;
  sumw +=  w0y + wmy +wpy;
  sumw += w0z + wmz +wpz;
  if(sumw == 0.0f)
    return;
  float mu = (w0x + w0y + w0z)*mu0;
  mu += wmx*nexint.s0 + wpx*nexint.s1;
  mu += wmy*nexint.s2 + wpy*nexint.s3;
  mu += wmz*nexint.s4 + wpz*nexint.s5;
  mu = (nrm*mu)/sumw;
  lhs = (uchar8)((uchar)mu, 32, 255, 0, 0, 0, 0, 0);
  CONVERT_FUNC_SAT_RTE(*mog, lhs);
}
void interp_alpha(float* alpha, float8* nbr_prob, uchar8* nbr_exist,
                  float lx, float ly, float lz,float4 center,float len){
  float fx = lx - center.s0, fy = ly - center.s1, fz = lz - center.s2;
  float emx = (float)(*nbr_exist).s0; float epx = (float)(*nbr_exist).s1;
  float emy = (float)(*nbr_exist).s2; float epy = (float)(*nbr_exist).s3;
  float emz = (float)(*nbr_exist).s4; float epz = (float)(*nbr_exist).s5;
  float w0x = len-fabs(fx), w0y = len-fabs(fy), w0z = len-fabs(fz);
  float wmx = emx*((fx<0)? -fx: 0.0f), wpx = epx*((fx>0)? fx: 0.0f);
  float wmy = emy*((fy<0)? -fy: 0.0f), wpy = epy*((fy>0)? fy: 0.0f);
  float wmz = emz*((fz<0)? -fz: 0.0f), wpz = epz*((fz>0)? fz: 0.0f);

  float sumw = w0x + wmx +wpx;
  sumw +=  w0y + wmy +wpy;
  sumw += w0z + wmz +wpz;
  if(sumw == 0.0f)
    return;
  float a = (w0x + w0y + w0z)*(*alpha);
  a += wmx*alph((*nbr_prob).s0,len) + wpx*alph((*nbr_prob).s1,len);
  a += wmy*alph((*nbr_prob).s2,len) + wpy*alph((*nbr_prob).s3,len);
  a += wmz*alph((*nbr_prob).s4,len) + wpz*alph((*nbr_prob).s5,len);
  a /= sumw;
  *alpha = a;
}
__kernel void transform_scene_interpolate(__constant  float           * centerX,//0
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
                                          //                                          __global    ushort4         * source_nobs_array,
                                          __global    uchar8          * nbr_exint_array,
                                          __global    uchar8          * nbr_exists_array,
                                          __global    float8          * nbr_prob_array,
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
	    int MAX_INNER_CELLS, MAX_CELLS;
	    get_max_inner_outer(&MAX_INNER_CELLS, &MAX_CELLS, target_scene_linfo->root_level);
            MAX_CELLS = 585;
            for (int i=0; i<MAX_CELLS; i++) {
                //if current bit is 0 and parent bit is 1, you're at a leaf
                int pi = (i-1)>>3;           //Bit_index of parent bit
                bool validParent = tree_bit_at(local_tree, pi) || (i==0); // special case for root
                int currDepth = get_depth(i);
                if (validParent && ( tree_bit_at(local_tree, i)==0 )) {

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
                    // float txg = xg; float tyg= yg; float tzg = zg;
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

                        float source_lx = clamp((txg - source_scene_origin.x)/source_scene_linfo->block_len - s_sub_blk_x,0.0f,1.0f);
                        float source_ly = clamp((tyg - source_scene_origin.y)/source_scene_linfo->block_len - s_sub_blk_y,0.0f,1.0f);
                        float source_lz = clamp((tzg - source_scene_origin.z)/source_scene_linfo->block_len - s_sub_blk_z,0.0f,1.0f);

                        float cell_minx,cell_miny,cell_minz,cell_len;
                        ushort bit_index = traverse_three(curr_tree_ptr,source_lx,source_ly,source_lz,
                                                             &cell_minx,&cell_miny,&cell_minz,&cell_len);
                        if(bit_index >=0 && bit_index < MAX_CELLS )
                          {
			    float4 cell_center = (float4) (cell_minx,cell_miny,cell_minz,0) + cell_len/2;
			    cell_center.s3=0;
                            unsigned int alpha_offset = data_index_root(curr_tree_ptr)+data_index_relative(curr_tree_ptr,bit_index,bit_lookup);
                            if( alpha_offset >=0  && alpha_offset < source_scene_linfo->data_len )
                            {
                              // here is where interpolation occurs
                              int currDepth = get_depth(i);
                              float side_len = 1.0f/((float)(1<<currDepth));
			      // source_lx /= side_len; source_ly /= side_len; source_lz /= side_len;
                              uchar8 nbr_exint = nbr_exint_array[alpha_offset];
                              uchar8 nbr_exist =  nbr_exists_array[alpha_offset];
                              float8 nbr_prob = nbr_prob_array[alpha_offset];

                              // interpolate alpha over the source
                              float alpha = source_scene_alpha_array[alpha_offset];
                              interp_alpha(&alpha, &nbr_prob, &nbr_exist, source_lx, source_ly, source_lz,cell_center,cell_len);
                              target_scene_alpha_array[dataIndex] = alpha;
			      // interpolate mog over the source
                              MOG_TYPE mog = source_scene_mog_array[alpha_offset];
                              interp_mog(&mog, &nbr_exint, &nbr_exist, source_lx, source_ly, source_lz,cell_center,cell_len);
                              target_scene_mog_array[dataIndex] = mog;
                            }
                        }
                    }
                }
            }
        }
    }
}
