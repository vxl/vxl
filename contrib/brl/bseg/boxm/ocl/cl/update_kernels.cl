//THIS IS UPDATE BIT SCENE OPT
//Created Sept 30, 2010, 
//Implements the parallel work group segmentation algorithm.  

#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif


#ifdef SEGLEN
__kernel
void
seg_len_main(        __constant  RenderSceneInfo    * linfo,
                     __global    ushort2            * block_ptrs,
                     __global    int4               * tree_array,       // tree structure for each block
                     __global    float              * alpha_array,      // alpha for each block
                     __global    int                * num_obs_array,    // num obs for each block
                     __global    float2             * cum_len_beta,     // cumulative ray length and beta aux vars
                     __global    uchar2             * mean_obs_cum_vis, // mean_obs per cell and cumulative visibility
                     __constant  uchar              * bit_lookup,       // used to get data_index
                     __local     uchar16            * local_tree,       // cache current tree into local memory
                     __global    float16            * camera,           // camera orign and SVD of inverse of camera matrix
                     __global    float16            * cammat,           // translated camera matrix 
                     __global    uint4              * imgdims,          // dimensions of the input image
                     __global    float4             * in_image,         // the input image
                     __global    int                * offsetfactor,     // 
                     __global    int                * offset_x,         // offset to the left and 
                     __global    int                * offset_y,         // right (which one of the four blocks)
                     __local     short2             * ray_bundle_array, // gives information for which ray takes over in the workgroup
                     __local     int                * cell_ptrs,        // local list of cell_ptrs (cells that are hit by this workgroup
                     __local     float4             * cached_aux_data,  // seg len cached aux data is only a float2
                     __local     uchar              * cumsum,           // cumulative sum for calculating data pointer
                     __global    float              * output)    
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
  
  //map_work_space_2d_offset(&i,&j,(*offset_x),(*offset_y));
  //int factor=(*offsetfactor);
  map_work_space_2d(&i,&j);
  int factor=1;

  // check to see if the thread corresponds to an actual pixel as in some 
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w) {
    return;
  }
  //if(i<320 || i>=328 || j<240 || j>=248)
  //  return;

  float4 inImage = in_image[j*get_global_size(0)*factor+i];
  float obs = inImage.x;
  float vis = inImage.z;
  barrier(CLK_LOCAL_MEM_FENCE);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  // ray origin, ray direction, inverse ray direction (make sure ray direction is never axis aligned)
  float4 ray_o = (float4) camera[2].s4567; ray_o.w = 1.0f;
  float4 ray_d = backproject(i, j, camera[0], camera[1], camera[2], ray_o);
  ray_o = ray_o - linfo->origin; ray_o.w = 1.0f; //translate ray o to zero out scene origin
  ray_o = ray_o/linfo->block_len; ray_o.w = 1.0f;

  //thresh ray direction components - too small a treshhold causes axis aligned 
  //viewpoints to hang in infinite loop (block loop)
  float thresh = exp2(-12.0f); 
  if (fabs(ray_d.x) < thresh) ray_d.x = copysign(thresh, ray_d.x);
  if (fabs(ray_d.y) < thresh) ray_d.y = copysign(thresh, ray_d.y);
  if (fabs(ray_d.z) < thresh) ray_d.z = copysign(thresh, ray_d.z);
  ray_d.w = 0.0; ray_d = normalize(ray_d);
  
  //store float 3's
  float ray_ox = ray_o.x;     float ray_oy = ray_o.y;     float ray_oz = ray_o.z;
  float ray_dx = ray_d.x;     float ray_dy = ray_d.y;     float ray_dz = ray_d.z;
  
  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------  

  cast_ray( i, j, 
            ray_ox, ray_oy, ray_oz, 
            ray_dx, ray_dy, ray_dz, 

            //scene info                               mix,weight3,numobs,cum_len, mean_obs
            linfo, block_ptrs, tree_array, alpha_array, 0, 0, num_obs_array, cum_len_beta, mean_obs_cum_vis,
           
            //utility info
            local_tree, bit_lookup, cumsum, factor,
            
            //SEGLEN SPECIFIC ARGS
            //factor,raybund,ptrs,cache,cache,image_vect (all NULL)
            cammat, ray_bundle_array, cell_ptrs, obs, vis, cached_aux_data, 
            
            //io info
            in_image, 0, output);
}
#endif

