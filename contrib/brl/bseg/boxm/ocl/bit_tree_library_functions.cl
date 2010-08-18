//:
// \file
//  Bit Tree library functions: These methods and structure mirror the
//  boct_bit_tree class in boct.  The bit tree is an octree with max
//  height of 4, who's structure is stored as an implicit bit data
//  structure, where each bit signifies that that node has been split:
//  i.e. 1 11110000 -> 3 level tree (root,1,2) where the 4 left most
//  nodes in level 1 have all split.  Number of nodes can be calculated
//  by sum(1-bits) + 1
#define X_MIN (short4)(1,0,0,0);
#define X_MAX (short4)(1,0,0,1);
#define Y_MIN (short4)(0,1,0,0);
#define Y_MAX (short4)(0,1,0,1);
#define Z_MIN (short4)(0,0,1,0);
#define Z_MAX (short4)(0,0,1,1);
#define NUM_LEVELS 4

// Note: in the code below the term "level" always refers to the
// 'index' of the level. That is, if there are 5 levels in an octree
// then the level of the root is 4 and level of the lowest leaves is 0.
// When referring to a count of levels the notation n_levels is used to
// distinguish the term from a level index.

//-------------------------------------------------------------
// the code is three independent codes ordered as:
// Xcode = ret.x; Ycode = ret.y; Zcode = ret.z; level = ret.w
//-------------------------------------------------------------
short4 loc_code(float4 point, short root_level)
{
  float max_val = 1 << root_level; // index of root
  ushort4 maxl = (ushort4)max_val;
  ushort4 temp = convert_ushort4_sat(max_val*point);
  ushort4 ret =  min(temp, maxl);
  ret.w = 0;
  return convert_short4(ret);
}

//---------------------------------------------------------------------
// the location code for a child at a level and given the parent's code
//---------------------------------------------------------------------
short4 child_loc_code(uchar child_index, short child_level, short4 parent_code)
{
  short4 mask = (short4)(1, 2, 4, 0);
  short4 shft = (short4)(0, 1, 2, 0);
  short4 ind = (short4)(child_index);
  short4 temp = mask & ind;
  temp = temp >> shft;
  temp = temp << (short4)(child_level);
  short4 loc = temp + parent_code;
  loc.w = child_level;
  return loc;
}

