void test_codes(int* i, int* n_codes, short4* code, short4* ncode)
{
  short4 codes[8], ncodes[8];
  *n_codes = 8;
  codes[0]=(short4)(0,0,0,0);   ncodes[0]=(short4)(1,0,0,0);
  codes[1]=(short4)(4,0,0,0);   ncodes[1]=(short4)(3,0,0,0);
  codes[2]=(short4)(5,1,1,0);   ncodes[2]=(short4)(0,3,0,0);
  codes[3]=(short4)(6,6,2,0);   ncodes[3]=(short4)(3,3,0,0);
  codes[4]=(short4)(1,1,6,0);   ncodes[4]=(short4)(0,0,3,0);
  codes[5]=(short4)(4,0,4,0);   ncodes[5]=(short4)(3,0,3,0);
  codes[6]=(short4)(4,4,5,0);   ncodes[6]=(short4)(0,3,3,0);
  codes[7]=(short4)(6,6,6,0);   ncodes[7]=(short4)(3,3,3,0);
  (*code) = codes[*i]; (*ncode) = ncodes[*i];
}

__kernel
void
test_ocl_traverse(__global int2* tree, __global int4* results, __local int2* ltree)
{
  //load global tree into local mem
  //event_t eventid = (event_t) 0;
  //event_t e = async_work_group_copy(ltree, tree, (size_t) 137, eventid);
  //wait_group_events (1, &eventid);
  //int4 a = tree[0];
  
  short4 code, ncode;
  int n_codes = 0;
  int i= 0;
  //determine the number of codes
  test_codes(&i, &n_codes, &code, &ncode);
  short4 found_loc_code = (short4)(0,0,0,0);
  int global_count=0;
  int cell_ptr = 0, data_ptr = 0;
  for (i = 0; i<8; ++i)
  {
    test_codes(&i, &n_codes, &code, &ncode);
    short4 root = (short4)(0,0,0,3); //rootlevel is 3 in these bit trees
    for (int k=0;k<10000;k++)
    {
      cell_ptr = traverse_woffset_mod_opt(tree, 0, root, code, &found_loc_code, &global_count, 1000, 0, 0);
      data_ptr = get_data_ptr(tree[cell_ptr]);
      //cell_ptr = traverse(0, ltree, 0, root, code, &found_loc_code, &global_count); 
      results[0] = k; //to stop caching. 
    }
    int4 res = convert_int4(found_loc_code);
    results[2*i]=res;
    res = (int4)cell_ptr;
    results[2*i+1]=res;
  }
}

__kernel
void
test_ocl_traverse_force(__global int2* tree, __global int4* results, __local int2* ltree)
{
/*
  event_t eventid = (event_t) 0;
  event_t e = async_work_group_copy(ltree, tree, (size_t) 137, eventid);
  wait_group_events (1, &eventid);

  int global_count=0;
  //start with (0,0,0,1) and try to find (2,0,0,0)
  // (you can't, you'll just find (1,1,1,0) w/ traverse force
  int result_ptr = 0;
  int cell_ptr   = 0;
  int start_ptr  = 9;
  short4 start_code     = (short4)(0,0,0,1);
  short4 found_loc_code = (short4)(0,0,0,0);
  short4 target_code    = (short4)(2,0,0,0);
  
  //do 10,000 traverse_forces for good measure
  for (int k=0;k<10000;k++)
  {
    cell_ptr = traverse_force_woffset_mod_opt(tree, 0, root, code, &found_loc_code, &global_count, 1000, 0, 0);
    //cell_ptr = traverse_force(0, ltree, start_ptr, start_code, target_code, &found_loc_code, &global_count);
  }
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;

  target_code = (short4)(0,2,0,0);
  cell_ptr = traverse_force(0, ltree, start_ptr, start_code, target_code, &found_loc_code, &global_count);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;

  target_code = (short4)(0,0,2,0);
  cell_ptr = traverse_force(0, ltree, start_ptr, start_code, target_code, &found_loc_code, &global_count);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;

  target_code = (short4)(2,2,2,0);
  cell_ptr = traverse_force(0, ltree, start_ptr, start_code, target_code, &found_loc_code, &global_count);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;
*/

}
