
__kernel
void
ray_stack_trace_main(__global int * nlevels,
                     __global float4  *origin,      // camera origin
                     __global float16 *svd_UtVW,    // SVD of inverse of camera matrix
                     __global int4    *cells,       // tree
                     __global float16 *cell_data,   // leaf data
                     __global uint4   *imgdims,     // dimensions of the image
                     __global uint4   *roidims,     // dimensions of the roi per block
                     __global float4  *global_bbox, // dimensions of the current bbox in global coordinate
                     __global float4  *inp,         // inp image
                     __local float16  *cam,         // local storage of cam
                     __local float4   *local_origin,// store the origin locally
                     __local float4   *bbox,        // local storage of bbox
                     __local uint4    *roi,
                  // __local float4   *local_img,
                     __local int * stack)           // local storage of bbox
{
  unsigned gid = get_global_id(0);
  unsigned lid = get_local_id(0);
  unsigned workgrpsize=get_local_size(0);

  if (lid == 0)
  {
    cam[0]=svd_UtVW[0];        // conjugate transpose of U
    cam[1]=svd_UtVW[1];        // V
    cam[2]=svd_UtVW[2];        // Winv(first4) and ray_origin(last four)
    (*local_origin) =origin[0];     // ray_origin
    (*bbox)=(*global_bbox);
    (*roi)=(*roidims);
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  float tnear = 0, tfar =0;

  int n_levels =(*nlevels);

  // set the nlevels here
  short4 root = (short4)(0,0,0,n_levels-1);

  // combine the following into one
  float4 cell_min, cell_max;
  float4 entry_pt, exit_pt;
  //// start ray trace

  float smallest_cellsize=(float)(1<<n_levels-1);
  smallest_cellsize=1/smallest_cellsize;
  uint img_bb_y=(*imgdims).y;
  unsigned i   = gid/img_bb_y;
  unsigned j   = gid-i*img_bb_y;
  if (i<(*roi).x || i>(*roi).y || j<(*roi).z || j> (*roi).w)
      return;

  // using local variables
  float4 ray_d = backproject(i,j,cam[0],cam[1],cam[2],(*local_origin));
  // bounding box of root
  cell_bounding_box(root,n_levels, &cell_min, &cell_max);
  float4 ray_o =(float4)(*local_origin);    // ray_origin

  // convertin global ray to local tree coordinates
  ray_o= (ray_o-(*bbox))/(*bbox).w;

  // find entry point of overall tree bounding box
  if (!cell_entry_point(ray_o, ray_d, cell_min, cell_max, &entry_pt))
    return;

  short4 entry_loc_code = loc_code(entry_pt, n_levels-1);
  short4 curr_loc_code;
  //// traverse to leaf cell that contains the entry point
  int stack_ptr=0;
  stack[lid]=0;
  int global_count=0;

  stack_ptr=traverse_force_stack(cells,  root, entry_loc_code,&curr_loc_code,stack,lid,workgrpsize,stack_ptr,&global_count);
  float4 data_return=inp[gid];

  // this cell is the first pierced by the ray
  // follow the ray through the cells until no neighbors are found
  while (true) {
    //// current cell bounding box
    cell_bounding_box(curr_loc_code, n_levels, &cell_min, &cell_max);
    // check to see how close tnear and tfar are
    int hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
    // special case whenray grazes edge or corner of cube

    if ( fabs(tfar-tnear)<smallest_cellsize/10)
    {
      entry_pt=entry_pt+ray_d*smallest_cellsize/2;
      entry_pt.w=0.5;

      if (any(entry_pt>=(float4)1.0f)|| any(entry_pt<=(float4)0.0f))
        break;
      entry_loc_code = loc_code(entry_pt, n_levels-1);
      stack_ptr=0;
      //// traverse to leaf cell that contains the entry point
      stack_ptr = traverse_stack(cells,  root, entry_loc_code,&curr_loc_code, lid,workgrpsize,stack,stack_ptr,&global_count);
      if (stack_ptr<0)
        break;

      cell_bounding_box(curr_loc_code, n_levels, &cell_min, &cell_max);
      hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
      if (hit)
        entry_pt=ray_o + tnear*ray_d;
    }
    if (!hit)
      break;

    ////////////////////////////////////////////////////////
    // the place where the ray trace function can be applied

    int stack_index=(int)lid +(int)workgrpsize*stack_ptr;
    int data_ptr = cells[stack[stack_index]].z;

    //int data_ptr = curr_cell.z;
    if ( data_ptr<0)
      break;

    // distance must be multiplied by the dimension of the bounding box
    float d = (tfar-tnear)*(*bbox).w;
    // no function pointers in OpenCL (spec 8.6a)
    // instead, user must provide source with a function named "step_cell"
    //float4 data_return=local_img[lid];
    step_cell(cell_data,  data_ptr, d, &data_return);
    global_count+=4;
    //local_img[lid]=data_return;

    //////////////////////////////////////////////////////////

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
    stack_ptr=neighbor_stack(cells,  curr_loc_code,exit_face, n_levels, &neighbor_code,stack,lid,workgrpsize,stack_ptr,&global_count);

    // if no neighbor then terminate ray
    if (stack_ptr<0)
      break;

    // traverse from the neighbor to the cell having the
    // required exit location code
    short4 exit_loc_code = loc_code(exit_pt, n_levels-1);

    stack_ptr = traverse_force_stack(cells,  neighbor_code,exit_loc_code, &curr_loc_code,stack, lid,workgrpsize,stack_ptr,&global_count);

    // the current cell is the cell reached by the neighbor's traverse

    // ray continues: make the current entry point the previous exit point
    entry_pt = exit_pt;
  }
  // note that the following code is application dependent
  // should have a cleanup functor for expected image
  // also it is not necessary to have a full float4 as the
  // output type a single scalar float array is sufficient
 // data_return.w+=global_count;
  inp[gid] = (float4)(data_return);//local_img[lid];//

  // end ray trace
}

