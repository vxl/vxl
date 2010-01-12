
__kernel
void 
test_loc_code(__global int4* cells, __global float2* cell_data,
              __global int4* results)
{
  short root_level = 3;
  int result_ptr = 0;

  float4 p = (float4)(0.1f, 0.1f, 0.1f, 0.0f);
  short4 code = loc_code(p, root_level);
  results[result_ptr++]= convert_int4(code);

  p = (float4)(0.251f, 0.1f, 0.1f, 0.0f);
  code = loc_code(p, root_level);
  results[result_ptr++]= convert_int4(code);

  p = (float4)(0.1f, 0.251f, 0.1f, 0.0f);
  code = loc_code(p, root_level);
  results[result_ptr++]= convert_int4(code);

  p = (float4)(0.499f, 0.251f, 0.499f, 0.0f);
  code = loc_code(p, root_level);
  results[result_ptr++]= convert_int4(code);

}

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
test_traverse(__global int4* cells, __global float2* cell_data,
              __global int4* results)
{
  short4 code, ncode;
  int n_codes = 0;
  int i= 0;
  //determine the number of codes
  test_codes(&i, &n_codes, &code, &ncode);
  short4 found_loc_code = (short4)(0,0,0,0);
  for(i = 0; i<n_codes; ++i)
    {
      test_codes(&i, &n_codes, &code, &ncode);
      short4 root = (short4)(0,0,0,2);
      int cell_ptr = traverse(cells, 0, root , code, &found_loc_code);
      int4 res = convert_int4(found_loc_code);
      results[2*i]=res;
      res = (int4)cell_ptr;
      results[2*i+1]=res;
    }

}

__kernel
void 
test_traverse_to_level(__global int4* cells, __global float2* cell_data,
                       __global int4* results)
{
  short4 code, ncode;
  int n_codes = 0;
  int i= 0;
  //determine the number of codes
  test_codes(&i, &n_codes, &code, &ncode);
  short4 found_loc_code = (short4)(0,0,0,0);
  for(i = 0; i<n_codes; ++i)
    {
      test_codes(&i, &n_codes, &code, &ncode);
      short4 root = (short4)(0,0,0,2);
      int level = 1;
      int cell_ptr = traverse_to_level(cells, 0, root , code,
                                       level, &found_loc_code);
      int4 res = convert_int4(found_loc_code);
      results[2*i]=res;
      res = (int4)cell_ptr;
      results[2*i+1]=res;
    }
}
__kernel
void 
test_traverse_force(__global int4* cells, __global float2* cell_data,
                    __global int4* results)
{
  int result_ptr = 0;
  short4 found_loc_code = (short4)(0,0,0,0);
  short4 start_code = (short4)(0,0,0,1);

  short4 target_code = (short4)(2,0,0,0);
  int cell_ptr = 
    traverse_force(cells, 1, start_code, target_code, &found_loc_code);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;

  target_code = (short4)(0,2,0,0);
  cell_ptr = 
    traverse_force(cells, 1, start_code, target_code, &found_loc_code);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;

  target_code = (short4)(0,0,2,0);
  cell_ptr = 
    traverse_force(cells, 1, start_code, target_code, &found_loc_code);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;

  target_code = (short4)(2,2,2,0);
  cell_ptr = 
    traverse_force(cells, 1, start_code, target_code, &found_loc_code);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;
}
__kernel
void 
test_cell_bounding_box(__global int4* cells, __global float2* cell_data,
                       __global int4* results)
{
  short4 code, ncode;
  int n_codes = 0;
  int i= 0;
  int n_levels = 3;
  //determine the number of codes
  test_codes(&i, &n_codes, &code, &ncode);
  for(i = 0; i<n_codes; ++i)
    {
      test_codes(&i, &n_codes, &code, &ncode);
      float4 cell_min, cell_max;
      cell_bounding_box(code, n_levels, &cell_min, &cell_max);
      float4 scale = (float4)(1000.0f);
      int4 res = convert_int4(cell_min*scale);
      results[2*i]=res;
      res = convert_int4(cell_max*scale);
      results[2*i+1]=res;
    }
}

__kernel
void 
test_common_ancestor(__global int4* cells, __global float2* cell_data,
                     __global int4* results)
{
  short4 code, ncode;
  int n_codes = 0;
  int i= 0;
  //determine the number of codes
  test_codes(&i, &n_codes, &code, &ncode);
  short4 ancestor_loc_code = (short4)(0,0,0,0);
  for(i = 0; i<n_codes; ++i)
    {
      test_codes(&i, &n_codes, &code, &ncode);
      int start_ptr = 9+8*i;
      int ancestor_ptr = common_ancestor(cells, start_ptr,
                                         code, ncode,
                                         &ancestor_loc_code);
      
      results[2*i] = convert_int4(ancestor_loc_code);
      results[2*i+1] = (int4)ancestor_ptr;
    }
}

