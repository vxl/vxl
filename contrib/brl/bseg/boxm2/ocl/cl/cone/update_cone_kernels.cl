//:
// \file
// This is brl/bseg/boxm2/ocl/cl/cone/update_cone_kernels.cl
// Created Sept 30, 2010,
// Implements the parallel work group segmentation algorithm.
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


#ifdef PASSONE
//: Define aux_args (like a functor struct)
// \todo Begin passing around AuxArgs* instead of the struct to save on registers
typedef struct
{
  //scene data
  __global float* alphas;
  __global MOG_TYPE* mog;

  //aux data
  __global int* cell_vol;
  __global int* cell_obs;

           //vis/pre along hte ray
           float* ray_vis;
           float* ray_pre;

           //per ball statistics
           float* pi_cum;
           float* vol_cum;
           float* vis_cum;

           //constants used by stepcell functions
           float obs;
           float volume_scale;
} AuxArgs;

void cast_cone_ray( int i, int j,                                     //pixel information
                    float ray_ox, float ray_oy, float ray_oz,         //ray origin
                    float ray_dx, float ray_dy, float ray_dz,         //ray direction
                    float cone_half_angle,
                    __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
                    __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16
                    __local     uchar16            * local_tree,      //local tree for traversing
                    __constant  uchar              * bit_lookup,      //0-255 num bits lookup table

                    __constant  float              * centerX,
                    __constant  float              * centerY,
                    __constant  float              * centerZ,

                    __local     uchar              * cumsum,          //cumulative sum helper for data pointer
                    __local     uchar              * visit_list,      //visit list for BFS, uses 10 chars per thread
                                float              * vis,             //passed in as starting visibility
                                AuxArgs            aux_args );

__kernel
void
pass_one(__constant  RenderSceneInfo    * linfo,
         __global    int4               * tree_array,       // tree structure for each block
         __global    float              * alpha_array,      // alpha for each block
         __global    MOG_TYPE           * mixture_array,    // mixture for each block
         __global    int                * aux_volume,        // seg len aux array
         __global    int                * aux_mean_obs,      // mean obs r aux array
         __constant  uchar              * bit_lookup,       // used to get data_index
         __global    float4             * ray_origins,
         __global    float4             * ray_directions,
         __global    uint4              * imgdims,          // dimensions of the input image
         __global    float              * in_image,         // the input image
         __global    float              * vis_image,        // visibility image (for keeping vis across blocks)
         __global    float              * pre_image,        // preinf image (for keeping pre across blocks)
         __global    float              * norm_image,        // norm image (for bayes update normalization factor)
         __global    float              * output,

         __constant  float              * centerX,          //cached lookup tables for center of cells
         __constant  float              * centerY,
         __constant  float              * centerZ,
         __local     uchar16            * local_tree,       // cache current tree into local memory
         __local     uchar              * cumsum,           // cumulative sum for calculating data pointer
         __local     uchar              * to_visit )        //local mem space for BFS on trees
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
  int imIndex = j*get_global_size(0) + i;
  barrier(CLK_LOCAL_MEM_FENCE);

  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ imIndex ];
  float4 ray_d = ray_directions[ imIndex ];
  float cone_half_angle = ray_d.w; ray_d.w = 0.0f;
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  AuxArgs aux_args;
  aux_args.alphas = alpha_array;
  aux_args.mog = mixture_array;
  aux_args.cell_vol = aux_volume;
  aux_args.cell_obs = aux_mean_obs; //&aux_array[linfo->num_buffer * linfo->data_len];

  float4 obs = in_image[imIndex];
  float vis = vis_image[imIndex];
  float pre = pre_image[imIndex];
  float norm = norm_image[imIndex];
  float pi_cum = 0.0f;
  float vol_cum = 0.0f;
  float vis_cum = 1.0f;
  aux_args.ray_vis = &vis;
  aux_args.ray_pre = &pre;
  aux_args.pi_cum = &pi_cum;
  aux_args.vol_cum = &vol_cum;
  aux_args.vis_cum = &vis_cum;
  aux_args.obs = in_image[imIndex];
  aux_args.volume_scale = linfo->block_len*linfo->block_len*linfo->block_len;

  float vis0=1.0f;
  cast_cone_ray(i, j,
                ray_ox, ray_oy, ray_oz,
                ray_dx, ray_dy, ray_dz, cone_half_angle,
                linfo, tree_array,                      //scene info
                local_tree, bit_lookup, centerX, centerY, centerZ,
                cumsum, to_visit, &vis0, aux_args);      //utility info

  //store vis/pre/norm
  vis_image[imIndex] = vis;
  pre_image[imIndex] = pre;
  norm_image[imIndex] = vis + pre;
}


