

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
	int global_count=0;
	//determine the number of codes
	test_codes(&i, &n_codes, &code, &ncode);
	uint lid=get_local_id(0);
	uint workgrpsize=get_local_size(0);

	short4 found_loc_code = (short4)(0,0,0,0);
	for (i = 0; i<n_codes; ++i)
	{
		test_codes(&i, &n_codes, &code, &ncode);
		short4 root = (short4)(0,0,0,2);
		int stack_ptr=0;
		stack[lid]=0;
		int stack_index=0;
		for(int k=0;k<10000;k++)
		{
			int temp_stack_ptr = traverse_stack(cells,  root , code, &found_loc_code,lid,workgrpsize, stack,stack_ptr,&global_count);
		}
		found_loc_code = (short4)(0,0,0,0);
		stack_ptr = traverse_stack(cells,  root , code, &found_loc_code,lid,workgrpsize, stack,stack_ptr,&global_count);
		stack_index=lid +workgrpsize*stack_ptr;

		int4 res = convert_int4(found_loc_code);
		results[3*i]=res;
		res = (int4)stack[stack_index];//cell_ptr;
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
  uint workgrpsize=get_local_size(0);

  int global_count=0;
  //determine the number of codes
  test_codes(&i, &n_codes, &code, &ncode);
  short4 found_loc_code = (short4)(0,0,0,0);
  for (i = 0; i<n_codes; ++i)
  {
    int stack_ptr=0;
    stack[lid]=0;
    int stack_index=0;
    test_codes(&i, &n_codes, &code, &ncode);
    short4 root = (short4)(0,0,0,2);
    int level = 1;
	//: # of iterations is fixed right now but hsould be a user input.
	for(int k=0;k<10000;k++)
	{
		int temp_stack_ptr = traverse_to_level_stack(cells,  root , code,level, &found_loc_code, 
                                       lid,workgrpsize, stack,stack_ptr,&global_count);
	}

    stack_ptr= traverse_to_level_stack(cells,  root , code,level, &found_loc_code, 
                                       lid,workgrpsize, stack,stack_ptr,&global_count);
    int4 res = convert_int4(found_loc_code);
    stack_index=lid +workgrpsize*stack_ptr;

    results[3*i]=res;
    res = (int4)stack[stack_index];
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
  uint workgrpsize=get_local_size(0);

  short4 found_loc_code = (short4)(0,0,0,0);
  short4 start_code = (short4)(0,0,0,1);

  uint lid=get_local_id(0);
  short4 target_code = (short4)(2,0,0,0);

  int stack_ptr=0;
  int global_count=0;
  stack[lid +workgrpsize*stack_ptr]=1;
  int stack_index=0;
 
  for( int k=0;k<10000;k++)
  {
	  int temp_stack_ptr = traverse_force_stack(cells, start_code, target_code, &found_loc_code,stack,lid,workgrpsize,stack_ptr,&global_count);
  }
  stack_ptr = traverse_force_stack(cells, start_code, target_code, &found_loc_code,stack,lid,workgrpsize,stack_ptr,&global_count);
  stack_index=lid +workgrpsize*stack_ptr;

  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)stack[stack_index];
  results[result_ptr++]=(int4)stack_ptr;

  target_code = (short4)(0,2,0,0);
  stack_ptr=0;
  stack[lid +workgrpsize*stack_ptr]=1;
   stack_index=0;
  stack_ptr =
    traverse_force_stack(cells,  start_code, target_code, &found_loc_code,stack,lid,workgrpsize,stack_ptr,&global_count);
  stack_index=lid +workgrpsize*stack_ptr;

  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)stack[stack_index];
  results[result_ptr++]=(int4)stack_ptr;


  target_code = (short4)(0,0,2,0);
  stack_ptr=0;
  stack[lid +workgrpsize*stack_ptr]=1;
  stack_index=0;
  stack_ptr =
    traverse_force_stack(cells,  start_code, target_code, &found_loc_code,stack,lid,workgrpsize,stack_ptr,&global_count);
  stack_index=lid +workgrpsize*stack_ptr;

  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)stack[stack_index];
  results[result_ptr++]=(int4)stack_ptr;

  target_code = (short4)(2,2,2,0);
  stack_ptr=0;
  stack[lid +workgrpsize*stack_ptr]=1;
   stack_index=0;
  stack_ptr =
    traverse_force_stack(cells,  start_code, target_code, &found_loc_code,stack,lid,workgrpsize,stack_ptr,&global_count);
  stack_index=lid +workgrpsize*stack_ptr;

  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)stack[stack_index];
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
  int global_count=0;
  uint lid=get_local_id(0);
  uint workgrpsize=get_local_size(0);

  //determine the number of codes
  test_codes(&i, &n_codes, &code, &ncode);
  short4 ancestor_loc_code = (short4)(0,0,0,0);
  short4 root = (short4)(0,0,0,2);

  for (i = 0; i<n_codes; ++i)
  {
    int stack_ptr=0;
    stack[lid]=0;

    test_codes(&i, &n_codes, &code, &ncode);
    short4 found_loc_code;
    stack_ptr = traverse_to_level_stack(cells,  root , code,0, &found_loc_code, lid,workgrpsize, stack,stack_ptr,&global_count);
	for( int k=0;k<10000;k++)
	{		
		int temp_stack_ptr = common_ancestor_stack(code, ncode,&ancestor_loc_code,stack_ptr);
	}
	stack_ptr = common_ancestor_stack(code, ncode,&ancestor_loc_code,stack_ptr);
    int ancestor_ptr=stack[lid+workgrpsize*stack_ptr];
    results[2*i] = convert_int4(ancestor_loc_code);
    results[2*i+1] = (int4)ancestor_ptr;
  }
}

