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
  __global int*   seg_len;
  __global int*   mean_obsR;
  __global int*   mean_obsG;
  __global int*   mean_obsB; 
           float4 obs; 
} AuxArgs;  

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*, 
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs); 
__kernel
void
seg_len_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    int                * aux_array,        // aux data array (four aux arrays strung together)
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float16            * camera,           // camera orign and SVD of inverse of camera matrix
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    uchar4             * in_image,         // the input image (RGB)
             __global    float              * vis_image,        // visibility image (initial visibilty for this block)
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

  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y) 
    return;
  
  //grab input image value (all 4 potential components) and vis from vis_image
  float4 obs = convert_float4(in_image[imIndex])/255.0f;       
  float vis = vis_image[imIndex];

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
  aux_args.mean_obsR = &aux_array[linfo->num_buffer * linfo->data_len]; 
  aux_args.mean_obsG = &aux_array[2 * linfo->num_buffer * linfo->data_len];
  aux_args.mean_obsB = &aux_array[3 * linfo->num_buffer * linfo->data_len];
  aux_args.obs = obs; 
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args);    //utility info
  
  //store vis for more
  vis_image[imIndex] = vis; 
}
#endif


//second pass compresses the mean obs rgb value into the mean obs aux data
#ifdef COMPRESS_RGB
__kernel
void
compress_rgb(__global RenderSceneInfo  * info,
             __global int              * aux_array)
{
  int gid = get_global_id(0);
  int datasize = info->data_len * info->num_buffer;
  if (gid<datasize)
  {
    //get the segment length
    int len_int = aux_array[gid]; 
    float cum_len  = convert_float(len_int)/SEGLEN_FACTOR; 

    //get cumulative observation values for r g and b
    int r_int = aux_array[datasize + gid]; 
    int g_int = aux_array[2*datasize + gid]; 
    int b_int = aux_array[3*datasize + gid];
    float4 meanRGB = (float4) 0.0f;
    meanRGB.x = convert_float(r_int) / (SEGLEN_FACTOR*cum_len); 
    meanRGB.y = convert_float(g_int) / (SEGLEN_FACTOR*cum_len); 
    meanRGB.z = convert_float(b_int) / (SEGLEN_FACTOR*cum_len); 

    //store them as uchar4, pack it into the old R slot
    uchar4 meanObs = convert_uchar4( meanRGB*255.0f ); 
    aux_array[datasize + gid] = as_int(meanObs); 
  }
}
#endif


#ifdef PREINF
typedef struct
{
  __global float*   alpha; 
  __global uchar16* mog; 
  __global int*     seg_len;      //seg len aux data
  __global int*     mean_obs;     //mean obs aux data (stored as chars for the RGB case)
           float*   pre_inf;
           float*   vis_inf; 
           float*   alpha_int;  
} AuxArgs;  

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*, 
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs); 

__kernel
void
pre_inf_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    uchar16            * mixture_array,    // mixture for each block
             __global    int                * aux_array,        // four aux arrays strung together
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float16            * camera,           // camera orign and SVD of inverse of camera matrix
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    uchar4             * in_image,         // the input image
             __global    float              * vis_image,        // visibility image (initial visibilty for this block)
             __global    float              * pre_image,        // pre image (initial pre for this block)
             __global    float              * alpha_int_image,  // alpha_integrated (in log space)
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
  int imIndex = j*get_global_size(0) + i; 

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;
  float vis = vis_image[imIndex]; 
  float pre = pre_image[imIndex]; 
  float aint = alpha_int_image[imIndex]; 

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
  aux_args.pre_inf = &pre; 
  aux_args.vis_inf = &vis; 
  aux_args.alpha_int = &aint; 
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args);    //utility info

  //store the vis_inf/pre_inf in the image      
  vis_image[imIndex] = vis; 
  pre_image[imIndex] = pre; 
  alpha_int_image[imIndex] = aint; 
}
#endif

#ifdef BAYES
typedef struct
{
  __global float*   alpha; 
  __global uchar16* mog;   
  __global int*     seg_len;
  __global int*     mean_obs; 
  __global int*     vis_array;
  __global int*     beta_array;

           float    norm; 
           float*   ray_vis; 
           float*   ray_pre; 
} AuxArgs;  

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*, 
              __global int4*,local uchar16*,constant uchar *,local uchar*,float*,AuxArgs); 

__kernel
void
bayes_main(__constant  RenderSceneInfo    * linfo,
           __global    int4               * tree_array,       // tree structure for each block
           __global    float              * alpha_array,      // alpha for each block
           __global    uchar16            * mixture_array,    // mixture for each block
           __global    int                * aux_array,        // four aux arrays strung together
           __constant  uchar              * bit_lookup,       // used to get data_index
           __global    float16            * camera,           // camera orign and SVD of inverse of camera matrix
           __global    uint4              * imgdims,          // dimensions of the input image
           __global    uchar4             * in_image,         // the input image
           __global    float              * vis_image,        // visibility image (initial visibilty for this block)
           __global    float              * pre_image,        // pre image (initial pre for this block)
           __global    float              * norm_image,       // norm image spat out by proc_norm_image
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
  int imIndex = j*get_global_size(0) + i; 

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y) 
    return;
  float vis0 = 1.0f;
  float norm = norm_image[imIndex];
  float vis  = vis_image[imIndex];
  float pre  = pre_image[imIndex];
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
  aux_args.seg_len    = aux_array;
  aux_args.mean_obs   = &aux_array[linfo->num_buffer * linfo->data_len]; 
  aux_args.vis_array  = &aux_array[2 * linfo->num_buffer * linfo->data_len];
  aux_args.beta_array = &aux_array[3 * linfo->num_buffer * linfo->data_len];
  
  aux_args.norm = norm; 
  aux_args.ray_vis = &vis; 
  aux_args.ray_pre = &pre; 
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis0, aux_args);    //utility info
            
  //write out vis and pre
  //in_image[imIndex].zw = (float2) (vis, pre); 
  vis_image[imIndex] = vis;
  pre_image[imIndex] = pre;
}
#endif


