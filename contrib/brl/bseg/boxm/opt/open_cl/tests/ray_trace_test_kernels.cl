void expected_image_funct(__global float16* cell_data, int data_ptr,
                          float d, float4* data_return)
{

  float16 data = cell_data[data_ptr];
  float alpha = data.s0;
   

  if(alpha<0) return;

  float expected_int_cell = data.s3;
  float alpha_integral = (*data_return).x;
  float vis            = (*data_return).y;
  float expected_int   = (*data_return).z;
  float intensity_norm = (*data_return).w;

  alpha_integral += alpha*d;
  float vis_prob_end = exp(-alpha_integral);
  float omega = vis - vis_prob_end;
  expected_int += expected_int_cell*omega;
  (*data_return).x = alpha_integral;
  (*data_return).y = vis_prob_end;
  (*data_return).z = expected_int;
  (*data_return).w = intensity_norm + omega;

}

__kernel 
void 
test_ray_trace(__global int4* cells, __global float16* cell_data,
               __global float4* origin, __global float4* dir,
               __global float4* results)
{
  float tnear = 0, tfar =0;
  float4 data_return = (float4)(0.0f, 1.0f, 0.0f, 0.0f);
  int neighbor_ptr = 0;
  int data_ptr = 0;
  short4 root = (short4)(0,0,0,8);
  short4 neighbor_code = (short4)-1;
  int root_ptr = 0;
  int n_levels = 9;
  float4 cell_min, cell_max;
  float4 entry_pt, exit_pt;
  int result_ptr = 0;
  // start ray trace
  unsigned gid = get_global_id(0);
  unsigned lid = get_local_id(0);
  unsigned bid = get_group_id(0);
  float4 ray_o = origin[gid];
  float4 ray_d = dir[gid];

  //bounding box of root
  cell_bounding_box(root, n_levels, &cell_min, &cell_max);
  //find entry point of overall tree bounding box
  if(!cell_entry_point(ray_o, ray_d, cell_min, cell_max, &entry_pt))
    return;

  short4 entry_loc_code = loc_code(entry_pt, root.w);
  short4 curr_loc_code;
  //traverse to leaf cell that contains the entry point

  int curr_cell_ptr = traverse_force(cells, root_ptr, root, entry_loc_code,
                                     &curr_loc_code);

  //this cell is the first pierced by the ray
  //follow the ray through the cells until no neighbors are found

  int count = 0;
  float sum = 0.0f;
  float psum = 0.0f;

  while(count<1000){
    //current cell bounding box
    cell_bounding_box(curr_loc_code, n_levels, &cell_min, &cell_max);

    //exit point of ray (if not found, terminate ray)
    if(!cell_exit_point(ray_o, ray_d, cell_min, cell_max, &exit_pt)){
      break;
    }
    ////////////////////////////////////////////////////////
    // the place where the ray trace function can be applied

    data_ptr = cells[curr_cell_ptr].z;
    if(data_ptr>=0){
      float d = distance(entry_pt, exit_pt);
      expected_image_funct(cell_data, data_ptr, d, &data_return);
    }
    ////////////////////////////////////////////////////////

    //if the ray pierces the volume surface then terminate the ray
    if(exit_pt.x>=1.0f||exit_pt.y>=1.0f||exit_pt.z>=1.0f)
      break;
    if(exit_pt.x<=0.0f||exit_pt.y<=0.0f||exit_pt.z<=0.0f)
      break;

    //ray continues: make the current entry point the previous exit point
    entry_pt = exit_pt;

    //location code of exit point
    short4 exit_loc_code = loc_code(exit_pt, root.w);
    //the exit face mask
    short4 exit_face = cell_exit_face(exit_pt, cell_min, cell_max);

    if(exit_face.x<0)//exit face not defined
      {
        //need to traverse from root to get to exit_pt
        curr_cell_ptr = traverse(cells, root_ptr, root, 
                                 exit_loc_code, &curr_loc_code);
        goto done; 
      }

    //find the neighboring cell at the exit face
    neighbor_ptr =   neighbor(cells, curr_cell_ptr, curr_loc_code,
                                 exit_face, n_levels, &neighbor_code);

    //if no neighbor then terminate ray
    if(neighbor_ptr<0)
      break;

    //traverse from the neighbor to the cell having the 
    //required exit location code
    curr_cell_ptr = traverse_force(cells, neighbor_ptr, neighbor_code,
                                   exit_loc_code, &curr_loc_code);
      
    //the current cell (cells[curr_cell_ptr])is the cell reached by 
    //the neighbor's traverse

  done:
    count++;
  }
  // note that the following code is application dependent
  // should have a cleanup functor for expected image
  // also it is not necessary to have a full float4 as the 
  // output type a single scalar float array is sufficient

  //output data
  float expected = data_return.z;
  float norm = data_return.w;
  if(norm>0)
    expected /= norm;
  else
    expected = 0.0f;
  results[gid] = (float4)expected;
  //end ray trace

}

