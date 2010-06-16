//  This file contains kernel function for ray tracing a bundle. 
//  The differene between this kernel and ray_Trace_main.cl is 
//  that data is copied from the memory for the whole bundle.
//#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
#if 0 //// code for ray bundle ray trace for coherent rays
__kernel
void
ray_bundle_trace_main(__global int * nlevels,
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
					   __local float4   *bbox,        // local storgae of bbox
					   __local uint4    *roi)//,
					   //__local uchar    *ray_bundle_array,
					   //__local short4   *cached_loc_codes,
					   //__local float2  *cached_data)
{
  //unsigned ls0  = get_local_size(0);
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  int i=0;int j=0;
  map_work_space_2d(&i,&j);

  if (llid == 0 )
  {
    cam[0]=svd_UtVW[0];  // conjugate transpose of U
    cam[1]=svd_UtVW[1];  // V
    cam[2]=svd_UtVW[2];  // Winv(first4) and ray_origin(last four)
    (*local_origin) =origin[0];    // ray_origin
    (*bbox)=(*global_bbox);
    (*roi)=(*roidims);
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  

 // //local_img[lid]=inp[gid];
  float tnear = 0.0f, tfar =0.0f;

  int root_ptr = 0;

  int4 root_cell=cells[root_ptr];

  int n_levels = (*nlevels);//root_cell.z;

  // set the nlevels here
  short4 root = (short4)(0,0,0,n_levels-1);

  // combine the following into one
  float4 cell_min, cell_max;
  float4 entry_pt, exit_pt;
  //// start ray trace

  float cellsize=(float)(1<<n_levels-1);
  cellsize=1/cellsize;
  if (i<(*roi).x || i>(*roi).y || j<(*roi).z || j> (*roi).w)
    return;
 float4 data_return=inp[j*get_global_size(0)+i];

 // // using local variables
 float4 ray_d = backproject(i,j,cam[0],cam[1],cam[2],(*local_origin));


 // // bounding box of root
  cell_bounding_box(root, root.w+1, &cell_min, &cell_max);
  float4 ray_o =(float4)(*local_origin);    // ray_origin

  // convertin global ray to local tree coordinates
  ray_o= (ray_o-(*bbox))/(*bbox).w;

  // find entry point of overall tree bounding box
  if (!cell_entry_point(ray_o, ray_d, cell_min, cell_max, &entry_pt))
    return;

  short4 entry_loc_code = loc_code(entry_pt, n_levels-1);
  short4 curr_loc_code=(short4)-1;
  int global_count=0;
  //// traverse to leaf cell that contains the entry point
  int curr_cell_ptr = traverse_force(cells, root_ptr, root, entry_loc_code,&curr_loc_code,&global_count);
  // this cell is the first pierced by the ray
  // follow the ray through the cells until no neighbors are found
  while (1)
  {
    //// current cell bounding box
    cell_bounding_box(curr_loc_code, n_levels, &cell_min, &cell_max);
    // check to see how close tnear and tfar are
    int hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
    // special case whenray grazes edge or corner of cube

    if ( fabs(tfar-tnear)<cellsize/10)
    {
      entry_pt=entry_pt+ray_d*cellsize/2;
      entry_pt.w=0.5;

      if (any(entry_pt>=(float4)1.0f)|| any(entry_pt<=(float4)0.0f))
        break;
      entry_loc_code = loc_code(entry_pt, n_levels-1);
      //// traverse to leaf cell that contains the entry point
      curr_cell_ptr = traverse(cells, root_ptr, root, entry_loc_code,&curr_loc_code,&global_count);
      if (curr_cell_ptr<0)
         break;

      cell_bounding_box(curr_loc_code, n_levels, &cell_min, &cell_max);
      hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
      if (hit)
        entry_pt=ray_o + tnear*ray_d;
    }
    if (!hit)
      break;
	//cached_loc_codes[llid]=curr_loc_code;
	//ray_bundle_array[llid]=llid;

    ////////////////////////////////////////////////////////
    // the place where the ray trace function can be applied
 //   load_data_using_loc_codes(ray_bundle_array,cached_loc_codes);
	//if(ray_bundle_array[llid]==llid)
	//{
 //     int data_ptr = cells[curr_cell_ptr].z;
	//  float16 data= cell_data[data_ptr];
	//  cached_data[llid] =(float2)(data.s0,data.s3);
	//  global_count+=4;
	//}
	///* wait for all workitems to finish loading */
	//barrier(CLK_LOCAL_MEM_FENCE); 

	int data_ptr = cells[curr_cell_ptr].z;
	if ( data_ptr<0)
		break;

	// distance must be multiplied by the dimension of the bounding box
    float d = (tfar-tnear)*(*bbox).w;
	//float16 data=cell_data[data_ptr];
    // no function pointers in OpenCL (spec 8.6a)
    // instead, user must provide source with a function named "step_cell"
    step_cell(cell_data,data_ptr, d, &data_return);

	global_count+=4;

	//step_cell(cached_data[ray_bundle_array[llid]],d, &data_return);

	//data_return=(float4)ray_bundle_array[llid];
    //////////////////////////////////////////////////////////
	//barrier(CLK_LOCAL_MEM_FENCE); 

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

    int neighbor_ptr=neighbor(cells, curr_cell_ptr,curr_loc_code,exit_face, n_levels, &neighbor_code,&global_count);
    // if no neighbor then terminate ray
    if (neighbor_ptr<0)
      break;

    // traverse from the neighbor to the cell having the
    // required exit location code
    short4 exit_loc_code = loc_code(exit_pt, n_levels-1);

    curr_cell_ptr = traverse_force(cells, neighbor_ptr, neighbor_code,exit_loc_code, &curr_loc_code,&global_count);
    // the current cell (cells[curr_cell_ptr])is the cell reached by
    // the neighbor's traverse
    // ray continues: make the current entry point the previous exit point
	entry_pt = exit_pt;
	
  }
  // note that the following code is application dependent
  // should have a cleanup functor for expected image
  // also it is not necessary to have a full float4 as the
  // output type a single scalar float array is sufficient
 data_return.w+=global_count;
 inp[j*get_global_size(0)+i]= (data_return);//local_img[lid];//

  //end ray trace
}

#endif
//  This file contains kernel function for ray tracing a bundle. 
//  The differene between this kernel and ray_Trace_main.cl is 
//  that data is copied from the memory for the whole bundle.
#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

__kernel
void
ray_bundle_trace_main(__global int * nlevels,
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
					   __local float4   *bbox,        // local storgae of bbox
					   __local uint4    *roi,
					   __local uchar    *ray_bundle_array,
					   __local short4   *cached_loc_codes,
					   __local float2  *cached_data)
{
  //unsigned ls0  = get_local_size(0);
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  int i=0;int j=0;
  map_work_space_2d(&i,&j);

  if (llid == 0 )
  {
    cam[0]=svd_UtVW[0];  // conjugate transpose of U
    cam[1]=svd_UtVW[1];  // V
    cam[2]=svd_UtVW[2];  // Winv(first4) and ray_origin(last four)
    (*local_origin) =origin[0];    // ray_origin
    (*bbox)=(*global_bbox);
    (*roi)=(*roidims);
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  

 // //local_img[lid]=inp[gid];
  float tnear = 0.0f, tfar =0.0f;

  int root_ptr = 0;

  int4 root_cell=cells[root_ptr];

  int n_levels = (*nlevels);//root_cell.z;

  // set the nlevels here
  short4 root = (short4)(0,0,0,n_levels-1);

  // combine the following into one
  float4 cell_min, cell_max;
  float4 entry_pt, exit_pt;
  //// start ray trace

  float cellsize=(float)(1<<n_levels-1);
  cellsize=1/cellsize;
  if (i<(*roi).x || i>(*roi).y || j<(*roi).z || j> (*roi).w)
    return;
 float4 data_return=inp[j*get_global_size(0)+i];

 // // using local variables
 float4 ray_d = backproject(i,j,cam[0],cam[1],cam[2],(*local_origin));


 // // bounding box of root
  cell_bounding_box(root, root.w+1, &cell_min, &cell_max);
  float4 ray_o =(float4)(*local_origin);    // ray_origin

  // convertin global ray to local tree coordinates
  ray_o= (ray_o-(*bbox))/(*bbox).w;

  // find entry point of overall tree bounding box
  if (!cell_entry_point(ray_o, ray_d, cell_min, cell_max, &entry_pt))
    return;

  short4 entry_loc_code = loc_code(entry_pt, n_levels-1);
  short4 curr_loc_code=(short4)-1;
  int global_count=0;
  int data_count=0;
  //// traverse to leaf cell that contains the entry point
  int curr_cell_ptr = traverse_force(cells, root_ptr, root, entry_loc_code,&curr_loc_code,&global_count);
  // this cell is the first pierced by the ray
  // follow the ray through the cells until no neighbors are found
  while (1)
  {
    //// current cell bounding box
    cell_bounding_box(curr_loc_code, n_levels, &cell_min, &cell_max);
    // check to see how close tnear and tfar are
    int hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
    // special case whenray grazes edge or corner of cube

    if ( fabs(tfar-tnear)<cellsize/10)
    {
      entry_pt=entry_pt+ray_d*cellsize/2;
      entry_pt.w=0.5;

      if (any(entry_pt>=(float4)1.0f)|| any(entry_pt<=(float4)0.0f))
        break;
      entry_loc_code = loc_code(entry_pt, n_levels-1);
      //// traverse to leaf cell that contains the entry point
      curr_cell_ptr = traverse(cells, root_ptr, root, entry_loc_code,&curr_loc_code,&global_count);
      if (curr_cell_ptr<0)
         break;

      cell_bounding_box(curr_loc_code, n_levels, &cell_min, &cell_max);
      hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
      if (hit)
        entry_pt=ray_o + tnear*ray_d;
    }
    if (!hit)
      break;
	cached_loc_codes[llid]=curr_loc_code;
	ray_bundle_array[llid]=llid;

    ////////////////////////////////////////////////////////
    // the place where the ray trace function can be applied
    load_data_using_loc_codes(ray_bundle_array,cached_loc_codes);
	if(ray_bundle_array[llid]==llid)
	{
      int data_ptr = cells[curr_cell_ptr].z;
	  float16 data= cell_data[data_ptr];
	  cached_data[llid] =(float2)(data.s0,data.s3);
	  data_count+=4;
	}
	// wait for all workitems to finish loading 
	barrier(CLK_LOCAL_MEM_FENCE); 

    // int data_ptr = curr_cell.z;
    // distance must be multiplied by the dimension of the bounding box
    float d = (tfar-tnear)*(*bbox).w;
    // no function pointers in OpenCL (spec 8.6a)
    // instead, user must provide source with a function named "step_cell"
	step_cell_bundle(cached_data[ray_bundle_array[llid]],d, &data_return);
	//step_cell(cached_data[ray_bundle_array[llid]],d, &data_return);

	//data_return=(float4)ray_bundle_array[llid];
    //////////////////////////////////////////////////////////
	//barrier(CLK_LOCAL_MEM_FENCE); 

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

    int neighbor_ptr=neighbor(cells, curr_cell_ptr,curr_loc_code,exit_face, n_levels, &neighbor_code,&global_count);
    // if no neighbor then terminate ray
    if (neighbor_ptr<0)
      break;

    // traverse from the neighbor to the cell having the
    // required exit location code
    short4 exit_loc_code = loc_code(exit_pt, n_levels-1);

    curr_cell_ptr = traverse_force(cells, neighbor_ptr, neighbor_code,exit_loc_code, &curr_loc_code,&global_count);
    // the current cell (cells[curr_cell_ptr])is the cell reached by
    // the neighbor's traverse
    // ray continues: make the current entry point the previous exit point
	entry_pt = exit_pt;
	
  }
  // note that the following code is application dependent
  // should have a cleanup functor for expected image
  // also it is not necessary to have a full float4 as the
  // output type a single scalar float array is sufficient
 data_return.w+=data_count;
 inp[j*get_global_size(0)+i]= (data_return);//local_img[lid];//

  //end ray trace
}

