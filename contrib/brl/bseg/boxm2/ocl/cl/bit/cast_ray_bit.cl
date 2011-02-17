////////////////////////////////////////////////////////////////////////////////
// Cast Ray Bit
// Main cast ray function for fixed grid of shallow octree ray tracing. 
// 
// Note: This is a bare-bones skeleton for ray tracing.  It is necessary to 
// define two preprocessor variables for this to do anything useful:
//    - AUX_ARGS : list of additional arguments to to this function (for instance data arrays)
//    - STEP_CELL : function and signature to be substituted in at the voxel level
//
// This function also depends on 'bit_tree_library_functions.cl'
////////////////////////////////////////////////////////////////////////////////

//example defines (passed in on C++ side)
//#define STEP_CELL step_cell_render(aux_args.mixture_array,aux_args.alpha_array,data_ptr,d,vis,aux_args.expected_int);
#define BLOCK_EPSILON .006125f
#define TREE_EPSILON  .005f

////////////////////////////////////////////////////////////////////////////////
//Helper methods (that will hopefully, one day, become vector ops) 
////////////////////////////////////////////////////////////////////////////////
inline float calc_pos(float t, float ray_o, float ray_d)
{
  return (ray_o + (t + TREE_EPSILON)*ray_d);
}

inline float calc_tfar( float ray_ox, float ray_oy, float ray_oz,
                        float ray_dx, float ray_dy, float ray_dz,
                        float max_facex, float max_facey, float max_facez)
{
  return min(min( (max_facex-ray_ox)*(1.0f/ray_dx), (max_facey-ray_oy)*(1.0f/ray_dy)), (max_facez-ray_oz)*(1.0f/ray_dz));
}                
inline float calc_tnear(float ray_ox, float ray_oy, float ray_oz,
                        float ray_dx, float ray_dy, float ray_dz,
                        float min_facex, float min_facey, float min_facez)
{
  return max(max( (min_facex-ray_ox)*(1.0f/ray_dx), (min_facey-ray_oy)*(1.0f/ray_dy)), (min_facez-ray_oz)*(1.0f/ray_dz));
}

//requries float position
inline void calc_cell_min( float* cell_minx, float* cell_miny, float* cell_minz, 
                    float posx, float posy, float posz, int4 dims)
{
  (*cell_minx) = clamp(floor(posx), 0.0f, dims.x-1.0f);
  (*cell_miny) = clamp(floor(posy), 0.0f, dims.y-1.0f);
  (*cell_minz) = clamp(floor(posz), 0.0f, dims.z-1.0f);
}

//requires cell_minx
inline int calc_blkI(float cell_minx, float cell_miny, float cell_minz, int4 dims)
{
  return convert_int(cell_minz + (cell_miny + cell_minx*dims.y)*dims.z);
}

//calc subblock exit point parameter (t) given cell_min and cell_length
inline float calc_cell_exit(float cell_minx, float cell_miny, float cell_minz, float cell_len, 
                     float ray_ox, float ray_oy, float ray_oz,
                     float ray_dx, float ray_dy, float ray_dz)
{
  //calculate the exit along the sub block exit point (t_subblock_exit)
  cell_minx = (ray_dx > 0) ? cell_minx+cell_len : cell_minx;
  cell_miny = (ray_dy > 0) ? cell_miny+cell_len : cell_miny;
  cell_minz = (ray_dz > 0) ? cell_minz+cell_len : cell_minz;
  return calc_tfar(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, cell_minx, cell_miny, cell_minz);   
}
////////////////////////////////////////////////////////////////////////////////
// END Helper methods 
////////////////////////////////////////////////////////////////////////////////

