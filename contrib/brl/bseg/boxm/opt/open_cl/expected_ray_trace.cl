
__kernel 
void 
expected_ray_trace(__global float4  *origin,
                   __global float16 *svd_UtVW,
                   __global int4    *cells, 
                   __global float16 *cell_data, 
                   __global uint4   *imgdims,
                   __global uint4   *roidims,
                   __global float4  *global_bbox,
                   __global float4  *inp,
                   __global float4  *results)
{
  float tnear = 0, tfar =0;

  int neighbor_ptr = 0;
  int data_ptr = 0;
  short4 neighbor_code = (short4)-1;
  int root_ptr = 0;
  int n_levels = 10;
  float cellsize=(float)(1<<n_levels-1);
  cellsize=1/cellsize;
  short4 root = (short4)(0,0,0,n_levels-1);

  float4 cell_min, cell_max;
  float4 entry_pt, exit_pt;
  int result_ptr = 0;
  //// start ray trace
  unsigned gid = get_global_id(0);
  unsigned lid = get_local_id(0);
  unsigned bid = get_group_id(0);

   

  float16 Ut   =svd_UtVW[0];  // conjugate transpose of U
  float16 V    =svd_UtVW[1];  // V
  float16 W    =svd_UtVW[2];  // Winv(first4) 
  float4 ray_o =origin[0];    // ray_origin
 
  uint4 roi=(uint4)((*roidims).xyzw);
  uint4 img_bb=(uint4)((*imgdims).xyzw);

  float4 bbox=(*global_bbox);

  unsigned i   = gid/img_bb.y;
  unsigned j   = gid-i*img_bb.y;

  float4 data_return=(float4) (inp[gid]);

  if(i<roi.x || i> roi.y || j<roi.z || j> roi.w)
      return;

  float4 ray_d = backproject(i,j,Ut,V,W,ray_o);


  //bounding box of root
  cell_bounding_box(root, n_levels, &cell_min, &cell_max);

  ray_o=(ray_o - bbox)/bbox.w;
  ray_o.w=1.0;
  //find entry point of overall tree bounding box
  if(!cell_entry_point(ray_o, ray_d, cell_min, cell_max, &entry_pt))
    return;


  short4 entry_loc_code = loc_code(entry_pt, root.w);
  short4 curr_loc_code;
  ////traverse to leaf cell that contains the entry point

  int curr_cell_ptr = traverse_force(cells, root_ptr, root, entry_loc_code,
                                     &curr_loc_code);

  //this cell is the first pierced by the ray
  //follow the ray through the cells until no neighbors are found

  int count = 0;
  float sum = 0.0f;
  float psum = 0.0f;

  while(count<1000){
    ////current cell bounding box
    cell_bounding_box(curr_loc_code, n_levels, &cell_min, &cell_max);


    // check to see how close tnear and tfar are
    int hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);

    if(fabs(tfar-tnear)<cellsize/100)
    {
        entry_pt=entry_pt+ray_d*cellsize/2;
        entry_loc_code = loc_code(entry_pt, root.w);
        ////traverse to leaf cell that contains the entry point
        curr_cell_ptr = traverse_force(cells, root_ptr, root, entry_loc_code,&curr_loc_code);

        if(curr_cell_ptr<0)
        {
             break;
        }

        cell_bounding_box(curr_loc_code, n_levels, &cell_min, &cell_max);
        hit = intersect_cell(ray_o, ray_d, cell_min, cell_max,&tnear, &tfar);

        if(tnear>tfar)
        {
            break;
        }
        entry_pt=ray_o + tnear*ray_d;

    }
    else if (tnear>tfar)
        break;
    //// exit point
    exit_pt=ray_o + tfar*ray_d;

    //exit point of ray (if not found, terminate ray)
    //if(!cell_exit_point(ray_o, ray_d, cell_min, cell_max, &exit_pt)){
    //    break;
    //}

    ////////////////////////////////////////////////////////
    // the place where the ray trace function can be applied
    data_ptr = cells[curr_cell_ptr].z;
    if(data_ptr>=0)
    {
      float d = distance(entry_pt, exit_pt);
      //data_return.x+=d;
      // multiply distance by the dimension of the bounding box
      expected_image_funct(cell_data, data_ptr, d*bbox.w, &data_return);
    }
    else break; 
    //////////////////////////////////////////////////////////

    //if the ray pierces the volume surface then terminate the ray
    if(exit_pt.x>=1.0f||exit_pt.y>=1.0f||exit_pt.z>=1.0f)
      break;
    if(exit_pt.x<=0.0f||exit_pt.y<=0.0f||exit_pt.z<=0.0f)
      break;
    short4 entry_face = cell_exit_face(entry_pt, cell_min, cell_max);

    //ray continues: make the current entry point the previous exit point
    entry_pt = exit_pt;

    //location code of exit point
    short4 exit_loc_code = loc_code(exit_pt, root.w);
    //the exit face mask
    //short4 exit_face = cell_exit_face_but_not_entry_face(exit_pt, cell_min, cell_max,entry_face);
    short4 exit_face = cell_exit_face(exit_pt, cell_min, cell_max);

    if(exit_face.x<0)//exit face not defined
      {
        //need to traverse from root to get to exit_pt
        curr_cell_ptr = traverse(cells, root_ptr, root,exit_loc_code, &curr_loc_code);
        goto done; 
      }

    //find the neighboring cell at the exit face
    neighbor_ptr =   neighbor(cells, curr_cell_ptr, curr_loc_code,exit_face, n_levels, &neighbor_code);

    //if no neighbor then terminate ray
    if(neighbor_ptr<0)
    {
      break;
    }
    //traverse from the neighbor to the cell having the 
    //required exit location code
    curr_cell_ptr = traverse_force(cells, neighbor_ptr, neighbor_code,exit_loc_code, &curr_loc_code);
      
    //the current cell (cells[curr_cell_ptr])is the cell reached by 
    //the neighbor's traverse

     //data_return=data_return+2;
  done:
    count++;
  }
  // note that the following code is application dependent
  // should have a cleanup functor for expected image
  // also it is not necessary to have a full float4 as the 
  // output type a single scalar float array is sufficient

  results[gid] = (float4)(data_return);

  //end ray trace

}

