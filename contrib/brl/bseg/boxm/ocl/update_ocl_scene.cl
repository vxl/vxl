#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

__kernel
void 
update_ocl_scene(__global int4    * scene_dims,  // level of the root.
                  __global float4  * scene_origin,
                  __global float4  * block_dims,
                  __global int4    * block_ptrs,
                  __global int     * root_level,
                  __global int     * num_buffer,
                  __global int     * len_buffer,
                  __global int4    * tree_array,
                  __global float16  * sample_array,
                  __global float4   * aux_data_array,
                  __global float16 * persp_cam, // camera orign and SVD of inverse of camera matrix
                  __global uint4   * imgdims,   // dimensions of the image
                  __global float4  * in_image,
                  __global int    * offsetfactor,
                  __global int    * offset_x,
                  __global int    * offset_y,
                  __local  float16 * local_copy_cam,
                  __local  uint4   * local_copy_imgdims,
                  __local uchar4   *ray_bundle_array,
                  __local int   *cell_ptrs,
                  __local float16  *cached_data,
                  __local float4   *cached_aux_data,
                  __local float4   * image_vect)    // input image and store vis_inf and pre_inf
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
  int rootlevel=(*root_level);
  int factor=(*offsetfactor);

  float cellsize=(float)(1<<rootlevel);
  cellsize=1/cellsize;
  int count=0;
  short4 root = (short4)(0,0,0,rootlevel);

  short4 exit_face=(short4)-1;

  //float4 debug_var=(float4)0;
  int curr_cell_ptr=-1;

  // get image coordinates
  int i=0,j=0;  map_work_space_2d_offset(&i,&j,(*offset_x),(*offset_y));

  int lenbuffer =(*len_buffer);
  // rootlevel of the trees.
  image_vect[llid]=in_image[j*get_global_size(0)*factor+i];
  barrier(CLK_LOCAL_MEM_FENCE);

  // check to see if the thread corresponds to an actual pixel as in some cases #of threads will be more than the pixels.
  if (i>=(*local_copy_imgdims).z || j>=(*local_copy_imgdims).w)
  {
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
  cell_min=origin;
  cell_max=blockdims*convert_float4(scenedims)+origin;
  int hit=intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
  if(!hit)
  {
      return;
  }
  entry_pt=ray_o + tnear*ray_d;

  int4 curr_block_index=convert_int4((entry_pt-origin)/blockdims);

  //: handling the border case where a ray pierces the max side
  curr_block_index=curr_block_index+(curr_block_index==scenedims);

  int global_count=0;

  //float length=0;
  while (!(any(curr_block_index<(int4)0)|| any(curr_block_index>=(scenedims))))
  {
     // Ray tracing with in each block

     // 3-d index to 1-d index
    int4 block = block_ptrs[curr_block_index.z
                           +curr_block_index.y*scenedims.z
                           +curr_block_index.x*scenedims.y*scenedims.z];

    //: tree offset is the root_ptr
    int root_ptr= block.x*lenbuffer+block.y;

    float4 local_ray_o= (ray_o-origin)/blockdims-convert_float4(curr_block_index);
    // canonincal bounding box of the tree
    //cell_bounding_box(root, rootlevel+1, &cell_min, &cell_max);
    float4 block_entry_pt=(entry_pt-origin)/blockdims-convert_float4(curr_block_index);
    short4 entry_loc_code = loc_code(block_entry_pt, rootlevel);
    short4 curr_loc_code=(short4)-1;
    // traverse to leaf cell that contains the entry point
    curr_cell_ptr = traverse_force_woffset(tree_array, root_ptr, root, entry_loc_code,&curr_loc_code,&global_count,root_ptr);

    // this cell is the first pierced by the ray
    // follow the ray through the cells until no neighbors are found

    while (true )
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
        curr_cell_ptr = traverse_woffset(tree_array, root_ptr, root, entry_loc_code,&curr_loc_code,&global_count,root_ptr);
        cell_bounding_box(curr_loc_code, rootlevel+1, &cell_min, &cell_max);
        hit = intersect_cell(local_ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
        if (hit)
          block_entry_pt=local_ray_o + tnear*ray_d;
      }
      if (!hit)
          break;

      cell_ptrs[llid]=curr_cell_ptr;
      ray_bundle_array[llid].x=llid;
      barrier(CLK_LOCAL_MEM_FENCE);

      int data_ptr = tree_array[curr_cell_ptr].z;

      ////////////////////////////////////////////////////////
      // the place where the ray trace function can be applied
      load_data_mutable_using_cell_ptrs(ray_bundle_array,cell_ptrs);
      if (ray_bundle_array[llid].x==llid)
      {
          //      /* cell data, i.e., alpha and app model is needed for some passes */
#if %%
          cached_data[llid] = sample_array[data_ptr];
#endif
          cached_aux_data[llid] =aux_data_array[data_ptr];
      }
      barrier(CLK_LOCAL_MEM_FENCE);
//
//      // distance must be multiplied by the dimension of the bounding box
        float d = (tfar-tnear)*blockdims.x;
//      // no function pointers in OpenCL (spec 8.6a)
//      // instead, user must provide source with a function named "step_cell"
       $$step_cell$$;
       if (ray_bundle_array[llid].x==llid)
      {
//          /* note that sample data is not changed during ray tracing passes */
         aux_data_array[data_ptr]=(float4)cached_aux_data[llid] ;
       }
     in_image[j*get_global_size(0)*factor+i] = image_vect[llid];
      //// exit point

      // Added aliitle extra to the exit point
      exit_pt=local_ray_o + (tfar+cellsize/10)*ray_d;exit_pt.w=0.5;

      // if the ray pierces the volume surface then terminate the ray
      if (any(exit_pt>=(float4)1.0f)|| any(exit_pt<=(float4)0.0f))
          break;

      //// required exit location code
      short4 exit_loc_code = loc_code(exit_pt, rootlevel);
      curr_cell_ptr = traverse_force_woffset(tree_array, root_ptr, root,exit_loc_code, &curr_loc_code,&global_count,root_ptr);

      block_entry_pt = local_ray_o + (tfar)*ray_d;
      count++;

    }

    // finding the next block

    // block bounding box

    cell_min=blockdims*convert_float4(curr_block_index)+origin;
    cell_max=cell_min+blockdims;
    if (!intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar))
    {
        //: this means the ray has hit a special case 
        //: two special cases
        //: (1)grazing the corner/edge and (2)grazing the side.

        // this is the first case
        if(tfar-tnear<blockdims.x/100)
        {
           
            entry_pt=entry_pt + blockdims.x/2 *ray_d;
            curr_block_index=convert_int4((entry_pt-origin)/blockdims);
            curr_block_index.w=0;
        }

    }
    else
    {
        entry_pt=ray_o + tfar *ray_d;ray_d.w=1;
        if(any(-1*(isless(fabs(entry_pt-cell_min),(float4)blockdims.x/100.0f)*isless(fabs(ray_d),(float4)1e-3))))
            break;
        if(any(-1*(isless(fabs(entry_pt-cell_max),(float4)blockdims.x/100.0f)*isless(fabs(ray_d),(float4)1e-3))))
            break;

        curr_block_index=convert_int4(floor((ray_o + tfar *ray_d+(blockdims.x/20.0f)*ray_d-origin)/blockdims));
        curr_block_index.w=0;
    }
    count++;

  }
}
__kernel
void
update_ocl_scene_main(__global float16 * sample_array,
                    __global float4  * aux_data_array,
                    __global int * lenbuffer,
                    __global int * numbuffer)
{
    int gid=get_global_id(0);
    int datasize= (*lenbuffer);
    if(gid<datasize)
    {   

        float16 data=sample_array[gid];
        float4 aux_data=aux_data_array[gid];
        if(aux_data.x>1e-10f)
        {
        update_cell(&data,aux_data,2.5f,0.09f,0.03f);
        
        }
        sample_array[gid]=data;
        aux_data_array[gid]=(float4)0.0f;
    }
}