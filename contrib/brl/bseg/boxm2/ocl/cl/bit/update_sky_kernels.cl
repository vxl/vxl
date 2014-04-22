#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#ifdef UPDATE_SKY
typedef struct
{
    __constant RenderSceneInfo * linfo;
    __global float*   alpha;
    float   obs;
} AuxArgs;
void step_cell_update_sky(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //slow beta calculation ----------------------------------------------------
    float  alpha    = aux_args.alpha[data_ptr];

    if(aux_args.obs < 0.1 )
    {
        aux_args.alpha[data_ptr] = 1e-20f;
    }

}
//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);
__kernel
    void
    update_sky_main(__constant  RenderSceneInfo    * linfo,
    __global    int4               * tree_array,       // tree structure for each block
    __global    float              * alpha_array,      // alpha for each block
    __constant  uchar              * bit_lookup,       // used to get data_index
    __global    float4             * ray_origins,
    __global    float4             * ray_directions,
    __global    float              * nearfarplanes,
    __global    uint4              * imgdims,          // dimensions of the input image
    __global    float              * in_image,         // the input image
    __global    float              * output,
    __local     uchar16            * local_tree,       // cache current tree into local memory
    __local     short2             * ray_bundle_array, // gives information for which ray takes over in the workgroup
    __local     int                * cell_ptrs,        // local list of cell_ptrs (cells that are hit by this workgroup
    __local     float4             * cached_aux_data,  // seg len cached aux data is only a float2
    __local     uchar              * cumsum )          // cumulative sum for calculating data pointer
{
    //get local id (0-63 for an 8x8) of this patch
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

    //initialize pre-broken ray information (non broken rays will be re initialized)
    ray_bundle_array[llid] = (short2) (-1, 0);
    cell_ptrs[llid] = -1;

    //----------------------------------------------------------------------------
    // get image coordinates and camera,
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    int i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);
    int imIndex = j*get_global_size(0) + i;

    //grab input image value (also holds vis)
    float obs = in_image[imIndex];
    float vis = 1.0f;  //no visibility in this pass
    barrier(CLK_LOCAL_MEM_FENCE);

    // cases #of threads will be more than the pixels.
    if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y || obs < 0.0f)
        return;

    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image,
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    float4 ray_o = ray_origins[ imIndex ];
    float4 ray_d = ray_directions[ imIndex ];
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image, have calculated ray
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    AuxArgs aux_args;
    aux_args.linfo    = linfo;
    aux_args.obs    = obs;
    aux_args.alpha = alpha_array;

    float nearplane = nearfarplanes[0]/linfo->block_len;
    float farplane = nearfarplanes[1]/linfo->block_len;
    cast_ray( i, j,
        ray_ox, ray_oy, ray_oz,
        ray_dx, ray_dy, ray_dz,
        linfo, tree_array,                                  //scene info
        local_tree, bit_lookup, cumsum, &vis, aux_args,nearplane,farplane);    //utility info


}
#endif // UPDATE_SKY

#ifdef UPDATE_SKY2
typedef struct
{
    __constant RenderSceneInfo * linfo;
    __global float*   aux0;
    __global float*   aux1;
    __global float*   alpha_array;
    float   obs;
    float   vis;
} AuxArgs;


// store the sum of probabilites of sky
void step_cell_accumulate_evidence_per_ray(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    float sky_prob_int = aux_args.obs;

    AtomicAdd(&aux_args.aux0[data_ptr], sky_prob_int*d);
    AtomicAdd(&aux_args.aux1[data_ptr], d);
    //aux_args.vis = p;
}



//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);
__kernel
    void
    accumulate_evidence_sky_ray_main(__constant  RenderSceneInfo    * linfo,
    __global    int4               * tree_array,       // tree structure for each block
    __global    float              * alpha_array,      // alpha for each block
    __global    float              * aux0_array,       // aux0 for each block
    __global    float              * aux1_array,       // aux1 for each block
    __constant  uchar              * bit_lookup,       // used to get data_index
    __global    float4             * ray_origins,
    __global    float4             * ray_directions,
    __global    float              * nearfarplanes,
    __global    uint4              * imgdims,          // dimensions of the input image
    __global    float              * in_image,         // the input image
    __global    float              * vis_image,         // the input image
    __global    float              * output,
    __local     uchar16            * local_tree,       // cache current tree into local memory
    __local     short2             * ray_bundle_array, // gives information for which ray takes over in the workgroup
    __local     int                * cell_ptrs,        // local list of cell_ptrs (cells that are hit by this workgroup
    __local     float4             * cached_aux_data,  // seg len cached aux data is only a float2
    __local     uchar              * cumsum )          // cumulative sum for calculating data pointer
{
    //get local id (0-63 for an 8x8) of this patch
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    //initialize pre-broken ray information (non broken rays will be re initialized)
    ray_bundle_array[llid] = (short2) (-1, 0);
    cell_ptrs[llid] = -1;
    //----------------------------------------------------------------------------
    // get image coordinates and camera,
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    int i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);
    int imIndex = j*get_global_size(0) + i;
    //grab input image value (also holds vis)
    float obs = (in_image[imIndex]);
    float vis = (vis_image[imIndex]);
    //float vis = 1.0f;  //no visibility in this pass
    barrier(CLK_LOCAL_MEM_FENCE);
    // cases #of threads will be more than the pixels.
    if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y || obs < 0.0f)
        return;
    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image,
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    float4 ray_o = ray_origins[ imIndex ];
    float4 ray_d = ray_directions[ imIndex ];
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image, have calculated ray
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    AuxArgs aux_args;
    aux_args.linfo = linfo;
    aux_args.obs   = obs;
    aux_args.alpha_array = alpha_array;
    aux_args.aux0  = aux0_array;
    aux_args.aux1  = aux1_array;
    aux_args.vis  = vis;
    float nearplane= nearfarplanes[0]/linfo->block_len;
    float farplane = nearfarplanes[1]/linfo->block_len;
    cast_ray( i, j,
              ray_ox, ray_oy, ray_oz,
              ray_dx, ray_dy, ray_dz,
              linfo, tree_array,                                  //scene info
              local_tree, bit_lookup, cumsum, &vis, aux_args,nearplane,farplane);    //utility info
    vis_image[imIndex] = aux_args.vis;
}

