//THIS IS UPDATE BIT SCENE OPT
//Created Sept 30, 2010,
//Implements the parallel work group segmentation algorithm.
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
#pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef SEGLEN
//
// accumulate the length of rays intersecting a cell. Used by update algorithms
// that weight the contribution of an observation in terms of the fraction of
// ray segment length intersected by the ray carrying the observation
//
typedef struct
{
    __global int* seg_len;
    __global int* mean_obs;

    __local  short2* ray_bundle_array;
    __local  int*    cell_ptrs;
    __local  float4* cached_aux;
    float   obs;
    __global float * output;
    float * ray_len;
    __constant RenderSceneInfo * linfo;
    float * nearplane;
    float * farplane;

} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);
__kernel
    void
    seg_len_main(__constant  RenderSceneInfo    * linfo,
    __global    int4               * tree_array,       // tree structure for each block
    __global    float              * alpha_array,      // alpha for each block
    __global    int                * aux_array0,       // seg length (total for all rays intersecting cell)
    __global    int                * aux_array1,       // mean observation( over all rays intersecting cell)
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
    aux_args.seg_len  = aux_array0;
    aux_args.mean_obs = aux_array1;
    aux_args.ray_bundle_array = ray_bundle_array;
    aux_args.cell_ptrs  = cell_ptrs;
    aux_args.cached_aux = cached_aux_data;
    aux_args.obs = obs;
    float nearplane = nearfarplanes[0]/linfo->block_len;
    float farplane = nearfarplanes[1]/linfo->block_len;

#ifdef DEBUG
    aux_args.output = output;
    float rlen = output[imIndex];
    aux_args.ray_len = &rlen;
#endif
    // the step cell for SEGLEN is in ../update_functors.cl
    // void step_cell_seglen(AuxArgs aux_args, int data_ptr, uchar llid, float d)
    cast_ray( i, j,
        ray_ox, ray_oy, ray_oz,
        ray_dx, ray_dy, ray_dz,
        linfo, tree_array,                                  //scene info
        local_tree, bit_lookup, cumsum, &vis, aux_args,nearplane,farplane);    //utility info

#ifdef DEBUG
    output[imIndex] = rlen;
#endif
}
#endif // SEGLEN


// compute the pre term in the bayes update computation
//  pre is computed by a forward traverse of the ray
//  pre(x)(I) = pre(x-1)(I) + vis(x)* p(I|x in S) * P(x in S)
//
#ifdef PREINF
typedef struct
{
    __global float* alpha;
    __global MOG_TYPE * mog;
    __global int* seg_len;
    __global int* mean_obs;
    float* vis_inf;
    float* pre_inf;
    float phi;
    float4 viewdir;
    __constant RenderSceneInfo * linfo;

} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);

__kernel
    void
    pre_inf_main(__constant  RenderSceneInfo    * linfo,
    __global    int4               * tree_array,       // tree structure for each block
    __global    float              * alpha_array,      // alpha for each block
    __global    MOG_TYPE           * mixture_array,    // mixture for each block
    __global    ushort4            * num_obs_array,    // num obs for each block
    __global    int                * aux_array0,       // seg length (total over rays)
    __global    int                * aux_array1,       // mean observation (over rays intesecting the cell)
    __constant  uchar              * bit_lookup,       // used to get data_index
    __global    float4             * ray_origins,
    __global    float4             * ray_directions,
    __global    float             * nearfarplanes,
    __global    uint4              * imgdims,          // dimensions of the input image
    __global    float              * vis_image,        // visibility image
    __global    float              * pre_image,        // preinf image
    __global    float              * output,
    __local     uchar16            * local_tree,       // cache current tree into local memory
    __local     uchar              * cumsum )           // cumulative sum for calculating data pointer
{
    //get local id (0-63 for an 8x8) of this patch
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

    //----------------------------------------------------------------------------
    // get image coordinates and camera,
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    int i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);

    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
        return;
    //float4 inImage = in_image[j*get_global_size(0) + i];
    float vis_inf = vis_image[j*get_global_size(0) + i];
    float pre_inf = pre_image[j*get_global_size(0) + i];

    if (vis_inf <0.0)
        return;
    //vis for cast_ray, never gets decremented so no cutoff occurs
    float vis = 1.0f;
    barrier(CLK_LOCAL_MEM_FENCE);

    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image,
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    float4 ray_o = ray_origins[ j*get_global_size(0) + i ];
    float4 ray_d = ray_directions[ j*get_global_size(0) + i ];
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

    float4 viewdir = (float4)(ray_dx,ray_dy,ray_dz,0);
    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image, have calculated ray
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    AuxArgs aux_args;
    aux_args.linfo   = linfo;
    aux_args.alpha   = alpha_array;
    aux_args.mog     = mixture_array;
    aux_args.seg_len   = aux_array0;
    aux_args.mean_obs  = aux_array1;
    aux_args.vis_inf = &vis_inf;
    aux_args.pre_inf = &pre_inf;
    aux_args.phi     = atan2(ray_d.y,ray_d.x);
    aux_args.viewdir = viewdir;
    float nearplane = nearfarplanes[0]/linfo->block_len;
    float farplane = nearfarplanes[1]/linfo->block_len;
    //
    // the step cell for preinf is in ../update_functors.cl
    // void step_cell_preinf(AuxArgs aux_args, int data_ptr, uchar llid, float d)
    //
    cast_ray( i, j,
        ray_ox, ray_oy, ray_oz,
        ray_dx, ray_dy, ray_dz,
        linfo, tree_array,                                  //scene info
        local_tree, bit_lookup, cumsum, &vis, aux_args,nearplane,farplane);    //utility info

    //store the vis_inf/pre_inf in the image
    vis_image[j*get_global_size(0)+i] = vis_inf;
    pre_image[j*get_global_size(0)+i] = pre_inf;
}
#endif // PREINF

