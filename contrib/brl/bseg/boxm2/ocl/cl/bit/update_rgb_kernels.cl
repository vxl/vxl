//THIS IS UPDATE BIT SCENE OPT
//Created Sept 30, 2010,
//Implements the parallel work group segmentation algorithm.
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef SEGLEN
typedef struct
{
  __global int* seg_len;
  __global int* mean_obsR;
  __global int* mean_obsG;
  __global int* mean_obsB;

  __local  short2* ray_bundle_array;
  __local  int*    cell_ptrs;
  __local  float4* cached_aux;
           float4  obs;
  __global float * output;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);
__kernel
void
seg_len_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    int                * aux_seg_len,        // seg len aux array
             __global    int                * aux_mean_obsY,      // mean obs r aux array
             __global    int                * aux_mean_obsU,      // mean obs r aux array
             __global    int                * aux_mean_obsV,      // mean obs r aux array
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float4             * ray_origins,
             __global    float4             * ray_directions,
             __global    float              * nearfarplanes,
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
  int imIndex = j*get_global_size(0) + i;

  //grab input image value (also holds vis)
  float4 obs = in_image[imIndex];
  float vis = 1.0f;  //no visibility in this pass
  barrier(CLK_LOCAL_MEM_FENCE);

  if (obs.x <0.0)
      return;
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y )
    return;

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ imIndex ];
  float4 ray_d = ray_directions[ imIndex ];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  AuxArgs aux_args;
  aux_args.seg_len  = aux_seg_len;
  aux_args.mean_obsR = aux_mean_obsY; //&aux_array[linfo->num_buffer * linfo->data_len];
  aux_args.mean_obsG = aux_mean_obsU; //&aux_array[2 * linfo->num_buffer * linfo->data_len];
  aux_args.mean_obsB = aux_mean_obsV; //&aux_array[3 * linfo->num_buffer * linfo->data_len];

  aux_args.ray_bundle_array = ray_bundle_array;
  aux_args.cell_ptrs  = cell_ptrs;
  aux_args.cached_aux = cached_aux_data;
  aux_args.obs = obs;
  float nearplane = nearfarplanes[0]/linfo->block_len;
  float farplane = nearfarplanes[1]/linfo->block_len;

  //-----YUV edit ----///
#ifdef YUV
  aux_args.obs = rgb2yuv(obs);
#endif
  aux_args.output = output;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,nearplane,farplane);    //utility info
}
#endif

//second pass compresses the mean obs rgb value into the mean obs aux data
#ifdef COMPRESS_RGB
__kernel
void
compress_rgb(__global RenderSceneInfo * info,
             __global int             * aux_seg_len,        // seg len aux array
             __global int             * aux_mean_obsY,      // mean obs r aux array
             __global int             * aux_mean_obsU,      // mean obs r aux array
             __global int             * aux_mean_obsV)      // mean obs r aux array)
{
  int gid = get_global_id(0);
  int datasize = info->data_len;
  if (gid<datasize)
  {
    //get the segment length
    int len_int = aux_seg_len[gid]; //this is cum_len * SEG_LEN

    //get cumulative observation values for r g and b
    int r_int = aux_mean_obsY[gid]; //aux_array[datasize + gid];   //Y int
    int g_int = aux_mean_obsU[gid]; //aux_array[2*datasize + gid]; //U int
    int b_int = aux_mean_obsV[gid]; // aux_array[3*datasize + gid]; //V int

    //normalize mean obs by CUM_LEN and SEGLEN_FACTOR
    int4 rgbs = (int4) (r_int, g_int, b_int, 0);
    float4 mean_obs =  convert_float4(rgbs) / (convert_float(len_int));

    //------------- YUV EDIT ---------------
    //now mean_obs should have Y, U, V where U in [-.436, .436] and V in [-.615, .615]
    //put them in a 0-1 range
#ifdef YUV
    mean_obs.y = (mean_obs.y + U_MAX)/U_RANGE;
    mean_obs.z = (mean_obs.z + V_MAX)/V_RANGE;
#endif
    //-----------------------------------

    //pack them in a single integer, and store in global memory
    uchar4 packed = convert_uchar4_sat_rte(mean_obs*255.0f);
    aux_mean_obsY[gid] = as_int(packed); //aux_array[datasize + gid] = as_int(packed);

    //zero out the rest of the aux array
    aux_mean_obsU[gid] = 0; //aux_array[2*datasize + gid] = 0;
    aux_mean_obsV[gid] = 0; // aux_array[3*datasize + gid] = 0;
  }
}
#endif

