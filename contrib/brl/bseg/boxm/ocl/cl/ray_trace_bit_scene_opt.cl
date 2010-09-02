
//RAY_TRACE_BIT_SCENE_OPT
//uses int2 tree cells and uchar8 mixture cells
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
                        __global   float4             * output,
                        __constant uchar              * bit_lookup)    // input image and store vis_inf and pre_inf
{
  //get local id (0-63 for an 8x8) of this patch 
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  
  //----------------------------------------------------------------------------
  // get image coordinates and camera, 
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  int i=0,j=0;  map_work_space_2d(&i,&j);

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
    gl_image[j*get_global_size(0)+i] = rgbaFloatToInt((float4)(0.0,0.0,0.0,0.0));
    in_image[j*get_global_size(0)+i] = (float)-1.0f;
    return;
  }
  
  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // begin ray trace
  //----------------------------------------------------------------------------
  // ray origin, ray direction
  float4 ray_o = (float4) local_copy_cam[2].s4567; ray_o.w = 1.0f;
  float4 ray_d = backproject(i, j, local_copy_cam[0],
                                   local_copy_cam[1],
                                   local_copy_cam[2], 
                                   ray_o);
  float4 ray_d_inv = 1.0/ray_d;

  float4 data_return = (float4) (0.0f,1.0f,0.0f,0.0f);
  float epsilon = 1.0/((float)(1<<linfo->root_level));  //side length of the finest cell 
  epsilon = epsilon/10.0;                               //epsilon is a tenth of the smallest cell side length, for grazing condtions...

  //// scene bounding box
  // Do we need cell_min AND cell_max... won't it suffice to have cell_min and cell_size?
  float4 cell_min = linfo->origin;
  float  cell_len = linfo->block_len; 

  //get parameters tnear and tfar for the cell and this ray
  float tnear = 0.0f, tfar = 0.0f;
  if (!intersect_cell_opt(ray_o, ray_d, ray_d_inv, cell_min, linfo->block_len*convert_float4(linfo->dims), &tnear, &tfar)) {
    gl_image[j*get_global_size(0)+i] = rgbaFloatToInt((float4)(0.0,0.0,0.0,0.0));
    in_image[j*get_global_size(0)+i] = (float)-1.0f;
    return;
  }

  //make sure tnear is at least 0...
  tnear = (tnear > 0) ? tnear : 0;
  float fardepth = tfar;
  
  //calculate the index of the intersected block
  //float4 exit_pt;
  float4 entry_pt = ray_o + tnear*ray_d;
  int4 curr_block_index = convert_int4((entry_pt-linfo->origin)/ ((float4) linfo->block_len) ); 
  
  // handling the border case where a ray pierces the max side
  curr_block_index   = curr_block_index + (curr_block_index == linfo->dims);
  curr_block_index.w = 0;
  int global_count=0;
  float global_depth = tnear;

  //----------------------------------------------------------------------------
  // Begin traversing the blocks, break when any curr_block_index value is
  // illegal (not between 0 and scenedims)
  //----------------------------------------------------------------------------
  int curr_cell_ptr = -1;
  while (!(any(curr_block_index<(int4)0) || any(curr_block_index>=(linfo->dims))))
  {
    // Ray tracing with in each block

    // 3-d index to 1-d index
    ushort2 block = block_ptrs[curr_block_index.z
                              +curr_block_index.y*linfo->dims.z
                              +curr_block_index.x*linfo->dims.y*linfo->dims.z];
                           
    // tree offset is the root_ptr
    int root_ptr = (int) block.x * linfo->tree_len + (int) block.y;
    
    //load global tree into local mem
    int rIndex = llid*16;
    uchar16 tbuff = tree_array[root_ptr];
    local_tree[rIndex+0]  = tbuff.s0; local_tree[rIndex+1]  = tbuff.s1; local_tree[rIndex+2]  = tbuff.s2; local_tree[rIndex+3]  = tbuff.s3; 
    local_tree[rIndex+4]  = tbuff.s4; local_tree[rIndex+5]  = tbuff.s5; local_tree[rIndex+6]  = tbuff.s6; local_tree[rIndex+7]  = tbuff.s7; 
    local_tree[rIndex+8]  = tbuff.s8; local_tree[rIndex+9]  = tbuff.s9; local_tree[rIndex+10] = tbuff.sa; local_tree[rIndex+11] = tbuff.sb; 
    local_tree[rIndex+12] = tbuff.sc; local_tree[rIndex+13] = tbuff.sd; local_tree[rIndex+14] = tbuff.se; local_tree[rIndex+15] = tbuff.sf;   

    //local ray origin (can compute first term outside loop)
    float4 local_ray_o = (ray_o-linfo->origin)/linfo->block_len - convert_float4(curr_block_index);
    
    //entry point in local block coordinates (need to ensure that this point is actually
    //between (0,1) for xyz
    float4 block_entry_pt = (entry_pt-linfo->origin)/linfo->block_len - convert_float4(curr_block_index);
    
    //--------------------------------------------------------------------------
    // ray trace small block
    // follow the ray through the cells until no neighbors are found
    //--------------------------------------------------------------------------
    while (true)
    {
      // traverse to leaf cell that contains the entry point, set bounding box
      curr_cell_ptr = traverse_opt_len(rIndex, local_tree, block_entry_pt, &cell_min, &cell_len);

      // check to see how close tnear and tfar are
      int hit = intersect_cell_opt(local_ray_o, ray_d, ray_d_inv, cell_min, (float4) cell_len, &tnear, &tfar);
      if (!hit)
        break;

      //int data_ptr =  block.x*lenbuffer+tree_array[curr_cell_ptr].z;
      //data offset is ushort pointed to by tree + bit offset
      ushort data_offset = data_index(rIndex, local_tree, curr_cell_ptr, bit_lookup);
      int data_ptr = block.x * linfo->data_len + (int) data_offset;

      //// distance must be multiplied by the dimension of the bounding box
      float d = (tfar-tnear)*linfo->block_len;
      global_depth += d;
      
      //-----------------------------------------------------------------------
      // RAY TRACE SPECIFIC FUNCTION replaces the step cell functor below
      // X:-) DO NOT DELETE THE LINE BELOW THIS IS A STRING REPLACEMNT
      /*$$step_cell$$*/
      // X:-)
      //-----------------------------------------------------------------------

      // Added a litle extra to the exit point
      block_entry_pt   = local_ray_o + (tfar + epsilon)*ray_d; 
      block_entry_pt.w = 0.5;

      // if the ray pierces the volume surface then terminate the ray
      if (any(block_entry_pt>=(float4)1.0f)|| any(block_entry_pt<=(float4)0.0f))
        break;

    }

    //--------------------------------------------------------------------------
    // finding the next block
    //--------------------------------------------------------------------------
    // block bounding box
    cell_min = linfo->block_len * convert_float4(curr_block_index) + linfo->origin;
    cell_len = linfo->block_len;
    if (!intersect_cell_opt(ray_o, ray_d, ray_d_inv, cell_min, (float4) cell_len, &tnear, &tfar))
    {
        // this means the ray has hit a special case
        // two special cases
        // (1) grazing the corner/edge and (2) grazing the side.

        // this is the first case
        if (tfar-tnear < linfo->block_len/100)
        {
            entry_pt = entry_pt + linfo->block_len/2 *ray_d;
            curr_block_index   = convert_int4((entry_pt-linfo->origin)/linfo->block_len);
            curr_block_index.w = 0;
        }

    }
    else
    {
        entry_pt=ray_o + tfar *ray_d;
        ray_d.w=1;
        if (any(-1*(isless(fabs(entry_pt-cell_min),(float4)linfo->block_len/100.0f)*isless(fabs(ray_d),(float4)1e-3))))
            break;
        if (any(-1*(isless(fabs(entry_pt-cell_min+cell_len),(float4)linfo->block_len/100.0f)*isless(fabs(ray_d),(float4)1e-3))))
            break;
        curr_block_index   = convert_int4(floor((entry_pt+(linfo->block_len/20.0f)*ray_d-linfo->origin)/linfo->block_len));
        curr_block_index.w = 0;
    }
  }
#ifdef DEPTH
  data_return.z+=(1-data_return.w)*fardepth;
#endif
#ifdef INTENSITY
  data_return.z+=(1-data_return.w)*1.0f;
#endif

  gl_image[j*get_global_size(0)+i]=rgbaFloatToInt((float4)data_return.z);
  in_image[j*get_global_size(0)+i]=(float)data_return.z;
    
  //output[0] = (float4) ((float) tree_len, (float) data_len, (float) num_buffer, 0.0);
}
