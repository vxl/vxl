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
           float * ray_len;
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);
__kernel
void
seg_len_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    int                * aux_array0,       // aux data array (four aux arrays strung together)
             __global    int                * aux_array1,       // aux data array (four aux arrays strung together)
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float4             * ray_origins,
             __global    float4             * ray_directions,
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    float              * in_image,         // the input image
             __global    float              * output,
             __local     uchar16            * local_tree,       // cache current tree into local memory
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
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);    //utility info

}
#endif // SEGLEN

#ifdef PREINF
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE * mog;
  __global float8 * num_obs;
  __global int* seg_len;
  __global int* mean_obs;
  __global float4* ray_dir;
           float* vis_inf;
           float* pre_inf;
           float phi;
           float* app_model_weights;
           float4 viewdir;
   __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);

__kernel
void
pre_inf_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    MOG_TYPE           * mixture_array,    // mixture for each block
             __global    float8             * num_obs_array,    // num obs for each block
             __global    float4             * aux_array,        // four aux arrays strung together
             __global    int                * aux_array0,        // four aux arrays strung together
             __global    int                * aux_array1,        // four aux arrays strung together
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float4             * ray_origins,
             __global    float4             * ray_directions,
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
  aux_args.num_obs = num_obs_array;
  aux_args.seg_len   = aux_array0;
  aux_args.mean_obs  = aux_array1;
  aux_args.ray_dir = aux_array;
  aux_args.vis_inf = &vis_inf;
  aux_args.pre_inf = &pre_inf;
  aux_args.phi     = atan2(ray_d.y,ray_d.x);
  aux_args.app_model_weights = app_model_weights;
  aux_args.viewdir = viewdir;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);    //utility info

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
typedef struct
{
  __global float*   alpha;
  __global MOG_TYPE * mog;
  __global float8 * num_obs;
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
           float* app_model_weights;
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);

__kernel
void
bayes_main(__constant  RenderSceneInfo    * linfo,
           __global    int4               * tree_array,        // tree structure for each block
           __global    float              * alpha_array,       // alpha for each block
           __global    MOG_TYPE           * mixture_array,     // mixture for each block
           __global    float8             * num_obs_array,     // num obs for each block
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
  aux_args.num_obs    = num_obs_array;
  aux_args.seg_len    = aux_array0;
  aux_args.mean_obs   = aux_array1;
  aux_args.vis_array  = aux_array2;
  aux_args.beta_array = aux_array3;
  aux_args.phi          = atan2(ray_d.y,ray_d.x);
  aux_args.app_model_weights = app_model_weights;
  aux_args.ray_bundle_array = ray_bundle_array;
  aux_args.cell_ptrs = cell_ptrs;
  aux_args.cached_vis = cached_vis;
  aux_args.norm = norm;
  aux_args.ray_vis = &vis;
  aux_args.ray_pre = &pre;

  //---debug
  //float outInt = output[j*get_global_size(0) + i];
  //aux_args.outInt = &outInt;
  //---------

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                   //scene info
            local_tree, bit_lookup, cumsum, &vis0, aux_args,0,MAXFLOAT);    //utility info

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
                      __global float            * alpha_array,
                      __global MOG_TYPE         * mixture_array,
                      __global NOBS_TYPE        * nobs_array,
                      __global float4           * ray_dir,
                      __global int              * aux_array0,
                      __global int              * aux_array1,
                      __global int              * aux_array2,
                      __global int              * aux_array3,
                      __global int              * update_alpha,     //update if not zero
                      __global int              * use_mask,         //use mask if not zero
                      __global float            *  mog_fixed_std,   //if 0 or less, variable var, otherwise use as fixed var
                      __global float            * output)
{
  int gid=get_global_id(0);
  int datasize = info->data_len ;//* info->num_buffer;
  if (gid<datasize)
  {
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


      //first, update alpha
      clamp(cell_beta,0.5f,2.0f);
      alpha *= cell_beta;
      if ( *update_alpha != 0 )
        alpha_array[gid] = max(alphamin,alpha);

      //second, update app model
#ifdef NUM_OBS_VIEW_COMPACT
      CONVERT_FUNC_FLOAT8(nobs,nobs_array[gid]);
#else
      float8 nobs     = nobs_array[gid];
#endif

#ifdef MOG_VIEW_DEP_COMPACT
      CONVERT_FUNC_FLOAT16(mixture,mixture_array[gid]);
#else
      float16 mixture = mixture_array[gid];
#endif

      //select view dependent mixture and nobs
      float app_model_weights[8] = {0};
      float4 viewdir = ray_dir[gid];
      compute_app_model_weights(app_model_weights, viewdir, &app_model_view_directions);
      update_view_dep_app(mean_obs,cell_vis, app_model_weights, (float*)(&mixture), (float*)(&nobs),* mog_fixed_std );

#ifdef NUM_OBS_VIEW_COMPACT
      CONVERT_FUNC_USHORT8(nobs_array[gid], nobs);
#else
      nobs_array[gid] = nobs;
#endif


#ifdef MOG_VIEW_DEP_COMPACT
      CONVERT_FUNC_UCHAR16(mixture_array[gid], mixture);
#else
      mixture_array[gid] = mixture;
#endif

    }
    else if (*use_mask)
      alpha_array[gid] = 0;

    //clear out aux data
    aux_array0[gid] = 0;
    aux_array1[gid] = 0;
    aux_array2[gid] = 0;
    aux_array3[gid] = 0;
  }
}

#endif // UPDATE_BIT_SCENE_MAIN

