__kernel
void
query_bit_scene(__constant  RenderSceneInfo    * linfo,
                __global    ushort2            * block_ptrs,
                __global    int4               * tree_array,       // tree structure for each block
                __global    float              * alpha_array,      // alpha for each block
                __global    uchar8             * mixture_array,    // mixture for each block
                __global    ushort4            * num_obs_array,    // num obs for each block
                __global    float2             * cum_len_beta,   // aux data used between passes
                __global    uchar2             * mean_obs_cum_vis,
                __constant  uchar              * bit_lookup,       // used to get data_index
                __local     uchar16            * local_tree,       // cache current tree into local memory
                __global    point3d            * input_query,
                __global    float              * output
                )
{
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    point3d query =(*input_query);

    for (unsigned i=0;i<linfo->num_buffer;i++)
        output[i] = -1.0f;
    query.x=(query.x-linfo->origin.x)/linfo->block_len;
    query.y=(query.y-linfo->origin.y)/linfo->block_len;
    query.z=(query.z-linfo->origin.z)/linfo->block_len;

    int block_index_x=(int)floor(query.x);
    int block_index_y=(int)floor(query.y);
    int block_index_z=(int)floor(query.z);

    if (block_index_x<0 || block_index_x>=linfo->dims.x) return;
    if (block_index_y<0 || block_index_y>=linfo->dims.y) return;
    if (block_index_z<0 || block_index_z>=linfo->dims.z) return;

    ushort2 block = block_ptrs[convert_int(block_index_z + (block_index_y + block_index_x*linfo->dims.y)*linfo->dims.z)];
    int root_ptr = block.x * linfo->tree_len + block.y;
    local_tree[llid] = as_uchar16(tree_array[root_ptr]);
    barrier(CLK_LOCAL_MEM_FENCE);

    query.x-=block_index_x;
    query.y-=block_index_y;
    query.z-=block_index_z;

    float cell_minx,cell_miny,cell_minz,cell_len;
    int data_ptr = traverse_three(local_tree,query.x,query.y,query.z,&cell_minx, &cell_miny, &cell_minz, &cell_len);
    data_ptr = data_index( 0, local_tree, data_ptr, bit_lookup);
    data_ptr = block.x * linfo->data_len + data_ptr;

    output[0]    = alpha_array[data_ptr];

    uchar8 mixture  = mixture_array[data_ptr];
    output[1]=(float)mixture.s0/255.0f;
    output[2]=(float)mixture.s1/255.0f;
    output[3]=(float)mixture.s2/255.0f;
    output[4]=(float)mixture.s3/255.0f;
    output[5]=(float)mixture.s4/255.0f;
    output[6]=(float)mixture.s5/255.0f;
    output[7]=(float)mixture.s6/255.0f;
    output[8]=(float)mixture.s7/255.0f;
    output[9]=(float)cum_len_beta[data_ptr].x;
    output[10]=(float)cum_len_beta[data_ptr].y/cum_len_beta[data_ptr].x;
    output[11]=(float)mean_obs_cum_vis[data_ptr].x/255.0;
    output[12]=(float)mean_obs_cum_vis[data_ptr].y/255.0;
}
