

void test_codes(int* i, int* n_codes, short4* code, short4* ncode)
{
  short4 codes[8], ncodes[8];
  *n_codes = 8;
  codes[0]=(short4)(0,0,0,0);   ncodes[0]=(short4)(1,0,0,0);
  codes[1]=(short4)(2,0,0,0);   ncodes[1]=(short4)(3,0,0,0);
  codes[2]=(short4)(0,2,0,0);   ncodes[2]=(short4)(0,3,0,0);
  codes[3]=(short4)(2,2,0,0);   ncodes[3]=(short4)(3,3,0,0);
  codes[4]=(short4)(0,0,2,0);   ncodes[4]=(short4)(0,0,3,0);
  codes[5]=(short4)(2,0,2,0);   ncodes[5]=(short4)(3,0,3,0);
  codes[6]=(short4)(0,2,2,0);   ncodes[6]=(short4)(0,3,3,0);
  codes[7]=(short4)(2,2,2,0);   ncodes[7]=(short4)(3,3,3,0);
  (*code) = codes[*i]; (*ncode) = ncodes[*i];
}

__kernel
void
test_traverse_stack(__global int4* cells, __global float2* cell_data,
                    __global int4* results,__local int * stack)
{
  short4 code, ncode;
  int n_codes = 0;
  int i= 0;
  //determine the number of codes
  test_codes(&i, &n_codes, &code, &ncode);
  uint lid=get_local_id(0);

  short4 found_loc_code = (short4)(0,0,0,0);
  for (i = 0; i<n_codes; ++i)
  {
    test_codes(&i, &n_codes, &code, &ncode);
    short4 root = (short4)(0,0,0,2);
    int stack_ptr= -1;
    int cell_ptr = traverse_stack(cells, 0, root , code, &found_loc_code,lid,3-1, stack,&stack_ptr);
    int4 res = convert_int4(found_loc_code);
    results[3*i]=res;
    res = (int4)cell_ptr;
    results[3*i+1]=res;
    results[3*i+2]=(int4)stack_ptr;
  }
}


__kernel
void
test_traverse_to_level_stack(__global int4* cells, __global float2* cell_data,
                             __global int4* results,__local int* stack)
{
  short4 code, ncode;
  int n_codes = 0;
  int i= 0;
  uint lid=get_local_id(0);
  //determine the number of codes
  test_codes(&i, &n_codes, &code, &ncode);
  short4 found_loc_code = (short4)(0,0,0,0);
  for (i = 0; i<n_codes; ++i)
  {
    int stack_ptr=-1;
    test_codes(&i, &n_codes, &code, &ncode);
    short4 root = (short4)(0,0,0,2);
    int level = 1;
    int cell_ptr = traverse_to_level_stack(cells, 0, root , code,
                                           level, &found_loc_code, lid,3-1, stack,&stack_ptr);
    int4 res = convert_int4(found_loc_code);
    results[3*i]=res;
    res = (int4)cell_ptr;
    results[3*i+1]=res;
    results[3*i+2]=(int4)stack_ptr;
  }
}


__kernel
void
test_traverse_force_stack(__global int4* cells, __global float2* cell_data,
                          __global int4* results,__local int* stack)
{
  int result_ptr = 0;
  short4 found_loc_code = (short4)(0,0,0,0);
  short4 start_code = (short4)(0,0,0,1);

  uint lid=get_local_id(0);
  int stack_ptr= -1;
  short4 target_code = (short4)(2,0,0,0);
  int cell_ptr =
    traverse_force_stack(cells, 1, start_code, target_code, &found_loc_code,stack,lid,3-1,&stack_ptr);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;
  results[result_ptr++]=(int4)stack_ptr;

  stack_ptr= -1;
  target_code = (short4)(0,2,0,0);
  cell_ptr =
    traverse_force_stack(cells, 1, start_code, target_code, &found_loc_code,stack,lid,3-1,&stack_ptr);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;
  results[result_ptr++]=(int4)stack_ptr;

  stack_ptr= -1;

  target_code = (short4)(0,0,2,0);
  cell_ptr =
    traverse_force_stack(cells, 1, start_code, target_code, &found_loc_code,stack,lid,3-1,&stack_ptr);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;
  results[result_ptr++]=(int4)stack_ptr;

  stack_ptr= -1;

  target_code = (short4)(2,2,2,0);
  cell_ptr =
    traverse_force_stack(cells, 1, start_code, target_code, &found_loc_code,stack,lid,3-1,&stack_ptr);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;
  results[result_ptr++]=(int4)stack_ptr;
}