#ifdef PREINF
typedef struct
{
  __global float* alpha;
  __global MOG_TYPE * mog;
  __global int* seg_len;
  __global int* mean_obs;
           float* vis_inf;
           float* pre_inf;
  __constant  RenderSceneInfo    * linfo;

} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);

__kernel
void
pre_inf_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    MOG_TYPE           * mixture_array,    // mixture for each block
             __global    ushort4            * num_obs_array,    // num obs for each block
             __global    int                * aux_seg_len,       // seg len aux array
             __global    int                * aux_mean_obs,      // mean obs aux array
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float4             * ray_origins,
             __global    float4             * ray_directions,
             __global    float              * nearfarplanes,
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

  if (vis_inf<0.0)
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
  aux_args.mog     = mixture_array;
  aux_args.seg_len   = aux_seg_len;
  aux_args.mean_obs  = aux_mean_obs; //&aux_array[linfo->num_buffer * linfo->data_len];
  aux_args.vis_inf = &vis_inf;
  aux_args.pre_inf = &pre_inf;
  float nearplane = nearfarplanes[0]/linfo->block_len;
  float farplane = nearfarplanes[1]/linfo->block_len;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,nearplane,farplane);    //utility info

  //store the vis_inf/pre_inf in the image
  vis_image[j*get_global_size(0)+i] = vis_inf;
  pre_image[j*get_global_size(0)+i] = pre_inf;
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
  __constant RenderSceneInfo *linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);

__kernel
void
bayes_main(__constant  RenderSceneInfo    * linfo,
           __global    int4               * tree_array,       // tree structure for each block
           __global    float              * alpha_array,      // alpha for each block
           __global    MOG_TYPE           * mixture_array,    // mixture for each block
           __global    ushort4            * num_obs_array,    // num obs for each block
           __global    int                * aux_seg_len,        // seg len aux array
           __global    int                * aux_mean_obs,      // mean obs r aux array
           __global    int                * aux_vis,      // mean obs r aux array
           __global    int                * aux_beta,      // mean obs r aux array
           __constant  uchar              * bit_lookup,       // used to get data_index
           __global    float4             * ray_origins,
           __global    float4             * ray_directions,
             __global    float              * nearfarplanes,
           __global    uint4              * imgdims,          // dimensions of the input image
           __global    float              * vis_image,        // visibility image (for keeping vis across blocks)
           __global    float              * pre_image,        // preinf image (for keeping pre across blocks)
           __global    float              * norm_image,        // norm image (for bayes update normalization factor)
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
  float norm = norm_image[j*get_global_size(0) + i];
  float vis = vis_image[j*get_global_size(0) + i];
  float pre = pre_image[j*get_global_size(0) + i];
  if (vis<0.0f)
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
  aux_args.linfo   = linfo;
  aux_args.alpha   = alpha_array;
  aux_args.mog     = mixture_array;
  aux_args.seg_len = aux_seg_len;
  aux_args.mean_obs   = aux_mean_obs;  //&aux_array[linfo->num_buffer * linfo->data_len];
  aux_args.vis_array  = aux_vis;       //&aux_array[2 * linfo->num_buffer * linfo->data_len];
  aux_args.beta_array = aux_beta;      //&aux_array[3 * linfo->num_buffer * linfo->data_len];

  aux_args.ray_bundle_array = ray_bundle_array;
  aux_args.cell_ptrs = cell_ptrs;
  aux_args.cached_vis = cached_vis;
  aux_args.norm = norm;
  aux_args.ray_vis = &vis;
  aux_args.ray_pre = &pre;
  float nearplane = nearfarplanes[0]/linfo->block_len;
  float farplane = nearfarplanes[1]/linfo->block_len;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis0, aux_args,nearplane,farplane);    //utility info

  //write out vis and pre
  vis_image[j*get_global_size(0)+i] = vis;
  pre_image[j*get_global_size(0)+i] = pre;
}
#endif