#ifdef PREINF
__kernel
void
pre_inf_main(__constant  RenderSceneInfo    * linfo,
             __global    ushort2            * block_ptrs,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    uchar8             * mixture_array,    // mixture for each block
             __global    uchar              * last_weight_array,//third weight for mixture model
             __global    ushort4            * num_obs_array,    // num obs for each block
             __global    float2             * cum_len_beta,    // cumulative ray length and beta aux vars
             __global    uchar2             * mean_obs_cum_vis, // mean_obs per cell and cumulative visibility
             __constant  uchar              * bit_lookup,       // used to get data_index
             __local     uchar16            * local_tree,       // cache current tree into local memory
             __global    float16            * camera,           // camera orign and SVD of inverse of camera matrix
             __global    float16            * cammat,           // translated camera matrix 
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    float4             * in_image,         // the input image
             __global    int                * offsetfactor,     // 
             __global    int                * offset_x,         // offset to the left and 
             __global    int                * offset_y,         // right (which one of the four blocks)
             __local     uchar              * cumsum,           // cumulative sum for calculating data pointer
             __global    float              * output)    
{

  //get local id (0-63 for an 8x8) of this patch 
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  
  //----------------------------------------------------------------------------
  // get image coordinates and camera, 
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  int i=0,j=0; map_work_space_2d(&i,&j);
  int factor=1;

  // check to see if the thread corresponds to an actual pixel as in some 
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w) {
    return;
  }
  float4 inImage = in_image[j*get_global_size(0)*factor+i];
  barrier(CLK_LOCAL_MEM_FENCE);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  // ray origin, ray direction, inverse ray direction (make sure ray direction is never axis aligned)
  float4 ray_o = (float4) camera[2].s4567; ray_o.w = 1.0f;
  float4 ray_d = backproject(i, j, camera[0], camera[1], camera[2], ray_o);
  ray_o = ray_o - linfo->origin; ray_o.w = 1.0f; //translate ray o to zero out scene origin
  ray_o = ray_o/linfo->block_len; ray_o.w = 1.0f;

  //thresh ray direction components - too small a treshhold causes axis aligned 
  //viewpoints to hang in infinite loop (block loop)
  float thresh = exp2(-12.0f); 
  if (fabs(ray_d.x) < thresh) ray_d.x = copysign(thresh, ray_d.x);
  if (fabs(ray_d.y) < thresh) ray_d.y = copysign(thresh, ray_d.y);
  if (fabs(ray_d.z) < thresh) ray_d.z = copysign(thresh, ray_d.z);
  ray_d.w = 0.0; ray_d = normalize(ray_d);
  
  //store float 3's
  float ray_ox = ray_o.x;     float ray_oy = ray_o.y;     float ray_oz = ray_o.z;
  float ray_dx = ray_d.x;     float ray_dy = ray_d.y;     float ray_dz = ray_d.z;
  
  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------  
  cast_ray( i, j, 
            ray_ox, ray_oy, ray_oz, 
            ray_dx, ray_dy, ray_dz, 

            //scene info                               (mixture array, weight3, numobs,    aux (cum_len), aux2(mean vis)
            linfo, block_ptrs, tree_array, alpha_array, mixture_array, last_weight_array, num_obs_array, cum_len_beta, mean_obs_cum_vis,
           
            //utility info
            local_tree, bit_lookup, cumsum, factor,
            
            //PREINF SPECIFIC ARGS
            cammat, inImage,
            
            //io info
            in_image, 0, output);
}
#endif

