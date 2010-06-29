// This function is to update the aux data
#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable


__kernel
void
update_aux( __global int * root_level,  // level of the root.
            __global int4    * tree_array,
            __global float16 * sample_array,
            __global float4  * aux_data_array,
            __global float16 * persp_cam,    // camera orign and SVD of inverse of camera matrix
            __global float4  * global_bbox_tree, // dimensions of the current bbox in global coordinate
            __global uint4   * imgdims,     // dimensions of the image
            __global float4  * in_image,  // input image and store vis_inf and pre_inf
            __global int    * offsetfactor,
            __global int    * offset_x,
            __global int    * offset_y,//,// which threads to run (0,0) (0,1) (1,0) (1,1)
            __local  float4   * local_copy_bbox,
            __local  float16  * local_copy_cam,
            __local uint4 * local_copy_imgdims,
            __local uchar4   *ray_bundle_array,
            __local short4   *cached_loc_codes,
            __local float16  *cached_data,
            __local float4   *cached_aux_data,
            __local float4   * image_vect)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  if (llid == 0 )
  {
    local_copy_cam[0]=persp_cam[0];  // conjugate transpose of U
    local_copy_cam[1]=persp_cam[1];  // V
    local_copy_cam[2]=persp_cam[2];  // Winv(first4) and ray_origin(last four)
    (*local_copy_bbox)=(*global_bbox_tree);
    (*local_copy_imgdims)=(*imgdims);
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  float4 ray_o=(float4)local_copy_cam[2].s4567;
  ray_o.w=1.0f;
  int i=0;
  int j=0;

  int factor=(*offsetfactor);
  //map_work_space_2d(&i,&j);
  map_work_space_2d_offset(&i,&j,(*offset_x),(*offset_y));
  if (i>=(*local_copy_imgdims).z && j>=(*local_copy_imgdims).w)
    return;

  float tnear = 0.0f, tfar =0.0f;
  int root_ptr = 0;
  int rootlevel=(*root_level);
  int4 root_cell=tree_array[root_ptr];
  // set the nlevels here
  short4 root = (short4)(0,0,0,rootlevel);

  // combine the following into one
  float4 cell_min, cell_max;
  float4 entry_pt, exit_pt;
  //// start ray trace

  float cellsize=(float)(1<<rootlevel);
  cellsize=1/cellsize;

  image_vect[llid]=in_image[j*get_global_size(0)*factor+i];

  // // using local variables
  float4 ray_d = backproject(i,j,local_copy_cam[0],local_copy_cam[1],local_copy_cam[2],ray_o);

  // bounding box of root
  cell_bounding_box(root, rootlevel+1, &cell_min, &cell_max);

  // convertin global ray to local tree coordinates
  ray_o= (ray_o-(*local_copy_bbox))/(*local_copy_bbox).w;

  // find entry point of overall tree bounding box
  if (!cell_entry_point(ray_o, ray_d, cell_min, cell_max, &entry_pt))
    return;

  short4 entry_loc_code = loc_code(entry_pt, rootlevel);
  short4 curr_loc_code=(short4)-1;
  int global_count=0;
  //// traverse to leaf cell that contains the entry point
  int curr_cell_ptr = traverse_force(tree_array, root_ptr, root, entry_loc_code,&curr_loc_code,&global_count);
  // this cell is the first pierced by the ray
  // follow the ray through the cells until no neighbors are found
  int count=0;
  while (true)
  {
    //// current cell bounding box
    cell_bounding_box(curr_loc_code, rootlevel+1, &cell_min, &cell_max);
    // check to see how close tnear and tfar are
    int hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
    // special case whenray grazes edge or corner of cube

    if ( fabs(tfar-tnear)<cellsize/10)
    {
      entry_pt=entry_pt+ray_d*cellsize/2;
      entry_pt.w=0.5;

      if (any(entry_pt>=(float4)1.0f)|| any(entry_pt<=(float4)0.0f))
        break;
      entry_loc_code = loc_code(entry_pt, rootlevel);
      //// traverse to leaf cell that contains the entry point
      curr_cell_ptr = traverse(tree_array, root_ptr, root, entry_loc_code,&curr_loc_code,&global_count);
      if (curr_cell_ptr<0)
        break;

      cell_bounding_box(curr_loc_code, rootlevel+1, &cell_min, &cell_max);
      hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
      if (hit)
        entry_pt=ray_o + tnear*ray_d;
    }
    if (!hit)
      break;

    cached_loc_codes[llid]=curr_loc_code;
    ray_bundle_array[llid]=llid;
    barrier(CLK_LOCAL_MEM_FENCE);

    int data_ptr = tree_array[curr_cell_ptr].z;

    ////////////////////////////////////////////////////////
    // the place where the ray trace function can be applied
    load_data_mutable_using_loc_codes(ray_bundle_array,cached_loc_codes);
    if (ray_bundle_array[llid].x==llid)
    {
      /* cell data, i.e., alpha and app model is needed for some passes */
#if %%
      cached_data[llid] = sample_array[data_ptr];
#endif
      cached_aux_data[llid] =aux_data_array[data_ptr];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    // distance must be multiplied by the dimension of the bounding box
    float d = (tfar-tnear)*(*local_copy_bbox).w;
    // no function pointers in OpenCL (spec 8.6a)
    // instead, user must provide source with a function named "step_cell"
    $$step_cell$$;
    if (ray_bundle_array[llid].x==llid)
    {
      /* note that sample data is not changed during ray tracing passes */
      aux_data_array[data_ptr]=cached_aux_data[llid] ;
    }
    in_image[j*get_global_size(0)*factor+i] = image_vect[llid];
    //// exit point
    exit_pt=ray_o + tfar*ray_d;
    exit_pt.w=0.5;

    // if the ray pierces the volume surface then terminate the ray
    if (any(exit_pt>=(float4)1.0f)|| any(exit_pt<=(float4)0.0f))
      break;

    // location code of exit point
    // the exit face mask
    short4 exit_face= cell_exit_face_safe(exit_pt,ray_d, cell_min, cell_max);
    if (exit_face.x<0) // exit face not defined
      break;

    // find the neighboring cell at the exit face

    short4 neighbor_code;

    int neighbor_ptr=neighbor(tree_array, curr_cell_ptr,curr_loc_code,exit_face, rootlevel+1, &neighbor_code,&global_count);
    // if no neighbor then terminate ray
    if (neighbor_ptr<0)
      break;

    // traverse from the neighbor to the cell having the
    // required exit location code
    short4 exit_loc_code = loc_code(exit_pt, rootlevel);

    curr_cell_ptr = traverse_force(tree_array, neighbor_ptr, neighbor_code,exit_loc_code, &curr_loc_code,&global_count);
    // the current cell (cells[curr_cell_ptr])is the cell reached by
    // the neighbor's traverse
    // ray continues: make the current entry point the previous exit point
    entry_pt = exit_pt;

    count++;
  }
  //in_image[j*get_global_size(0)*factor+i]=(float4)(-1,-1,i,j);
}


// This function is to update the data
__kernel
void
update_main(__global float16 * sample_array,
            __global float4  * aux_data_array,
            __global uint * data_size)
{
    int gid=get_global_id(0);
    int datasize= (*data_size);
    if(gid<datasize)
    {   

        float16 data=sample_array[gid];
        float4 aux_data=aux_data_array[gid];
        update_cell(&data,aux_data,2.5f,0.15f,0.09f);
        sample_array[gid]=data;
    }

}