__kernel
void 
test_cell_exit_face(__global int4* cells, __global float2* cell_data,
                     __global int4* results)
{
  int n_levels = 3;
  int result_ptr = 0;
  short4 code = (short4)(0,0,0,0);
  float4 cell_min, cell_max;
  cell_bounding_box(code, n_levels, &cell_min, &cell_max);

  float4 exit_point = (float4)(cell_min.x, 0.01f, 0.01f, 0.0f);
  short4 exit_face = cell_exit_face(exit_point, cell_min, cell_max);
  results[result_ptr++] = convert_int4(exit_face);

  exit_point = (float4)(cell_max.x, 0.01f, 0.01f, 0.0f);
  exit_face = cell_exit_face(exit_point, cell_min, cell_max);
  results[result_ptr++] = convert_int4(exit_face);

  exit_point = (float4)(0.01f, cell_min.y, 0.01f, 0.0f);
  exit_face = cell_exit_face(exit_point, cell_min, cell_max);
  results[result_ptr++] = convert_int4(exit_face);

  exit_point = (float4)(0.01f, cell_max.y, 0.01f, 0.0f);
  exit_face = cell_exit_face(exit_point, cell_min, cell_max);
  results[result_ptr++] = convert_int4(exit_face);

  exit_point = (float4)(0.01f, 0.01f, cell_min.z, 0.0f);
  exit_face = cell_exit_face(exit_point, cell_min, cell_max);
  results[result_ptr++] = convert_int4(exit_face);

  exit_point = (float4)(0.01f, 0.01f, cell_max.z, 0.0f);
  exit_face = cell_exit_face(exit_point, cell_min, cell_max);
  results[result_ptr++] = convert_int4(exit_face);
}
__kernel
void 
test_neighbor(__global int4* cells, __global float2* cell_data,
              __global int4* results)
{
  short4 faces[6];
  faces[0]=X_MIN;   faces[1]=X_MAX;
  faces[2]=Y_MIN;   faces[3]=Y_MAX;
  faces[4]=Z_MIN;   faces[5]=Z_MAX;
  int n_levels = 3;
  int cell_ptr = 9;
  int result_ptr = 0;
  // test the lower left cell, which has no neighbors on the min faces
  short4 code = (short4)(0,0,0,0);
  for(int j=0; j<6; ++j){
    short4 eface = faces[j];
    short4 neighbor_code = (short4)-1;

    int neighbor_ptr = neighbor(cells, cell_ptr, code,
                                eface, n_levels , &neighbor_code);
    results[result_ptr++]=convert_int4(neighbor_code);
    
    results[result_ptr++]=(int4)neighbor_ptr;
  }
  // test a cell having all neighbors
  code = (short4)(1,1,1,0);
  cell_ptr = 16;
  for(int j=0; j<6; ++j){
    short4 eface = faces[j];
    short4 neighbor_code = (short4)-1;

    int neighbor_ptr = neighbor(cells, cell_ptr, code,
                                eface, n_levels , &neighbor_code);
    results[result_ptr++]=convert_int4(neighbor_code);
    
    results[result_ptr++]=(int4)neighbor_ptr;
  }
}

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
  for(int i = 0; i<n_rays; ++i){
    //bounding box of root
    cell_bounding_box(root, n_levels, &cell_min, &cell_max);
    //find entry point of overall tree bounding box
    if(!cell_entry_point(origin[i], dir[i], cell_min, cell_max, &entry_pt))
      continue;
    short4 entry_loc_code = loc_code(entry_pt, root.w);
    short4 curr_loc_code;
    //traverse to cell that contains the entry point
    int curr_cell_ptr = traverse_force(cells, root_ptr, root, entry_loc_code,
                                       &curr_loc_code);
    //this cell is the first pierced by the ray
    //follow the ray through the cells until no neighbors are found

    int count = 0;
    while(count<1000){
      //current cell bounding box
      cell_bounding_box(curr_loc_code, n_levels, &cell_min, &cell_max);

    int hit = intersect_cell(origin[i], dir[i], cell_min, cell_max,&tnear, &tfar);

    if(fabs(tfar-tnear)<cellsize/100)
    {
        entry_pt=entry_pt+dir[i]*cellsize/2;
        entry_loc_code = loc_code(entry_pt, root.w);
        ////traverse to leaf cell that contains the entry point
        curr_cell_ptr = traverse_force(cells, root_ptr, root, entry_loc_code,&curr_loc_code);

        if(curr_cell_ptr<0)
        {
             break;
        }

        cell_bounding_box(curr_loc_code, n_levels, &cell_min, &cell_max);
        hit = intersect_cell(origin[i], dir[i], cell_min, cell_max,&tnear, &tfar);

        if(tnear>tfar)
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
    if(exit_pt.x>=1.0f||exit_pt.y>=1.0f||exit_pt.z>=1.0f)
        break;
    if(exit_pt.x<=0.0f||exit_pt.y<=0.0f||exit_pt.z<=0.0f)
        break;
    entry_pt = exit_pt;
    //location code of exit point
    short4 exit_loc_code = loc_code(exit_pt, root.w);
    //the exit face mask
    short4 exit_face = cell_exit_face(exit_pt, cell_min, cell_max);
    if(exit_face.x<0)
    {
        //need to traverse from root to get to exit_pt
        curr_cell_ptr = traverse(cells, root_ptr, root, 
            exit_loc_code, &curr_loc_code);
#if 0
        results[result_ptr++]=convert_int4(curr_loc_code);
#endif
        goto done; 
    }
    //find the neighboring cell at the exit face
    int neighbor_ptr =  neighbor(cells, curr_cell_ptr, curr_loc_code,
        exit_face, n_levels, &neighbor_code);
    if(neighbor_ptr<0)
        break;

    //traverse from the neighbor to the cell having the 
    //required exit location code
    curr_cell_ptr = traverse_force(cells, neighbor_ptr, neighbor_code,
        exit_loc_code, &curr_loc_code);

    //the current cell is now the cell reached by the neighbor's traverse
done:
    count++;
    }
  }
}
