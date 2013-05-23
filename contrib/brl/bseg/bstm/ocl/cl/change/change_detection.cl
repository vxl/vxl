#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef CHANGE

typedef struct
{
  __global float*       alpha;
  __global MOG_TYPE *   mog;
           float4       intensity;
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
                            __global    float4             * in_image,      // input image and store vis_inf and pre_inf
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
  float4 intensity      = in_image[imIndex[llid]];
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
  CONVERT_FUNC_FLOAT16(mixture, aux_args.mog[data_ptr_tt])/NORM;

  float img_in = aux_args.intensity.x;

  float prob_den = view_dep_mixture_model(img_in, mixture, aux_args.app_model_weights);

  float alpha = aux_args.alpha[data_ptr_tt];
  float prob  = 1.0f - exp(-alpha*d);
  float omega = (*aux_args.vis)*prob;
  (*aux_args.vis) = (*aux_args.vis)*(1.0f-prob);

  //set change
  (*aux_args.change) += prob_den*omega;
}

#endif
