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
  __global float* output;
  __global float* alpha;
  __global MOG_TYPE * mog;
  __global int* seg_len;
  __global int* mean_obs;
  __global float4* ray_dir;
           float* vis_inf;
           float* pre_inf;
           float* app_model_weights;
           float4 viewdir;
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
             __global    float4             * raydir,           // ray direction
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


  //compute weights for each app model viewing direction
  //based on the ray dir
  float app_model_weights[8] = {0};
  float4 viewdir = (float4)(ray_dx,ray_dy,ray_dz,0);
  compute_app_model_weights(app_model_weights, viewdir,&app_model_view_directions);

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
  aux_args.ray_dir = raydir;
  aux_args.vis_inf = &vis_inf;
  aux_args.pre_inf = &pre_inf;
  aux_args.app_model_weights = app_model_weights;
  aux_args.viewdir = viewdir;

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
           float* app_model_weights;
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

  //compute weights for each app model viewing direction
  //based on the ray dir
  float app_model_weights[8] = {0};
  float4 viewdir = (float4)(ray_dx,ray_dy,ray_dz,0);
  compute_app_model_weights(app_model_weights, viewdir,&app_model_view_directions);

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
  aux_args.app_model_weights = app_model_weights;
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
__kernel
void
update_bit_scene_main(__global RenderSceneInfo  * info,
                      __global int2             * time_tree_array,
                      __global float            * alpha_array,
                      __global MOG_TYPE         * mixture_array,
                      __global NOBS_TYPE        * nobs_array,
                      __global float4           * ray_dir,
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

    //get cell cumulative length and make sure it isn't 0
    int len_int = aux_array0[gid];
    float cum_len  = convert_float(len_int)/SEGLEN_FACTOR;

    //minimum alpha value, don't let blocks get below this
    float  alphamin = -log(1.0f-0.001f)/cell_min;

    //update cell if alpha and cum_len are greater than 0
    if (alpha > 0.0f && cum_len > 1e-10f )
    {
      if( (*update_changes_only == 0 || change[gid] > 0.5f) )
      {
        int obs_int = aux_array1[gid];
        int vis_int = aux_array2[gid];
        int beta_int= aux_array3[gid];

        float mean_obs = convert_float(obs_int) / convert_float(len_int);
        float cell_vis  = convert_float(vis_int) / (convert_float(len_int)*info->block_len);
        float cell_beta = convert_float(beta_int) / (convert_float(len_int)*info->block_len);

        //first, update alpha
        clamp(cell_beta,0.5f,2.0f);
        alpha *= cell_beta;
        if ( *update_alpha != 0 )
          alpha_array[data_ptr_tt] = max(alphamin,alpha);

        //second, update app model
  #ifdef NUM_OBS_VIEW_COMPACT
        CONVERT_FUNC_FLOAT8(nobs,nobs_array[data_ptr_tt]);
  #else
        float8 nobs     = nobs_array[data_ptr_tt];
  #endif


  #ifdef MOG_VIEW_DEP_COMPACT
        CONVERT_FUNC_FLOAT16(mixture,mixture_array[data_ptr_tt]);
  #else
        float16 mixture = mixture_array[data_ptr_tt];
  #endif

        //select view dependent mixture and nobs
        float app_model_weights[8] = {0};
        float4 viewdir = ray_dir[gid];
        compute_app_model_weights(app_model_weights, viewdir, &app_model_view_directions);
        update_view_dep_app(mean_obs,cell_vis, app_model_weights, (float*)(&mixture), (float*)(&nobs),*mog_fixed_std );

  #ifdef NUM_OBS_VIEW_COMPACT
        CONVERT_FUNC_USHORT8(nobs_array[data_ptr_tt], nobs);
  #else
        nobs_array[data_ptr_tt] = nobs;
  #endif


  #ifdef MOG_VIEW_DEP_COMPACT
        CONVERT_FUNC_UCHAR16(mixture_array[data_ptr_tt], mixture);
  #else
        mixture_array[data_ptr_tt] = mixture;
  #endif
      }
    }
    else if (*use_mask)
      alpha_array[data_ptr_tt] = alphamin;
  }
}

#endif // UPDATE_BIT_SCENE_MAIN
