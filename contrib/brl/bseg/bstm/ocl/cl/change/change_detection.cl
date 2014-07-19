#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef CHANGE_DETECT

typedef struct
{
  __global float*       alpha;
  __global MOG_TYPE *   mog;
  #ifdef COLOR
           float4       intensity;
  #else
           float        intensity;
  #endif
           float*       change;
           float*       vis;
           float*       app_model_weights;

} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*, __global int2*, __local uchar16*,__local uchar8*,__constant uchar*,__local uchar*,float*,AuxArgs);

//------------------------------------------------------------------------------
// 1x1 change detection (simple, single ray independent CD).
//------------------------------------------------------------------------------
__kernel
void
change_detection_bit_scene( __constant  RenderSceneInfo    * linfo,
                            __global    int4               * tree_array,
                            __global    int2               * time_tree_array,
                            __global    float              * alpha_array,
                            __global    MOG_TYPE           * mixture_array,
                            __global    float4             * ray_origins,
                            __global    float4             * ray_directions,
#ifdef COLOR
                            __global    float4             * in_image,      // input image and store vis_inf and pre_inf
#else
                            __global    float              * in_image,      // input image and store vis_inf and pre_inf
#endif
                            __global    float              * change_image,      // input image and store vis_inf and pre_inf
                            __global    uint4              * exp_image_dims,
                            __global    float              * output,
                            __constant  uchar              * bit_lookup,
                            __global    float              * vis_image,
                            __global    float              * time,
                            __local     uchar16            * local_tree,
                            __local     uchar8             * local_time_tree,
                            __local     uchar              * cumsum,        //cumulative sum helper for data pointer
                            __local     int                * imIndex)
{
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  imIndex[llid] = j*get_global_size(0)+i;

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*exp_image_dims).z || j>=(*exp_image_dims).w)
    return;


  //change, change_exp
  float change          = change_image[imIndex[llid]];
#ifdef COLOR
  float4 intensity      = in_image[imIndex[llid]];
#else
  float intensity       = in_image[imIndex[llid]];
#endif
  float vis             = vis_image[imIndex[llid]];


  float4 ray_o = ray_origins[ imIndex[llid] ];
  float4 ray_d = ray_directions[ imIndex[llid] ];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  float app_model_weights[8] = {0};
  float4 viewdir = (float4)(ray_dx,ray_dy,ray_dz,0);
  compute_app_model_weights(app_model_weights, viewdir,&app_model_view_directions);


  AuxArgs aux_args;
  aux_args.alpha        = alpha_array;
  aux_args.mog          = mixture_array;
  aux_args.intensity    = intensity;
  aux_args.change       = &change;
  aux_args.vis          = &vis;
  aux_args.app_model_weights = app_model_weights;

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


//step cell functor
void step_cell_change(AuxArgs aux_args, int data_ptr_tt, float d)
{
#ifdef COLOR
  float4 img_in = aux_args.intensity;
#else
  float img_in = aux_args.intensity;
#endif

#ifdef  MOG_TYPE_8
  CONVERT_FUNC_FLOAT8(data, aux_args.mog[data_ptr_tt])/NORM;

  #ifdef COLOR
      float prob_den=gauss_prob_density_rgb(img_in, data.s0123, data.s4567);
  #else
    // float prob_den=gauss_3_mixture_prob_density(img_in,
    //                                             data.s0,data.s1,data.s2,
    //                                             data.s3,data.s4,data.s5,
    //                                             data.s6,data.s7,1.0f-data.s2-data.s5);

    //use only most significant component
    float prob_den= gauss_prob_density(img_in, data.s0,data.s1);
  #endif

#else
  CONVERT_FUNC_FLOAT16(mixture, aux_args.mog[data_ptr_tt]);
  float prob_den = view_dep_mixture_model(img_in, mixture, aux_args.app_model_weights);
#endif

  float alpha = aux_args.alpha[data_ptr_tt];
  float prob  = 1.0f - exp(-alpha*d);
  float omega = (*aux_args.vis)*prob;
  (*aux_args.vis) = (*aux_args.vis)*(1.0f-prob);

  //set change
  (*aux_args.change) += prob_den*omega;
}
#endif


#ifdef LABELING


typedef struct
{
  __constant  RenderSceneInfo* linfo;

  __global float*            alpha;
  __global LABEL_TYPE*       label_buf;
           LABEL_TYPE        label;
           float*            vis;

} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*, __global int2*, __local uchar16*,__local uchar8*,__constant uchar*,__local uchar*,float*,AuxArgs);

