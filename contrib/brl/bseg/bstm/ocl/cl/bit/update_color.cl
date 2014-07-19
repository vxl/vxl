//THIS IS UPDATE BIT SCENE OPT
//Created Sept 30, 2010,
//Implements the parallel work group segmentation algorithm.
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif


#ifdef SEGLEN
typedef struct
{
  __global int* seg_len;
  __global int* mean_obsR;
  __global int* mean_obsG;
  __global int* mean_obsB;
           float4   obs;
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*, __global int2*, __local uchar16*,__local uchar8*,__constant uchar*,__local uchar*,float*,AuxArgs);

__kernel
void
seg_len_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    int2               * time_tree_array,
             __global    float              * alpha_array,      // alpha for each block
             __global    int                * aux_seg_len,        // seg len aux array
             __global    int                * aux_mean_obsY,      // mean obs r aux array
             __global    int                * aux_mean_obsU,      // mean obs r aux array
             __global    int                * aux_mean_obsV,      // mean obs r aux array
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float4             * ray_origins,
             __global    float4             * ray_directions,
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    float4             * in_image,         // the input image
             __global    float              * time,
             __local     uchar16            * local_tree,       // cache current tree into local memory
             __local     uchar8             * local_time_tree,
             __local     uchar              * cumsum )         // cumulative sum for calculating data pointer
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
  int imIndex = j*get_global_size(0) + i;

  //grab input image value (also holds vis)
  float4 obs = in_image[imIndex];
  float vis = 1.0f;  //no visibility in this pass
  barrier(CLK_LOCAL_MEM_FENCE);

  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y )
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
  aux_args.seg_len  = aux_seg_len;
  aux_args.mean_obsR = aux_mean_obsY; //&aux_array[linfo->num_buffer * linfo->data_len];
  aux_args.mean_obsG = aux_mean_obsU; //&aux_array[2 * linfo->num_buffer * linfo->data_len];
  aux_args.mean_obsB = aux_mean_obsV; //&aux_array[3 * linfo->num_buffer * linfo->data_len];
#ifdef YUV
  aux_args.obs = rgb2yuv(obs);
#else
  aux_args.obs = (obs);
#endif

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            *time,
            linfo, tree_array, time_tree_array,                   //scene info
            local_tree, local_time_tree, bit_lookup, cumsum, &vis, aux_args);


}
#endif // SEGLEN

//second pass compresses the mean obs rgb value into the mean obs aux data
#ifdef COMPRESS_RGB
__kernel
void
compress_rgb(__global RenderSceneInfo * info,
             __global int             * aux_seg_len,        // seg len aux array
             __global int             * aux_mean_obsY,      // mean obs r aux array
             __global int             * aux_mean_obsU,      // mean obs r aux array
             __global int             * aux_mean_obsV)
{
  int gid = get_global_id(0);

  int num_time_trees = info->tree_len;
  if (gid<num_time_trees)
  {
    //get the segment length
    int len_int = aux_seg_len[gid]; //this is cum_len * SEG_LEN
    float cum_len  = convert_float(len_int)/SEGLEN_FACTOR;

    if(cum_len > 1e-10f)
    {
      //get cumulative observation values for r g and b
      int r_int = aux_mean_obsY[gid]; //aux_array[datasize + gid];   //Y int
      int g_int = aux_mean_obsU[gid]; //aux_array[2*datasize + gid]; //U int
      int b_int = aux_mean_obsV[gid]; // aux_array[3*datasize + gid]; //V int

      //normalize mean obs by CUM_LEN and SEGLEN_FACTOR
      int4 rgbs = (int4) (r_int, g_int, b_int, 0);
      float4 mean_obs = (float4) convert_float4(rgbs) / (convert_float(len_int));


  #ifdef YUV
      //------------- YUV EDIT ---------------
      //now mean_obs should have Y, U, V where U in [-.436, .436] and V in [-.615, .615]
      //put them in a 0-1 range
      mean_obs.y = (mean_obs.y + U_MAX)/U_RANGE;
      mean_obs.z = (mean_obs.z + V_MAX)/V_RANGE;
  #endif

      //pack them in a single integer, and store in global memory
      uchar4 packed = convert_uchar4_sat_rte(mean_obs*255.0f);
      aux_mean_obsY[gid] = as_int(packed); //aux_array[datasize + gid] = as_int(packed);
    }
    else
      aux_mean_obsY[gid] = 0;

    //zero out the rest of the aux array
    aux_mean_obsU[gid] = 0; //aux_array[2*datasize + gid] = 0;
    aux_mean_obsV[gid] = 0; // aux_array[3*datasize + gid] = 0;
  }
}
#endif



