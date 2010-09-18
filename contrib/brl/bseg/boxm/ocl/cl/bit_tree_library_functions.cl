//:
// \file
//  Bit Tree library functions: These methods and structure mirror the
//  boct_bit_tree class in boct.  The bit tree is an octree with max
//  height of 4, who's structure is stored as an implicit bit data
//  structure, where each bit signifies that that node has been split:
//  i.e. 1 11110000 -> 3 level tree (root,1,2) where the 4 left most
//  nodes in level 1 have all split.  Number of nodes can be calculated
//  by sum(1-bits) + 1


//--------------------------------------------------------------------
// returns size (number of data cells) for given tree
//--------------------------------------------------------------------
int num_cells(__local uchar* tree)
{
  //count bits for each byte
  int count = 0 ;
  for (int i=0; i<10; i++) {
    uchar n = tree[i];
    while (n)  {
      count++ ;
      n &= (n - 1) ;
    }
  }
  return 8*count+1;
}

//--------------------------------------------------------------------
// loc code to absolute index //UNTESTED
//--------------------------------------------------------------------
int loc_code_to_index(short4 loc_code, int root_level)
{
  int level = loc_code.w;
  int depth = root_level - level;

  //index of first node at depth
  int level_index = (int) (1>>(3*depth)-1) / 7;

  //need to map the location code to a number between 0 and 2^(num_levels-1)
  //note: i believe X needs to be the LSB, followed by Y and Z (Z,Y,X)
  int ri = root_level-1;
  ushort packed = 0;
  for (int i=depth; i>0; i--, ri--)  {
    ushort mask = 1<<ri;
    //get bit at mask
    ushort4 mxyz = (mask & loc_code)>>ri;

    packed += (mxyz.z << (3*i-1))
            + (mxyz.y << (3*i-2))
            + (mxyz.x << (3*i-3));
  }
  return level_index + packed;
}

//---------------------------------------------------------------------
// Tree Bit manipulation helper functions
//---------------------------------------------------------------------

uchar tree_bit_at(int rIndex, __local uchar* tree, int index)
{
  //make sure it's in bounds - all higher cells are leaves and thus 0
  if (index > 72)
    return 0;

  //root is special case
  if (index == 0)
    return tree[rIndex+0];

  //second generation is sort of a special case (speeds up code)
  if (index < 9)
    return (1<<(index-1) & tree[rIndex+1])>>(index-1); // ? 1 : 0;

  //third or 4th generation treated in same way,
  int i  = ((index-9)/8 + 2); //byte index i
  int bi = (index-9)%8;
  return (1<<bi & tree[rIndex+i])>>bi; // ? 1 : 0;
}

/*
uchar tree_bit_at(int rIndex, __local uchar* tree, int index)
{
  //make sure it's in bounds - all higher cells are leaves and thus 0
  if (index > 72)
    return 0;
 
  //root is special case
  if (index == 0)
    return tree[rIndex+0];
 
  //second generation is sort of a special case (speeds up code)
  if (index < 9)
    return (1<<(index-1) & tree[rIndex+1])>>(index-1); // ? 1 : 0;
 
  int temp=(index-9);
  temp=temp>>3;
  int i=temp+2;
  int bi =temp- temp<<3;
  //third or 4th generation treated in same way,
  //int i  = ((index-9)/8 + 2); //byte index i
  //int bi = (index-9)%8;
  return (1<<bi & tree[rIndex+i])>>bi; // ? 1 : 0;
}
*/


void set_tree_bit_at(__local uchar* tree, int index, bool val)
{
  if (index > 72)
    return;

  //zero is a special case,
  if (index == 0)
    tree[0] = (val) ? 1 : 0;

  int byte_index = (int) (index-1.0)/8.0+1;
  int child_offset = (index-1)%8;
  unsigned char mask = 1<<child_offset;
  unsigned char byte = tree[byte_index];
  tree[byte_index] = (val)? (byte | mask) : (byte & (mask ^ 0xFF));
}

