#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef RENDER
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE*  mog;
  float* expint;
  float * maxomega;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs, float tnear, float tfar);
__kernel
void
render_bit_scene( __constant  RenderSceneInfo    * linfo,
                  __global    int4               * tree_array,
                  __global    float              * alpha_array,
                  __global    MOG_TYPE           * mixture_array,
                  __global    float4             * ray_origins,
                  __global    float4             * ray_directions,
                  __global    float              * nearfarplanes,
                  __global    float              * exp_image,      // input image and store vis_inf and pre_inf
                  __global    uint4              * exp_image_dims,
                  __global    float              * output,
                  __constant  uchar              * bit_lookup,
                  __global    float              * vis_image,
                  __global    float              * max_omega_image,
                  __local     uchar16            * local_tree,
                  __local     uchar              * cumsum,        //cumulative sum helper for data pointer
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
  //calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  //uint  eint    = as_uint(exp_image[imIndex[llid]]);
  //uchar echar   = convert_uchar(eint);
  //float expint  = convert_float(echar)/255.0f;
  float expint  = exp_image[imIndex[llid]];
  float vis     = vis_image[imIndex[llid]];
  float max_omega     = max_omega_image[imIndex[llid]];

  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.mog    = mixture_array;
  aux_args.expint = &expint;
  aux_args.maxomega = &max_omega;
  float nearplane = nearfarplanes[0]/linfo->block_len;
  float farplane = nearfarplanes[1]/linfo->block_len;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,nearplane,farplane);      //utility info

  //store the expected intensity (as UINT)
  exp_image[imIndex[llid]] =  expint;
  //store visibility at the end of this block
  vis_image[imIndex[llid]] = vis;
  max_omega_image[imIndex[llid]] = max_omega;
}
#endif


#ifdef RENDER_VIEW_DEP
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE*  mog;
  float* expint;
  float * maxomega;
  float* app_model_weights;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs,float tnear, float tfar);
__kernel
void
render_bit_scene( __constant  RenderSceneInfo    * linfo,
                  __global    int4               * tree_array,
                  __global    float              * alpha_array,
                  __global    MOG_TYPE           * mixture_array,
                  __global    float4             * ray_origins,
                  __global    float4             * ray_directions,
                  __global    float              * nearfarplanes,
                  __global    float              * exp_image,      // input image and store vis_inf and pre_inf
                  __global    uint4              * exp_image_dims,
                  __global    float              * output,
                  __constant  uchar              * bit_lookup,
                  __global    float              * vis_image,
                  __global    float              * max_omega_image,
                  __local     uchar16            * local_tree,
                  __local     uchar              * cumsum,        //cumulative sum helper for data pointer
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
  //calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  float app_model_weights[8] = {0};
  float4 viewdir = (float4)(ray_dx,ray_dy,ray_dz,0);
  compute_app_model_weights(app_model_weights, viewdir,&app_model_view_directions);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  //uint  eint    = as_uint(exp_image[imIndex[llid]]);
  //uchar echar   = convert_uchar(eint);
  //float expint  = convert_float(echar)/255.0f;
  float expint  = exp_image[imIndex[llid]];
  float vis     = vis_image[imIndex[llid]];
  float max_omega     = max_omega_image[imIndex[llid]];

  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.mog    = mixture_array;
  aux_args.expint = &expint;
  aux_args.app_model_weights = app_model_weights;
  aux_args.maxomega = &max_omega;



  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info



  //store the expected intensity (as UINT)
  exp_image[imIndex[llid]] =  expint;
  //store visibility at the end of this block
  vis_image[imIndex[llid]] = vis;
  max_omega_image[imIndex[llid]] = max_omega;
}
#endif


#ifdef RENDER_DEPTH
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  float* expdepth;
  float* expdepthsqr;
  float* probsum;
  float* t;
  float* vis;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs,float tnear, float tfar);
