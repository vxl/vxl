#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif
#ifdef RENDER_VIEW_DEP
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE*  mog;
  float4* expint;
  float * maxomega;
  float* app_model_weights;
  float* vis;
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

  float app_model_weights[8] = {0};
  float4 viewdir = (float4)(ray_dx,ray_dy,ray_dz,0);
  compute_app_model_weights(app_model_weights, viewdir,(__constant float4*)&app_model_view_directions);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  //uint  eint    = as_uint(exp_image[imIndex[llid]]);
  //uchar echar   = convert_uchar(eint);
  //float expint  = convert_float(echar)/255.0f;
  float4 expint  = exp_image[imIndex[llid]];
#ifdef YUV
  expint = rgb2yuv(expint);
#endif
  float vis     = vis_image[imIndex[llid]];
  float max_omega     = max_omega_image[imIndex[llid]];

  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.mog    = mixture_array;
  aux_args.expint = &expint;
  aux_args.vis = &vis;
  aux_args.app_model_weights = app_model_weights;
  aux_args.maxomega = &max_omega;



  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info


  //store the expected intensity (as UINT)
#ifdef YUV
  expint = yuv2rgb(expint);
#endif
  exp_image[imIndex[llid]] =  expint;
  //store visibility at the end of this block
  vis_image[imIndex[llid]] = vis;
  max_omega_image[imIndex[llid]] = max_omega;
}



void step_cell_render(AuxArgs    aux_args,
		                  int        data_ptr,
                      float      d)
{
  float alpha = aux_args.alpha[data_ptr];
  float diff_omega=exp(-alpha*d);

  float4 expected_app_cell= (float4)(0.0f,0.0f,0.0f,0.0f);
  // for rendering only

  if (diff_omega<0.995f) {
#ifdef MOG_VIEW_DEP_COLOR_COMPACT
      int8 mixture = aux_args.mog[data_ptr];
#else
      int16 mixture = aux_args.mog[data_ptr];
#endif
    int * mixture_array = (int*)(&mixture);
    float * app_model_weights = aux_args.app_model_weights;
    float sum_weights = 0.0f;
    for(short i= 0; i < 8; i++)
    {
#ifdef YUV
        // YUV
#ifdef MOG_VIEW_DEP_COLOR_COMPACT
      float4 tmp_mu = unpack_yuv(mixture_array[i]);
#else
      float4 tmp_mu = unpack_yuv(mixture_array[2*i]);
#endif
#else
      // RGB
#ifdef MOG_VIEW_DEP_COLOR_COMPACT
      float4 tmp_mu = convert_float4(unpack_uchar4(mixture_array[i])) / 255.0f;
#else
      float4 tmp_mu = convert_float4(unpack_uchar4(mixture_array[2*i])) / 255.0f;
#endif
#endif
      bool viewpoint_has_data = tmp_mu.s3 > 0.0f;
      if(viewpoint_has_data && (app_model_weights[i] > 0.01)) {
          expected_app_cell += app_model_weights[i] * tmp_mu;
          sum_weights += app_model_weights[i];
      }
    }
    // need to normalize because we may have encountered some viewpoints w/no data.
    if (sum_weights > 0.0f) {
        expected_app_cell /= sum_weights;
    }
  }

  float omega=(*aux_args.vis) * (1.0f - diff_omega);
  (*aux_args.vis) *= diff_omega;
  (*aux_args.expint)+= expected_app_cell * omega;
}


#endif //RENDER_VEW_DEP
