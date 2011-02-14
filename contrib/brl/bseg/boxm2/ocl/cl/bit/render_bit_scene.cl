#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif
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



#if 1
void cast_ray(
          //---- RAY ARGUMENTS -------------------------------------------------
          int i, int j,                                     //pixel information
          float ray_ox, float ray_oy, float ray_oz,         //ray origin
          float ray_dx, float ray_dy, float ray_dz,         //ray direction

          //---- SCENE ARGUMENTS------------------------------------------------
          __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
          __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16
          __global    float              * alpha_array,     //voxel density buffer
          __global    int2               * mixture_array,   //appearance model buffer

          //---- UTILITY ARGUMENTS----------------------------------------------
          __local     uchar16            * local_tree,      //local tree for traversing
          __constant  uchar              * bit_lookup,      //0-255 num bits lookup table
          __local     uchar              * cumsum,          //cumulative sum helper for data pointer
          __local     int                * imIndex,         //image index

          //---- OUTPUT ARGUMENTS-----------------------------------------------
          __global    float              * in_image,       //input image
#ifdef CHANGE
          __global    uint              * in_exp_image,       //input image (Read only)
          __global    uint              * prob_exp_image,       //input image
#endif
          __global    uint               * exp_image,       //input image and store vis_inf and pre_inf
          __global    float              * vis_image,       //gl_image automatically rendered to the screen
          __global    float              * output)          //debug output buffer
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  // pixel values/depth map to be returned
  float vis   = vis_image[imIndex[llid]];
  uint  eint  = as_uint(exp_image[imIndex[llid]]);
  uchar echar = convert_uchar(eint);
  float expected_int = convert_float(echar)/255.0f;

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
  while (tblock < tfar && vis > .01f) 
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
    #ifdef RENDER
          step_cell_render(mixture_array, alpha_array, data_ptr, d, &vis, &expected_int);
    #endif
    ////////////////////////////////////////////////////////////////////////////////
    // END Step Cell Functor
    ////////////////////////////////////////////////////////////////////////////////
  }

#ifdef RENDER
  exp_image[imIndex[llid]] =  rgbaFloatToInt((float4) expected_int); //expected_int;
#endif

  //store visibility at the end of this block
  vis_image[imIndex[llid]] = vis;
}
#endif


#if 0 // Multi Loop render method
void cast_ray(
          //---- RAY ARGUMENTS -------------------------------------------------
          int i, int j,                                     //pixel information
          float ray_ox, float ray_oy, float ray_oz,         //ray origin
          float ray_dx, float ray_dy, float ray_dz,         //ray direction

          //---- SCENE ARGUMENTS------------------------------------------------
          __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
          __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16
          __global    float              * alpha_array,     //voxel density buffer
          __global    uchar8             * mixture_array,   //appearance model buffer

          //---- UTILITY ARGUMENTS----------------------------------------------
          __local     uchar16            * local_tree,      //local tree for traversing
          __constant  uchar              * bit_lookup,      //0-255 num bits lookup table
          __local     uchar              * cumsum,          //cumulative sum helper for data pointer
          __local     int                * imIndex,         //image index

          //---- OUTPUT ARGUMENTS-----------------------------------------------
          __global    float              * in_image,       //input image
#ifdef CHANGE
          __global    uint              * in_exp_image,       //input image (Read only)
          __global    uint              * prob_exp_image,       //input image
#endif
          __global    uint               * exp_image,       //input image and store vis_inf and pre_inf
          __global    float              * vis_image,       //gl_image automatically rendered to the screen
          __global    float              * output)          //debug output buffer
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  // pixel values/depth map to be returned
  float vis   = vis_image[imIndex[llid]];
  uint  eint  = as_uint(exp_image[imIndex[llid]]);
  uchar echar = convert_uchar(eint);
  float expected_int = convert_float(echar)/255.0f;

#ifdef CHANGE
  uint  in_eint  = as_uint(in_exp_image[imIndex[llid]]);
  uchar in_echar = convert_uchar(in_eint);
  float exp_intensity= convert_float(in_echar)/255.0f;
  float intensity=in_image[imIndex[llid]];
  uint  in_prob_eint  = as_uint(prob_exp_image[imIndex[llid]]);
  uchar in_prob_echar = convert_uchar(in_prob_eint);
  float expected_prob_int= convert_float(in_prob_echar)/255.0f;
