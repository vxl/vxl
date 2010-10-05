
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
    results[i] = (int4) (tree_bit_at(0, ltree, 4*i),  tree_bit_at(0, ltree, 4*i+1), 
                      tree_bit_at(0, ltree, 4*i+2),tree_bit_at(0, ltree, 4*i+3));
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
test_traverse(__global int4* tree, __global int4* results, __global float4* bbox_results, __local uchar* ltree, __constant uchar * bit_lookup)
{
  
  //event_t eventid = (event_t) 0;
  //event_t e = async_work_group_copy(ltree, tree, (size_t)1, eventid);
  //wait_group_events (1, &eventid);  
  short4 code, ncode;
  int n_codes = 0;
  int i= 0;
  //determine the number of codes
  test_codes(&i, &n_codes, &code, &ncode);
  short4 found_loc_code = (short4)(0,0,0,0);
  int global_count=0;
  int cell_ptr =0, data_ptr=0, cell_opt= 0;
  float4 p = (float4) 1.0;
  float4 cell_min = (float4) 0.0;
  float4 cell_max = (float4) 0.0;
  float4 cell_min_opt = (float4) 0.0;
  float4 cell_max_opt = (float4) 0.0;
  
  for (i = 0; i<8; ++i)
  {
    test_codes(&i, &n_codes, &code, &ncode);
    short4 root = (short4)(0,0,0,3); //rootlevel is 3 in these bit trees
    for (int k=0;k<10000;k++)
    {
      int4 ibuff = (*tree);
      uchar16 tbuff = as_uchar16(ibuff);
      ltree[0] = tbuff.s0; ltree[1] = tbuff.s1; ltree[2] = tbuff.s2; ltree[3] = tbuff.s3; 
      ltree[4] = tbuff.s4; ltree[5] = tbuff.s5; ltree[6] = tbuff.s6; ltree[7] = tbuff.s7; 
      ltree[8] = tbuff.s8; ltree[9] = tbuff.s9; ltree[10] = tbuff.sa;ltree[11] = tbuff.sb; 
      ltree[12] = tbuff.sc;ltree[13] = tbuff.sd;ltree[14] = tbuff.se;ltree[15] = tbuff.sf;   
      
      //load global tree into local mem
      //cell_ptr = traverse(0, ltree, 0, root, code, &found_loc_code, &global_count); 
      //cell_bounding_box(found_loc_code, 4, &cell_min, &cell_max);
      //data_ptr = data_index(0, ltree, cell_ptr, bit_lookup);
      
      p = convert_float4(code)/(float4)8.0; 
      cell_opt = traverse_opt(0, ltree, p, &cell_min_opt, &cell_max_opt);
      
      //results[0] = k; //TO fool the compiler's optimization into not caching local tree
    }
    //found loc code
    int4 res = convert_int4(found_loc_code);
    results[2*i]=res;
    //next results are index 
    res = (int4)(cell_opt, cell_opt, cell_ptr, cell_ptr);
    results[2*i+1]=res;

    //store bounding box results
    bbox_results[4*i]   = cell_min;
    bbox_results[4*i+1] = cell_max;
    bbox_results[4*i+2] = cell_min_opt;
    bbox_results[4*i+3] = cell_max_opt;
  }
}

__kernel
void
test_traverse_force(__global uchar16* tree, __global int4* results, __local uchar* ltree, __constant uchar * bit_lookup)
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
    //cell_ptr = traverse_force(0, ltree, start_ptr, start_code, target_code, &found_loc_code, &global_count);
  }
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;

  target_code = (short4)(0,2,0,0);
  //cell_ptr = traverse_force(0, ltree, start_ptr, start_code, target_code, &found_loc_code, &global_count);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;

  target_code = (short4)(0,0,2,0);
  //cell_ptr = traverse_force(0, ltree, start_ptr, start_code, target_code, &found_loc_code, &global_count);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;

  target_code = (short4)(2,2,2,0);
  //cell_ptr = traverse_force(0, ltree, start_ptr, start_code, target_code, &found_loc_code, &global_count);
  results[result_ptr++]=convert_int4(found_loc_code);
  results[result_ptr++]=(int4)cell_ptr;
}

#define TESTNUM 100000
__kernel
void test_float4(__global float4* out)
{
    out[0] = (float4) 0.0;
    out[1] = (float4) 0.0;
  
    //do 10000 multiplications
    for(int i=1; i<TESTNUM+1; i++) {
      float4 a = (float4) ((float) i, 
                           (float) i+1.5, 
                           (float) i/2.3, 
                           (float) 1);
      float4 b = (float4) ((float) i-2.3, 
                           (float) i+1.1, 
                           (float) i/3.3, 
                           (float) 1); 
      float4 c = a*b;
      out[0] += c;
    }
    //do 10000 multiplications
    for(int i=1; i<TESTNUM+1; i++) {
      float4 a = (float4) ((float) i, 
                           (float) i+1.5, 
                           (float) i/2.3, 
                           (float) 1);
      float4 b = (float4) ((float) i-2.3, 
                           (float) i+1.1, 
                           (float) i/3.3, 
                           (float) 1); 
      float4 c = a/b;
      out[1] += c;
    }
    
  
}
__kernel
void test_float3(__global float4* out)
{
    out[0] = (float4) 0.0;
    out[1] = (float4) 0.0;
  
    //do 10000 multiplications
    for(int i=1; i<TESTNUM+1; i++) {
      float ax = (float) i;
      float ay = (float) i+1.5;
      float az = (float) i/2.3;

      float bx = (float) i-2.3;
      float by = (float) i+1.1;
      float bz = (float) i/3.3;

      float cx = ax*bx;
      float cy = ay*by;
      float cz = az*bz;
      out[0] += (float4) (cx, cy, cz, 1.0);
    }
    
    //do 10000 multiplications
    for(int i=1; i<TESTNUM+1; i++) {
      float ax = (float) i;
      float ay = (float) i+1.5;
      float az = (float) i/2.3;

      float bx = (float) i-2.3;
      float by = (float) i+1.1;
      float bz = (float) i/3.3;

      float cx = ax/bx;
      float cy = ay/by;
      float cz = az/bz;
      out[1] += (float4) (cx, cy, cz, 1.0);
    }
}


__kernel
void test_scene_info(RenderSceneInfo info, __global float4* out)
{
  out[0] = info.scene_origin;
  out[1] = convert_float4(info.scene_dims);
  out[2] = (float4) info.block_len;
  
  out[3] = (float4) info.root_level;
  out[4] = (float4) info.num_buffer;
  out[5] = (float4) info.tree_buffer_length;
  out[6] = (float4) info.data_buffer_length;
  
  float3 a = (float3) {1.1, 2.2, 3.3};
  float3 b = (float3) {.5, .6, .7};
  float3 c = f3add(a,b);
  out[0] = (float4) (c.x, c.y, c.z, 1.0);

  
  
  int4 curr_block_index = (int4) (5, 5, 5, 1);
  int4 scenedims = (int4) (5, 4, 4, 1);
  int4 compare = (curr_block_index == scenedims);
  out[0] = convert_float4(compare);
  
}