//------------------------------------------------------------------------------
// 1x1 change detection (simple, single ray independent CD).
//------------------------------------------------------------------------------
__kernel
void
label_scene( __constant  RenderSceneInfo    * linfo,
                            __global    int4               * tree_array,
                            __global    int2               * time_tree_array,
                            __global    float              * alpha,
                            __global    LABEL_TYPE         * label_buf,
                            __global    float4             * ray_origins,
                            __global    float4             * ray_directions,
                            __global    float              * change_image,
                            __global    float              * vis_image,
                            __global    uint4              * image_dims,
                            __constant  uchar              * bit_lookup,
                            __global    float              * time,
                            __global    float              * change_prob_t,
                            __global    LABEL_TYPE         * label,
                            __local     uchar16            * local_tree,
                            __local     uchar8             * local_time_tree,
                            __local     uchar              * cumsum,        //cumulative sum helper for data pointer
                            __local     int                * imIndex)
{
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  imIndex[llid] = j*get_global_size(0)+i;

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*image_dims).z || j>=(*image_dims).w)
    return;

  //if change is less than t, nothing to do here.
  float change          = change_image[imIndex[llid]];
  float vis             = vis_image[imIndex[llid]];

  // if( change > *change_prob_t) //don't label change voxels, consider only non-change pixels
  //   return;

  float4 ray_o = ray_origins[ imIndex[llid] ];
  float4 ray_d = ray_directions[ imIndex[llid] ];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  AuxArgs aux_args;
  aux_args.linfo        = linfo;
  aux_args.label        = ( change < *change_prob_t) ? 1 : *label ;
  aux_args.alpha        = alpha;
  aux_args.label_buf    = label_buf;

  aux_args.vis = &vis;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            (*time),
            linfo, tree_array, time_tree_array,                   //scene info
            local_tree, local_time_tree, bit_lookup, cumsum, &vis, aux_args);

  vis_image[imIndex[llid]]        = vis;
}


//step cell functor
void step_cell_label(AuxArgs aux_args, int data_ptr,int data_ptr_tt, float d)
{
  float alpha = aux_args.alpha[data_ptr_tt];
  float p = 1 - exp(-alpha*d*aux_args.linfo->block_len);

  //if no change, label until hit surface
  if( aux_args.label == 1)
  {
    float vis = (*aux_args.vis);

    (*aux_args.vis) *= (1-p);
    if(vis > 0.75)
      aux_args.label_buf[data_ptr] = 1;
  }
  else if( aux_args.label_buf[data_ptr] != 1 ) //if labeling change and current label is not no-change
  {
    // float vis = (*aux_args.vis);
    // (*aux_args.vis) *= pow( (1-p) , 1- (aux_args.label_buf[data_ptr] == 2) );
    // if(vis > 0.75)
    aux_args.label_buf[data_ptr] = aux_args.label;
  }
}

#endif

#ifdef CHANGE_ACCUM

typedef struct
{
  __constant  RenderSceneInfo    * linfo;
  __global float* alpha;
  __global float* seglen_array;
  __global float* vis_array;
  __global float* change_array;
  float       change_img;
  float*      vis;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*, __global int2*, __local uchar16*,__local uchar8*,__constant uchar*,__local uchar*,float*,AuxArgs);

//------------------------------------------------------------------------------
// 1x1 change detection (simple, single ray independent CD).
//------------------------------------------------------------------------------
__kernel
void
update_change( __constant  RenderSceneInfo    * linfo,
                __global    int4               * tree_array,
                __global    int2               * time_tree_array,
                __global    float              * alpha,
                __global    float4             * ray_origins,
                __global    float4             * ray_directions,
                __global    float              * change_image,      // input image and store vis_inf and pre_inf
                __global    uint4              * exp_image_dims,
                __global    float              * seglen_array,
                __global    float              * vis_array,
                __global    float              * change_array,
                __constant  uchar              * bit_lookup,
                __global    float              * vis_image,
                __global    float              * time,
                __global    float              * output,
                __local     uchar16            * local_tree,
                __local     uchar8             * local_time_tree,
                __local     uchar              * cumsum,        //cumulative sum helper for data pointer
                __local     int                * imIndex)
{
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  imIndex[llid] = j*get_global_size(0)+i;

  float change_p = change_image[imIndex[llid]];

  //IMPORTANT NOTE
  //if the pixel is sure to be non-change, no need to accumulate seglen, vis, change
  //Such voxels will receive no seglen, and therefore their change prob will be set to 0
  //by the update_change_kernel kernel.
  if (i>=(*exp_image_dims).z || j>=(*exp_image_dims).w  || change_p <= 0.0f)
    return;

  float4 ray_o = ray_origins[ imIndex[llid] ];
  float4 ray_d = ray_directions[ imIndex[llid] ];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  float vis = vis_image[imIndex[llid]];
  float vis_dummy = 1;

  AuxArgs aux_args;
  aux_args.linfo = linfo;

  aux_args.alpha = alpha;
  aux_args.seglen_array = seglen_array;
  aux_args.vis_array = vis_array;
  aux_args.change_array = change_array;
  aux_args.change_img = change_p;
  aux_args.vis = &vis;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            (*time),
            linfo, tree_array, time_tree_array,                   //scene info
            local_tree, local_time_tree, bit_lookup, cumsum, &vis_dummy, aux_args);

  vis_image[imIndex[llid]]        = vis;
}