//--------------------------------------------------------------------
// returns the short offset of the data  //TEST THIS
// unpacks offset (ushort) from tree[10] and tree[11],
// also counts the bits until parent of index, adds it to the offset
//--------------------------------------------------------------------
ushort data_index(int rIndex, __local uchar* tree, int bit_index, __constant uchar* bit_lookup)
{
  ////Unpack data offset (offset to root data)
  //tree[10] and [11] should form the short that refers to data offset
  ushort root_offset = as_ushort((uchar2) (tree[rIndex+11], tree[rIndex+10]));
  
  //root and first gen are special case, return just the root offset + bit_index
  if(bit_index < 9)
    return root_offset+bit_index;
 
  //otherwise get parent index, parent byte index and relative bit index
  int pi      = (bit_index-1)>>3;      // automatically rounding downwards
  int a       = (pi-1)>>3; 
  int byte_i  = a + 1;          //byte index for parent
  int bit_i   = (pi-1)-(a<<3);              //bit index for pi in byte_i
  
  //count bits for each byte before bit_i
  int count = 0;
  for(int i=0; i<byte_i; i++) {
    uchar n = tree[rIndex+i];
    count += bit_lookup[n];
  }
  
  //count bits before bit_i in parent
  uchar n = tree[rIndex+byte_i] << (8-bit_i);
  count += bit_lookup[n];
  
  //relative index = num_bits*8 + 1;
  count = 8*count+1 + (bit_index-1)%8;
  return count+ root_offset;
}

//optimized to use minimal registers
int data_index_opt(int rIndex, __local uchar* tree, ushort bit_index, __constant uchar* bit_lookup)
{
  ////Unpack data offset (offset to root data)
  //tree[10] and [11] should form the short that refers to data offset
  //root and first gen are special case, return just the root offset + bit_index 
  int count_offset=(int)as_ushort((uchar2) (tree[rIndex+11], tree[rIndex+10]));
  if(bit_index < 9)
    return count_offset + bit_index;
 
  //otherwise get parent index, parent byte index and relative bit index
  uchar oneuplevel=(bit_index-1)>>3;
  uchar byte_index= ((oneuplevel-1)>>3) +1;

  uchar sub_bit_index=8-((oneuplevel-1)&(8-1));
  int count=0;
  for(int i=0;i<byte_index;i++)
        count += bit_lookup[tree[rIndex+i]];

  uchar temp=tree[rIndex+byte_index]<<sub_bit_index;
  count=count+bit_lookup[temp];
  uchar finestleveloffset=(bit_index-1)&(8-1);
  count = 8*count+1 +finestleveloffset;

  return count + count_offset;
}

//optimized to use cumulative sum counts
int data_index_opt2(__local uchar* tree, ushort bit_index, __constant uchar* bit_lookup, __local uchar* cumsum, int *cumIndex)
{
  //root and first gen are special case, return just the root offset + bit_index 
  int count_offset=(int)as_ushort((uchar2) (tree[11], tree[10]));
  if(bit_index < 9)
    return count_offset + bit_index;
    
  //otherwise get parent index, parent byte index and relative bit index
  uchar oneuplevel        = (bit_index-1)>>3;           //Bit_index of parent bit
  uchar byte_index        = ((oneuplevel-1)>>3) +1;     //byte_index of parent bit
  uchar sub_bit_index     = 8-((oneuplevel-1)&(8-1));   //[0-7] bit index of parent bit

  for(int i=(*cumIndex); i<byte_index; i++) {
    cumsum[i] = cumsum[i-1] + bit_lookup[tree[i]];
    (*cumIndex) = i;
  }

  uchar bits_before_parent = tree[byte_index]<<sub_bit_index; //number of bits before parent bit [0-6] in parent byte
  bits_before_parent       = bit_lookup[bits_before_parent];
  uchar finestleveloffset = (bit_index-1)&(8-1);              //[0-7] bit index of cell being looked up (@bit_index)
  int count = (cumsum[byte_index-1] + bits_before_parent)*8 + 1 + finestleveloffset; 
  return count + count_offset;
}

