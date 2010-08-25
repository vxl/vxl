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

  //need to map the location code to a number between 0 and 2^(num_levels-1)
  //note: i believe X needs to be the LSB, followed by Y and Z (Z,Y,X)
  ushort packed = 0;
  ushort mask = 1;
  for (int i=0; i<depth-1; i++) {
    ushort mz = (mask & loc_code.z); //>>i;
    ushort my = (mask & loc_code.y); //>>i;
    ushort mx = (mask & loc_code.x); //>>i;

    //vcl_cout<<"Packed = "<<packed<< "   mask: "<<mask<<'\n'
    //        <<mz<<' '<<my<<' '<<mx<<vcl_endl;
    //note that mz is shifted to the right i times, and then left
    //3*i times... can just shift to the left 2*i times..
    packed += (mx <<  2*i)
            + (my << (2*i + 1))
            + (mz << (2*i + 2));
    mask <<= 1;
  }
  return packed;
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
  uchar2 chars = (uchar2) (tree[rIndex+11], tree[rIndex+10]);
  ushort root_offset = as_ushort(chars);
  
  //root and first gen are special case, return just the root offset + bit_index
  if(bit_index < 9)
    return root_offset+bit_index;
 
  //otherwise get parent index, parent byte index and relative bit index
  int pi      = (bit_index-1)>>3;     // automatically rounding downwards
  int byte_i  = (pi-1)/8 + 1;        //byte index for parent
  int bit_i   = (pi-1)%8;              //bit index for pi in byte_i
  
  //count bits for each byte before bit_i
  int count = 0;
  for(int i=0; i<byte_i; i++) {
    uchar n = tree[rIndex+i];
    count += bit_lookup[n];
  }
  
  //count bits before bit_i in parent
  uchar mask = 1;
  for(int i=0; i<bit_i; i++) {
    count += (mask & tree[rIndex+byte_i])?1:0;
    mask = mask<<1;
  }

  
  //relative index = num_bits*8 + 1;
  count = 8*count+1 + (bit_index-1)%8;
  return count+ root_offset;


/*//CODE BELOW WORKS USING variable pi from above
  //check to make sure that the parent of this index is one, otherwise return failure;
  //if (tree_bit_at(tree, pi) != 1) 
  //  return -100;

  //add up bits that occur before the parent index
  int di = 1;
  for (int i=0; i<pi; i++)
    di += 8*tree_bit_at(rIndex, tree,i);

  //offset for child...
  di += (bit_index-1)%8;

  return di+root_offset;  
*/
}


//--------------------------------------------------------------------------
// maps an offset and a depth to a location code
//-------------------------------------------------------------------------

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

#if 0

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