#ifdef COMBINE_PRE_VIS
__kernel
    void
    combine_pre_vis(__global float* preInf, __global float* visInf,
    __global float* blkPre, __global float* blkVis,
    __global uint4* imgdims)
{
    //----------------------------------------------------------------------------
    // get image coordinates and camera,
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    int i=get_global_id(0);
    int j=get_global_id(1);
    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
        return;
    int index = j*get_global_size(0) + i;

    //update pre before vis
    preInf[index] += blkPre[index]*visInf[index];

    //update vis
    visInf[index] *= blkVis[index];
}
#endif //COMBINE_PRE_VIS

#ifdef BAYES
// the bayes update is as follows
//                    pre(x-1)(I) + vis(x)* p(I|x in S) * P(x in S)
// p(x in S | I) =    ----------------------------------------
//                    pre(x-1)(I) + vis(x)* p(I|x in S) * P(x in S) + post(x)
//
typedef struct
{
    __global float*   alpha;
    __global MOG_TYPE * mog;
    __global int* seg_len;
    __global int* mean_obs;
    __global int* vis_array;
    __global int* beta_array;

    float   norm;
    float*  ray_vis;
    float*  ray_pre;

    float*  outInt;

    //local caching args
    __local  short2* ray_bundle_array;
    __local  int*    cell_ptrs;
    __local  float*  cached_vis;
    float phi;
    float4 viewdir;
    __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);

__kernel
    void
    bayes_main(__constant  RenderSceneInfo    * linfo,
    __global    int4               * tree_array,        // tree structure for each block
    __global    float              * alpha_array,       // alpha for each block
    __global    MOG_TYPE           * mixture_array,     // mixture for each block
    __global    ushort4            * num_obs_array,     // num obs for each block
    __global    int                * aux_array0,        // four aux arrays strung together
    __global    int                * aux_array1,        // four aux arrays strung together
    __global    int                * aux_array2,        // four aux arrays strung together
    __global    int                * aux_array3,        // four aux arrays strung together
    __constant  uchar              * bit_lookup,        // used to get data_index
    __global    float4             * ray_origins,
    __global    float4             * ray_directions,
    __global    float              * nearfarplanes,
    __global    uint4              * imgdims,           // dimensions of the input image
    __global    float              * vis_image,         // visibility image (for keeping vis across blocks)
    __global    float              * pre_image,         // preinf image (for keeping pre across blocks)
    __global    float              * norm_image,        // norm image (for bayes update normalization factor)
    __global    float              * output,
    __local     uchar16            * local_tree,        // cache current tree into local memory
    __local     short2             * ray_bundle_array,  // gives information for which ray takes over in the workgroup
    __local     int                * cell_ptrs,         // local list of cell_ptrs (cells that are hit by this workgroup
    __local     float              * cached_vis,        // cached vis used to sum up vis contribution locally
    __local     uchar              * cumsum)            // cumulative sum for calculating data pointer
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

    // check to see if the thread corresponds to an actual pixel as in some
    // cases #of threads will be more than the pixels.
    if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
        return;
    float vis0 = 1.0f;
    float norm = norm_image[j*get_global_size(0) + i];
    float vis = vis_image[j*get_global_size(0) + i];
    float pre = pre_image[j*get_global_size(0) + i];
    if (vis <0.0)
        return;
    barrier(CLK_LOCAL_MEM_FENCE);
    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image,
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    float4 ray_o = ray_origins[ j*get_global_size(0) + i ];
    float4 ray_d = ray_directions[ j*get_global_size(0) + i ];
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    //calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
    calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
    float4 viewdir = (float4)(ray_dx,ray_dy,ray_dz,0);

    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image, have calculated ray
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    AuxArgs aux_args;
    aux_args.linfo      = linfo;
    aux_args.alpha      = alpha_array;
    aux_args.mog        = mixture_array;
    aux_args.seg_len    = aux_array0;
    aux_args.mean_obs   = aux_array1;
    aux_args.vis_array  = aux_array2;
    aux_args.beta_array = aux_array3;
    aux_args.phi          = atan2(ray_d.y,ray_d.x);
    aux_args.viewdir = viewdir;
    aux_args.ray_bundle_array = ray_bundle_array;
    aux_args.cell_ptrs = cell_ptrs;
    aux_args.cached_vis = cached_vis;
    aux_args.norm = norm;
    aux_args.ray_vis = &vis;
    aux_args.ray_pre = &pre;
    float nearplane = nearfarplanes[0]/linfo->block_len;
    float farplane = nearfarplanes[1]/linfo->block_len;

    cast_ray( i, j,
        ray_ox, ray_oy, ray_oz,
        ray_dx, ray_dy, ray_dz,
        linfo, tree_array,                                  //scene info
        local_tree, bit_lookup, cumsum, &vis0, aux_args,nearplane,farplane);    //utility info

    //write out vis and pre
    vis_image[j*get_global_size(0)+i] = vis;
    pre_image[j*get_global_size(0)+i] = pre;

    //---debug
    //output[j*get_global_size(0) + i] = outInt;
    //----
}
#endif // BAYES

