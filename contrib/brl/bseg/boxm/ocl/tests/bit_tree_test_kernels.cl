
__kernel
void
test_loc_code(__global int4* cells, __global int4* results)
{
  short root_level = 3;
  int result_ptr = 0;
  float4 p = (float4)(0.1f, 0.1f, 0.1f, 0.0f);
  for (int k=0;k<10000;k++)
  {
    short4 code = loc_code(p, root_level);
  }
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

__kernel
void
test_bit_at(__global uchar16* tree, __global int4* results, __local uchar* ltree)
{
  //load global tree into local mem
  uchar16 tbuff = (*tree);
  ltree[0] = tbuff.s0; ltree[1] = tbuff.s1; ltree[2] = tbuff.s2; ltree[3] = tbuff.s3; 
  ltree[4] = tbuff.s4; ltree[5] = tbuff.s5; ltree[6] = tbuff.s6; ltree[7] = tbuff.s7; 
  ltree[8] = tbuff.s8; ltree[9] = tbuff.s9; ltree[10] = tbuff.sa;ltree[11] = tbuff.sb; 
  ltree[12] = tbuff.sc;ltree[13] = tbuff.sd;ltree[14] = tbuff.se;ltree[15] = tbuff.sf;   
  
  //grab the first 8 1's
  for(int i=0; i<16; i++) {
    results[i] = (int4) (tree_bit_at(ltree, 4*i),  tree_bit_at(ltree, 4*i+1), 
                      tree_bit_at(ltree, 4*i+2),tree_bit_at(ltree, 4*i+3));
  }
}

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
test_traverse(__global uchar16* tree, __global int4* results, __local uchar* ltree)
{
  //load global tree into local mem
  uchar16 tbuff = (*tree);
  ltree[0] = tbuff.s0; ltree[1] = tbuff.s1; ltree[2] = tbuff.s2; ltree[3] = tbuff.s3; 
  ltree[4] = tbuff.s4; ltree[5] = tbuff.s5; ltree[6] = tbuff.s6; ltree[7] = tbuff.s7; 
  ltree[8] = tbuff.s8; ltree[9] = tbuff.s9; ltree[10] = tbuff.sa;ltree[11] = tbuff.sb; 
  ltree[12] = tbuff.sc;ltree[13] = tbuff.sd;ltree[14] = tbuff.se;ltree[15] = tbuff.sf;   
  
  short4 code, ncode;
  int n_codes = 0;
  int i= 0;
  //determine the number of codes
  test_codes(&i, &n_codes, &code, &ncode);
  short4 found_loc_code = (short4)(0,0,0,0);
  int global_count=0;
  int cell_ptr =0;
  for (i = 3; i<4; ++i)
  {
    test_codes(&i, &n_codes, &code, &ncode);
    short4 root = (short4)(0,0,0,3); //rootlevel is 3 in these bit trees
    for (int k=0;k<1;k++)
    {
      cell_ptr = traverse(ltree, 0, root, code, &found_loc_code, &global_count); 
    }
    int4 res = convert_int4(found_loc_code);
    results[2*i]=res;
    res = (int4)cell_ptr;
    results[2*i+1]=res;
  }
  
  
}

__kernel
void
test_traverse_force(__global uchar16* tree, __global int4* results, __local uchar* ltree)
{
  int global_count=0;
  
  //load global tree into local mem
  uchar16 tbuff = (*tree);
  ltree[0] = tbuff.s0; ltree[1] = tbuff.s1; ltree[2] = tbuff.s2; ltree[3] = tbuff.s3; 
  ltree[4] = tbuff.s4; ltree[5] = tbuff.s5; ltree[6] = tbuff.s6; ltree[7] = tbuff.s7; 
  ltree[8] = tbuff.s8; ltree[9] = tbuff.s9; ltree[10] = tbuff.sa;ltree[11] = tbuff.sb; 
  ltree[12] = tbuff.sc;ltree[13] = tbuff.sd;ltree[14] = tbuff.se;ltree[15] = tbuff.sf;  
  
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
    cell_ptr = traverse_force(ltree, start_ptr, start_code, target_code, &found_loc_code, &global_count);
  }
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;

  target_code = (short4)(0,2,0,0);
  cell_ptr = traverse_force(ltree, start_ptr, start_code, target_code, &found_loc_code, &global_count);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;

  target_code = (short4)(0,0,2,0);
  cell_ptr = traverse_force(ltree, start_ptr, start_code, target_code, &found_loc_code, &global_count);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;

  target_code = (short4)(2,2,2,0);
  cell_ptr = traverse_force(ltree, start_ptr, start_code, target_code, &found_loc_code, &global_count);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;

}