#endif

  //determine the minimum face:
  //get parameters tnear and tfar for the scene
  float max_facex = (ray_dx > 0.0f) ? (linfo->dims.x) : 0.0f;
  float max_facey = (ray_dy > 0.0f) ? (linfo->dims.y) : 0.0f;
  float max_facez = (ray_dz > 0.0f) ? (linfo->dims.z) : 0.0f;
  //float tfar = min(min( (max_facex-ray_ox)*(1.0f/ray_dx), (max_facey-ray_oy)*(1.0f/ray_dy)), (max_facez-ray_oz)*(1.0f/ray_dz));
  float tfar = calc_tfar(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, max_facex, max_facey, max_facez); 
  float min_facex = (ray_dx < 0.0f) ? (linfo->dims.x) : 0.0f;
  float min_facey = (ray_dy < 0.0f) ? (linfo->dims.y) : 0.0f;
  float min_facez = (ray_dz < 0.0f) ? (linfo->dims.z) : 0.0f;
  //float tblock = max(max( (min_facex-ray_ox)*(1.0f/ray_dx), (min_facey-ray_oy)*(1.0f/ray_dy)), (min_facez-ray_oz)*(1.0f/ray_dz));
  float tblock = calc_tnear(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, min_facex, min_facey, min_facez); 
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
  while (tblock < tfar && vis > .01f)
  {
    //-------------------------------------------------------------------------
    // get small block and necessary information
    // Note: Can probably eliminate posx,posy,posz and replace with just a calc
    // of position based on ray_o or local ray o and the current t value
    //-------------------------------------------------------------------------
    //find entry point (adjusted) and the current block index
    float posx = calc_pos(tblock, ray_ox, ray_dx); //(ray_ox + (tblock + TREE_EPSILON)*ray_dx);
    float posy = calc_pos(tblock, ray_oy, ray_dy); //(ray_oy + (tblock + TREE_EPSILON)*ray_dy);
    float posz = calc_pos(tblock, ray_oz, ray_dz); //(ray_oz + (tblock + TREE_EPSILON)*ray_dz);

    //curr block index (var later used as cell_min), check to make sure block index isn't 192 or -1
    float cell_minx, cell_miny, cell_minz; 
    calc_cell_min( &cell_minx, &cell_miny, &cell_minz, posx, posy, posz, linfo->dims);

    //load current block/tree
    int blkIndex = calc_blkI(cell_minx, cell_miny, cell_minz, linfo->dims); 
    local_tree[llid] = as_uchar16(tree_array[blkIndex]);
    ushort buff_index = as_ushort((uchar2) (local_tree[llid].sd, local_tree[llid].sc));

    //initialize cumsum buffer and cumIndex
    cumsum[llid*10] = local_tree[llid].s0;
    int cumIndex = 1;
    //barrier(CLK_LOCAL_MEM_FENCE);

    //local ray origin is entry point (point should be in [0,1])
    //(note that cell_min is the current block index at this point)
    //setting local_ray_o to block_pos allows ttree to start at 0
    float lrayx = (posx - cell_minx);
    float lrayy = (posy - cell_miny);
    float lrayz = (posz - cell_minz);

    //get scene level t exit value.  check to make sure that the ray is progressing.
    //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
    float texit = calc_cell_exit(cell_minx, cell_miny, cell_minz, 1.0f, ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz); 
    if (texit <= tblock) break; //need this check to make sure the ray is progressing

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
      float t1 = calc_cell_exit(cell_minx, cell_miny, cell_minz, cell_len, lrayx, lrayy, lrayz, ray_dx, ray_dy, ray_dz);

      //make sure ray goes through the cell with positive seg length
      if (t1 <= ttree) break;

      //// distance must be multiplied by the dimension of the bounding box
      float d = (t1-ttree) * linfo->block_len;
      ttree = t1;

////////////////////////////////////////////////////////////////////////////////
// Step Cell Functor
////////////////////////////////////////////////////////////////////////////////
#ifdef RENDER
      step_cell_render(mixture_array, alpha_array, data_ptr, d, &vis, &expected_int);
#endif
#ifdef DEPTH
      step_cell_render_depth2((tblock+t1)*linfo->block_len, alpha_array, data_ptr, d, &vis, &expected_int);
#endif
#ifdef CHANGE
      step_cell_change_detection_uchar8_w_expected(mixture_array,alpha_array,data_ptr,d,&vis,&expected_int,&expected_prob_int,intensity,exp_intensity);
      //step_cell_change_detection_uchar8(mixture_array,alpha_array,data_ptr,d,&vis,&expected_int,intensity);
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

#ifdef RENDER
  exp_image[imIndex[llid]] =  rgbaFloatToInt((float4) expected_int); //expected_int;
#endif
#ifdef DEPTH
  expected_int += vis*tfar*linfo->block_len;
  exp_image[imIndex[llid]] =  rgbaFloatToInt((float4) expected_int);
#endif
#ifdef CHANGE
      exp_image[imIndex[llid]] =  rgbaFloatToInt((float4) 1.0f/(1.0f+expected_int)); //expected_int;
      prob_exp_image[imIndex[llid]] =  rgbaFloatToInt((float4) 1.0f/(1.0f+expected_prob_int)); //expected_int;
#endif

  //store visibility at the end of this block
  vis_image[imIndex[llid]] = vis;
}
#endif