#ifdef BAYES
__kernel
void
bayes_main(__constant  RenderSceneInfo    * linfo,
           __global    ushort2            * block_ptrs,
           __global    int4               * tree_array,       // tree structure for each block
           __global    float              * alpha_array,      // alpha for each block
           __global    uchar8             * mixture_array,    // mixture for each block
           __global    uchar              * last_weight_array,//third weight for mixture model
           __global    int                * num_obs_array,    // num obs for each block
           __global    float2             * cum_len_beta,    // cumulative ray length and beta aux vars
           __global    uchar2             * mean_obs_cum_vis, // mean_obs per cell and cumulative visibility
           __constant  uchar              * bit_lookup,       // used to get data_index
           __local     uchar16            * local_tree,       // cache current tree into local memory
           __global    float16            * camera,           // camera orign and SVD of inverse of camera matrix
           __global    float16            * cammat,           // translated camera matrix 
           __global    uint4              * imgdims,          // dimensions of the input image
           __global    float4             * in_image,         // the input image
           __global    int                * offsetfactor,     // 
           __global    int                * offset_x,         // offset to the left and 
           __global    int                * offset_y,         // right (which one of the four blocks)
           __local     short2             * ray_bundle_array, // gives information for which ray takes over in the workgroup
           __local     int                * cell_ptrs,        // local list of cell_ptrs (cells that are hit by this workgroup
           __local     uchar              * cumsum,           // cumulative sum for calculating data pointer
           __global    float              * output)    
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
  //map_work_space_2d_offset(&i,&j,(*offset_x),(*offset_y));
  //int factor=(*offsetfactor);
  map_work_space_2d(&i,&j);
  int factor=1;

  // check to see if the thread corresponds to an actual pixel as in some 
  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w) {
    return;
  }
  //image_vect[llid] = in_image[j*get_global_size(0)*factor+i];
  float4 inImage = in_image[j*get_global_size(0)*factor+i];
  float norm = inImage.x;
  float vis  = inImage.z;
  float pre  = inImage.w;
  barrier(CLK_LOCAL_MEM_FENCE);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  // ray origin, ray direction, inverse ray direction (make sure ray direction is never axis aligned)
  float4 ray_o = (float4) camera[2].s4567; ray_o.w = 1.0f;
  float4 ray_d = backproject(i, j, camera[0], camera[1], camera[2], ray_o);
  ray_o = ray_o - linfo->origin; ray_o.w = 1.0f; //translate ray o to zero out scene origin
  ray_o = ray_o/linfo->block_len; ray_o.w = 1.0f;

  //thresh ray direction components - too small a treshhold causes axis aligned 
  //viewpoints to hang in infinite loop (block loop)
  float thresh = exp2(-12.0f); 
  if (fabs(ray_d.x) < thresh) ray_d.x = copysign(thresh, ray_d.x);
  if (fabs(ray_d.y) < thresh) ray_d.y = copysign(thresh, ray_d.y);
  if (fabs(ray_d.z) < thresh) ray_d.z = copysign(thresh, ray_d.z);
  ray_d.w = 0.0; ray_d = normalize(ray_d);
  
  //store float 3's
  float ray_ox = ray_o.x;     float ray_oy = ray_o.y;     float ray_oz = ray_o.z;
  float ray_dx = ray_d.x;     float ray_dy = ray_d.y;     float ray_dz = ray_d.z;
  
  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------  
   cast_ray( i, j, 
            ray_ox, ray_oy, ray_oz, 
            ray_dx, ray_dy, ray_dz, 

            //scene info                                              //numobs and mixture null
            linfo, block_ptrs, tree_array, alpha_array, mixture_array, last_weight_array, num_obs_array, cum_len_beta, mean_obs_cum_vis,
           
            //utility info
            local_tree, bit_lookup, cumsum, factor,
            
            //BAYES SPECIFIC ARGUMENTS
            //factor,raybund,ptrs,cache,cache,image_vect (all NULL)
            cammat, ray_bundle_array, cell_ptrs, norm, vis, pre,
            
            //io info
            in_image, 0, output);
}
#endif


/*
 * normalize the pre_inf image... 
 */
