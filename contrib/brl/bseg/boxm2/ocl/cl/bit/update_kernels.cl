//THIS IS UPDATE BIT SCENE OPT
//Created Sept 30, 2010,
//Implements the parallel work group segmentation algorithm.
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif
#ifdef MOG_TYPE_16 
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs=convert_ushort8_sat_rte(data);
    #define MOG_TYPE ushort8
    #define NORM 65535;
#endif
#ifdef MOG_TYPE_8 
   #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs=convert_uchar8_sat_rte(data);
   #define MOG_TYPE uchar8
   #define NORM 255;
#endif



#ifdef SEGLEN
typedef struct
{
  __global int* seg_len;
  __global int* mean_obs; 

  __local  short2* ray_bundle_array; 
  __local  int*    cell_ptrs; 
  __local  float4* cached_aux; 
           float   obs; 
} AuxArgs;  

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*, 
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs); 
__kernel
void
seg_len_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    ushort4            * num_obs_array,    // num obs for each block
             __global    int                * aux_array,        // aux data array (four aux arrays strung together)
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float16            * camera,           // camera orign and SVD of inverse of camera matrix
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    float4             * in_image,         // the input image
             __global    float              * output,
             __local     uchar16            * local_tree,       // cache current tree into local memory
             __local     short2             * ray_bundle_array, // gives information for which ray takes over in the workgroup
             __local     int                * cell_ptrs,        // local list of cell_ptrs (cells that are hit by this workgroup
             __local     float4             * cached_aux_data,  // seg len cached aux data is only a float2
             __local     uchar              * cumsum )          // cumulative sum for calculating data pointer
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
  
  //grab input image value (also holds vis)
  float4 inImage = in_image[j*get_global_size(0) + i];
  float obs = inImage.x;
  float vis = inImage.z;
  barrier(CLK_LOCAL_MEM_FENCE);

  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y) 
    return;

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);  

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  AuxArgs aux_args; 
  aux_args.seg_len  = aux_array;
  aux_args.mean_obs = &aux_array[linfo->num_buffer * linfo->data_len]; 
  aux_args.ray_bundle_array = ray_bundle_array;
  aux_args.cell_ptrs  = cell_ptrs;
  aux_args.cached_aux = cached_aux_data; 
  aux_args.obs = obs; 
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args);    //utility info
}
#endif

#ifdef PREINF
typedef struct
{
  __global float* alpha; 
  __global MOG_TYPE * mog; 
  __global int* seg_len;
  __global int* mean_obs; 
           float4* inImage; 
} AuxArgs;  

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*, 
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs); 

__kernel
void
pre_inf_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    MOG_TYPE           * mixture_array,    // mixture for each block
             __global    ushort4            * num_obs_array,    // num obs for each block
             __global    int                * aux_array,        // four aux arrays strung together
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float16            * camera,           // camera orign and SVD of inverse of camera matrix
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    float4             * in_image,         // the input image
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
  float4 inImage = in_image[j*get_global_size(0) + i];
  float vis = 1.0f; 
  barrier(CLK_LOCAL_MEM_FENCE);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);  

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  AuxArgs aux_args; 
  aux_args.alpha   = alpha_array; 
  aux_args.mog     = mixture_array; 
  aux_args.seg_len   = aux_array;
  aux_args.mean_obs  = &aux_array[linfo->num_buffer * linfo->data_len]; 
  aux_args.inImage = &inImage; 
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args);    //utility info

  //store the vis_inf/pre_inf in the image          
  in_image[j*get_global_size(0)+i] = inImage;
}
#endif

#ifdef BAYES
typedef struct
{
  __global float*   alpha; 
  __global MOG_TYPE * mog; 
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
} AuxArgs;  

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*, 
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs); 

__kernel
void
bayes_main(__constant  RenderSceneInfo    * linfo,
           __global    int4               * tree_array,       // tree structure for each block
           __global    float              * alpha_array,      // alpha for each block
           __global    MOG_TYPE           * mixture_array,    // mixture for each block
           __global    ushort4            * num_obs_array,    // num obs for each block
           __global    int                * aux_array,        // four aux arrays strung together
           __constant  uchar              * bit_lookup,       // used to get data_index
           __global    float16            * camera,           // camera orign and SVD of inverse of camera matrix
           __global    uint4              * imgdims,          // dimensions of the input image
           __global    float4             * in_image,         // the input image
           __global    float              * output,
           __local     uchar16            * local_tree,       // cache current tree into local memory
           __local     short2             * ray_bundle_array, // gives information for which ray takes over in the workgroup
           __local     int                * cell_ptrs,        // local list of cell_ptrs (cells that are hit by this workgroup
           __local     float              * cached_vis,       // cached vis used to sum up vis contribution locally
           __local     uchar              * cumsum)          // cumulative sum for calculating data pointer
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
  float4 inImage = in_image[j*get_global_size(0) + i];
  float norm = inImage.x;
  float vis  = inImage.z;
  float pre  = inImage.w;
  barrier(CLK_LOCAL_MEM_FENCE);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);  
  
  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  AuxArgs aux_args; 
  aux_args.alpha   = alpha_array; 
  aux_args.mog     = mixture_array; 
  aux_args.seg_len = aux_array;
  aux_args.mean_obs   = &aux_array[linfo->num_buffer * linfo->data_len]; 
  aux_args.vis_array  = &aux_array[2 * linfo->num_buffer * linfo->data_len];
  aux_args.beta_array = &aux_array[3 * linfo->num_buffer * linfo->data_len];
  
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
            local_tree, bit_lookup, cumsum, &vis0, aux_args);    //utility info
            
  //write out vis and pre
  in_image[j*get_global_size(0)+i].zw = (float2) (vis, pre); 
}
#endif


