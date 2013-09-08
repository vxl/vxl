#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef RENDER_ALPHA_INTEGRAL
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  float* alpha_int;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs,float tnear, float tfar);
__kernel
void
render_alpha_integral( __constant  RenderSceneInfo * linfo,
                       __global    int4            * tree_array,
                       __global    float           * alpha_array,
                       __global    float4          * ray_origins,
                       __global    float4          * ray_directions,
                       __global    float           * alpha_int_image, // input image and store vis_inf and pre_inf
                       __global    uint4           * exp_image_dims,
                       __global    float           * output,
                       __constant  uchar           * bit_lookup,
                       __local     uchar16         * local_tree,
                       __local     uchar           * cumsum,          // cumulative sum helper for data pointer
                       __local     int             * imIndex)
{
  //----------------------------------------------------------------------------
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*exp_image_dims).z || j>=(*exp_image_dims).w)
    return;

  //Store image index (may save a register).  Also initialize VIS and expected_int
  imIndex[llid] = j*get_global_size(0)+i;

  //----------------------------------------------------------------------------
  // Calculate ray origin, and direction
  // (make sure ray direction is never axis aligned)
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ imIndex[llid] ];
  float4 ray_d = ray_directions[ imIndex[llid] ];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
#if 0
  calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
#endif
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
#if 0
  uint  eint    = as_uint(exp_image[imIndex[llid]]);
  uchar echar   = convert_uchar(eint);
  float expint  = convert_float(echar)/255.0f;
#endif
  float alpha_int  = alpha_int_image[imIndex[llid]];
  float vis     = 1.0;
  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.alpha_int = &alpha_int;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info
  //store the expected intensity (as UINT)
  alpha_int_image[imIndex[llid]] =  alpha_int;
}
#endif

#ifdef RENDER_USING_ALPHA_INTEGRAL
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE*  mog;
  float* expint;
  float* alpha_int;
  float* alpha_int_cum;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs,float tnear, float tfar);
__kernel
void
render_using_alpha_integral( __constant  RenderSceneInfo    * linfo,
                             __global    int4               * tree_array,
                             __global    float              * alpha_array,
                             __global    MOG_TYPE           * mixture_array,
                             __global    float4             * ray_origins,
                             __global    float4             * ray_directions,
                             __global    float              * exp_image,     // input image and store vis_inf and pre_inf
                             __global    float              * alpha_integral_image,
                             __global    float              * alpha_integral_cum_image,
                             __global    uint4              * exp_image_dims,
                             __global    float              * output,
                             __constant  uchar              * bit_lookup,
                             __local     uchar16            * local_tree,
                             __local     uchar              * cumsum,        // cumulative sum helper for data pointer
                             __local     int                * imIndex)
{
  //----------------------------------------------------------------------------
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*exp_image_dims).z || j>=(*exp_image_dims).w)
    return;

  //Store image index (may save a register).  Also initialize VIS and expected_int
  imIndex[llid] = j*get_global_size(0)+i;

  //----------------------------------------------------------------------------
  // Calculate ray origin, and direction
  // (make sure ray direction is never axis aligned)
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ imIndex[llid] ];
  float4 ray_d = ray_directions[ imIndex[llid] ];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
#if 0
  calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
#endif
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
#if 0
  uint  eint          = as_uint(exp_image[imIndex[llid]]);
  uchar echar         = convert_uchar(eint);
  float expint        = convert_float(echar)/255.0f;
#endif
  float expint        = exp_image[imIndex[llid]];
  float alpha_int     = alpha_integral_image[imIndex[llid]];
  float alpha_int_cum = alpha_integral_cum_image[imIndex[llid]];
  AuxArgs aux_args;
  aux_args.alpha      = alpha_array;
  aux_args.mog        = mixture_array;
  aux_args.expint     = &expint;
  aux_args.alpha_int  = &alpha_int;
  aux_args.alpha_int_cum = &alpha_int_cum;

  float vis  = 1.0f;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info

  //store the expected intensity (as UINT)
  exp_image[imIndex[llid]] =  expint;
  //store visibility at the end of this block
  alpha_integral_image[imIndex[llid]] = alpha_int;
}

#endif