void cast_ray(
          //---- RAY ARGUMENTS -------------------------------------------------
          int i, int j,                                     //pixel information
          float ray_ox, float ray_oy, float ray_oz,         //ray origin
          float ray_dx, float ray_dy, float ray_dz,         //ray direction

          //---- SCENE ARGUMENTS------------------------------------------------
          __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
          __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16
          
          //---- UTILITY ARGUMENTS----------------------------------------------
          __local     uchar16            * local_tree,      //local tree for traversing
          __constant  uchar              * bit_lookup,      //0-255 num bits lookup table
          __local     uchar              * cumsum,          //cumulative sum helper for data pointer
                      float              * vis,             //passed in as starting visibility

          //----aux arguments defined by host at compile time-------------------
          AuxArgs aux_args )
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  //determine the minimum face:
  //get parameters tnear and tfar for the scene
  float max_facex = (ray_dx > 0.0f) ? (linfo->dims.x) : 0.0f;
  float max_facey = (ray_dy > 0.0f) ? (linfo->dims.y) : 0.0f;
  float max_facez = (ray_dz > 0.0f) ? (linfo->dims.z) : 0.0f;
  float tfar = calc_tfar(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, max_facex, max_facey, max_facez); 
  float min_facex = (ray_dx < 0.0f) ? (linfo->dims.x) : 0.0f;
  float min_facey = (ray_dy < 0.0f) ? (linfo->dims.y) : 0.0f;
  float min_facez = (ray_dz < 0.0f) ? (linfo->dims.z) : 0.0f;
  float tblock = calc_tnear(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, min_facex, min_facey, min_facez); 
  tblock = (tblock > 0.0f) ? tblock : 0.0f;    //make sure tnear is at least 0...
  tfar -= BLOCK_EPSILON;   //make sure tfar is within the last block so texit surpasses it (and breaks from the outer loop)
  if (tfar <= tblock) 
    return;
  
  //calculate entry point here, entry sub_block, and its index
  min_facex = calc_pos(tblock, ray_ox, ray_dx); //(ray_ox + (tblock + TREE_EPSILON)*ray_dx);
  min_facey = calc_pos(tblock, ray_oy, ray_dy); //(ray_oy + (tblock + TREE_EPSILON)*ray_dy);
  min_facez = calc_pos(tblock, ray_oz, ray_dz); //(ray_oz + (tblock + TREE_EPSILON)*ray_dz);
  
  //curr block index (var later used as cell_min), check to make sure block index isn't 192 or -1
  float cell_minx, cell_miny, cell_minz; 
  calc_cell_min( &cell_minx, &cell_miny, &cell_minz, min_facex, min_facey, min_facez, linfo->dims);

  //load current block/tree
  int blkIndex = calc_blkI(cell_minx, cell_miny, cell_minz, linfo->dims); 
  local_tree[llid] = as_uchar16(tree_array[blkIndex]);
  ushort buff_index = as_ushort((uchar2) (local_tree[llid].sd, local_tree[llid].sc));

  //initialize cumsum buffer and cumIndex
  cumsum[llid*10] = local_tree[llid].s0;
  int cumIndex = 1;
   
  //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
  float texit = calc_cell_exit(cell_minx, cell_miny, cell_minz, 1.0f, ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz); 
  
  //----------------------------------------------------------------------------
  // Begin traversing the blocks, break when any curr_block_index value is
  // illegal (not between 0 and scenedims)
  //----------------------------------------------------------------------------
  while (tblock < tfar && (*vis) > .01f) 
  {
    //-------------------------------------------------------------------------
    //find entry point (adjusted) and the current block index
    float posx = calc_pos(tblock, ray_ox, ray_dx); //(ray_ox + (tblock + TREE_EPSILON)*ray_dx);
    float posy = calc_pos(tblock, ray_oy, ray_dy); //(ray_oy + (tblock + TREE_EPSILON)*ray_dy);
    float posz = calc_pos(tblock, ray_oz, ray_dz); //(ray_oz + (tblock + TREE_EPSILON)*ray_dz);
    if(tblock >= texit) {

      //curr block index (var later used as cell_min), check to make sure block index isn't 192 or -1
      calc_cell_min( &cell_minx, &cell_miny, &cell_minz, posx, posy, posz, linfo->dims);

      //load current block/tree
      blkIndex = calc_blkI(cell_minx, cell_miny, cell_minz, linfo->dims); 
      local_tree[llid] = as_uchar16(tree_array[blkIndex]);
      buff_index = as_ushort((uchar2) (local_tree[llid].sd, local_tree[llid].sc));

      //initialize cumsum buffer and cumIndex
      cumsum[llid*10] = local_tree[llid].s0;
      cumIndex = 1;

      //get scene level t exit value.  check to make sure that the ray is progressing.
      //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
      texit = calc_cell_exit(cell_minx, cell_miny, cell_minz, 1.0f, ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz); 
    }

    // traverse to leaf cell that contains the entry point, set bounding box
    float vox_len=1.0f; 
    float vox_minx, vox_miny, vox_minz;
    int data_ptr = traverse_three(&local_tree[llid],
                                  posx-cell_minx, posy-cell_miny, posz-cell_minz,
                                  &vox_minx, &vox_miny, &vox_minz, &vox_len);
    data_ptr = data_index_cached(&local_tree[llid], data_ptr, bit_lookup, &cumsum[llid*10], &cumIndex, linfo->data_len);
    data_ptr = (buff_index*linfo->data_len) + data_ptr;

    // get texit along the voxel
    float t_vox_exit = calc_cell_exit(vox_minx+cell_minx, vox_miny+cell_miny, vox_minz+cell_minz, vox_len, 
                                      ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz); 
                                    
    //make sure ray goes through the cell with positive seg length (or push it along)
    if (t_vox_exit <= tblock) break;

    //// distance must be multiplied by the dimension of the bounding box
    float d = (t_vox_exit - tblock) * linfo->block_len;
    tblock = t_vox_exit;

    ////////////////////////////////////////////////////////////////////////////////
    // Step Cell Functor
    ////////////////////////////////////////////////////////////////////////////////
    //step_cell_render(aux_args.mixture_array, aux_args.alpha_array, data_ptr, d, vis, aux_args.expected_int);
    STEP_CELL;
    
#ifdef SEGLEN
  #ifdef ATOMIC_OPT
      // --------- faster and less accurate method... --------------------------
      //keep track of cells being hit
      aux_args.cell_ptrs[llid] = data_ptr;
      aux_args.cached_aux[llid] = (float4) 0.0f;  //leaders retain the mean obs and the cell length
      barrier(CLK_LOCAL_MEM_FENCE);

      //segment workgroup
      load_data_mutable_opt(aux_args.ray_bundle_array,aux_args.cell_ptrs);
      
      //back to normal mean of mean obs... 
      seg_len_obs_functor(d, aux_args.obs, aux_args.ray_bundle_array, aux_args.cached_aux);
      barrier(CLK_LOCAL_MEM_FENCE);                
      
      //set aux data here (for each leader.. )
      if(aux_args.ray_bundle_array[llid].y==1) 
      {
        //scale!
        int seg_int = convert_int_rte(aux_args.cached_aux[llid].x * SEGLEN_FACTOR); 
        int cum_obs = convert_int_rte(aux_args.cached_aux[llid].y * SEGLEN_FACTOR);  
        
        //atomically update the cells
        atom_add(&aux_args.seg_len[data_ptr], seg_int); 
        atom_add(&aux_args.mean_obs[data_ptr], cum_obs);
      }
      //------------------------------------------------------------------------ 
  #else
      //SLOW and accurate method
      int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
      atom_add(&aux_args.seg_len[data_ptr], seg_int);  
      int cum_obs = convert_int_rte(d * aux_args.obs * SEGLEN_FACTOR); 
      atom_add(&aux_args.mean_obs[data_ptr], cum_obs);
  #endif
  
      //reset cell_ptrs to negative one every time (prevents invisible layer bug)
      aux_args.cell_ptrs[llid] = -1;
#endif
#ifdef PREINF
      
      //keep track of cells being hit
      //cell data, i.e., alpha and app model is needed for some passes 
      float  alpha    = aux_args.alpha[data_ptr];
      float8 mixture  = convert_float8(aux_args.mog[data_ptr])/255.0f;
      float  weight3  = (1.0f-mixture.s2-mixture.s5);
      
      float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR; 
      float mean_obs = convert_float(aux_args.mean_obs[data_ptr])/SEGLEN_FACTOR;
      mean_obs = mean_obs/cum_len;
     
      //calculate pre_infinity denomanator (shape of image)
      pre_infinity_opt(d, cum_len, mean_obs, aux_args.inImage, alpha, mixture, weight3);
      
#endif
#ifdef BAYES
  #ifdef ATOMIC_OPT
      //keep track of cells being hit
      aux_args.cell_ptrs[llid] = data_ptr;
      barrier(CLK_LOCAL_MEM_FENCE);
      load_data_mutable_opt(aux_args.ray_bundle_array, aux_args.cell_ptrs);
      
      //if this current thread is a segment leader...
      //cell data, i.e., alpha and app model is needed for some passes 
      float  alpha    = aux_args.alpha[data_ptr];
      float8 mixture  = convert_float8(aux_args.mog[data_ptr])/255.0f;
      float weight3   = (1.0f-mixture.s2-mixture.s5);
      
      //load aux data
      float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR; 
      float mean_obs = convert_float(aux_args.mean_obs[data_ptr])/SEGLEN_FACTOR;
      mean_obs = mean_obs/cum_len;
      float cell_beta = 0.0f;
      float cell_vis  = 0.0f;
      barrier(CLK_LOCAL_MEM_FENCE);

      //calculate bayes ratio
      bayes_ratio_functor( d, 
                          mean_obs,
                          aux_args.ray_pre,
                          aux_args.ray_vis,
                          aux_args.norm, 
                          &cell_beta,
                          &cell_vis,
                          aux_args.ray_bundle_array,
                          aux_args.cell_ptrs, 
                          aux_args.cached_vis,
                          alpha, 
                          mixture, 
                          weight3);
          
    //set aux data here (for each leader.. )
    if(aux_args.ray_bundle_array[llid].y==1) 
    {
      int beta_int = convert_int_rte(cell_beta * SEGLEN_FACTOR);
      atom_add(&aux_args.beta_array[data_ptr], beta_int);  
      int vis_int  = convert_int_rte(cell_vis * SEGLEN_FACTOR); 
      atom_add(&aux_args.vis_array[data_ptr], vis_int);
    } 
  #else
    //slow beta calculation ----------------------------------------------------
    float  alpha    = aux_args.alpha[data_ptr];
    float8 mixture  = convert_float8(aux_args.mog[data_ptr])/255.0f;
    float weight3   = (1.0f-mixture.s2-mixture.s5);
    
    //load aux data
    float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR; 
    float mean_obs = convert_float(aux_args.mean_obs[data_ptr])/SEGLEN_FACTOR;
    mean_obs = mean_obs/cum_len;
    
    float ray_beta, vis_cont; 
    bayes_ratio_ind( d, 
                     alpha,
                     mixture, 
                     weight3, 
                     cum_len, 
                     mean_obs, 
                     aux_args.norm,
                     aux_args.ray_pre, 
                     aux_args.ray_vis, 
                     &ray_beta, 
                     &vis_cont); 
  
    //discretize and store beta and vis contribution
    int beta_int = convert_int_rte(ray_beta * SEGLEN_FACTOR);
    atom_add(&aux_args.beta_array[data_ptr], beta_int);  
    int vis_int  = convert_int_rte(vis_cont * SEGLEN_FACTOR); 
    atom_add(&aux_args.vis_array[data_ptr], vis_int);         
    //-------------------------------------------------------------------------- */          
  #endif

    //reset cell_ptrs to -1 every time
    aux_args.cell_ptrs[llid] = -1;
#endif 
    ////////////////////////////////////////////////////////////////////////////////
    // END Step Cell Functor
    ////////////////////////////////////////////////////////////////////////////////
  }
}