#ifdef PROC_NORM
// normalize the pre_inf image...
//
__kernel
    void
    proc_norm_image (  __global float* norm_image,
    __global float * in_image,
    __global float* vis_image,
    __global float* pre_image,
    __global uint4 * imgdims,
    __global float * appbuffer)
{
    // linear global id of the normalization imagee,
    int i=0;
    int j=0;
    i=get_global_id(0);
    j=get_global_id(1);
    float vis = vis_image[j*get_global_size(0) + i];
    float intensity = in_image[j*get_global_size(0) + i];

    //float density = (exp(-(intensity - appbuffer[0])*(intensity - appbuffer[0])/(2*appbuffer[1]*appbuffer[1]) ))/(sqrt(2*3.141)*appbuffer[1]);

    if (i>=(*imgdims).z && j>=(*imgdims).w && vis<0.0f)
        return;

    float pre = pre_image[j*get_global_size(0) + i];
    float norm = (pre+vis);
    norm_image[j*get_global_size(0) + i] = norm;

    // the following  quantities have to be re-initialized before
    // the bayes_ratio kernel is executed
    vis_image[j*get_global_size(0) + i] = 1.0f; // initial vis = 1.0f
    pre_image[j*get_global_size(0) + i] = 0.0f; // initial pre = 0.0
}
#endif // PROC_NORM

#ifdef UPDATE_BIT_SCENE_MAIN
// Update each cell using its aux data
// given the observation this function updates the appearance distribution and the alpha
// depending on the boolean values update_app and update alpha
//
__kernel
void
update_bit_scene_main(__global RenderSceneInfo  * info,           // scene information such as sub_block length
                      __global float            * alpha_array,    // the occlusion density for this cell
                      __global MOG_TYPE         * mixture_array,  // the intensity distribution
                      __global ushort4          * nobs_array,     // related to number of images used to update
                      __global int              * aux_array0,     // cumulative length
                      __global int              * aux_array1,     // mean observation
                      __global int              * aux_array2,     // visibility
                      __global int              * aux_array3,     // beta term (bayes update ratio in terms of occlusion density)
                      __global int              * update_alpha,   // update alpha if not zero
                      __global float            * mog_var,        // if 0 or less, variable var, otherwise use as fixed var
                      __global int              * update_app,     // update appearance if not zero
                      __global float            * output)         // for debugging purposes
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;//* info->num_buffer;
    if (gid<datasize)
    {
        //if alpha is less than zero don't update
        float  alpha    = alpha_array[gid];
        float  cell_min = info->block_len/(float)(1<<info->root_level);

#ifdef ATOMIC_FLOAT
        float cum_len = as_float(aux_array0[gid]);
#else
        //get cell cumulative length and make sure it isn't 0
        int len_int = aux_array0[gid];
        float cum_len  = convert_float(len_int);//SEGLEN_FACTOR;
#endif

        //minimum alpha value, don't let blocks get below this
        float  alphamin = -log(1.0f-0.0001f)/cell_min;

        //update cell if alpha and cum_len are greater than 0
        if (alpha > 0.0f && cum_len > 1e-10f)
        {
#ifdef ATOMIC_FLOAT
            float mean_obs = as_float(aux_array1[gid]) / cum_len;
            float cell_vis  = as_float(aux_array2[gid]) / cum_len;
            float cell_beta = as_float(aux_array3[gid])/ (cum_len* info->block_len);
#else
            int obs_int = aux_array1[gid];
            int vis_int = aux_array2[gid];
            int beta_int= aux_array3[gid];
            float mean_obs = convert_float(obs_int) / convert_float(len_int);
            float cell_vis  = convert_float(vis_int) / convert_float(len_int);
            float cell_beta = convert_float(beta_int) / (convert_float(len_int));
#endif
            float4 aux_data = (float4) (cum_len, mean_obs, cell_beta, cell_vis);
            float4 nobs     = convert_float4(nobs_array[gid]);
            CONVERT_FUNC_FLOAT8(mixture,mixture_array[gid])/NORM;
            float16 data = (float16) (alpha,
                                     (mixture.s0), (mixture.s1), (mixture.s2), (nobs.s0),
                                     (mixture.s3), (mixture.s4), (mixture.s5), (nobs.s1),
                                     (mixture.s6), (mixture.s7), (nobs.s2), (nobs.s3/100.0),
                                     0.0, 0.0, 0.0);
            //
            //use aux data to update cells
            // update cell is in ../ray_bundle_library_opt.cl
            // void update_cell(float16 * data, float4 aux_data,float t_match, float init_sigma, float min_sigma)
            //
            //update_cell(&data, aux_data, 2.5f, 0.07f, 0.05f);
            update_cell(&data, aux_data, 2.5f, 0.1f, 0.05f);
            if ( *update_app != 0 )
            {
                //set appearance model (figure out if var is fixed or not)
                float8 post_mix       = (float8) (data.s1, data.s2, data.s3,
                                                  data.s5, data.s6, data.s7,
                                                  data.s9, data.sa)*(float) NORM;
                float4 post_nobs      = (float4) (data.s4, data.s8, data.sb, data.sc*100.0);
                //check if mog_var is fixed, if so, overwrite variance in post_mix
                if (*mog_var > 0.0f) {
                    post_mix.s1 = (*mog_var) * (float) NORM;
                    post_mix.s4 = (*mog_var) * (float) NORM;
                    post_mix.s7 = (*mog_var) * (float) NORM;
                }
                //reset the cells in memory
                CONVERT_FUNC_SAT_RTE(mixture_array[gid],post_mix);
                nobs_array[gid] = convert_ushort4_sat_rte(post_nobs);
            }
            //write alpha if update alpha is 0
            if ( *update_alpha != 0 )
                alpha_array[gid] = max(alphamin,data.s0);
        }
#ifdef WITH_MASK
        // If a mask is specified, zero out cells outside of the mask.
        else // Added for silhouettes, NEED TO REMOVE LATER
          alpha_array[gid] = 0;
#endif // WITH_MASK

        //clear out aux data
        aux_array0[gid] = 0;
        aux_array1[gid] = 0;
        aux_array2[gid] = 0;
        aux_array3[gid] = 0;
    }
}

