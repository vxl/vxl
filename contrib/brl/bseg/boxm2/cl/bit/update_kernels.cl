//THIS IS UPDATE BIT SCENE OPT
//Created Sept 30, 2010,
//Implements the parallel work group segmentation algorithm.

#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

#ifdef SEGLEN
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
  // check to see if the thread corresponds to an actual pixel as in some 

  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w) {
    return;
  }

  float4 inImage = in_image[j*get_global_size(0) + i];
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

            //scene info                              
            linfo, 
            tree_array, 
            alpha_array, 
            0,                                                //mixture_array
            num_obs_array,                                    //num_obs_arrya
            aux_array,                                        //seg_len_array, 
            &aux_array[linfo->num_buffer * linfo->data_len],   //mean_obs_array,
            0,                //vis_array
            0,                //beta_array

            //utility info
            local_tree, bit_lookup, cumsum,

            //SEGLEN SPECIFIC ARGS
            //factor,raybund,ptrs,cache,cache,image_vect (all NULL)
            ray_bundle_array, cell_ptrs, obs, vis, cached_aux_data,

            //io info
            output);
}
#endif

#ifdef PREINF
__kernel
void
pre_inf_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    float              * alpha_array,      // alpha for each block
             __global    uchar8             * mixture_array,    // mixture for each block
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
  if (i>=(*imgdims).z || j>=(*imgdims).w) {
    return;
  }
  float4 inImage = in_image[j*get_global_size(0) + i];
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

            //scene info                               
            linfo, 
            tree_array, 
            alpha_array, 
            mixture_array,  //mixture_array
            num_obs_array,  //num_obs_arrya
            
            aux_array,                                        //seg_len_array, 
            &aux_array[linfo->num_buffer * linfo->data_len],   //mean_obs_array,
            &aux_array[2 * linfo->num_buffer * linfo->data_len],   //vis_array,
            &aux_array[3 * linfo->num_buffer * linfo->data_len],   //beta_array,

            //utility info
            local_tree, bit_lookup, cumsum,

            //PREINF SPECIFIC ARGS
            &inImage,

            //io info
            output);
  
  //store the vis_inf/pre_inf in the image          
  in_image[j*get_global_size(0)+i] = inImage;
}
#endif


#ifdef BAYES
__kernel
void
bayes_main(__constant  RenderSceneInfo    * linfo,
           __global    int4               * tree_array,       // tree structure for each block
           __global    float              * alpha_array,      // alpha for each block
           __global    uchar8             * mixture_array,    // mixture for each block
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
  if (i>=(*imgdims).z || j>=(*imgdims).w) {
    return;
  }
  float4 inImage = in_image[j*get_global_size(0) + i];
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

            //scene info       
            linfo, 
            tree_array, 
            alpha_array, 
            mixture_array,  //mixture_array
            num_obs_array,  //num_obs_arrya
            
            aux_array,                                        //seg_len_array, 
            &aux_array[linfo->num_buffer * linfo->data_len],   //mean_obs_array,
            &aux_array[2 * linfo->num_buffer * linfo->data_len],   //vis_array,
            &aux_array[3 * linfo->num_buffer * linfo->data_len],   //beta_array,
            
            //utility info
            local_tree, bit_lookup, cumsum,

            //BAYES SPECIFIC ARGUMENTS
            //factor,raybund,ptrs,cache,cache,image_vect (all NULL)
            ray_bundle_array, cell_ptrs, cached_vis, norm, &vis, &pre,

            //io info
            output);
            
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
  int lgid = get_global_id(0) + get_global_size(0)*get_global_id(1);

  int i=0;
  int j=0;
  map_work_space_2d(&i,&j);
  i=get_global_id(0);
  j=get_global_id(1);

  if (i>=(*imgdims).z && j>=(*imgdims).w)
    return;

  if (i>=(*imgdims).z && j>=(*imgdims).w)
    return;

  float4 vect = image[j*get_global_size(0)+i];
  float mult =
#if 0
               (p_inf[0].x>0.0f) ? 1.0f :
               gauss_prob_density(vect.x, p_inf[0].y, p_inf[0].z);
#else
               0.5f;
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
                      __global uchar8           * mixture_array,
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
      float8 mixture  = convert_float8(mixture_array[gid]);
      float16 data = (float16) (alpha,
                     (mixture.s0/255.0), (mixture.s1/255.0), (mixture.s2/255.0), (nobs.s0),
                     (mixture.s3/255.0), (mixture.s4/255.0), (mixture.s5/255.0), (nobs.s1),
                     (mixture.s6/255.0), (mixture.s7/255.0), (nobs.s2), (nobs.s3/100.0),
                     0.0, 0.0, 0.0);

      //use aux data to update cells
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
    
    //clear out aux data
    aux_array[gid] = 0; 
    aux_array[gid + datasize] = 0; 
    aux_array[gid + 2*datasize] = 0;
    aux_array[gid + 3*datasize] = 0;
  }
  
}