__kernel
void
render_depth( __constant  RenderSceneInfo    * linfo,
              __global    int4               * tree_array,
              __global    float              * alpha_array,
              __global    float4             * ray_origins,
              __global    float4             * ray_directions,
              __global    float              * exp_image,        // camera orign and SVD of inverse of camera matrix
              __global    float              * exp_sqr_image,    // input image and store vis_inf and pre_inf
              __global    uint4              * exp_image_dims,   // sum of squares.
              __global    float              * output,
              __constant  uchar              * bit_lookup,
              __global    float              * vis_image,
              __global    float              * prob_image,
              __global    float              * t_image,
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
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------

  float expdepth    = 0.0f;
  float expdepthsqr = 0.0f;
  float probsum     = prob_image[imIndex[llid]];
  float vis_rec     = vis_image[imIndex[llid]];
  float t           = t_image[imIndex[llid]];
  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.expdepth = &expdepth;
  aux_args.expdepthsqr = &expdepthsqr;
  aux_args.probsum = &probsum;
  aux_args.t = &t;
  aux_args.vis = &vis_rec;

  float vis = 1.0;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info

  //store the expected intensity
  exp_image[imIndex[llid]] += (* aux_args.expdepth);
  exp_sqr_image[imIndex[llid]] += (* aux_args.expdepthsqr);
  prob_image[imIndex[llid]] = (* aux_args.probsum);
  //store visibility at the end of this block
  vis_image[imIndex[llid]]  = vis_rec;
  t_image[imIndex[llid]]  = (* aux_args.t) ;
}
#endif

#ifdef CHANGE
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global int2*  mog;
  float* expint;
  float* intensity;
  float* exp_prob_int;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,constant RenderSceneInfo*,
              global int4*,local uchar16*,constant uchar*,local uchar*, float*, AuxArgs,float tnear, float tfar);

__kernel
void
change_detection_bit_scene( __constant  RenderSceneInfo    * linfo,
                            __global    int4               * tree_array,
                            __global    float              * alpha_array,
                            __global    uchar8             * mixture_array,
                            __global    float4             * ray_origins,
                            __global    float4             * ray_directions,
                            //          __global    float16            * camera,        // camera orign and SVD of inverse of camera matrix
                            __global    float              * in_image,      // input image and store vis_inf and pre_inf
                            __global    uint               * exp_image,      // input image and store vis_inf and pre_inf
                            __global    uint               * prob_exp_image,       //input image
                            __global    uint               * change_image,      // input image and store vis_inf and pre_inf
                            __global    uint4              * exp_image_dims,
                            __global    float              * output,
                            __constant  uchar              * bit_lookup,
                            __global    float              * vis_image,
                            __local     uchar16            * local_tree,
                            __local     uchar              * cumsum,        //cumulative sum helper for data pointer
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
  imIndex[llid] = j*get_global_size(0)+i;

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*exp_image_dims).z || j>=(*exp_image_dims).w) {
    return;
  }

  //----------------------------------------------------------------------------
  // Calculate ray origin, and direction
  // (make sure ray direction is never axis aligned)
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ imIndex[llid] ];
  float4 ray_d = ray_directions[ imIndex[llid] ];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  //calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  //initial expected intensity from this camera angle
  uint  eint  = as_uint(exp_image[imIndex[llid]]);
  uchar echar = convert_uchar(eint);
  float expected_int = convert_float(echar)/255.0f;
  //input from exp_img
  uint  in_eint  = as_uint(in_exp_image[imIndex[llid]]);
  uchar in_echar = convert_uchar(in_eint);
  float exp_intensity= convert_float(in_echar)/255.0f;
  //input from actual image
  float intensity=in_image[imIndex[llid]];
  //input from probab
  uint  in_prob_eint  = as_uint(prob_exp_image[imIndex[llid]]);
  uchar in_prob_echar = convert_uchar(in_prob_eint);
  float expected_prob_int= convert_float(in_prob_echar)/255.0f;
  AuxArgs aux_args;
  aux_args.alpha = alpha_array; aux_args.mog = mixture_array;
  aux_args.expint = &exp_intensity;
  aux_args.intensity = &intensity;
  aux_args.exp_prob_int = &expected_prob_int;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,

            //scene info
            linfo, tree_array,

            //utility info
            local_tree, bit_lookup, cumsum, &vis,

            //RENDER SPECIFIC ARGS
            aux_args,0,MAXFLOAT);

  //expected image gets rendered
  exp_image[imIndex[llid]] =  rgbaFloatToInt((float4) 1.0f/(1.0f+expected_int)); //expected_int;
  prob_exp_image[imIndex[llid]] =  rgbaFloatToInt((float4) 1.0f/(1.0f+expected_prob_int)); //expected_int;
  vis_image[imIndex[llid]] = vis;
}
#endif