#endif // UPDATE_BIT_SCENE_MAIN
#ifdef UPDATE_BIT_BASED_DISPERSION
// Update each cell using its aux data
//
__kernel
void
update_bit_scene_based_dispersion(__constant  float           * centerX,
                            __constant  float           * centerY,
                            __constant  float           * centerZ,
                            __constant  uchar              * bit_lookup,        // used to get data_index
                            __global RenderSceneInfo    * info,
                            __global int4               * tree,
                            __global float              * alpha_array,
                            __global MOG_TYPE           * mixture_array,
                            __global ushort4            * nobs_array,
                            __global int                * aux_array0,
                            __global int                * aux_array1,
                            __global int                * aux_array2,
                            __global int                * aux_array3,
                            __global float              * aux0_dir_x,
                            __global float              * aux0_dir_y,
                            __global float              * aux0_dir_z,
                            __global float              * aux0_vis_exp,
                            __global float4              * ray_origin,
                            __global int                * update_alpha,     //update if not zero
                            __global float              * mog_var,          //if 0 or less, variable var, otherwise use as fixed var
                            __global int                * update_app,     //update if not zero
                            __global float              * output,
                            __local  uchar              * cumsum_wkgp,
                            __local  uchar16            * local_trees)
{
    int gid = get_global_id(0);
    int lid = get_local_id(0);
    int  MAX_CELLS = 585;
    int numTrees = info->dims.x * info->dims.y * info->dims.z;
    //: each thread will work on a sblock of A to match it to block in B
    if (gid < numTrees)
    {
        int index_x =(int)( (float)gid/(info->dims.y * info->dims.z) );
        int rem_x   =(gid - (float)index_x*(info->dims.y * info->dims.z) );
        int index_y =rem_x/info->dims.z;
        int rem_y   =rem_x - index_y*info->dims.z;
        int index_z =rem_y;


       local_trees[lid] = as_uchar16(tree[gid]);
        __local uchar* tree_ptr = &local_trees[lid];
        __local uchar * cumsum = &cumsum_wkgp[lid*10];
        // iterate through leaves
        cumsum[0] = (*tree_ptr);
        int cumIndex = 1;
        for (int i=0; i<MAX_CELLS; i++)
        {
            //if current bit is 0 and parent bit is 1, you're at a leaf
            int pi = (i-1)>>3;           //Bit_index of parent bit
            bool validParent = tree_bit_at(tree_ptr, pi) || (i==0); // special case for root
            int currDepth = get_depth(i);
            if (validParent && ( tree_bit_at(tree_ptr, i)==0 )) {
                //find side length for cell of this level = block_len/2^currDepth
                float cell_len = info->block_len/(float) (1<<currDepth);
                //: for each leaf node xform the cell and find the correspondence in another block.
                //get alpha value for this cell;
                int dataIndex = data_index_relative(tree_ptr, i, bit_lookup) + data_index_root(tree_ptr); //gets absolute position
                //######## UPDATE CELL ################
                //#####################################
                //#####################################
                //if alpha is less than zero don't update
                float  alpha    = alpha_array[dataIndex];
                float  cell_min = info->block_len/(float)(1<<info->root_level);
                #ifdef ATOMIC_FLOAT
                    float cum_len = as_float(aux_array0[dataIndex]);
                #else
                    //get cell cumulative length and make sure it isn't 0
                    int len_int = aux_array0[dataIndex];
                    float cum_len  = convert_float(len_int);//SEGLEN_FACTOR;
                #endif
                //minimum alpha value, don't let blocks get below this
                float  alphamin = -log(1.0f-0.0001f)/cell_min;

                //update cell if alpha and cum_len are greater than 0
                if (alpha > 0.0f && cum_len > 1e-10f)
                {
#ifdef ATOMIC_FLOAT
                    float mean_obs = as_float(aux_array1[dataIndex])  / cum_len;
                    float cell_vis  = as_float(aux_array2[dataIndex]) / cum_len;
                    float cell_beta = as_float(aux_array3[dataIndex]) / (cum_len* info->block_len);
#else
                    int obs_int = aux_array1[dataIndex];
                    int vis_int = aux_array2[dataIndex];
                    int beta_int= aux_array3[dataIndex];
                    float mean_obs  = convert_float(obs_int)  / convert_float(len_int);
                    float cell_vis  = convert_float(vis_int)  / convert_float(len_int);
                    float cell_beta = convert_float(beta_int) / convert_float(len_int);
#endif
                    float4 center = (float4)( info->origin.x + ((float)index_x+centerX[i])*info->block_len,
                                              info->origin.y + ((float)index_y+centerY[i])*info->block_len,
                                              info->origin.z + ((float)index_z+centerZ[i])*info->block_len, 0.0f)  ;
                    float4  viewing_dir = normalize(center - ray_origin[0]);
                    float  accum_vis   = aux0_vis_exp[dataIndex];
                    float4 accum_dir   = (float4)(aux0_dir_x[dataIndex],aux0_dir_y[dataIndex],aux0_dir_z[dataIndex],0.0f)/accum_vis;
                    float p_dispersion = length(accum_dir); // previous dispersion
                    accum_dir = normalize ( accum_dir );
                    float c_dispersion = dot(viewing_dir,accum_dir);

                    if( p_dispersion*c_dispersion > 0.93 )
                        cell_beta = 1.0;
                    aux0_dir_x[dataIndex] += viewing_dir.x*cell_vis;
                    aux0_dir_y[dataIndex] += viewing_dir.y*cell_vis;
                    aux0_dir_z[dataIndex] += viewing_dir.z*cell_vis;
                    aux0_vis_exp[dataIndex] +=cell_vis;

                    float4 aux_data = (float4) (cum_len, mean_obs,cell_beta, cell_vis);
                    float4 nobs     = convert_float4(nobs_array[dataIndex]);
                    CONVERT_FUNC_FLOAT8(mixture,mixture_array[dataIndex])/NORM;
                    float16 data = (float16) (alpha,
                                              (mixture.s0), (mixture.s1), (mixture.s2), (nobs.s0),
                                              (mixture.s3), (mixture.s4), (mixture.s5), (nobs.s1),
                                              (mixture.s6), (mixture.s7), (nobs.s2), (nobs.s3/100.0),
                                              0.0, 0.0, 0.0);
                    //use aux data to update cells
                    update_cell(&data, aux_data, 2.5f, 0.10f, 0.05f);


                    if ( *update_app != 0 )
                    {
                        //set appearance model (figure out if var is fixed or not)
                        float8 post_mix       = (float8) (data.s1, data.s2, data.s3,
                                                          data.s5, data.s6, data.s7,
                                                          data.s9, data.sa)*(float) NORM;
                        float4 post_nobs      = (float4) (data.s4, data.s8, data.sb, data.sc*100.0);
                        //check if mog_var is fixed, if so, overwrite variance in post_mix
                        if (*mog_var > 0.0f) {
                            post_mix.s1 = (*mog_var) * (float) NORM;
                            post_mix.s4 = (*mog_var) * (float) NORM;
                            post_mix.s7 = (*mog_var) * (float) NORM;
                        }
                        //reset the cells in memory
                        CONVERT_FUNC_SAT_RTE(mixture_array[dataIndex],post_mix);
                        nobs_array[dataIndex] = convert_ushort4_sat_rte(post_nobs);
                    }
                    //write alpha if update alpha is 0
                    if ( *update_alpha != 0 )
                        alpha_array[dataIndex] = max(alphamin,data.s0);
                }

                //clear out aux data
                aux_array0[dataIndex] = 0;
                aux_array1[dataIndex] = 0;
                aux_array2[dataIndex] = 0;
                aux_array3[dataIndex] = 0;

                //########################################
            }
        }
    }

}

