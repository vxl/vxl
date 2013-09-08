#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif


#ifdef RENDER_SUN_VIS
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global float* auxsun;
  float* expint;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs,float tnear, float tfar);
__kernel
void
render_sun_vis_scene( __constant  RenderSceneInfo    * linfo,
                  __global    int4               * tree_array,
                  __global    float              * alpha_array,
                  __global    float              * auxsun,
                  __global    float4             * ray_origins,
                  __global    float4             * ray_directions,
                  //__global    float16            * camera,        // camera orign and SVD of inverse of camera matrix
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
  float expint  = exp_image[imIndex[llid]];
  float vis     = vis_image[imIndex[llid]];
  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.auxsun    = auxsun;
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