// normalize the pre_inf image...
//
__kernel
void
proc_norm_image (  __global float* norm_image,
                   __global float* vis_image,
                   __global float* pre_image,
                   __global uint4 * imgdims)
{
  // linear global id of the normalization image
  int i=0;
  int j=0;
  i=get_global_id(0);
  j=get_global_id(1);

  if (i>=(*imgdims).z && j>=(*imgdims).w)
    return;

  float vis = vis_image[j*get_global_size(0) + i];
  float pre = pre_image[j*get_global_size(0) + i];

  if (vis<0.0)  return;
  float norm = (pre + vis);
  norm_image[j*get_global_size(0) + i] = norm;

  // the following  quantities have to be re-initialized before
  // the bayes_ratio kernel is executed
  vis_image[j*get_global_size(0) + i] = 1.0f; // initial vis = 1.0f
  pre_image[j*get_global_size(0) + i] = 0.0f; // initial pre = 0.0
}


// Update each cell using its aux data
//
// Aux Data = [cell_len, mean_obs*cell_len, beta, cum_vis]
void update_rgb_appearance(float8* mixture, float4* nobs, float mean_obs, float cell_vis, float t_match, float init_sigma, float min_sigma)
{
  float mu0 = (*mixture).s0, sigma0 = (*mixture).s1, w0 = (*mixture).s2;
  float mu1 = (*mixture).s3, sigma1 = (*mixture).s4, w1 = (*mixture).s5;
  float mu2 = (*mixture).s6, sigma2 = (*mixture).s7;
  float w2=0.0f;

  if (w0>0.0f && w1>0.0f)
    w2 = 1.0f - w0 - w1;

  short Nobs0 = (short)(*nobs).s0, Nobs1 = (short)(*nobs).s1, Nobs2 = (short)(*nobs).s2;
  float Nobs_mix = (*nobs).s3;

  update_gauss_3_mixture(mean_obs,              //mean observation
                         cell_vis,              //cell_visability
                         t_match,
                         init_sigma,min_sigma,
                         &mu0,&sigma0,&w0,&Nobs0,
                         &mu1,&sigma1,&w1,&Nobs1,
                         &mu2,&sigma2,&w2,&Nobs2,
                         &Nobs_mix);
  (*mixture) = (float8) (mu0, sigma0, w0, mu1, sigma1, w1, mu2, sigma2);
  (*nobs) = (float4) ( (float) Nobs0, (float) Nobs1, (float) Nobs2, (float) Nobs_mix );
}

void update_single_gauss(float8* mixture, float* nobs, float mean_obs, float cell_vis, float min_sigma)
{
  (*nobs) += cell_vis;
  float rho = cell_vis / (*nobs);
  float mu = (*mixture).s0;
  float sigma = (*mixture).s4;
  update_gauss(mean_obs, rho, &mu, &sigma, min_sigma);
  (*mixture).s0 = mu;
  (*mixture).s4 = sigma;
}