#endif // UPDATE_BIT_SCENE_MAIN
#ifdef UPDATE_APP_GREY
__kernel
    void
    update_mog3_main(__global RenderSceneInfo  * info,
    __global MOG_TYPE         * mixture_array,
    __global ushort4          * nobs_array,
    __global int              * aux_array0,
    __global int              * aux_array1,
    __global int              * aux_array2,
    __global float            * output)
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;
    if (gid<datasize)
    {
        //get cell cumulative length and make sure it isn't 0
        int len_int = aux_array0[gid];
        float cum_len  = convert_float(len_int)/SEGLEN_FACTOR;
        //update cell if alpha and cum_len are greater than 0
        if (cum_len > 1e-10f)
        {
            int obs_int = aux_array1[gid];
            int vis_int = aux_array2[gid];
            output[0] = obs_int;

            float mean_obs = convert_float(obs_int) / convert_float(len_int);
            float cell_vis  = convert_float(vis_int) / convert_float(len_int);

            float4 nobs     = convert_float4(nobs_array[gid]);
            CONVERT_FUNC_FLOAT8(mixture,mixture_array[gid])/NORM;

            float mu0 = mixture.s0, sigma0 = mixture.s1, w0 = mixture.s2;
            float mu1 = mixture.s3, sigma1 = mixture.s4, w1 = mixture.s5;
            float mu2 = mixture.s6, sigma2 = mixture.s7, w2 = 0.0;
            if (w0>0.0f && w1>0.0f)
                w2=1-w0-w1;

            short Nobs0 = (short)nobs.s0, Nobs1 = (short)nobs.s1, Nobs2 = (short)nobs.s2;
            float Nobs_mix = nobs.s3/100.0;
            update_gauss_3_mixture(mean_obs,              //mean observation
                cell_vis,              //cell_visability
                2.5f,0.07f,0.02f,
                &mu0,&sigma0,&w0,&Nobs0,
                &mu1,&sigma1,&w1,&Nobs1,
                &mu2,&sigma2,&w2,&Nobs2,
                &Nobs_mix);
            //reset the cells in memory
            float8 post_mix       = (float8) (mu0, sigma0, w0,
                mu1, sigma1, w1,
                mu2, sigma2)*(float) NORM;
            float4 post_nobs      = (float4) (Nobs0, Nobs1, Nobs2,Nobs_mix*100.0);
            CONVERT_FUNC_SAT_RTE(mixture_array[gid],post_mix);
            nobs_array[gid]       = convert_ushort4_sat_rte(post_nobs);
        }
    }
}
#endif // UPDATE_APP_GREY