void step_cell_one(AuxArgs aux_args, int data_ptr, float intersect_volume)
{
  //make sure intersect volume reflects real world scale
  intersect_volume *= aux_args.volume_scale;

  //increment cell volume and cell mean obs, used later
  int vol_int = convert_int_rte(intersect_volume * SEGLEN_FACTOR);
  atom_add( &aux_args.cell_vol[data_ptr], vol_int );
  int cum_obs = convert_int_rte(intersect_volume * aux_args.obs * SEGLEN_FACTOR);
  atom_add( &aux_args.cell_obs[data_ptr], cum_obs );

  // if total length of rays is too small, do nothing
  float8 mixture = convert_float8(aux_args.mog[data_ptr]) / NORM;
  float PI = gauss_3_mixture_prob_density( aux_args.obs,
                                           mixture.s0,
                                           mixture.s1,
                                           mixture.s2,
                                           mixture.s3,
                                           mixture.s4,
                                           mixture.s5,
                                           mixture.s6,
                                           mixture.s7,
                                           (1.0f-mixture.s2-mixture.s5)
                                          );
  float gamma = aux_args.alphas[data_ptr];
  float temp = exp(-intersect_volume*gamma);  //visibility of intersection of this cell and ball

  //accumulate ball statistics
  (*aux_args.pi_cum) += PI*intersect_volume;
  (*aux_args.vol_cum) += intersect_volume;
  (*aux_args.vis_cum) *= temp;
}

void compute_ball_properties(AuxArgs aux_args)
{
  float vis = (*aux_args.ray_vis); //(*vis_img_)(i,j);
  float pre = (*aux_args.ray_pre); //(*pre_img_)(i,j);
  float PI=0.0;
  if ( *aux_args.vol_cum>1e-12f) PI = (*aux_args.pi_cum) / (*aux_args.vol_cum);

  //incrememnt pre and vis;
  float vis_cum = (*aux_args.vis_cum);
  pre += vis*(1.0-vis_cum)*PI;
  vis *= vis_cum;
  (*aux_args.ray_pre) = pre;
  (*aux_args.ray_vis) = vis;

  //reset ball values
  (*aux_args.vis_cum) = 1.0f;
  (*aux_args.pi_cum) = 0.0f;
  (*aux_args.vol_cum) = 0.0f;
}
#endif

#ifdef BAYES
typedef struct
{
  __global float*   alphas;
  __global MOG_TYPE * mog;

  //aux data
  __global int* cell_vol;
  __global int* cell_obs;
  __global int* cell_vis;
  __global int* cell_beta;

           float   norm;
           float*  ray_vis;
           float*  ray_pre;
           float   obs;

           //ball level members
           float* pi_cum;
           float* vol_cum;
           float* vis_cum;
           float* beta_cum;

           float volume_scale;
} AuxArgs;

void cast_cone_ray( int i, int j,                                     //pixel information
                    float ray_ox, float ray_oy, float ray_oz,         //ray origin
                    float ray_dx, float ray_dy, float ray_dz,         //ray direction
                    float cone_half_angle,
                    __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
                    __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16
                    __local     uchar16            * local_tree,      //local tree for traversing
                    __constant  uchar              * bit_lookup,      //0-255 num bits lookup table

                    __constant  float              * centerX,
                    __constant  float              * centerY,
                    __constant  float              * centerZ,

                    __local     uchar              * cumsum,          //cumulative sum helper for data pointer
                    __local     uchar              * visit_list,      //visit list for BFS, uses 10 chars per thread
                                float              * vis,             //passed in as starting visibility
                                AuxArgs            aux_args );