__kernel 
void 
proc_norm_image(__global float4* image, __global float4* p_inf,__global uint4   * imgdims)
{
    /* linear global id of the normalization image */
    int lgid = get_global_id(0) + get_global_size(0)*get_global_id(1);

    int i=0;
    int j=0;
    map_work_space_2d(&i,&j);
    
    if (i>=(*imgdims).z && j>=(*imgdims).w)
        return;

    float4 vect = image[j*get_global_size(0)+i];
    float mult = (p_inf[0].x>0.0f) ? 1.0f :
        gauss_prob_density(vect.x, p_inf[0].y, p_inf[0].z);
    /* compute the norm image */
    vect.x = vect.w + mult * vect.z;
    /* the following  quantities have to be re-initialized before
    *the bayes_ratio kernel is executed
    */
    vect.y = 0.0f;/* clear alpha integral */
    vect.z = 1.0f; /* initial vis = 1.0 */
    vect.w = 0.0f; /* initial pre = 0.0 */
    /* write it back */
    image[j*get_global_size(0)+i] = vect;
}



/*
 * Update each cell using it's aux data
 */ 
__kernel
void
update_bit_scene_main(__global RenderSceneInfo  * info,
                      __global float            * alpha_array,
                      __global uchar8           * mixture_array,
                      __global uchar            * last_weight_array,
                      __global ushort4          * nobs_array,
                      __global float2           * cum_len_beta,    // cumulative ray length and beta aux vars
                      __global uchar2           * mean_obs_cum_vis, // mean_obs per cell and cumulative visibility
                      __global float            * output)
{
    int gid=get_global_id(0);
    int datasize = info->data_len * info->num_buffer;
    if (gid<datasize)
    {
      //if alpha is less than zero don't update
      float  alpha    = alpha_array[gid];
      float  cell_min = info->block_len/(float)(1<<info->root_level);

      //minimum alpha value, don't let blocks get below this
      float  alphamin = -log(1.0-0.0001)/cell_min;

      if(alpha > 0.0) 
      {
          //load global data into registers
          //load aux data into local mem
          float2 cl_beta  = cum_len_beta[gid];
          float2 mean_vis = convert_float2(mean_obs_cum_vis[gid])/255.0f;
          float4 aux_data = (float4) (cl_beta.x,            //cell length
                                      mean_vis.x,           //mean observation
                                      cl_beta.y,            //beta (bayes update ratio)
                                      mean_vis.y);          //cell visability
          
          float4 nobs     = convert_float4(nobs_array[gid]); nobs.s3 /= 100.0f;
          float8 mixture  = convert_float8(mixture_array[gid])/255.0f;
          float  weight3  = convert_float(last_weight_array[gid])/255.0f;
          
          //old method --------------------------------------------------------------------------
/*
          float16 data = (float16) (alpha, 
                         (mixture.s0/255.0), (mixture.s1/255.0), (mixture.s2/255.0), (nobs.s0), 
                         (mixture.s3/255.0), (mixture.s4/255.0), (mixture.s5/255.0), (nobs.s1),
                         (mixture.s6/255.0), (mixture.s7/255.0), (nobs.s2), (nobs.s3/100.0), 
                         0.0, 0.0, 0.0);
*/
          //------------------------------------------------------------------------------------
 
          //use aux data to update cells 
          if (aux_data.x>1e-10f)
              update_cell2(&alpha, &mixture, &weight3, aux_data, 2.5f, 0.09f, 0.03f);

          //OLD --------------------------------------------------------------------------------------
          //use aux data to update cells 
/*
          if (aux_data.x>1e-10f)
              update_cell(&data, aux_data, 2.5f, 0.09f, 0.03f);

          //reset the cells in memory 
          alpha_array[gid]      = max(alphamin,data.s0);
          float8 post_mix       = (float8) (data.s1, data.s2, data.s3, 
                                            data.s5, data.s6, data.s7, 
                                            data.s9, data.sa)*255.0;
          float4 post_nobs      = (float4) (data.s4, data.s8, data.sb, data.sc*100.0);
          mixture_array[gid]    = convert_uchar8_sat_rte(post_mix);
          nobs_array[gid]       = convert_ushort4_sat_rte(post_nobs);     
*/
          //old method --------------------------------------------------------------------------


          //reset the cells in memory 
          alpha_array[gid]      = max(alphamin, alpha);
          mixture_array[gid]    = convert_uchar8_sat_rte(mixture*255.0f);
          last_weight_array[gid]= convert_uchar_sat_rte(weight3*255.0f); 
      }
      cum_len_beta[gid] = (float2) 0.0f;
      mean_obs_cum_vis[gid] = (uchar2) 0;
    }
}


