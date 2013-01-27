#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef COMPINDEX_LABEL
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE*  mog;
  float* expint;
  float * maxomega;
} AuxArgs;

//forward declare cast ray
void cast_ray_render_vis2(int,int,float,float,float,float,float,float,
                          __constant  RenderSceneInfo*, __global int4*,
                          __local uchar16*, __constant  uchar *,__local uchar *,
                          float *, float *, AuxArgs);


void step_cell_label_max(__global MOG_TYPE * cell_data,
                         __global float    * alpha_data,
                                  int        data_ptr,
                                  float      d,
                                  float    * vis,
                                  float    * expected_i,
                                  float    * max_omega)
{
  float alpha = alpha_data[data_ptr];
  float diff_omega=exp(-alpha*d);
  float expected_int_cell=0.0f;
  // for rendering only

  float omega=(*vis) * (1.0f - diff_omega);
  (*vis) *= diff_omega;
  if (omega > (*max_omega) )
  {
    (*expected_i)=cell_data[data_ptr];
    (*max_omega) = omega ;
  }
}

__kernel
void
compute_index_label(
                     __constant  RenderSceneInfo    * linfo,
                     __global    int4               * tree_array,
                     __global    float              * alpha_array,
                     __global    MOG_TYPE           * mixture_array,
                     __constant  float4             * ray_o,
                     __global    float4             * directions,
                     __constant  int                * ray_size,
                     __constant  float              * max_dist,
                     __global    float              * exp_image,      // input image and store vis_inf and pre_inf
                     __constant  uchar              * bit_lookup,
                     __global    float              * vis_image,
                     __global    float              * max_omega_image,
                     __local     uchar16            * local_tree,
                     __local     uchar              * cumsum
                   )
{
  int gid=get_global_id(0);
  if (gid >= *ray_size)
      return;

  float4 ray_d = directions[ gid ];

  //declare ray
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray_generic_cam(linfo, *ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------

  float expint  = exp_image[ gid ];
  float vis     = vis_image[ gid ];
  float max_omega     = max_omega_image[ gid ];

  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.mog    = mixture_array;
  aux_args.expint = &expint;
  aux_args.maxomega = &max_omega;
  float tfar_max = (*max_dist)/linfo->block_len;

  cast_ray_render_vis2( 1, 1,
                        ray_ox, ray_oy, ray_oz,
                        ray_dx, ray_dy, ray_dz,
                        linfo, tree_array,                                    //scene info
                        local_tree, bit_lookup, cumsum, &tfar_max, &vis, aux_args); //utility info

  exp_image[ gid ] =  expint;
  //store visibility at the end of this block, step function in cast ray updates vis value for each voxel along the ray
  vis_image[ gid ] = vis;
  max_omega_image[ gid ] = max_omega;
}
#endif