#ifdef RENDER
__kernel
void
render_bit_scene( __constant  RenderSceneInfo    * linfo,
                  __global    int4               * tree_array,
                  __global    float              * alpha_array,
                  __global    int2               * mixture_array,
                  __global    float16            * camera,        // camera orign and SVD of inverse of camera matrix
                  __global    float              * exp_image,      // input image and store vis_inf and pre_inf
                  __global    uint4              * exp_image_dims,
                  __global    float              * output,
                  __constant  uchar              * bit_lookup,
                  __global    float              * vis_image,
                  __local     uchar16            * local_tree,
                  __local     uchar              * cumsum,        //cumulative sum helper for data pointer
                  __local     int                * imIndex)
{
  //----------------------------------------------------------------------------
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  imIndex[llid] = j*get_global_size(0)+i;

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*exp_image_dims).z || j>=(*exp_image_dims).w) {
    return;
  }

  //----------------------------------------------------------------------------
  // Calculate ray origin, and direction
  // (make sure ray direction is never axis aligned)
  //----------------------------------------------------------------------------
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
  ray_d.w = 0.0f; ray_d = normalize(ray_d);

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
            linfo, tree_array, alpha_array, mixture_array,

            //utility info
            local_tree, bit_lookup, cumsum,

            //RENDER SPECIFIC ARGS
            imIndex,

            0,  // input image
            //io info
            exp_image,
            vis_image,
            output);
}
#endif

#ifdef CHANGE
__kernel
void
change_detection_bit_scene( __constant  RenderSceneInfo    * linfo,
                            __global    int4               * tree_array,
                            __global    float              * alpha_array,
                            __global    uchar8             * mixture_array,
                            __global    float16            * camera,        // camera orign and SVD of inverse of camera matrix
                            __global    float              * in_image,      // input image and store vis_inf and pre_inf
                            __global    uint              * exp_image,      // input image and store vis_inf and pre_inf
                            __global    uint              * prob_exp_image,       //input image
                            __global    uint               * change_image,      // input image and store vis_inf and pre_inf
                            __global    uint4              * exp_image_dims,
                            __global    float              * output,
                            __constant  uchar              * bit_lookup,
                            __global    float              * vis_image,
                            __local     uchar16            * local_tree,
                            __local     uchar              * cumsum,        //cumulative sum helper for data pointer
                            __local     int                * imIndex)
{
  //----------------------------------------------------------------------------
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  imIndex[llid] = j*get_global_size(0)+i;

  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*exp_image_dims).z || j>=(*exp_image_dims).w) {
    //exp_image[imIndex[llid]] = 0.0f;
    return;
  }

  //----------------------------------------------------------------------------
  // Calculate ray origin, and direction
  // (make sure ray direction is never axis aligned)
  //----------------------------------------------------------------------------
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
  ray_d.w = 0.0f; ray_d = normalize(ray_d);

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
            linfo, tree_array, alpha_array, mixture_array,

            //utility info
            local_tree, bit_lookup, cumsum,

            //RENDER SPECIFIC ARGS
            imIndex,
            in_image,
            exp_image,
            prob_exp_image,//input image
            //io info
            change_image,
            vis_image,
            output);
}
#endif