#endif

#ifdef UPDATE_SKY2_MAIN
/////////////////////////////////////////////////////////////////
__kernel void accumulate_evidence_sky_cell_main( __constant RenderSceneInfo  * linfo,
                                                 __constant uchar            * bit_lookup,       // used to get data_index
                                                 __global   int4             * tree_array,
                                                 __global   float            * aux0_temp_array,
                                                 __global   float            * aux1_temp_array,
                                                 __global   float            * aux0_array,
                                                 __global   float            * aux1_array,
                                                 __local    uchar16          * tree,
                                                 __local    uchar            * all_cumsum)
{
    unsigned gid = get_global_id(0);
    unsigned lid = get_local_id(0);


    if(gid < linfo->dims.x*linfo->dims.y*linfo->dims.z)
    {
        tree[lid] = as_uchar16(tree_array[gid]);
        __local uchar* local_tree = &tree[lid];
        __local uchar*   cumsum   = &all_cumsum[10*lid];
        //cast local pointers to uchar arrays

        cumsum[0] = local_tree[0];

        int cumIndex = 1;
        int numSplit = 0;

        //FOR ALL LEAVES IN CURRENT TREE
        for (int i = 0; i < 585; ++i) {
            if ( is_leaf(local_tree, i) ) {
                ///////////////////////////////////////
                //LEAF CODE
                int currDepth = get_depth(i);
                float side_len = 1.0f/(float) (1<<currDepth);
                int currIdx = data_index_relative(local_tree, i, bit_lookup) + data_index_root(local_tree);
                float cum_sky_prob = aux0_temp_array[currIdx];
                float cum_seg_len = aux1_temp_array[currIdx];
                if(cum_seg_len > 1e-10)
                {
                   aux0_array[currIdx]= aux0_array[currIdx]+ (cum_sky_prob/cum_seg_len)*(cum_sky_prob/cum_seg_len);
                   aux1_array[currIdx]= aux1_array[currIdx]+1;
                }
            }
        }
    }
}



/////////////////////////////////////////////////////////////////
__kernel void update_sky_main( __constant RenderSceneInfo  * linfo,
                               __constant uchar            * bit_lookup,       // used to get data_index
                                __global float            * chisqr_lookup,       // used to get data_index
                               __global   int4             * tree_array,
                               __global   float            * alpha_array,
                               __global   float            * aux0_array,
                               __global   float            * aux1_array,
                               __local    uchar16          * tree,
                               __local    uchar            * all_cumsum)
{
    unsigned gid = get_global_id(0);
    unsigned lid = get_local_id(0);


    if(gid < linfo->dims.x*linfo->dims.y*linfo->dims.z)
    {
        tree[lid] = as_uchar16(tree_array[gid]);
        __local uchar* local_tree = &tree[lid];
        __local uchar*   cumsum   = &all_cumsum[10*lid];
        //cast local pointers to uchar arrays
        cumsum[0] = local_tree[0];
        int cumIndex = 1;
        int numSplit = 0;
        //FOR ALL LEAVES IN CURRENT TREE
        for (int i = 0; i < 585; ++i) {
            if ( is_leaf(local_tree, i) ) {
                ///////////////////////////////////////
                //LEAF CODE
                int currDepth = get_depth(i);
                float side_len = 1.0f/(float) (1<<currDepth);
                int currIdx = data_index_relative(local_tree, i, bit_lookup) + data_index_root(local_tree);
                float curr_alpha = alpha_array[currIdx];
                float cum        = aux0_array[currIdx];
                float count      = aux1_array[currIdx];
                if(count > 1  ){
                    float chisqr_cdf_val = chisqr_cdf(chisqr_lookup,3, cum);


                    //printf("%f,%f,%f\n",chisqr_cdf_val,cum,count);
                    //float curr_prob =( 1- exp(-curr_alpha* side_len * linfo->block_len)) *(1- chisqr_cdf_val) ;
                    //float alpha = -log(1 - curr_prob)/(side_len * linfo->block_len );
                                        if(chisqr_cdf_val > 0.75)
                        alpha_array[currIdx] = 0.00000001;
                }
            }
        }
    }
}

#endif // UPDATE_SKY