__kernel
void
test_neighbor_stack(__global int4* cells, __global float2* cell_data,
                    __global int4* results,__local int* stack)
{
  short4 faces[6];
  uint lid=get_local_id(0);
  uint workgrpsize=get_local_size(0);
  faces[0]=X_MIN;   faces[1]=X_MAX;
  faces[2]=Y_MIN;   faces[3]=Y_MAX;
  faces[4]=Z_MIN;   faces[5]=Z_MAX;

  int n_levels = 3;
  int result_ptr = 0;
  int global_count=0;
  short4 root = (short4)(0,0,0,2);

  // test the lower left cell, which has no neighbors on the min faces
  short4 code = (short4)(0,0,0,0);
  for (int j=0; j<6; ++j) {
    short4 eface = faces[j];
    short4 neighbor_code = (short4)-1;
    int stack_ptr = 0;
    stack[lid]=0;

    short4 found_loc_code=(short4)-1;

    stack_ptr = traverse_to_level_stack(cells, root , code,0, &found_loc_code, lid,workgrpsize, stack,stack_ptr,&global_count);
    stack_ptr = neighbor_stack(cells,  code,eface, n_levels , &neighbor_code,stack,lid,workgrpsize,stack_ptr,&global_count);
    int stack_index=lid +workgrpsize*stack_ptr;

    results[result_ptr++]=convert_int4(neighbor_code);

    results[result_ptr++]=(int4)stack[stack_index];
  }
  // test a cell having all neighbors
  code = (short4)(1,1,1,0);
  int cell_ptr = 16;
  for (int j=0; j<6; ++j) {
    short4 eface = faces[j];
    short4 neighbor_code = (short4)-1;
    int stack_ptr = 0;
    stack[lid]=0;
    short4 found_loc_code;
    stack_ptr = traverse_to_level_stack(cells,  root , code,0, &found_loc_code, lid,workgrpsize, stack,stack_ptr,&global_count);
	for( int k=0;k<10000;k++)
	{		
		int temp_stack_ptr = neighbor_stack(cells,  code,eface, n_levels , &neighbor_code,stack,lid,workgrpsize,stack_ptr,&global_count);
	}
	stack_ptr = neighbor_stack(cells,  code,eface, n_levels , &neighbor_code,stack,lid,workgrpsize,stack_ptr,&global_count);
    int stack_index=lid +workgrpsize*stack_ptr;

    results[result_ptr++]=convert_int4(neighbor_code);

    results[result_ptr++]=(int4)stack[stack_index];
  }
}

