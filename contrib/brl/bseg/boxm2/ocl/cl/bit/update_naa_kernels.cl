//THIS IS update_naa_kernels.cl
//Created March 3 2012
//Implements kernels for updating with normal-albedo-array (naa) appearance model

#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef AUX_PREVIS_NAA
typedef struct
{
  __global float*   alpha;
  __global float16 *radiance_reflectance_factors;
  __global float16 *radiance_offsets;
  __global float16 *radiance_var_reflectance_sqrd_factors;
  __global float16 *radiance_var_offsets;
  __global float *naa_apm; // 32 floats per cell
  __global int* seg_len;
  __global int* mean_obs;
  __global int* vis_array;
  __global int* pre_array;
  __local  short2* ray_bundle_array;
  __local  int*    cell_ptrs;
  __local  float*  cached_vis;
           float*  ray_vis;
           float*  ray_pre;
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);

__kernel
void
aux_previs_main_naa(__constant  RenderSceneInfo    * linfo,
                    __global    int4               * tree_array,        // tree structure for each block
                    __global    float              * alpha_array,       // alpha for each block
                    __global    float16            * radiance_reflectance_factors, // scales for computing radiance from albedo
                    __global    float16            * radiance_offsets,        // offset value for computing radiance from albedo
                    __global    float16            * radiance_var_reflectance_sqrd_factors, // scales for computing radiance  variance from albedo^2
                    __global    float16            * radiance_var_offsets,    // offset values for computing radiance variance from albedo
                    __global    float              * naa_apm_array,       // albedo (per normal) and weights for each block (32 floats per cell)
                    __global    int                * aux_array0,        // four aux arrays strung together
                    __global    int                * aux_array1,        // four aux arrays strung together
                    __global    int                * aux_array2,        // four aux arrays strung together
                    __global    int                * aux_array3,        // four aux arrays strung together
                    __constant  uchar              * bit_lookup,        // used to get data_index
                    __global    float4             * ray_origins,
                    __global    float4             * ray_directions,
                    __global    uint4              * imgdims,           // dimensions of the input image
                    __global    float              * vis_image,         // visibility image (for keeping vis across blocks)
                    __global    float              * pre_image,         // preinf image (for keeping pre across blocks)
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
  float vis0 = 1.0f;
  float vis = vis_image[j*get_global_size(0) + i];
  float pre = pre_image[j*get_global_size(0) + i];

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
  aux_args.linfo    = linfo;
  aux_args.alpha      = alpha_array;
  aux_args.naa_apm    = naa_apm_array;
  aux_args.seg_len    = aux_array0;
  aux_args.mean_obs   = aux_array1;
  aux_args.vis_array  = aux_array2;
  aux_args.pre_array  = aux_array3;

  aux_args.radiance_reflectance_factors = radiance_reflectance_factors;
  aux_args.radiance_offsets = radiance_offsets;
  aux_args.radiance_var_reflectance_sqrd_factors = radiance_var_reflectance_sqrd_factors;
  aux_args.radiance_var_offsets = radiance_var_offsets;

  aux_args.ray_bundle_array = ray_bundle_array;
  aux_args.cell_ptrs = cell_ptrs;
  aux_args.cached_vis = cached_vis;
  aux_args.ray_vis = &vis;
  aux_args.ray_pre = &pre;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis0, aux_args,0,MAXFLOAT);    //utility info

  //write out vis and pre
  vis_image[j*get_global_size(0)+i] = vis;
  pre_image[j*get_global_size(0)+i] = pre;
}
#endif // AUX_PREVIS_NAA

#ifdef PREINF_NAA
typedef struct
{
  __global float* alpha;
  __global float16 *radiance_reflectance_factors;
  __global float16 *radiance_offsets;
  __global float16 *radiance_var_reflectance_sqrd_factors;
  __global float16 *radiance_var_offsets;
  __global float *naa_apm; // 32 floats per cell
  __global int* seg_len;
  __global int* mean_obs;
           float* vis_inf;
           float* pre_inf;
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);