//---------------------------------------------------------------------
// the bits in the child index are stored as [00...00ZYX], so the child
// index ranges from 0-7. The level index ranges from 0 to n_levels-1
//---------------------------------------------------------------------
uchar child_index(short4 code, short level)
{
  if (level == 0) return (uchar)255; // flag for error
  short child_bit = 1 << (level-1);
  short4 mask = (short4)child_bit;
  short4 index = mask & code;
  uchar ret = (uchar)0;
  if (index.x) ret += 1;
  if (index.y) ret += 2;
  if (index.z) ret += 4;
  return ret;
}

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
// returns the short offset of the data  //TEST THIS
//--------------------------------------------------------------------
ushort data_index(__local uchar* tree)
{
  //tree[10] and [11] should form the short that refers to data offset
  uchar2 chars = (uchar2) (tree[10], tree[11]);
  ushort index = as_ushort(chars);
  return index;
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
uchar tree_bit_at(__local uchar* tree, int index)
{
  //make sure it's in bounds - all higher cells are leaves and thus 0
  if (index > 72)
    return 0;

  //root is special case
  if (index == 0)
    return tree[0];

  //second generation is sort of a special case (speeds up code)
  if (index < 9)
    return (1<<(index-1) & tree[1]) ? 1 : 0;

  //third or 4th generation treated in same way,
  int i  = (int) (index-9.0)/8.0 + 2; //byte index i
  int bi = (index-9)%8;
  return (1<<bi & tree[i]) ? 1 : 0;
}

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
int traverse(__local uchar* tree, int cell_index, short4 cell_loc_code,
             short4 target_loc_code, short4* found_loc_code, int * global_count)
{
  int target_level = target_loc_code.w;
  if (target_level < 0)
    return -1;

  //initialize current cell (curr_cell = has_children?)
  uchar curr_cell    = tree_bit_at(tree, cell_index);
  int curr_level     = cell_loc_code.w;
  (*found_loc_code)  = cell_loc_code;

  while (target_level<curr_level && curr_cell == 1)
  {
    //update found loc code (loc code belonging to correct child)
    uchar c_index     = child_index(target_loc_code, curr_level);
    (*found_loc_code) = child_loc_code(c_index, curr_level-1, *found_loc_code);

    //update cell_index = first_child_index + child_offset
    cell_index = (cell_index*8+1) + (int) c_index; //8i+1 + c_index
    curr_cell  = tree_bit_at(tree, cell_index);

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
int traverse_force(__local uchar* tree, int cell_ptr, short4 cell_loc_code,
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
  uchar curr_cell  = tree_bit_at(tree, cell_ptr); // the root of the tree to search
  short4 curr_code = cell_loc_code;
  curr_code.w = curr_level;
  (*global_count)++;

  //if the curr cell has children go to the correct one
  while (level<curr_level && curr_cell == 1)
  {
    //update found loc code (loc code belonging to correct child)
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

    //update cell_index = first_child_index + child_offset
    cell_ptr = (cell_ptr*8 + 1) + (int) c_index;
    curr_cell = tree_bit_at(tree, cell_ptr);

    found_cell_ptr = cell_ptr;
    (*found_loc_code) = curr_code;
    --curr_level;
    (*global_count)++;
  }
  return found_cell_ptr;
}

#if 0 // lots of lines commented out for now...

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


//---------------------------------------------------------------------
// The vector result for the exit face as a short vector in X, Y, Z
// The element corresponding to the exit coordinate has the value 1
// The other coordinate elements are 0. The return has the 'w' element
// set to 1 if the result is a max face otherwise if a min face it is 0.
// cell_min is the minimum point in the cell and cell_max is the
// maximum point in the cell.
//---------------------------------------------------------------------
short4 cell_exit_face(float4 exit_point, float4 cell_min, float4 cell_max)
{
  float4 min_diff =   fabs(exit_point-cell_min);
  float4 max_diff =   fabs(exit_point-cell_max);

  float min=1e5f;
  short4 faceid=(short4)(-1,-1,-1,-1);
  if (min_diff.x<min)
  {
    min=min_diff.x;
    faceid=X_MIN;
  }
  if (min_diff.y<min)
  {
    min=min_diff.y;
    faceid=Y_MIN;
  }
  if (min_diff.z<min)
  {
    min=min_diff.z;
    faceid=Z_MIN;
  }
  if (max_diff.x<min)
  {
    min=max_diff.x;
    faceid=X_MAX;
  }
  if (max_diff.y<min)
  {
    min=max_diff.y;
    faceid=Y_MAX;
  }
  if (max_diff.z<min)
  {
    min=max_diff.z;
    faceid=Z_MAX;
  }

  return faceid;
}

short4 cell_exit_face_safe(float4 exit_point,float4 ray_d, float4 cell_min, float4 cell_max)
{
  float4 min_diff =   fabs(exit_point-cell_min);
  float4 max_diff =   fabs(exit_point-cell_max);

  float min=1e5f;
  short4 faceid=(short4)(-1,-1,-1,-1);
  if (min_diff.x<min && ray_d.x<0)
  {
    min=min_diff.x;
    faceid=X_MIN;
  }
  if (min_diff.y<min && ray_d.y<0)
  {
    min=min_diff.y;
    faceid=Y_MIN;
  }
  if (min_diff.z<min && ray_d.z<0)
  {
    min=min_diff.z;
    faceid=Z_MIN;
  }
  if (max_diff.x<min && ray_d.x>0)
  {
    min=max_diff.x;
    faceid=X_MAX;
  }
  if (max_diff.y<min&& ray_d.y>0)
  {
    min=max_diff.y;
    faceid=Y_MAX;
  }
  if (max_diff.z<min&& ray_d.z>0)
  {
    min=max_diff.z;
    faceid=Z_MAX;
  }

  return faceid;
}

short4 cell_exit_face_but_not_entry_face(float4 exit_point, float4 cell_min, float4 cell_max,short4 entry_face)
{
  float4 min_diff =  fabs(exit_point-cell_min);
  float4 max_diff =  fabs(exit_point-cell_max);

  short4 faceid=(short4) -1;

  float min=1.0f;

  // short4 temp; not used at the moment
  if (min_diff.x<min)
  {
    if (!(entry_face.x==1 && entry_face.w==0 ))
    {
      min=min_diff.x;
      faceid=X_MIN;
    }
  }
  if (min_diff.y<min )
  {
    if (!(entry_face.y==1 && entry_face.w==0 ))
    {
      min=min_diff.y;
      faceid=Y_MIN;
    }
  }
  if (min_diff.z<min)
  {
    if (!(entry_face.z==1 && entry_face.w==0 ))
    {
      min=min_diff.z;
      faceid=Z_MIN;
    }
  }
  if (max_diff.x<min )
  {
    if (!(entry_face.x==1 && entry_face.w==1 ))
    {
      min=max_diff.x;
      faceid=X_MAX;
    }
  }
  if (max_diff.y<min )
  {
    if (!(entry_face.y==1 && entry_face.w==1 ))
    {
      min=max_diff.y;
      faceid=Y_MAX;
    }
  }
  if (max_diff.z<min )
  {
    if (!(entry_face.z==1 && entry_face.w==1 ))
    {
      min=max_diff.z;
      faceid=Z_MAX;
    }
  }

  return faceid;
}

//-------------------------------------------------------------------
// Given the location code determine the bounding box for the
// cell in local tree coordinates, i.e. the max bounds of the
// tree are (0,0,0)->(1,1,1)
//-------------------------------------------------------------------
void cell_bounding_box(short4 loc_code, int n_levels,
                       float4* cell_min, float4* cell_max)
{
  float tree_size = (float)(1<<(n_levels-1));
  float cell_size = ((float)(1<<loc_code.w))/tree_size;
  float4 rtsize = (float4)(1.0f/tree_size);
  (*cell_min) = convert_float4(loc_code);
  (*cell_min) *= rtsize;
  float4 csize = (float4)cell_size;
  (*cell_max) = (*cell_min) + csize;
  (*cell_min).w = 0.0f;   (*cell_max).w = 0.0f;
}


//--------------------------------------------------------------------------
// Given the ray origin, ray_o and its direction, ray_d and the cell min
// and max points, find the ray parameters, tnear and tfar that correspond
// to the entry and exit points of the cell-ray intersection. If the ray
// does not intersect the cell, 0 is returned.
//-------------------------------------------------------------------------
int intersect_cell(float4 ray_o, float4 ray_d, float4 cell_min, float4 cell_max, float *tnear, float *tfar)
{
  // compute intersection of ray with all six cell planes
  float4 invR = (float4)(1.0f,1.0f,1.0f,1.0f) / ray_d;

  float4 tmin = invR * (cell_min - ray_o);
  float4 tmax = invR * (cell_max - ray_o);

  // re-order intersections to find smallest and largest on each axis
  // minimum t values for either bounding plane
  float4 tmin_s =   min(tmax, tmin);
  // maximum t values for either bounding plane
  float4 tmax_s =   max(tmax, tmin);

  if (ray_d.x ==0.0f) {
    tmin_s.x = -3.4e38f;
    tmax_s.x = 3.4e38f;
  }

  if (ray_d.y ==0.0f) {
    tmin_s.y = -3.4e38f;
    tmax_s.y = 3.4e38f;
  }

  if (ray_d.z ==0.0f) {
    tmin_s.z = -3.4e38f;
    tmax_s.z = 3.4e38f;
  }

  // find the largest tmin and the smallest tmax
  float largest_tmin =   max(  max(tmin_s.x, tmin_s.y),   max(tmin_s.x, tmin_s.z));
  float smallest_tmax =   min(  min(tmax_s.x, tmax_s.y),   min(tmax_s.x, tmax_s.z));
  *tnear = largest_tmin;
  *tfar = smallest_tmax;
  return smallest_tmax > largest_tmin;
}

//--------------------------------------------------------------------------
// Find the ray entry point to a box that encloses the entire octree
// Returns 0 if there is no intersection.
//-------------------------------------------------------------------------
int cell_entry_point(float4 ray_org, float4 ray_dir,
                     float4 cell_min, float4 cell_max, float4* entry_pt)
{
  float tnear = 0, tfar =0;
  int hit = intersect_cell(ray_org, ray_dir, cell_min, cell_max,
                           &tnear, &tfar);
  if (!hit) {
    (*entry_pt)=(float4)(-1.0f, -1.0f, -1.0f, -1.0f);
    return 0;
  }
  else {
    (*entry_pt) = ray_org + tnear*ray_dir;
    return 1;
  }
}

//--------------------------------------------------------------------------
// Find the ray exit point from a cell that is specified by its loc_code
// Returns 0 if there is no intersection with the cell
//-------------------------------------------------------------------------

int cell_exit_point(float4 ray_org, float4 ray_dir,
                    float4 cell_min, float4 cell_max, float4* exit_pt)
{
  float tnear = 0, tfar =0;
  int hit = intersect_cell(ray_org, ray_dir, cell_min, cell_max,
                           &tnear, &tfar);
  if (!hit) {
    (*exit_pt)=(float4)(tnear, tfar, -1.0f, -1.0f);
    return 0;
  }
  else {
    (*exit_pt) = ray_org + tfar*ray_dir;
    return 1;
  }
}

int cell_contains_exit_pt(int n_levels, short4 loc_code, float4 exit_pt)
{
  exit_pt.w = 0.0f; ///should be no side effects since arg is copied
  float4 cell_min, cell_max;
  cell_bounding_box(loc_code, n_levels, &cell_min, &cell_max);
  int4 test =isless(exit_pt , cell_min);
  if (any(test)) return 0;
  test =isgreater(exit_pt , cell_max);
  if (any(test)) return 0;
  return 1;
}
#endif // 0


// end of library kernels