__kernel
void
bayes_main(__constant  RenderSceneInfo    * linfo,
           __global    int4               * tree_array,       // tree structure for each block
           __global    float              * alpha_array,      // alpha for each block
           __global    MOG_TYPE           * mixture_array,    // mixture for each block
           __global    int                * aux_vol,        // seg len aux array
           __global    int                * aux_obs,      // mean obs r aux array
           __global    int                * aux_vis,      // mean obs r aux array
           __global    int                * aux_beta,      // mean obs r aux array
           __constant  uchar              * bit_lookup,       // used to get data_index
           __global    float4             * ray_origins,
           __global    float4             * ray_directions,
           __global    uint4              * imgdims,          // dimensions of the input image
           __global    float              * in_image,         // input image
           __global    float              * vis_image,        // visibility image (for keeping vis across blocks)
           __global    float              * pre_image,        // preinf image (for keeping pre across blocks)
           __global    float              * norm_image,        // norm image (for bayes update normalization factor)
           __global    float              * output,

           __constant  float              * centerX,          //cached lookup tables for center of cells
           __constant  float              * centerY,
           __constant  float              * centerZ,
           __local     uchar16            * local_tree,       // cache current tree into local memory
           __local     uchar              * cumsum,           // cumulative sum for calculating data pointer
           __local     uchar              * to_visit )        //local mem space for BFS on trees
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
  float norm = norm_image[j*get_global_size(0) + i];
  float vis = 1.0f; //vis_image[j*get_global_size(0) + i];
  float pre = 0.0f; //pre_image[j*get_global_size(0) + i];
  float obs = in_image[j*get_global_size(0) + i];
  barrier(CLK_LOCAL_MEM_FENCE);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origins[ j*get_global_size(0) + i ];
  float4 ray_d = ray_directions[ j*get_global_size(0) + i ];
  float cone_half_angle = ray_d.w; ray_d.w = 0.0f;
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray_generic_cam(linfo, ray_o, ray_d, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  AuxArgs aux_args;
  aux_args.alphas   = alpha_array;
  aux_args.mog     = mixture_array;

  //global aux
  aux_args.cell_vol = aux_vol;
  aux_args.cell_obs  = aux_obs; //&aux_array[linfo->num_buffer * linfo->data_len];
  aux_args.cell_vis  = aux_vis; //&aux_array[2 * linfo->num_buffer * linfo->data_len];
  aux_args.cell_beta = aux_beta; //&aux_array[3 * linfo->num_buffer * linfo->data_len];
  aux_args.norm = norm;
  aux_args.obs = obs;
  aux_args.ray_vis = &vis;
  aux_args.ray_pre = &pre;
  float pi_cum=0.0f;
  float vol_cum = 0.0f;
  float vis_cum = 1.0f;
  float beta_cum = 1.0f;
  aux_args.pi_cum = &pi_cum;
  aux_args.vol_cum = &vol_cum;
  aux_args.vis_cum = &vis_cum;
  aux_args.beta_cum = &beta_cum;
  aux_args.volume_scale = linfo->block_len*linfo->block_len*linfo->block_len;

  float vis0 = 1.0f;
  cast_cone_ray(i, j,
                ray_ox, ray_oy, ray_oz,
                ray_dx, ray_dy, ray_dz, cone_half_angle,
                linfo, tree_array,                                    //scene info
                local_tree, bit_lookup, centerX, centerY, centerZ,
                cumsum, to_visit, &vis0, aux_args);      //utility info

  //write out vis and pre
  vis_image[j*get_global_size(0)+i] = vis;
  pre_image[j*get_global_size(0)+i] = pre;
}


bool step_cell(AuxArgs aux_args, int data_ptr, float intersect_volume)
{
  //rescale intersect volume
  intersect_volume *= aux_args.volume_scale;

  //rescale aux args, calculate mean obs
  float8 mixture = convert_float8(aux_args.mog[data_ptr]) / NORM;
  float PI = gauss_3_mixture_prob_density( aux_args.obs,
                                           mixture.s0,
                                           mixture.s1,
                                           mixture.s2,
                                           mixture.s3,
                                           mixture.s4,
                                           mixture.s5,
                                           mixture.s6,
                                           mixture.s7,
                                           (1.0f-mixture.s2-mixture.s5)
                                          );
  float vis = (*aux_args.ray_vis);
  float gamma = aux_args.alphas[data_ptr];

  //sum cumulative vis
  int vis_int = convert_int_rte( vis*intersect_volume*SEGLEN_FACTOR );
  atom_add( &aux_args.cell_vis[data_ptr], vis_int );

  //update ball properties
  float temp=exp(-intersect_volume*gamma);
  *aux_args.pi_cum += PI*intersect_volume;
  *aux_args.vol_cum += intersect_volume;
  *aux_args.vis_cum *= temp;
  return true;
}