#ifdef UPDATE_ALPHA
// Update each cell using its aux data
//
__kernel
    void
    update_bit_scene_main(__global RenderSceneInfo  * info,
    __global float            * alpha_array,
    __global int              * aux_array0,
    __global int              * aux_array1,
    __global int              * aux_array2,
    __global int              * aux_array3,
    __global float            * output)
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;//* info->num_buffer;
    if (gid<datasize)
    {
        //if alpha is less than zero don't update
        float  alpha    = alpha_array[gid];
        float  cell_min = info->block_len    ;

        //get cell cumulative length and make sure it isn't 0
        int len_int = aux_array0[gid];
        float cum_len  = convert_float(len_int)/SEGLEN_FACTOR;

        //minimum alpha value, don't let blocks get below this
        float  alphamin = -log(1.0f-0.0001f)/cell_min;

        //update cell if alpha and cum_len are greater than 0
        if (alpha > 0.0f && cum_len > 1e-10f)
        {
            int beta_int= aux_array3[gid];
            float cell_beta = convert_float(beta_int) / (convert_float(len_int));
            cell_beta = clamp(cell_beta, 0.25,16.0);
            alpha = alpha * cell_beta;
            alpha_array[gid]      = max(alpha,alphamin);
        }

        //clear out aux data
        aux_array0[gid] = 0;
        aux_array1[gid] = 0;
        aux_array2[gid] = 0;
        aux_array3[gid] = 0;
    }
}
#endif // UPDATE_ALPHA

#ifdef UPDATE_ALPHA_ONLY