__kernel
void
pre_inf_naa_main(__constant  RenderSceneInfo    * linfo,
                 __global    int4               * tree_array,       // tree structure for each block
                 __global    float              * alpha_array,      // alpha for each block
                 __global    float16            * radiance_reflectance_factors, // scales for computing radiance from albedo
                 __global    float16            * radiance_offsets,        // offset value for computing radiance from albedo
                 __global    float16            * radiance_var_reflectance_sqrd_factors, // scales for computing radiance  variance from albedo^2
                 __global    float16            * radiance_var_offsets,    // offset values for computing radiance variance from albedo
                 __global    float              * naa_apm_array,       // albedo (per normal) and weights for each block (32 floats per cell)
                 __global    int                * aux_array0,        // four aux arrays strung together
                 __global    int                * aux_array1,        // four aux arrays strung together
                 __constant  uchar              * bit_lookup,       // used to get data_index
                 __global    float4             * ray_origins,
                 __global    float4             * ray_directions,
                 __global    uint4              * imgdims,          // dimensions of the input image
                 __global    float              * vis_image,        // visibility image
                 __global    float              * pre_image,        // preinf image
                 __global    float              * output,
                 __local     uchar16            * local_tree,       // cache current tree into local memory
                 __local     uchar              * cumsum )           // cumulative sum for calculating data pointer
{
  //get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

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
  //float4 inImage = in_image[j*get_global_size(0) + i];
  float vis_inf = vis_image[j*get_global_size(0) + i];
  float pre_inf = pre_image[j*get_global_size(0) + i];

  if (vis_inf <0.0)
    return;
  //vis for cast_ray, never gets decremented so no cutoff occurs
  float vis = 1.0f;
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
  aux_args.linfo   = linfo;
  aux_args.alpha   = alpha_array;
  aux_args.radiance_reflectance_factors = radiance_reflectance_factors;
  aux_args.radiance_offsets = radiance_offsets;
  aux_args.radiance_var_reflectance_sqrd_factors = radiance_var_reflectance_sqrd_factors;
  aux_args.radiance_var_offsets = radiance_var_offsets;
  aux_args.naa_apm = naa_apm_array;
  aux_args.seg_len   = aux_array0;
  aux_args.mean_obs  = aux_array1;
  aux_args.vis_inf = &vis_inf;
  aux_args.pre_inf = &pre_inf;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);    //utility info

  //store the vis_inf/pre_inf in the image
  vis_image[j*get_global_size(0)+i] = vis_inf;
  pre_image[j*get_global_size(0)+i] = pre_inf;
}
#endif // PREINF_NAA

#ifdef PROC_NORM_NAA
// normalize the pre_inf image...
//
__kernel
void
proc_norm_image (  __global float* norm_image,
                   __global float* vis_image,
                   __global float* pre_image,
                   __global uint4* imgdims,
                   __global float* background_density)
{
  // linear global id of the normalization image
  int i=0;
  int j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  float vis = vis_image[j*get_global_size(0) + i];

  if (i>=(*imgdims).z && j>=(*imgdims).w && vis<0.0f)
    return;

  float pre = pre_image[j*get_global_size(0) + i];
  // "background" distribution is no longer uniform (0,1)
  float norm = pre + vis * (*background_density);
  norm_image[j*get_global_size(0) + i] = norm;

  // the following  quantities have to be re-initialized before
  // the bayes_ratio kernel is executed
  vis_image[j*get_global_size(0) + i] = 1.0f; // initial vis = 1.0f
  pre_image[j*get_global_size(0) + i] = 0.0f; // initial pre = 0.0
}
#endif // PROC_NORM_NAA

