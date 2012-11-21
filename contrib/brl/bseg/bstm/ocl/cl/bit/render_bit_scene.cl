#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#if GAUSS_TYPE_2
    #define MOG_TYPE uchar2
    #define CONVERT_FUNC(lhs,data) uchar2 lhs1 = as_uchar2(data); uchar8 lhs = as_uchar8(0.0); lhs.s0 = lhs1.s0; lhs.s1 = lhs1.s1; lhs.s2 = 255;
    #define NORM 255;
#endif

#ifdef RENDER
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE*  mog;
  float* expint;
  float*   vis;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*, __global int2*, __local uchar16*,__local uchar8*,__constant uchar*,__local uchar*,float*,AuxArgs);
__kernel
void
render_bit_scene( __constant  RenderSceneInfo    * linfo,
                  __global    int4               * tree_array,
                  __global    int2               * time_tree_array,
                  __global    float              * alpha_array,
                  __global    MOG_TYPE           * mixture_array,
                  __global    float4             * ray_origins,
                  __global    float4             * ray_directions,
                  __global    float              * exp_image,      // input image and store vis_inf and pre_inf
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
  aux_args.vis    = &vis;

  
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            *time,
            linfo, tree_array, time_tree_array,                   //scene info
            local_tree, local_time_tree, bit_lookup, cumsum, &vis, aux_args); 
              
              
  //store the expected intensity (as UINT)
  exp_image[imIndex[llid]] =  expint;
  //store visibility at the end of this block
  vis_image[imIndex[llid]] = vis;
}



void step_cell_render(AuxArgs aux_args, int data_ptr_tt, float d)
{
  float alpha = aux_args.alpha[data_ptr_tt];
  float diff_omega=exp(-alpha*d);
  float expected_int_cell=0.0f;
  // for rendering only
  if (diff_omega<0.995f)
  {
    CONVERT_FUNC(udata,aux_args.mog[data_ptr_tt]);
    float8  data=convert_float8(udata)/(float)NORM;
    EXPECTED_INT(expected_int_cell,data);
  }
  float omega=(*aux_args.vis) * (1.0f - diff_omega);
  (*aux_args.vis)  *= diff_omega;
  (*aux_args.expint) +=expected_int_cell*omega;
}
#endif
