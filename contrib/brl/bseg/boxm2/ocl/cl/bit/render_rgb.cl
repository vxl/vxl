#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef RENDER
//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
// NOTE THAT RGB MOG is stored as follows:
// [mu_0R, mu_0G, mu_0B, x] [sig_0R, sig_0G, sig_0B, w_0] [mu_1R, mu_1G, mu_1B, x] [sig_1R, sig_1G, sig_1B, x];
typedef struct
{
  __global float*   alpha; //alpha
  __global int2 *   mog;    //mixture of 2 3-d gaussians (as uchar 16
           float*   vis;
           float4*  expint; //expected intensity (3 channels)
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,__local uchar16*,__constant uchar*,__local uchar*,float*,AuxArgs, float tnear, float tfar);
__kernel
void
render_bit_scene( __constant  RenderSceneInfo    * linfo,
                  __global    int4               * tree_array,
                  __global    float              * alpha_array,
                  __global    int2               * mixture_array,
                  __global    float4             * ray_origins,
                  __global    float4             * ray_directions,
                  __global    float              * nearfarplanes,
                  __global    float4             * exp_image,      // input image and store vis_inf and pre_inf
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

  float4  expint = exp_image[imIndex[llid]];
#ifdef YUV
  expint = rgb2yuv(expint);
#endif
  float  vis     = vis_image[imIndex[llid]];
  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.mog    = mixture_array;
  aux_args.expint = &expint;
  aux_args.vis    = &vis;
  float nearplane = nearfarplanes[0]/linfo->block_len;
  float farplane = nearfarplanes[1]/linfo->block_len;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,nearplane,farplane);      //utility info

  //store the expected intensity (as UINT)
  //YUV edit
#ifdef YUV
  expint = yuv2rgb(expint);
#endif
  exp_image[imIndex[llid]] = expint;

  //store visibility at the end of this block
  vis_image[imIndex[llid]] = vis;
}

void step_cell_render(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
  float alpha = aux_args.alpha[data_ptr];
  float diff_omega = exp(-alpha*d);
  float4 expected_int_cell = 0.0f;

  // for rendering only
  if (diff_omega<0.995f)
  {
    uchar8 udata = as_uchar8(aux_args.mog[data_ptr]);
    float8 data = convert_float8(udata)/255.0f;

    //expected cell is just the means
    expected_int_cell = (float4)data.s0123;

    //undo the step taken in compress RGB U/V is in [0,1],
    // put them back in ranges U in [-.436, .436] and V in [-.615, .615]
#ifdef YUV
    expected_int_cell.y = expected_int_cell.y*U_RANGE - U_MAX;
    expected_int_cell.z = expected_int_cell.z*V_RANGE - V_MAX;
#endif
  }

  //calc and store visibility
  float omega = (*aux_args.vis) * (1.0f - diff_omega);
  (*aux_args.vis) *= diff_omega;
  (*aux_args.expint) += expected_int_cell*omega;
}

#endif // RENDER
