#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif

__kernel
void
ray_trace_bit_scene_opt(__global   RenderSceneInfo    * info,
                        __local    RenderSceneInfo    * linfo,
                        __global   ushort2            * block_ptrs,
                        __global   uchar16            * tree_array,
                        __global   float              * alpha_array,
                        __global   uchar8             * mixture_array,
                        __global   float16            * persp_cam, // camera orign and SVD of inverse of camera matrix
                        __global   uint4              * imgdims,   // dimensions of the image
                        __local    uchar              * local_tree,
                        __local    float16            * local_copy_cam,
                        __local    uint4              * local_copy_imgdims,
                        __global   float              * in_image,
                        __global   uint               * gl_image, 
                        //__global   float4             * output,
                        __constant uchar              * bit_lookup)    // input image and store vis_inf and pre_inf
{
  //get local id (0-63 for an 8x8) of this patch 
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  
  //----------------------------------------------------------------------------
  // get image coordinates and camera, 
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  int i=0,j=0;  map_work_space_2d(&i,&j);
  int imIndex = j*get_global_size(0)+i; 

  //only copy in camera once to local memory (sync threads)
  if (llid == 0 ) {
    local_copy_cam[0] = persp_cam[0];  // conjugate transpose of U
    local_copy_cam[1] = persp_cam[1];  // V
    local_copy_cam[2] = persp_cam[2];  // Winv(first4) and ray_origin(last four)
    (*local_copy_imgdims) = (*imgdims);
    
    //load scene information into local vars
    (*linfo) = (*info);
    linfo->dims.w = 1;  //for safety purposes
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  
  // check to see if the thread corresponds to an actual pixel as in some 
  // cases #of threads will be more than the pixels.
  if (i>=(*local_copy_imgdims).z || j>=(*local_copy_imgdims).w) {
    gl_image[imIndex] = rgbaFloatToInt((float4)(0.0,0.0,0.0,0.0));
    in_image[imIndex] = (float)-1.0f;
    return;
  }
  
  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  // ray origin, ray direction, inverse ray direction (make sure ray direction is never axis aligned)
  float4 ray_o = (float4) local_copy_cam[2].s4567; ray_o.w = 1.0f;
  float4 ray_d = backproject(i, j, local_copy_cam[0],
                                   local_copy_cam[1],
                                   local_copy_cam[2], 
                                   ray_o);
  ray_o = ray_o - linfo->origin; ray_o.w = 1.0f; //translate ray o to zero out scene origin

  //thresh ray direction components - too small a treshhold causes axis aligned 
  //viewpoints to hang in infinite loop (block loop)
  float thresh = exp2(-12.0f); 
  if (fabs(ray_d.x) < thresh) ray_d.x = copysign(thresh, ray_d.x);
  if (fabs(ray_d.y) < thresh) ray_d.y = copysign(thresh, ray_d.y);
  if (fabs(ray_d.z) < thresh) ray_d.z = copysign(thresh, ray_d.z);
  ray_d.w = 0.0; ray_d = normalize(ray_d);
  //float4 ray_d_inv = 1.0/ray_d;

  //store float 3's
  float ray_ox = ray_o.x;     float ray_oy = ray_o.y;     float ray_oz = ray_o.z;
  float ray_dx = ray_d.x;     float ray_dy = ray_d.y;     float ray_dz = ray_d.z;
  //float ray_ix = ray_d_inv.x; float ray_iy = ray_d_inv.y; float ray_iz = ray_d_inv.z;

  // pixel values/depth map to be returned
  float4 data_return = (float4) (0.0f,1.0f,0.0f,0.0f);
  float epsilon = linfo->block_len/100.0;                    //block epsilon

  //determine the minimum face:
  //get parameters tnear and tfar for the scene
  float max_facex = (ray_dx > 0.0) ? (linfo->block_len*linfo->dims.x) : 0.0;
  float max_facey = (ray_dy > 0.0) ? (linfo->block_len*linfo->dims.y) : 0.0;
  float max_facez = (ray_dz > 0.0) ? (linfo->block_len*linfo->dims.z) : 0.0;
  float tfar   = min(min( (max_facex-ray_ox)*(1.0/ray_dx), (max_facey-ray_oy)*(1.0/ray_dy)), (max_facez-ray_oz)*(1.0/ray_dz));
  float min_facex = (ray_dx < 0.0) ? (linfo->block_len*linfo->dims.x) : 0.0;
  float min_facey = (ray_dy < 0.0) ? (linfo->block_len*linfo->dims.y) : 0.0;
  float min_facez = (ray_dz < 0.0) ? (linfo->block_len*linfo->dims.z) : 0.0;
  float tblock = max(max( (min_facex-ray_ox)*(1.0/ray_dx), (min_facey-ray_oy)*(1.0/ray_dy)), (min_facez-ray_oz)*(1.0/ray_dz));
  if (tfar <= tblock) {
    gl_image[imIndex] = rgbaFloatToInt((float4)(0.0,0.0,0.0,0.0));
    in_image[imIndex] = (float)-1.0f;
    return;
  }
  //make sure tnear is at least 0...
  tblock = (tblock > 0) ? tblock : 0;
  
  //make sure tfar is within the last block so texit surpasses it (and breaks from the outer loop)
  tfar = tfar - epsilon;   
  
  //used for depth map 
  float global_depth = tblock;

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
    float posx = ray_ox + (tblock+epsilon)*ray_dx;
    float posy = ray_oy + (tblock+epsilon)*ray_dy;
    float posz = ray_oz + (tblock+epsilon)*ray_dz;
    
    //curr block index (var later used as cell_min), check to make sure block index isn't 192 or -1
    float cell_minx = floor(posx/linfo->block_len);
    float cell_miny = floor(posy/linfo->block_len);
    float cell_minz = floor(posz/linfo->block_len);
    cell_minx = (cell_minx > linfo->dims.x) ? cell_minx-1.0 : cell_minx;
    cell_minx = (cell_minx < 0.0)           ? cell_minx+1.0 : cell_minx;
    cell_miny = (cell_miny > linfo->dims.y) ? cell_miny-1.0 : cell_miny;
    cell_miny = (cell_miny < 0.0)           ? cell_miny+1.0 : cell_miny;
    cell_minz = (cell_minz > linfo->dims.z) ? cell_minz-1.0 : cell_minz;
    cell_minz = (cell_minz < 0.0)           ? cell_minz+1.0 : cell_minz;

    //load current block/tree 
    ushort2 block = block_ptrs[convert_int(cell_minz + (cell_miny + cell_minx*linfo->dims.y)*linfo->dims.z)];

    // tree offset is the root_ptr
    int root_ptr = (int) block.x * linfo->tree_len + (int) block.y;
    uchar16 tbuff = tree_array[root_ptr];
    int rIndex = llid*16;
    local_tree[rIndex+0]  = tbuff.s0; local_tree[rIndex+1]  = tbuff.s1; local_tree[rIndex+2]  = tbuff.s2; local_tree[rIndex+3]  = tbuff.s3; 
    local_tree[rIndex+4]  = tbuff.s4; local_tree[rIndex+5]  = tbuff.s5; local_tree[rIndex+6]  = tbuff.s6; local_tree[rIndex+7]  = tbuff.s7; 
    local_tree[rIndex+8]  = tbuff.s8; local_tree[rIndex+9]  = tbuff.s9; local_tree[rIndex+10] = tbuff.sa; local_tree[rIndex+11] = tbuff.sb; 
    local_tree[rIndex+12] = tbuff.sc; local_tree[rIndex+13] = tbuff.sd; local_tree[rIndex+14] = tbuff.se; local_tree[rIndex+15] = tbuff.sf;   
    
    //entry point in local block coordinates (point should be in [0,1]) 
    //(note that cell_min is the current block index at this point)
    //setting local_ray_o to block_pos allows ttree to start at 0
    posx = posx/linfo->block_len - cell_minx;
    posy = posy/linfo->block_len - cell_miny;
    posz = posz/linfo->block_len - cell_minz;

    //float4 local_ray_o = pos; 
    float lrayx = posx;
    float lrayy = posy;
    float lrayz = posz;
    
    //cell_min now equals block lower left corner. 
    cell_minx = linfo->block_len * cell_minx;
    cell_miny = linfo->block_len * cell_miny;
    cell_minz = linfo->block_len * cell_minz;
    float cell_len = linfo->block_len;
    
    //get scene level t exit value.  check to make sure that the ray is progressing. 
    //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
    cell_minx = (ray_dx > 0) ? cell_minx+cell_len : cell_minx; 
    cell_miny = (ray_dy > 0) ? cell_miny+cell_len : cell_miny; 
    cell_minz = (ray_dz > 0) ? cell_minz+cell_len : cell_minz; 
    //float texit = min(min( (cell_minx-ray_ox)*ray_ix, (cell_miny-ray_oy)*ray_iy), (cell_minz-ray_oz)*ray_iz);
    float texit = min(min( (cell_minx-ray_ox)*(1.0/ray_dx), (cell_miny-ray_oy)*(1.0/ray_dy)), (cell_minz-ray_oz)*(1.0/ray_dz));
    if(texit <= tblock) break; //need this check to make sure the ray is progressing

    //ttree starts at 0, ttree_exit is t exit value in the tree level (scaled from scene level)
    texit = (texit-tblock-2*epsilon)/linfo->block_len;
    float ttree = 0.0;
    while (ttree < texit)
    {
      // traverse to leaf cell that contains the entry point, set bounding box
      int data_ptr = traverse_three(rIndex, local_tree, (float4) (posx,posy,posz,.5), &cell_minx, &cell_miny, &cell_minz, &cell_len);

      // check to see how close tnear and tfar are
      cell_minx = (ray_dx > 0) ? cell_minx+cell_len : cell_minx; 
      cell_miny = (ray_dy > 0) ? cell_miny+cell_len : cell_miny; 
      cell_minz = (ray_dz > 0) ? cell_minz+cell_len : cell_minz;
      //float t1 = min(min( (cell_minx-lrayx)*ray_ix, (cell_miny-lrayy)*ray_iy), (cell_minz-lrayz)*ray_iz);
      float t1 = min(min( (cell_minx-lrayx)*(1.0/ray_dx), (cell_miny-lrayy)*(1.0/ray_dy)), (cell_minz-lrayz)*(1.0/ray_dz));

      //data offset is ushort pointed to by tree + bit offset
      //ushort data_offset = data_index_opt(rIndex, local_tree, curr_cell_ptr, bit_lookup);
      data_ptr = data_index_opt(rIndex, local_tree, data_ptr, bit_lookup);
      data_ptr = block.x * linfo->data_len + data_ptr;

      //// distance must be multiplied by the dimension of the bounding box
      float d = (t1-ttree)*linfo->block_len;
      global_depth += d;
    
      //-----------------------------------------------------------------------
      // RAY TRACE SPECIFIC FUNCTION replaces the step cell functor below
      // X:-) DO NOT DELETE THE LINE BELOW THIS IS A STRING REPLACEMNT
      /*$$step_cell$$*/
      // X:-)
      //-----------------------------------------------------------------------
      
      // Added a litle extra to the exit point
      posx = lrayx + (t1+epsilon)*ray_dx;
      posy = lrayy + (t1+epsilon)*ray_dy;
      posz = lrayz + (t1+epsilon)*ray_dz;

      //update current t parameter
      if(t1 <= ttree) break;
      ttree = t1;
    }
    
    //--------------------------------------------------------------------------
    // finding the next block (using exit point already found before tree loop)
    //--------------------------------------------------------------------------
    //scale texit back up
    texit = texit*linfo->block_len + tblock + 2*epsilon;
    tblock = texit;
  }
  
#ifdef DEPTH
  data_return.z+=(1-data_return.w)*tfar;
#endif
#ifdef INTENSITY
  data_return.z+=(1-data_return.w)*1.0f;
#endif

  gl_image[imIndex]=rgbaFloatToInt((float4)data_return.z);
  in_image[imIndex]=(float)data_return.z;
  
}
