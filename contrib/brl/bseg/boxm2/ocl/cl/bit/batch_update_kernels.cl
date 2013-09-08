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

#ifdef AUX_PREVIS
typedef struct
{
  __global float*   alpha;
  __global MOG_TYPE * mog;
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
aux_previs_main(__constant  RenderSceneInfo    * linfo,
                __global    int4               * tree_array,        // tree structure for each block
                __global    float              * alpha_array,       // alpha for each block
                __global    MOG_TYPE           * mixture_array,     // mixture for each block
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
  aux_args.mog        = mixture_array;
  aux_args.seg_len    = aux_array0;
  aux_args.mean_obs   = aux_array1;
  aux_args.vis_array  = aux_array2;
  aux_args.pre_array  = aux_array3;

  aux_args.ray_bundle_array = ray_bundle_array;
  aux_args.cell_ptrs = cell_ptrs;
  aux_args.cached_vis = cached_vis;
  aux_args.ray_vis = &vis;
  aux_args.ray_pre = &pre;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis0, aux_args,0, MAXFLOAT);    //utility info

  //write out vis and pre
  vis_image[j*get_global_size(0)+i] = vis;
  pre_image[j*get_global_size(0)+i] = pre;
}
#endif //

#ifdef AUX_LEN_INT_VIS
typedef struct
{
  __global float*   alpha;
  __global int* seg_len;
  __global int* mean_obs;
  __global int* vis_array;
  __global int* pre_array;

  __local  short2* ray_bundle_array;
  __local  int*    cell_ptrs;
  __local  float*  cached_vis;
           float*  ray_vis;
           float*  ray_pre;
           float   obs;
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);

__kernel
void
aux_len_int_vis_main(__constant  RenderSceneInfo    * linfo,
                     __global    int4               * tree_array,        // tree structure for each block
                     __global    float              * alpha_array,       // alpha for each block
                     __global    int                * aux_array0,        // four aux arrays strung together
                     __global    int                * aux_array1,        // four aux arrays strung together
                     __global    int                * aux_array2,        // four aux arrays strung together
                     __global    int                * aux_array3,        // four aux arrays strung together
                     __constant  uchar              * bit_lookup,        // used to get data_index
                     __global    float4             * ray_origins,
                     __global    float4             * ray_directions,
                     __global    uint4              * imgdims,           // dimensions of the input image
                     __global    float              * vis_image,         // visibility image (for keeping vis across blocks)
                     __global    float              * in_image,          // preinf image (for keeping pre across blocks)
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
  aux_args.seg_len    = aux_array0;
  aux_args.mean_obs   = aux_array1;
  aux_args.vis_array  = aux_array2;
  aux_args.pre_array  = aux_array3;

  aux_args.ray_bundle_array = ray_bundle_array;
  aux_args.cell_ptrs = cell_ptrs;
  aux_args.cached_vis = cached_vis;
  aux_args.ray_vis = &vis;
  aux_args.obs     = in_image[j*get_global_size(0) + i];

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis0, aux_args,0,MAXFLOAT);    //utility info

  //write out vis and pre
  vis_image[j*get_global_size(0)+i] = vis;
}
#endif //

#ifdef UPDATE_AUX_DIRECTION
typedef struct
{
  __global int* len;
  __global int* X;
  __global int* Y;
  __global int* Z;

  float xdir;
  float ydir;
  float zdir;

  __local  short2* ray_bundle_array;
  __local  int*    cell_ptrs;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);

__kernel
void
aux_directions_main(__constant  RenderSceneInfo    * linfo,
                    __global    int4               * tree_array,        // tree structure for each block
                    __global    int                * aux_array0,        // four aux arrays strung together
                    __global    int                * aux_array1,        // four aux arrays strung together
                    __global    int                * aux_array2,        // four aux arrays strung together
                    __global    int                * aux_array3,        // four aux arrays strung together
                    __constant  uchar              * bit_lookup,        // used to get data_index
                    __global    float4             * ray_origins,
                    __global    float4             * ray_directions,
                    __global    uint4              * imgdims,           // dimensions of the input image
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
  aux_args.len    = aux_array0;
  aux_args.X      = aux_array1;
  aux_args.Y      = aux_array2;
  aux_args.Z      = aux_array3;

  aux_args.xdir = ray_dx;
  aux_args.ydir = ray_dy;
  aux_args.zdir = ray_dz;

  float vis =1.0;
  aux_args.ray_bundle_array = ray_bundle_array;
  aux_args.cell_ptrs = cell_ptrs;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);    //utility info
}
#endif