//step cell functor
void step_cell_update(AuxArgs aux_args, int data_ptr, int data_ptr_tt, float d)
{
  //add the seglen, vis and change
  AtomicAdd( &(aux_args.seglen_array[data_ptr]), d );
  AtomicAdd( &(aux_args.vis_array[data_ptr]), (*aux_args.vis)*d );
  AtomicAdd( &(aux_args.change_array[data_ptr]), aux_args.change_img*d );

  //update vis
  float alpha = aux_args.alpha[data_ptr_tt];
  (*aux_args.vis) = (*aux_args.vis) * exp(-alpha*d*(aux_args.linfo->block_len));
}

#endif //CHANGE_ACCUM

#ifdef CHANGE_UPDATE
// Update each cell using its aux data
//
__kernel
void
update_change_kernel(__global RenderSceneInfo  * info,
                      __global float            * cum_seglen,
                      __global float            * cum_vis,
                      __global float            * cum_change,
                      __global float            * change_array,
                      __global    float         * output
                      )
{
  int gid=get_global_id(0);
  uchar llid = get_local_id(0);

  int num_time_trees = info->tree_len;


  if (gid<num_time_trees)
  {
    float cum_len = cum_seglen[gid];
    if(cum_len > 1e-07f && change_array[gid] > 0.0f )
    {
      float change_p = cum_change[gid] / cum_len;
      float vis = cum_vis[gid] / cum_len;
      float posterior = ((1+change_array[gid])/2) * change_p + change_array[gid] * (1- vis) * (1-change_p);
      //float prior = change_array[gid];
      //float beta = clamp(posterior / prior,0.5f,2.0f);
      change_array[gid] = posterior;
    }
    else
      change_array[gid] = 0.0f;
  }


}

#endif // CHANGE_UPDATE

#ifdef ACCUM_CHANGE_LL
#define EPSILON .00125f

// Update each cell using its aux data
//
__kernel
void
update_change_kernel(__global RenderSceneInfo  * info,
                      __global float            * cum_seglen,
                      __global float            * cum_vis,
                      __global float            * cum_change,
                      __global float            * change_ll_array,
                      __global float            * no_change_ll_array,
                      __global float            * change_array,
                      __global int              * use_mask
                      )
{
  int gid=get_global_id(0);
  uchar llid = get_local_id(0);

  int num_time_trees = info->tree_len;

  if (gid<num_time_trees)
  {
    float cum_len = cum_seglen[gid];
    if(cum_len > 1e-07f && change_array[gid] > 0.0f )
    {
      float change_p = cum_change[gid] / cum_len;
      float vis = cum_vis[gid] / cum_len;
      //appearance models for change/non-change
      if(change_p < EPSILON)
        change_p = EPSILON;
      float PI_B = 1.0f / change_p - 1;
      if(PI_B < EPSILON)
        PI_B = EPSILON;
      float PI_F = 1.0f;

      //priors
      float prior = 0.5f;
      float P_C_GIVEN_B = prior * (1-vis);
      float P_C_GIVEN_F = (1.0f+prior)/2.0f;
      float P_B = 0.5f;
      float P_F = 1.0f-P_B;
      float P_C = P_C_GIVEN_B * P_B + P_C_GIVEN_F * P_F;

      float P_B_GIVEN_C = P_B * P_C_GIVEN_B / P_C;
      float P_F_GIVEN_C = P_F * P_C_GIVEN_F / P_C;

      float P_B_GIVEN_NC = P_B * (1-P_C_GIVEN_B) / (1-P_C);
      float P_F_GIVEN_NC = P_F * (1-P_C_GIVEN_F) / (1-P_C);


      float ll = log2( PI_F * P_F_GIVEN_C + PI_B * P_B_GIVEN_C );
      float ll_not = log2( PI_F * P_F_GIVEN_NC + PI_B * P_B_GIVEN_NC );

      change_ll_array[gid] += ll ;
      no_change_ll_array[gid] += ll_not;

      float decision_prior = 0.5f;
      // if( (change_ll_array[gid] + log2(decision_prior)  ) > (no_change_ll_array[gid] + log2(1-decision_prior)) )
      //   change_array[gid] = 1.0f;
      // else
      //   change_array[gid] = 0.01f;

      float change_prob = exp2(change_ll_array[gid]);
      float no_change_prob = exp2(no_change_ll_array[gid]);
      change_array[gid] = change_prob / (change_prob + no_change_prob);
    }
    else {
      change_ll_array[gid] = 0.0f;
      no_change_ll_array[gid] = 1.0f;

      change_array[gid] = 0.0f;
    }


  }


}

#endif // ACCUM_CHANGE_LL
