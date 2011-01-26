#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable

#define BLOCK_EPSILON .006125f
#define TREE_EPSILON  .005f

void
cast_ray( 
          //---- RAY ARGUMENTS -------------------------------------------------
          int i, int j,                                     //pixel information
          float ray_ox, float ray_oy, float ray_oz,         //ray origin
          float ray_dx, float ray_dy, float ray_dz,         //ray direction
           
          //---- SCENE ARGUMENTS------------------------------------------------
          __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
          __global    ushort2            * block_ptrs,      //3d array of blk pointers
          __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16
          __global    float              * alpha_array,     //voxel density buffer
          __global    uchar8             * mixture_array,   //appearance model buffer
          //__global    uchar              * last_weight_array,//third weight for mixture model
          __global    ushort4            * num_obs_array,   // num obs for each block
          
          __global    int                * seg_len_array,    //four aux data arrays
          __global    int                * mean_obs_array,
          __global    int                * vis_array,
          __global    int                * beta_array,
          //---- UTILITY ARGUMENTS----------------------------------------------
          __local     uchar16            * local_tree,      //local tree for traversing
          __constant  uchar              * bit_lookup,      //0-255 num bits lookup table
          __local     uchar              * cumsum,          //cumulative sum helper for data pointer
                      int                  factor,          //factor

          //---- SEPARATE ARGS FOR EACH STEP CELL FUNCTOR... -------------------
#if defined(CHANGE) || defined(RENDER)
          __local     int                * imIndex,         //image index
#endif
#ifdef SEGLEN
          __global    float16            * mat_cam, 
          __local     short2             * ray_bundle_array,//gives information for which ray takes over in the workgroup
          __local     int                * cell_ptrs,       //local list of cell_ptrs (cells that are hit by this workgroup
                      float                inImgObs,         //input image observation (no need for image_vect... )
                      float                ray_vis, 
          __local     float4             * cached_aux_data,  //local data per ray in workgroup
#endif
#ifdef PREINF  
          __global    float16            * mat_cam, 
                      float4               image_vect,      //input image and store vis_inf and pre_inf
#endif
#ifdef BAYES
          __global    float16            * mat_cam, 
          __local     short2             * ray_bundle_array,//gives information for which ray takes over in the workgroup
          __local     int                * cell_ptrs,       //local list of cell_ptrs (cells that are hit by this workgroup
          __local     float              * cached_vis,
                      float                norm,
                      float                ray_vis,
                      float                ray_pre,
#endif 

          //---- OUTPUT ARGUMENTS-----------------------------------------------
          __global    float4             * in_image,        //input image and store vis_inf and pre_inf
          __global    uint               * gl_image,        //gl_image automatically rendered to the screen
          __global    float              * output)          //debug output buffer
{
  
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  // pixel values/depth map to be returned
  float vis = 1.0f;
  float expected_int = 0.0f;
#ifdef CHANGE
  float intensity=in_image[imIndex[llid]].x;
  float e_intensity=in_image[imIndex[llid]].y;
  float e_expected_int=0.0f;
#endif

  //determine the minimum face:
  //get parameters tnear and tfar for the scene
  float max_facex = (ray_dx > 0.0f) ? (linfo->dims.x) : 0.0f;
  float max_facey = (ray_dy > 0.0f) ? (linfo->dims.y) : 0.0f;
  float max_facez = (ray_dz > 0.0f) ? (linfo->dims.z) : 0.0f;
  float tfar = min(min( (max_facex-ray_ox)*(1.0f/ray_dx), (max_facey-ray_oy)*(1.0f/ray_dy)), (max_facez-ray_oz)*(1.0f/ray_dz));
  float min_facex = (ray_dx < 0.0f) ? (linfo->dims.x) : 0.0f;
  float min_facey = (ray_dy < 0.0f) ? (linfo->dims.y) : 0.0f;
  float min_facez = (ray_dz < 0.0f) ? (linfo->dims.z) : 0.0f;
  float tblock = max(max( (min_facex-ray_ox)*(1.0f/ray_dx), (min_facey-ray_oy)*(1.0f/ray_dy)), (min_facez-ray_oz)*(1.0f/ray_dz));

  if (tfar <= tblock) {
#ifdef RENDER
    gl_image[imIndex[llid]] = rgbaFloatToInt((float4)(0.0f,0.0f,0.0f,0.0f));
    in_image[imIndex[llid]] = (float)0.0f;
#endif
    in_image[j*get_global_size(0)+i].x = 0.0f; 
    return; 
  }
  //make sure tnear is at least 0...
  tblock = (tblock > 0.0f) ? tblock : 0.0f;
  
  //make sure tfar is within the last block so texit surpasses it (and breaks from the outer loop)
  tfar -= BLOCK_EPSILON;   
  
  
  //used for depth map 
  //----------------------------------------------------------------------------
  // Begin traversing the blocks, break when any curr_block_index value is
  // illegal (not between 0 and scenedims)
  //----------------------------------------------------------------------------
  while(tblock < tfar && vis > .01f) 
  {
    //-------------------------------------------------------------------------
    // get small block and necessary information
    // Note: Can probably eliminate posx,posy,posz and replace with just a calc
    // of position based on ray_o or local ray o and the current t value
    //-------------------------------------------------------------------------
    //find entry point (adjusted) and the current block index
    float posx = (ray_ox + (tblock + TREE_EPSILON)*ray_dx);
    float posy = (ray_oy + (tblock + TREE_EPSILON)*ray_dy);
    float posz = (ray_oz + (tblock + TREE_EPSILON)*ray_dz);
    
    //curr block index (var later used as cell_min), check to make sure block index isn't 192 or -1
    float cell_minx = clamp(floor(posx), 0.0f, linfo->dims.x-1.0f);
    float cell_miny = clamp(floor(posy), 0.0f, linfo->dims.y-1.0f);
    float cell_minz = clamp(floor(posz), 0.0f, linfo->dims.z-1.0f);

    //load current block/tree 
    int blkIndex = convert_int(cell_minz + (cell_miny + cell_minx*linfo->dims.y)*linfo->dims.z); 
    ushort2 block = block_ptrs[blkIndex];                       
    int root_ptr = block.x * linfo->tree_len + block.y;         
    local_tree[llid] = as_uchar16(tree_array[root_ptr]);        
    
    //initialize cumsum buffer and cumIndex
    cumsum[llid*10] = local_tree[llid].s0;                     
    int cumIndex = 1;                                             
#ifndef CPU
    barrier(CLK_LOCAL_MEM_FENCE);                               
#endif

    //local ray origin is entry point (point should be in [0,1]) 
    //(note that cell_min is the current block index at this point)
    //setting local_ray_o to block_pos allows ttree to start at 0
    float lrayx = (posx - cell_minx);
    float lrayy = (posy - cell_miny);
    float lrayz = (posz - cell_minz);
    
    //get scene level t exit value.  check to make sure that the ray is progressing. 
    //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
    cell_minx = (ray_dx > 0) ? cell_minx+1.0f : cell_minx; 
    cell_miny = (ray_dy > 0) ? cell_miny+1.0f : cell_miny; 
    cell_minz = (ray_dz > 0) ? cell_minz+1.0f : cell_minz; 
    float texit = min(min( (cell_minx-ray_ox)*(1.0f/ray_dx), (cell_miny-ray_oy)*(1.0f/ray_dy)), (cell_minz-ray_oz)*(1.0f/ray_dz));
    if(texit <= tblock) break; //need this check to make sure the ray is progressing
        
    //ttree starts at 0, ttree_exit is t exit value in the tree level (scaled from scene level)
    texit = (texit - tblock - BLOCK_EPSILON);  
    float ttree = 0.0f;                 
    while (ttree < texit)              
    {
      // point in tree coordinates
      posx = (lrayx + (ttree + TREE_EPSILON)*ray_dx);  
      posy = (lrayy + (ttree + TREE_EPSILON)*ray_dy);   
      posz = (lrayz + (ttree + TREE_EPSILON)*ray_dz);   
      
      // traverse to leaf cell that contains the entry point, set bounding box
      ////data offset is ushort pointed to by tree + bit offset
      float cell_len;
      int data_ptr = traverse_three(&local_tree[llid], 
                                    posx,posy,posz, 
                                    &cell_minx, &cell_miny, &cell_minz, &cell_len); 
      data_ptr = data_index_cached(&local_tree[llid], data_ptr, bit_lookup, &cumsum[llid*10], &cumIndex, linfo->data_len);
      data_ptr = (block.x*linfo->data_len) + data_ptr;  
      
      // check to see how close tnear and tfar are
      cell_minx = (ray_dx > 0.0f) ? cell_minx+cell_len : cell_minx; 
      cell_miny = (ray_dy > 0.0f) ? cell_miny+cell_len : cell_miny; 
      cell_minz = (ray_dz > 0.0f) ? cell_minz+cell_len : cell_minz;
      float t1 = min(min( (cell_minx-lrayx)*(1.0f/ray_dx), (cell_miny-lrayy)*(1.0f/ray_dy)), (cell_minz-lrayz)*(1.0f/ray_dz));
  
      //make sure ray goes through the cell with positive seg length
      if(t1 <= ttree) break;

      //// distance must be multiplied by the dimension of the bounding box
      float d = (t1-ttree) * linfo->block_len;
      ttree = t1;

////////////////////////////////////////////////////////////////////////////////
// Step Cell Functor
////////////////////////////////////////////////////////////////////////////////
#ifdef RENDER
      step_cell_render(mixture_array, alpha_array, data_ptr, d, &vis, &expected_int);
#endif
#ifdef CHANGE
      step_cell_change_detection_uchar8_w_expected(mixture_array,alpha_array,data_ptr,d,&vis,&expected_int,&e_expected_int,intensity,e_intensity);
#endif
#ifdef SEGLEN

  #ifdef ATOMIC_OPT
      //SLOW and accurate method
      int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
      atom_add(&seg_len_array[data_ptr], seg_int);  
      int cum_obs = convert_int_rte(d * inImgObs * SEGLEN_FACTOR); 
      atom_add(&mean_obs_array[data_ptr], cum_obs);
  #else
      // --------- faster and less accurate method... --------------------------
      //keep track of cells being hit
      cell_ptrs[llid] = data_ptr;
      cached_aux_data[llid] = (float4)0.0f;  //leaders retain the mean obs and the cell length
      barrier(CLK_LOCAL_MEM_FENCE);

      //segment workgroup
      load_data_mutable_opt(ray_bundle_array,cell_ptrs);
      
      //back to normal mean of mean obs... 
      seg_len_obs_functor(d, inImgObs, ray_bundle_array, cached_aux_data);
      barrier(CLK_LOCAL_MEM_FENCE);                
      
      //set aux data here (for each leader.. )
      if(ray_bundle_array[llid].y==1) 
      {
        //scale!
        int seg_int = convert_int_rte(cached_aux_data[llid].x * SEGLEN_FACTOR); 
        int cum_obs = convert_int_rte(cached_aux_data[llid].y * SEGLEN_FACTOR);  
        
        //atomically update the cells
        atom_add(&seg_len_array[data_ptr], seg_int); 
        atom_add(&mean_obs_array[data_ptr], cum_obs);
      }
      //------------------------------------------------------------------------*/
  #endif
      //reset cell_ptrs to negative one every time (prevents invisible layer bug)
      cell_ptrs[llid] = -1;
#endif
#ifdef PREINF
      
      //keep track of cells being hit
      //cell data, i.e., alpha and app model is needed for some passes 
      float  alpha    = alpha_array[data_ptr];
      float8 mixture  = convert_float8(mixture_array[data_ptr])/255.0f;
      float  weight3  = (1.0f-mixture.s2-mixture.s5);
      
      float cum_len  = convert_float(seg_len_array[data_ptr])/SEGLEN_FACTOR; 
      float mean_obs = convert_float(mean_obs_array[data_ptr])/SEGLEN_FACTOR;
      mean_obs = mean_obs/cum_len;
     
      //calculate pre_infinity denomanator (shape of image)
      pre_infinity_opt(d, cum_len, mean_obs, &image_vect, alpha, mixture, weight3);
      
      //aux data doesn't need to be set, just in_image
      in_image[j*get_global_size(0)+i] = image_vect;
#endif
#ifdef BAYES

  #ifdef ATOMIC_OPT
      //slow beta calculation ----------------------------------------------------
      float  alpha    = alpha_array[data_ptr];
      float8 mixture  = convert_float8(mixture_array[data_ptr])/255.0f;
      float weight3   = (1.0f-mixture.s2-mixture.s5);
      
      //load aux data
      float cum_len  = convert_float(seg_len_array[data_ptr])/SEGLEN_FACTOR; 
      float mean_obs = convert_float(mean_obs_array[data_ptr])/SEGLEN_FACTOR;
      mean_obs = mean_obs/cum_len;
      
      float ray_beta, vis_cont; 
      bayes_ratio_ind( d, 
                       alpha,
                       mixture, 
                       weight3, 
                       cum_len, 
                       mean_obs, 
                       norm,
                       &ray_pre, 
                       &ray_vis, 
                       &ray_beta, 
                       &vis_cont); 
    
      //discretize and store beta and vis contribution
      int beta_int = convert_int_rte(ray_beta * SEGLEN_FACTOR);
      atom_add(&beta_array[data_ptr], beta_int);  
      int vis_int  = convert_int_rte(vis_cont * SEGLEN_FACTOR); 
      atom_add(&vis_array[data_ptr], vis_int);         
      //--------------------------------------------------------------------------          
  #else
      //keep track of cells being hit
      cell_ptrs[llid] = data_ptr;
      barrier(CLK_LOCAL_MEM_FENCE);
      load_data_mutable_opt(ray_bundle_array, cell_ptrs);
      
      //if this current thread is a segment leader...
      //cell data, i.e., alpha and app model is needed for some passes 
      float  alpha    = alpha_array[data_ptr];
      float8 mixture  = convert_float8(mixture_array[data_ptr])/255.0f;
      float weight3   = (1.0f-mixture.s2-mixture.s5);
      
      //load aux data
      float cum_len  = convert_float(seg_len_array[data_ptr])/SEGLEN_FACTOR; 
      float mean_obs = convert_float(mean_obs_array[data_ptr])/SEGLEN_FACTOR;
      mean_obs = mean_obs/cum_len;
      float cell_beta = 0.0f;
      float cell_vis  = 0.0f;
      barrier(CLK_LOCAL_MEM_FENCE);

      //calculate bayes ratio
      bayes_ratio_functor( d, 
                          mean_obs,
                          &ray_pre,
                          &ray_vis,
                          norm, 
                          &cell_beta,
                          &cell_vis,
                          ray_bundle_array,
                          cell_ptrs, 
                          cached_vis,
                          alpha, 
                          mixture, 
                          weight3);
          
    //set aux data here (for each leader.. )
    if(ray_bundle_array[llid].y==1) 
    {
      int beta_int = convert_int_rte(cell_beta * SEGLEN_FACTOR);
      atom_add(&beta_array[data_ptr], beta_int);  
      int vis_int  = convert_int_rte(cell_vis * SEGLEN_FACTOR); 
      atom_add(&vis_array[data_ptr], vis_int);
    } 
  #endif

    //reset cell_ptrs to -1 every time
    cell_ptrs[llid] = -1;
#endif 
////////////////////////////////////////////////////////////////////////////////
// END Step Cell Functor
////////////////////////////////////////////////////////////////////////////////
    
    }
    //--------------------------------------------------------------------------
    // finding the next block (using exit point already found before tree loop)
    //--------------------------------------------------------------------------
    //scale texit back up
    texit = texit + tblock + BLOCK_EPSILON;
    tblock = texit;
  }

  
#if defined(CHANGE) 
#if defined(CHANGE_OLD)
  expected_int/=(1-vis);
  float fgbelief=1.0/(1.0+expected_int);
  uchar4 out;
  out.x=convert_uchar(fgbelief*255.0);
  float alpha=(1/(1+(3*fgbelief)));
  out.y=convert_uchar(intensity*alpha*255.0);
  out.z=0;
  out.w=convert_uchar(255);
  in_image[imIndex[llid]].w=fgbelief;
  gl_image[imIndex[llid]] = as_uint(out);
#else
  expected_int/=(1-vis);
  e_expected_int/=(1-vis);
  float pm=e_expected_int/(e_expected_int+1)-0.5*min(e_expected_int,1/e_expected_int);
  float fgbelief=1/(1+expected_int)-0.5*min(expected_int,1/expected_int);
  //// Bayesian
  uchar4 out;
  out.x=convert_uchar(pm*fgbelief*255.0);
  float alpha=(1/(1+(3*pm*fgbelief)));
  out.y=convert_uchar(alpha*intensity*255.0);
  out.z=0;
  out.w=convert_uchar(255);
  in_image[imIndex[llid]].w=fgbelief;
  gl_image[imIndex[llid]] = as_uint(out);
#endif
#endif
#ifdef RENDER
  //expected_int += vis*0.5f;

  gl_image[imIndex[llid]] = rgbaFloatToInt((float4) expected_int);
  in_image[imIndex[llid]] = (float4) expected_int;
#endif
  

}
