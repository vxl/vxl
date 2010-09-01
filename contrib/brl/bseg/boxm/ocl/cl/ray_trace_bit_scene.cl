#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif


//RAY_TRACE_BIT_SCENE_OPT
//uses int2 tree cells and uchar8 mixture cells
__kernel
void
ray_trace_bit_scene_opt(__global  RenderSceneInfo * info,
                        __global  ushort2 * block_ptrs,
                        __global  uchar16 * tree_array,
                        __global  float   * alpha_array,
                        __global  uchar8  * mixture_array,
                        __global  float16 * persp_cam, // camera orign and SVD of inverse of camera matrix
                        __global  uint4   * imgdims,   // dimensions of the image
                        __local   uchar   * local_tree,
                        __local   float16 * local_copy_cam,
                        __local   uint4   * local_copy_imgdims,
                        __global  float   * in_image,
                        __global  uint    * gl_image, 
                        __global  float4  * output,
                        __constant uchar  * bit_lookup)    // input image and store vis_inf and pre_inf
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
  
  //load scene information into registers (runs faster with tree_len, data_len and rootlevel in local variables)
  int tree_len  = info->tree_buffer_length;
  int data_len  = info->data_buffer_length;
  int rootlevel = info->root_level;
  float4 data_return = (float4) (0.0f,1.0f,0.0f,0.0f);
  float cellsize = 1.0/((float)(1<<rootlevel));   //for checking the grazing conditions
  float epsilon = cellsize/10.0;                  //epsilon is a tenth of the smallest cell side length

  //// small block dimensions, full scene dimensions...
  float block_len = info->block_len;
  int4 scenedims  = info->scene_dims;
  scenedims.w = 1;  //for safety purposes

  //// scene bounding box
  // Do we need cell_min AND cell_max... won't it suffice to have cell_min and cell_size?
  float4 cell_min = info->scene_origin;
  float  cell_len = block_len; 

  //get parameters tnear and tfar for the cell and this ray
  float tnear = 0.0f, tfar = 0.0f;
  if (!intersect_cell(ray_o, ray_d, cell_min, block_len*convert_float4(scenedims), &tnear, &tfar)) {
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
  int4 curr_block_index = convert_int4( (entry_pt - info->scene_origin)/(float4)(block_len, block_len, block_len, 1.0));

  // handling the border case where a ray pierces the max side
  curr_block_index   = curr_block_index + (curr_block_index == scenedims);
  curr_block_index.w = 0;
  int global_count=0;
  float global_depth = tnear;

  //----------------------------------------------------------------------------
  // Begin traversing the blocks, break when any curr_block_index value is
  // illegal (not between 0 and scenedims)
  //----------------------------------------------------------------------------
  int curr_cell_ptr = -1;
  while (!(any(curr_block_index<(int4)0) || any(curr_block_index>=(scenedims))))
  {
    // Ray tracing with in each block

    // 3-d index to 1-d index
    ushort2 block = block_ptrs[curr_block_index.z
                              +curr_block_index.y*scenedims.z
                              +curr_block_index.x*scenedims.y*scenedims.z];
                           
    // tree offset is the root_ptr
    int root_ptr = (int) block.x * tree_len + (int) block.y;
    
    //load global tree into local mem
    int rIndex = llid*16;
    uchar16 tbuff = tree_array[root_ptr];
    local_tree[rIndex+0]  = tbuff.s0; local_tree[rIndex+1]  = tbuff.s1; local_tree[rIndex+2]  = tbuff.s2; local_tree[rIndex+3]  = tbuff.s3; 
    local_tree[rIndex+4]  = tbuff.s4; local_tree[rIndex+5]  = tbuff.s5; local_tree[rIndex+6]  = tbuff.s6; local_tree[rIndex+7]  = tbuff.s7; 
    local_tree[rIndex+8]  = tbuff.s8; local_tree[rIndex+9]  = tbuff.s9; local_tree[rIndex+10] = tbuff.sa; local_tree[rIndex+11] = tbuff.sb; 
    local_tree[rIndex+12] = tbuff.sc; local_tree[rIndex+13] = tbuff.sd; local_tree[rIndex+14] = tbuff.se; local_tree[rIndex+15] = tbuff.sf;   

    //local ray origin (can compute first term outside loop)
    float4 local_ray_o = (ray_o - info->scene_origin)/block_len - convert_float4(curr_block_index);
    
    //entry point in local block coordinates (need to ensure that this point is actually
    //between (0,1) for xyz
    float4 block_entry_pt = (entry_pt - info->scene_origin)/block_len - convert_float4(curr_block_index);
    //exit_pt = block_entry_pt;
    
    //--------------------------------------------------------------------------
    // ray trace small block
    // follow the ray through the cells until no neighbors are found
    //--------------------------------------------------------------------------
    while (true)
    {
      // traverse to leaf cell that contains the entry point, set bounding box
      curr_cell_ptr = traverse_opt_len(rIndex, local_tree, block_entry_pt, &cell_min, &cell_len);

      // check to see how close tnear and tfar are
      int hit = intersect_cell(local_ray_o, ray_d, cell_min, (float4) cell_len, &tnear, &tfar);
      if (!hit)
        break;

      //int data_ptr =  block.x*lenbuffer+tree_array[curr_cell_ptr].z;
      //data offset is ushort pointed to by tree + bit offset
      ushort data_offset = data_index(rIndex, local_tree, curr_cell_ptr, bit_lookup);
      int data_ptr = block.x*data_len + (int) data_offset;

      //// distance must be multiplied by the dimension of the bounding box
      float d = (tfar-tnear)*block_len;
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
      
      //new entry point is: 
      //block_entry_pt = local_ray_o + (tfar)*ray_d;
    }

    //--------------------------------------------------------------------------
    // finding the next block
    //--------------------------------------------------------------------------
    // block bounding box
    cell_min = block_len * convert_float4(curr_block_index) + info->scene_origin;
    cell_len = block_len;
    if (!intersect_cell(ray_o, ray_d, cell_min, (float4) cell_len, &tnear, &tfar))
    {
        // this means the ray has hit a special case
        // two special cases
        // (1) grazing the corner/edge and (2) grazing the side.

        // this is the first case
        if (tfar-tnear < block_len/100)
        {
            entry_pt = entry_pt + block_len/2 *ray_d;
            curr_block_index   = convert_int4((entry_pt-info->scene_origin)/block_len);
            curr_block_index.w = 0;
        }

    }
    else
    {
        entry_pt=ray_o + tfar *ray_d;
        ray_d.w=1;
        if (any(-1*(isless(fabs(entry_pt-cell_min),(float4)block_len/100.0f)*isless(fabs(ray_d),(float4)1e-3))))
            break;
        if (any(-1*(isless(fabs(entry_pt-cell_min+cell_len),(float4)block_len/100.0f)*isless(fabs(ray_d),(float4)1e-3))))
            break;
        curr_block_index   = convert_int4(floor((entry_pt+(block_len/20.0f)*ray_d-info->scene_origin)/block_len));
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


#if 0
//RAY_TRACE_BIT_SCENE
//uses uchar16 tree cells and uchar8 mixture cells
__kernel
void
ray_trace_bit_scene(__global  int4    * scene_dims,  
                    __global  float4  * scene_origin,
                    __global  float4  * block_dims,
                    __global  ushort2 * block_ptrs,
                    __private uint      root_level,
                    __private int       num_buffer,
                    __private int       tree_buffer_length,
                    __global  uchar16 * tree_array,
                    __private int       data_buffer_length,
                    __global  float   * alpha_array,
                    __global  uchar8  * mixture_array,
                    __global  float16 * persp_cam, // camera orign and SVD of inverse of camera matrix
                    __global  uint4   * imgdims,   // dimensions of the image
                    __local   uchar   * local_tree,
                    __local   float16 * local_copy_cam,
                    __local   uint4   * local_copy_imgdims,
                    __global  float   * in_image,
                    __global  uint    * gl_image, 
                    __global  float4  * output,
                    __constant uchar   * bit_lookup)    // input image and store vis_inf and pre_inf
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  if (llid == 0 )
  {
    local_copy_cam[0]=persp_cam[0];  // conjugate transpose of U
    local_copy_cam[1]=persp_cam[1];  // V
    local_copy_cam[2]=persp_cam[2];  // Winv(first4) and ray_origin(last four)
    (*local_copy_imgdims)=(*imgdims);
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  // camera origin
  float4 ray_o=(float4)local_copy_cam[2].s4567;
  ray_o.w=1.0f;
  int rootlevel = root_level;

  //cell size of what?
  float cellsize=(float)(1<<rootlevel);
  cellsize=1/cellsize;
  short4 root = (short4)(0,0,0,rootlevel);
  short4 exit_face=(short4)-1;
  int curr_cell_ptr=-1;

  // get image coordinates
  int i=0,j=0;  map_work_space_2d(&i,&j);
  int tree_len = tree_buffer_length;
  int data_len = data_buffer_length;
  //in_image[j*get_global_size(0)+i]=0.0f;
  // rootlevel of the trees.

  // check to see if the thread corresponds to an actual pixel as in some cases #of threads will be more than the pixels.
  if (i>=(*local_copy_imgdims).z || j>=(*local_copy_imgdims).w) {
    gl_image[j*get_global_size(0)+i]=rgbaFloatToInt((float4)(0.0,0.0,0.0,0.0));
    in_image[j*get_global_size(0)+i]=(float)-1.0f;
    return;
  }
  float4 origin=(*scene_origin);
  float4 data_return=(float4)(0.0f,1.0f,0.0f,0.0f);
  float tnear = 0.0f, tfar =0.0f;
  float4 ray_d = backproject(i,j,local_copy_cam[0],local_copy_cam[1],local_copy_cam[2],ray_o);

  //// scene origin
  float4 blockdims=(*block_dims);
  int4 scenedims=(int4)(*scene_dims).xyzw;
  scenedims.w=1;blockdims.w=1; // for safety purposes.

  float4 cell_min, cell_max;
  float4 entry_pt, exit_pt;

  //// scene bounding box
  cell_min = origin;
  cell_max = blockdims*convert_float4(scenedims)+origin;
  int hit  = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
  if (!hit) {
    gl_image[j*get_global_size(0)+i]=rgbaFloatToInt((float4)(0.0,0.0,0.0,0.0));
    in_image[j*get_global_size(0)+i]=(float)-1.0f;
    return;
  }

  //make sure tnear is at least 0...
  tnear=tnear>0?tnear:0;
  float fardepth=tfar;
  entry_pt = ray_o + tnear*ray_d;
  int4 curr_block_index = convert_int4((entry_pt-origin)/blockdims);

  // handling the border case where a ray pierces the max side
  curr_block_index=curr_block_index+(curr_block_index==scenedims);
  int global_count=0;
  float global_depth=tnear;

  while (!(any(curr_block_index<(int4)0) || any(curr_block_index>=(scenedims))))
  {
    // Ray tracing with in each block

    // 3-d index to 1-d index
    ushort2 block = block_ptrs[curr_block_index.z
                           +curr_block_index.y*scenedims.z
                           +curr_block_index.x*scenedims.y*scenedims.z];
                           
    // tree offset is the root_ptr
    int root_ptr = (int) block.x * tree_len + (int) block.y;
    
    //get tree into local memory...
    //load global tree into local mem
    int rIndex = llid*16;
    uchar16 tbuff = tree_array[root_ptr];
    local_tree[rIndex+0] = tbuff.s0; local_tree[rIndex+1] = tbuff.s1; local_tree[rIndex+2] = tbuff.s2; local_tree[rIndex+3] = tbuff.s3; 
    local_tree[rIndex+4] = tbuff.s4; local_tree[rIndex+5] = tbuff.s5; local_tree[rIndex+6] = tbuff.s6; local_tree[rIndex+7] = tbuff.s7; 
    local_tree[rIndex+8] = tbuff.s8; local_tree[rIndex+9] = tbuff.s9;local_tree[rIndex+10] = tbuff.sa;local_tree[rIndex+11] = tbuff.sb; 
    local_tree[rIndex+12]= tbuff.sc;local_tree[rIndex+13] = tbuff.sd;local_tree[rIndex+14] = tbuff.se;local_tree[rIndex+15] = tbuff.sf;   
//    event_t eventid = (event_t) 0;
//    event_t e = async_work_group_copy(local_tree, (uchar*) &tree_array[root_ptr], (size_t)16, eventid);
//    wait_group_events (1, &eventid);

    float4 local_ray_o= (ray_o-origin)/blockdims-convert_float4(curr_block_index);
    
    // canonincal bounding box of the tree
    float4 block_entry_pt=(entry_pt-origin)/blockdims-convert_float4(curr_block_index);
    short4 entry_loc_code = loc_code(block_entry_pt, rootlevel);
    short4 curr_loc_code=(short4)-1;
    
    // traverse to leaf cell that contains the entry point
    //curr_cell_ptr = traverse_force_woffset(tree_array, root_ptr, root, entry_loc_code,&curr_loc_code,&global_count,root_ptr);
    curr_cell_ptr = traverse_force(rIndex, local_tree, 0, root, entry_loc_code, &curr_loc_code, &global_count);

    // this cell is the first pierced by the ray
    // follow the ray through the cells until no neighbors are found
    while (true)
    {
      //// current cell bounding box
      cell_bounding_box(curr_loc_code, rootlevel+1, &cell_min, &cell_max);
     
      // check to see how close tnear and tfar are
      int hit = intersect_cell(local_ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
     
      // special case whenray grazes edge or corner of cube
      if (fabs(tfar-tnear)<cellsize/10)
      {
        block_entry_pt=block_entry_pt+ray_d*cellsize/2;
        block_entry_pt.w=0.5;
        if (any(block_entry_pt>=(float4)1.0f)|| any(block_entry_pt<=(float4)0.0f))
            break;

        entry_loc_code = loc_code(block_entry_pt, rootlevel);
        //// traverse to leaf cell that contains the entry point
        curr_cell_ptr = traverse(rIndex, local_tree, 0, root, entry_loc_code, &curr_loc_code, &global_count);
        cell_bounding_box(curr_loc_code, rootlevel+1, &cell_min, &cell_max);
        hit = intersect_cell(local_ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
        if (hit)
          block_entry_pt=local_ray_o + tnear*ray_d;
      }
      if (!hit)
          break;

      //int data_ptr =  block.x*lenbuffer+tree_array[curr_cell_ptr].z;
      //data offset is ushort pointed to by tree + bit offset
      ushort data_offset = data_index(rIndex, local_tree, curr_cell_ptr, bit_lookup);
      int data_ptr = block.x*data_len + (int) data_offset;

      //// distance must be multiplied by the dimension of the bounding box
      float d = (tfar-tnear)*(blockdims.x);
      global_depth+=d;
      
      // X:-) DO NOT DELETE THE LINE BELOW THIS IS A STRING REPLACEMNT
      /*$$step_cell$$*/
      // X:-)

      // Added aliitle extra to the exit point
      exit_pt=local_ray_o + (tfar+cellsize/10)*ray_d;exit_pt.w=0.5;

      // if the ray pierces the volume surface then terminate the ray
      if (any(exit_pt>=(float4)1.0f)|| any(exit_pt<=(float4)0.0f))
        break;

      //// required exit location code
      short4 exit_loc_code = loc_code(exit_pt, rootlevel);
      curr_cell_ptr = traverse_force(rIndex, local_tree, 0, root, exit_loc_code, &curr_loc_code,&global_count);

      block_entry_pt = local_ray_o + (tfar)*ray_d;
    }

    // finding the next block

    // block bounding box
    cell_min=blockdims*convert_float4(curr_block_index)+origin;
    cell_max=cell_min+blockdims;
    if (!intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar))
    {
        // this means the ray has hit a special case
        // two special cases
        // (1) grazing the corner/edge and (2) grazing the side.

        // this is the first case
        if (tfar-tnear<blockdims.x/100)
        {
            entry_pt=entry_pt + blockdims.x/2 *ray_d;
            curr_block_index=convert_int4((entry_pt-origin)/blockdims);
            curr_block_index.w=0;
        }

    }
    else
    {
        entry_pt=ray_o + tfar *ray_d;
        ray_d.w=1;
        if (any(-1*(isless(fabs(entry_pt-cell_min),(float4)blockdims.x/100.0f)*isless(fabs(ray_d),(float4)1e-3))))
            break;
        if (any(-1*(isless(fabs(entry_pt-cell_max),(float4)blockdims.x/100.0f)*isless(fabs(ray_d),(float4)1e-3))))
            break;
        curr_block_index=convert_int4(floor((entry_pt+(blockdims.x/20.0f)*ray_d-origin)/blockdims));
        curr_block_index.w=0;
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
#endif