#ifdef RENDER_PHONGS
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global float8 *  mog;
  __global float4* sundir;
  float4 * ray_dir;

  float* expint;
} AuxArgs;
void step_cell_render_phongs(AuxArgs   args,
                             int      data_ptr,
                             float    d,
                             float  * vis,
                             float  * expected_i)
{
  float alpha = args.alpha[data_ptr];
  float diff_omega=exp(-alpha*d);
  float expected_int_cell=0.0f;
  // for rendering only
  if (diff_omega<0.995f)
  {
      float8  data=args.mog[data_ptr];
      expected_int_cell = 0.0;
      if (data.s0 >0.0)
      {
          float costheta = cos(data.s3);
          float sintheta = sin(data.s4);
          float cosphi = cos(data.s4);
          float sinphi = sin(data.s4);
          float4 normal =(float4) ( sintheta*cosphi,sintheta*sinphi,costheta,0.0f);
          float4 sundir = args.sundir[0];
          float4 raydir = args.ray_dir[0];

          float dotviewnormal = dot(normal, raydir);
          float4 mirrorv= raydir-2*dotviewnormal*normal;
          mirrorv.w=0.0;

          float dotmirrorvlight= min(fabs(dot(mirrorv, sundir)),1.0);
          expected_int_cell =data.s0*fabs(dot(normal,sundir));//+min(data.s1,1.0)*pow(dotmirrorvlight,fabs(data.s2));
      }
  }
  float omega=(*vis) * (1.0f - diff_omega);
  (*vis) *= diff_omega;
  (*expected_i)+=expected_int_cell*omega;
}

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs,float tnear, float tfar);
__kernel
void
render_bit_scene( __constant  RenderSceneInfo    * linfo,
                  __global    int4               * tree_array,
                  __global    float              * alpha_array,
                  __global    float8             * mixture_array,
                  __global    float4             * ray_origins,
                  __global    float4             * ray_directions,
                  __global    float4             * sun_direction,
                  __global    float              * exp_image,      // input image and store vis_inf and pre_inf
                  __global    uint4              * exp_image_dims,
                  __global    float              * output,
                  __constant  uchar              * bit_lookup,
                  __global    float              * vis_image,
                  __local     uchar16            * local_tree,
                  __local     uchar              * cumsum,        //cumulative sum helper for data pointer
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
  //calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  //uint  eint    = as_uint(exp_image[imIndex[llid]]);
  //uchar echar   = convert_uchar(eint);
  //float expint  = convert_float(echar)/255.0f;
  float expint  = exp_image[imIndex[llid]];
  float vis     = vis_image[imIndex[llid]];
  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.mog    = mixture_array;
  aux_args.expint = &expint;
  aux_args.sundir = sun_direction;

  aux_args.ray_dir = &ray_d;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info

  //store the expected intensity (as UINT)
  exp_image[imIndex[llid]] =  expint;
  //store visibility at the end of this block
  vis_image[imIndex[llid]] = vis;
}
#endif


#ifdef RENDER_Z_IMAGE
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  float* exp_z;
  float* exp_z_sqr;
  float* probsum;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs, float tnear, float tfar);
__kernel
void
render_z_image( __constant  RenderSceneInfo    * linfo,
                __global    int4               * tree_array,
                __global    float              * alpha_array,
                __global    float4             * ray_origins,
                __global    float4             * ray_directions,
                __global    float              * exp_image,
                __global    float              * exp_sqr_image, // sum of squares.
                __global    uint4              * exp_image_dims,
                __global    float              * output,
                __constant  uchar              * bit_lookup,
                __global    float              * vis_image,
                __global    float              * prob_image,
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

  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------

  float exp_z   = 0.0f;
  float exp_z_sqr = 0.0f;
  float probsum = prob_image[imIndex[llid]];
  float vis     = vis_image[imIndex[llid]];
  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.exp_z = &exp_z;
  aux_args.exp_z_sqr = &exp_z_sqr;
  aux_args.probsum = &probsum;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args, 0 , MAXFLOAT);      //utility info

  //store the expected intensity
  exp_image[imIndex[llid]] += *aux_args.exp_z;
  exp_sqr_image[imIndex[llid]] += *aux_args.exp_z_sqr;
  prob_image[imIndex[llid]] = *aux_args.probsum;
  //store visibility at the end of this block
  vis_image[imIndex[llid]]  = vis;
}
#endif

#ifdef RENDER_NAA
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global float* naa_apm;
  __global float16* radiance_scales;
  __global float16* radiance_offsets;
  float* expint;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs,float tnear, float tfar);