bool compute_ball_properties(AuxArgs aux_args)
{
  float vis = (*aux_args.ray_vis);
  float pre = (*aux_args.ray_pre);
  float PI=0.0;
  if ( *aux_args.vol_cum>1e-12f ) PI = (*aux_args.pi_cum) / (*aux_args.vol_cum);

  //incrememnt beta pre and vis along the ray
  (*aux_args.beta_cum) = (pre+vis*PI)/aux_args.norm;
  pre += vis*(1.0f - (*aux_args.vis_cum) )*PI;
  vis *= (*aux_args.vis_cum);
  (*aux_args.ray_pre) = pre;
  (*aux_args.ray_vis) = vis;

  //reset ball values
  (*aux_args.vis_cum) = 1.0f;
  (*aux_args.pi_cum) = 0.0f;
  (*aux_args.vol_cum) = 0.0f;
  return true;
}

bool redistribute(AuxArgs aux_args, int data_ptr, float intersect_volume)
{
  intersect_volume *= aux_args.volume_scale;
  int beta_int = convert_int_rte( intersect_volume * (*aux_args.beta_cum) * SEGLEN_FACTOR );
  atom_add( &aux_args.cell_beta[data_ptr], beta_int );
  return true;
}

#endif

__kernel
void
update_cone_data( __global RenderSceneInfo  * info,
                  __global float            * alpha_array,
                  __global MOG_TYPE         * mixture_array,
                  __global ushort4          * nobs_array,
                  __global int              * aux_vol,        // seg len aux array
                  __global int              * aux_obs,      // mean obs r aux array
                  __global int              * aux_vis,      // mean obs r aux array
                  __global int              * aux_beta)     // mean obs r aux array
{
  float  cell_min = info->block_len/(float)(1<<info->root_level);
  float  alphamin = -log(1.0-0.0001)/cell_min;
  float t_match = 2.5f;
  float init_sigma = 0.09f;
  float min_sigma = 0.03f;
  int gid=get_global_id(0);
  int datasize = info->data_len * info->num_buffer;
  if (gid<datasize)
  {
    float cell_vol = convert_float(aux_vol[gid]) / SEGLEN_FACTOR;
    if (cell_vol>1e-10f)
    {
      float beta = convert_float(aux_beta[gid])/(cell_vol*SEGLEN_FACTOR);
      float vis  = convert_float(aux_vis[gid])/(cell_vol*SEGLEN_FACTOR);
      float mean_obs = convert_float(aux_obs[gid])/(cell_vol*SEGLEN_FACTOR);

      float alpha = alpha_array[gid];
      MOG_TYPE mog_bytes = mixture_array[gid];
      float8 mog = convert_float8(mog_bytes) / (float) NORM;
      ushort4 num_obs = nobs_array[gid];

      //update alpha
      alpha = alpha*beta;

      //update gauss 3 mixture with mean-obs
      float mu0 = mog.s0, sigma0 = mog.s1, w0 = mog.s2;
      float mu1 = mog.s3, sigma1 = mog.s4, w1 = mog.s5;
      float mu2 = mog.s6, sigma2 = mog.s7;
      float w2=0.0f;
      if (w0>0.0f && w1>0.0f)
        w2=1.0f-w0-w1;

      short Nobs0 = (short) num_obs.s0,
            Nobs1 = (short) num_obs.s1,
            Nobs2 = (short) num_obs.s2;
      float Nobs_mix = num_obs.s3/100.0f;
      update_gauss_3_mixture( mean_obs, vis, t_match, init_sigma, min_sigma,
                              &mu0,&sigma0,&w0,&Nobs0,
                              &mu1,&sigma1,&w1,&Nobs1,
                              &mu2,&sigma2,&w2,&Nobs2,
                              &Nobs_mix );

      //reset the cells in memory
      alpha_array[gid]      = max(alphamin, alpha);
      float8 post_mix       = (float8) (mu0, sigma0, w0,
                                        mu1, sigma1, w1,
                                        mu2, sigma2) * (float) NORM;
      CONVERT_FUNC_SAT_RTE(mixture_array[gid],post_mix)
      float4 post_nobs      = (float4) (Nobs0, Nobs1, Nobs2, Nobs_mix*100.0);
      nobs_array[gid]       = convert_ushort4_sat_rte(post_nobs);
    }

    //zero out all aux data
    aux_vol[gid] = 0;
    aux_obs[gid] = 0;
    aux_vis[gid] = 0;
    aux_beta[gid] = 0;
  }
}


