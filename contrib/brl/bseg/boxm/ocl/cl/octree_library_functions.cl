
//---------------------------------------------------------------------
// tree cells are arranged into int2's.  first int is either parent_ptr
// or negative block index, second is [child_ptr | data_ptr] (as shorts)
//Note that data_ptrs are USHORTS, while child_ptrs are merely shorts
//---------------------------------------------------------------------
int get_child_ptr(int2 cell)
{
  short2 child_data = as_short2(cell.y);
  return (int) child_data.y;
}

void set_child_ptr(int2 *cell, int child_ptr)
{
  short2 child_data = as_short2((*cell).y);
  child_data.y = (short) child_ptr;
  (*cell).y = as_int(child_data);
}

int get_data_ptr(int2 cell)
{
  ushort2 child_data = as_ushort2(cell.y);
  return (int) child_data.x;
}

void set_data_ptr(int2 *cell, int data_ptr)
{
  ushort2 child_data = as_ushort2((*cell).y);
  child_data.x = (short) data_ptr;
  (*cell).y = as_int(child_data);
}

int2 pack_cell(int4 cell)
{
  int2 packed;
  //first int gets negative index or parent pointer
  packed.x = (cell.x < 0)? -1*cell.w : cell.x;

  //pack child and data pointer as two shorts
  short child = (short) (cell.y);
  ushort data = (ushort)(cell.z);
  int packed_child_data = (child << 16) | data;
  packed.y = packed_child_data;

  return packed;
}


#ifndef USEIMAGE
//-----------------------------------------------------------------
// Traverse from the specified root_cell to the cell specified by loc_code.
// Return the array pointer to the resulting cell. If a leaf node is
// encoutered during the traversal down the tree before the specified
// code is reached, the leaf node index is returned.
//-----------------------------------------------------------------
int traverse(__global int4* cells, int cell_ptr, short4 cell_loc_code,
             short4 target_loc_code, short4* found_loc_code, int * global_count)
{
  int found_cell_ptr = cell_ptr;
  int ret = -1;
  int level = target_loc_code.w;
  if ( level < 0)
    return ret;
  int4 curr_cell = cells[cell_ptr];
  (*global_count)++;
  int curr_level = cell_loc_code.w;
  *found_loc_code = cell_loc_code;
  while (level<curr_level && curr_cell.y>0)
  {
    int c_ptr = curr_cell.y;
    uchar c_index = child_index(target_loc_code, curr_level);
    (*found_loc_code) =
      child_loc_code(c_index, curr_level-1, *found_loc_code);
    c_ptr += c_index;
    curr_cell = cells[c_ptr];
    (*global_count)++;
    found_cell_ptr = c_ptr;
    --curr_level;
  }
  return found_cell_ptr;
}

int traverse_woffset(__global int4* cells, int cell_ptr, short4 cell_loc_code,
                     short4 target_loc_code, short4* found_loc_code, int * global_count,int tree_offset)
{
  int found_cell_ptr = cell_ptr;
  int ret = -1;
  int level = target_loc_code.w;
  if ( level < 0)
    return ret;
  int4 curr_cell = cells[cell_ptr];
  (*global_count)++;
  int curr_level = cell_loc_code.w;
  *found_loc_code = cell_loc_code;
  while (level<curr_level && curr_cell.y>0)
  {
    int c_ptr = curr_cell.y+tree_offset;
    uchar c_index = child_index(target_loc_code, curr_level);
    (*found_loc_code) =
      child_loc_code(c_index, curr_level-1, *found_loc_code);
    c_ptr += c_index;
    curr_cell = cells[c_ptr];
    (*global_count)++;
    found_cell_ptr = c_ptr;
    --curr_level;
  }
  return found_cell_ptr;
}

int traverse_woffset_mod(__global int4* cells, int cell_ptr, short4 cell_loc_code,
                         short4 target_loc_code, short4* found_loc_code, int * global_count,int lenbuffer, int bufferindex,int buffoffset)
{
  int found_cell_ptr = cell_ptr;
  int ret = -1;
  int level = target_loc_code.w;
  if ( level < 0)
    return ret;
  int4 curr_cell = cells[cell_ptr];
  (*global_count)++;
  int curr_level = cell_loc_code.w;
  *found_loc_code = cell_loc_code;
  while (level<curr_level && curr_cell.y>0)
  {
    int c_ptr = (curr_cell.y+buffoffset)%lenbuffer+bufferindex*lenbuffer;
    uchar c_index = child_index(target_loc_code, curr_level);
    (*found_loc_code) =
      child_loc_code(c_index, curr_level-1, *found_loc_code);
    c_ptr += c_index;
    curr_cell = cells[c_ptr];
    (*global_count)++;
    found_cell_ptr = c_ptr;
    --curr_level;
  }
  return found_cell_ptr;
}