// Update each cell using its aux data - update alpha only
//
__kernel
    void
    update_bit_scene_main_alpha_only(__global RenderSceneInfo  * info,
    __global float            * alpha_array,
    __global int              * aux_array0,
    __global int              * aux_array1,
    __global int              * aux_array2,
    __global int              * aux_array3)
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;//* info->num_buffer;
    if (gid<datasize)
    {
#if 1
        //if alpha is less than zero don't update
        float  alpha    = alpha_array[gid];

        float  cell_min = info->block_len/(float)(1<<info->root_level);

        //get cell cumulative length and make sure it isn't 0
        int len_int = aux_array0[gid];
        float cum_len  = convert_float(len_int)/SEGLEN_FACTOR;

        //minimum alpha value, don't let blocks get below this
        float  alphamin = -log(1.0f-0.0001f)/cell_min;

        //update cell if alpha and cum_len are greater than 0
        if (alpha > 0.0f && cum_len > 1e-10f)
        {
            int obs_int = aux_array1[gid];
            int vis_int = aux_array2[gid];
            int beta_int= aux_array3[gid];

            float mean_obs = convert_float(obs_int) / convert_float(len_int);
            float cell_vis  = convert_float(vis_int) / (convert_float(len_int)*info->block_len);
            float cell_beta = convert_float(beta_int) / (convert_float(len_int)* info->block_len);

            clamp(cell_beta, 0.8, 1.25);
            alpha *= cell_beta;

            alpha_array[gid] = max(alphamin, alpha);
        }
        //clear out aux data
        aux_array0[gid] = 0;
        aux_array1[gid] = 0;
        aux_array2[gid] = 0;
        aux_array3[gid] = 0;
#endif // 1
    }
}

#endif // UPDATE_ALPHA_ONLY

int calc_blkunisgned(unsigned cell_minx, unsigned cell_miny, unsigned cell_minz, int4 dims)
{
    return cell_minz + (cell_miny + cell_minx*dims.y)*dims.z;
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
// operate only on leaves
#ifdef EXTRACTQ


//: Median filter for a block
__kernel void extractQ_from_beta(__constant RenderSceneInfo * linfo,
                                 __global   uchar16         * trees,
                                 __global   float           * alphas,
                                 __global   float           * aux0,
                                 __global   float           * aux3,
                                 __constant uchar           * lookup,
                                 __local    uchar           * cumsum,
                                 __local    uchar16         * all_local_tree)
{
    //make sure local_tree points to the right one in shared memory
    uchar llid = (uchar)(get_local_id(0) + (get_local_id(1) + get_local_id(2)*get_local_size(1))*get_local_size(0));
    __local uchar16* local_tree    = &all_local_tree[llid];
    __local uchar*   csum          = &cumsum[llid*10];

    //global id should be the same as treeIndex
    unsigned gidX = get_global_id(0);
    unsigned gidY = get_global_id(1);
    unsigned gidZ = get_global_id(2);

    //tree Index is global id
    unsigned treeIndex = calc_blkunisgned(gidX, gidY, gidZ, linfo->dims);
    if (gidX < linfo->dims.x && gidY < linfo->dims.y && gidZ <linfo->dims.z) {
        int MAX_INNER_CELLS, MAX_CELLS;
        get_max_inner_outer(&MAX_INNER_CELLS, &MAX_CELLS, linfo->root_level);
        //1. get current tree information
        (*local_tree)    = trees[treeIndex];
        //FOR ALL LEAVES IN CURRENT TREE
        for (int i = 0; i < MAX_CELLS; ++i) {
            if ( is_leaf(local_tree, i) ) {
                ///////////////////////////////////////
                //LEAF CODE
                int currDepth = get_depth(i);
                float side_len = 1.0f/(float) (1<<currDepth);
                //if you've collected a nonzero amount of probs, update it
                int currIdx = data_index_relative(local_tree, i, lookup) + data_index_root(local_tree);
                float curr_prob = 1.0f-exp(-alphas[currIdx] * side_len * linfo->block_len );
                float beta  = aux3[currIdx]/(aux0[currIdx]* linfo->block_len) ;
                float Q = beta ; //(1-beta*curr_prob)/(beta*(1-curr_prob)) ;
                aux3[currIdx] = Q;
                //END LEAF CODE
                ///////////////////////////////////////
            } //end leaf IF
        } //end leaf for
    } //end global id IF
}

#endif

#ifdef APPLYBETA
// Update each cell using its aux data
//
__kernel
void apply_beta(__global RenderSceneInfo  * info,
                      __global float              * aux_array3_prev,
                      __global float              * aux_array3_curr,
                      __global float              * aux_array0_curr,
                      __global float              * alpha_array)
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;
    if (gid<datasize)
    {
        float factor = 1.0;
        if(aux_array0_curr[gid] > 1e-10f &&  aux_array3_curr[gid] > 1e-10f)
            factor = aux_array3_curr[gid]/aux_array0_curr[gid];

        //factor = clamp(factor, 0.125f, 8.0f);
        float alpha= alpha_array[gid];
        alpha = alpha *  factor;
        float prev_factor = aux_array3_prev[gid];

        if(prev_factor > 1e-10f )
            alpha= alpha/prev_factor;


        alpha_array[gid] = alpha ;
        aux_array3_prev[gid] = factor;
    }
}