//-----------------------------------------------------------------
// New traverse: uses only target point and returns cell index
// this can also easily return the level
// perhaps can also return data index
//-----------------------------------------------------------------
int traverse_opt(int rIndex, __local uchar* tree, float4 p, float4 *cell_min, float4* cell_max )
{
  // vars to replace "tree_bit_at"
  uchar curr_bit        = tree[rIndex];   //store root bit
  int curr_child_offset = 0;              //root offset is 0
  int curr_depth        = 0;              //root depth is 0
  
  //bit index to be returned
  int bit_index = 0;
  uchar4 code = (uchar4) 1;
  
  //clamp point
  float4 point = clamp(p, 0.0001, 0.9999);

  // while the curr node has children
  while(curr_bit && curr_depth < 3) {
    
    //determine child offset and bit index for given point
    point += point;                                             //point = point*2
    code = (convert_uchar4_rtn(point) & (uchar4) 1);            //code.xyz = lsb of floor(point.xyz)
    code <<= ((uchar4) (0,1,2,0));                     
    int c_offset = code.x + code.y + code.z;                    //c_index = binary(zyx)
    bit_index = (8*bit_index + 1) + c_offset;                   //i = 8i + 1 + c_index
    
    //update value of curr_bit and level
    int curr_byte = (curr_depth + 1) + curr_child_offset; 
    curr_bit  = (1<<c_offset) & tree[rIndex+curr_byte];
    curr_child_offset = c_offset;
    curr_depth++;
  }
  
  // calculate cell bounding box 
  float4 cell_size = (float4) (1.0 / (float) (1<<curr_depth));      //likely to be correct
  (*cell_min) = floor(point) * cell_size;    //cell_min is bottom left point
  (*cell_max) = (*cell_min) + cell_size;
  (*cell_min).w = 0.0f;   (*cell_max).w = 0.0f;

  return bit_index;
}

//-----------------------------------------------------------------
// New traverse: uses only target point and returns cell index
// this can also easily return the level
// perhaps can also return data index
//-----------------------------------------------------------------
int traverse_opt_len(int rIndex, __local uchar* tree, float4 point, float4 *cell_min, float* cell_len )
{
  // vars to replace "tree_bit_at"
  uchar curr_bit        = tree[rIndex];   //store root bit
  int curr_child_offset = 0;              //root offset is 0
  int curr_depth        = 0;              //root depth is 0
  
  //bit index to be returned
  int bit_index = 0;
  uchar4 code = (uchar4) 1;
  
  //clamp point
  point = clamp(point, 0.0001, 0.9999);

  // while the curr node has children
  while(curr_bit && curr_depth < 3) {
    
    //determine child offset and bit index for given point
    point += point;                                             //point = point*2
    code = (convert_uchar4_rtn(point) & (uchar4) 1);            //code.xyz = lsb of floor(point.xyz)
    code <<= ((uchar4) (0,1,2,0));                     
    int c_offset = code.x + code.y + code.z;                    //c_index = binary(zyx)
    bit_index = (8*bit_index + 1) + c_offset;                   //i = 8i + 1 + c_index
    
    //update value of curr_bit and level
    int curr_byte = (curr_depth + 1) + curr_child_offset; 
    curr_bit  = (1<<c_offset) & tree[rIndex+curr_byte];
    curr_child_offset = c_offset;
    curr_depth++;
  }
  
  // calculate cell bounding box 
  (*cell_len) = 1.0 / (float) (1<<curr_depth);
  (*cell_min) = floor(point) * (*cell_len);
  (*cell_min).w = 0.0f;

  return bit_index;
}