// normalize the pre_inf image...
//
__kernel
void
proc_norm_image(__global float* pre_inf,
                __global float* vis_inf,
                __global float* norm_img,
                __global uint4 * imgdims)
{
  // linear global id of the normalization image
  int lgid = get_global_id(0) + get_global_size(0)*get_global_id(1);

  int i=0,j=0;
  map_work_space_2d(&i,&j);
  i=get_global_id(0);
  j=get_global_id(1);
  int imIndex = j*get_global_size(0)+i; 

  if (i>=(*imgdims).z && j>=(*imgdims).w)
    return;

  //get the vector of pre,vis,
  float vis_i = vis_inf[imIndex]; 
  float pre_i = pre_inf[imIndex]; 

  //multiplyer for norm image
  float mult = 1.0f; 

  // compute the norm image
  //vect.x = vect.w + mult * vect.z;
  norm_img[imIndex] = pre_i + mult * vis_i; 
  
  // the following  quantities have to be re-initialized before
  // clear alpha integral, pre and vis
  vis_inf[imIndex] = 1.0f; 
  pre_inf[imIndex] = 0.0f; 
}


// Update each cell using its aux data
//
__kernel
void
update_bit_scene_main(__global RenderSceneInfo  * info,
                      __global float            * alpha_array,
                      __global uchar16          * mixture_array,
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

    float  alphamin = -log(1.0 - 0.0001)/cell_min; //minimum alpha value, don't let blocks get below this

    //update cell if alpha and cum_len are greater than 0
    if (alpha > 0.0f && cum_len > 1e-10f)
    {
      float4 mean_obs = convert_float4( as_uchar4( aux_array[datasize + gid] ) ); //interpret the 32 bits as a uchar4 
      mean_obs = mean_obs/ (cum_len * 255.0f); 
      float cell_vis  = convert_float(aux_array[2*datasize + gid])/(SEGLEN_FACTOR * cum_len);
      float cell_beta = convert_float(aux_array[3*datasize + gid])/SEGLEN_FACTOR;
      //float4 aux_data = (float4) (cum_len, mean_obs, cell_beta, cell_vis/cum_len);
      float4 nobs     = convert_float4(nobs_array[gid]);
      float16 mixture = convert_float16(mixture_array[gid]);

      //use aux data to update cells
      float t_match = 2.5f; float init_sigma = 0.03f; float min_sigma = 0.03f; 
      float4 mu0 = mixture.s0123; float4 sigma0 = mixture.s4567; 
      float  w0  = mixture.s7;
      float4 mu1 = mixture.s89AB; float4 sigma1 = mixture.sCDEF; 
      float  w1  = 0.0f;
      if(w0>0.0f)
        w1 = 1.0f - w0; 

      short Nobs0 = (short)nobs.s0, Nobs1 = (short)nobs.s1; 
      float Nobs_mix = nobs.s3/100.0f;
      /*update_gauss_2_mixture_rgb(mean_obs, 
                                 cell_vis, 
                                 t_match, 
                                 init_sigma, 
                                 min_sigma, 
                                 &mu0, &sigma0, &w0, &Nobs0,
                                 &mu1, &sigma1, &w1, &Nobs1,
                                 &Nobs_mix);  */

    //DEBUG DEUBG
      float m0 = mu0.x; float s0 = sigma0.x; 
      float m1 = mu1.x; float s1 = sigma1.x; 
      float m2 = 0.0f; float s2 = 0.0f; float w2 = 0.0f; 
      short Nobs2 = 0; 
      update_gauss_3_mixture(   mean_obs.x,              //mean observation
                                 cell_vis,              //cell_visability
                                 t_match,                 
                                 init_sigma,min_sigma,
                                 &m0,&s0,&w0,&Nobs0,
                                 &m1,&s1,&w1,&Nobs1,
                                 &m2,&s2,&w2,&Nobs2,
                                 &Nobs_mix);
      mu0.x = m0; sigma0.x = s0; 
      mu1.x = m1; sigma1.x = s1;
    //END DEBUG DEUBG
      
      //update alpha
      alpha *= cell_beta / cum_len;  // (*data).s0 *= aux_data.z/aux_data.x;

      //reset the cells in memory   
      alpha_array[gid]      = max(alphamin, alpha);
      
      //store post mix
      float16 post_mix = (float16) (mu0, sigma0, mu1, sigma1) * 255.0f; 
      post_mix.s7 = (w0*255.0f); 
      mixture_array[gid]    = convert_uchar16_sat_rte(post_mix);
      nobs_array[gid]       = (ushort4) ((ushort)Nobs0, (ushort)Nobs1, 0, convert_ushort( Nobs_mix*100.0f ) ); 
    }
    
    //clear out aux data
    aux_array[gid] = 0; 
    aux_array[gid + datasize] = 0; 
    aux_array[gid + 2*datasize] = 0;
    aux_array[gid + 3*datasize] = 0;
  }
  
}
