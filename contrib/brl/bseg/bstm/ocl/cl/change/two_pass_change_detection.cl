#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif


#ifdef CHANGE_SEGLEN

typedef struct
{
  __global int* seg_len;
  __global int* mean_obs;
           float obs;
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
             __global    float              * output,
            __local     uchar16            * local_tree,
            __local     uchar8             * local_time_tree,
            __local     uchar              * cumsum)          // cumulative sum for calculating data pointer
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
  aux_args.seg_len  = aux_array0;
  aux_args.mean_obs = aux_array1;
  aux_args.obs = obs;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            (*time),
            linfo, tree_array, time_tree_array,                   //scene info
            local_tree, local_time_tree, bit_lookup, cumsum, &vis, aux_args);
}
//Update step cell functor::seg_len
void step_cell_seglen(AuxArgs aux_args, int data_ptr, int data_ptr_tt,float d)
{
    //SLOW and accurate method
    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);
    int cum_obs = convert_int_rte(d * aux_args.obs * SEGLEN_FACTOR);
    atom_add(&aux_args.mean_obs[data_ptr], cum_obs);
}
#endif


#ifdef AUX_CHANGE
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __constant  RenderSceneInfo* linfo;

  //cell data
  __global float*       alpha;
  __global MOG_TYPE *   mog;
  __global int*         seg_len;
  __global int*         mean_obs;
           float*       app_model_weights;
           float*       change;
           float*       ray_vis;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*, __global int2*, __local uchar16*,__local uchar8*,__constant uchar*,__local uchar*,float*,AuxArgs);

//------------------------------------------------------------------------------
// 1x1 change detection (simple, single ray independent CD).
//------------------------------------------------------------------------------

__kernel
void
change_kernel    ( __constant  RenderSceneInfo     * linfo,
                    __global    int4               * tree_array,
                    __global    int2               * time_tree_array,
                    __global    float              * alpha_array,
                    __global    MOG_TYPE           * mixture_array,
                    __global    int                * aux_seg_len,
                    __global    int                * aux_mean_obs,
                    __global    float4             * ray_origins,
                    __global    float4             * ray_directions,
                    __global    float              * change_image,      // input image and store vis_inf and pre_inf
                    __global    uint4              * exp_image_dims,
                    __global    float              * time,
                    __global    float              * output,
                    __constant  uchar              * bit_lookup,
                    __global    float              * vis_image,
                    __local     uchar16            * local_tree,
                    __local     uchar8             * local_time_tree,
                    __local     uchar              * cumsum,        //cumulative sum helper for data pointer
                    __local     int                * imIndex)
{
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=get_global_id(0);
  int j=get_global_id(1);
  imIndex[llid] = j*get_global_size(0)+i;

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*exp_image_dims).z || j>=(*exp_image_dims).w)
    return;

  //-------- grab center pixels - change, change_exp
  float change          = change_image[imIndex[llid]];
  float vis             = vis_image[imIndex[llid]];

  //------- calc offset ray (potentially neighboring ray)-----------
  float4 ray_o = ray_origins[ imIndex[llid] ];
  float4 ray_d = ray_directions[ imIndex[llid] ];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  float app_model_weights[8] = {0};
  float4 viewdir = (float4)(ray_dx,ray_dy,ray_dz,0);
  compute_app_model_weights(app_model_weights, viewdir,&app_model_view_directions);


  //------- Set Aux Args -------------
  AuxArgs aux_args;
  aux_args.linfo        = linfo;
  aux_args.alpha        = alpha_array;
  aux_args.mog          = mixture_array;
  aux_args.seg_len      = aux_seg_len;
  aux_args.mean_obs     = aux_mean_obs;
  aux_args.app_model_weights = app_model_weights;
  aux_args.ray_vis      = &vis;
  aux_args.change       = &change;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            (*time),
            linfo, tree_array, time_tree_array,                   //scene info
            local_tree, local_time_tree, bit_lookup, cumsum, &vis, aux_args);

  //expected image gets rendered
  change_image[imIndex[llid]]     = change;  //expected_int;
  vis_image[imIndex[llid]]        = vis;
}


// Change detection step cell functor
void step_cell_change2(AuxArgs aux_args, int data_ptr, int data_ptr_tt,float d)
{
  //d-normalize the ray seg len
  d *= aux_args.linfo->block_len;

  //get cell cumulative length and make sure it isn't 0
  int len_int = aux_args.seg_len[data_ptr];
  float cum_len  = convert_float(len_int)/SEGLEN_FACTOR;

  int obs_int = aux_args.mean_obs[data_ptr];
  float mean_obs = convert_float(obs_int) / convert_float(len_int);

#ifdef  MOG_TYPE_8
  CONVERT_FUNC_FLOAT8(mixture, aux_args.mog[data_ptr_tt])/NORM;
  float prob_den= gauss_prob_density(mean_obs, mixture.s0,mixture.s1);
#else
  CONVERT_FUNC_FLOAT16(mixture, aux_args.mog[data_ptr_tt]);
  float prob_den = view_dep_mixture_model(mean_obs, mixture, aux_args.app_model_weights);
#endif


  //calculate prob density of expected image
  float alpha = aux_args.alpha[data_ptr_tt];
  float prob  = 1.0f-exp(-alpha*d);
  float omega = (*aux_args.ray_vis)*prob;
  (*aux_args.ray_vis) *= (1.0f-prob);
  (*aux_args.change) += prob_den*omega;
}

#endif