#ifdef AUX_PREVISPOST
typedef struct
{
  __global float*   alpha;
  __global MOG_TYPE * mog;
  __global int* seg_len;
  __global int* mean_obs;
  __global int* vis_array;
  __global int* pre_array;
  __global int* post_array;
  __constant RenderSceneInfo * linfo;
           float*  ray_vis;
           float*  ray_pre;
           float*  vis_inf;
           float*  pre_inf;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);

__kernel
void
aux_previspost_main(__constant  RenderSceneInfo    * linfo,
                    __global    int4               * tree_array,        // tree structure for each block
                    __global    float              * alpha_array,       // alpha for each block
                    __global    MOG_TYPE           * mixture_array,     // mixture for each block
                    __global    int                * aux_array0,        // seglen
                    __global    int                * aux_array1,        // meanobs
                    __global    int                * aux_array2,        // pre array
                    __global    int                * aux_array3,        // vis array
                    __global    int                * aux_array4,        // post array
                    __constant  uchar              * bit_lookup,        // used to get data_index
                    __global    float4             * ray_origins,
                    __global    float4             * ray_directions,
                    __global    uint4              * imgdims,           // dimensions of the input image
                    __global    float              * vis_image,         // visibility image (for keeping vis across blocks)
                    __global    float              * pre_image,         // preinf image (for keeping pre across blocks)
                    __global    float              * vis_inf_image,     // vis_inf image
                    __global    float              * pre_inf_image,     // pre_inf image
                    __global    float              * output,
                    __local     uchar16            * local_tree,        // cache current tree into local memory
                    __local     uchar              * cumsum)            // cumulative sum for calculating data pointer
{
  // get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  // initialize pre-broken ray information (non broken rays will be re initialized)
  //ray_bundle_array[llid] = (short2) (-1, 0);
  //cell_ptrs[llid] = -1;

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

  float vis_inf = vis_inf_image[j*get_global_size(0) + i];
  float pre_inf = pre_inf_image[j*get_global_size(0) + i];

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
  aux_args.alpha      = alpha_array;
  aux_args.mog        = mixture_array;
  aux_args.seg_len    = aux_array0;
  aux_args.mean_obs   = aux_array1;

  aux_args.pre_array  = aux_array2;
  aux_args.vis_array  = aux_array3;
  aux_args.post_array  = aux_array4;

  //aux_args.ray_bundle_array = ray_bundle_array;
  //aux_args.cell_ptrs = cell_ptrs;
  //aux_args.cached_vis = cached_vis;
  aux_args.ray_vis = &vis;
  aux_args.ray_pre = &pre;
  aux_args.vis_inf = &vis_inf;
  aux_args.pre_inf = &pre_inf;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                   //scene info
            local_tree, bit_lookup, cumsum, &vis0, aux_args,0,MAXFLOAT);    //utility info

  //write out vis and pre
  vis_image[j*get_global_size(0)+i] = vis;
  pre_image[j*get_global_size(0)+i] = pre;
}
#endif //AUX_PREVISPOST


#ifdef CONVERT_AUX
__kernel void
convert_aux_int_to_float(__constant  RenderSceneInfo    * linfo,
                         __global float* aux_array0,
                         __global float* aux_array1,
                         __global float* aux_array2,
                         __global float* aux_array3)
{
  int gid=get_global_id(0);
  int datasize = linfo->data_len ;
  if (gid<datasize)
  {
    int obs0= as_int(aux_array0[gid]);
    int obs1= as_int(aux_array1[gid]);
    int obs2= as_int(aux_array2[gid]);
    int obs3= as_int(aux_array3[gid]);

    aux_array0[gid]=((float)obs0);
    aux_array1[gid]=((float)obs1);
    aux_array2[gid]=((float)obs2);
    aux_array3[gid]=((float)obs3);
  }
}
#endif //CONVERT_AUX

#ifdef CONVERT_AUX_NORMALIZE
__kernel void
convert_aux_and_normalize(__constant  RenderSceneInfo    * linfo,
                          __global float* aux_array0,
                          __global float* aux_array1,
                          __global float* aux_array2, //vis
                          __global float* aux_array3, //pre
                          __global float* aux_array4) //post
{
  int gid=get_global_id(0);
  int datasize = linfo->data_len ;//* info->num_buffer;
  if (gid<datasize)
  {
    int obs0= as_int(aux_array0[gid]);
    int obs1= as_int(aux_array1[gid]);
    int obs2= as_int(aux_array2[gid]);
    int obs3= as_int(aux_array3[gid]);
    int obs4= as_int(aux_array4[gid]);

    aux_array0[gid]= (((float)obs0)/SEGLEN_FACTOR) * linfo->block_len;
    aux_array1[gid]= (((float)obs1)/SEGLEN_FACTOR) * linfo->block_len;
    aux_array2[gid]= (((float)obs2)/SEGLEN_FACTOR) * linfo->block_len;
    aux_array3[gid]= (((float)obs3)/SEGLEN_FACTOR) * linfo->block_len;
    aux_array4[gid]= (((float)obs4)/SEGLEN_FACTOR) * linfo->block_len;
  }
}
#endif //CONVERT_AUX_NORMALIZE