//takes three floats instaed of float4s
//TODO optimize point here - makei t a float 3 instead of a float4
ushort traverse_three(__local uchar* tree, 
                      float pointx, float pointy, float pointz, 
                      float *cell_minx, float *cell_miny, float *cell_minz, float *cell_len )
{
  // vars to replace "tree_bit_at"
  //force 1 register: curr = (bit, child_offset, depth, c_offset)
  int curr_bit = convert_int(tree[0]);
  int child_offset = 0;
  int depth = 0;  
  
  //bit index to be returned
  ushort bit_index = 0;
  
  //clamp point
  pointx = clamp(pointx, 0.0001, 0.9999);
  pointy = clamp(pointy, 0.0001, 0.9999);
  pointz = clamp(pointz, 0.0001, 0.9999);

  // while the curr node has children
  while(curr_bit && depth < 3) {
    //determine child offset and bit index for given point
    pointx += pointx;                                             //point = point*2
    pointy += pointy;
    pointz += pointz;                                           
    int4 code =  (int4) (convert_int_rtn(pointx) & 1, 
                         convert_int_rtn(pointy) & 1,
                         convert_int_rtn(pointz) & 1, 0);         //code.xyz = lsb of floor(point.xyz)
    int c_index = code.x + (code.y<<1) + (code.z<<2);             //c_index = binary(zyx)    
    bit_index = (8*bit_index + 1) + c_index;                      //i = 8i + 1 + c_index
    
    //update value of curr_bit and level
    curr_bit = (1<<c_index) & tree[(depth+1 + child_offset)];      //int curr_byte = (curr.z + 1) + curr.y; 
    child_offset = c_index;
    depth++;
  }
  
  // calculate cell bounding box 
  (*cell_len) = 1.0 / (float) (1<<depth);
  (*cell_minx) = floor(pointx) * (*cell_len);
  (*cell_miny) = floor(pointy) * (*cell_len);
  (*cell_minz) = floor(pointz) * (*cell_len);
  return bit_index;
}



#if 0
//takes three floats instaed of float4s
//TODO optimize point here - makei t a float 3 instead of a float4
ushort traverse_three(int rIndex, __local uchar* tree, float4 point, 
                   float *cell_minx, float *cell_miny, float *cell_minz, float *cell_len )
{
  // vars to replace "tree_bit_at"
  //force 1 register
  // curr = (bit, child_offset, depth, c_offset)
  uchar4 curr = (uchar4) (tree[rIndex], 0, 0, 0);
  
  //bit index to be returned
  ushort bit_index = 0;
  
  //clamp point
  point = clamp(point, 0.0001, 0.9999);

  // while the curr node has children
  while(curr.x && curr.z < 3) {
    //determine child offset and bit index for given point
    point += point;                                             //point = point*2
    uchar4 code = (convert_uchar4_rtn(point) & (uchar4) 1);     //code.xyz = lsb of floor(point.xyz)
    curr.w = code.x + (code.y<<1) + (code.z<<2);                //c_index = binary(zyx)    
    bit_index = (8*bit_index + 1) + curr.w;                     //i = 8i + 1 + c_index
    
    //update value of curr_bit and level
    //int curr_byte = (curr.z + 1) + curr.y; 
    curr.x  = (1<<curr.w) & tree[rIndex+ (curr.z+1+curr.y)];
    curr.y = curr.w;
    curr.z++;
  }
  
  // calculate cell bounding box 
  (*cell_len) = 1.0 / (float) (1<<curr.z);
  (*cell_minx) = floor(point.x) * (*cell_len);
  (*cell_miny) = floor(point.y) * (*cell_len);
  (*cell_minz) = floor(point.z) * (*cell_len);
  return bit_index;
}

