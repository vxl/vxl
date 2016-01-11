#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif


#ifdef MOG_TYPE_16
    #define MOG_TYPE int4
#endif
#ifdef MOG_TYPE_8
    #define MOG_TYPE int2
#endif

#ifdef PROB_IMAGE
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE *  mog;
  float intensity;
  float* prob_image;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,constant RenderSceneInfo*,
              global int4*,local uchar16*,constant uchar*,local uchar*, float*, AuxArgs, float tnearf, float tfarf);

__kernel
void
compute_probability_of_image( __constant  RenderSceneInfo    * linfo,
                            __global    int4               * tree_array,
                            __global    float              * alpha_array,
                            __global    MOG_TYPE           * mixture_array,
                            __global    float16            * camera,        // camera orign and SVD of inverse of camera matrix
                            __global    float              * in_image,      // input image and store vis_inf and pre_inf
                            __global    float              * prob_image,      // input image and store vis_inf and pre_inf
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
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  //initial expected intensity from this camera angle
  float prob            = prob_image[imIndex[llid]];
  float intensity       = in_image[imIndex[llid]];
  float vis             = vis_image[imIndex[llid]];
  AuxArgs aux_args;
  aux_args.alpha        = alpha_array;
  aux_args.mog          = mixture_array;
  aux_args.intensity    = intensity;
  aux_args.prob_image   = &prob;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,

            //scene info
            linfo, tree_array,

            //utility info
            local_tree, bit_lookup, cumsum, &vis,

            //RENDER SPECIFIC ARGS
            aux_args,0, MAXFLOAT);

  //expected image gets rendered
  prob_image[imIndex[llid]]       = prob; //expected_int;
  vis_image[imIndex[llid]]        = vis;
}

#endif
#ifdef PROB_CUBIC_IMAGE


//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE *  mog;
  __global float  *  aux0;
  __global float *  aux1;
  float phi;
  float* prob_image;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,constant RenderSceneInfo*,
              global int4*,local uchar16*,constant uchar*,local uchar*, float*, AuxArgs,float tnear, float tfar);

__kernel
void
cubic_compute_probability_of_image( __constant  RenderSceneInfo  *  linfo,
                            __global    int4               *  tree_array,
                            __global    float              *  alpha_array,
                            __global    MOG_TYPE           *  mixture_array,
                            __global    float              *  aux0,
                            __global    float              *  aux1,
                  __global    float4             * ray_origins,
                  __global    float4             * ray_directions,
                            __global    float              *  prob_image,      // input image and store vis_inf and pre_inf
                            __global    float              *  vis_image,
                            __global    uint4              *  exp_image_dims,
                            __global    float              *  output,
                            __constant  uchar              *  bit_lookup,
                            __local     uchar16            *  local_tree,
                            __local     uchar              *  cumsum,        //cumulative sum helper for data pointer
                            __local     int                *  imIndex)
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
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  //initial expected intensity from this camera angle
  float prob            = prob_image[imIndex[llid]];
  float vis             = vis_image[imIndex[llid]];
  AuxArgs aux_args;
  aux_args.alpha         = alpha_array;
  aux_args.mog           = mixture_array;
  aux_args.aux0          = aux0;
  aux_args.aux1          = aux1;
  aux_args.prob_image    = &prob;
  aux_args.phi     = atan2(ray_d.y,ray_d.x);
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,

            //scene info
            linfo, tree_array,

            //utility info
            local_tree, bit_lookup, cumsum, &vis,

            //RENDER SPECIFIC ARGS
            aux_args);

  //expected image gets rendered
  prob_image[imIndex[llid]]       = prob;
  vis_image[imIndex[llid]]        = vis;
}

#endif