// normalize the pre_inf image...
//
__kernel
void
proc_norm_image(__global float4* image, __global float4* p_inf, __global uint4 * imgdims)
{
  // linear global id of the normalization image

  int i=0;
  int j=0;
  i=get_global_id(0);
  j=get_global_id(1);

  if (i>=(*imgdims).z && j>=(*imgdims).w)
    return;

  float4 vect = image[j*get_global_size(0)+i];
  float mult =
#if 0
               (p_inf[0].x>0.0f) ? 1.0f :
               gauss_prob_density(vect.x, p_inf[0].y, p_inf[0].z);
#else
               1.0f;
#endif
  // compute the norm image
  vect.x = vect.w + mult * vect.z;
  // the following  quantities have to be re-initialized before
  // the bayes_ratio kernel is executed
  vect.y = 0.0f; // clear alpha integral
  vect.z = 1.0f; // initial vis = 1.0
  vect.w = 0.0f; // initial pre = 0.0
  // write it back
  image[j*get_global_size(0)+i] = vect;
}


// Update each cell using its aux data
//
__kernel
void
update_bit_scene_main(__global RenderSceneInfo  * info,
                      __global float            * alpha_array,
                      __global MOG_TYPE         * mixture_array,
                      __global ushort4          * nobs_array,
                      __global int              * aux_array,
                      __global float            * output)
{
  int gid=get_global_id(0);
  int datasize = info->data_len * info->num_buffer;
  if (gid<datasize)
  {
    //if alpha is less than zero don't update
    float  alpha    = alpha_array[gid];
    float  cell_min = info->block_len/(float)(1<<info->root_level);

    //get cell cumulative length and make sure it isn't 0
    int len_int = aux_array[gid]; 
    float cum_len  = convert_float(len_int)/SEGLEN_FACTOR; 

    //minimum alpha value, don't let blocks get below this
    float  alphamin = -log(1.0-0.0001)/cell_min;
    
    //update cell if alpha and cum_len are greater than 0
    if (alpha > 0.0f && cum_len > 1e-10f)
    {
      int obs_int = aux_array[datasize + gid]; 
      int vis_int = aux_array[2*datasize + gid]; 
      int beta_int= aux_array[3*datasize + gid];
      float mean_obs = convert_float(obs_int)/SEGLEN_FACTOR;
      mean_obs = mean_obs / cum_len;  
      float cell_vis  = convert_float(vis_int)/SEGLEN_FACTOR;
      float cell_beta = convert_float(beta_int)/SEGLEN_FACTOR;
      float4 aux_data = (float4) (cum_len, mean_obs, cell_beta, cell_vis/cum_len);
      float4 nobs     = convert_float4(nobs_array[gid]);
      float8 mixture  = convert_float8(mixture_array[gid])/NORM;
      float16 data = (float16) (alpha,
                                 (mixture.s0), (mixture.s1), (mixture.s2), (nobs.s0),
                                 (mixture.s3), (mixture.s4), (mixture.s5), (nobs.s1),
                                 (mixture.s6), (mixture.s7), (nobs.s2), (nobs.s3/100.0),
                                 0.0, 0.0, 0.0);

      //use aux data to update cells
      //update_cell(&data, aux_data, 2.5f, 0.06f, 0.02f);
      update_cell(&data, aux_data, 2.5f, 0.02f, 0.033f);
      //update_cell(&data, aux_data, 2.5f, 0.01f, 0.00375f);

      //reset the cells in memory
      alpha_array[gid]      = max(alphamin,data.s0);
      float8 post_mix       = (float8) (data.s1, data.s2, data.s3,
                                        data.s5, data.s6, data.s7,
                                        data.s9, data.sa)*(float) NORM;
      float4 post_nobs      = (float4) (data.s4, data.s8, data.sb, data.sc*100.0);
      CONVERT_FUNC_SAT_RTE(mixture_array[gid],post_mix)
      nobs_array[gid]       = convert_ushort4_sat_rte(post_nobs);

    }
    
    //clear out aux data
    aux_array[gid] = 0; 
    aux_array[gid + datasize] = 0; 
    aux_array[gid + 2*datasize] = 0;
    aux_array[gid + 3*datasize] = 0;
  }
  
}
