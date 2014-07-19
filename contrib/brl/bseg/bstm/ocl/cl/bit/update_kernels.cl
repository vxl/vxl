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
  __global int* mean_obs;
           float   obs;
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
             __global    int                * aux_array0,       // aux data array (four aux arrays strung together)
             __global    int                * aux_array1,       // aux data array (four aux arrays strung together)
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float4             * ray_origins,
             __global    float4             * ray_directions,
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    float              * in_image,         // the input image
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
  aux_args.obs = obs;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            *time,
            linfo, tree_array, time_tree_array,                   //scene info
            local_tree, local_time_tree, bit_lookup, cumsum, &vis, aux_args);


}
#endif // SEGLEN

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
                   __global float * in_image,
                   __global float4 * app_density)
{
  // linear global id of the normalization image
  int i=0;
  int j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  float vis;

  //CORRECT HERE!!!!!!!!!!!!!!!!!!!!!!!!!
  /*
  if (app_density[0].x == 0.0f)
    vis = vis_image[j*get_global_size(0) + i] * gauss_prob_density(in_image[j*get_global_size(0) + i] , app_density[0].y,app_density[0].z);
  else
  */
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

/* Aux Data = [cell_len, mean_obs*cell_len, beta, cum_vis]  */
void update_cell(float16 * data, float4 aux_data,float t_match, float init_sigma, float min_sigma)
{
    float mu0 = (*data).s1, sigma0 = (*data).s2, w0 = (*data).s3;
    float mu1 = (*data).s5, sigma1 = (*data).s6, w1 = (*data).s7;
    float mu2 = (*data).s9, sigma2 = (*data).sa;
    float w2=0.0f;


    if (w0>0.0f && w1>0.0f)
        w2=1-(*data).s3-(*data).s7;

    short Nobs0 = (short)(*data).s4, Nobs1 = (short)(*data).s8, Nobs2 = (short)(*data).sb;
    float Nobs_mix = (*data).sc;

    update_gauss_3_mixture(aux_data.y,              //mean observation
                           aux_data.w,              //cell_visability
                           t_match,
                           init_sigma,min_sigma,
                           &mu0,&sigma0,&w0,&Nobs0,
                           &mu1,&sigma1,&w1,&Nobs1,
                           &mu2,&sigma2,&w2,&Nobs2,
                           &Nobs_mix);

    float beta = aux_data.z; //aux_data.z/aux_data.x;
    clamp(beta,0.5f,2.0f);
    (*data).s0 *= beta;
    (*data).s1=mu0; (*data).s2=sigma0, (*data).s3=w0;(*data).s4=(float)Nobs0;
    (*data).s5=mu1; (*data).s6=sigma1, (*data).s7=w1;(*data).s8=(float)Nobs1;
    (*data).s9=mu2; (*data).sa=sigma2, (*data).sb=(float)Nobs2;
    (*data).sc=(float)Nobs_mix;
}



__kernel
void
update_bit_scene_main(__global RenderSceneInfo  * info,
                      __global int2             * time_tree_array,
                      __global float            * alpha_array,
                      __global MOG_TYPE         * mixture_array,
                      __global NOBS_TYPE        * nobs_array,
                      __global int              * aux_array0,
                      __global int              * aux_array1,
                      __global int              * aux_array2,
                      __global int              * aux_array3,
                      __global float            * change,
                      __global int              * use_mask,         //use mask if not zero
                      __global float            * mog_fixed_std,
                      __global float            * time,             // time
                      __global int              * update_alpha,     //update if not zero
                      __global int              * update_changes_only,     //update if not zero
                      __local  uchar8           * local_time_tree)
{
  int gid=get_global_id(0);
  uchar llid = get_local_id(0);

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

#ifdef ATOMIC_FLOAT
    float cum_len = as_float(aux_array0[gid]);
#else
    //get cell cumulative length and make sure it isn't 0
    int len_int = aux_array0[gid];
    float cum_len  = convert_float(len_int)/SEGLEN_FACTOR;
#endif

    //minimum alpha value, don't let blocks get below this
    float  alphamin = -log(1.0f-0.001f)/cell_min;

    //update cell if alpha and cum_len are greater than 0
    if (alpha > 0.0f && cum_len > 1e-10f && (*update_changes_only == 0 || change[gid] >= 0.3f) )
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
      float cell_beta = convert_float(beta_int) / (convert_float(len_int)* info->block_len);
#endif


      float4 aux_data = (float4) (cum_len, mean_obs, cell_beta, cell_vis);
      float4 nobs     = convert_float4(nobs_array[data_ptr_tt]);

      CONVERT_FUNC_FLOAT8(mixture,mixture_array[data_ptr_tt])/NORM;

      float16 data = (float16) (alpha,
                                 (mixture.s0), (mixture.s1), (mixture.s2), (nobs.s0),
                                 (mixture.s3), (mixture.s4), (mixture.s5), (nobs.s1),
                                 (mixture.s6), (mixture.s7), (nobs.s2), (nobs.s3/100.0),
                                 0.0, 0.0, 0.0);

      //use aux data to update cells
      update_cell(&data, aux_data, 2.5f, 0.06f, 0.02f);

      //set appearance model (figure out if var is fixed or not
      float8 post_mix       = (float8) (data.s1, data.s2, data.s3,
                                        data.s5, data.s6, data.s7,
                                        data.s9, data.sa)*(float) NORM;
      float4 post_nobs      = (float4) (data.s4, data.s8, data.sb, data.sc*100.0);

      //check if mog_fixed_std is fixed, if so, overwrite variance in post_mix
      if (*mog_fixed_std > 0.0f) {
        post_mix.s1 = (*mog_fixed_std) * (float) NORM;
        post_mix.s4 = (*mog_fixed_std) * (float) NORM;
        post_mix.s7 = (*mog_fixed_std) * (float) NORM;
      }

      //write alpha if update alpha is 0
      if ( *update_alpha != 0 )
        alpha_array[data_ptr_tt] = max(alphamin,data.s0);

      //reset the cells in memory
      CONVERT_FUNC_SAT_RTE(mixture_array[data_ptr_tt],post_mix);
      nobs_array[data_ptr_tt] = convert_ushort4_sat_rte(post_nobs);

    }
    // else if (*use_mask && cum_len < 1e-10f)
    //   alpha_array[data_ptr_tt] = alphamin;
  }
}

#endif // UPDATE_BIT_SCENE_MAIN