//Updates 3 independent gaussian distributions
void update_yuv_appearance(float8* mixture, float* nobs, float4 mean_obs, float cell_vis, float min_sigma)
{
  //update NOBS and calculate rho
  (*nobs) += cell_vis;
  if ( *nobs > 0.001f ) {
    float rho = cell_vis / (*nobs);

    //update y channel
    float mu = (*mixture).s0;
    float sigma = (*mixture).s4;
    update_gauss(mean_obs.x, rho, &mu, &sigma, min_sigma);
    (*mixture).s0 = mu;
    (*mixture).s4 = 0.07;
    //u channel
    mu = (*mixture).s1;
    sigma = (*mixture).s5;
    update_gauss(mean_obs.y, rho, &mu, &sigma, min_sigma);
    (*mixture).s1 = mu;
    (*mixture).s5 = 0.1;
    //v channel
    mu = (*mixture).s2;
    sigma = (*mixture).s6;
    update_gauss(mean_obs.z, rho, &mu, &sigma, min_sigma);
    (*mixture).s2 = mu;
    (*mixture).s6 = 0.1;
  }
}


void update_3d_gauss(float8* mixture, float* nobs, float4 mean_obs, float cell_vis, float min_sigma)
{
  (*nobs) += cell_vis;
  float rho = cell_vis / (*nobs);
  float4 mu = (*mixture).s0123;
  float4 sigma = (*mixture).s4567;
  update_gauss_3d(mean_obs, rho, &mu, &sigma, min_sigma);
  (*mixture).s0123 = mu;
  (*mixture).s4567 = sigma;
}


__kernel
void
update_bit_scene_main(__global RenderSceneInfo  * info,
                      __global float            * alpha_array,
                      __global MOG_TYPE         * mixture_array,
                      __global ushort           * nobs_array,
                      __global int              * aux_seg_len,        // seg len aux array
                      __global int              * aux_mean_obs,      // mean obs r aux array
                      __global int              * aux_vis,       // mean obs r aux array
                      __global int              * aux_beta,      // mean obs r aux array
                      __global int              * update_alpha,
                      __global float            * output)
{
  float t_match = 2.5f;
  float init_sigma = 0.03f;
  float min_sigma = 0.02f;
  int gid=get_global_id(0);
  int datasize = info->data_len ;
  if (gid<datasize)
  {
    //if alpha is less than zero don't update
    float  alpha    = alpha_array[gid];
    float  cell_min = info->block_len/(float)(1<<info->root_level);

    //get cell cumulative length and make sure it isn't 0
    int len_int = aux_seg_len[gid];
    float cum_len  = info->block_len * convert_float(len_int)/SEGLEN_FACTOR;

    //minimum alpha value, don't let blocks get below this
    float  alphamin = -log(1.0-0.0001)/cell_min;

    //update cell if alpha and cum_len are greater than 0
    if (alpha > 0.0f && cum_len > 1e-10f)
    {
      int obs_int = aux_mean_obs[gid];
      int vis_int = aux_vis[gid];
      int beta_int= aux_beta[gid];

      //mean obs is already normalized
      float4 meanObs = convert_float4(as_uchar4(obs_int))/255.0f;

      float cell_vis  = convert_float(vis_int) / (convert_float(len_int) * info->block_len); //(SEGLEN_FACTOR*cum_len);
      float cell_beta = convert_float(beta_int) / (convert_float(len_int) * info->block_len); //(SEGLEN_FACTOR*cum_len);
      CONVERT_FUNC_FLOAT8(mixture,mixture_array[gid])/NORM;

      //single gauss appearance update
      float nob_single = convert_float(nobs_array[gid])/100.0f;

      //-----YUV edit ----
      update_yuv_appearance(&mixture, &nob_single, meanObs, cell_vis, min_sigma);
      //-----------------

      nobs_array[gid] = nob_single * 100.0f;

      //update alpha
      clamp(cell_beta,0.125f,8.0f);
      alpha *= cell_beta;

      //reset the cells in memory
      if (*update_alpha)
        alpha_array[gid]      = max(alphamin, alpha);
      float8 post_mix       = mixture * (float) NORM;
      CONVERT_FUNC_SAT_RTE(mixture_array[gid],post_mix);
    }

    //clear out aux data
    aux_seg_len[gid] = 0;
    aux_mean_obs[gid] = 0;
    aux_vis[gid] = 0;
    aux_beta[gid] = 0;
  }
}