//: traverse_woffset using int2 tree cells
int traverse_woffset_mod_opt(__global int2* cells, int cell_ptr, short4 cell_loc_code,
                             short4 target_loc_code, short4* found_loc_code, int * global_count,int lenbuffer, int bufferindex,int buffoffset)
{
  int found_cell_ptr = cell_ptr;
  int ret = -1;
  int level = target_loc_code.w;
  if ( level < 0)
    return ret;

  //curr_cell's y has child pointer and data pointer packed
  int2 curr_cell = cells[cell_ptr];

  (*global_count)++;
  int curr_level = cell_loc_code.w;
  *found_loc_code = cell_loc_code;
  while (level<curr_level && get_child_ptr(curr_cell)>0)
  {
    int c_ptr = (get_child_ptr(curr_cell) + buffoffset)%lenbuffer + bufferindex*lenbuffer;
    uchar c_index = child_index(target_loc_code, curr_level);
    (*found_loc_code) =
      child_loc_code(c_index, curr_level-1, *found_loc_code);
    c_ptr += c_index;

    //update curr cell and child_ptr
    curr_cell = cells[c_ptr];

    (*global_count)++;
    found_cell_ptr = c_ptr;
    --curr_level;
  }
  return found_cell_ptr;
}

//-----------------------------------------------------------------
// Traverse from the specified root_cell to the cell specified by loc_code.
// Return the array pointer to the resulting cell. If a leaf node is
// encoutered during the traversal down the tree before the specified
// code is reached, the leaf node index is returned.
//-----------------------------------------------------------------
int traverse_to_level(__global int4* cells, int cell_ptr,
                      short4 cell_loc_code, short4 target_loc_code,
                      short target_level,
                      short4* found_loc_code,int *global_count)
{
  int found_cell_ptr = cell_ptr;
  int ret = -1;
  int level = target_level;
  if ( level < 0)
    return ret;
  int4 curr_cell = cells[cell_ptr];
  (*global_count)++;
  int curr_level = cell_loc_code.w;
  *found_loc_code = cell_loc_code;
  while (level<curr_level && curr_cell.y>0)
  {
    int c_ptr = curr_cell.y;

    uchar c_index = child_index(target_loc_code, curr_level);
    (*found_loc_code) =
      child_loc_code(c_index, curr_level-1, *found_loc_code);
    c_ptr += c_index;
    curr_cell = cells[c_ptr];
    (*global_count)++;
    found_cell_ptr = c_ptr;
    --curr_level;
  }
  return found_cell_ptr;
}

