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
// returns depth at particular index i for an octree
// this can be replaced by an expression
//--------------------------------------------------------------------
int get_depth(int i)
{
  //root
  if (i==0)
    return 0;

  //1st gen
  if (i < 9)
    return 1;

  //2nd gen
  if (i < 73)
    return 2;

  //3rd gen...
  if (i < 585)
    return 3;

  if (i < 4681)
    return 4;
}

//--------------------------------------------------------------------
// Returns whether or not a cell at 'i' has children that are all leaves
//--------------------------------------------------------------------
bool children_are_leaves(__local uchar* tree, int i)
{
  if (i==0)
    return tree[1] == 0;
  else if (i<9)
    return tree[i+1] == 0;
  else
    return true;
#if 0
  if (i > 72) {
    int ci = (i<<3) + 1;                   // bit index of first child
    int bi = ((ci-1)>>3) +1;               // byte_index of parent bit
    return tree[bi] == 0;                  // if all bits at tree[bi] are 0, then they're all leaves...
  }
#endif
}

//---------------------------------------------------------------------
// Tree Bit manipulation helper functions
//---------------------------------------------------------------------
uchar tree_bit_at(__local uchar* tree, int index)
{
  //make sure it's in bounds - all higher cells are leaves and thus 0
  if (index > 72 || index < 0)
    return 0;

  //root is special case
  if (index == 0)
    return tree[0];

  //second generation is sort of a special case (speeds up code)
  if (index < 9)
    return (1<<(index-1) & tree[1])>>(index-1); // ? 1 : 0;

  //third or 4th generation treated in same way,
  int a = (index-9)>>3;
  int i = a+2;
  int bi = (index-9) - (a<<3);
  // int i  = ((index-9)/8 + 2);          //byte index i
  //int bi = (index-9)%8;
  return (1<<bi & tree[i])>>bi; // ? 1 : 0;
}

void set_tree_bit_at(__local uchar* tree, int index, bool val)
{
  if (index > 72 || index < 0)
    return;

  //zero is a special case,
  if (index == 0) {
    tree[0] = (val) ? 1 : 0;
    return;
  }

  int byte_index = convert_int((index-1.0)/8.0+1);
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
int data_index(int rIndex, __local uchar* tree, ushort bit_index, __constant uchar* bit_lookup)
{
  ////Unpack data offset (offset to root data)
  //tree[10] and [11] should form the short that refers to data offset
  //root and first gen are special case, return just the root offset + bit_index
  int count_offset=(int)as_ushort((uchar2) (tree[rIndex+11], tree[rIndex+10]));
  if (bit_index < 9)
    return (count_offset+bit_index) - (((count_offset+bit_index)>>16)<<16);

  //otherwise get parent index, parent byte index and relative bit index
  uchar oneuplevel=(bit_index-1)>>3;
  uchar byte_index= ((oneuplevel-1)>>3) +1;

  uchar sub_bit_index=8-((oneuplevel-1)&(8-1));
  int count=0;
  for (int i=0;i<byte_index;i++)
    count += bit_lookup[tree[rIndex+i]];

  uchar temp=tree[rIndex+byte_index]<<sub_bit_index;
  count=count+bit_lookup[temp];
  uchar finestleveloffset=(bit_index-1)&(8-1);
  count = 8*count+1 +finestleveloffset;

  return (count_offset+count) - (((count_offset+count)>>16)<<16);
}

//optimized to use cumulative sum counts
int data_index_cached(__local uchar* tree, ushort bit_index, __constant uchar* bit_lookup, __local uchar* cumsum, int *cumIndex, int *cycleCount)
{
  //root and first gen are special case, return just the root offset + bit_index
  int count = (int)as_ushort((uchar2) (tree[11], tree[10]));
  *cycleCount+=8;

  if (bit_index < 9)
    return count + bit_index;

  //otherwise get parent index, parent byte index and relative bit index
  uchar oneuplevel        = (bit_index-1)>>3;           //Bit_index of parent bit
  uchar byte_index        = ((oneuplevel-1)>>3) +1;     //byte_index of parent bit
  uchar sub_bit_index     = 8-((oneuplevel-1)&(8-1));   //[0-7] bit index of parent bit
  *cycleCount += 12;

  for (; (*cumIndex) < byte_index; ++(*cumIndex))  {
    cumsum[(*cumIndex)] = cumsum[(*cumIndex)-1] + bit_lookup[tree[(*cumIndex)]];
    *cycleCount += 14;
  }

  uchar bits_before_parent = tree[byte_index]<<sub_bit_index; //number of bits before parent bit [0-6] in parent byte
  bits_before_parent       = bit_lookup[bits_before_parent];
  uchar finestleveloffset = (bit_index-1)&(8-1);              //[0-7] bit index of cell being looked up (@bit_index)
  count += (cumsum[byte_index-1] + bits_before_parent)*8 + 1 + finestleveloffset;
  *cycleCount += 17;
  return count;
}

//takes three floats instaed of float4s
//TODO optimize point here - makei t a float 3 instead of a float4
ushort traverse_three(__local uchar* tree,
                      float pointx, float pointy, float pointz,
                      float *cell_minx, float *cell_miny, float *cell_minz, float *cell_len, int *cycleCount)
{
  // vars to replace "tree_bit_at"
  //force 1 register: curr = (bit, child_offset, depth, c_offset)
  int curr_bit = convert_int(tree[0]);
  int child_offset = 0;
  int depth = 0;

  //bit index to be returned
  ushort bit_index = 0;

  //clamp point
  pointx = clamp(pointx, 0.0001f, 0.9999f);
  pointy = clamp(pointy, 0.0001f, 0.9999f);
  pointz = clamp(pointz, 0.0001f, 0.9999f);

  *cycleCount += 15;

  // while the curr node has children
  while (curr_bit && depth < 3) {
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

    *cycleCount += 32;
  }

  // calculate cell bounding box
  (*cell_len) = 1.0 / (float) (1<<depth);
  (*cell_minx) = floor(pointx) * (*cell_len);
  (*cell_miny) = floor(pointy) * (*cell_len);
  (*cell_minz) = floor(pointz) * (*cell_len);
  *cycleCount += 24;
  return bit_index;
}

// end of library kernels