#ifdef PREINF
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE * mog;
  __global int* seg_len;
  __global int* mean_obs;
           float* vis_inf;
           float* pre_inf;

   __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*, __global int2*, __local uchar16*,__local uchar8*,__constant uchar*,__local uchar*,float*,AuxArgs);


__kernel
void
pre_inf_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    int2               * time_tree_array,
             __global    float              * alpha_array,      // alpha for each block
             __global    MOG_TYPE           * mixture_array,    // mixture for each block
             __global    int                * aux_array0,        // four aux arrays strung together
             __global    int                * aux_array1,        // four aux arrays strung together
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float4             * ray_origins,
             __global    float4             * ray_directions,
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    float              * vis_image,        // visibility image
             __global    float              * pre_image,        // preinf image
             __global    float              * time,             // time
             __global    float              * output,
             __local     uchar16            * local_tree,       // cache current tree into local memory
             __local     uchar8             * local_time_tree,
             __local     uchar              * cumsum )          // cumulative sum for calculating data pointer
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

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            *time,
            linfo, tree_array, time_tree_array,                   //scene info
            local_tree, local_time_tree, bit_lookup, cumsum, &vis, aux_args);

  //store the vis_inf/pre_inf in the image
  vis_image[j*get_global_size(0)+i] = vis_inf;
  pre_image[j*get_global_size(0)+i] = pre_inf;
}
#endif // PREINF

#ifdef BAYES
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
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*, __global int2*, __local uchar16*,__local uchar8*,__constant uchar*,__local uchar*,float*,AuxArgs);

__kernel
void
bayes_main(__constant  RenderSceneInfo    * linfo,
           __global    int4               * tree_array,        // tree structure for each block
           __global    int2               * time_tree_array,
           __global    float              * alpha_array,       // alpha for each block
           __global    MOG_TYPE           * mixture_array,     // mixture for each block
           __global    int                * aux_array0,        // four aux arrays strung together
           __global    int                * aux_array1,        // four aux arrays strung together
           __global    int                * aux_array2,        // four aux arrays strung together
           __global    int                * aux_array3,        // four aux arrays strung together
           __constant  uchar              * bit_lookup,        // used to get data_index
           __global    float4             * ray_origins,
           __global    float4             * ray_directions,
           __global    uint4              * imgdims,           // dimensions of the input image
           __global    float              * vis_image,         // visibility image (for keeping vis across blocks)
           __global    float              * pre_image,         // preinf image (for keeping pre across blocks)
           __global    float              * norm_image,        // norm image (for bayes update normalization factor)
           __global    float              * time,              // time
           __global    float              * output,
           __local     uchar16            * local_tree,        // cache current tree into local memory
           __local     uchar8             * local_time_tree,
           __local     uchar              * cumsum)            // cumulative sum for calculating data pointer
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

  float vis_dummy = 1.0f;
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
  aux_args.norm = norm;
  aux_args.ray_vis = &vis;
  aux_args.ray_pre = &pre;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            *time,
            linfo, tree_array, time_tree_array,                   //scene info
            local_tree, local_time_tree, bit_lookup, cumsum, &vis_dummy, aux_args);

  //write out vis and pre
  vis_image[j*get_global_size(0)+i] = vis;
  pre_image[j*get_global_size(0)+i] = pre;

}
#endif // BAYES

#ifdef PROC_NORM
// normalize the pre_inf image...
//
// normalize the pre_inf image...
//
__kernel
void
proc_norm_image (  __global float* norm_image,
                   __global float* vis_image,
                   __global float* pre_image,
                   __global uint4 * imgdims,
                   __global float * in_image)
{
  // linear global id of the normalization image
  int i=0;
  int j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  float vis;


  vis = vis_image[j*get_global_size(0) + i];

  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y || vis < 0.0f)
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
//

void update_3d_gauss(float8* mixture, float* nobs, float4 mean_obs, float cell_vis, float min_sigma)
{
  (*nobs) += cell_vis;
  float rho = cell_vis / (*nobs);
  float4 mu = (*mixture).s0123;
  float4 sigma = (*mixture).s4567;
  update_gauss_3d(mean_obs, rho, &mu, &sigma, min_sigma);
  (*mixture).s0123 = mu;
  (*mixture).s4567 = sigma;
}