#if 0

//: CAST RAY USED FOR UPDATE - no heuristic cutoffs
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
          __global    int4               * tree_array,      //3d array of octrees (loaded as int4, but read as uchar16)
          __global    float              * alpha_array,     //voxel density buffer
          __global    uchar8             * mixture_array,   //appearance model buffer
          __global    ushort4            * num_obs_array,   // num obs for each block
          
          // Aux data turned into one array
          __global    int                * seg_len_array,    //four aux data arrays
          __global    int                * mean_obs_array,
          __global    int                * vis_array,
          __global    int                * beta_array,
          
          //---- UTILITY ARGUMENTS----------------------------------------------
          __local     uchar16            * local_tree,      //local tree for traversing
          __constant  uchar              * bit_lookup,      //0-255 num bits lookup table
          __local     uchar              * cumsum,          //cumulative sum helper for data pointer

          //---- SEPARATE ARGS FOR EACH STEP CELL FUNCTOR... -------------------
#ifdef SEGLEN
          __local     short2             * ray_bundle_array,  //gives information for which ray takes over in the workgroup
          __local     int                * cell_ptrs,         //local list of cell_ptrs (cells that are hit by this workgroup
                      float                inImgObs,          //input image observation (no need for image_vect... )
                      float                ray_vis, 
          __local     float4             * cached_aux_data,   //local data per ray in workgroup