__kernel
void
test_common_ancestor_stack(__global int4* cells, __global float2* cell_data,
                           __global int4* results,__local int* stack)
{
  short4 code, ncode;
  int n_codes = 0;
  int i= 0;
  uint lid=get_local_id(0);

  //determine the number of codes
  test_codes(&i, &n_codes, &code, &ncode);
  short4 ancestor_loc_code = (short4)(0,0,0,0);
  short4 root = (short4)(0,0,0,2);

  for (i = 0; i<n_codes; ++i)
  {
    int stack_ptr= -1;

    test_codes(&i, &n_codes, &code, &ncode);
    short4 found_loc_code;
    int cell_ptr = traverse_to_level_stack(cells, 0, root , code,0, &found_loc_code, lid,3-1, stack,&stack_ptr);

    int start_ptr = 9+8*i;
    int ancestor_ptr = common_ancestor_stack(cells, start_ptr,
                                             code, ncode,
                                             &ancestor_loc_code,&stack_ptr);
    ancestor_ptr=stack[lid*2+stack_ptr];
    results[2*i] = convert_int4(ancestor_loc_code);
    results[2*i+1] = (int4)ancestor_ptr;
    //results[2*i+2] = (int4)stack_ptr;
  }
}

__kernel
void
test_neighbor_stack(__global int4* cells, __global float2* cell_data,
                    __global int4* results,__local int* stack)
{
  short4 faces[6];
  uint lid=get_local_id(0);

  faces[0]=X_MIN;   faces[1]=X_MAX;
  faces[2]=Y_MIN;   faces[3]=Y_MAX;
  faces[4]=Z_MIN;   faces[5]=Z_MAX;
  int n_levels = 3;
  //int cell_ptr = 9;
  int result_ptr = 0;
  short4 root = (short4)(0,0,0,2);

  // test the lower left cell, which has no neighbors on the min faces
  short4 code = (short4)(0,0,0,0);
  for (int j=0; j<6; ++j) {
    short4 eface = faces[j];
    short4 neighbor_code = (short4)-1;
    int stack_ptr = -1;
    short4 found_loc_code=(short4)-1;

    int cell_ptr = traverse_to_level_stack(cells, 0, root , code,0, &found_loc_code, lid,3-1, stack,&stack_ptr);
    int neighbor_ptr = neighbor_stack(cells, cell_ptr, code,eface, n_levels , &neighbor_code,stack,lid,&stack_ptr);
    results[result_ptr++]=convert_int4(neighbor_code);

    results[result_ptr++]=(int4)neighbor_ptr;
  }
  // test a cell having all neighbors
  code = (short4)(1,1,1,0);
  int cell_ptr = 16;
  for (int j=0; j<6; ++j) {
    short4 eface = faces[j];
    short4 neighbor_code = (short4)-1;
    int stack_ptr = -1;
    short4 found_loc_code;

    int cell_ptr = traverse_to_level_stack(cells, 0, root , code,0, &found_loc_code, lid,3-1, stack,&stack_ptr);

    int neighbor_ptr = neighbor_stack(cells, cell_ptr, code,eface, n_levels , &neighbor_code,stack,lid,&stack_ptr);
    results[result_ptr++]=convert_int4(neighbor_code);
    results[result_ptr++]=neighbor_ptr;
  }
}