//-----------------------------------------------------------------
// New traverse: uses only target point and returns cell index
// this can also easily return the level
// perhaps can also return data index
//-----------------------------------------------------------------
int traverse_opt_len_float3(int rIndex, __local uchar* tree, float4 p, float3 *cell_min, float* cell_len )
{
  // vars to replace "tree_bit_at"
  uchar curr_bit        = tree[rIndex];   //store root bit
  int curr_child_offset = 0;              //root offset is 0
  int curr_depth        = 0;              //root depth is 0
  
  //bit index to be returned
  int bit_index = 0;
  uchar4 code = (uchar4) 1;
  
  //clamp point
  float4 point = clamp(p, 0.0001, 0.9999);

  // while the curr node has children
  while(curr_bit && curr_depth < 3) {
    
    //determine child offset and bit index for given point
    point += point;                                             //point = point*2
    code = (convert_uchar4_rtn(point) & (uchar4) 1);            //code.xyz = lsb of floor(point.xyz)
    code <<= ((uchar4) (0,1,2,0));                     
    int c_offset = code.x + code.y + code.z;                    //c_index = binary(zyx)
    bit_index = (8*bit_index + 1) + c_offset;                   //i = 8i + 1 + c_index
    
    //update value of curr_bit and level
    int curr_byte = (curr_depth + 1) + curr_child_offset; 
    curr_bit  = (1<<c_offset) & tree[rIndex+curr_byte];
    curr_child_offset = c_offset;
    curr_depth++;
  }
  
  // calculate cell bounding box 
  //float4 cell_size = (float4) (1.0 / (float) (1<<curr_depth));      //likely to be correct
  //(*cell_min) = floor(point) * cell_size;    //cell_min is bottom left point
  //(*cell_max) = (*cell_min) + cell_size;
  //(*cell_min).w = 0.0f;   (*cell_max).w = 0.0f;
  (*cell_len) = 1.0 / (float) (1<<curr_depth);
  point = floor(point) * (*cell_len);
  (*cell_min) = (float3) {point.x, point.y, point.z};

  return bit_index;
}
#endif

#if 0
//-----------------------------------------------------------------
// Traverse from the specified root_cell to the cell specified by loc_code.
// Return the array pointer to the resulting cell. If a leaf node is
// encoutered during the traversal down the tree before the specified
// code is reached, the leaf node index is returned.
// takes in a uchar* bit tree.
// cell_loc_code is the start cell = usually the root.
// cell_loc_code.w = start level = root_level (NUM_LEVELS-1)
// found_cell_ptr = bit index of octree (will usually contain a 0)
//-----------------------------------------------------------------

int traverse(int rIndex, __local uchar* tree, int cell_index, short4 cell_loc_code,
             short4 target_loc_code, short4* found_loc_code, int * global_count)
{
  int target_level = target_loc_code.w;
  if (target_level < 0)
    return -1;

  //initialize current cell (curr_cell = has_children?)
  uchar curr_cell    = tree_bit_at(rIndex, tree, cell_index);
  int curr_level     = cell_loc_code.w;
  (*found_loc_code)  = cell_loc_code;

  while (target_level<curr_level && curr_cell)
  {
    //update found loc code (loc code belonging to correct child)
    uchar c_index     = child_index(target_loc_code, curr_level);
    (*found_loc_code) = child_loc_code(c_index, curr_level-1, *found_loc_code);

    //update cell_index = first_child_index + child_offset
    cell_index = (cell_index*8+1) + (int) c_index; //8i+1 + c_index
    curr_cell  = tree_bit_at(rIndex, tree, cell_index);

    //decrement curr_level
    --curr_level;
  }
  return cell_index;
}