#if 0
__kernel
void
update_bit_scene_main(__global RenderSceneInfo  * info,
                      __global float            * alpha_array,
                      __global uchar8           * mixture_array,
                      __global ushort4          * nobs_array,
                      __global float2           * cum_len_beta,    // cumulative ray length and beta aux vars
                      __global uchar2           * mean_obs_cum_vis, // mean_obs per cell and cumulative visibility
                      __global float            * output)
{
    int gid=get_global_id(0);
    int datasize = info->data_len * info->num_buffer;
    if (gid<datasize)
    {
      //if alpha is less than zero don't update
      float  alpha    = alpha_array[gid];
      float  cell_min = info->block_len/(float)(1<<info->root_level);

      //minimum alpha value, don't let blocks get below this
      float  alphamin = -log(1.0-0.0001)/cell_min;

      if(alpha > 0.0) 
      {
          //load global data into registers
          //load aux data into local mem
          float2 cl_beta  = cum_len_beta[gid];
          float2 mean_vis = convert_float2(mean_obs_cum_vis[gid])/255.0f;
          float4 aux_data = (float4) (cl_beta.x,            //cell length
                                      mean_vis.x,           //mean observation
                                      cl_beta.y,            //beta (bayes update ratio)
                                      mean_vis.y);          //cell visability
          
          float4 nobs     = convert_float4(nobs_array[gid]);
          float8 mixture  = convert_float8(mixture_array[gid]);
          float16 data = (float16) (alpha, 
                         (mixture.s0/255.0), (mixture.s1/255.0), (mixture.s2/255.0), (nobs.s0), 
                         (mixture.s3/255.0), (mixture.s4/255.0), (mixture.s5/255.0), (nobs.s1),
                         (mixture.s6/255.0), (mixture.s7/255.0), (nobs.s2), (nobs.s3/100.0), 
                         0.0, 0.0, 0.0);
          
          //use aux data to update cells 
          if (aux_data.x>1e-10f)
              update_cell(&data, aux_data, 2.5f, 0.09f, 0.03f);

          //reset the cells in memory 
          alpha_array[gid]      = max(alphamin,data.s0);
          float8 post_mix       = (float8) (data.s1, data.s2, data.s3, 
                                            data.s5, data.s6, data.s7, 
                                            data.s9, data.sa)*255.0;
          float4 post_nobs      = (float4) (data.s4, data.s8, data.sb, data.sc*100.0);
          mixture_array[gid]    = convert_uchar8_sat_rte(post_mix);
          nobs_array[gid]       = convert_ushort4_sat_rte(post_nobs);     
      }
      cum_len_beta[gid] = (float2) 0.0f;
      mean_obs_cum_vis[gid] = (uchar2) 0;
    }
}
#endif
  
/*
 * Divides alpha by cell length (so we don't have to store cell length) 
 */
__kernel
void
divide_alpha( __global RenderSceneInfo  * info,
              __global float            * alpha_array,
              __global float2           * cum_len_beta,    // cumulative ray length and beta aux vars
              __global uchar2           * mean_obs_cum_vis, // mean_obs per cell and cumulative visibility              
              __global float            * output)
{ 
  int gid=get_global_id(0);
  int datasize = info->data_len * info->num_buffer;
  if (gid<datasize)
  {

    //if alpha is less than zero don't update
    float  alpha    = alpha_array[gid];
    float  cell_len = cum_len_beta[gid].x;
    if(alpha > 0.0 && cell_len > 10e-10f) {
       alpha_array[gid] = alpha/cell_len;     
    }
  } 
}

