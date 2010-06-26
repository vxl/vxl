// This function is to update the aux data
#pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
uint rgbaFloatToInt(float4 rgba)
{
    rgba.x = clamp(rgba.x,0.0f,1.0f);  
    rgba.y = clamp(rgba.y,0.0f,1.0f);  
    rgba.z = clamp(rgba.z,0.0f,1.0f);  
    rgba.w = clamp(rgba.w,0.0f,1.0f);  
    return ((uint)(rgba.w*255.0f)<<24) | ((uint)(rgba.z*255.0f)<<16) | ((uint)(rgba.y*255.0f)<<8) | (uint)(rgba.x*255.0f);
}
#if 0
__kernel
void
ray_trace_all_blocks( __global int4 * scene_dims,  // level of the root.
					  __global float4 * scene_origin,
					  __global float4 * block_dims,
					  __global int * block_ptrs,
					  __global int * root_level,
					  __global int4    * tree_array,
					  __global float8 * sample_array,
					  __global float * alpha_array,
					  __global float16 * persp_cam,    // camera orign and SVD of inverse of camera matrix
					__global uint4   * imgdims,     // dimensions of the image
					__global float4  * in_image,  // input image and store vis_inf and pre_inf
					__local  float16  * local_copy_cam,
					__local uint4 * local_copy_imgdims)
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

  //: camera origin
  float4 ray_o=(float4)local_copy_cam[2].s4567;
  ray_o.w=1.0f;
  
  //: get image coordinates
  int i=0,j=0;
  map_work_space_2d(&i,&j);

  //: rootlevel of the trees.
  int rootlevel=(*root_level);

  //: check to see if the thread corresponds to an actual pixel as in some cases #of threads will be more than the pixels.
  if (i>=(*local_copy_imgdims).z && j>=(*local_copy_imgdims).w)
    return;

  float4 data_return=(float4)(0.0f,1.0f,0.0f,0.0f);
  float tnear = 0.0f, tfar =0.0f;
  float4 ray_d = backproject(i,j,local_copy_cam[0],local_copy_cam[1],local_copy_cam[2],ray_o);

  //: scene origin
  float4 origin=(*scene_origin);
  float4 blockdims=(*block_dims);
  int4 scenedims=(int4)(*scene_dims).xyzw;
  scenedims.w=1;blockdims.w=1; //: for safety purposes.

  float4 cell_min, cell_max;
  float4 entry_pt, exit_pt;
  
  //: scene bounding box
  cell_min=origin;
  cell_max=blockdims*convert_float4(scenedims)+origin;
  int hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
  if (!hit)
	return;

  entry_pt=ray_o + tnear*ray_d;

  int4 curr_block_index=convert_int4((entry_pt-origin)/blockdims);
  if(curr_block_index.x==scenedims.x)
	curr_block_index.x--;
  if(curr_block_index.y==scenedims.y)
	curr_block_index.y--;
  if(curr_block_index.z==scenedims.z)
	curr_block_index.z--;

  curr_block_index.w=0;

  //// combine the following into one
  float cellsize=(float)(1<<rootlevel);
  cellsize=1/cellsize;
  int count=0;
  float4 debug_var;
  while(1)
  {
	  if (any(curr_block_index<(int4)0)|| any(curr_block_index>=(scenedims)))
		  break;

	  // Ray tracing with in each block
	  
		  //: 3-d index to 1-d index
	  int root_ptr = block_ptrs[curr_block_index.z*scenedims.x*scenedims.y
								+curr_block_index.y*scenedims.x
								+curr_block_index.x];

	  //// set the nlevels here
	  short4 root = (short4)(0,0,0,rootlevel);
	  float4 block_origin=blockdims*convert_float4(curr_block_index)+origin;
	  //: ray origin is moved to canonical coordinates
	  float4 local_ray_o= (ray_o-block_origin)/blockdims;
	  int4 root_cell=tree_array[root_ptr];

	  //: canonincal bounding box of the tree
      cell_bounding_box(root, rootlevel+1, &cell_min, &cell_max);

	  if (!cell_entry_point(local_ray_o, ray_d, cell_min, cell_max, &entry_pt))
	    return;


	  short4 entry_loc_code = loc_code(entry_pt, rootlevel);
	  short4 curr_loc_code=(short4)-1;
	  int global_count=0;
	  ////// traverse to leaf cell that contains the entry point
	  int curr_cell_ptr = traverse_force(tree_array, root_ptr, root, entry_loc_code,&curr_loc_code,&global_count);
	  debug_var=(float4)curr_cell_ptr;//convert_float4(entry_loc_code);
	  // this cell is the first pierced by the ray
	  // follow the ray through the cells until no neighbors are found
	  
	  while (1)
	  {
		  //// current cell bounding box
		  cell_bounding_box(curr_loc_code, rootlevel+1, &cell_min, &cell_max);
		  // check to see how close tnear and tfar are
		  int hit = intersect_cell(local_ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
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
			  hit = intersect_cell(local_ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
			  if (hit)
				  entry_pt=local_ray_o + tnear*ray_d;
		  }
		  if (!hit)
			  break;


		  int data_ptr = tree_array[curr_cell_ptr].z;
		  // distance must be multiplied by the dimension of the bounding box
		  float d = (tfar-tnear)*(blockdims.x);
		  // no function pointers in OpenCL (spec 8.6a)
		  // instead, user must provide source with a function named "step_cell"

		  //half16 data=(sample_array[data_ptr]);
		  step_cell(sample_array,alpha_array,data_ptr,d,&data_return);

		  //// exit point
		  exit_pt=local_ray_o + tfar*ray_d;
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

	  //: finding the next block

	  //: block bounding box
	  cell_min=blockdims*convert_float4(curr_block_index)+origin;
	  cell_max=cell_min+blockdims;
	  hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
	  if (!hit)
		  return;
      exit_pt =ray_o + tfar *ray_d;

	  short4 exit_face= cell_exit_face_safe(exit_pt,ray_d, cell_min, cell_max);
	  if (exit_face.x<0) // exit face not defined
		  break;

	  if(exit_face.w==0)
		  curr_block_index=curr_block_index-convert_int4(exit_face);
	  else
		  curr_block_index=curr_block_index+convert_int4(exit_face);

	  curr_block_index.w=0;

	  entry_pt=exit_pt;
  }



  data_return.z+=(1-data_return.w)*0.5f;
  //// find entry point of overall tree bounding box

  //// note that the following code is application dependent
  //// should have a cleanup functor for expected image
  //// also it is not necessary to have a full float4 as the
  //// output type a single scalar float array is sufficient
  in_image[j*get_global_size(0)+i]=(float4)data_return;//(float4)(i,j,scenedims.x,0);//(i,j,0,0);//count;//aux_data_array[j*get_global_size(0)+i];
}
#endif

__kernel
void
ray_trace_all_blocks( __global int4 * scene_dims,  // level of the root.
					  __global float4 * scene_origin,
					  __global float4 * block_dims,
					  __global int * block_ptrs,
					  __global int * root_level,
					  __global int4    * tree_array,
					  __global float8 * sample_array,
					  __global float * alpha_array,
					  __global float16 * persp_cam,    // camera orign and SVD of inverse of camera matrix
				      __global uint4   * imgdims,     // dimensions of the image
					  __local  float16  * local_copy_cam,
					  __local uint4 * local_copy_imgdims,
				      __global uint  * in_image)  // input image and store vis_inf and pre_inf

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

  //: camera origin
  float4 ray_o=(float4)local_copy_cam[2].s4567;
  ray_o.w=1.0f;
  
  //: get image coordinates
  int i=0,j=0;
  map_work_space_2d(&i,&j);

  //: rootlevel of the trees.
  int rootlevel=(*root_level);

  //: check to see if the thread corresponds to an actual pixel as in some cases #of threads will be more than the pixels.
  if (i>=(*local_copy_imgdims).z && j>=(*local_copy_imgdims).w)
  {
    in_image[j*get_global_size(0)+i]=rgbaFloatToInt((float4)(0.0,0.0,0.0,0.0));

    return;
  }
  float4 data_return=(float4)(0.0f,1.0f,0.0f,0.0f);
  float tnear = 0.0f, tfar =0.0f;
  float4 ray_d = backproject(i,j,local_copy_cam[0],local_copy_cam[1],local_copy_cam[2],ray_o);

  //: scene origin
  float4 origin=(*scene_origin);
  float4 blockdims=(*block_dims);
  int4 scenedims=(int4)(*scene_dims).xyzw;
  scenedims.w=1;blockdims.w=1; //: for safety purposes.

  float4 cell_min, cell_max;
  float4 entry_pt, exit_pt;
  
  //: scene bounding box
  cell_min=origin;
  cell_max=blockdims*convert_float4(scenedims)+origin;
  int hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
  if (!hit)
	  {
    in_image[j*get_global_size(0)+i]=rgbaFloatToInt((float4)(0.0,0.0,0.0,0.0));
return;
  }
  entry_pt=ray_o + tnear*ray_d;

  int4 curr_block_index=convert_int4((entry_pt-origin)/blockdims);
  if(curr_block_index.x==scenedims.x)
	curr_block_index.x--;
  if(curr_block_index.y==scenedims.y)
	curr_block_index.y--;
  if(curr_block_index.z==scenedims.z)
	curr_block_index.z--;

  curr_block_index.w=0;

  //// combine the following into one
  float cellsize=(float)(1<<rootlevel);
  cellsize=1/cellsize;
  int count=0;
  float4 debug_var;
  while(1)
  {
	  if (any(curr_block_index<(int4)0)|| any(curr_block_index>=(scenedims)))
		  break;

	  // Ray tracing with in each block
	  
		  //: 3-d index to 1-d index
	  int root_ptr = block_ptrs[curr_block_index.z*scenedims.x*scenedims.y
								+curr_block_index.y*scenedims.x
								+curr_block_index.x];

	  //// set the nlevels here
	  short4 root = (short4)(0,0,0,rootlevel);
	  float4 block_origin=blockdims*convert_float4(curr_block_index)+origin;
	  //: ray origin is moved to canonical coordinates
	  float4 local_ray_o= (ray_o-block_origin)/blockdims;
	  int4 root_cell=tree_array[root_ptr];

	  //: canonincal bounding box of the tree
      cell_bounding_box(root, rootlevel+1, &cell_min, &cell_max);

	  if (!cell_entry_point(local_ray_o, ray_d, cell_min, cell_max, &entry_pt))
	    return;


	  short4 entry_loc_code = loc_code(entry_pt, rootlevel);
	  short4 curr_loc_code=(short4)-1;
	  int global_count=0;
	  ////// traverse to leaf cell that contains the entry point
	  int curr_cell_ptr = traverse_force(tree_array, root_ptr, root, entry_loc_code,&curr_loc_code,&global_count);
	  debug_var=(float4)curr_cell_ptr;//convert_float4(entry_loc_code);
	  // this cell is the first pierced by the ray
	  // follow the ray through the cells until no neighbors are found
	  
	  while (1)
	  {
		  //// current cell bounding box
		  cell_bounding_box(curr_loc_code, rootlevel+1, &cell_min, &cell_max);
		  // check to see how close tnear and tfar are
		  int hit = intersect_cell(local_ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
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
			  hit = intersect_cell(local_ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
			  if (hit)
				  entry_pt=local_ray_o + tnear*ray_d;
		  }
		  if (!hit)
			  break;


		  int data_ptr = tree_array[curr_cell_ptr].z;
		  // distance must be multiplied by the dimension of the bounding box
		  float d = (tfar-tnear)*(blockdims.x);
		  // no function pointers in OpenCL (spec 8.6a)
		  // instead, user must provide source with a function named "step_cell"

		  //half16 data=(sample_array[data_ptr]);
		  step_cell_render(sample_array,alpha_array,data_ptr,d,&data_return);

		  //// exit point
		  exit_pt=local_ray_o + tfar*ray_d;
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

	  //: finding the next block

	  //: block bounding box
	  cell_min=blockdims*convert_float4(curr_block_index)+origin;
	  cell_max=cell_min+blockdims;
	  hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);
	  if (!hit)
		  return;
      exit_pt =ray_o + tfar *ray_d;

	  short4 exit_face= cell_exit_face_safe(exit_pt,ray_d, cell_min, cell_max);
	  if (exit_face.x<0) // exit face not defined
		  break;

	  if(exit_face.w==0)
		  curr_block_index=curr_block_index-convert_int4(exit_face);
	  else
		  curr_block_index=curr_block_index+convert_int4(exit_face);

	  curr_block_index.w=0;

	  entry_pt=exit_pt;
  }



  data_return.z+=(1-data_return.w)*0.5f;
  data_return= (float4)data_return.z;
  //// find entry point of overall tree bounding box

  //// note that the following code is application dependent
  //// should have a cleanup functor for expected image
  //// also it is not necessary to have a full float4 as the
  //// output type a single scalar float array is sufficient
  in_image[j*get_global_size(0)+i]=rgbaFloatToInt(data_return);
}