//-----------------------------------------------------------------
// Traverse from the current cell to find the cell whose code is
// closest to the specified target_loc_code and lies in the sub-tree of
// the root cell at cell_ptr with cell_loc_code. The cell array pointer
// of the found cell is returned. Note that the specified target code may
// not actually lie in the sub-tree. This method is necessary since small
// errors may lead to a computed code for a point that lies outside the
// cell of interest
//-----------------------------------------------------------------
int traverse_force(int rIndex, __local uchar* tree, int cell_ptr, short4 cell_loc_code,
                   short4 target_loc_code, short4* found_loc_code, int * global_count)
{
  //init - check for legal call
  int found_cell_ptr = cell_ptr;
  (*found_loc_code) = cell_loc_code;
  int ret = (int)-1;
  int level = target_loc_code.w;
  if ( level < 0)
    return ret;

  //begin traversal
  int curr_level   = cell_loc_code.w;
  uchar curr_cell  = tree_bit_at(rIndex, tree, cell_ptr); // the root of the tree to search
  short4 curr_code = cell_loc_code;
  curr_code.w = curr_level;
  (*global_count)++;

  //if the curr cell has children go to the correct one
  while (level<curr_level && curr_cell)
  {
    //update found loc code (loc code belonging to correct child)
    short4 child_bit = (short4)(1);
    child_bit = child_bit << (short4)(curr_level-1);
    short4 code_diff = target_loc_code-curr_code;
    // TODO: find a way to compute the following as a vector op
    uchar c_index = 0;
    c_index = (uchar) (code_diff.x >= child_bit.x) +
              (uchar) ((code_diff.y >= child_bit.y)<<1) +
              (uchar) ((code_diff.z >= child_bit.z)<<2);
   // if (code_diff.x >= child_bit.x)
   //   c_index += 1;
   // if (code_diff.y >= child_bit.y)
   //   c_index += 2;
   // if (code_diff.z >= child_bit.z)
   //   c_index += 4;
    curr_code = child_loc_code(c_index, curr_level-1, curr_code);

    //update cell_index = first_child_index + child_offset
    cell_ptr = (cell_ptr*8 + 1) + (int) c_index;
    curr_cell = tree_bit_at(rIndex, tree, cell_ptr);

    found_cell_ptr = cell_ptr;
    (*found_loc_code) = curr_code;
    --curr_level;
    (*global_count)++;
  }
  return found_cell_ptr;
}

//--------------------------------------------------------------------
// Find the common ancestor of a cell given a binary difference
//
int common_ancestor(__global int4* cells, int cell_ptr, short4 cell_loc_code,
                    short4 target_loc_code, short4* ancestor_loc_code, int * global_count)
{
  short4 bin_diff = cell_loc_code ^ target_loc_code;
  short curr_level = (short)cell_loc_code.w;
  int curr_cell_ptr = cell_ptr;
  (*ancestor_loc_code) = cell_loc_code;
  int4 curr_cell = cells[curr_cell_ptr];
  (*global_count)++;
  short4 mask = (short4)(1 << (curr_level));
  short4 shift_one =(short4)1; // shift the mask by 1 as a vector
  short4 arg = bin_diff & mask; // masking the bits of the difference (xor)
  while (arg.x>0||arg.y>0||arg.z>0) // might be done as vector op
  {
    curr_cell_ptr = curr_cell.x;
    curr_cell = cells[curr_cell_ptr];
    (*global_count)++;
    // clear the code bit at each level while ascending to common ancestor
    short4 clear_bits = ~(short4)(mask);
    curr_level++;
    (*ancestor_loc_code) = (*ancestor_loc_code) & clear_bits;
    (*ancestor_loc_code).w = curr_level;
    mask = mask << shift_one;
    arg = bin_diff & mask;
  }
  return curr_cell_ptr;
}

int common_ancestor_woffset(__global int4* cells, int cell_ptr, short4 cell_loc_code,
                            short4 target_loc_code, short4* ancestor_loc_code, int * global_count, int tree_offset)
{
  short4 bin_diff = cell_loc_code ^ target_loc_code;
  short curr_level = (short)cell_loc_code.w;
  int curr_cell_ptr = cell_ptr;
  (*ancestor_loc_code) = cell_loc_code;
  int4 curr_cell = cells[curr_cell_ptr];
  (*global_count)++;
  short4 mask = (short4)(1 << (curr_level));
  short4 shift_one =(short4)1; // shift the mask by 1 as a vector
  short4 arg = bin_diff & mask; // masking the bits of the difference (xor)
  while (arg.x>0||arg.y>0||arg.z>0) // might be done as vector op
  {
    curr_cell_ptr = curr_cell.x+tree_offset;
    curr_cell = cells[curr_cell_ptr];
    (*global_count)++;
    // clear the code bit at each level while ascending to common ancestor
    short4 clear_bits = ~(short4)(mask);
    curr_level++;
    (*ancestor_loc_code) = (*ancestor_loc_code) & clear_bits;
    (*ancestor_loc_code).w = curr_level;
    mask = mask << shift_one;
    arg = bin_diff & mask;
  }
  return curr_cell_ptr;
}