#ifdef BAYES_NAA
typedef struct
{
  __global float*   alpha;
  __global float16 *radiance_reflectance_factors;
  __global float16 *radiance_offsets;
  __global float16 *radiance_var_reflectance_sqrd_factors;
  __global float16 *radiance_var_offsets;
  __global float * naa_apm;
  __global int* seg_len;
  __global int* mean_obs;
  __global int* vis_array;
  __global int* beta_array;

  __local  short2* ray_bundle_array;
  __local  int*    cell_ptrs;
  __local  float*  cached_vis;
           float   norm;
           float*  ray_vis;
           float*  ray_pre;
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);

__kernel
void
bayes_main_naa(__constant  RenderSceneInfo    * linfo,
               __global    int4               * tree_array,        // tree structure for each block
               __global    float              * alpha_array,       // alpha for each block
               __global    float16            * radiance_reflectance_factors, // scales for computing radiance from albedo
               __global    float16            * radiance_offsets,        // offset value for computing radiance from albedo
               __global    float16            * radiance_var_reflectance_sqrd_factors, // scales for computing radiance  variance from albedo^2
               __global    float16            * radiance_var_offsets,    // offset values for computing radiance variance from albedo
               __global    float              * naa_apm_array,
               __global    int                * aux_array0,        // four aux arrays strung together
               __global    int                * aux_array1,        // four aux arrays strung together
               __global    int                * aux_array2,        // four aux arrays strung together
               __global    int                * aux_array3,        // four aux arrays strung together
               __constant  uchar              * bit_lookup,        // used to get data_index
               __global    float4             * ray_origins,
               __global    float4             * ray_directions,
               __global    uint4              * imgdims,           // dimensions of the input image
               __global    float              * vis_image,         // visibility image (for keeping vis across blocks)
               __global    float              * pre_image,         // preinf image (for keeping pre across blocks)
               __global    float              * norm_image,        // norm image (for bayes update normalization factor)
               __global    float              * output,
               __local     uchar16            * local_tree,        // cache current tree into local memory
               __local     short2             * ray_bundle_array,  // gives information for which ray takes over in the workgroup
               __local     int                * cell_ptrs,         // local list of cell_ptrs (cells that are hit by this workgroup
               __local     float              * cached_vis,        // cached vis used to sum up vis contribution locally
               __local     uchar              * cumsum)            // cumulative sum for calculating data pointer
{
  //get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  //initialize pre-broken ray information (non broken rays will be re initialized)
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
  float vis0 = 1.0f;
  float norm = norm_image[j*get_global_size(0) + i];
  float vis = vis_image[j*get_global_size(0) + i];
  float pre = pre_image[j*get_global_size(0) + i];
  if (vis <0.0)
    return;

  barrier(CLK_LOCAL_MEM_FENCE);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ j*get_global_size(0) + i ];
  float4 ray_d = ray_directions[ j*get_global_size(0) + i ];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  //calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  AuxArgs aux_args;
  aux_args.linfo      = linfo;
  aux_args.alpha      = alpha_array;
  aux_args.radiance_reflectance_factors = radiance_reflectance_factors;
  aux_args.radiance_offsets = radiance_offsets;
  aux_args.radiance_var_reflectance_sqrd_factors = radiance_var_reflectance_sqrd_factors;
  aux_args.radiance_var_offsets = radiance_var_offsets;
  aux_args.naa_apm    = naa_apm_array;
  aux_args.seg_len    = aux_array0;
  aux_args.mean_obs   = aux_array1;
  aux_args.vis_array  = aux_array2;
  aux_args.beta_array = aux_array3;

  aux_args.ray_bundle_array = ray_bundle_array;
  aux_args.cell_ptrs = cell_ptrs;
  aux_args.cached_vis = cached_vis;
  aux_args.norm = norm;
  aux_args.ray_vis = &vis;
  aux_args.ray_pre = &pre;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis0, aux_args,0,MAXFLOAT);    //utility info

  //write out vis and pre
  vis_image[j*get_global_size(0)+i] = vis;
  pre_image[j*get_global_size(0)+i] = pre;
}
#endif // BAYES_NAA