__kernel
void
render_bit_scene( __constant  RenderSceneInfo    * linfo,
                  __global    int4               * tree_array,
                  __global    float              * alpha_array,
                  __global    float              * naa_array,
                  __global    float16            * radiance_scales,
                  __global    float16            * radiance_offsets,
                  __global    float4             * ray_origins,
                  __global    float4             * ray_directions,
                  __global    float              * exp_image,      // input image and store vis_inf and pre_inf
                  __global    uint4              * exp_image_dims,
                  __global    float              * output,
                  __constant  uchar              * bit_lookup,
                  __global    float              * vis_image,
                  __local     uchar16            * local_tree,
                  __local     uchar              * cumsum,        //cumulative sum helper for data pointer
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
  //calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  //uint  eint    = as_uint(exp_image[imIndex[llid]]);
  //uchar echar   = convert_uchar(eint);
  //float expint  = convert_float(echar)/255.0f;
  float expint  = exp_image[imIndex[llid]];
  float vis     = vis_image[imIndex[llid]];
  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.naa_apm    = naa_array;
  aux_args.radiance_scales = radiance_scales;
  aux_args.radiance_offsets = radiance_offsets;
  aux_args.expint = &expint;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info

  //store the expected intensity (as UINT)
  exp_image[imIndex[llid]] =  expint;
  //store visibility at the end of this block
  vis_image[imIndex[llid]] = vis;
}
#endif

#ifdef RENDER_ALBEDO_NORMAL
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global float* naa_apm;
           float4* expected_albedo_normal;
  __global float16* normals_x;
  __global float16* normals_y;
  __global float16* normals_z;
  float* expint;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs,float tnear, float tfar);
__kernel
void
render_albedo_normal( __constant  RenderSceneInfo    * linfo,
                      __global    int4               * tree_array,
                      __global    float              * alpha_array,
                      __global    float              * naa_array,
                      __global    float16            * normals_x,
                      __global    float16            * normals_y,
                      __global    float16            * normals_z,
                      __global    float4             * ray_origins,
                      __global    float4             * ray_directions,
                      __global    float4             * exp_image,      // expected albedo and normal values
                      __global    uint4              * exp_image_dims,
                      __global    float              * output,
                      __constant  uchar              * bit_lookup,
                      __global    float              * vis_image,
                      __local     uchar16            * local_tree,
                      __local     uchar              * cumsum,        //cumulative sum helper for data pointer
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
  //calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  //uint  eint    = as_uint(exp_image[imIndex[llid]]);
  //uchar echar   = convert_uchar(eint);
  //float expint  = convert_float(echar)/255.0f;
  float4 exp_albedo_normal  = exp_image[imIndex[llid]];
  float vis = vis_image[imIndex[llid]];
  AuxArgs aux_args;
  aux_args.alpha = alpha_array;
  aux_args.naa_apm = naa_array;
  aux_args.normals_x = normals_x;
  aux_args.normals_y = normals_y;
  aux_args.normals_z = normals_z;
  aux_args.expected_albedo_normal = &exp_albedo_normal;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info

  //store the expected albedo and normal
  exp_image[imIndex[llid]] =  exp_albedo_normal;
  //store visibility at the end of this block
  vis_image[imIndex[llid]] = vis;
}
#endif


#ifdef RENDER_VISIBILITY
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  float* vis;
} AuxArgs;

void cast_ray_render_vis(int,int,float,float,float,float,float,float,
                         __constant RenderSceneInfo*, __global int4*,
                         __local uchar16*, __constant uchar *,__local uchar *,
                         float*, AuxArgs);
__kernel
void
render_visibiltiy( __constant  RenderSceneInfo    * linfo,
                   __global    int4               * tree_array,
                   __global    float              * alpha_array,
                   __global    float4             * ray_origin,
                   __global    float4             * ray_directions,
                   __global    uint4              * exp_image_dims,
                   __global    float              * output,
                   __constant  uchar              * bit_lookup,
                   __global    float              * vis_image,
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
  float4 ray_o = *ray_origin;
  float4 ray_d = ray_directions[ imIndex[llid] ];

  float tfar_max = ray_d.w/linfo->block_len;
  ray_d.w =0.0;
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------


  float vis_rec     = vis_image[imIndex[llid]];

  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;

  aux_args.vis = &vis_rec;

  cast_ray_render_vis( i, j,
                       ray_ox, ray_oy, ray_oz,
                       ray_dx, ray_dy, ray_dz,
                       linfo, tree_array,                                    //scene info
                       local_tree, bit_lookup, cumsum, &tfar_max, aux_args); //utility info


  //store visibility at the end of this block
  vis_image[imIndex[llid]]  = vis_rec;
}

#endif