int traverse_to_level_woffset(__global int4* cells, int cell_ptr,
                              short4 cell_loc_code, short4 target_loc_code,
                              short target_level,
                              short4* found_loc_code,int *global_count, int tree_offset)
{
  int found_cell_ptr = cell_ptr;
  int ret = -1;
  int level = target_level;
  if ( level < 0)
    return ret;
  int4 curr_cell = cells[cell_ptr];
  (*global_count)++;
  int curr_level = cell_loc_code.w;
  *found_loc_code = cell_loc_code;
  while (level<curr_level && curr_cell.y>0)
  {
    int c_ptr = curr_cell.y+tree_offset;

    uchar c_index = child_index(target_loc_code, curr_level);
    (*found_loc_code) =
      child_loc_code(c_index, curr_level-1, *found_loc_code);
    c_ptr += c_index;
    curr_cell = cells[c_ptr];
    (*global_count)++;
    found_cell_ptr = c_ptr;
    --curr_level;
  }
  return found_cell_ptr;
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


// tree_offset is the root_ptr index and all the ptrs are offset relative to the root

int traverse_force(__global int4* cells, int cell_ptr, short4 cell_loc_code,
                   short4 target_loc_code, short4* found_loc_code, int * global_count)
{
  int found_cell_ptr = cell_ptr;
  (*found_loc_code) = cell_loc_code;
  int ret = (int)-1;
  int level = target_loc_code.w;
  if ( level < 0)
    return ret;
  int curr_level = cell_loc_code.w;
  int4 curr_cell = cells[cell_ptr]; // the root of the tree to search
  (*global_count)++;
  short4 curr_code = cell_loc_code;
  curr_code.w = curr_level;
  while (level<curr_level && curr_cell.y>0)
  {
    int c_ptr = curr_cell.y;
    short4 child_bit = (short4)(1);
    child_bit = child_bit << (short4)(curr_level-1);
    short4 code_diff = target_loc_code-curr_code;
    // TODO: find a way to compute the following as a vector op
    uchar c_index = 0;

    if (code_diff.x >= child_bit.x)
      c_index += 1;
    if (code_diff.y >= child_bit.y)
      c_index += 2;
    if (code_diff.z >= child_bit.z)
      c_index += 4;
    curr_code = child_loc_code(c_index, curr_level-1, curr_code);
    c_ptr += c_index;
     curr_cell = cells[c_ptr];
    found_cell_ptr = c_ptr;
    (*found_loc_code) = curr_code;
    --curr_level;
    (*global_count)++;
  }
  return found_cell_ptr;
}

int traverse_force_woffset(__global int4* cells, int cell_ptr, short4 cell_loc_code,
                           short4 target_loc_code, short4* found_loc_code, int * global_count, int tree_offset)
{
  int found_cell_ptr = cell_ptr;
  (*found_loc_code) = cell_loc_code;
  int ret = (int)-1;
  int level = target_loc_code.w;
  if ( level < 0)
    return ret;
  int curr_level = cell_loc_code.w;
  int4 curr_cell = cells[cell_ptr]; // the root of the tree to search
  (*global_count)++;
  short4 curr_code = cell_loc_code;
  curr_code.w = curr_level;
  while (level<curr_level && curr_cell.y>0)
  {
    int c_ptr = curr_cell.y+tree_offset;
    short4 child_bit = (short4)(1);
    child_bit = child_bit << (short4)(curr_level-1);
    short4 code_diff = target_loc_code-curr_code;
    // TODO: find a way to compute the following as a vector op
    uchar c_index = 0;

    if (code_diff.x >= child_bit.x)
      c_index += 1;
    if (code_diff.y >= child_bit.y)
      c_index += 2;
    if (code_diff.z >= child_bit.z)
      c_index += 4;
    curr_code = child_loc_code(c_index, curr_level-1, curr_code);
    c_ptr += c_index;
     curr_cell = cells[c_ptr];
    found_cell_ptr = c_ptr;
    (*found_loc_code) = curr_code;
    --curr_level;
    (*global_count)++;
  }
  return found_cell_ptr;
}

int traverse_force_woffset_mod(__global int4* cells, int cell_ptr, short4 cell_loc_code,
                               short4 target_loc_code, short4* found_loc_code,
                               int * global_count,int lenbuffer, int bufferindex,int buffoffset)
{
   int found_cell_ptr = cell_ptr;
  (*found_loc_code) = cell_loc_code;
  int ret = (int)-1;
  int level = target_loc_code.w;
  if ( level < 0)
    return ret;
  int curr_level = cell_loc_code.w;
  int4 curr_cell = cells[cell_ptr]; // the root of the tree to search
  (*global_count)++;
  short4 curr_code = cell_loc_code;
  curr_code.w = curr_level;
  while (level<curr_level && curr_cell.y>0)
  {
    int c_ptr = (curr_cell.y+buffoffset)%lenbuffer+bufferindex*lenbuffer;
    short4 child_bit = (short4)(1);
    child_bit = child_bit << (short4)(curr_level-1);
    short4 code_diff = target_loc_code-curr_code;
    // TODO: find a way to compute the following as a vector op
    uchar c_index = 0;

    if (code_diff.x >= child_bit.x)
      c_index += 1;
    if (code_diff.y >= child_bit.y)
      c_index += 2;
    if (code_diff.z >= child_bit.z)
      c_index += 4;
    curr_code = child_loc_code(c_index, curr_level-1, curr_code);
    c_ptr += c_index;
    curr_cell = cells[c_ptr];
    found_cell_ptr = c_ptr;
    (*found_loc_code) = curr_code;
    --curr_level;
    (*global_count)++;
  }
  return found_cell_ptr;
}

//: traverse_force_woffset_mod_opt using int2 tree cells
int traverse_force_woffset_mod_opt(__global int2* cells, int cell_ptr, short4 cell_loc_code,
                                   short4 target_loc_code, short4* found_loc_code,
                                   int * global_count,int lenbuffer, int bufferindex,int buffoffset)
{
  int found_cell_ptr = cell_ptr;
  (*found_loc_code) = cell_loc_code;
  int ret = (int)-1;
  int level = target_loc_code.w;
  if ( level < 0)
    return ret;
  int curr_level = cell_loc_code.w;

  //curr_cell's y has child pointer and data pointer packed
  int2 curr_cell = cells[cell_ptr];

  (*global_count)++;
  short4 curr_code = cell_loc_code;
  curr_code.w = curr_level;
  while (level<curr_level && get_child_ptr(curr_cell)>0)
  {
    int c_ptr = (get_child_ptr(curr_cell) + buffoffset)%lenbuffer + bufferindex*lenbuffer;
    short4 child_bit = (short4)(1);
    child_bit = child_bit << (short4)(curr_level-1);
    short4 code_diff = target_loc_code-curr_code;
    // TODO: find a way to compute the following as a vector op
    uchar c_index = 0;

    if (code_diff.x >= child_bit.x)
      c_index += 1;
    if (code_diff.y >= child_bit.y)
      c_index += 2;
    if (code_diff.z >= child_bit.z)
      c_index += 4;
    curr_code = child_loc_code(c_index, curr_level-1, curr_code);
    c_ptr += c_index;

    //update curr cell
    curr_cell = cells[c_ptr];

    found_cell_ptr = c_ptr;
    (*found_loc_code) = curr_code;
    --curr_level;
    (*global_count)++;
  }
  return found_cell_ptr;
}

int traverse_force_local(__local int4* cells, int cell_ptr, short4 cell_loc_code,
                         short4 target_loc_code, short4* found_loc_code, int * global_count)
{
  int found_cell_ptr = cell_ptr;
  (*found_loc_code) = cell_loc_code;
  int ret = (int)-1;
  int level = target_loc_code.w;
  if ( level < 0)
    return ret;
  int curr_level = cell_loc_code.w;
  int4 curr_cell = cells[cell_ptr]; // the root of the tree to search
  (*global_count)++;
  short4 curr_code = cell_loc_code;
  curr_code.w = curr_level;
  while (level<curr_level && curr_cell.y>0)
  {
    int c_ptr = curr_cell.y;
    short4 child_bit = (short4)(1);
    child_bit = child_bit << (short4)(curr_level-1);
    short4 code_diff = target_loc_code-curr_code;
    // TODO: find a way to compute the following as a vector op
    int c_index = 0;

    if (code_diff.x >= child_bit.x)
      c_index += 1;
    if (code_diff.y >= child_bit.y)
      c_index += 2;
    if (code_diff.z >= child_bit.z)
      c_index += 4;
    curr_code = child_loc_code(c_index, curr_level-1, curr_code);
    c_ptr += c_index;
    curr_cell = cells[c_ptr];
    (*global_count)++;
    found_cell_ptr = c_ptr;
    *found_loc_code = curr_code;
    --curr_level;
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


//--------------------------------------------------------------------
// Find the common ancestor of a cell given a binary difference
//


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

#ifdef USEIMAGE
const sampler_t RowSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

int4 get_cell(__read_only image2d_t cells, uint width, int cell_ptr)
{
  int2 pos;
  pos.x=cell_ptr%width;
  pos.y=(cell_ptr/width);
  return read_imagei(cells,RowSampler,pos);
}

int traverse_force(__read_only image2d_t cells, int cell_ptr, short4 cell_loc_code,
                   short4 target_loc_code, short4* found_loc_code)
{
  uint width=get_image_width(cells);
  int found_cell_ptr = cell_ptr;
  (*found_loc_code) = cell_loc_code;
  int ret = (int)-1;
  int level = target_loc_code.w;
  if ( level < 0)
    return ret;
  int curr_level = cell_loc_code.w;
  int4 curr_cell =get_cell(cells,width, found_cell_ptr);
  short4 curr_code = cell_loc_code;
  curr_code.w = curr_level;
  while (level<curr_level && curr_cell.y>0)
  {
    int c_ptr = curr_cell.y;
    short4 child_bit = (short4)(1);
    child_bit = child_bit << (short4)(curr_level-1);
    short4 code_diff = target_loc_code-curr_code;
    // TODO: find a way to compute the following as a vector op
    uchar c_index = 0;
    if (code_diff.x >= child_bit.x)
      c_index += 1;
    if (code_diff.y >= child_bit.y)
      c_index += 2;
    if (code_diff.z >= child_bit.z)
      c_index += 4;
    curr_code = child_loc_code(c_index, curr_level-1, curr_code);
    c_ptr += c_index;
    curr_cell =get_cell(cells,width, c_ptr);
    found_cell_ptr = c_ptr;
    *found_loc_code = curr_code;
    --curr_level;
  }
  return found_cell_ptr;
}

int common_ancestor(__read_only image2d_t cells, int cell_ptr, short4 cell_loc_code,
                    short4 target_loc_code, short4* ancestor_loc_code)
{
  uint width=get_image_width(cells);
  short4 bin_diff = cell_loc_code ^ target_loc_code;
  short curr_level = (short)cell_loc_code.w;
  int curr_cell_ptr = cell_ptr;
  (*ancestor_loc_code) = cell_loc_code;
  int4 curr_cell =get_cell(cells,width, curr_cell_ptr);
  short4 mask = (short4)(1 << (curr_level));
  short4 shift_one =(short4)1;  // shift the mask by 1 as a vector
  short4 arg = bin_diff & mask; // masking the bits of the difference (xor)
  while (arg.x>0||arg.y>0||arg.z>0) // might be done as vector op
  {
    curr_cell_ptr = curr_cell.x;
    curr_cell =get_cell(cells,width, curr_cell_ptr);
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

int traverse_to_level(__read_only image2d_t cells, int cell_ptr,
                      short4 cell_loc_code, short4 target_loc_code,
                      short target_level,short4* found_loc_code)
{
  uint width=get_image_width(cells);
  int found_cell_ptr = cell_ptr;
  int ret = -1;
  int level = target_level;
  if ( level < 0)
    return ret;
  int4 curr_cell =get_cell(cells,width, found_cell_ptr);
  int curr_level = cell_loc_code.w;
  *found_loc_code = cell_loc_code;
  while (level<curr_level && curr_cell.y>0)
  {
    int c_ptr = curr_cell.y;

    uchar c_index = child_index(target_loc_code, curr_level);
    (*found_loc_code) =
      child_loc_code(c_index, curr_level-1, *found_loc_code);
    c_ptr += c_index;
    curr_cell =get_cell(cells,width, c_ptr);
    found_cell_ptr = c_ptr;
    --curr_level;
  }
  return found_cell_ptr;
}

//-----------------------------------------------------------------
// Traverse from the specified root_cell to the cell specified by loc_code.
// Return the array pointer to the resulting cell. If a leaf node is
// encoutered during the traversal down the tree before the specified
// code is reached, the leaf node index is returned.
//-----------------------------------------------------------------
int traverse(__read_only image2d_t cells, int cell_ptr, short4 cell_loc_code,
             short4 target_loc_code, short4* found_loc_code)
{
  uint width=get_image_width(cells);
  int found_cell_ptr = cell_ptr;
  int ret = -1;
  int level = target_loc_code.w;
  if ( level < 0)
    return ret;
  int4 curr_cell = get_cell(cells,width, found_cell_ptr);
  int curr_level = cell_loc_code.w;
  *found_loc_code = cell_loc_code;
  while (level<curr_level && curr_cell.y>0)
  {
    int c_ptr = curr_cell.y;
    uchar c_index = child_index(target_loc_code, curr_level);
    (*found_loc_code) =
      child_loc_code(c_index, curr_level-1, *found_loc_code);
    c_ptr += c_index;
    curr_cell = get_cell(cells,width, c_ptr);
    found_cell_ptr = c_ptr;
    --curr_level;
  }
  return found_cell_ptr;
}

//-------------------------------------------------------------------
// Given the cell loc_code and the exit face, find the neighboring cell.
//-------------------------------------------------------------------
int neighbor(__read_only image2d_t cells,int cell_ptr,  short4 cell_loc_code,
             short4 exit_face, short n_levels, short4* neighbor_code)
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
                                      &ancestor_loc_code);
  if (ancestor_ptr<0) {
    (*neighbor_code) = error;
    return neighbor_ptr;
  }
  neighbor_ptr =
    traverse_to_level(cells, ancestor_ptr, ancestor_loc_code,
                      (*neighbor_code), cell_level, neighbor_code);
  return neighbor_ptr;
}

#endif