#if 0 // function commented out
__kernel
void
test_ray_trace(__global int4* cells, __global float2* cell_data,
               __global int4* results)
{
  int n_rays = 6;
  float4 origin[6];
  float4 dir[6];

  float tnear = 0, tfar =0;
  //rays normal to cell faces
  origin[0]= (float4)(0.125f, 0.125f, 10.0f, 0.0f);
  origin[1]= (float4)(-10.0f, 0.125f, 0.125f, 0.0f);
  origin[2]= (float4)(0.125f, -10.0f, 0.125f, 0.0f);
  //diagonal rays
  origin[3]= (float4)(10.0f, 10.0f, 10.0f, 0.0f);
  origin[4]= (float4)(-10.0f, 11.0f, 11.0f, 0.0f);
  origin[5]= (float4)(11.0f, -10.0f, 11.0f, 0.0f);
  //rays normal to cell faces
  dir[0]=(float4)(0.0f, 0.0f, -1.0f, 0.0f);
  dir[1]=(float4)(1.0f, 0.0f, 0.0f, 0.0f);
  dir[2]=(float4)(0.0f, 1.0f, 0.0f, 0.0f);
  //diagonal rays
  dir[3]=(float4)(-0.577350f, -0.577350f,  -0.577350f, 0.0f);
  dir[4]=(float4)(0.577350f, -0.577350f,  -0.577350f, 0.0f);
  dir[5]=(float4)(-0.577350f, 0.577350f,  -0.577350f, 0.0f);
  //initialize at root
  short4 root = (short4)(0,0,0,2);
  short4 neighbor_code = (short4)-1;
  int root_ptr = 0;
  int n_levels = 3;
  float cellsize=(float)(1<<n_levels-1);
  cellsize=1/cellsize;
  float4 cell_min, cell_max;
  float4 entry_pt, exit_pt;
  int result_ptr = 0;
  for (int i = 0; i<n_rays; ++i) {
    //bounding box of root
    cell_bounding_box(root, n_levels, &cell_min, &cell_max);
    //find entry point of overall tree bounding box
    if (!cell_entry_point(origin[i], dir[i], cell_min, cell_max, &entry_pt))
      continue;
    short4 entry_loc_code = loc_code(entry_pt, root.w);
    short4 curr_loc_code;
    //traverse to cell that contains the entry point
    int curr_cell_ptr = traverse_force(cells, root_ptr, root, entry_loc_code,
                                       &curr_loc_code);
    //this cell is the first pierced by the ray
    //follow the ray through the cells until no neighbors are found

    int count = 0;
    while (count<1000) {
      //current cell bounding box
      cell_bounding_box(curr_loc_code, n_levels, &cell_min, &cell_max);

      int hit = intersect_cell(origin[i], dir[i], cell_min, cell_max,&tnear, &tfar);

      if ( fabs(tfar-tnear)<cellsize/100)
      {
        entry_pt=entry_pt+dir[i]*cellsize/2;
        entry_loc_code = loc_code(entry_pt, root.w);
        ////traverse to leaf cell that contains the entry point
        curr_cell_ptr = traverse_force(cells, root_ptr, root, entry_loc_code,&curr_loc_code);

        if (curr_cell_ptr<0)
        {
          break;
        }

        cell_bounding_box(curr_loc_code, n_levels, &cell_min, &cell_max);
        hit = intersect_cell(origin[i], dir[i], cell_min, cell_max,&tnear, &tfar);

        if (tnear>tfar)
        {
          break;
        }
        entry_pt=origin[i] + tnear*dir[i];
      }
      else if (tnear>tfar)
        break;
      //// exit point
      exit_pt=origin[i] + tfar*dir[i];

      ////////////////////////////////////////////////////////
      // the place where the ray trace function can be applied
      results[result_ptr++]=convert_int4(curr_loc_code);
      float d = distance(entry_pt, exit_pt);
      results[result_ptr++]=convert_int4(1000.0f*(float4)d);
      ////////////////////////////////////////////////////////
      if (exit_pt.x>=1.0f||exit_pt.y>=1.0f||exit_pt.z>=1.0f)
        break;
      if (exit_pt.x<=0.0f||exit_pt.y<=0.0f||exit_pt.z<=0.0f)
        break;
      entry_pt = exit_pt;
      //location code of exit point
      short4 exit_loc_code = loc_code(exit_pt, root.w);
      //the exit face mask
      short4 exit_face = cell_exit_face(exit_pt, cell_min, cell_max);
      if (exit_face.x<0)
      {
        //need to traverse from root to get to exit_pt
        curr_cell_ptr = traverse(cells, root_ptr, root,
                                 exit_loc_code, &curr_loc_code);
#if 0
        results[result_ptr++]=convert_int4(curr_loc_code);
#endif
      }
      else {
        //find the neighboring cell at the exit face
        int neighbor_ptr =  neighbor(cells, curr_cell_ptr, curr_loc_code,
                                     exit_face, n_levels, &neighbor_code);
        if (neighbor_ptr<0)
          break;

        //traverse from the neighbor to the cell having the
        //required exit location code
        curr_cell_ptr = traverse_force(cells, neighbor_ptr, neighbor_code,
                                       exit_loc_code, &curr_loc_code);

        //the current cell is now the cell reached by the neighbor's traverse
      }
      count++;
    }
  }
}
#endif // 0