//-------------------------------------------------------------------
// Given the cell loc_code and the exit face, find the neighboring cell.
//-------------------------------------------------------------------
int neighbor(__global int4* cells,int cell_ptr,  short4 cell_loc_code,
             short4 exit_face, short n_levels, short4* neighbor_code,int * global_count)
{
  short cell_level = cell_loc_code.w;
  short cell_size = 1<<cell_level;
  short4 error = (short4)-1;
  int neighbor_ptr = -1;
  // if the neighbor is on the min face
  if (exit_face.w==0)
  {
    short4 zero = (short4)0;
    (*neighbor_code) = cell_loc_code - exit_face;
    (*neighbor_code).w = 0; // smallest cell level possible
    short4 test =(short4)((*neighbor_code) < zero);
    if (any(test)) {
      (*neighbor_code) = error;
      return neighbor_ptr;
    }
  }
  else {
    short4 largest = (short4)(1<<(n_levels-1));
    short4 csize = (short4)cell_size;
    csize.w = 0;
    (*neighbor_code) = cell_loc_code + (csize*exit_face);
    (*neighbor_code).w = 0;
    short4 test =(short4)((*neighbor_code) >= largest);
    if (any(test)) {
      (*neighbor_code) = error;
      return neighbor_ptr;
    }
  }
  short4 ancestor_loc_code = error;
  int ancestor_ptr =  common_ancestor(cells, cell_ptr, cell_loc_code,
                                      (*neighbor_code),
                                      &ancestor_loc_code,global_count);
  if (ancestor_ptr<0) {
    (*neighbor_code) = error;
    return neighbor_ptr;
  }
  neighbor_ptr =
    traverse_to_level(cells, ancestor_ptr, ancestor_loc_code,
                      (*neighbor_code), cell_level, neighbor_code,global_count);
  return neighbor_ptr;
}

int neighbor_woffset(__global int4* cells,int cell_ptr,  short4 cell_loc_code,
                     short4 exit_face, short n_levels, short4* neighbor_code,int * global_count, int tree_offset)
{
  short cell_level = cell_loc_code.w;
  short cell_size = 1<<cell_level;
  short4 error = (short4)-1;
  int neighbor_ptr = -1;
  // if the neighbor is on the min face
  if (exit_face.w==0)
  {
    short4 zero = (short4)0;
    (*neighbor_code) = cell_loc_code - exit_face;
    (*neighbor_code).w = 0; // smallest cell level possible
    short4 test =(short4)((*neighbor_code) < zero);
    if (any(test)) {
      (*neighbor_code) = error;
      return neighbor_ptr;
    }
  }
  else {
    short4 largest = (short4)(1<<(n_levels-1));
    short4 csize = (short4)cell_size;
    csize.w = 0;
    (*neighbor_code) = cell_loc_code + (csize*exit_face);
    (*neighbor_code).w = 0;
    short4 test =(short4)((*neighbor_code) >= largest);
    if (any(test)) {
      (*neighbor_code) = error;
      return neighbor_ptr;
    }
  }
  short4 ancestor_loc_code = error;
  int ancestor_ptr =  common_ancestor_woffset(cells, cell_ptr, cell_loc_code,
                                              (*neighbor_code),
                                              &ancestor_loc_code,global_count,tree_offset);
  if (ancestor_ptr<0) {
    (*neighbor_code) = error;
    return neighbor_ptr;
  }
  neighbor_ptr =
    traverse_to_level_woffset(cells, ancestor_ptr, ancestor_loc_code,
                              (*neighbor_code), cell_level, neighbor_code,global_count,tree_offset);
  return neighbor_ptr;
}

#endif


// end of library kernels