#endif // APPLYBETA
#ifdef PRODUCTQ
// Update each cell using its aux data
//
__kernel
    void
    compute_product_Q(__global RenderSceneInfo  * info,
    __global float              * aux_array3,
    __global float              * aux_array_product3)
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;//* info->num_buffer;
    if (gid<datasize)
    {
        if(aux_array3[gid] > 0.0)
            aux_array_product3[gid] = aux_array_product3[gid] *aux_array3[gid];
    }
}

#endif // PRODUCTQ

#ifdef UPDATEP
__kernel void update_P_using_Q(__constant RenderSceneInfo * linfo,__global uchar16 * trees,__global float* alphas,__global float * aux3_product,__global float * pinit,__constant uchar * lookup,__local uchar * cumsum,__local uchar16 * all_local_tree)
{
    //make sure local_tree points to the right one in shared memory
    uchar llid = (uchar)(get_local_id(0) + (get_local_id(1) + get_local_id(2)*get_local_size(1))*get_local_size(0));
    __local uchar16* local_tree    = &all_local_tree[llid];
    __local uchar*   csum          = &cumsum[llid*10];

    //global id should be the same as treeIndex
    unsigned gidX = get_global_id(0);
    unsigned gidY = get_global_id(1);
    unsigned gidZ = get_global_id(2);

    //tree Index is global id
    unsigned treeIndex = calc_blkunisgned(gidX, gidY, gidZ, linfo->dims);
    if (gidX < linfo->dims.x && gidY < linfo->dims.y && gidZ <linfo->dims.z) {
        int MAX_INNER_CELLS, MAX_CELLS;
        get_max_inner_outer(&MAX_INNER_CELLS, &MAX_CELLS, linfo->root_level);
        //1. get current tree information
        (*local_tree)    = trees[treeIndex];
        //FOR ALL LEAVES IN CURRENT TREE
        for (int i = 0; i < MAX_CELLS; ++i) {
            if ( is_leaf(local_tree, i) ) {
                ///////////////////////////////////////
                //LEAF CODE
                int currDepth = get_depth(i);
                float side_len = 1.0f/(float) (1<<currDepth);
                //if you've collected a nonzero amount of probs, update it
                int currIdx = data_index_relative(local_tree, i, lookup) + data_index_root(local_tree);
                //float curr_prob = 1.0f-exp(-alphas[currIdx] * side_len * linfo->block_len );
                float post_prob = pinit[0]/(pinit[0]+aux3_product[currIdx]*(1-pinit[0]));
                alphas[currIdx] = -(log(1 - post_prob)) / (side_len * linfo->block_len) ;

                //END LEAF CODE
                ///////////////////////////////////////
            } //end leaf IF
        } //end leaf for
    } //end global id IF
}

#endif

#ifdef REMOVE_LOW_NOBS
//: function to compute new alpa from the cumulative factors.
__kernel
void remove_low_nobs_main(__constant RenderSceneInfo * linfo,
                          __global uchar16 * trees,
                          __global float* alpha,
                          __global short4* num_obs, 
                          __global float*  num_obs_thresh, 
                          __constant uchar * lookup,
                          __local uchar * cumsum,       // for computing data index 
                          __local uchar16 * all_local_tree)
{
    //make sure local_tree points to the right one in shared memory
    uchar llid = (uchar)(get_local_id(0) + (get_local_id(1) + get_local_id(2)*get_local_size(1))*get_local_size(0));
    __local uchar16* local_tree    = &all_local_tree[llid];
    __local uchar*   csum          = &cumsum[llid*10];

    //global id should be the same as treeIndex
    unsigned gidX = get_global_id(0);
    unsigned gidY = get_global_id(1);
    unsigned gidZ = get_global_id(2);

    //tree Index is global id
    unsigned treeIndex = calc_blkunisgned(gidX, gidY, gidZ, linfo->dims);
    if (gidX < linfo->dims.x && gidY < linfo->dims.y && gidZ <linfo->dims.z) {
        int MAX_INNER_CELLS, MAX_CELLS;
        get_max_inner_outer(&MAX_INNER_CELLS, &MAX_CELLS, linfo->root_level);
        //1. get current tree information
        (*local_tree)    = trees[treeIndex];
        //FOR ALL LEAVES IN CURRENT TREE
        for (int i = 0; i < MAX_CELLS; ++i) {
            if ( is_leaf(local_tree, i) ) {
              ///////////////////////////////////////
              //LEAF CODE
              int currIdx = data_index_relative(local_tree, i, lookup) + data_index_root(local_tree);
              float cur_alpha = alpha[currIdx];
              float n0 = (float)(num_obs[currIdx].x);
              float n1 = (float)(num_obs[currIdx].y);
              float n2 = (float)(num_obs[currIdx].z);
              float sum = n0+n1+n2;
              if(sum<*num_obs_thresh)
                cur_alpha = 0.0f;
              alpha[currIdx] = cur_alpha;
              //END LEAF CODE
              ///////////////////////////////////////
            } //end leaf IF
        } //end leaf for
    } //end global id IF
}
#endif //REMOVE_LOW_NOBS
