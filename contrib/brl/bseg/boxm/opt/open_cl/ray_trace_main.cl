
__kernel
void
ray_trace_main(__global float4  *origin,
               __global float16 *svd_UtVW,
               __global int4    *cells,
               __global float16 *cell_data,
               __global uint4   *imgdims,
               __global uint4   *roidims,
               __global float4  *global_bbox,
               __global float4  *inp,
               __global float4  *results,
               __local float16  *cam,
               __local float4   *local_origin,
               __local float4   *bbox,
               __local uint4    *roi)
{
  unsigned gid = get_global_id(0);
#if 0 // full body commented out
  if (get_local_id(0) == 0)
  {
    cam[0]=svd_UtVW[0];  // conjugate transpose of U
    cam[1]=svd_UtVW[1];  // V
    cam[2]=svd_UtVW[2];  // Winv(first4) and ray_origin(last four)
    local_origin[0] =origin[0];    // ray_origin
    (*bbox)=(*global_bbox);
    (*roi)=(*roidims);
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  float tnear = 0, tfar =0;

  int root_ptr = 0;
  //int n_levels = 9;
  // set the nlevels here
  short4 root = (short4)(0,0,0,9-1);

  short4 neighbor_code = (short4)-1;
  float cellsize=(float)(1<<root.w);
  cellsize=1/cellsize;
  //combine the following into one
  float4 cell_min, cell_max;
  float4 entry_pt, exit_pt;

  //// start ray trace

  float4 ray_o =local_origin[0];    // ray_origin

  uint img_bb_y=(*imgdims).y;
  unsigned i   = gid/img_bb_y;
  unsigned j   = gid-i*img_bb_y;

  if (i<(*roi).x || i>(*roi).y || j<(*roi).z || j> (*roi).w)
      return;

  // using local variables
  float4 ray_d = backproject(i,j,cam[0],cam[1],cam[2],ray_o);


  //bounding box of root
  cell_bounding_box(root, root.w+1, &cell_min, &cell_max);

  // convertin global ray to local tree coordinates
  ray_o= (ray_o-(*bbox))/(*bbox).w;

  //find entry point of overall tree bounding box
  if (!cell_entry_point(ray_o, ray_d, cell_min, cell_max, &entry_pt))
    return;

  short4 exit_face=(short4)-1;

  short4 entry_loc_code = loc_code(entry_pt, root.w);
  short4 curr_loc_code;
  ////traverse to leaf cell that contains the entry point

  int curr_cell_ptr = traverse_force(cells, root_ptr, root, entry_loc_code,&curr_loc_code);

  float4 data_return=inp[gid];


  //this cell is the first pierced by the ray
  //follow the ray through the cells until no neighbors are found
  while (true) {
    ////current cell bounding box
    cell_bounding_box(curr_loc_code, root.w+1, &cell_min, &cell_max);
    // check to see how close tnear and tfar are
    int hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
    // int hit = intersect_cell(entry_pt, ray_d, cell_min, cell_max,&tnear, &tfar);

    // special case whenray grazes edge or corner of cube
    if (vcl_fabs(tfar-tnear)<cellsize/20)
    {
      entry_pt=entry_pt+ray_d*cellsize/2;
      if (entry_pt.x>=1.0f||entry_pt.y>=1.0f||entry_pt.z>=1.0f || entry_pt.x<=0.0f||entry_pt.y<=0.0f||entry_pt.z<=0.0f)
        break;
      entry_loc_code = loc_code(entry_pt, root.w);
      ////traverse to leaf cell that contains the entry point
      curr_cell_ptr = traverse(cells, root_ptr, root, entry_loc_code,&curr_loc_code);

      if (curr_cell_ptr<0)
        break;
      cell_bounding_box(curr_loc_code, root.w+1, &cell_min, &cell_max);
      hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
      if (hit)
        entry_pt=ray_o + tnear*ray_d;
    }
    if (!hit)
      break;

    ////////////////////////////////////////////////////////
    // the place where the ray trace function can be applied
    int data_ptr = cells[curr_cell_ptr].z;
    if (data_ptr<0)
      break;
    //distance must be multiplied by the dimension of the bounding box
    float d = (tfar-tnear)*(*bbox).w;
    // no function pointers in OpenCL (spec 8.6a)
    // instead, user must provide source with a function named "step_cell"
    step_cell(cell_data, data_ptr, d, &data_return);

    //////////////////////////////////////////////////////////

    //// exit point
    exit_pt=ray_o + tfar*ray_d;

    //if the ray pierces the volume surface then terminate the ray
    if (exit_pt.x>=1.0f||exit_pt.y>=1.0f||exit_pt.z>=1.0f||exit_pt.x<=0.0f||exit_pt.y<=0.0f||exit_pt.z<=0.0f)
      break;

    //location code of exit point
    short4 exit_loc_code = loc_code(exit_pt, root.w);

    //the exit face mask
    exit_face = cell_exit_face(exit_pt, cell_min, cell_max);
    if (exit_face.x<0)//exit face not defined
      break;

    //find the neighboring cell at the exit face
    int neighbor_ptr =   neighbor(cells, curr_cell_ptr, curr_loc_code,exit_face, root.w+1, &neighbor_code);
    //if no neighbor then terminate ray
    if (neighbor_ptr<0)
      break;

    //traverse from the neighbor to the cell having the
    //required exit location code
    curr_cell_ptr = traverse_force(cells, neighbor_ptr, neighbor_code,exit_loc_code, &curr_loc_code);

    //the current cell (cells[curr_cell_ptr])is the cell reached by
    //the neighbor's traverse

    //ray continues: make the current entry point the previous exit point
    entry_pt = exit_pt;
  }
  // note that the following code is application dependent
  // should have a cleanup functor for expected image
  // also it is not necessary to have a full float4 as the
  // output type a single scalar float array is sufficient

  results[gid] = (float4)(data_return);
#endif // 0
  //end ray trace
}