#ifdef CONVERT_NOBS_INT_SHORT
__kernel void
convert_nobs_int_short(__constant  RenderSceneInfo    * linfo,
                       __global unsigned int* num_obs,
                       __global unsigned short* num_obsShort)
{
   int gid=get_global_id(0);
   int datasize = linfo->data_len ;//* info->num_buffer;
   if (gid<datasize) {
        num_obsShort[gid]=(unsigned short)(num_obs[gid]);
   }
}
#endif //CONVERT_NOBS_INT_SHORT
#ifdef CONVERT_AUX_XYZ_THETAPHI
__kernel void
convert_aux_xyz_to_thetaphi(__constant  RenderSceneInfo * linfo,
                            __global float* aux_array0,
                            __global float* aux_array1,
                            __global float* aux_array2,
                            __global float* aux_array3)
{
  int gid=get_global_id(0);
  int datasize = linfo->data_len ;//* info->num_buffer;
  if (gid<datasize)
  {
    float obs0= (float) as_int(aux_array0[gid]);
    float obs1= (float) as_int(aux_array1[gid]);
    float obs2= (float) as_int(aux_array2[gid]);
    float obs3= (float) as_int(aux_array3[gid]);

    float phi   = atan2(obs2,obs1);
    float denom = sqrt(obs1*obs1+obs2*obs2+obs3*obs3);
    float theta = acos(obs3/denom);

    aux_array0[gid]=theta;
    aux_array1[gid]=phi;
  }
}
#endif //CONVERT_AUX

#ifdef SEGLENNOBS
typedef struct
{
  __global int* seg_len;
  __global int* mean_obs;
  __global uint* nobs;
  __local  short2* ray_bundle_array;
  __local  int*    cell_ptrs;
  __local  float4* cached_aux;
           float   obs;
  __global float * output;
  __constant RenderSceneInfo * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);


__kernel void
seg_len_nobs_main(__constant  RenderSceneInfo    * linfo,
                  __global    int4               * tree_array,       // tree structure for each block
                  __global    float              * alpha_array,      // alpha for each block
                  __global    int                * aux_array0,       // aux data array (four aux arrays strung together)
                  __global    int                * aux_array1,       // aux data array (four aux arrays strung together)
                  __global    uint               * nobs,             // data array to keep track of the number of observations per cell
                  __constant  uchar              * bit_lookup,       // used to get data_index
                  __global    float4             * ray_origins,
                  __global    float4             * ray_directions,
                  __global    uint4              * imgdims,          // dimensions of the input image
                  __global    float              * in_image,         // the input image
                  __global    float              * output,
                  __local     uchar16            * local_tree,       // cache current tree into local memory
                  __local     short2             * ray_bundle_array, // gives information for which ray takes over in the workgroup
                  __local     int                * cell_ptrs,        // local list of cell_ptrs (cells that are hit by this workgroup
                  __local     float4             * cached_aux_data,  // seg len cached aux data is only a float2
                  __local     uchar              * cumsum )          // cumulative sum for calculating data pointer
{
  // get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  // initialize pre-broken ray information (non broken rays will be re initialized)
  ray_bundle_array[llid] = (short2) (-1, 0);
  cell_ptrs[llid] = -1;

  // ----------------------------------------------------------------------------
  // get image coordinates and camera,
  // check for validity before proceeding
  // ----------------------------------------------------------------------------
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  int imIndex = j*get_global_size(0) + i;

  // grab input image value (also holds vis)
  float obs = in_image[imIndex];

  float vis = 1.0f;  //no visibility in this pass
  barrier(CLK_LOCAL_MEM_FENCE);

  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y || obs < 0.0f)
    return;

  // ----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  // ----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ imIndex ];
  float4 ray_d = ray_directions[ imIndex ];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  // ----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  // ----------------------------------------------------------------------------

  AuxArgs aux_args;
  aux_args.linfo    = linfo;
  aux_args.seg_len  = aux_array0;
  aux_args.mean_obs = aux_array1;
  aux_args.nobs = nobs;
  aux_args.ray_bundle_array = ray_bundle_array;
  aux_args.cell_ptrs  = cell_ptrs;
  aux_args.cached_aux = cached_aux_data;
  aux_args.obs = obs;
  aux_args.output = output;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0, MAXFLOAT);    //utility info
}

#endif //SEGLENNOBS