#ifdef UPDATE_APP_YUV
__kernel
    void
    update_yuv_main(__global RenderSceneInfo  * info,
    __global MOG_TYPE         * apm_array,
    __global ushort           * nobs_array,
    __global int              * aux_array0, //seg_len
    __global int              * aux_array1, //vis
    __global int              * aux_array2, //YUV
    __global float            * output)
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;
    if (gid<datasize)
    {
        //get cell cumulative length and make sure it isn't 0
        int len_int = aux_array0[gid];
        float cum_len  = convert_float(len_int)/SEGLEN_FACTOR;
        //update cell if alpha and cum_len are greater than 0
        if (cum_len > 0)
        {

            int vis_int = aux_array1[gid];
            int obs_int = aux_array2[gid];

            float4 mean_obs = convert_float4(as_uchar4(obs_int))/255.0f;
            float cell_vis  = convert_float(vis_int) / convert_float(len_int);

            CONVERT_FUNC_FLOAT8(apm,apm_array[gid])/NORM;

            //single gauss appearance update
            float nobs_single = convert_float(nobs_array[gid])/100.0f;
            update_yuv_appearance(&apm, &nobs_single, mean_obs, cell_vis, 0.02f);

            nobs_array[gid] = nobs_single * 100.0f;

            float8 post_mix       = apm * (float) NORM;
            CONVERT_FUNC_SAT_RTE(apm_array[gid],post_mix);
        }
    }
}
#endif // UPDATE_APP_YUV

#ifdef SEGLEN_VIS
typedef struct
{
  __global float* alpha;
  __global int* seg_len;
  __global int* mean_vis;
  __global int* mean_obsR;
  __global int* mean_obsG;
  __global int* mean_obsB;

  __local  short2* ray_bundle_array;
  __local  int*    cell_ptrs;
           float4  obs;
           float * vis_inf;
  __global float * output;
  __constant  RenderSceneInfo    * linfo;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs, float tnear, float tfar);
__kernel
void
seg_len_vis_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    int                * aux_seg_len,        // seg len aux array
             __global    int                * aux_vis,          // weighted vis sum
             __global    int                * aux_mean_obsY,    // mean obs y aux array
             __global    int                * aux_mean_obsU,    // mean obs u aux array
             __global    int                * aux_mean_obsV,    // mean obs v aux array
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float4             * ray_origins,
             __global    float4             * ray_directions,
             __global    float              * nearfarplanes,
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    float4             * in_image,         // the input image
             __global    float              * vis_image,        // visibility image
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
  int imIndex = j*get_global_size(0) + i;

  AuxArgs aux_args;

  //grab input image value (also holds vis)
  float4 obs = in_image[imIndex];
  float vis_inf = vis_image[imIndex];
  aux_args.vis_inf = &vis_inf;
  barrier(CLK_LOCAL_MEM_FENCE);

  if (obs.x <0.0)
      return;
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y )
    return;

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,w
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ imIndex ];
  float4 ray_d = ray_directions[ imIndex ];
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  aux_args.linfo   = linfo;
  aux_args.alpha = alpha_array;
  aux_args.seg_len  = aux_seg_len;
  aux_args.mean_vis = aux_vis;
  aux_args.mean_obsR = aux_mean_obsY; //&aux_array[linfo->num_buffer * linfo->data_len];
  aux_args.mean_obsG = aux_mean_obsU; //&aux_array[2 * linfo->num_buffer * linfo->data_len];
  aux_args.mean_obsB = aux_mean_obsV; //&aux_array[3 * linfo->num_buffer * linfo->data_len];

  aux_args.ray_bundle_array = ray_bundle_array;
  aux_args.cell_ptrs  = cell_ptrs;
  float nearplane = nearfarplanes[0]/linfo->block_len;
  float farplane = nearfarplanes[1]/linfo->block_len;

  aux_args.obs = rgb2yuv(obs);
  aux_args.output = output;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis_inf, aux_args,nearplane,farplane);    //utility info

  vis_image[ imIndex ] = *(aux_args.vis_inf);
}
#endif