#endif
#ifdef PREINF  
                      float4             * image_vect,      //input image and store vis_inf and pre_inf
#endif
#ifdef BAYES
          __local     short2             * ray_bundle_array,//gives information for which ray takes over in the workgroup
          __local     int                * cell_ptrs,       //local list of cell_ptrs (cells that are hit by this workgroup
          __local     float              * cached_vis,
                      float                norm,
                      float              * ray_vis,
                      float              * ray_pre,
#endif 
          //---- OUTPUT ARGUMENTS-----------------------------------------------
          __global    float              * output)          //debug output buffer
{
  
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

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
  while(tblock < tfar) 
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
    local_tree[llid] = as_uchar16(tree_array[blkIndex]); 
    ushort buff_index = as_ushort((uchar2) (local_tree[llid].sd, local_tree[llid].sc));
    
    //initialize cumsum buffer and cumIndex
    cumsum[llid*10] = local_tree[llid].s0;                     
    int cumIndex = 1;                                         
    barrier(CLK_LOCAL_MEM_FENCE);                                       

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
      data_ptr = (buff_index*linfo->data_len) + data_ptr;  
      
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
#ifdef SEGLEN
  #ifdef ATOMIC_OPT
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
      //------------------------------------------------------------------------ 
  #else
      //SLOW and accurate method
      int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
      atom_add(&seg_len_array[data_ptr], seg_int);  
      int cum_obs = convert_int_rte(d * inImgObs * SEGLEN_FACTOR); 
      atom_add(&mean_obs_array[data_ptr], cum_obs);
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
      pre_infinity_opt(d, cum_len, mean_obs, image_vect, alpha, mixture, weight3);
      
#endif
#ifdef BAYES
  #ifdef ATOMIC_OPT
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
                          ray_pre,
                          ray_vis,
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
  #else
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
                     ray_pre, 
                     ray_vis, 
                     &ray_beta, 
                     &vis_cont); 
  
    //discretize and store beta and vis contribution
    int beta_int = convert_int_rte(ray_beta * SEGLEN_FACTOR);
    atom_add(&beta_array[data_ptr], beta_int);  
    int vis_int  = convert_int_rte(vis_cont * SEGLEN_FACTOR); 
    atom_add(&vis_array[data_ptr], vis_int);         
    //-------------------------------------------------------------------------- */          
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
  
}


#endif
