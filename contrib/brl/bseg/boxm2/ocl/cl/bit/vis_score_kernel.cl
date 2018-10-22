#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef RAYTRACE_PASS

typedef struct
{
  __global float * alpha;
  __global int  * seg_len;
#ifdef USE_SURFACE_NORMALS
  __global float4* normal;
#endif
  float4 view_dir;
  __global int  * vis_array;
  __local  short2* ray_bundle_array;
  __local  int*    cell_ptrs;
 // __local  float*  cached_vis;
  float*  ray_vis;
} AuxArgs;


//bayes step cell functor
void step_cell_vis_score(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //slow beta calculation ----------------------------------------------------

    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);
    //calculate this ray's contribution to beta
    float alpha = aux_args.alpha[data_ptr];

    float cell_vis = (*aux_args.ray_vis);

    // apply sigmoid function to push towards 0 or 1
    constexpr float sigmoid_center = 0.4;
    const float sigmoid_slope = 15.0f;
    float vis_score = 1.0f/(1.0f + exp(-sigmoid_slope * (cell_vis - sigmoid_center)));

#ifdef USE_SURFACE_NORMALS
    float4 cell_normal = aux_args.normal[data_ptr];
    // make sure cell_normal has mag. 1
    cell_normal /= length(cell_normal.s012);

    vis_score *= fabs(dot(cell_normal.s012, aux_args.view_dir.s012));
#endif
    (*aux_args.ray_vis) *= exp(-alpha * d);

    //discretize and store beta and vis contribution
    int vis_int  = convert_int_rte(d * vis_score * SEGLEN_FACTOR);
    atom_add(&aux_args.vis_array[data_ptr], vis_int);
    //--------------------------------------------------------------------------
}

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);

__kernel
void
vis_score_raytrace_main(__constant  RenderSceneInfo    * linfo,
                     __global    int4               * tree_array,        // tree structure for each block
                     __global    float              * alpha_array,       // alpha for each block
#ifdef USE_SURFACE_NORMALS
                     __global    float4             * normal_array,
#endif
                     __global    int                * aux_array0,        // seg_len accumulation
                     __global    int                * aux_array1,        // score accumulation
                     __constant  uchar              * bit_lookup,        // used to get data_index
                     __global    float4             * ray_origins,
                     __global    float4             * ray_directions,
                     __global    uint4              * imgdims,           // dimensions of the input image
                     __global    float              * vis_image,         // visibility image (for keeping vis across blocks)
                     __global    float              * output,
                     __local     uchar16            * local_tree,        // cache current tree into local memory
                     __local     short2             * ray_bundle_array,  // gives information for which ray takes over in the workgroup
                     __local     int                * cell_ptrs,         // local list of cell_ptrs (cells that are hit by this workgroup
                     __local     float              * cached_vis,        // cached vis used to sum up vis contribution locally
                     __local     uchar              * cumsum)            // cumulative sum for calculating data pointer
{
  // get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  // initialize pre-broken ray information (non broken rays will be re initialized)
  ray_bundle_array[llid] = (short2) (-1, 0);
  cell_ptrs[llid] = -1;

  //----------------------------------------------------------------------------
  // get image coordinates and camera,
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;
  float vis = vis_image[j*get_global_size(0) + i];
  float vis0 = 1.0; // don't want to terminate raytrace early, even if visibility is 0

  barrier(CLK_LOCAL_MEM_FENCE);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ j*get_global_size(0) + i ];
  float4 ray_d = ray_directions[ j*get_global_size(0) + i ];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;

  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  AuxArgs aux_args;
  aux_args.alpha       = alpha_array;
#ifdef USE_SURFACE_NORMALS
  aux_args.normal      = normal_array;
#endif
  aux_args.view_dir    = ray_d;
  aux_args.seg_len     = aux_array0;
  aux_args.vis_array   = aux_array1;

  aux_args.ray_bundle_array = ray_bundle_array;
  aux_args.cell_ptrs = cell_ptrs;
  //aux_args.cached_vis = cached_vis;
  aux_args.ray_vis = &vis;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis0, aux_args,0,MAXFLOAT);    //utility info

  //write out vis and pre
  vis_image[j*get_global_size(0)+i] = vis;

}
#endif

#ifdef UPDATE_PASS
__kernel void
update_vis_score_main(__constant  RenderSceneInfo    * linfo,
                         __global float* aux_array0,
                         __global float* aux_array1,
                         __global float* vis_score)
{
  int gid=get_global_id(0);
  int datasize = linfo->data_len ;
  if (gid<datasize)
  {
    int seg_len_sum_int = as_int(aux_array0[gid]);
    int vis_score_sum_int = as_int(aux_array1[gid]);
    float this_vis_score = 0.0f;
    if(seg_len_sum_int > 0) {
        this_vis_score = ((float)vis_score_sum_int)/((float)seg_len_sum_int);
    }
    // take max vis score
    if (vis_score[gid] < this_vis_score) {
        vis_score[gid] = this_vis_score;
    }
  }
}
#endif

#ifdef RENDER_VIS_SCORE

typedef struct
{
  __global float* alpha;
  __global float*  vis_score;
  float* exp_vis_score;
  float * vis;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs,float tnear, float tfar);
__kernel
void
render_vis_score_bit_scene( __constant  RenderSceneInfo    * linfo,
                       __global    int4               * tree_array,
                       __global    float              * alpha_array,
                       __global    ushort             * vis_score_array,
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
  float exp_vis_score  = exp_image[imIndex[llid]];
  float vis       = vis_image[imIndex[llid]];
  float max_omega = max_omega_image[imIndex[llid]];

  AuxArgs aux_args;
  aux_args.alpha  = alpha_array;
  aux_args.vis = &vis;
  aux_args.vis_score    = vis_score_array;
  aux_args.exp_vis_score = &exp_vis_score;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info

  //store the expected intensity (as UINT)
  exp_image[imIndex[llid]] =  exp_vis_score;
  //store visibility at the end of this block
  vis_image[imIndex[llid]] = vis;
  max_omega_image[imIndex[llid]] = max_omega;
}

void step_cell_render_vis_score(AuxArgs aux_args,
                                int        data_ptr,
                                float      d)
{
  float alpha = aux_args.alpha[data_ptr];
  float diff_omega=exp(-alpha*d);

  const float vis_score = aux_args.vis_score[data_ptr];

  float omega=(*aux_args.vis) * (1.0f - diff_omega);
  (*aux_args.vis) *= diff_omega;
  (*aux_args.exp_vis_score) += vis_score*omega;
}

#endif