//Updates 3 independent gaussian distributions
void update_yuv_appearance(float8* mixture, float* nobs, float4 mean_obs, float cell_vis, float min_sigma)
{
  //update NOBS and calculate rho
  (*nobs) += cell_vis;
  if ( *nobs > 0.001f ) {
    float rho = cell_vis / (*nobs);

    //update y channel
    float mu = (*mixture).s0;
    float sigma = (*mixture).s4;
    update_gauss(mean_obs.x, rho, &mu, &sigma, min_sigma);
    (*mixture).s0 = mu;
    (*mixture).s4 = 0.07;
    //u channel
    mu = (*mixture).s1;
    sigma = (*mixture).s5;
    update_gauss(mean_obs.y, rho, &mu, &sigma, min_sigma);
    (*mixture).s1 = mu;
    (*mixture).s5 = 0.1;
    //v channel
    mu = (*mixture).s2;
    sigma = (*mixture).s6;
    update_gauss(mean_obs.z, rho, &mu, &sigma, min_sigma);
    (*mixture).s2 = mu;
    (*mixture).s6 = 0.1;
  }
}

__kernel
void
update_bit_scene_main(__global RenderSceneInfo  * info,
                      __global int2             * time_tree_array,
                      __global float            * alpha_array,
                      __global MOG_TYPE         * mixture_array,
                      __global ushort           * nobs_array,
                      __global int              * aux_array0,
                      __global int              * aux_array1,
                      __global int              * aux_array2,
                      __global int              * aux_array3,
                      __global float            * change,
                      __global float            * mog_fixed_std,
                      __global float            * time,             // time
                      __global int              * update_alpha,     //update if not zero
                      __global int              * update_changes_only,     //update if not zero
                      __global float            * output,
                      __local  uchar8           * local_time_tree)
{
  int gid=get_global_id(0);
  uchar llid = get_local_id(0);
  float min_sigma = 0.02f;

  int num_time_trees = info->tree_len;
  if (gid<num_time_trees)
  {
    //compute data ptr via traversing the time tree
    int time_tree_index = floor(*time);
    //load the time tree
    local_time_tree[llid] = as_uchar8( time_tree_array[gid * info->dims.w + time_tree_index ]);
    int bit_index_t = traverse_tt(&local_time_tree[llid], *time - time_tree_index);
    int data_ptr_tt = data_index_root_tt(&local_time_tree[llid])+ get_relative_index_tt(&local_time_tree[llid],bit_index_t);

    //if alpha is less than zero don't update
    float  alpha    = alpha_array[data_ptr_tt];
    float  cell_min = info->block_len;

    //get cell cumulative length and make sure it isn't 0
    int len_int = aux_array0[gid];
    float cum_len  = convert_float(len_int)/SEGLEN_FACTOR;

    //minimum alpha value, don't let blocks get below this
    float  alphamin = -log(1.0f-0.001f)/cell_min;

    //update cell if alpha and cum_len are greater than 0
    if (alpha > 0.0f && cum_len > 1e-10f  && (*update_changes_only == 0 || change[gid] > 0.3f))
    {
      int obs_int = aux_array1[gid];
      int vis_int = aux_array2[gid];
      int beta_int= aux_array3[gid];

      //mean obs is already normalized
      float4 meanObs = convert_float4(as_uchar4(obs_int))/255.0f;
      float mean_obs = meanObs.x;
      float cell_vis  = convert_float(vis_int) / (convert_float(len_int) * info->block_len); //(SEGLEN_FACTOR*cum_len);
      float cell_beta = convert_float(beta_int) / (convert_float(len_int) * info->block_len); //(SEGLEN_FACTOR*cum_len);
      CONVERT_FUNC_FLOAT8(mixture,mixture_array[data_ptr_tt])/NORM;

      //single gauss appearance update
      float nob_single = convert_float(nobs_array[data_ptr_tt])/100.0f;

#ifdef YUV
      //-----YUV edit ----
      update_yuv_appearance(&mixture, &nob_single, meanObs, cell_vis, min_sigma);
      //-----------------
#else
      update_3d_gauss(&mixture, &nob_single, meanObs, cell_vis, min_sigma);
#endif
      nobs_array[data_ptr_tt] = nob_single * 100.0f;


      //update alpha
      clamp(cell_beta,0.125f,8.0f);
      alpha *= cell_beta;
      //reset the cells in memory
      if (*update_alpha)
        alpha_array[data_ptr_tt]      = max(alphamin, alpha);

      float8 post_mix       = mixture * (float) NORM;

      if (*mog_fixed_std > 0.0f) {
        post_mix.s4 = (*mog_fixed_std) * (float) NORM;
        post_mix.s5 = (*mog_fixed_std) * (float) NORM;
        post_mix.s6 = (*mog_fixed_std) * (float) NORM;
      }


      CONVERT_FUNC_SAT_RTE(mixture_array[data_ptr_tt],post_mix);
    }

    //clear out aux data
    aux_array0[gid] = 0;
    aux_array1[gid] = 0;
    aux_array2[gid] = 0;
    aux_array3[gid] = 0;

  }
}

#endif // UPDATE_BIT_SCENE_MAIN
